//******************************************************************************
// COPYRIGHT Ericsson Utvecklings AB, Sweden 2011.
// All rights reserved.
//
// The Copyright to the computer program(s) herein 
// is the property of Ericsson Utvecklings AB, Sweden.
// The program(s) may be used and/or copied only with 
// the written permission from Ericsson Utvecklings AB or in 
// accordance with the terms and conditions stipulated in the 
// agreement/contract under which the program(s) have been 
// supplied.
//
// NAME
// CPS_BUAP_FILEMGR_Main.cpp
//
// DESCRIPTION 
// Main program for the bufilemgr process.
// 
//
// DOCUMENT NO
// 190 89-CAA 109 1414
//
// AUTHOR 
// 110912 XDT/DEK XNGUDAN
//
//******************************************************************************
// === Revision history ===
// 990512 BIR  PA1 Created.
// 990607 BIR  PA2 Updated.
// 110912 DANH     Updated (migrated from Win2k3 to Linux).
//******************************************************************************


#include "CPS_BUAP_Linux.h"


#include "CPS_BUAP_Trace.h"
#include "CPS_BUAP_Events.h"
#include "CPS_BUAP_FILEMGR.h"
#include "CPS_BUAP_FILEMGR_Service.h"
#include "ACS_APGCC_AmfTypes.h"

//#include "ACS_PRC_Process.h"

#include <iostream>
#include <cstring>
#include <sys/eventfd.h>
#include <pthread.h>
#include <signal.h>
#include <syslog.h>

#define STR_NO_SERVICE "noservice"
#define STR_SERVICE "service"
const int FAILED = -100;


int run_noservice(bool isRunAsBg);
int run_as_service();
void printHelpMessage(const std::string &appName);




/**
 * Run BUAP File Manager Server - Global thread function
 *
 * @param   piTerminateEventFd      Pointer to a terminated Event
 * @return  N/A
 */
void *runBuapFileMgr(void *piTerminateEventFd)
{
   newTRACE(("runBuapFileMgr()", 0));

   try
   {
      CPS_BUAP_FILEMGR filemgr((int *)piTerminateEventFd);
      filemgr.Run();
   } catch (...)
   {
      TRACE((LOG_LEVEL_ERROR, "BUAP_FileMgr runs with Exception!!!", 0));
      EVENT((CPS_BUAP_Events::death_of_server, 0, "CPS_BUAP_filemgr"));
   }

   pthread_exit(NULL);
}


/**
 * main() function
 *
 * @param   argc, argv[]
 * @return
 */
int main(int argc, char *argv[])
{
   //**************************************************************************
   // Set the priority of the process. DR PP1
   //**************************************************************************
   //TODO: wait for DR for this
   //AP_InitProcess("CPS_BUAP_FILEMGR",AP_SERVICE);
   // AP_SetCleanupAndCrashRoutine("CPS_BUAP_FILEMGR", NULL);

   initTRACE();
   newTRACE(("main()", 0));

   // TODO: print syslog messages
   openlog ("CPS_BUAP_FILEMGR_Main", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL0);

   int ret_val = EXIT_SUCCESS;

   //**************************************************************************
   // Check if any arguments was given
   //**************************************************************************
   switch (argc)
   {
       case 1:
       {
           TRACE((LOG_LEVEL_INFO, "run as no service.", 0));

           ret_val = run_noservice(true);

           break;
       }
       case 2:
       {
           string strParam2(argv[1]);
           transform(strParam2.begin(), strParam2.end(), strParam2.begin(), (int(*)(int)) ::tolower);
           if (strParam2 == STR_NO_SERVICE)
           {
               ret_val = run_noservice(false);
           }
           else if (strParam2 == STR_SERVICE)
           {
               ret_val = run_as_service();
           }
           else
           {
               printHelpMessage(argv[0]);
               ret_val = EXIT_FAILURE;
           }
           break;
       }
       default:
       {
           printHelpMessage(argv[0]);
           ret_val = EXIT_FAILURE;
           break;
       }
   } //switch

   TRACE(("Main() returned with %d" , 0, ret_val));

   syslog (LOG_INFO, "CPS_BUAP_FILEMGR_Main() returned with %d", ret_val);
   closelog();

   return ret_val;
}


/**
 * Run as console app (only in debug mode)
 *
 * @param   N/A
 * @return  -
 */
int run_noservice(bool isRunAsBg)
{
   newTRACE((LOG_LEVEL_INFO, "You are running in NOSERVICE mode for debugging only\n", 0));

   int ret_val = -1;

   try
   {
       // Block all signals for background thread.
       sigset_t new_mask;
       sigfillset(&new_mask);
       sigset_t old_mask;
       pthread_sigmask(SIG_BLOCK, &new_mask, &old_mask);

       int iEventFd = eventfd(0, 0);
       if (iEventFd == -1)
       {
          TRACE((LOG_LEVEL_ERROR, "Create event failed!!!", 0));
          return EXIT_FAILURE;
       }

       pthread_t filemgrThread;

       TRACE(("Server started", 0));

       ret_val = pthread_create(&filemgrThread, NULL, runBuapFileMgr, (void*) &iEventFd);
       if (ret_val != 0)
       {
          TRACE((LOG_LEVEL_ERROR, "Can not create File Manager thread!!!", 0));
          EVENT((CPS_BUAP_Events::beginthreadex_fail, 0, ""));
       }

       // Restore previous signals.
       pthread_sigmask(SIG_SETMASK, &old_mask, 0);

       // Wait for signal indicating time to shut down.
       sigset_t wait_mask;
       sigemptyset(&wait_mask);

       sigaddset(&wait_mask, SIGINT);
       sigaddset(&wait_mask, SIGQUIT);
       sigaddset(&wait_mask, SIGTERM);

       pthread_sigmask(SIG_BLOCK, &wait_mask, 0);

       int sig = 0;

       if (isRunAsBg)
       {
           sigwait(&wait_mask, &sig);

       }
       else
       {
           std::cin.get();

       }
       // Inform File Manager service to stop
       CPS_BUAP_FILEMGR::set_event(iEventFd);

       // Wait for FIle Manager thread to terminate itself
       ret_val = pthread_join(filemgrThread, NULL);


   }
   catch (...)
   {

       TRACE((LOG_LEVEL_ERROR, "Exception, exit BUFILEMGR server.", 0));
       EVENT((CPS_BUAP_Events::death_of_server, 0, "CPS_BUAP_filemgr"));

       return FAILED;
   }
   return ret_val;
}

/**
 * Run as amf service
 *
 * @param   N/A
 * @return  -
 */
int run_as_service()
{

    ACS_APGCC_HA_ReturnType retCode = ACS_APGCC_HA_SUCCESS;
    int iSigtermEventFd = -1;

    newTRACE((LOG_LEVEL_INFO, "Start BUFILEMGR server as service.", 0));

    iSigtermEventFd = eventfd(0, 0);

    if (iSigtermEventFd == -1)
    {
        TRACE((LOG_LEVEL_ERROR, "Create iSigtermEventFd event failed!!!", 0));
        return -1;
    }

    try
    {
        CPS_BUAP_FILEMGR server((int*) &iSigtermEventFd);
        // Construct server
        CPS_BUAP_FILEMGR_Service serviceObj(CPS_BUAP_FILEMGR_Service::BUFILEMGR_DEF_DAEMON_NAME, server);

        // Activate service
        serviceObj.run();

    }
    catch (...)
    {
        TRACE((LOG_LEVEL_ERROR, "Exception, exit BUFILEMGR server.", 0));
        EVENT((CPS_BUAP_Events::death_of_server, 0, "CPS_BUAP_filemgr"));

        return FAILED;
    }

    TRACE((LOG_LEVEL_INFO, "Exit BUFILEMGR server.", 0));

    return retCode;
}


/**
 * Print help message
 *
 */
void printHelpMessage(const std::string &appName)
{
   std::cout
         << "This service is intended to run as an AMF Service without argument"
         << std::endl;
   std::cout
         << "If you want to run it as a normal application (for debugging only), please use:"
         << std::endl;
   std::cout << "Usage: " << appName << "noservice" << std::endl;
}

