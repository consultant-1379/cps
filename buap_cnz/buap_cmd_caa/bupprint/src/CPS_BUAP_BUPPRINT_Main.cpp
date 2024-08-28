//******************************************************************************
//
// NAME
//      %File_name:CPS_BUAP_BUPPRINT_Main.cpp%
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
//      Main program for the bupprint command.

// DOCUMENT NO
//	190 89-CAA 109 0082

// AUTHOR 
// 	1997/08/06 by U/B/SD David Wade

// CHANGES
// 991102 BIR PA2 Setting of process priority added.

// SEE ALSO 
// 	<Related classes, (abstract) baseclasses, friend classes, ...>
//
//******************************************************************************

#include <stdio.h>

//#include "ACS_PRC_Process.h"
//#include <ACS_ExceptionHandler.h>

#include "CPS_BUAP_Trace.h"
#include "CPS_BUAP_BUPPRINT.h"
#include "CPS_BUAP_Config.h"

#ifdef _DEBUG
//:ag1
bool g_consoleMode = true;
#endif
//******************************************************************************
//	main()
//******************************************************************************
int main(int argc, char** argv)
{
  
  //**************************************************************************
  // Set the priority of the process.
  //**************************************************************************
	//AP_InitProcess("BUPPRINT",AP_COMMAND);

	initTRACE();
	//AP_SetCleanupAndCrashRoutine("BUPPRINT", NULL);

  try
    {
      newTRACE(("main()", 0));

      // Check if system is classic or not
      if (!CPS_BUAP_Config::instance().isClassicCP())
      {
          // bupprint command not run on non-classic system
          throw CPS_BUAP_BUPPRINT::eIllegalCommand;
      }

      CPS_BUAP_BUPPRINT BUPPRINT;

      BUPPRINT.Initialise();

      BUPPRINT.ParseCommandLine(argc, argv);

      BUPPRINT.ExecuteCommand();
    }
  catch ( CPS_BUAP_BUPPRINT::eError EE )  // lan
    {
      switch(EE)
        {
          case CPS_BUAP_BUPPRINT::eNoError:
            break;

          case CPS_BUAP_BUPPRINT::eSyntaxError:
            // syntax error already reported
            printf("usage: bupprint -acdlm\n");
            break;

          case CPS_BUAP_BUPPRINT::eNoServer:
            printf("bupprint: failed to connect to server\n");
            break;

          case CPS_BUAP_BUPPRINT::eCPFSerror:
            printf("bupprint: CPFS error\n");
            break;

          case CPS_BUAP_BUPPRINT::eIllegalCommand:
              printf("bupprint: Illegal command in this system configuration\n");
              break;

          case CPS_BUAP_BUPPRINT::eUnknown:
          default:
            printf("bupprint: unknown internal error\n");
        }

        return EE;
    }
  catch ( ... )
    {
      printf("bupprint: unknown internal exception\n");
      return CPS_BUAP_BUPPRINT::eUnknown;
    }
  //:ag1
  return 0;
}
