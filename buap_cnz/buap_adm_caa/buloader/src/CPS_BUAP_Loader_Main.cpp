//*****************************************************************************
//
// COPYRIGHT Ericsson Utvecklings AB, Sweden 1999.
// All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson Utvecklings AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.
//
//  NAME
//  CPS_BUAP_Loader_Main.cpp
//
//  DESCRIPTION 
//  Main program for the buloader process.
//
//  DOCUMENT NO
//  190 89-CAA 109 1412
//
//  AUTHOR 
//  990814 UAB/B/SF Birgit Berggren
//  101215 xchihoa  Port to Linux for APG43L
//
//*****************************************************************************
// === Revision history ===
// 990914 BIR PA1 Created.
//******************************************************************************
#include "CPS_BUAP_Linux.h"
#include "CPS_BUAP_Trace.h"
#include "CPS_BUAP_Loader_Service.h"
#include "CPS_BUAP_Loader.h"
#include "CPS_BUAP_Events.h"

#include "ACS_PRC_Process.h"

#include <iostream>
#include <syslog.h>

#define STR_NO_SERVICE "noservice"
#define STR_SERVICE "service"

bool set_event(int iEventFd);
int run_noservice(bool isRunAsBg);
void catch_signal();
void install_signal_handler();
void printHelpMessage(const char* appName);

CPS_BUAP_Loader_Service *pLoaderService = NULL;

int main(int argc, char *argv[])
{
   //**************************************************************************
   // Set the priority of the process. DR PP1
   //**************************************************************************
   //TODO: wait for DR for this
   //AP_InitProcess("CPS_BUAP_LOADER",AP_SERVICE);
   // AP_SetCleanupAndCrashRoutine("CPS_BUAP_LOADER", NULL);

   initTRACE();
   newTRACE(("main()", 0));

   // TODO: print syslog messages
   openlog ("CPS_BUAP_Loader_Main", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL0);

   int ret_val = EXIT_SUCCESS;

   //**************************************************************************
   // Check if any arguments was given
   //**************************************************************************
   switch (argc)
   {
   case 1:
   {
      install_signal_handler();
      ret_val = run_noservice(true);
      break;
   }
   case 2:
   {
      string strParam2(argv[1]);
      transform(strParam2.begin(), strParam2.end(), strParam2.begin(), (int(*)(int)) ::tolower);
      if (strParam2 == STR_NO_SERVICE)
      {
         install_signal_handler();
         ret_val = run_noservice(false);
      }
      else if (strParam2 == STR_SERVICE)
      {
         CPS_BUAP_Loader_Service loaderService("cps_buaploaderd", "root");

         pLoaderService = &loaderService;

         ret_val = pLoaderService->amfInitialize();
         pLoaderService = NULL;

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

   syslog (LOG_INFO, "CPS_BUAP_Loader_Main() returned with %d", ret_val);
   closelog();

   return ret_val;
}

//**********************************************************************
// Run as console app (only in debug mode)
//**********************************************************************
int run_noservice(bool isRunAsBg)
{
   newTRACE((LOG_LEVEL_INFO, "You are running in NOSERVICE mode for debugging only.", 0));

   pthread_t loaderThread;

   int ret_val = pthread_create(&loaderThread, NULL, runBuapLoader,NULL);
   if (ret_val != 0)
   {
      TRACE((LOG_LEVEL_ERROR, "Can not create Loader thread!!!", 0));
      //TODO: raise event
   }

   if(!isRunAsBg)
   {
       std::cin.get();

       // Inform Loader service to stop
       CPS_BUAP_Loader::setStopEvent();
   }

   // Wait for Loader thread to terminate itself
   ret_val = pthread_join(loaderThread, NULL);

   return ret_val;
}


void catch_signal(int signal)
{
   newTRACE(("catch_signal(%d)", 0, signal));

   // Receive signal SIGTERM, thus stop the service
   if ((signal == SIGTERM) | (signal == SIGINT) | (signal == SIGQUIT))
   {
      CPS_BUAP_Loader::setStopEvent();
   }
}

void install_signal_handler()
{
   newTRACE(("install_signal_handler", 0));

   struct sigaction signalAction;

   sigemptyset(&signalAction.sa_mask);
   signalAction.sa_handler = catch_signal;
   signalAction.sa_flags = 0;

   // Register to handle SIGTERM
   sigaction(SIGINT, &signalAction, NULL);
   sigaction(SIGQUIT, &signalAction, NULL);
   sigaction(SIGTERM, &signalAction, NULL);
}

void printHelpMessage(const char* appName)
{
   std::cout
         << "This service is intended to run as an AMF Service without argument"
         << std::endl;
   std::cout
         << "If you want to run it as a normal application (for debugging only), please use:"
         << std::endl;
   std::cout << "Usage: " << appName << "noservice" << std::endl;
}

