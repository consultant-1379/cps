/*
NAME
	TCPConnection.h -
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


#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include "boost/asio.hpp"
#include "boost/array.hpp"
#include "boost/noncopyable.hpp"
#include "boost/enable_shared_from_this.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/shared_array.hpp"

/// Fwd declaration
class TCPServer;

/// Constant
const std::size_t DEFAULT_BUFFER_SIZE = 1024;

class TCPConnection : public boost::enable_shared_from_this<TCPConnection>,
                       private boost::noncopyable
{
public:
    /// Construct a connection with the given io_service.
    explicit TCPConnection(TCPServer& server, boost::asio::io_service& io_service, 
                            const std::size_t& buffer_size = DEFAULT_BUFFER_SIZE);
    ~TCPConnection(void);

    /// Get the socket.
    boost::asio::ip::tcp::socket& socket() {return socket_;};

    /// Send data
    void send(const char* data, const std::size_t& num);

    /// Start the first read asynchronous operation.
    void start(void);

    /// Stop all asynchronous operations.
    void stop(void);
    
    // Retrieve TCP Server 
    TCPServer& getTCPServer() const { return server_; }

private:
    /// Handle completion of a read operation.
    void handle_read(const boost::system::error_code& e,
                     std::size_t bytes_transferred);

    /// Handle completion of a write operation.
    void handle_write(const boost::system::error_code& e,
                      std::size_t bytes_transferred);

    /// Socket for the connection.
    boost::asio::ip::tcp::socket socket_;    
  
    /// Buffer for incoming data.
    std::size_t buffer_size_;
    boost::shared_array<char> buffer_ptr_;

    /// TCPServer
    TCPServer& server_;
};

typedef boost::shared_ptr<TCPConnection> TCPConnection_ptr;

#endif
