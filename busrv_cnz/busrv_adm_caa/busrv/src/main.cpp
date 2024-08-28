/*
NAME
   File_name: main.cpp

COPYRIGHT Ericsson AB, Sweden 2012. All rights reserved.

   The Copyright to the computer program(s) herein is the property of Ericsson
   Utvecklings AB, Sweden.
   The program(s) may be used and/or copied only with the written permission from
   Ericsson Utvecklings AB or in accordance with the terms and conditions
   stipulated in the agreement/contract under which the program(s) have been
   supplied.

DESCRIPTION
   The entry point of BUSRV.

DOCUMENT NO
   190 89-CAA 109 1409

AUTHOR
   2010-08-25 by Quyen Dao

SEE ALSO


Revision history
----------------
2010-08-25 xquydao Created
2010-10-20 xdtthng Updated

*/

#include <iostream>
#include <set>
#include <boost/thread.hpp>
#include <boost/regex.hpp>
#include <algorithm>
#include <string>
#include <cstdlib>

// For signal handling
#include <signal.h>
#include <stddef.h>
#include <sys/types.h>

#ifdef _APGUSERSVC_
#include <pwd.h>
#include <unistd.h>
#include <grp.h>
#else
#include <sys/capability.h>
#endif

#include "Config.h"
#include "CPS_BUSRV_Trace.h"
#include "BUService.h"

#include "BUParamsOI.h"
#include "BUParamsOM.h"
#include "GAServer.h"

const char* NOSERVICE = "noservice";
const char* SERVICE   = "service";

void help(char*);
static int run_as_console();
static int run_as_service(void);
static int run_as_service(int);
static bool initialise();

// For signal handling
void catch_signal();
void install_signal_handler();
BUServer* g_pServer = NULL;
BUService* g_pService = NULL;
BUParamsOI* g_pOI = NULL;

int main(int argc, char* argv[])
{
    // Perform initialisation as required by AP
    initialise();

    // No TRACE shall be invoked before this point
    // initTRACE();
    // newTRACE((LOG_LEVEL_INFO, "main() starts", 0));
        
    int retVal = EXIT_SUCCESS;

    // ------------------------------------------------------------------------
    // Parsing command line
    // ------------------------------------------------------------------------
    	
    int opt;
    bool helpOpt = false;
    opterr = 0;
    while((opt = getopt(argc, argv, "h")) != -1) {
        switch(opt) {
        case 'h':
        case '?':
            helpOpt = true;
            break;
        default:;
        }
    }

    if (helpOpt) {
        help(argv[0]);
        return EXIT_FAILURE;
    }
    
    if (argc == 1) {
    
        // Run as Linux daemon for testing purposes
        // This mode was used in the early stage of the project
        retVal = run_as_service(0xffff);
    }
    else if (argc == 2) {
    	string t(argv[1]);
    	transform(t.begin(), t.end(), t.begin(),(int (*)(int)) ::tolower);
    	if (t == NOSERVICE) {
            retVal = run_as_console();
        }
        else if (t == SERVICE) {
            // Run as AMF Service
        	retVal = run_as_service();
        }
        else {
            help(argv[0]);
            retVal = EXIT_FAILURE;
        }
    }
    else {
        help(argv[0]);
        retVal = EXIT_FAILURE;
    }


    // Change for Tracing Order

    // No tracing shall be done after main() returns.
    // This is the last trace
    //newTRACE((LOG_LEVEL_INFO, "main() returns with return code: %d", 0, retVal));
    //termTRACE();
    return retVal;
}

int run_as_console()
{
	// Change for Tracing Order
    // No TRACE shall be invoked before this point
    initTRACE();
    newTRACE((LOG_LEVEL_INFO, "run_as_console()", 0));

        int ret = EXIT_SUCCESS;

    try {
        /// Construct server and JobThread
        BUServer   server;
        JobThread  jobThread(server);

        BUParamsOI oi;
        BUParamsOM om;

        /// Create CPReloadParam & instance in IMM then sync file to IMM
        if (!om.createAndSyncBUParamObject())
        {
            if (om.getLastError() != BUParamsCommon::UNSUPPORTED_SYSTEM)
            {
                newTRACE((LOG_LEVEL_ERROR, "Failed to create CPReload object", 0));
                // TODO: keep running?
            }
        }

        server.set(&jobThread);

        /// Start JobThread thread
        boost::thread jobThread_thread(boost::bind(&JobThread::run, &jobThread));
        jobThread.waitUntilRunning();

        /// Start BUServer thread
        boost::thread server_thread(boost::bind(&BUServer::run, &server));
        server.waitUntilRunning();

        /// Start OI
        boost::thread oi_thread(boost::bind(&BUParamsOI::run, &oi));

        GAServer m_gaServer(Config::DSD_SERVICE_NAME, Config::DSD_SERVICE_DOMAIN);
        m_gaServer.start();

        sleep(3);const
        bool serverRunning = server.isRunning();
        newTRACE((LOG_LEVEL_INFO, "Server is running %d", 0, serverRunning ));
        if (serverRunning) {
            std::cout << "Press Enter key to stop" << std::endl;
            std::cin.get();
        }

        m_gaServer.stop();

        /// Stop server when Enter key is pressed
        server.stop();

        /// Stop OI
        oi.stop();

        /// Wait until all thread are completed
        oi_thread.join();

        jobThread_thread.join();
        server_thread.join();
    }
    catch (...) {
        //TRACE((LOG_LEVEL_ERROR, "Exception : terminate server!", 0))
        ret =  EXIT_FAILURE;
    }

    TRACE((LOG_LEVEL_INFO, "Run_as_console returns with code: %d", 0, ret));
    termTRACE();

    return ret;
}

int run_as_service(void)
{
//    newTRACE((LOG_LEVEL_INFO, "run_as_service(void)", 0));

	int ret = EXIT_SUCCESS;

 try
   {

        BUService service(BUService::buDaemonName);

        // No TRACE shall be invoked before this point
        initTRACE();
        newTRACE((LOG_LEVEL_INFO, "run_as_service(void)", 0));
        
        // Setting the global flag indicating that
        // BUSRV is running as AMF service
        g_pService = &service;

#ifdef _APGUSERSVC_
    // To be removed
    unsigned int cpsGrpId;
    unsigned int cpsUsrId;
    struct passwd *pw = getpwnam("apgusersvc");
    struct group* grpCPS = getgrnam("CPSUSRGRP");

    if ((pw != NULL) && (grpCPS != NULL))
    {
        cpsGrpId = grpCPS->gr_gid;
        cpsUsrId = pw->pw_uid;

        Config::setCPSUserId(cpsUsrId);

        int res = setegid( cpsGrpId );
        int res1 = seteuid( cpsUsrId );
        if ((res == 0) && (res1 == 0))
        {
            TRACE((LOG_LEVEL_INFO, "%s(), user<%d> and group:<%d>", 0, __func__, cpsUsrId, cpsGrpId));
        }
    }

#else

    // Hard Limit solution - Clear CAP_SYS_RESOURCE on root for the current process id
    cap_t cap = cap_get_proc();
    cap_value_t cap_list[1];
    cap_list[0] = CAP_SYS_RESOURCE;
    if (cap_set_flag(cap, CAP_EFFECTIVE, 1, cap_list, CAP_CLEAR) == -1)
        TRACE((LOG_LEVEL_ERROR, "cap_set_flag() Failed to clear CAP_SYS_RESOURCE for root user!", 0));
    if (cap_set_proc(cap))
        TRACE((LOG_LEVEL_ERROR, "cap_set_proc() Failed to clear CAP_SYS_RESOURCE for root user!", 0));
    if (cap_free(cap) == -1)
          TRACE((LOG_LEVEL_ERROR, "cap_free failed!", 0));
#endif


        // This will loop until receive USR2 signal
        // Or immadm -o3 cmd
        service.run();
        g_pService = NULL;
    }
    catch (...)
    {
        g_pService = NULL;
       // TRACE((LOG_LEVEL_ERROR, "Exception : Terminate service", 0));
       	ret = EXIT_FAILURE;
    }

    newTRACE((LOG_LEVEL_INFO, "run_as_service() returns with code: %d", 0, ret));
    termTRACE();

    return ret;
}

void help(char* argv)
{
    try {
        boost::regex pattern("(.*)\\/(.*)");
        boost::cmatch m;
        const char *name = boost::regex_match(argv, m, pattern) ?
                m[2].str().c_str() : argv;    
        std::cout << "Usage: " << name << " [noservice]" << std::endl;
    }
    catch (...) {
        // This is the best we can do in this case
        std::cout << "Usage: cps_busrvd [noservice]" << std::endl;
    }
}

// Perform AP initialisation such as: 
// - AP_InitProcess and 
// - AP_SetCleanupAndCrashRoutine
//
// Shall not call any function that uses TRACE

bool initialise()
{
	bool res = true;
	
	// Perform tasks required by AP; Empty for the time being		
	return res;
}

// In case the AMF service is not stable, this can run in the background
// 
// ============================================================================
int run_as_service(int)
{
    // Change for Tracing Order

    // No TRACE shall be invoked before this point
	initTRACE();
    newTRACE((LOG_LEVEL_INFO, "run_as_service(int)", 0));

    // Register signal to handle
    install_signal_handler();
    
    int ret = EXIT_SUCCESS;
    try {

#ifdef _APGUSERSVC_
        unsigned int cpsGrpId;
        unsigned int cpsUsrId;
        struct passwd *pw = getpwnam("apgusersvc");
        struct group* grpCPS = getgrnam("CPSUSRGRP");

        if ((pw != NULL) && (grpCPS != NULL))
        {
            cpsGrpId = grpCPS->gr_gid;
            cpsUsrId = pw->pw_uid;

            int res = setegid( cpsGrpId );
            int res1 = seteuid( cpsUsrId );
            if ((res == 0) && (res1 == 0))
            {
                TRACE((LOG_LEVEL_INFO, "%s(), user<%d> and group:<%d>", 0, __func__, cpsUsrId, cpsGrpId));
            }            
        }

#else

    // Clear CAP_SYS_RESOURCE on root
    cap_t cap = cap_get_proc();
    cap_value_t cap_list[1];
    cap_list[0] = CAP_SYS_RESOURCE;
    if (cap_set_flag(cap, CAP_EFFECTIVE, 1, cap_list, CAP_CLEAR) == -1)
        TRACE((LOG_LEVEL_ERROR, "cap_set_flag() Failed to clear CAP_SYS_RESOURCE for root user!", 0));
    if (cap_set_proc(cap))
        TRACE((LOG_LEVEL_ERROR, "cap_set_proc() Failed to clear CAP_SYS_RESOURCE for root user!", 0));
    if (cap_free(cap) == -1)
          TRACE((LOG_LEVEL_ERROR, "cap_free failed!", 0));
        // handle error ;

#endif

        /// Construct server and JobThread
    	BUServer  server;
        JobThread  jobThread(server);

        BUParamsOI oi;
        BUParamsOM om;

       /// Create CPReloadParam & instance in IMM then sync file to IMM
        if (!om.createAndSyncBUParamObject())
       { 
            if (om.getLastError() != BUParamsCommon::UNSUPPORTED_SYSTEM)
            {
                TRACE((LOG_LEVEL_ERROR, "Failed to create CPReload object", 0));
                // TODO: keep running?
            }
        }

        server.set(&jobThread);

        /// Start JobThread thread
        boost::thread jobThread_thread(boost::bind(&JobThread::run, &jobThread));
        jobThread.waitUntilRunning();

        /// Start BUServer thread
        boost::thread server_thread(boost::bind(&BUServer::run, &server));
        server.waitUntilRunning();
        g_pServer = &server;

        /// Start OI
        boost::thread oi_thread(boost::bind(&BUParamsOI::run, &oi));
        g_pOI = &oi;

        /// Wait until server thread is killed
        oi_thread.join();

        jobThread_thread.join();
        server_thread.join();
       
        g_pServer = NULL;
        g_pOI = NULL;
    }
    catch (...) {
        g_pServer = NULL;
        g_pOI = NULL;
        ret = EXIT_FAILURE;
    }

    TRACE((LOG_LEVEL_INFO, "run_as_service(int) returns with code: %d", 0, ret));
    termTRACE();

    return ret;
}

void catch_signal(int signal)
{
    newTRACE((LOG_LEVEL_INFO, "catch_signal(%d)", 0, signal));

    if (signal != SIGTERM)
    {
        return;
    }

    // Receive signal SIGTERM, thus stop the server & oi
    if (g_pServer)
    {
        g_pServer->stop();
    }

    if (g_pOI)
    {
        g_pOI->stop();
    }
}

void install_signal_handler()
{
    struct sigaction signalAction;

    signalAction.sa_handler = catch_signal;
    sigemptyset(&signalAction.sa_mask);
    signalAction.sa_flags = 0;

    // Register to handle SIGTERM
    sigaction(SIGTERM, &signalAction, NULL);
}
