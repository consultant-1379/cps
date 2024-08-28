/*
NAME
    File_name:CPS_FTP_Server.h

Ericsson Utvecklings AB

    COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

    The Copyright to the computer program(s) herein is the property of
    Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
    copied only with the written permission from Ericsson Utvecklings AB
    or in accordance with the terms and conditions stipulated in the
    agreement/contract under which the program(s) have been supplied.

DESCRIPTION
    Class implements the server functionality of BUFTPD.

DOCUMENT NO
    190 89-CAA 109 1415

AUTHOR
    2011-14-07 by XDT/DEK/XQUYDAO


SEE ALSO
    -

Revision history
----------------
2011-14-07 xquydao Created

*/
#ifndef CPS_FTP_SERVER_H_
#define CPS_FTP_SERVER_H_


#include "CPS_FTP_Connection.h"
#include "CPS_FTP_Event_FD.h"

#include <string>
#include <map>
#include <set>
#include <sys/types.h>

#include "boost/thread/mutex.hpp"
#include "boost/thread/condition_variable.hpp"

#define MAX_LISTENER 5

class CPS_FTP_Server
{
public:
    static const unsigned int DEFAULT_PORT;

public:
    CPS_FTP_Server(const std::string addresses[MAX_LISTENER], unsigned int port = CPS_FTP_Server::DEFAULT_PORT);
    virtual ~CPS_FTP_Server();

    /**
     * Run the FTP server
     *
     * Initialize the listener (maximum 2)
     * Run the 'select' loop or event loop in order to handle
     * - Accept connection event
     * - Dispatching incoming data  to corresponding connection obj
     * - Stop server event
     * - Data transfer event
     *
     * @param  N/A
     * @return N/A
     */
    void run();

    /**
     * Stop the server
     *
     * @param  N/A
     * @return N/A
     */
    void stop();

    /**
     * Handler of the connection state change
     *
     * @param[in] connection  The state changed connection
     * @param[in] event       The notification event
     * @return N/A
     */
    void OnConnectionChange(CPS_FTP_Connection_Ptr connection, e_notification event);

    /**
     * This function is used to block the calling thread
     * Until FTP Server finishes start up.
     *
     * @param N/A
     * @return N/A
     */
    void waitUntilRunning(void);

    /**
     * Report the running status of the server
     *
     * @param N/A
     * @return true  BUFTP server is running
     *         false BUFTP server is NOT running
     */
    bool isRunning(void);

private:
    /**
     * Initialize the server
     *
     * Open socket then listen at given addr:port in the constructor.
     * It will listen at 2 addr:port at maximum
     *
     * @param N/A
     * @return result of the initialization
     *         false Non of the address:port can be listened
     *         true  At least 1 address:port is listened successfully
     */
    bool init();

    /**
     * Stop the server
     *
     * Close all the listener & connections
     *
     * @param N/A
     * @return N/A
     */
    void handleStop();

    /**
     * Accept a connection from the given listener
     *
     * @param[in] listenerSocket  The listener (representing addr:port) of the incoming connection
     * @return The accepted connection
     */
    CPS_FTP_Connection_Ptr acceptConnection(int listenerSocket);

    /**
     * Add File Descriptor of control socket of the connection to the master event list
     * so that any event happens on the control socket of the connection, the connection will be triggered to handle
     *
     * Subscribe a notification handler to the connection so that it will notify server the change and the server will act accordingly
     *
     * @param[in] connection The connection of control socket to be added
     * @return N/A
     */
    void addConnection(CPS_FTP_Connection_Ptr connection);

    /**
     * Remove File Descriptor of control socket of the connection from master event list
     *
     * @param[in] connection The connection of control socket to be removed
     * @return N/A
     */
    void removeConnection(CPS_FTP_Connection_Ptr connection);

    /**
     * Add File Descriptor of data socket of a connection to the server to the master event list
     * so that any event happens on the data socket of the connection, the connection will be triggered to handle
     *
     * Beside that it adds the data transfer event of the connection to the master event list as well. This event
     * indicates the connection has data to transfer
     *
     * @param[in] connection The connection of data socket to be added
     * @return N/A
     */
    void addDataConnection(CPS_FTP_Connection_Ptr connection);

    /**
     * Remove File Descriptor of data socket of a connection from the server's master event list
     * Remove data transfer event of a connection from the master event list
     *
     * @param[in] connection The connection of data socket to be added
     * @return N/A
     */
    void removeDataConnection(CPS_FTP_Connection_Ptr connection);

    /**
     * Add FD to 'select' or event loop
     *
     * @param[in] fd The file descriptor (event) to be added
     * @return N/A
     */
    void addFD(int fd);

    /**
     * Remove FD from 'select' or event loop
     *
     * @param[in] fd The file descriptor (event) to be removed
     * @return N/A
     */
    void removeFD(int fd);

private:
    // Addresses and port to be listen
    std::string m_addresses[MAX_LISTENER];
    unsigned int m_port;

    // FD of the listener
    int m_listenerSockets[MAX_LISTENER];

    // Master file descriptor list or event list
    fd_set m_masterFDs;

    // Temp event list
    fd_set m_readFDs;

    // The maximum FD in the master event list
    int m_maxFD;

    // Event indicate to stop
    CPS_FTP_Event_FD m_stopEvent;

    // Map between socket & FTP connection (which has control and data socket)
    std::map<int, CPS_FTP_Connection_Ptr> m_socketMap;

    // List of marked delete event
    std::set<int> m_delSocketSet;

    // Running status of server
    bool m_running;

    // For condition variable
    boost::mutex    m_mutex;
    boost::condition_variable m_condition;
};

#endif /* CPS_FTP_SERVER_H_ */
