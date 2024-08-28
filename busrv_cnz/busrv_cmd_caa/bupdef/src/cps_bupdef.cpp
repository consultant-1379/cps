/*
NAME
File_name:cps_bupdef.cpp

  COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.
  
  The Copyright to the computer program(s) herein is the property of Ericsson
  Utvecklings AB, Sweden.
  The program(s) may be used and/or copied only with the written permission from
  Ericsson Utvecklings AB or in accordance with the terms and conditions
  stipulated in the agreement/contract under which the program(s) have been
  supplied.
	 
  DESCRIPTION
    main() and code for the cmdline program bupdef
		
  DOCUMENT NO
    190 89-CAA 109 0387
		  
  AUTHOR
    2003-01-20 by EAB/UKY/SF Lillemor Pettersson
			 
  SEE ALSO
    -
				
  Revision history
  ----------------
  2003-01-20 uablan Created
  2006-03-01 uablan Remove the check of loadInProgress flag (HG62821).
  2006-12-14 uablan Added check if on active or passive node.
  2007-01-12 uablan Add option -cp for multiple system
  2010-05-23 xdtthng modified for SSI
  2011-03-01 xquydao Updated for APG43L project
  2011-03-31 xdtthng introduce long option; restructure long switch statement
*/

#if 0
#include "ACS_PRC_Process.H"
#include <ACS_ExceptionHandler.h>


#include "EnvInit.h"
#include "FallbackMgr.h"



#include "acs_prc_api.h"

#include "commondll.h"
#endif

#include "cps_bupdef.h"
#include "Config.h"
#include "CPS_Utils.h"
#include "CPS_BUSRV_Trace.h"
#include "SBCId.h"
#include "SBCList.h"
#include "CodeException.h"
#include "CPS_Parameters.h"
#include "DataFile.h"
#include "DateTime.h"
#include "CPS_Utils.h"
#include "InvalidValueError.h"
#include "FreeArgv.h"
#include "BUPFunx.h"
#include "BUParamsOM.h"
#include <unistd.h>
#include <iostream>
#include <getopt.h>
#include <boost/regex.hpp>
#include <vector>
#include <algorithm>
#include <utility>

using namespace std;

void executeCommandLine(int argc, char **argv);
void writeToFile();
void validateFFR(const char*);
void validateSFR(const char*);
void validateSupTime(const char*);

//void validateCP(const char*);
//bool isDigits(const char*);

void writeToFile(const Config::CLUSTER&);

int g_ffr = 0xFF;
int g_sfr = 0xFF;
int g_sup = 0xFf;
string g_cpname = "";

struct option longopts[] = {
   { "cp", required_argument, 0, 'c' },
   { 0, 0, 0, 0 }
};

const string CLUSTER_STR("CLUSTER");
const Config::CLUSTER g_cluster;

bool cluster_opt_specified = false;
bool g_underground = false;

eSystemType sys;
u_int16 APZ_Profile;

// Extern variable for getopt function
extern char *optarg;
extern int optind, optopt, opterr;


int main(int argc, char* argv[])
{
    // No TRACE shall be invoked before this point
    initTRACE();
    newTRACE((LOG_LEVEL_INFO, "bupdef main() starts", 0));

    int exitCode = eNoError;

    try
    {
        // Check if system is classic or not
        #if 0
        if (Config::instance().isClassicCP())
        {
            // bupdef command not run on classic system
            throw eIllegalCommand;
        }
        #endif

        // Check if system is single or multiple
		sys = eSystemType(Config::instance().isMultipleCP());
        TRACE((LOG_LEVEL_INFO, "Type of system: %s", 0, (sys == eSingle) ? "single" : "multiple"));

		char **argvLong = 0;
		cps_utils::argvCpy(argvLong, argv, argc);		
		boost::shared_ptr<char*> pArgv(argvLong, FreeArgv(argc));
		executeCommandLine(argc, pArgv.get());

        if (cluster_opt_specified) {
			writeToFile(g_cluster);
			// This should not be happened because it's already handled
			// this case in executeCommandLine();
			TRACE((LOG_LEVEL_WARN, "Not support writeToFile(g_cluster)", 0));
		}
		else {
			writeToFile();
		}

    }
    catch (eError x)
    {
        switch (x)
        {
        case eSyntaxError:
        case eIllegalOption:
        case eMandatoryOption:
            if (sys == eSingle)
            {
                cout << "Usage: bupdef [-n ffr] [-m sfr] [-s sup]" << endl;
            }
            else
            {
                cout << "Usage: bupdef -cp cpName [-n ffr] [-m sfr] [-s sup]" << endl;
            }
            break;

        case eInvalidValue:
            // invalid value already reported
            break;

        case eConfigServiceError:
            cout << "CP name or CP group is not defined" << endl;
            break;

        case eDumpLoadorSwitchInProgress:
            cout << "Ongoing load or dump or SBC switch or SBC rotation" << endl;
            break;

        case eIllegalCommand:
            cout << "Illegal command in this system configuration" << endl;
            break;            
            
        case eExecError: // fall thru
        default:
            cout << "Error when executing (general fault)" << endl;
        }

        exitCode = x;
    }
    catch (FileSequenceError& e) {
        cout << "File missing in sequence for option <" << e() << ">" << endl;
        exitCode = eMissingSbcInSequence;
    }
    catch (CodeException& x)
    {
        switch (x.errcode())
        {
        case CodeException::CP_APG43_CONFIG_ERROR:
            cout << "CP name or CP group is not defined" << endl;
            exitCode = eConfigServiceError;
            break;

        default:
            cout << "Internal exception" << endl;
            exitCode = eUnknown;
            break;

        }
    }
    catch (...)
    {
        cout << "Internal exception" << endl;
        exitCode = eUnknown;
    }

    TRACE((LOG_LEVEL_INFO, "Command exits with code: %d", 0, exitCode));
    return exitCode;
}
//
//	
//===========================================================================
void executeCommandLine(int argc, char **argv)
{
    newTRACE((LOG_LEVEL_INFO, "executeCommandLine(%d, argv)", 0, argc));

    int opt = 0;
 	char *nStr, *mStr, *sStr, *cStr;
	nStr = mStr = sStr = cStr = 0;
	string tStr;
	vector<Argument> options;
   
    opterr = 0;
    try
    {			
		while((opt = getopt_long(argc, argv, "n:m:s:", longopts, NULL)) != -1)
        {
            switch (opt)
            {
            case 'c':
				if (cStr || nStr || mStr || sStr)
             		throw eSyntaxError;				
             		
				if (sys == eSingle)
					throw eIllegalOption;
					
				tStr = g_cpname = cStr = optarg;

				//transform(tStr.begin(), tStr.end(), tStr.begin(), (int (*)(int)) ::toupper);
				//cluster_opt_specified = tStr == CLUSTER_STR;
				
				// Directly change g_cpname to upper case for IO
				transform(g_cpname.begin(), g_cpname.end(), g_cpname.begin(), (int (*)(int)) ::toupper);
				cluster_opt_specified = g_cpname == CLUSTER_STR;				
				break;
				
            case 'n':				
				if (nStr)
             		throw eSyntaxError;
            		
				nStr = optarg;
				options.push_back(Argument(validateFFR, optarg));
				break;				

            case 'm':
				if (mStr)
             		throw eSyntaxError;
             		
				mStr = optarg;
				options.push_back(Argument(validateSFR, optarg));
				break;

            case 's':
				if (sStr)
             		throw eSyntaxError;
             		
            	sStr = optarg;
				options.push_back(Argument(validateSupTime, optarg));
                break;
                
            default:
                throw eSyntaxError;
            }
        }

        // Have redundant argument
		if (optind < argc)
             throw eSyntaxError;

        // If multiple system option,"cp" is mandatory
        if ((sys == eMultiple) && g_cpname.empty())
            throw eMandatoryOption;

		// Validate FFR, SFR and Supervision time
		vector<Argument>::iterator it;
		for (it = options.begin(); it != options.end(); ++it)
			(*it)();
    }
    catch (eError x)
    {
        switch (x)
        {
        case eSyntaxError:
            cout << "Incorrect usage" << endl;
            throw;
        case eMandatoryOption:
            cout << "Option -cp is mandatory in this system configuration" << endl;
            throw;
        case eIllegalOption:
            cout << "Illegal option in this system configuration" << endl;
            throw;
        default:
            throw;
        }
    }
    catch (InvalidValueError& e) {
    	cout << "Invalid value <" << e.getValue() << "> for option <" << e.getOpt() << ">" << endl;
    	throw eInvalidValue;
    }
    catch (FileSequenceError&) {
    	throw;
    }
    catch (...)
    {
        throw eExecError;
    }
}
//
//	
//===========================================================================

void writeToFile()
{
    newTRACE((LOG_LEVEL_INFO, "writeToFile()", 0));

	try
    {
        if (sys == eSingle)
        {
            // Create directory structure
            Config::instance().createDirStructure();

			BUParamsOM om;
			BUParamsCommon::BUParam params;

			//if (om.getBUParam(params))
			//	throw eUnknown;

            // Get old (or default value) if option not set.
            // Write the new settings to busrv.ini
            // Get old (or default value) if option not set.
            if (g_ffr == 0xFF)
            {
                g_ffr = Config::instance().getSbcToReloadFromFFR();
            }

            if (g_sfr == 0xFF)
            {
                g_sfr = Config::instance().getSbcToReloadFromSFR();
            }

            if (g_sup == 0xFF)
            {
                g_sup = Config::instance().getSupervisionTime();
            }
            params.ffr = g_ffr;
            params.sfr = g_sfr;
            params.supTime = g_sup;

            if (!om.setBUParam(params, BUParamsCommon::CP_SINGLE)) {
            	TRACE((LOG_LEVEL_ERROR, "Write to file() fail at setBUParam()", 0))
            	throw eUnknown;
            }

            Config::instance().save(g_sup, g_ffr, g_sfr);

            // Reset the reload sequence
            DataFile::startReloadSeq(true);
        }

        if (sys == eMultiple)
        {
            CPID cpid;
            ACS_CS_API_IdList cpList;

            // Make a list of CPid(s)
            // try first to see if it is a CP name
            int res = BUPFunx::convertCPNametoCPId(g_cpname.c_str(), cpid);
            if (res == ACS_CS_API_NS::Result_Success)
            {
                //put cpid into cplist
                cpList.setSize(1);
                cpList.setValue(cpid, 0);
            }
            else
            {
                // check if CP Group
                res = BUPFunx::CPNameorGrptoListofCPid(g_cpname.c_str(),
                        cpList);
                if (res != ACS_CS_API_NS::Result_Success)
                throw eConfigServiceError;
            }
			BUParamsOM om;
			BUParamsCommon::BUParam params;
            for (u_int32 i = 0; i < cpList.size(); i++)
            {
                // create directory structure
                Config::instance().createDirStructure(cpList[i]);

                params.reset();
    			//result = om.getBUParam(params, cpList[i]);

    			//if (result)
    			//	throw eUnknown;

                if (g_ffr == 0xFF)
                    g_ffr = Config::instance().getSbcToReloadFromFFR(cpList[i]);
                if (g_sfr == 0xFF)
                    g_sfr = Config::instance().getSbcToReloadFromSFR(cpList[i]);
                if (g_sup == 0xFF)
                    g_sup = Config::instance().getSupervisionTime(cpList[i]);

    			// Get old (or default value) if option not set.
                params.ffr = g_ffr;
                params.sfr = g_sfr;
                params.supTime = g_sup;

                if (cpList[i] != 1001 && cpList[i] != 1002)
                	throw eUnknown;

                BUParamsCommon::IMM_CP_TYPE type = cpList[i] == 1001 ?
                		BUParamsCommon::CP_SPX1 : BUParamsCommon::CP_SPX2;

                if (!om.setBUParam(params, type))
                	throw eUnknown;

                Config::instance().save(g_sup, g_ffr, g_sfr, cpList[i]);
                
                // reset the reload sequence
                DataFile::startReloadSeq(true, cpList[i]);

            }//for
        }//eMultiple
    }//try
    catch (eError x)
    {
        throw;
    }
    catch (CodeException& x)
    {
        switch (x.errcode())
        {
        case CodeException::CP_APG43_CONFIG_ERROR:
            throw eConfigServiceError;
            break;
        }
    }
    catch (...)
    {
        throw eExecError;
    }
}

//===========================================================================
void writeToFile(const Config::CLUSTER& t) 
{
	newTRACE((LOG_LEVEL_INFO, "CPS_bupdef::writeToFile(CLUSTER)",0 ));

	try{
		Config::instance().createDirStructure(t);

		// Get old (or default value) if option not set.
		if (g_ffr == 0xFF)
			g_ffr = Config::instance().getSbcToReloadFromFFR(t);	
		//cout << "ffr is: " << g_ffr << endl;

		if (g_sfr == 0xFF)
			g_sfr = Config::instance().getSbcToReloadFromSFR(t);
		//cout << "sfr is: " << g_sfr << endl;

		if (g_sup == 0xFF)
			g_sup = Config::instance().getSupervisionTime(t);
		//cout << "sup is: " << g_sup << endl;

		Config::instance().save(g_sup, g_ffr, g_sfr, t);

		Config::instance().createDataBusrvDirStructure(t);
		DataFile::startReloadSeq(true, t);
		// Remove just for now
		DataFile::srmReloadSBC(SBCId(static_cast<u_int16>(0xFF)), t);
		DataFile::lastLoadedSBC(SBCId(static_cast<u_int16 >(0)), t);	// Reset last loaded sbc

		// reset the reload sequence 

		ACS_CS_API_IdList cpList;
		ACS_CS_API_CP * CP = ACS_CS_API::createCPInstance();

		ACS_CS_API_NS::CS_API_Result res = CP->getCPList(cpList);

		if (res != ACS_CS_API_NS::Result_Success) {
			ACS_CS_API::deleteCPInstance(CP);
			throw eExecError;
		}

		//cout << "writeToFile(CLUSTER), cps_bupdef.cpp" << endl;
		DateTime now;
		now.setNull();
		for (u_int32 i = 0; i < cpList.size(); i++) {
			CPID cpId = cpList[i];
			if (cpId > 63) continue;
			const Config::CLUSTER t(cpId, true);

			DataFile::startReloadSeq(true, t);
			DataFile::lastLoadedSBC(SBCId(static_cast<u_int16 >(0)), t);	// Reset last loaded sbc
			DataFile::lastLoadedTimeForFallback(now, t);
			DataFile::srmReloadSBC(SBCId(static_cast<u_int16 >(0xFF)), t);
		}
		ACS_CS_API::deleteCPInstance(CP);

		BUParamsOM om;
		BUParamsCommon::BUParam params;
		// Get old (or default value) if option not set.
        params.ffr = g_ffr;
        params.sfr = g_sfr;
        params.supTime = g_sup;

        if (!om.setBUParam(params, BUParamsCommon::CP_CLUSTER))
        	throw eUnknown;
	}
	catch(...) {
      throw eExecError;
	}

}
//
//	
//===========================================================================
void validateFFR(const char* nStr)
{            
	SBCList sbcList;
    char* endptr = 0;
    long int optval = strtol(nStr, &endptr, 10);

    // Check argument range for parameter -n
    if ((endptr[0]) || (optval < 0) || (optval > 99))
        throw InvalidValueError(nStr, 'n');

    if (cluster_opt_specified)
    {
        // Note: this section of code is not executed in non-classic dual
#if CLUSTER
        // FFR range for CLUSTER is 0 - 29
        if (optval > 29)
                throw InvalidValueError(nStr, 'n');

        sbcList.update(SBCId::FIRST, true, g_cluster);

        if (sbcList.count() <= optval)
            throw eMissingSbcInSequence;
#endif
    }
    else if (!g_cpname.empty())
    {
        // check cpgroup/cpid
        CPID cpid;
        ACS_CS_API_IdList cpList;

        // Make a list of CPid(s)
        // try first to see if it is a CP name
        int res = BUPFunx::convertCPNametoCPId(
                g_cpname.c_str(), cpid);
        if (res == ACS_CS_API_NS::Result_Success)
        {
            //put cpid into cplist
            cpList.setSize(1);
            cpList.setValue(cpid, 0);
        }
        else
        {
            // check if CP Group
            res = BUPFunx::CPNameorGrptoListofCPid(
                    g_cpname.c_str(), cpList);
            if (res != ACS_CS_API_NS::Result_Success)
                throw eConfigServiceError;
        }
        for (u_int32 i = 0; i < cpList.size(); ++i)
        {
            sbcList.update(SBCId::FIRST, true, cpList[i]);
            //cout << "cp is is: " << cpList[i] << " cout is: " << sbcList.count()<< endl;
            if (sbcList.count() <= optval)
                throw FileSequenceError('n');
         
            //cout << "CP " << cpList[i] << " update OK: " << sbcList.count() << endl;
        }//for
    }
    else
    {
        // Check whether the input FFR is in the FFR range defined by FMS
        sbcList.update(SBCId::FIRST, true);
        if (sbcList.count() <= optval) 
        	throw FileSequenceError('n');
    }
    g_ffr = optval;
}
//
//	
//===========================================================================
void validateSFR(const char* mStr)
{            
	SBCList sbcList;
    char* endptr = 0;
    long int optval = strtol(mStr, &endptr, 10);

    // Check argument range for parameter -m
    if ((endptr[0]) || ((optval != 0) && ((optval < 100) || (optval > 127))))
        throw InvalidValueError(mStr, 'm');

    if (optval != 0)
    {
        if (cluster_opt_specified)
        {
            // Note: this section of code is not executed in non-classic dual

            sbcList.update(SBCId::SECOND, true, g_cluster);
            if (sbcList.count() < (optval - SBCId::FFR_LAST_ID))
            {
                throw eMissingSbcInSequence;
            }
        }
        else if (!g_cpname.empty())
        {
            // check cpgroup/cpid
            CPID cpid;
            ACS_CS_API_IdList cpList;

            // Make a list of CPid(s)
            // try first to see if it is a CP name
            int res = BUPFunx::convertCPNametoCPId(
                    g_cpname.c_str(), cpid);
            if (res == ACS_CS_API_NS::Result_Success)
            {
                //put cpid into cplist
                cpList.setSize(1);
                cpList.setValue(cpid, 0);
            }
            else
            {
                // check if CP Group
                res = BUPFunx::CPNameorGrptoListofCPid(
                        g_cpname.c_str(), cpList);
                if (res != ACS_CS_API_NS::Result_Success)
                    throw eConfigServiceError;
            } 
            for (u_int32 i = 0; i < cpList.size(); i++)
            {
                sbcList.update(SBCId::SECOND, true, cpList[i]);
                if (sbcList.count() < (optval- SBCId::FFR_LAST_ID)) 
                	throw FileSequenceError('m');
               
            }//for
        }
        else
        {
            // Check whether the input SFR is in the SFR range defined by FMS
            sbcList.update(SBCId::SECOND, true);
            if (sbcList.count() < (optval - SBCId::FFR_LAST_ID))
                throw FileSequenceError('m');
        }
    }
    g_sfr = optval;
}
//
//	
//===========================================================================
void validateSupTime(const char* sup)
{            
    char* endptr = 0;
    long int optval = strtol(sup, &endptr, 10);

    if ((endptr[0]))
		throw InvalidValueError(sup, 's');
		
	try {
        Config::instance().validate(optval);
	}
	catch (...) {
		throw InvalidValueError(sup, 's');
	}
	
	g_sup = optval;
}
#if 0
//
//	
//===========================================================================
void validateCP(const char* cpOpt)
{            
	if (sys == eSingle)
		throw eIllegalOption;
		
	g_cpname = cpOpt;		// Get CP name or group

	string tStr = g_cpname;
	transform(tStr.begin(), tStr.end(), tStr.begin(), (int (*)(int)) ::toupper);
	cluster_opt_specified = tStr == CLUSTER_STR;
}
//
//	
//===========================================================================
bool isDigits(const char* str)
{
    static const boost::regex digits("\\d+");
    return boost::regex_match(str, digits);
}
#endif
           

