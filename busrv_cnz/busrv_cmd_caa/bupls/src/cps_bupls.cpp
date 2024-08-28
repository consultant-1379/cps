
//******************************************************************************
//
// NAME
//      %File_name:CPS_BUPLS.cpp%
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
// 	2003-01-21 EAB/UKY/SF Lillemor Pettersson

// SEE ALSO 
// 	<Related classes, (abstract) baseclasses, friend classes, ...>
//
// Revision history
// ----------------
// 2003-01-21 Create by Lillemor Pettersson
// 2006-12-07 Update commnad accoring to DR SPOE (Single Point Of Entr
// 2007-03-02 Update to work with Multiple CP System
// 2010-05-23 Updated by XDT/DEK XDTTHNG for SSI
// 2011-02-23 Updated by XDT/DEK XTUANGU for APG43L
//******************************************************************************

#include "CPS_BUSRV_Trace.h"
#include "CodeException.h"
#include "cps_bupls.h"
#include "Config.h"
#include "DataFile.h"
#include "BUPFunx.h"
#include "CPS_Utils.h"
#include "FreeArgv.h"
#include <string>
#include <algorithm>
#include <unistd.h>
#include <getopt.h>
#include <boost/shared_ptr.hpp>

void executeCommandLine(int argc, char** argv);
void executeCommand();

//
//============================================================
string g_cpname = "";
eSystemType sys;

struct option longopts[] = {
   { "cp", required_argument, 0, 'c' },
   { 0, 0, 0, 0 }
};

// ---------------------------------------
// 
const std::string CLUSTER_STR("CLUSTER");
const Config::CLUSTER g_cluster;
bool cluster_opt_specified = false;


int main(int argc, char* argv[])
{

	// No TRACE shall be invoked before this point
	initTRACE();
	newTRACE((LOG_LEVEL_INFO, "bupls main()", 0));

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
		// check if system is single or multiple
		sys = eSystemType(Config::instance().isMultipleCP());

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
				printf("Usage: bupls \n");
			}
			else
			{
				printf("Usage: bupls -cp cpName \n");
			}
			break;
		case eConfigServiceError:
			printf("CP name is not defined\n");
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
void executeCommandLine(int argc, char** argv)
{
	newTRACE((LOG_LEVEL_INFO, "executeCommandLine()", 0));

	int opt = 0;
	bool cpOptionUsed = false;
	opterr = 0;

	try
	{
		string tStr;
		while((opt = getopt_long(argc, argv, "", longopts, NULL)) != -1)
		{
			switch (opt)
			{
			case 'c':
				if (cpOptionUsed)
             		throw eSyntaxError;
             		
				if (sys == eSingle)
					throw eIllegalOption;
	
				cpOptionUsed = true;
				g_cpname = optarg;		// Get CP name or group

				// Do not remove this
				// ------------------
				// For the time being, IO only works with upper case
				//
				//tStr = g_cpname;
				//transform(tStr.begin(), tStr.end(), tStr.begin(), (int (*)(int)) ::toupper);
				//cluster_opt_specified = tStr == CLUSTER_STR;
				
				// Directly change g_cpname to upper case for IO
				transform(g_cpname.begin(), g_cpname.end(), g_cpname.begin(), (int (*)(int)) ::toupper);
				cluster_opt_specified = g_cpname == CLUSTER_STR;
				
				break;

			default: 
				throw eSyntaxError;
			}
		}

		if (optind < argc)
		{
			throw eSyntaxError;
		}

		// if multiple system -> option "cp" is mandatory
		if ((sys == eMultiple) && (cpOptionUsed == false))
		{
			throw eMandatoryOption;
		}

	}
	catch (eError x)
	{
		 switch(x)
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
	catch(...)
	{
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
		if (sys == eSingle) {
				
			printf("RELOAD PARAMETERS\n");
			printf("    FFR   SFR   SUP\n");
			printf("     %2d   %3d  %4d\n\n", Config::instance().getSbcToReloadFromFFR(), 
			Config::instance().getSbcToReloadFromSFR(), Config::instance().getSupervisionTime());

			// calculate century
			int century = (DataFile::lastLoadedTimeForFallback().year() > 80) ? 19 : 20;

			printf("LAST RELOADED FILE\n");
			printf("    FILE       TIME\n");
			printf("    RELFSW%-3d  %d%02d-%02d-%02d %02d:%02d\n", 
				DataFile::lastLoadedSBC().id(), century,
				DataFile::lastLoadedTimeForFallback().year(),
				DataFile::lastLoadedTimeForFallback().month(),
				DataFile::lastLoadedTimeForFallback().day(),
				DataFile::lastLoadedTimeForFallback().hour(),
				DataFile::lastLoadedTimeForFallback().minute());
		}
		else if (cluster_opt_specified)
		{
			// print info from busrv.ini. 
			printf("RELOAD PARAMETERS\n");
			printf("    FFR   SFR   SUP\n");
			printf("     %2d   %3d  %4d\n\n", Config::instance().getSbcToReloadFromFFR(g_cluster),
			Config::instance().getSbcToReloadFromSFR(g_cluster), Config::instance().getSupervisionTime(g_cluster));

			
			DateTime loadedTime = DataFile::lastLoadedTimeForFallback(g_cluster);
			int century = (loadedTime.year() > 80) ? 19 : 20;
			printf("LAST RELOADED FILE\n");
			printf("    FILE       TIME\n");
			printf("    RELFSW%-3d  %d%02d-%02d-%02d %02d:%02d\n", 
				DataFile::lastLoadedSbcInfo(g_cluster).id(), century,
				loadedTime.year(),
				loadedTime.month(),
				loadedTime.day(),
				loadedTime.hour(),
				loadedTime.minute());

		}
		else
		{
			
			CPID cpId;
			int res = BUPFunx::convertCPNametoCPId(g_cpname.c_str(), cpId);
			if (res != ACS_CS_API_NS::Result_Success) {
				throw eConfigServiceError;
			}

			// print info from busrv.ini. 
			printf("RELOAD PARAMETERS\n");
			printf("    FFR   SFR   SUP\n");
			printf("     %2d   %3d  %4d\n\n", Config::instance().getSbcToReloadFromFFR(cpId), 
			Config::instance().getSbcToReloadFromSFR(cpId), Config::instance().getSupervisionTime(cpId));

			// calculate century
			int century = (DataFile::lastLoadedTimeForFallback(cpId).year() > 80) ? 19 : 20;

			printf("LAST RELOADED FILE\n");
			printf("    FILE       TIME\n");
			printf("    RELFSW%-3d  %d%02d-%02d-%02d %02d:%02d\n", 
				DataFile::lastLoadedSBC(cpId).id(), century,
				DataFile::lastLoadedTimeForFallback(cpId).year(),
				DataFile::lastLoadedTimeForFallback(cpId).month(),
				DataFile::lastLoadedTimeForFallback(cpId).day(),
				DataFile::lastLoadedTimeForFallback(cpId).hour(),
				DataFile::lastLoadedTimeForFallback(cpId).minute());
				

		}//eMultiple
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


