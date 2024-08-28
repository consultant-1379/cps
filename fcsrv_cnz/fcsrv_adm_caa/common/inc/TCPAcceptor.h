/*
NAME
	TCPAcceptor -
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


#ifndef TCP_ACCEPTOR_H_
#define TCP_ACCEPTOR_H_

#include "boost/asio.hpp"
#include "boost/noncopyable.hpp"
#include "boost/enable_shared_from_this.hpp"

#include "TCPConnection.h"

/// Fwd declaration
class TCPServer;

class TCPAcceptor : public boost::enable_shared_from_this<TCPAcceptor>,
                     private boost::noncopyable
{
public:
    /// Construct a TCPAcceptor
    explicit TCPAcceptor(boost::asio::ip::tcp::endpoint endpoint, TCPServer& server, boost::asio::io_service& io_service);
    virtual ~TCPAcceptor(void);

    /// Start the acceptor
    void start(void);

    /// Stop the acceptor
    void stop(void);
    
    /// Bind the endpoint
    bool bind();

private:
    /// Handle completion of an asynchronous accept operation.
    void handle_accept(const boost::system::error_code& e);

    /// Address and port to listen
    boost::asio::ip::tcp::endpoint endpoint_;

    /// tcp server reference
    TCPServer& server_;

    /// acceptor to listen then accept connections
    boost::asio::ip::tcp::acceptor acceptor_;

    /// Current accepting connection
    TCPConnection_ptr current_connection_ptr_;

};

typedef boost::shared_ptr<TCPAcceptor> TCPAcceptor_ptr;

#endif /* TCP_ACCEPTOR_H_ */
