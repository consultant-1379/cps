/*
NAME
  File_name:TCPServer.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

	The Copyright to the computer program(s) herein is the property of Ericsson
	Utvecklings AB, Sweden.
	The program(s) may be used and/or copied only with the written permission from
	Ericsson Utvecklings AB or in accordance with the terms and conditions
	stipulated in the agreement/contract under which the program(s) have been
	supplied.

DESCRIPTION
    global (static member) utility function for extrating system info.

DOCUMENT NO
   190 89-CAA 109 0583

AUTHOR
   2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on boost::asio examples

SEE ALSO
	-

Revision history
----------------
2011-11-15 xtuudoo Created
2012-09-20 xngudan Implemented Component Error Report
2012-11-01 xngudan Used DSD to fetch the listening IP addresses
*/

#include <algorithm>
#include <time.h>
#include "boost/bind.hpp"
#include "boost/thread/thread.hpp"

#include "Config.h"
#include "TCPServer.h"
#include "CPS_FCSRV_Trace.h"
#include "CPS_Parameters.h"
#include "CPS_Utils.h"
#include "CodeException.h"
#include "FCService.h"


using namespace std;

// Forward declaration
extern FCService* g_pService;

TCPServer::TCPServer() : port_(Config::instance().fcsrvPort()), m_running(false)
{
    //newTRACE((LOG_LEVEL_INFO, "TCPServer::TCPServer()", 0));
}

TCPServer::~TCPServer(void)
{
    //newTRACE((LOG_LEVEL_INFO, "TCPServer::~TCPServer()", 0));
}

bool TCPServer::init(void)
{
    newTRACE((LOG_LEVEL_INFO, "TCPServer::init()", 0));
    
    acceptor_list_.clear();
    connection_list_.clear();
    addresses_.clear();
    io_service_.reset();


    static const char* IP_LISTENING_ADDRESS[4] = {
        "192.168.169.1",
        "192.168.169.2",
        "192.168.170.1",
        "192.168.170.2"
    };

    //vector<string> IP_LISTENING_ADDRESS;

    boost::asio::ip::address addr;
    TCPAcceptor_ptr acceptor_ptr;

    try { 

#if 0
    	// Get the IP Addresses from DSD
    	if (!Config::instance().getIPListeningAddr(IP_LISTENING_ADDRESS))
    	{
    		THROW_XCODE("DSD cannot get Listening IP Address on the current node",
    									CodeException::DSD_GET_IPADDR_FAILED);
    	}
#endif

        // This loop returns if successfully bound at least an IP address  
        for (int i = 0; i < 2; ++i ) {
            addr = boost::asio::ip::address::from_string(IP_LISTENING_ADDRESS[i]);
            acceptor_ptr.reset(new TCPAcceptor(boost::asio::ip::tcp::endpoint(addr, port_), *this, io_service_));
            if (acceptor_ptr->bind()) {
                acceptor_list_.insert(acceptor_ptr);
                //acceptor_ptr->start();
                Config::instance().simAp(Config::APG_ONE_IP_ADDRESS);
                TRACE((LOG_LEVEL_INFO, "Bind successful on address: %s ", 0, IP_LISTENING_ADDRESS[i]));
                
                addr = boost::asio::ip::address::from_string(IP_LISTENING_ADDRESS[i + 2]);
                acceptor_ptr.reset(new TCPAcceptor(boost::asio::ip::tcp::endpoint(addr, port_), *this, io_service_));
                
                if (acceptor_ptr->bind()) {
                    Config::instance().simAp(Config::APG_TWO_IP_ADDRESS);
                    TRACE((LOG_LEVEL_INFO, "Bind successful on address: %s", 0, IP_LISTENING_ADDRESS[i+2]));
                    acceptor_list_.insert(acceptor_ptr);    // Both IP addresses are bound
                }
                return true;
            }
            else {
                addr = boost::asio::ip::address::from_string(IP_LISTENING_ADDRESS[i + 2]);
                acceptor_ptr.reset(new TCPAcceptor(boost::asio::ip::tcp::endpoint(addr, port_), *this, io_service_));
                
                if (acceptor_ptr->bind()) {
                    Config::instance().simAp(Config::APG_ONE_IP_ADDRESS);
                    acceptor_list_.insert(acceptor_ptr);    // One IP address is bound
                    TRACE((LOG_LEVEL_INFO, "Bind successful on address: %s", 0, IP_LISTENING_ADDRESS[i+2]));
                    return true;
                }
             }
             // No IP address has been bound successfully at this point
        } // for

        // If none of the IP_LISTENING_ADDRESS can be bound we try to bind all ip adress
        // When we use SimAP we also bind all ip addresses because of the non existing IP_LISTENING_ADDRESS on a SimAP.
        addr = boost::asio::ip::address_v4::any();
        acceptor_ptr.reset(new TCPAcceptor(boost::asio::ip::tcp::endpoint(addr, port_), *this, io_service_));
        
        if (acceptor_ptr->bind()) {
            Config::instance().simAp(Config::SIMAP_ALL_IP_ADDRESS);
            acceptor_list_.insert(acceptor_ptr);
            //acceptor_ptr->start();
            return true;
        }
        else {
        	TRACE((LOG_LEVEL_ERROR, "TCPServer::init; unable to bind on all IP addresses used in SimAP", 0));
        }
        
        THROW_XCODE("TCP init failed", CodeException::TCP_INIT_FAILED);
    }
    catch (CodeException& e) {
        EventReporter::instance().write(e.what());
        TRACE((LOG_LEVEL_ERROR, "%s", 0, e.what().c_str()));
    }
    catch (boost::system::system_error &e) {
        boost::system::error_code ec = e.code();
        std::ostringstream ss;
        ss << "TCPServer::init() failed. Boost Error code: " << ec.value() << " - " << ec.message().c_str();
        TRACE((LOG_LEVEL_ERROR, "%s", 0, ss.str().c_str()));
        EventReporter::instance().write(ss.str().c_str());;
    }
    catch (...) {
        EventReporter::instance().write("Unknown exception in TCP initialization.");
        TRACE((LOG_LEVEL_ERROR, "TCPServer::init(); Unknown exception", 0));
    }

    TRACE((LOG_LEVEL_INFO, "TCPServer::init() returns false", 0));
    return false;
}

void TCPServer::run(void)
{
    newTRACE((LOG_LEVEL_INFO, "TCPServer::run()", 0));

    try
    {
        if (isTRACEABLE()) {
            std::ostringstream ss;
            ss << "TCPServer::run() thread id "<< boost::this_thread::get_id();
            TRACE((LOG_LEVEL_INFO, "%s", 0, ss.str().c_str()));
        }
    	
        // Initialize the server in 3 times
        int retries = 0;
        TRACE((LOG_LEVEL_INFO, "Attempt to bind IP address; initial running flag is %d", 0, m_running));
        while (!this->init())
        {
            TRACE((LOG_LEVEL_WARN, "Failed to initialize the server: %d times", 0, ++retries));

            // Do not retry if run as console
            if (g_pService && retries < 3) {
                //boost::this_thread::sleep(sleepTime);
                struct timespec req;
                req.tv_sec = 2UL;    // Sleeps for 2 seconds
                req.tv_nsec = 0L;
                nanosleep(&req, NULL);
                continue;
            }

            // Try 3 times but failed, exit the loop and report the error
            // Report error to AMF if it's running as service
            m_running = false;
            if (g_pService) {
                TRACE((LOG_LEVEL_FATAL, "Reporting component error to AMF", 0));
                g_pService->componentReportError(ACS_APGCC_COMPONENT_RESTART);
                EventReporter::instance().write("Reporting component error to AMF");
            }

            TRACE((LOG_LEVEL_ERROR, "Attempt to bind IP address failed; running flag is %d", 0, m_running));
            return;
        }
        TRACE((LOG_LEVEL_INFO, "Attempt to bind IP address succeeded; running flag is %d", 0, m_running));

        // Start acceptor to listen to client connections
        for_each(acceptor_list_.begin(), acceptor_list_.end(), boost::bind(&TCPAcceptor::start, _1));

        // The io_service::run() call will block until all asynchronous operations
        // have finished. While the server is running, there is always at least one
        // asynchronous operation outstanding: the asynchronous accept call waiting
        // for new incoming connections.
        io_service_.run();
        TRACE((LOG_LEVEL_INFO, "TCPServer::run() exits returning from io_service_.run() without error", 0));
        return;
    }
    catch (boost::system::system_error& e)
    {
        boost::system::error_code ec = e.code();
        TRACE((LOG_LEVEL_ERROR, "TCPServer::run() failed. Error code: %d - %s", 0, ec.value(), ec.message().c_str()));
        /// stop the server to clean up
        this->handle_stop();
    }
    catch (CodeException& e) {
        TRACE((LOG_LEVEL_ERROR, "TCPServer::run() failed - %s", 0, e.what().c_str()));
        /// stop the server to clean up
        this->handle_stop();
    }

    if (g_pService) {
        TRACE((LOG_LEVEL_FATAL, "Reporting component error to AMF", 0));
        g_pService->componentReportError(ACS_APGCC_COMPONENT_RESTART);
        EventReporter::instance().write("Reporting component error to AMF");
    }
    TRACE((LOG_LEVEL_INFO, "TCPServer::run() returns with some error conditions", 0));
}

void TCPServer::stop(void)
{
    // Post stop request to io_server
    // in order to shut down server gracefully
    // Note: this function can be called at any threads.
    io_service_.post(boost::bind(&TCPServer::handle_stop, this));    
}

void TCPServer::handle_stop(void)
{
    newTRACE((LOG_LEVEL_INFO, "TCPServer::handle_stop()", 0));
    /// Close the acceptors
    std::set<TCPAcceptor_ptr>::iterator accIt = acceptor_list_.begin();

    for_each(accIt, acceptor_list_.end(), boost::bind(&TCPAcceptor::stop, _1));
    acceptor_list_.clear();

    // Close all the connections
    // The connection will be automatically remove from the connection_list
    std::set<TCPConnection_ptr>::iterator connIt = connection_list_.begin();
    
    TRACE((LOG_LEVEL_INFO, "Size of connection list is %d", 0, connection_list_.size()));
    while (connIt != connection_list_.end())
        (*connIt++)->stop();
    
    // There should be no handles queueing for io_service at this point
    // Attempt to reset io_service internal state, preparing for the next run
    TRACE((LOG_LEVEL_INFO, "TCPServer::handle_stop(), about to call io_service.reset()", 0));
    io_service_.reset();
    TRACE((LOG_LEVEL_INFO, "TCPServer::handle_stop(), io_service.reset() returns", 0));
    TRACE((LOG_LEVEL_INFO, "TCPServer::handle_stop(), Server successfully stopped", 0));
}

void TCPServer::incoming_data_handler(TCPConnection_ptr, const char*, std::size_t)
{
    /// Do nothing, the real service is handled by subclass
}

void TCPServer::incoming_connection_handler(TCPConnection_ptr connection)
{
    /// Add the connection to the list
    connection_list_.insert(connection);
}

void TCPServer::handle_close(TCPConnection_ptr connection)
{
    /// Connection is closed, remove it from the list
    connection_list_.erase(connection);
}

void TCPServer::send(const TCPConnection_ptr connection, const char* data, const std::size_t& num)
{
    connection->send(data, num);
    
    // Perform house keeping if needed on this connection
}
