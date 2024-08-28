//******************************************************************************
// COPYRIGHT Ericsson Utvecklings AB, Sweden 2012.
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
// CPS_BUAP_Main.cpp
//
// DESCRIPTION
// -
//
// DOCUMENT NO
// 190 89-CAA 109 1414
//
// AUTHOR
//  XDT/DEK XNGUDAN
//
//******************************************************************************
// === Revision history ===
// 2012-01-04   XNGUDAN    Create
//******************************************************************************


#include "CPS_BUAP_Service.h"

#include <string>
#include <sys/eventfd.h>
#include <iostream>
#include <signal.h>
#include <boost/thread/thread.hpp>
#include "boost/thread/recursive_mutex.hpp"
#include <boost/bind.hpp>


using namespace std;

#define STR_NO_SERVICE "noservice"
#define STR_SERVICE "service"
const int FAILED = -100;

// Define some local functions
int run_noservice(bool isRunAsBg);
int run_as_service();
void catch_signal(int signal);
void install_signal_handler();
bool initialize();
void printHelpMessage(const std::string &appName);

// Global declaration for filemgr, parmgr, loader services.
CPS_BUAP_Loader*  pLoaderServer = NULL;
CPS_BUAP_PARMGR*  pParServer    = NULL;
CPS_BUAP_FILEMGR* pFileServer   = NULL;
StaticParamsOI*   pStaticParamsOI = NULL;
ClassicOI*        pClassicOI    = NULL;

// Global declaration for reporting error.
CPS_BUAP_Service* pService = NULL;
bool reportErrorFlag = false;
boost::recursive_mutex mutex;

/**
 * @brief: Main
 *
 */

int main(int argc, char* argv[])
{
 /* CHANGE FOR TRACING ORDER

    initTRACE();
    newTRACE((LOG_LEVEL_INFO, "main()", 0));
 */
    int ret_val = EXIT_SUCCESS;

    // Initialize instance for BUPARMGR
    initialize();

    //**************************************************************************
    // Check if any arguments was given
    //**************************************************************************
    switch (argc)
    {
    case 1:
    {
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

 // TRACE((LOG_LEVEL_INFO, "Main() returned with %d", 0, ret_val));
    
    //termTRACE();
    return ret_val;
}

/**
 * @brief: run_noservice
 *
 */
int run_noservice(bool isRunAsBg)
{
	initTRACE();

    newTRACE((LOG_LEVEL_INFO, "You are running in NOSERVICE mode for debugging only...\n", 0));

    int result = EXIT_SUCCESS;
    // Register signal to handle
    install_signal_handler();

    try
    {
        // Constructor server
        const char* strReloadFileName = CPS_BUAP_Config::instance().cpsReladmpar().c_str();
        TRACE(("reladmpar file path + name: %s", 0, strReloadFileName));

        ClassicOI classicOI;
        StaticParamsOI staticParamsOI;
        CPS_BUAP_PARMGR parMgr(strReloadFileName);
        CPS_BUAP_Loader loader;
        CPS_BUAP_FILEMGR fileMgr;

        pClassicOI      = &classicOI;
        pStaticParamsOI   = &staticParamsOI;
        pParServer      = &parMgr;
        pLoaderServer   = &loader;
        pFileServer     = &fileMgr;

        // Start OI thread
        boost::thread threadClassic(boost::bind(&ClassicOI::run, &classicOI));
        classicOI.waitUntilRunning();

        boost::thread threadStaticparams(boost::bind(&StaticParamsOI::run, &staticParamsOI));
        staticParamsOI.waitUntilRunning();

        // Start BUPARMGR thread
        boost::thread threadPar(boost::bind(&CPS_BUAP_PARMGR::Run, &parMgr));
        parMgr.waitUntilRunning();

        // Start BULOADER thread
        boost::thread threadLoad(boost::bind(&CPS_BUAP_Loader::Run, &loader));
        loader.waitUntilRunning();

        // Start BUFILEMGR thread
        boost::thread threadFile(boost::bind(&CPS_BUAP_FILEMGR::Run, &fileMgr));
        fileMgr.waitUntilRunning();

        if (!isRunAsBg)
        {
            std::cin.get();

            // Inform service to stop
            fileMgr.stop();
            loader.stop();
            parMgr.stop();
            staticParamsOI.stop();
            classicOI.stop();
        }

        // Wait for thread to terminate itself
        threadFile.join();
        threadLoad.join();
        threadPar.join();
        threadStaticparams.join();
        threadClassic.join();

    }
    catch (...)
    {
        TRACE((LOG_LEVEL_ERROR, "Exception : terminate server!", 0));
        result = EXIT_FAILURE;
    }

    pClassicOI      = NULL;
    pStaticParamsOI = NULL;
    pParServer      = NULL;
    pLoaderServer   = NULL;
    pFileServer     = NULL;

    TRACE((LOG_LEVEL_INFO, "Run_as_console returns with %d ", 0, result));
    //termTRACE();

    return result;
}


/**
 * @brief: run_as_service
 *
 */
int run_as_service()
{
    ACS_APGCC_HA_ReturnType retCode = ACS_APGCC_HA_SUCCESS;

  // newTRACE((LOG_LEVEL_INFO, "Start BUAP server as service.", 0));

       
        const char* strReloadFileName = CPS_BUAP_Config::instance().cpsReladmpar().c_str();
   //   TRACE(("reladmpar file path + name: %s", 0, strReloadFileName));

        ClassicOI classicOI;
        StaticParamsOI staticParamsOI;
        CPS_BUAP_PARMGR parMgr(strReloadFileName);
        CPS_BUAP_Loader loader;
        CPS_BUAP_FILEMGR fileMgr;

        pClassicOI      = &classicOI;
        pStaticParamsOI = &staticParamsOI;
        pParServer      = &parMgr;
        pLoaderServer   = &loader;
        pFileServer     = &fileMgr;


        // Constructor service
        CPS_BUAP_Service serviceObj(CPS_BUAP_Service::BUAP_DEF_DAEMON_NAME,
                                    parMgr, loader, fileMgr, staticParamsOI, classicOI);

        initTRACE();
        newTRACE((LOG_LEVEL_INFO, "Start BUAP server as service.", 0));

        pService = &serviceObj;
    try
    {
        // Activate service
        serviceObj.run();

    }
    catch (...)
    {
        TRACE((LOG_LEVEL_ERROR, "Exception, exit BUAP server.", 0));
        EVENT((CPS_BUAP_Events::death_of_server, 0, "CPS_BUAP_Service"));

    	pClassicOI      = NULL;
    	pStaticParamsOI = NULL;
    	pParServer      = NULL;
    	pLoaderServer   = NULL;
    	pFileServer     = NULL;

        return FAILED;
    }

    TRACE((LOG_LEVEL_INFO, "Exit BUAP server.", 0));
    pClassicOI      = NULL;
    pStaticParamsOI = NULL;
    pParServer      = NULL;
    pLoaderServer   = NULL;
    pFileServer     = NULL;

    TRACE((LOG_LEVEL_INFO, "Run_as_service returns with %d " , 0, retCode));
   // termTRACE();

    return retCode;
}

/**
 * @brief: catch_signal
 *
 */
void catch_signal(int signal)
{
    newTRACE(("catch_signal(%d)", 0, signal));

    // Receive signal SIGTERM, thus stop the service
    if ((signal == SIGINT) | (signal == SIGQUIT) | (signal == SIGTERM))
    {
	if (pFileServer)
	{
            pFileServer->stop();
        }

        if (pLoaderServer) 
        {
            pLoaderServer->stop();
        }

	if (pParServer) 
	{
            pParServer->stop();
	}

	if (pStaticParamsOI)
	{
            pStaticParamsOI->stop();
	}

	if (pClassicOI)
	{
            pClassicOI->stop();
	}
    }
}

/**
 * @brief: install_signal_handler
 *
 */
void install_signal_handler()
{
    newTRACE(("install_signal_handler()", 0));

    struct sigaction signalAction;

    sigemptyset(&signalAction.sa_mask);
    signalAction.sa_handler = catch_signal;
    signalAction.sa_flags = 0;

    // Register to handle SIGTERM
    sigaction(SIGINT, &signalAction, NULL);
    sigaction(SIGQUIT, &signalAction, NULL);
    sigaction(SIGTERM, &signalAction, NULL);
}

/**
 * @brief: initialize
 *
 */
bool initialize()
{
    bool retval = true;
 /* CHANGE FOR TRACING ORDER

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
 */
    return retval;
}

/**
 * @brief: printHelpMessage
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

