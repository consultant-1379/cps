/*
 * TCPAcceptor.cpp
 *
 *  Created on: Sep 15, 2010
 *      Author: xquydao
 *              xdtthng
 */
#include <sstream>
#include <iostream>
#include "boost/bind.hpp"
#include "TCPAcceptor.h"
#include "TCPServer.h"
#include "CPS_BUSRV_Trace.h"
#include "EventReporter.h"
#include "BUService.h"


extern BUService* g_pService;

TCPAcceptor::TCPAcceptor(boost::asio::ip::tcp::endpoint endpoint, TCPServer& server, boost::asio::io_service& io_service) :
    endpoint_(endpoint),
    server_(server),
    acceptor_(io_service)
{

    newTRACE((LOG_LEVEL_INFO, "TCPAcceptor::TCPAcceptor()", 0));
    
}

TCPAcceptor::~TCPAcceptor()
{
    newTRACE((LOG_LEVEL_INFO, "TCPAcceptor::~TCPAcceptor()", 0));
    // TODO Auto-generated destructor stub
}

bool TCPAcceptor::bind(void)
{
    newTRACE((LOG_LEVEL_INFO, "TCPAcceptor::bind(void)", 0));
	try {
	    // Setting the SOL_SOCKET/SO_REUSEADDR option
	    acceptor_.open(endpoint_.protocol());
	    acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
	    acceptor_.bind(endpoint_);
	    return true;
    }
    catch (boost::system::system_error &e) {
        boost::system::error_code ec = e.code();
        std::ostringstream ss;
        ss << "TCPAcceptor::bind() failed. Boost Error code: " << ec.value() << " - " << ec.message().c_str();
        TRACE((LOG_LEVEL_ERROR, "%s", 0, ss.str().c_str()));
        EventReporter::instance().write(ss.str().c_str()); 
    }
    catch (...) {
        EventReporter::instance().write("TCPServer::init(); Unknown exception");
        TRACE((LOG_LEVEL_ERROR, "TCPServer::init(); Unknown exception", 0));
    }
    return false;
}

void TCPAcceptor::start(void)
{
    newTRACE((LOG_LEVEL_INFO, "TCPAcceptor::start()", 0));
    acceptor_.listen();

    current_connection_ptr_.reset(new TCPConnection(server_, acceptor_.get_io_service()));
    acceptor_.async_accept(current_connection_ptr_->socket(),
                           boost::bind(&TCPAcceptor::handle_accept, shared_from_this(),
                           boost::asio::placeholders::error));

    if (isTRACEABLE()) {
        std::stringstream ss;
        ss << "Server started at " << acceptor_.local_endpoint();
        TRACE((LOG_LEVEL_INFO, "%s", 0, ss.str().c_str()));
    }
}

void TCPAcceptor::handle_accept(const boost::system::error_code& e)
{
    newTRACE((LOG_LEVEL_INFO, "TCPAcceptor::handle_accept(const boost::system::error_code&)", 0));
    if (e) {
        if (e != boost::asio::error::operation_aborted) {
            if (isTRACEABLE()) {
                std::stringstream ss;
                ss << "Accept failed. Error code : "<< e.value() << " - " << e.message() << std::endl;
                TRACE((LOG_LEVEL_ERROR, "%s", 0, ss.str().c_str()));
            }

            if (e.value() == EMFILE) {
            	if (g_pService) {
     				ACS_APGCC_ReturnType res = g_pService->componentReportError(ACS_APGCC_COMPONENT_RESTART);
    	    		TRACE((LOG_LEVEL_FATAL, "Reporting component error to AMF returns <%d>", 0, res));
    	    		if (res != ACS_APGCC_SUCCESS) {
    	    			TRACE((LOG_LEVEL_FATAL, "Failed to report component error to AMF; set fatal error flag", 0));
    	    			server_.fatalError(1);
    	    		}
            	}
            }
        }
		// Give it a go
		EventReporter::instance().write("TCPAcceptor::handle_accept() returns on system error");
        TRACE((LOG_LEVEL_WARN, "TCPAcceptor::handle_accept() returns on system error", 0));
        return;
   }

    /// Notify server incoming connection
    server_.incoming_connection_handler(current_connection_ptr_);

    /// Start this connection after being accepted
    current_connection_ptr_->start();

    /// Wait to accept another new connection
    current_connection_ptr_.reset(new TCPConnection(server_, acceptor_.get_io_service()));

    // Clear previously reported fatal error
    if (server_.fatalError()) {
    	if (g_pService->componentClearError() == ACS_APGCC_SUCCESS) {
    		server_.fatalError(0);
    	}
    }
    acceptor_.async_accept(current_connection_ptr_->socket(),
                           boost::bind(&TCPAcceptor::handle_accept, shared_from_this(),
                           boost::asio::placeholders::error));
}

void TCPAcceptor::stop(void)
{
    newTRACE((LOG_LEVEL_INFO, "TCPAcceptor::stop(void)", 0));
    if (acceptor_.is_open()) {
        if (acceptor_.local_endpoint().port() > 0) {
            // In case port is 0 that means acceptor is open but couldn't bind
            // Therefore no need to print "acceptor is closed" for that case
            if (isTRACEABLE()) {
                std::stringstream ss;
                ss << "Server " << acceptor_.local_endpoint() << " is closed.";
                TRACE((LOG_LEVEL_INFO, "%s", 0, ss.str().c_str()));
            }
        }
        acceptor_.close();
    }
}
