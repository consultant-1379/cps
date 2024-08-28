/*
NAME
    File_name:CPS_FTP_Server.cpp

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

#include "CPS_FTP_Server.h"
#include "CPS_FTP_Trace.h"
#include "CPS_FTP_Events.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>

#include <iostream>
#include <algorithm>

#include "boost/bind.hpp"

#include "CPS_FTP_Service.h"

using namespace std;

const unsigned int CPS_FTP_Server::DEFAULT_PORT = 2345;

extern CPS_FTP_Service* g_pService;

CPS_FTP_Server::CPS_FTP_Server(const string addresses[MAX_LISTENER], unsigned int port) : m_port(port), m_running(false)
{
    // newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Server::CPS_FTP_Server(,%d)", 0, port));

    for (int i = 0; i < MAX_LISTENER; i++)
    {
        m_addresses[i] = addresses[i];
        m_listenerSockets[i] = -1;
    }

    FD_ZERO(&m_masterFDs);
    FD_ZERO(&m_readFDs);
    m_maxFD = -1;
}

CPS_FTP_Server::~CPS_FTP_Server()
{
   //  newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Server::~CPS_FTP_Server()", 0));
}

/**
 * Initialize the server
 *
 * Open socket then listen at given addr:port in the constructor.
 * It will listen at 2 addr:port next together at maximum
 *
 * @param N/A
 * @return result of the initialization
 *         false Non of the address:port can be listened
 *         true  At least 1 address:port is listened successfully
 */
bool CPS_FTP_Server::init()
{
    newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Server::init()", 0));

    bool result[MAX_LISTENER];
    int n = MAX_LISTENER;

    for (int i = 0; i < n; i++)
    {
        result[i] = false;

        // Only listen to localhost when it's the last one
        if ((i == MAX_LISTENER - 1) && (m_addresses[i] != "127.0.0.1"))
        {
            m_listenerSockets[i] = -1;
            continue;
        }

        // Get the listener
        m_listenerSockets[i] = socket(AF_INET, SOCK_STREAM, 0);
        if (m_listenerSockets[i] == -1)
        {
            TRACE((LOG_LEVEL_ERROR, "Failed to get listener: %d - %s", 0, errno, strerror(errno)));
            continue;
        }

        // Set socket option REUSE PORT
        int optval = 1;
        if (setsockopt(m_listenerSockets[i], SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
        {
            TRACE((LOG_LEVEL_ERROR, "Failed to set REUSE_PORT option: %d - %s", 0, errno, strerror(errno)));
            continue;
        }

        // Start to Bind
        struct sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        if (inet_aton(m_addresses[i].c_str(), &serverAddr.sin_addr) == 0)
        {
            TRACE((LOG_LEVEL_ERROR, "Invalid ip address: %s", 0, m_addresses[i].c_str()));
            continue;
        }

        serverAddr.sin_port = htons(m_port);
        memset(&(serverAddr.sin_zero), '\0', sizeof(serverAddr.sin_zero));

        if (bind(m_listenerSockets[i], (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
        {
            TRACE((LOG_LEVEL_ERROR, "Failed to bind %s:%u - %d - %s", 0, m_addresses[i].c_str(), m_port, errno, strerror(errno)));
            continue;
        }

        // Start to Listen
        if (listen(m_listenerSockets[i], 2) == -1)
        {
            TRACE((LOG_LEVEL_ERROR, "Failed to listen %s:%u - %d - %s", 0, m_addresses[i].c_str(), m_port, errno, strerror(errno)));
            continue;
        }

        // Mark it as successful
        result[i] = true;

        // Check if needs to bind more (even: try to bind the next one, odd: stop binding)
        n = (i % 2) ? i : (i + 2);

        if (n >= MAX_LISTENER)
        {
            break;
        }
    }

    bool finalResult = false;
    // Reset the unsuccessful listener
    for (int i = 0; i < MAX_LISTENER; i++)
    {
        if (!result[i])
        {
            if (m_listenerSockets[i] != -1)
            {
                close(m_listenerSockets[i]);
                m_listenerSockets[i] = -1;
            }
        }
        else
        {
            finalResult = true;
        }
    }

    if (finalResult == false)
    {
        TRACE((LOG_LEVEL_ERROR, "Failed to initialize the server - All address can't be bound.", 0));
        EVENT((CPS_FTP_Events::listener_init_fail, 0, m_port, errno, strerror(errno)));
    }

    return finalResult;
}

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
void CPS_FTP_Server::run()
{
    newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Server::run()", 0));

    // Use condition variable to notify other thread
    // that server is started
    {
        try
        {
            boost::lock_guard<boost::mutex> lock(m_mutex);
        }
        catch (...)
        {
            TRACE((LOG_LEVEL_ERROR, "Failed to lock the server thread", 0));
        }

        m_running = true;
    }
    m_condition.notify_one();

    struct timeval tv;
    int retval = 0;
    int retries = 0;

    // Init the server in 3 times
    while (!this->init())
    {
        TRACE((LOG_LEVEL_ERROR, "Failed to initialize the server: %d times", 0, ++retries));

        if (retries < 3)
        {
            // Retry in 2 seconds
            sleep(2);
            continue;
        }

        // Try 3 times but failed, exit the loop and report the error
        //this->stop();
        m_running = false;

        // Report error to AMF if it's running as service
        if (g_pService)
        {
            TRACE((LOG_LEVEL_FATAL, "Reporting error to AMF", 0));
            EVENT((CPS_FTP_Events::Unrecoverable_error_report, 0));
            g_pService->componentReportError(ACS_APGCC_COMPONENT_RESTART);
        }

        return;
    }

    // Init the master FDs list
    FD_ZERO(&m_masterFDs);
    FD_ZERO(&m_readFDs);
    for (int i = 0; i < MAX_LISTENER; i++)
    {
        if (m_listenerSockets[i] != -1)
        {
            // Add listener socket to the event list
            this->addFD(m_listenerSockets[i]);
        }
    }

    // Add stop event to the event list
    this->addFD(m_stopEvent.getFd());

    bool recvStopEvent = false;
    TRACE((LOG_LEVEL_INFO, "Start event loop", 0));
    // Run event loop
    while (true)
    {
        m_readFDs = m_masterFDs;
        // Set select interval time to 10
        tv.tv_sec = 10;
        tv.tv_usec = 0;
        
        retval = select(m_maxFD + 1, &m_readFDs, NULL, NULL, &tv);

        if (retval == -1)
        {
            // A signal was caught
            if (errno == EINTR)
            {
                continue;
            }

            TRACE((LOG_LEVEL_ERROR, "Failed to 'select': %d - %s", 0, errno, strerror(errno)));
            EVENT((CPS_FTP_Events::select_call_fail, 0, errno, strerror(errno)));

            break;
        }

        if (retval == 0)
        {
            // No event
            continue;
        }

        if (FD_ISSET(m_stopEvent.getFd(), &m_readFDs))
        {
            recvStopEvent = true;
            m_stopEvent.resetEvent();
            this->handleStop();

            // exit the loop
            break;
        }

        // Check if new incoming connection
        for (int i = 0; i < MAX_LISTENER; i++)
        {
            if (m_listenerSockets[i] != -1 && FD_ISSET(m_listenerSockets[i], &m_readFDs))
            {
#if _SINGLE_CONN
                bool delPrevConn = (m_socketMap.size() > 0) ? true : false;
                CPS_FTP_Connection_Ptr prevConnection =(*m_socketMap.begin()).second;
#endif

                // Accept new connection
                CPS_FTP_Connection_Ptr connection = this->acceptConnection(m_listenerSockets[i]);

                if (connection.get() != NULL)
                {
                    // Accept successfully then add connection to the map.
                    this->addConnection(connection);
                }
#if _SINGLE_CONN
                // Delete previous connection after accepting
                if (delPrevConn)
                {
                    TRACE((LOG_LEVEL_INFO, "New connection come -> delete old connection: %d", 0,prevConnection->getControlSocket()));

                    prevConnection->stop();
                }
#endif
            }
        }

        // Check if any incoming ftp control/data message
        map<int, CPS_FTP_Connection_Ptr>::iterator it;
        for (it = m_socketMap.begin(); it != m_socketMap.end(); ++it)
        {
            if (FD_ISSET((*it).first, &m_readFDs))
            {
                CPS_FTP_Connection_Ptr connection = (*it).second;
                connection->handleIncommingPacket((*it).first);
            }
        }

        // Check any FD (event) needed to remove from the list
        if (m_delSocketSet.size() > 0)
        {
            set<int>::iterator itSet;
            for (itSet = m_delSocketSet.begin(); itSet != m_delSocketSet.end(); ++itSet)
            {
                TRACE((LOG_LEVEL_INFO, "Removing FD: %d from the event list", 0, *itSet));
                m_socketMap.erase(*itSet);
            }
            m_delSocketSet.clear();

            TRACE((LOG_LEVEL_INFO, "Event list count: %d", 0, m_socketMap.size()));
        }
    }

    if (!recvStopEvent && g_pService)
    {
        // While loop is exited because of fault
        // Report error to AMF if it's running as service
        TRACE((LOG_LEVEL_FATAL, "Reporting error to AMF", 0));
        g_pService->componentReportError(ACS_APGCC_COMPONENT_RESTART);

    }

    // Exit Event loop so that server is not running anymore.
    m_running = false;
}

/**
 * Accept a connection from the given listener
 *
 * @param[in] listenerSocket  The listener (representing addr:port) of the incoming connection
 * @return The accepted connection
 */
CPS_FTP_Connection_Ptr CPS_FTP_Server::acceptConnection(int listenerSocket)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Server::acceptConnection(%d)", 0, listenerSocket));

    int newfd = -1;
    struct sockaddr_in clientAddr;
    socklen_t len = sizeof(clientAddr);
    CPS_FTP_Connection_Ptr connection;

    newfd = accept(listenerSocket, (struct sockaddr *) &clientAddr, &len);

    if (newfd == -1)
    {
        TRACE((LOG_LEVEL_ERROR, "Accept new connection failed: %d - %s", 0, errno, strerror(errno)));
        EVENT((CPS_FTP_Events::listener_accept_fail, 0, listenerSocket, errno, strerror(errno)));
    }
    else
    {
        TRACE((LOG_LEVEL_INFO, "Accepted new connection: %s", 0, inet_ntoa(clientAddr.sin_addr)));
        connection.reset(new CPS_FTP_Connection(newfd, clientAddr));
    }

    return connection;
}

/**
 * Add File Descriptor of control socket of the connection to the master event list
 * so that any event happens on the control socket of the connection, the connection will be triggered to handle
 *
 * Subscribe a notification handler to the connection so that it will notify server the change and the server will act accordingly
 *
 * @param[in] connection The connection of control socket to be added
 * @return N/A
 */
void CPS_FTP_Server::addConnection(CPS_FTP_Connection_Ptr connection)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Server::addConnection(%d)", 0, connection->getControlSocket()));

    if (connection->getControlSocket() == -1)
    {
        TRACE((LOG_LEVEL_ERROR, "Failed to add connection with FD: %d", 0, connection->getControlSocket()));
		EVENT((CPS_FTP_Events::ctrl_connection_add_fail, 0, connection->getControlSocket()));
        return;
    }

    // Add connection to the map
    m_socketMap.insert(pair<int, CPS_FTP_Connection_Ptr>(connection->getControlSocket(), connection));

    // Register notification event
    connection->registerNotification(boost::bind(&CPS_FTP_Server::OnConnectionChange, this, _1, _2));

    // Add the connection control socket to event list
    this->addFD(connection->getControlSocket());
}

/**
 * Remove File Descriptor of control socket of the connection from master event list
 *
 * @param[in] connection The connection of control socket to be removed
 * @return N/A
 */
void CPS_FTP_Server::removeConnection(CPS_FTP_Connection_Ptr connection)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Server::removeConnection(%d)", 0, connection->getControlSocket()));

    int controlSocket = connection->getControlSocket();

    // Remove connection event from event list
    if (controlSocket != -1)
    {
        this->removeFD(controlSocket);
        m_delSocketSet.insert(controlSocket);
    }
}

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
void CPS_FTP_Server::addDataConnection(CPS_FTP_Connection_Ptr connection)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Server::addDataConnection(%d)", 0, connection->getDataSocket()));

    if (connection->getDataSocket() == -1 || connection->getDataTransferEvent() == -1)
    {
        TRACE((LOG_LEVEL_ERROR, "Failed to add data connection with: %d %d %d", 0, connection->getControlSocket(), connection->getDataSocket(), connection->getDataTransferEvent()));
		EVENT((CPS_FTP_Events::data_connection_add_fail, 0, connection->getControlSocket(), connection->getDataSocket(), connection->getDataTransferEvent()));

        return;
    }

    // Add data connection to the map
    m_socketMap.insert(pair<int, CPS_FTP_Connection_Ptr>(connection->getDataSocket(), connection));
    m_socketMap.insert(pair<int, CPS_FTP_Connection_Ptr>(connection->getDataTransferEvent(), connection));

    // Add data transfer event to the event list
    this->addFD(connection->getDataSocket());
    this->addFD(connection->getDataTransferEvent());
}

/**
 * Remove File Descriptor of data socket of a connection from the server's master event list
 * Remove data transfer event of a connection from the master event list
 *
 * @param[in] connection The connection of data socket to be added
 * @return N/A
 */
void CPS_FTP_Server::removeDataConnection(CPS_FTP_Connection_Ptr connection)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Server::removeDataConnection(%d)", 0, connection->getDataSocket()));

    int dataSocket        = connection->getDataSocket();
    int dataTransferEvent = connection->getDataTransferEvent();

    // Remove data socket from the event list
    if (dataSocket != -1)
    {
        this->removeFD(dataSocket);
        m_delSocketSet.insert(dataSocket); // Mark as deleted
    }

    if (dataTransferEvent != -1)
    {
        this->removeFD(dataTransferEvent);
        m_delSocketSet.insert(dataTransferEvent); // Mark as deleted
    }
}

/**
 * Handler of the connection state change
 *
 * @param[in] connection  The state changed connection
 * @param[in] event       The notification event
 * @return N/A
 */
void CPS_FTP_Server::OnConnectionChange(CPS_FTP_Connection_Ptr connection, e_notification event)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Server::OnConnectionChange(%d, %d)", 0, connection->getControlSocket(), event));

    switch (event)
    {
    case CONTROL_SOCKET_CLOSING:
        this->removeConnection(connection);
        break;

    case DATA_SOCKET_CONNECTED:
        this->addDataConnection(connection);
        break;

    case DATA_SOCKET_CLOSING:
        this->removeDataConnection(connection);
        break;

    default:
        break;
    }
}

/**
 * Stop the server
 *
 * @param  N/A
 * @return N/A
 */
void CPS_FTP_Server::stop()
{
    newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Server::stop()", 0));

    // Tell event loop to stop
    m_stopEvent.setEvent();
}

/**
 * Stop the server
 *
 * Close all the listener & connections
 *
 * @param N/A
 * @return N/A
 */
void CPS_FTP_Server::handleStop()
{
    newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Server::handleStop()", 0));

    this->removeFD(m_stopEvent.getFd());

    // Stop the listeners
    for (int i = 0; i < MAX_LISTENER; i++)
    {
        if (m_listenerSockets[i] != -1)
        {
            this->removeFD(m_listenerSockets[i]);
            close(m_listenerSockets[i]);

            TRACE((LOG_LEVEL_INFO, "Closed listener %d", 0, m_listenerSockets[i]));
            m_listenerSockets[i] = -1;
        }
    }

    // Stop all the connections
    map<int, CPS_FTP_Connection_Ptr>::iterator it;
    for (it = m_socketMap.begin(); it != m_socketMap.end(); ++it)
    {
        if (FD_ISSET((*it).first, &m_masterFDs))
        {
            CPS_FTP_Connection_Ptr connection = (*it).second;
            connection->stop();
        }
    }
    // Check any event needed to remove from the list
    if (m_delSocketSet.size() > 0)
    {
        set<int>::iterator itSet;
        for (itSet = m_delSocketSet.begin(); itSet != m_delSocketSet.end(); ++itSet)
        {
            TRACE((LOG_LEVEL_INFO, "Removing FD: %d from the event list", 0, *itSet));
            m_socketMap.erase(*itSet);
        }
        m_delSocketSet.clear();

        TRACE((LOG_LEVEL_INFO, "Event list count: %d", 0, m_socketMap.size()));
    }
}

/**
 * Add FD to 'select' or event loop
 *
 * @param[in] fd The file descriptor (event) to be added
 * @return N/A
 */
void CPS_FTP_Server::addFD(int fd)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Server::addFD(%d)", 0, fd));

    FD_SET(fd, &m_masterFDs);
    m_maxFD = max(fd, m_maxFD);
}

/**
 * Remove FD from 'select' or event loop
 *
 * @param[in] fd The file descriptor (event) to be removed
 * @return N/A
 */
void CPS_FTP_Server::removeFD(int fd)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Server::removeFD(%d)", 0, fd));

    FD_CLR(fd, &m_masterFDs);
    FD_CLR(fd, &m_readFDs);
}

/**
 * This function is used to block the calling thread
 * Until FTP Server finishes start up.
 *
 * @param N/A
 * @return N/A
 */
void CPS_FTP_Server::waitUntilRunning(void)
{
    newTRACE((LOG_LEVEL_TRACE, "CPS_FTP_Server::waitUntilRunning()", 0));

    boost::unique_lock<boost::mutex> lock(m_mutex);

    while (m_running == false)
    {
        TRACE((LOG_LEVEL_INFO, "Wait until CPS_FTP_Server Thread signal", 0));
        m_condition.wait(lock);
        TRACE((LOG_LEVEL_INFO, "CPS_FTP_Server Thread signaled", 0));
    }
}

/**
 * Report the running status of the server
 *
 * @param N/A
 * @return true  BUFTP server is running
 *         false BUFTP server is NOT running
 */
bool CPS_FTP_Server::isRunning(void)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Server::isRunning(void) - %d", 0, (int) m_running));

    return m_running;
}
