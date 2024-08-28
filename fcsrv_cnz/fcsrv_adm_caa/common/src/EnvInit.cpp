/*
NAME
   File_name:EnvInit.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

	The Copyright to the computer program(s) herein is the property of Ericsson
	Utvecklings AB, Sweden.
	The program(s) may be used and/or copied only with the written permission from
	Ericsson Utvecklings AB or in accordance with the terms and conditions
	stipulated in the agreement/contract under which the program(s) have been
	supplied.
	
DESCRIPTION
   Initiation of the process depending on the environment - APG-40 or local test.

DOCUMENT NO
   190 89-CAA 109 1410

AUTHOR 
   2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on EnvInit.cpp of FCSRV in Windows

SEE ALSO 
	-

Revision history
----------------
2001-09-14 Created

*/

#include "EnvInit.h"

#include "ACS_PRC_Process.H"
#include <ACS_ExceptionHandler.h>

#ifdef _DEBUG
#pragma comment(lib, "libACS_EXH_R1A_dmdn6d.lib")
#pragma comment(lib, "libACS_PRCAPI_R1A_DMDN6d.lib")
#else
#pragma comment(lib, "libACS_EXH_R1A_dmdn6.lib")
#pragma comment(lib, "libACS_PRCAPI_R1A_DMDN6.lib")
#endif
//
//
//
bool EnvInit::init(const char* appname, int prtype) {

	// Note - both these functions return void
	// Note that AP_InitProcess must preceed AP_SetCleanupAndCrashRoutine
	if (prtype == ap_command)
		AP_InitProcess(appname, AP_COMMAND); // set process priority (why?)
	else
		AP_InitProcess(appname, AP_SERVICE); // set process priority (why?)

	AP_SetCleanupAndCrashRoutine(appname, NULL);
	return true; // always true, incl. for future use
}
