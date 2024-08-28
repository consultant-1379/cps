
/*
NAME
	SYSTEMINFO -
LIBRARY 3C++
PAGENAME SYSTEMINFO
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION
   -.

ERROR HANDLING
   -

DOCUMENT NO
	190 89-CAA 109 0582

AUTHOR
   2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on boost::asio examples

SEE ALSO

Revision history
----------------


*/


#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "boost/asio.hpp"
#include "boost/noncopyable.hpp"
#include <set>
#include <string>

#include "TCPConnection.h"
#include "TCPAcceptor.h"

class TCPServer : private boost::noncopyable
{
public:

    TCPServer();
    virtual ~TCPServer(void);

    /// start the acceptor and run io_service loop (demultiplex event).
    virtual void run(void);

    /// Stop the server
    virtual void stop(void);

    /// Incoming Data Handler
    virtual void incoming_data_handler(TCPConnection_ptr connection, const char* data, std::size_t num) = 0;

    /// Incoming Connection Handler
    void incoming_connection_handler(TCPConnection_ptr connection);

    /// Connection closed handler
    void handle_close(TCPConnection_ptr connection);    

    /// Send data on connection
    void send(const TCPConnection_ptr connection, const char* data, const std::size_t& num);

    // Check if TCPServer is running
    bool isRunning();

protected:
    /// initialize
    bool init(void);

    /// Stop request handler
    void handle_stop(void);

    /// The io_service used to perform asynchronous operations.
    boost::asio::io_service io_service_;

    /// Listening port number
    unsigned short port_;

    // Indicate if TCPServer is running
    bool m_running;

    /// Acceptors used to listen for incoming connections.
    std::set<TCPAcceptor_ptr> acceptor_list_;

    /// List of connected Socket
    std::set<TCPConnection_ptr> connection_list_;

    /// Listening addresses
    std::set<std::string> addresses_;
};

#endif

