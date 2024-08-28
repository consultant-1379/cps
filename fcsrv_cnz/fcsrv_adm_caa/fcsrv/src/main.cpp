/*
NAME
    File_name:main.cpp

Ericsson Utvecklings AB

    COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

    The Copyright to the computer program(s) herein is the property of
    Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
    copied only with the written permission from Ericsson Utvecklings AB
    or in accordance with the terms and conditions stipulated in the
    agreement/contract under which the program(s) have been supplied.

DESCRIPTION
    Class implements a debug context class used in the development of the
    reset of the code. Note that no methods are to be called directly,
    rather by use of macros so that the debug code may be conditionally
    compiled out.

DOCUMENT NO
    190 89-CAA 109 1409

AUTHOR
    2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on main.cpp of BUSRV's main.cpp


SEE ALSO
    -

Revision history
----------------
2011-11-15   xtuudoo  Create, based on main.cpp of BUSRV
2012-09-11   xngudan  Updated with service handling

*/

#include <iostream>
#include <set>
#include <boost/thread.hpp>
#include <boost/regex.hpp>
#include <algorithm>
#include <string>
#include <cstdlib>
#include <unistd.h>

// For signal handling
#include <signal.h>
#include <stddef.h>

#include "Config.h"
#include "CPS_FCSRV_Trace.h"
#include "FCService.h"

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

// Global variable
FCServer* g_pServer = NULL;
FCService* g_pService = NULL;


int main(int argc, char* argv[])
{
	// Perform initialisation as required by AP
	initialise();


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

    	// Run as AMF service
        //retVal = run_as_service();

        // In case AMF service is not stable enough to be used just
        // comment the above out and uncomment the following line
        //
        retVal = run_as_service(0xffff);
    }
    else if (argc == 2) {
    	string t(argv[1]);
    	transform(t.begin(), t.end(), t.begin(),(int (*)(int)) ::tolower);
    	if (t == NOSERVICE) {
            retVal = run_as_console();
        }
        else if (t == SERVICE) {
        	//retVal = run_as_service(0xffff);
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

    return retVal;
}

int run_as_console()
{
    // No TRACE shall be invoked before this point
    initTRACE();

    newTRACE((LOG_LEVEL_INFO, "run_as_console()", 0));

    // Register signals to handler
    install_signal_handler();

    try {
        /// Construct server and JobThread
        FCServer server;
        JobThread   jobThread(server);
        server.set(&jobThread);

        /// Start JobThread thread
        boost::thread jobThread_thread(boost::bind(&JobThread::run, &jobThread));
        jobThread.waitUntilRunning();

        /// Start FCServer thread
        boost::thread server_thread(boost::bind(&FCServer::run, &server));
    	server.waitUntilRunning();
    	g_pServer = &server;

    	sleep(3);
    	bool serverRunning = server.isRunning();
    	TRACE((LOG_LEVEL_INFO, "Server is running %d", 0, serverRunning ));
    	if (serverRunning)
    	{
    		std::cout << "Press Enter key to stop" << std::endl;
    		std::cin.get();
    	}

		// Stop server when Enter key is pressed
		server.stop();

        /// Wait until server thread is completed
        jobThread_thread.join();
        server_thread.join();
    }
    catch (...) {
        return EXIT_FAILURE;
    }

    // This is the last trace
    TRACE((LOG_LEVEL_INFO, "Exit run_as_console", 0));
    termTRACE();

    return EXIT_SUCCESS;
}

int run_as_service(void)
{
    FCService service(FCService::fcDaemonName);
    g_pService = &service;

    // No TRACE shall be invoked before this point
    initTRACE();
    newTRACE((LOG_LEVEL_INFO, "run_as_service", 0));

    try
    {
        // This will loop until receive USR2 signal
        // Or immadm -o3 cmd
        service.run();
    }
    catch (...)
    {
    	g_pService = NULL;
        return EXIT_FAILURE;
    }

    g_pService = NULL;

    // This is the last trace
    TRACE((LOG_LEVEL_INFO, "Exit cps_fcsrvd", 0));
    termTRACE();

    return EXIT_SUCCESS;
}

void help(char* argv)
{
    boost::regex pattern("(.*)\\/(.*)");
    boost::cmatch m;
    const char *name = boost::regex_match(argv, m, pattern) ?
            m[2].str().c_str() : argv;

    std::cout << "Usage: " << name << " [noservice]" << std::endl;
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
    // No TRACE shall be invoked before this point
    initTRACE();
    newTRACE((LOG_LEVEL_INFO, "run_as_service(int)", 0));

    // Register signal to handle
    install_signal_handler();

    try {
        /// Construct server and JobThread
        FCServer server;
        JobThread   jobThread(server);
        server.set(&jobThread);

        /// Start JobThread thread
        boost::thread jobThread_thread(boost::bind(&JobThread::run, &jobThread));
        jobThread.waitUntilRunning();

        /// Start FCServer thread
        boost::thread server_thread(boost::bind(&FCServer::run, &server));
    	server.waitUntilRunning();

        g_pServer = &server;

        /// Wait until server thread is killed
        jobThread_thread.join();
        server_thread.join();

        g_pServer = NULL;
    }
    catch (...) {
        return EXIT_FAILURE;
    }

    TRACE((LOG_LEVEL_INFO, "Exit run_as_service(int)", 0));

    termTRACE();

    return EXIT_SUCCESS;
}

void catch_signal(int signal)
{
    newTRACE((LOG_LEVEL_INFO, "catch_signal(%d)", 0, signal));

    // Receive signal SIGTERM, thus stop the server
    if (signal == SIGTERM && g_pServer)
    {
    	if (g_pServer)
    	{
    		g_pServer->stop();
    	}
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
