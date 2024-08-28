/*
NAME
    File_name:CPS_FTP_Main.cpp

Ericsson Utvecklings AB

    COPYRIGHT Ericsson Utvecklings AB, Sweden 1999, 2011. All rights reserved.

    The Copyright to the computer program(s) herein is the property of
    Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
    copied only with the written permission from Ericsson Utvecklings AB
    or in accordance with the terms and conditions stipulated in the
    agreement/contract under which the program(s) have been supplied.

DESCRIPTION
    Entry point of the program

DOCUMENT NO
    190 89-CAA 109 1415

AUTHOR    
    2011-14-07 by XDT/DEK/XQUYDAO

SEE ALSO
    -

Revision history
----------------
2011-14-07 xquydao Updated for APG43L

*/

#include "CPS_FTP_Server.h"
#include "CPS_FTP_Service.h"
#include "CPS_FTP_Trace.h"

#include <string>
#include <iostream>

#include "boost/regex.hpp"

// For signal handling
#include <signal.h>
#include <stddef.h>

// For getservbyname
#include <netdb.h>
#include <netinet/in.h>


using namespace std;

const string AMF = "amf";
const string SERVER_ADDRESS[MAX_LISTENER] = {"192.168.169.1", "192.168.170.1",
                                             "192.168.169.2", "192.168.170.2", "127.0.0.1"};
void run_as_no_service();
void run_as_amf_service();
void help(char* argv);

// For signal handling
void catch_signal();
void install_signal_handler();
CPS_FTP_Server* g_pServer = NULL;
CPS_FTP_Service* g_pService = NULL;

// Obtain the buftpd port
int obtain_port();

int main(int argc, char* argv[])
{
    int retVal = EXIT_SUCCESS;

    if (argc == 1)
    {
        run_as_no_service();
    }
    else if (argc == 2 && (string(argv[1]) == AMF))
    {
        run_as_amf_service();
    }
    else
    {
        help(argv[0]);
        retVal = EXIT_FAILURE;
    }

    return retVal;
}

void catch_signal(int signal)
{
    newTRACE((LOG_LEVEL_INFO, "catch_signal(%d)", 0, signal));

    if (g_pServer == NULL)
    {
        return;
    }

    // Receive signal SIGTERM, thus stop the service
    if (signal == SIGTERM)
    {
        g_pServer->stop();
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

void run_as_no_service()
{
    initTRACE();	
    newTRACE((LOG_LEVEL_INFO, "run_as_no_service()", 0));

    // Register signal to handle
    install_signal_handler();

    CPS_FTP_Server server(SERVER_ADDRESS, obtain_port());
    g_pServer = &server;

    // start server
    server.run();

    g_pServer = NULL;

    // No tracing shall be done after main() returns.
    // This is the last trace
    TRACE((LOG_LEVEL_INFO, "run_as_no_service() returns", 0));
    termTRACE();
}

void run_as_amf_service()
{
    CPS_FTP_Server server(SERVER_ADDRESS, obtain_port());
    CPS_FTP_Service service(CPS_FTP_Service::BUFTP_DEF_DAEMON_NAME, server);
    g_pService = &service;

    initTRACE();
    newTRACE((LOG_LEVEL_INFO, "run_as_amf_service()", 0));

    // This will run into the infinite loop until receive USR2 signal
    // Or immadm -o3 cmd
    service.run();

    g_pService = NULL;

    // No tracing shall be done after main() returns.
    // This is the last trace
    TRACE((LOG_LEVEL_INFO, "run_as_amf_service() returns", 0));
    termTRACE();
}

void help(char* argv)
{
    boost::regex pattern("(.*)\\/(.*)");
    boost::cmatch m;
    try
    {
        const char *name = boost::regex_match(argv, m, pattern) ?
                m[2].str().c_str() : argv;
        cout << "Usage: " << name << " [amf]" << endl;
    }
    catch (...)
    {
        cout << "Usage: cps_buftpd [amf]" << endl;
    }
}


int obtain_port()
{
    //newTRACE((LOG_LEVEL_INFO, "obtain_port()", 0));

    int port;
    struct servent *serv = NULL;
    serv = getservbyname("buftpd", "tcp");

    if (serv == NULL)
    {
    //    TRACE((LOG_LEVEL_ERROR, "Failed to obtain the buftpd port. Use default: %d", 0, CPS_FTP_Server::DEFAULT_PORT));
        port = CPS_FTP_Server::DEFAULT_PORT;
    }
    else
    {
        port = ntohs(serv->s_port);
    }

    return port;
}
