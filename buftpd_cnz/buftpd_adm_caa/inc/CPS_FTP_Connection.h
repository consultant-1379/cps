/*
NAME
    File_name:CPS_FTP_Connection.h

Ericsson Utvecklings AB

    COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

    The Copyright to the computer program(s) herein is the property of
    Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
    copied only with the written permission from Ericsson Utvecklings AB
    or in accordance with the terms and conditions stipulated in the
    agreement/contract under which the program(s) have been supplied.

DESCRIPTION
    Class implements the connection handling FTP protocol.
    In A FTP connection, it includes:
    - a control socket to handle FTP command/reply such as USER, PASS..
    - a data socket to handle data transferring to the client
    - a data transfer event FD to trigger data transfer. Since the data to transfer usually is big
    it needs to be splitted into multi parts to send in order to avoid blocking the server.
    After sending 1 part of the data, it will enable/set this event to indicate that it still has data to send.
    And the server will come back trigger the data transfer after processing other event.

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
#ifndef CPS_FTP_CONNECTION_H_
#define CPS_FTP_CONNECTION_H_

#include "CPS_FTP_Event_FD.h"
#include "CPS_BUAP_BUFile.h"

#include <string>
#include <netinet/in.h>
#include <map>

#include "boost/enable_shared_from_this.hpp"
#include "boost/signal.hpp"



// FTP connection internal state
enum e_connection_state
{
    INIT = 0,  // Initialized (not-connected)
    CONNECTED, // Connection established
    USER,      // USER command processed
    LOGGED,    // User logged in
    PORT,      // PORT command processed
    RETRIEVING // File retrieval in progress
};

// Notification from FTP connection to Server
enum e_notification
{
    NONE = 0,
    CONTROL_SOCKET_CLOSING,
    DATA_SOCKET_CONNECTED,
    DATA_SOCKET_CLOSING,
};

// Forward declaration
class CPS_FTP_Connection;
typedef boost::shared_ptr<CPS_FTP_Connection> CPS_FTP_Connection_Ptr;

typedef boost::signal<void (CPS_FTP_Connection_Ptr, e_notification)> Notifier;
typedef boost::signals::connection Notification;

class CPS_FTP_Connection : public boost::enable_shared_from_this<CPS_FTP_Connection>
{

public:
    CPS_FTP_Connection(int controlSocket, struct sockaddr_in clientAddr);
    virtual ~CPS_FTP_Connection();

    int getControlSocket() const {return m_controlSocket;}
    int getDataSocket() const {return m_dataSocket;}
    int getDataTransferEvent() const {return m_dataTransferEvent.getFd();}

    /**
     * Handle incoming data for the connection
     *
     *
     * @param[i] socket The socket which data come from
     * @return N/A
     */
    void handleIncommingPacket(int socket);

    /**
     * Stop the connection
     *
     * @param  N/A
     * @return N/A
     */
    void stop();

    /**
     * Subscribe notification handler for the connection change
     *
     * @param[in] subscriberHandler Handler (callback) to call when the connection has sth to notify
     * @return notification (or key) of the subscription
     */
    Notification registerNotification(Notifier::slot_function_type subscriberHandler);

    /**
     * Un-subscribe notification handler
     *
     * @param[in] subscriber The key of the subscription to be removed
     * @return N/A
     */
    void unregisterNotification(Notification subscriber);

private:
    /**
     * Handle incoming data from control socket
     *
     * @param N/A
     * @return N/A
     */
    void handleControlPacket();

    /**
     * Handle incoming data from data socket
     *
     * @param N/A
     * @return N/A
     */
    void handleDataPacket();

    /**
     * Parse the input FTP command then call the corresponding handler
     *
     * @param cmd FTP command to be parsed
     * @return N/A
     */
    void parseFtpCmd(const std::string& cmd);

    /**
     * Send the reply to the peer (client)
     *
     * @param data The reply to be sent
     * @return N/A
     */
    void sendControlData(const std::string& data);

    /**
     * Open & connect a data socket to the peer (client) for data transferring
     *
     *
     * @param N/A
     * @return true   Open data connection successfully
     *         false  Unable to open data connection
     */
    bool openDataConnection();

    /**
     * Transfer file (data) to the peer
     *
     * The file is divided into multi parts to send to avoid blocking
     * After a part is sent, if there is still other parts to need to be sent,
     * it will raise a event to indicate this connection has data need to be sent.
     *
     * @param N/A
     * @return N/A
     */
    void transferData();

    /**
     * Close data socket
     *
     * @param N/A
     * @return N/A
     */
    void closeDataConnection();

    /**
     * Handler of USER command
     *
     * @param argument The input argument
     * @return N/A
     */
    void doUSER(const std::string& argument);

    /**
     * Handler of PASS command
     *
     * @param argument The input argument
     * @return N/A
     */
    void doPASS(const std::string& argument);

    /**
     * Handler of SYST command
     *
     * @param argument The input argument
     * @return N/A
     */
    void doSYST(const std::string& argument);

    /**
     * Handler of TYPE command
     *
     * @param argument The input argument
     * @return N/A
     */
    void doTYPE(const std::string& argument);

    /**
     * Handler of PORT command
     *
     * @param argument The input argument
     * @return N/A
     */
    void doPORT(const std::string& argument);

    /**
     * Handler of RETR command
     *
     * @param argument The input argument
     * @return N/A
     */
    void doRETR(const std::string& argument);

    /**
     * Handler of ABOR command
     *
     * @param argument The input argument
     * @return N/A
     */
    void doABOR(const std::string& argument);

    /**
     * Handler of QUIT command
     *
     * @param argument The input argument
     * @return N/A
     */
    void doQUIT(const std::string& argument);

    /**
     * Handler of invalid command
     *
     * @param argument The input argument
     * @return N/A
     */
    void doUNKOWN(const std::string& argument);

private:
    // The first Greeting message for this connection
    static const std::string GREETING_MSG;

    // Data buffer size
    static const unsigned long BUFFER_SIZE;

    // Delay in nano seconds for data transfer
    static const long DELAY_NS;
	
	// MAXIMUM Retry Times
	static const int MAX_RETRIES;

private:
    // Control socket
    int m_controlSocket;

    // Data socket
    int m_dataSocket;

    // Address of the data socket
    in_addr_t m_dataAddr;

    // Port of the data socket
    unsigned int m_dataPort;

    // BUAP API to fetch file
    CPS_BUAP_BUFile_Ptr m_bufile;

    // Data transfer event to indicate there is still data to send
    CPS_FTP_Event_FD m_dataTransferEvent;

    // Transferred byte
    unsigned long m_byteTransferred;

    // Retry times
    int m_retryTimes;

    // Internal state of the connection
    e_connection_state m_state;

    // Notifier which is used to notify the subscriber for the connection state change
    Notifier m_notifier;

    // Address & port of this connection
    struct sockaddr_in m_clientAddr;
};


#endif /* CPS_FTP_CONNECTION_H_ */
