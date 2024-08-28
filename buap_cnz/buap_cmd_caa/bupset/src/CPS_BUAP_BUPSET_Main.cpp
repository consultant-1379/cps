
//******************************************************************************
//
// NAME
//      %File_name:CPS_BUAP_BUPSET_MAIN.cpp%
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
// 	Main program for the bupset command.

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
//#include "CPS_BUAP_Linux.h"

//#include "ACS_PRC_Process.h"
//#include <ACS_ExceptionHandler.h>

#include "CPS_BUAP_Config.h"

#include "CPS_BUAP_BUPSET.h"
#include "CPS_BUAP_Trace.h"

#include <stdio.h>
#include <iostream>
#include <string>

using namespace std;

//******************************************************************************
//	main()
//******************************************************************************
int main(int argc, char** argv)
{
    //**************************************************************************
	// Set the priority of the process.
	//**************************************************************************
	//AP_InitProcess("BUPSET",AP_COMMAND);
	
	initTRACE();
	//AP_SetCleanupAndCrashRoutine("BUPSET", NULL);
	
	try
	{
      newTRACE(("main()", 0));
		
      // Check if system is classic or not
        if (!CPS_BUAP_Config::instance().isClassicCP())
        {
            // bupset command not run on non-classic system
            throw CPS_BUAP_BUPSET::eIllegalCommand;
        }

      CPS_BUAP_BUPSET bupset;
#if 0
      BUPSET.Initialise();
		
      BUPSET.ParseCommandLine(argc, argv);
		
      BUPSET.ExecuteCommand();
#endif

      if (!bupset.init())
      {
          cout << bupset.getLastErrorText() << endl;
          //return EXIT_FAILURE;
          throw CPS_BUAP_BUPSET::eNoServer;
      }

      int opt, clh, incl1, incl2, loaz, ntaz, ntcz, sup;
      string incla;
      //bool print = false;

      while ((opt = getopt(argc, argv, "rfm:t:c:o:d:1:2:s:")) != EOF)
      {
          switch (opt)
          {
          case 'r':
              if (!bupset.doReset())
              {
                  cout << "Error : " << bupset.getLastErrorText() << endl;
              }
              exit(EXIT_SUCCESS);
              break;

          case 'f':
              cout << "Not yet supported!" << endl;
              break;

          case 'm':
              clh = atoi(optarg);
              bupset.setCLH((uint32_t) clh);
              break;

          case 't':
              ntaz = atoi(optarg);
              bupset.setNTAZ((uint32_t) ntaz);
              break;

          case 'c':
              ntcz = atoi(optarg);
              bupset.setNTCZ((uint32_t) ntcz);
              break;

          case 'o':
              loaz = atoi(optarg);
              bupset.setLOAZ((uint32_t) loaz);
              break;

          case 'd':
              incla = optarg;
              bupset.setINCLA(incla);
              break;

          case '1':
              incl1 = atoi(optarg);
              bupset.setINCL1((uint32_t) incl1);
              break;

          case '2':
              incl2 = atoi(optarg);
              bupset.setINCL2((uint32_t) incl2);
              break;

          case 's':
              sup = atoi(optarg);
              bupset.setSUP((uint32_t) sup);
              break;

          default: /* '?' */
              cout << "Syntax: bupset [ -f] [ -r] [ -1 INCL1] [ -2 INCL2] [ -c NTCZ] "
                      "[ -d INCLA] [ -m CLH] [ -o LOAZ] [ -s SUP] [ -t NTAZ]" << endl;
              exit(EXIT_FAILURE);
              break;
          }
      }

      if (!bupset.update())
      {
          cout << "Error : " << bupset.getLastErrorText() << endl;
          return EXIT_FAILURE;
      }

	}
	catch ( CPS_BUAP_BUPSET::eError &EE )
	{
      switch(EE)
		{
		case CPS_BUAP_BUPSET::eNoError:
			break;
			
		case CPS_BUAP_BUPSET::eSyntaxError:
			// syntax error already reported
			printf("usage: bupset -rf -1 INCL1 -2 INCL2 -c NTCZ -d INCLA\n");
			printf("                  -m CLH   -o LOAZ  -s SUP  -t NTAZ\n");
			break;
			
		case CPS_BUAP_BUPSET::eInvalidValue:
			// invalid value already reported
			break;
			
		case CPS_BUAP_BUPSET::eNoServer:
			printf("bupset: failed to connect to IMM\n");
			break;
			
		case CPS_BUAP_BUPSET::eLoadInProgress:
			printf("bupset: load in progress\n");
			break;
			
		case CPS_BUAP_BUPSET::eCPFSerror:
			printf("bupset: CPFS error\n");
			break;
		case CPS_BUAP_BUPSET::eIllegalCommand:
          printf("bupset: Illegal command in this system configuration\n");
          break;

		case CPS_BUAP_BUPSET::eUnknown:
		default:
			printf("bupset: unknown internal error\n");
		    break;
		}
      return int(EE);
	}
	catch ( ... )
	{
      printf("bupset: unknown internal exception\n");
      return int(CPS_BUAP_BUPSET::eUnknown);
	}

    return EXIT_SUCCESS;
}
