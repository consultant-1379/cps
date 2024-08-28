//******************************************************************************
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 2011.
//  All rights reserved.
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
//  CPS_BUAP_PARMGR_Main.cpp
//
//  DESCRIPTION 
// 	Main program for the buparmgr process.
//
//  DOCUMENT NO
//	190 89-CAA 109 1413
//
//  AUTHOR 
// 	1999/07/22 by UAB/B/SF Birgit Berggren
//
//  2011/11/01 by XDT/DEK XNGUDAN
//
//******************************************************************************
// === Revision history ===
// 990722 BIR PA1 Created.
// 010909 XCHIHOA Ported to Linux for APG43L
// 111101 XNGUDAN Updated with HA Adaptation
//******************************************************************************

#include "CPS_BUAP_PARMGR.h"
#include "CPS_BUAP_PARMGR_Service.h"
#include "CPS_BUAP_Linux.h"
#include "CPS_BUAP_Config.h"
#include "CPS_BUAP_Trace.h"
#include "CPS_BUAP_Events.h"
#include "CPS_BUAP_PARMGR_Global.h"
#include "CPS_BUAP_Exception.h"

#include <string.h>
#include <sys/eventfd.h>
#include <iostream>
#include <syslog.h>


#define STR_NO_SERVICE "noservice"
#define STR_SERVICE "service"

// fwd for local function
//----------------------------------------------------------------------------

int run_noservice(bool isRunAsBg);
int run_as_service();
void catch_signal();
void install_signal_handler();
void printHelpMessage(const char* appName);

bool initialize();
void *runBuapParMgr(void *piTerminateEventFd);

int iStopEventFd = -1;
const int FAILED = -100;

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
    openlog ("CPS_BUAP_PARMGR_Main", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL0);

    int ret_val = EXIT_SUCCESS;
    iStopEventFd = eventfd(0, 0);

    if (iStopEventFd == -1)
    {
        TRACE((LOG_LEVEL_ERROR, "Create iStopEventFd event failed!!!", 0));
        EVENT((CPS_BUAP_Events::lost_handle, 0, "Create iStopEventFd event failed!"));
        return -1;
    }

    TRACE(("Create iStopEventFd event %d", 0, iStopEventFd));

    initialize();

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
            ret_val = run_as_service();
            break;
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

    TRACE(("Main() returned with %d", 0, ret_val));

    syslog (LOG_INFO, "CPS_BUAP_PARMGR_Main() returned with %d", ret_val);
    closelog();

    close(iStopEventFd);

    return ret_val;
}

//**********************************************************************
// Run as console app (only in debug mode)
//**********************************************************************
int run_noservice(bool isRunAsBg)
{
    newTRACE((LOG_LEVEL_INFO, "You are running in NOSERVICE mode for debugging only\nPress Enter key to stop...", 0));

    pthread_t parmgrThread;

    int ret_val = pthread_create(&parmgrThread, NULL, runBuapParMgr, (void*) &iStopEventFd);
    if (ret_val != 0)
    {
        TRACE((LOG_LEVEL_ERROR, "Can not create Parmgr thread!!!", 0));
        EVENT((CPS_BUAP_Events::beginthreadex_fail, 0, ""))
    }

    if (!isRunAsBg)
    {
        std::cin.get();

        // Inform Loader service to stop
        CPS_BUAP_PARMGR::set_event(iStopEventFd);
    }

    // Wait for Loader thread to terminate itself
    ret_val = pthread_join(parmgrThread, NULL);

    return ret_val;
}

//**********************************************************************
// run_as_service()
//**********************************************************************
int run_as_service()
{
    ACS_APGCC_HA_ReturnType retCode = ACS_APGCC_HA_SUCCESS;

    newTRACE((LOG_LEVEL_INFO, "Start BUPARMGR server as service.", 0));

    try
    {
        const char* strReloadFileName = CPS_BUAP_Config::instance().cpsReladmpar().c_str();
        TRACE(("reladmpar file path + name: %s", 0, strReloadFileName));

        CPS_BUAP_PARMGR parMgr((int*) &iStopEventFd, strReloadFileName);

        // Constructor service
        CPS_BUAP_PARMGR_Service serviceObj(CPS_BUAP_PARMGR_Service::BUPARMGR_DEF_DAEMON_NAME, parMgr);

        // Activate service
        serviceObj.run();

    }
    catch (...)
    {
        TRACE((LOG_LEVEL_ERROR, "Exception, exit BUPARMGR server.", 0));
        EVENT((CPS_BUAP_Events::death_of_server, 0, "CPS_BUAP_parmgr"));

        return FAILED;
    }

    TRACE((LOG_LEVEL_INFO, "Exit BUPARMGR server.", 0));

    return retCode;
}

void catch_signal(int signal)
{
    newTRACE(("catvch_signal(%d)", 0, signal));

    // Receive signal SIGTERM, thus stop the service
    if ((signal == SIGINT) | (signal == SIGQUIT) | (signal == SIGTERM))
    {
        CPS_BUAP_PARMGR::set_event(iStopEventFd);
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

bool initialize()
{
    bool retval = true;
    newTRACE(("initialize()", 0));
    try
    {
        // force first read of registry
        //const CPS_BUAP_Config& forceInit = CPS_BUAP_Config::instance();
        CPS_BUAP_Config::instance();

    } catch (...)
    {
        EVENT((CPS_BUAP_Events::no_expand_path_val, 0, "Error in force reading of registry during process start-up."));
    }

    TRACE(("initialize return value %d", 0, retval));
    return retval;
}

void *runBuapParMgr(void *piTerminateEventFd)
{
   newTRACE(("runBuapParMgr()", 0));

   try
   {

      const char* strReloadFileName = CPS_BUAP_Config::instance().cpsReladmpar().c_str();
      TRACE(("reladmpar file path + name: %s", 0, strReloadFileName));

      CPS_BUAP_PARMGR parMgr((int*) piTerminateEventFd, strReloadFileName);
      parMgr.Run();

   } catch (...)
   {
      TRACE((LOG_LEVEL_ERROR, "BUAP_PARMGR runs with Exception!!!", 0));
      EVENT((CPS_BUAP_Events::death_of_server, 0, "CPS_BUAP_filemgr"));
   }

   pthread_exit(NULL);
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

