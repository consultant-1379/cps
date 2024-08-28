/*
 * TCPServer.cpp
 *
 *  Created on: Aug 25, 2010
 *      Author: xquydao
 *              xdtthng 
 *      Modified based on boost::asio example
 */
#include <algorithm>
#include "boost/bind.hpp"
#include <boost/thread/thread.hpp>

#ifdef _APGUSERSVC_
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>
#endif


#include "Config.h"
#include "TCPServer.h"
#include "CPS_BUSRV_Trace.h"
//#include "CPS_Utils.h"
#include "CriticalSection.h"

#include "CodeException.h"
#include "BUService.h"

extern BUService* g_pService;
extern BUServer* g_pServer;
//static boost::posix_time::milliseconds sleepTime = boost::posix_time::milliseconds(2000);

CriticalSection& TCPServer::CS_TCPServer() const
{
	// This is to ensure that s_cs is constructed exactly once on first use
	static CriticalSection s_cs;
	return s_cs;
}

void TCPServer::fatalError(u_int32 code)
{
	newTRACE((LOG_LEVEL_INFO, "TCPServer::fatalError(%u)", 0, code));
	AutoCS a(CS_TCPServer());
	m_fatal_error = code;
}

u_int32 TCPServer::fatalError() const
{
	AutoCS a(CS_TCPServer());
	return m_fatal_error;
}

TCPServer::TCPServer() : port_(Config::instance().busrvPort()), m_running(false), m_fatal_error(0)
{
    //newTRACE((LOG_LEVEL_INFO, "TCPServer::TCPServer()", 0));
}

TCPServer::~TCPServer(void)
{
    //newTRACE((LOG_LEVEL_INFO, "TCPServer::~TCPServer()", 0));
}

bool TCPServer::isRunning()
{
    newTRACE((LOG_LEVEL_INFO, "TCPServer::isRunning(void)", 0));

    return m_running;
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

    boost::asio::ip::address addr;
    TCPAcceptor_ptr acceptor_ptr;

    try { 
        // This loop returns if successfully bound at least an IP address  
        for (int i = 0; i < 2; ++i ) {
        	TRACE((LOG_LEVEL_INFO, "TCPServer attempts binding IP address <%s>", 0, IP_LISTENING_ADDRESS[i]));
            addr = boost::asio::ip::address::from_string(IP_LISTENING_ADDRESS[i]);
            acceptor_ptr.reset(new TCPAcceptor(boost::asio::ip::tcp::endpoint(addr, port_), *this, io_service_));
            if (acceptor_ptr->bind()) {
                acceptor_list_.insert(acceptor_ptr);
                Config::instance().simAp(Config::APG_ONE_IP_ADDRESS);
                TRACE((LOG_LEVEL_INFO, "Bind successful on address: %s ", 0, IP_LISTENING_ADDRESS[i]));
                addr = boost::asio::ip::address::from_string(IP_LISTENING_ADDRESS[i + 2]);
                acceptor_ptr.reset(new TCPAcceptor(boost::asio::ip::tcp::endpoint(addr, port_), *this, io_service_));
                
                if (acceptor_ptr->bind()) {
                    Config::instance().simAp(Config::APG_TWO_IP_ADDRESS);
                    TRACE((LOG_LEVEL_INFO, "Bind successful on address: %s", 0, IP_LISTENING_ADDRESS[i+2]));
                    acceptor_list_.insert(acceptor_ptr);    // Both IP addresses are bound 
                }
                return true; // Could be both IP addresses or one IP address are bound
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
              
		// None of the IP_LISTENING_ADDRESS can be bound, try to bind all ip adress
        // For SimAP, none of IP_LISTENING_ADDRESS is defined.
        //
	    TRACE((LOG_LEVEL_INFO, "TCPServer::init attemps binding on all IP addresses used in SimAP", 0));
        addr = boost::asio::ip::address_v4::any();
        acceptor_ptr.reset(new TCPAcceptor(boost::asio::ip::tcp::endpoint(addr, port_), *this, io_service_));

        if (acceptor_ptr->bind()) {
            Config::instance().simAp(Config::SIMAP_ALL_IP_ADDRESS);
            acceptor_list_.insert(acceptor_ptr);
			TRACE((LOG_LEVEL_INFO, "Bind successful on all address used in SimAP", 0));
            return true;	// All IP addresses are bound
        }
        else {
        	TRACE((LOG_LEVEL_WARN, "TCPServer::init; unable to bind on all IP addresses used in SimAP", 0));
        }
                
        THROW_XCODE("TCP init failed", CodeException::TCP_INIT_FAILED);
    }
    catch (CodeException& e) {
        EventReporter::instance().write(e.what().c_str());
        TRACE((LOG_LEVEL_ERROR, "%s", 0, e.what().c_str()));
    }
    catch (boost::system::system_error &e) {
        boost::system::error_code ec = e.code();
        std::ostringstream ss;
        ss << "TCPServer::init() failed. Boost Error code: " << ec.value() << " - " << ec.message().c_str();
        TRACE((LOG_LEVEL_ERROR, "%s", 0, ss.str().c_str()));
        EventReporter::instance().write(ss.str().c_str()); 
    }
    catch (...) {
        EventReporter::instance().write("TCPServer::init(); Unknown exception");
        TRACE((LOG_LEVEL_ERROR, "TCPServer::init(); Unknown exception", 0));
    }
    TRACE((LOG_LEVEL_WARN, "TCPServer::init() returns false", 0));
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


#ifdef _APGUSERSVC_
        // Get current effective user Id
        uid_t currentEffectiveId = geteuid();

        // Set effective user as root Id
        if( seteuid(0) != 0 )
        {
            TRACE((LOG_LEVEL_ERROR, "Failed to switch to root user! Try again.", 0));
            if( seteuid(0) != 0 )
            {
                TRACE((LOG_LEVEL_ERROR, "Failed to switch to root user - error <%d>", 0, errno));
            }
        }
#endif

 	// init the server in 3 times
 		int retries = 0;
        TRACE((LOG_LEVEL_INFO, "Attempt to bind IP address; initial running flag is %d", 0, m_running));
    	while (!this->init()) {
        	TRACE((LOG_LEVEL_WARN, "Failed to initialize the server: %d times", 0, ++retries));

			// Do not retry if run as console 
        	if ((g_pService || g_pServer) && retries < 3) {
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

       		TRACE((LOG_LEVEL_WARN, "Attempt to bind IP address failed; running flag is %d", 0, m_running));
         	return;
    	}
       	TRACE((LOG_LEVEL_INFO, "Attempt to bind IP address succeeded; running flag is %d", 0, m_running));

#ifdef _APGUSERSVC_
        // Restore previous effective user Id (apgusesvc)
        if( seteuid(currentEffectiveId) != 0 )
        {
            TRACE((LOG_LEVEL_ERROR, "Failed to switch back to apg user! Try again.", 0));
            if( seteuid(currentEffectiveId) != 0 )
            {
                TRACE((LOG_LEVEL_ERROR, "Failed to switch back to apg user - error <%d>", 0, errno));
            }
        }
#endif

        // Start acceptor to listen to client connections
        for_each(acceptor_list_.begin(), acceptor_list_.end(), boost::bind(&TCPAcceptor::start, _1));

        // The io_service::run() call will block until all asynchronous operations
        // have finished. While the server is running, there is always at least one
        // asynchronous operation outstanding: the asynchronous accept call waiting
        // for new incoming connections.
        TRACE((LOG_LEVEL_INFO, "TCPServer::run() about to call io_service_.run()", 0));
        io_service_.run();
    	TRACE((LOG_LEVEL_INFO, "TCPServer::run() exits returning from io_service_.run() without error", 0));
    	return;
    }
    catch (boost::system::system_error& e)
    {
        boost::system::error_code ec = e.code();
        TRACE((LOG_LEVEL_ERROR, "TCPServer::run() failed. Boost Error code: %d - %s", 0, ec.value(), ec.message().c_str()));
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
    TRACE((LOG_LEVEL_WARN, "TCPServer::run() returns with some error conditions", 0));
}

void TCPServer::stop(void)
{
    newTRACE((LOG_LEVEL_INFO, "TCPServer::stop()", 0));

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
