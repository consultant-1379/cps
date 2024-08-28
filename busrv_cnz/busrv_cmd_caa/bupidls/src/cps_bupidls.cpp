//******************************************************************************
//
// NAME
//      %File_name:CPS_BUPIDLS.cpp%
//
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 1997.
//  All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson Utvecklings AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.

// .DESCRIPTION 
//      

// DOCUMENT NO
//	190 89-CAA 109 0082

// AUTHOR 
// 	2003-02-11 EAB/UKY/SF Lillemor Petterssons

// SEE ALSO 
// 	<Related classes, (abstract) baseclasses, friend classes, ...>
//
// Revision history
// ----------------
// 2003-02-11 Create  by Lillemor Pettersson
// 2005-05-19 uablan  Added more MiddleWare info: UPBBloadModules & PCIHloadModules
//                    to be printed.
// 2005-08-23 uablan  Make separate printouts for apz21250 and apz21240.
// 2007-02-02 uablan  make separate printout for APZ21255 and forward. dynamic FW.
// 2008-12-17 xtbaklu HJ93917 - bupidls returns Error when executing (general fault) 
//                    Default cpId=0  in printSBCMiddlewareXXX() functions caused 
//                    Config::getPath() to fail on One CP Systems.
//                    Changed some int data types to please the compiler.
//                    Also added some more traces.
// 2010-05-23 xdtthng XDT/DEK XDTHNG modified for SSI
// 2011-02-23 xtuangu XDT/DEK XTUANGU modified for APG43L
//******************************************************************************

#include "CPS_BUSRV_Trace.h"
#include "cps_bupidls.h"
#include "Config.h"
#include "SBCId.h"
#include "SBCList.h"
#include "FileFunx.h"
#include "APBackupInfo.h"
#include "CodeException.h"
#include "CPS_Utils.h"
#include "InvalidValueError.h"
#include "BUPMsg.h"
#include "BUPFunx.h"
#include "APZHwVariant.h"
#include "FreeArgv.h"
#include "acs_prc_api.h"

#ifndef LOCAL_BUILD					
#include "EnvFMS.h"
#include "fms_cpf_file.h"
#endif

#include <algorithm>
#include <getopt.h>

#define UNDEF_CPID 0xFFFF

void executeCommandLine(int argc, char **argv);
void executeCommand();
void printSBCMiddleware      (SBCId sbcId, CPID cpId=UNDEF_CPID);
void printSBCMiddleware      (SBCId sbcId, const Config::CLUSTER&);
void printSBCMiddleware_21240(SBCId sbcId, CPID cpId=UNDEF_CPID);
void printSBCMiddleware_21250(SBCId sbcId, CPID cpId=UNDEF_CPID);
void printSBCMiddleware_21255(SBCId sbcId, CPID cpId=UNDEF_CPID);
void printSBCMiddleware_21255(SBCId sbcId, const Config::CLUSTER&);
void printSBCMiddleware_common(const APBackupInfo& buinfo);

//const char* toCStr(ostringstream& osStr, const u_int8* pStr, int n);

//
// global var
//===========================================================================
int g_sbc;
string g_cpname = "";
eSystemType sys;

const string CLUSTER_STR("CLUSTER");
const Config::CLUSTER g_cluster;
bool cluster_opt_specified = false;
bool extra_opt_specified = false;
bool admarker_opt_sepcified = false;

struct option longopts[] = {
   { "cp", required_argument, 0, 'c' },
   { "admarker", no_argument, 0, 'a' },
   { 0, 0, 0, 0 }
};

int main(int argc, char* argv[])
{
	initTRACE();
	newTRACE((LOG_LEVEL_INFO, "bupidls main()", 0));

	//**************************************************************************
	// Set the priority of the process and report the process to the
	// CleanupAndCrashRoutine.
	//**************************************************************************
	//EnvInit::init("BUPIDLS", EnvInit::ap_command);


	try
	{
		//Check if APZ type is classic or not
		if(Config::instance().isClassicCP())
		{
			printf("Illegal command in this system configuration.\n");
			return eIllegalCommand;
		}

        ACS_PRC_API prcApi;
        // return code from ACS_PRC_API::askForNodeState()
        // -1 Error detected
        // 1  Active
        // 2  Passive
        if (prcApi.askForNodeState() != 1) {
            printf("Unable to connect to BUSRV\n");
            return eServerUnreachable;
        }
        
		// check if system is single or multiple
		sys = eSystemType(Config::instance().isMultipleCP());

		g_sbc = -1; // no sbc
		char **argvLong = 0;
		cps_utils::argvCpy(argvLong, argv, argc);		
		boost::shared_ptr<char*> pArgv(argvLong, FreeArgv(argc));
		executeCommandLine(argc, pArgv.get());
		executeCommand();
	}
	catch(eError EE)
	{ // lan
		switch(EE)
		{
		case eNoError:
			break;

		case eSyntaxError:
		case eIllegalOption:
		case eMandatoryOption:
			if (sys == eSingle)
			{
				printf("Usage: bupidls [-n fnr] [-i]\n");
			}
			else
			{
				printf("Usage: bupidls -cp cpName [-n fnr] [-i]\n");
			}
			break;

		case eConfigServiceError:
			printf("CP name is not defined\n");
			break;
						
		case eInvalidValue:
			// invalid value already reported
			break;
			
		case eSbcNotFound:
			// SBC missing already reported
			break;
			
		case eBadBackupInfo:
			// Bad BUINFO or BUINFO missing
		    printf("BUINFO incomplete or non-existing\n");
			break;
			
		case eBuinfoAccessError:
			// syntax error already reported
			printf("Can not access BUINFO\n");
			break;

		case eExecError: // fall thru
		default:
			printf("Error when executing (general fault)\n");
		}
		return EE;
	}
	catch(CodeException& x)
	{
		switch(x.errcode())
		{
		case CodeException::CP_APG43_CONFIG_ERROR:
			printf("CP name is not defined\n");
			return eConfigServiceError;

        default:
            cout << "Internal exception" << endl;
            return eUnknown;

		}
	}
	catch(...)
	{
		printf("Internal exception\n");
		return eUnknown;
	}

	return 0;
}
//******************************************************************************
//	executeCommandLine()
//******************************************************************************
void executeCommandLine(int argc, char **argv)
{
	newTRACE((LOG_LEVEL_INFO, "executeCommandLine()", 0));

	int opt = 0;
 	char *nStr, *cStr;
	nStr = cStr = 0;
	opterr = 0;

	try
	{
		string tStr;
		while((opt = getopt_long(argc, argv, "an:i", longopts, NULL)) != -1)
		{
			switch (opt)
			{
			case 'c':
				if (cStr || nStr || extra_opt_specified)
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
				break;
				
			case 'i':
				if (extra_opt_specified)
             		throw eSyntaxError;
				extra_opt_specified = true;

				break;

			case 'a':
				admarker_opt_sepcified = true;

				break;

			case '?':
			default: 
				throw eSyntaxError;
            }
		}

		// Check if having non-option
		if (optind < argc) {
			throw eSyntaxError;
		}

		// if multiple system -> option "cp" is mandatory
		if ((sys == eMultiple) && (!cStr)) {
			throw eMandatoryOption;
		}

		if (nStr) {
			char *endptr;
			long int optval = strtol(nStr, &endptr, 10);
			if((endptr[0]) || (optval < 0) || (optval > 127))
				throw InvalidValueError(nStr, 'n');
		
			g_sbc = optval;
		}

	}
	catch(eError EE)
	{
		switch(EE)
		{
		case eSyntaxError:
			printf("Incorrect usage.\n");
			throw;
			
		case eMandatoryOption:
			 printf("Option -cp is mandatory in this system configuration\n");
			 throw;
		case eIllegalOption:
			 printf("Illegal option in this system configuration\n");
			 throw;

		default:
			 throw;
		}
	}
    catch (InvalidValueError& e) {
    	cout << "Invalid value <" << e.getValue() << "> for option <" << e.getOpt() << ">" << endl;
    	throw eInvalidValue;
    }
	catch(...) {
		throw eExecError;
	}
}
//******************************************************************************
//	executeCommand()
//******************************************************************************
void executeCommand()
{
	newTRACE((LOG_LEVEL_INFO, "executeCommand()", 0));

	try
	{
		if (g_sbc == -1)
		{
			if (sys == eSingle)
			{
				// print header
				printf("PRODUCT IDENTITIES\n\n");
				
				//get a list of all existing SBCs
				SBCList sbcList(SBCId::COMPLETE, false);

				for (u_int32 i=0; i < sbcList.count(); ++i)
				{
					printSBCMiddleware(sbcList.sbcId(i));
				}
			}
			else if (cluster_opt_specified)
			{
				// print header
				printf("PRODUCT IDENTITIES\n\n");
				
				SBCList sbcList(SBCId::COMPLETE, false, g_cluster);
				for (u_int32 i=0; i < sbcList.count(); i++) {
					printSBCMiddleware(sbcList.sbcId(i), g_cluster);
				}
			} 
			else {
				CPID cpId;

				int res = BUPFunx::convertCPNametoCPId(g_cpname.c_str(), cpId);
				if (res != ACS_CS_API_NS::Result_Success) {
					throw eConfigServiceError;
				}

				// print header
				printf("PRODUCT IDENTITIES\n\n");
				
				//get a list of all existing SBCs for this CP
				SBCList sbcList(SBCId::COMPLETE, false, cpId);

				for (u_int32 i=0; i < sbcList.count(); ++i)
				{
					printSBCMiddleware(sbcList.sbcId(i), cpId);
				}
			}

		}
		else
		{
			// only print data for a specific SBC
			SBCId sbcId;
			sbcId.id(g_sbc);

			if (sys == eSingle)
			{
				// make sure the sbc exists
#ifdef LOCAL_BUILD					
				if(!FileFunx::dirExists(Config::instance().sbcDir(sbcId).c_str()))
#else
				if (!EnvFMS::dirExists(sbcId)) 
#endif			
					throw BUPMsg::SBC_NOT_FOUND;

				// print header and info for a specific sbc
				printf("PRODUCT IDENTITIES\n\n");
				printSBCMiddleware(sbcId);
			}
			else if (cluster_opt_specified)
			{
				
				if (!EnvFMS::dirExists(sbcId, g_cluster))
				//if(!FileFunx::dirExists(Config::instance().sbcDir(sbcId, g_cluster).c_str()))
					throw BUPMsg::SBC_NOT_FOUND;

				// print header and info for a specific sbc
				printf("PRODUCT IDENTITIES\n\n");
				printSBCMiddleware(sbcId, g_cluster);

			} 
			else
			{			
				// multiple CP system
				CPID cpId;

				int res = BUPFunx::convertCPNametoCPId(g_cpname.c_str(), cpId);
				if (res != ACS_CS_API_NS::Result_Success)
					throw eConfigServiceError;

				// make sure the sbc exists
#ifdef LOCAL_BUILD					
				if(!FileFunx::dirExists(Config::instance().sbcDir(sbcId,cpId).c_str()))
#else
				if (!EnvFMS::dirExists(sbcId, cpId)) 
#endif			
					throw BUPMsg::SBC_NOT_FOUND;

				// print header and info for a specific sbc
				printf("PRODUCT IDENTITIES\n\n");
				printSBCMiddleware(sbcId,cpId);
			}
		}
	}
	//
	// catch all error & set exitcode
	//

	catch(BUPMsg::ERROR_CODE exitcode)
	{
		switch (exitcode)
		{
			case BUPMsg::SBC_NOT_FOUND:
				if (g_sbc != -1)
				{
				    // sbc is specified -> print error msg
					printf("RELFSW%d not found\n", g_sbc);
				}
				throw eSbcNotFound;
			case BUPMsg::BAD_BACKUPINFO:
				throw eBadBackupInfo;
			default:
				throw eExecError;
		}
	}

	catch(eError x)
	{
		throw;
	}

	catch(CodeException& x)
	{
		switch(x.errcode())
		{
		case CodeException::CP_APG43_CONFIG_ERROR:
			throw eConfigServiceError;
			break;
		}
	}
	catch(...)
	{
		throw eExecError;
	}
}


void printSBCMiddleware(SBCId sbcId, const Config::CLUSTER& t) 
{

	newTRACE((LOG_LEVEL_INFO, "printSBCMiddleware(sbcId=%d, CLUSTER)", 0));
	int error = eNoError;

	// Access BACKUPINFO for a sbc
	try
	{
		APBackupInfo buinfo(Config::instance().sbcDir(sbcId, t).c_str());
		BackupInfo_Base::BI_ERROR_CODE ec = buinfo.valid();

		if(ec != BackupInfo_Base::BEC_NO_ERROR)
			error = eBadBackupInfo;
	}
	catch(const CodeException& x)
	{
		switch(x.errcode())
		{
			case CodeException::BAD_BACKUPINFO: // fall thru
			case CodeException::BUINFO_MISSING:
				error = eBadBackupInfo;
				break;
			case CodeException::BUINFO_ACCESS_DENIED:
				error = eBuinfoAccessError;
				break;
			default:
				throw eExecError;
		}		
	}
	catch(...)
	{
		throw eExecError;
	}

	// Print middleware info
	printf("FILE RELFSW%d\n\n", sbcId.id());

	switch (error)
	{
		case eNoError:
			{
				//APBackupInfo buinfo(Config::instance().sbcDir(sbcId, t).c_str());
				APBackupInfo buinfo(Config::instance().sbcDir(sbcId, t).c_str());
				if (buinfo.info()->apzVersion.version() == 40 )
				{
					//Need to provide ??
					//APZ 212 40 is not used as blade
					//printSBCMiddleware_21240(sbcId, cpId);
				}
				else if (buinfo.info()->apzVersion.version()== 50)
				{
					//Need to provide ??
					//APZ 212 50 is not used as blade
					//printSBCMiddleware_21250(sbcId, cpId);
				}
				else
				{
					printSBCMiddleware_21255(sbcId, t);
				}
				break;
			}
			
		case eBadBackupInfo:
			printf("BUINFO incomplete or non-existing\n\n");
			break;

		case eBuinfoAccessError:
			printf("BUINFO not accessable\n\n");
			break;

		default:
			break;
	}	

}

void printSBCMiddleware(SBCId sbcId, CPID cpId)
{
	newTRACE((LOG_LEVEL_INFO, "printSBCMiddleware(sbcId=%d, cpId=%d )", 0, sbcId.id(), cpId));

	int error = eNoError;

	// Access BACKUPINFO for a sbc
	try
	{
		APBackupInfo buinfo(Config::instance().sbcDir(sbcId, cpId).c_str());

		BackupInfo_Base::BI_ERROR_CODE ec = buinfo.valid();
		if(ec != BackupInfo_Base::BEC_NO_ERROR)
			error = eBadBackupInfo;
	}
	catch(const CodeException& x)
	{
		switch(x.errcode())
		{
			case CodeException::BAD_BACKUPINFO: // fall thru
			case CodeException::BUINFO_MISSING:
				error = eBadBackupInfo;
				break;
			case CodeException::BUINFO_ACCESS_DENIED:
				error = eBuinfoAccessError;
				break;
			default:
				throw eExecError;
		}		
	}
	catch(...)
	{
		throw eExecError;
	}

	// Print middleware info
	printf("FILE RELFSW%d\n\n", sbcId.id());

	switch (error)
	{
		case eNoError:
			{
				//APBackupInfo buinfo(Config::instance().sbcDir(sbcId, cpId).c_str());
				APBackupInfo buinfo(Config::instance().sbcDir(sbcId, cpId).c_str());
				if (buinfo.info()->apzVersion.version() == 40 )
					printSBCMiddleware_21240(sbcId, cpId);
				else if (buinfo.info()->apzVersion.version()== 50)
					printSBCMiddleware_21250(sbcId, cpId);
				else
					printSBCMiddleware_21255(sbcId, cpId);
				break;
			}
		case eBadBackupInfo:
		    if (g_sbc != -1)
		    {
		        // option -n is specified -> throw error in order to return exit code.
		        throw eBadBackupInfo;
		    }
		    else
		    {
		        printf("BUINFO incomplete or non-existing\n\n");
		    }
			break;

		case eBuinfoAccessError:
			printf("BUINFO not accessable\n\n");
			break;

		default:
			break;
	}	
}


void printSBCMiddleware_21240(SBCId sbcId, CPID cpId)
{
	newTRACE((LOG_LEVEL_INFO, "printSBCMiddleware_21240(sbcId=%d, cpId=%d )", 0, sbcId.id(), cpId));

	//APBackupInfo buinfo(Config::instance().sbcDir(sbcId, cpId).c_str());
	APBackupInfo buinfo(Config::instance().sbcDir(sbcId, cpId).c_str());
	const BackupInfo_Base::Info* pv3 = buinfo.info();
	ostringstream osStr;
	int n;

	printf("    SYSTEM\n");
	printf("    APZ %d %02d/%d R%dA\n\n", buinfo.info()->apzVersion.type(),
		pv3->apzVersion.version(), 
		pv3->apzVersion.prodNumInfo(),
		pv3->apzVersion.revision());

	printf("    RPHMI MICRO PROGRAM IN PROM\n");
	n = pv3->mw2.nofCharacters();
	if (n == 0)
		printf("    -\n\n");
	else
	{
		printf("    %s\n\n", pv3->mw2.toStr().c_str());
		//printf("    %s\n\n", toCStr(osStr, pv3->mw2.character(), n));
		//printf("    %s\n\n", buinfo.info()->mw2.character());
	}

	printf("    RPHMI FLASH LOAD MODULES\n");
	n = pv3->mw3.nofCharacters();
	if (n == 0)
		printf("    -\n\n");
	else
	{
		printf("    %s\n\n", pv3->mw3.toStr().c_str());
		//printf("    %s\n\n", toCStr(osStr, pv3->mw3.character(), n));
		//printf("    %s\n\n", buinfo.info()->mw3.character());
	}

	printf("    SYSTEM BOOT IMAGE\n");
	n = pv3->mw4.nofCharacters();
	if (n == 0)
		printf("    -\n\n");
	else
	{
		printf("    %s\n\n", pv3->mw4.toStr().c_str());
		//printf("    %s\n\n", toCStr(osStr, pv3->mw4.character(), n));
		//printf("    %s\n\n", buinfo.info()->mw4.character());
	}

	printf("    PLEX ENGINE DUMP\n");
	n = pv3->mw5.nofCharacters();
	if (n == 0)
		printf("    -\n\n");
	else
	{
		printf("    %s\n\n", pv3->mw5.toStr().c_str());
		//printf("    %s\n\n", toCStr(osStr, pv3->mw5.character(), n));
		//printf("    %s\n\n", buinfo.info()->mw5.character());
	}

	// Print Exchanged Id
	if (extra_opt_specified)
	{
		printf("    EXCHANGE ID\n");
		n = pv3->exchangeId.nofCharacters();
		if (n == 0)
			printf("    -\n\n");
		else
		{
			printf("    %s\n\n", pv3->exchangeId.toStr().c_str());
			//printf("    %s\n\n", toCStr(osStr, pv3->exchangeId.character(), n));
			//printf("    %s\n\n", pv3->exchangeId.character());
		}
	}
}

void printSBCMiddleware_21250(SBCId sbcId, CPID cpId)
{
	newTRACE((LOG_LEVEL_INFO, "printSBCMiddleware_21250(sbcId=%d, cpId=%d )", 0, sbcId.id(), cpId));

	//APBackupInfo buinfo(Config::instance().sbcDir(sbcId, cpId).c_str());
	APBackupInfo buinfo(Config::instance().sbcDir(sbcId, cpId).c_str());
	const BackupInfo_Base::Info* pv3 = buinfo.info();
	ostringstream osStr;
	int n;

	printf("    SYSTEM\n");
	printf("    APZ %d %02d/%d R%dA\n\n", buinfo.info()->apzVersion.type(),
		pv3->apzVersion.version(), 
		pv3->apzVersion.prodNumInfo(),
		pv3->apzVersion.revision());

	printf("    CPHW DUMP\n");
	n = pv3->mw1.nofCharacters();
	if (n == 0)
		printf("    -\n\n");
	else
	{
		printf("    %s\n\n", pv3->mw1.toStr().c_str());
		//printf("    %s\n\n", toCStr(osStr, pv3->mw1.character(), n));
		//printf("    %s\n\n", buinfo.info()->mw1.character());
	}

	printf("    RPHMI FLASH LOAD MODULES\n");
	n = pv3->mw2.nofCharacters();
	if (n == 0)
		printf("    -\n\n");
	else
	{
		printf("    %s\n\n", pv3->mw2.toStr().c_str());
		//printf("    %s\n\n", toCStr(osStr, pv3->mw2.character(), n));
		//printf("    %s\n\n", buinfo.info()->mw2.character());
	}

	printf("    UPPB FLASH LOAD MODULES\n");
	n = pv3->mw3.nofCharacters();
	if (n == 0)
		printf("    -\n\n");
	else
	{
		printf("    %s\n\n", pv3->mw3.toStr().c_str());
		//printf("    %s\n\n", toCStr(osStr, pv3->mw3.character(), n));
		//printf("    %s\n\n", buinfo.info()->mw3.character());
	}

	printf("    CPSB FLASH LOAD MODULES\n");
	n = pv3->mw4.nofCharacters();
	if (n == 0)
		printf("    -\n\n");
	else
	{
		printf("    %s\n\n", pv3->mw4.toStr().c_str());
		//printf("    %s\n\n", toCStr(osStr, pv3->mw4.character(), n));
		//printf("    %s\n\n", buinfo.info()->mw4.character());
	}

	printf("    PCIH FLASH LOAD MODULES\n");
	n = pv3->mw5.nofCharacters();
	if (n == 0)
		printf("    -\n\n");
	else
	{
		printf("    %s\n\n", pv3->mw5.toStr().c_str());
		//printf("    %s\n\n", toCStr(osStr, pv3->mw5.character(), n));
		//printf("    %s\n\n", buinfo.info()->mw5.character());
	}

	printf("    PLEX ENGINE DUMP\n");
	n = pv3->mw6.nofCharacters();
	if (n == 0)
		printf("    -\n\n");
	else
	{
		printf("    %s\n\n", pv3->mw6.toStr().c_str());
		//printf("    %s\n\n", toCStr(osStr, pv3->mw6.character(), n));
		//printf("    %s\n\n", buinfo.info()->mw6.character());
	}

	printf("    CONFIGURATION FILE\n");
	n = pv3->mw7.nofCharacters();
	if (n == 0)
		printf("    -\n\n");
	else
	{
		printf("    %s\n\n", pv3->mw3.toStr().c_str());
		//printf("    %s\n\n", toCStr(osStr, pv3->mw7.character(), n));
		//printf("    %s\n\n", buinfo.info()->mw7.character());
	}

	// Print Exchanged Id
	if (extra_opt_specified)
	{
		printf("    EXCHANGE ID\n");
		n = pv3->exchangeId.nofCharacters();
		if (n == 0)
			printf("    -\n\n");
		else
		{
			printf("    %s\n\n", pv3->exchangeId.toStr().c_str());
			//printf("    %s\n\n", toCStr(osStr, pv3->exchangeId.character(), n));
			//printf("    %s\n\n", pv3->exchangeId.character());
		}
	}
}

void printSBCMiddleware_21255(SBCId sbcId, const Config::CLUSTER& t)
{
	newTRACE((LOG_LEVEL_INFO, "printSBCMiddleware_21255(sbcId=%d, ClUSTER )", 0, sbcId.id()));

	APBackupInfo buinfo(Config::instance().sbcDir(sbcId, t).c_str());
	printSBCMiddleware_common(buinfo);

}

void printSBCMiddleware_21255(SBCId sbcId, CPID cpId)
{
	newTRACE((LOG_LEVEL_INFO, "printSBCMiddleware_21255(sbcId=%d, cpId=%d )", 0, sbcId.id(), cpId));

	APBackupInfo buinfo(Config::instance().sbcDir(sbcId, cpId).c_str());
	printSBCMiddleware_common(buinfo);

}

void printSBCMiddleware_common(const APBackupInfo& buinfo)
{

	newTRACE((LOG_LEVEL_INFO, "printSBCMiddleware_common()", 0));

	// Version 3 of BUINFO
	const BackupInfo_Base::Info* pv3 = buinfo.info();
	ostringstream osStr;
	int n;

    printf("    SYSTEM\n");
	printf("    APZ %d %02d%s/%d R%dA\n\n", buinfo.info()->apzVersion.type(),
		pv3->apzVersion.version(), 
		pv3->apzHwVariant.toStr().c_str(),
		pv3->apzVersion.prodNumInfo(),
		pv3->apzVersion.revision());

	printf("    CPHW DUMP\n");
	n = pv3->mw1.nofCharacters();
	if (n == 0)
		printf("    -\n\n");
	else
	{
		printf("    %s\n\n", pv3->mw1.toStr().c_str());
		//printf("    %s\n\n", toCStr(osStr, pv3->mw1.character(), n));
		//printf("    %s\n\n", pv3->mw1.character());
	}

	printf("    PLEX ENGINE DUMP\n");
	n = pv3->mw2.nofCharacters();
	if (n == 0)
		printf("    -\n\n");
	else
	{
		printf("    %s\n\n", pv3->mw2.toStr().c_str());
		//printf("    %s\n\n", toCStr(osStr, pv3->mw2.character(), n));
		//printf("    %s\n\n", pv3->mw2.character());
	}

	printf("    CONFIGURATION FILE\n");
	n = pv3->mw3.nofCharacters();
	if (n == 0)
		printf("    -\n\n");
	else
	{
		printf("    %s\n\n", pv3->mw3.toStr().c_str());
		//printf("    %s\n\n", toCStr(osStr, pv3->mw3.character(), n));
		//printf("    %s\n\n", pv3->mw3.character());
	}

	// Version 4 of BUINFO
	if (pv3->version.major() > BackupInfo_Base::VERSION_MAJOR )
	{
		const BackupInfo_Base::Info_V4* p = buinfo.info_V4();
		//printf("    LDD1 Overall O&M Profile\n");
		//printf("    %d\n\n", p->ldd1OMProfile);
		printf("    LDD1 APZ Profile\n");
		printf("    %d\n\n", p->ldd1APZProfile);
		printf("    LDD1 APT Profile\n");
		printf("    %d\n\n", p->ldd1APTProfile);
		printf("    LDD1 CP ID\n");
		printf("    %d\n\n", p->ldd1CpId);

		//printf("    LDD2 Overall O&M Profile\n");
		//printf("    %d\n\n", p->ldd2OMProfile);
		printf("    LDD2 APZ Profile\n");
		printf("    %d\n\n", p->ldd2APZProfile);
		printf("    LDD2 APT Profile\n");
		printf("    %d\n\n", p->ldd2APTProfile);
		printf("    LDD2 CP ID\n");
		printf("    %d\n\n", p->ldd2CpId);
	}

	// Print Exchanged Id
	if (extra_opt_specified)
	{
		printf("    EXCHANGE ID\n");
		n = pv3->exchangeId.nofCharacters();
		if (n == 0)
			printf("    -\n\n");
		else
		{
			printf("    %s\n\n", pv3->exchangeId.toStr().c_str());
			//printf("    %s\n\n", pv3->exchangeId.character());
		}
	}

	// Print Automatic Dump Adjustment Marker
	if (admarker_opt_sepcified)
	{
		printf("    Automatic Dump Adjustment Marker\n");
			printf("    %d\n\n", buinfo.getAdMarker());
	}
}

/*
const char* toCStr(ostringstream& osStr, const u_int8* pStr, int n)
{
	if (!osStr.str().empty())
		osStr.seekp(0);
	copy(&pStr[0], &pStr[n], ostream_iterator<u_int8>(osStr));
	osStr << ends;
	return osStr.str().c_str();
}
*/

