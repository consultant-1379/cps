/*
NAME
  File_name: TCPAcceptor.cpp

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

*/

#include <sstream>
#include <iostream>
#include "boost/bind.hpp"
#include "TCPAcceptor.h"
#include "TCPServer.h"
#include "CPS_FCSRV_Trace.h"
#include "EventReporter.h"

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
        }
        return;
    }

    /// Notify server incoming connection
    server_.incoming_connection_handler(current_connection_ptr_);

    /// Start this connection after being accepted
    current_connection_ptr_->start();

    /// Wait to accept another new connection
    current_connection_ptr_.reset(new TCPConnection(server_, acceptor_.get_io_service()));
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
