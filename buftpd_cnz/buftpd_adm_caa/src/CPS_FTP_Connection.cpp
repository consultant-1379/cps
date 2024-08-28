/*
NAME
    File_name:CPS_FTP_Connection.cpp

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
2011-07-14 xquydao Created
2011-02-14 xquydao Updated to call addtional call from api
2012-09-27 xvunguy Updated to catch exception thrown by boost::regex_search

*/

#include "CPS_FTP_Connection.h"
#include "CPS_FTP_Trace.h"
#include "CPS_FTP_Events.h"

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cerrno>
#include <cstring>

#include "boost/regex.hpp"
#include "boost/tokenizer.hpp"
#include "boost/lexical_cast.hpp"


using namespace std;

const string CPS_FTP_Connection::GREETING_MSG = "220- Welcome to the CPS Back Up FTP server\n"
                                                "220- Unauthorised access forbidden\n"
                                                "220 Limited FTP functionality implemented";

const unsigned long CPS_FTP_Connection::BUFFER_SIZE = 6 * 1024; // 6144
const long CPS_FTP_Connection::DELAY_NS = 1 * 1000000; // 1ms
const int CPS_FTP_Connection::MAX_RETRIES = 5;

CPS_FTP_Connection::CPS_FTP_Connection(int controlSocket, struct sockaddr_in clientAddr) : m_controlSocket(controlSocket),
                                                                                           m_dataSocket(-1),
                                                                                           m_dataAddr(0),
                                                                                           m_dataPort(0),
                                                                                           m_byteTransferred(0),
                                                                                           m_retryTimes(0),
                                                                                           m_state(INIT),
                                                                                           m_clientAddr(clientAddr)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Connection::CPS_FTP_Connection(%d)", 0, controlSocket));

    this->sendControlData(CPS_FTP_Connection::GREETING_MSG);
    m_state = CONNECTED;
}

CPS_FTP_Connection::~CPS_FTP_Connection()
{
    newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Connection::~CPS_FTP_Connection()", 0));
}

/**
 * Handle incoming data for the connection
 *
 *
 * @param[i] socket The socket which data come from
 * @return N/A
 */
void CPS_FTP_Connection::handleIncommingPacket(int socket)
{
    // Transfer data is called plenty of times, better to not show this trace for transfer data case
    if (m_dataTransferEvent.getFd() != socket)
    {
        newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Connection::HandleIncommingData(%d)", 0, socket));
    }

    if (m_controlSocket == socket)
    {
        this->handleControlPacket();
    }
    else if (m_dataSocket == socket)
    {
        this->handleDataPacket();
    }
    else if (m_dataTransferEvent.getFd() == socket)
    {
        m_dataTransferEvent.resetEvent();
        this->transferData();
    }
}

/**
 * Handle incoming data from control socket
 *
 * @param N/A
 * @return N/A
 */
void CPS_FTP_Connection::handleControlPacket()
{
    newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Connection::HandleControlPacket()", 0));

    char buffer[256] = {0};
    int bytes = 0;

    bytes = recv(m_controlSocket, buffer, sizeof(buffer), 0);

    if (bytes <= 0)
    {
        TRACE((LOG_LEVEL_ERROR, "Recv failed or peer shutdown the connection: %d - %s", 0, bytes, strerror(errno)));

        this->stop();
        return;
    }

    // Obtain the FTP command
    string temp(buffer, bytes);
    size_t pos = temp.find_first_of("\r\n", 0);

    if (pos == string::npos)
    {
        TRACE((LOG_LEVEL_ERROR, "Can't obtain the FTP command: %d - %s", 0, bytes, strerror(errno)));
        return;
    }

    string ftpCmd = temp.substr(0, pos);
    this->parseFtpCmd(ftpCmd);
}

/**
 * Open & connect a data socket to the peer (client) for data transferring
 *
 *
 * @param N/A
 * @return true   Open data connection successfully
 *         false  Unable to open data connection
 */
bool CPS_FTP_Connection::openDataConnection(void)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Connection::openDataConnection(void)", 0));

    struct sockaddr_in dataSockAddr;

    // Open data socket in order to transfer file
    m_dataSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_dataSocket == -1)
    {
        TRACE((LOG_LEVEL_ERROR, "Failed to open data socket: %d - %s", 0, errno, strerror(errno)));
        EVENT((CPS_FTP_Events::datasock_open_fail, 0, errno, strerror(errno)));

        return false;
    }

    dataSockAddr.sin_family      =  AF_INET;
    dataSockAddr.sin_addr.s_addr =  m_dataAddr;
    dataSockAddr.sin_port        =  htons(m_dataPort);
    memset(&(dataSockAddr.sin_zero), '\0', sizeof(dataSockAddr.sin_zero));

    if (connect(m_dataSocket, (struct sockaddr *)&dataSockAddr, sizeof(dataSockAddr)) == -1)
    {
        TRACE((LOG_LEVEL_ERROR, "Failed to connect to socket: %d - %s", 0, errno, strerror(errno)));
        EVENT((CPS_FTP_Events::datasock_connect_fail, 0, errno, strerror(errno)));

        close(m_dataSocket);
        m_dataSocket = -1;
        return false;
    }

    return true;
}

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
void CPS_FTP_Connection::transferData(void)
{
#if _MORE_TRACE
    newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Connection::transferData(void)", 0));
#endif

    if (m_state != RETRIEVING)
    {
#if _MORE_TRACE
        TRACE((LOG_LEVEL_WARN, "Connection state is not RETRIEVING: %d", 0, m_state));
#else
        newTRACE((LOG_LEVEL_WARN, "Connection state is not RETRIEVING: %d", 0, m_state));
#endif
        return;
    }

    string reply = "";
    char* buffer = new char[CPS_FTP_Connection::BUFFER_SIZE];
    CPS_BUAP_BUFILE_RETCODE ret;
    uint32_t readBytes = 0;
    ssize_t bytes = 0;
    ssize_t len   = 0;
    ssize_t index = 0;

    // used by nanosleep
    struct timespec req;
    struct timespec rem;

    // Read file
    ret = m_bufile->readSubfile(buffer, CPS_FTP_Connection::BUFFER_SIZE, readBytes);

    if (ret != CPS_BUAP_BUFILE_OK) // EOF
    {
        reply = (ret != CPS_BUAP_BUFILE_EOF) ? "451 Requested action aborted. Local error in processing" : "226 Binary transfer complete";

        // Since LACPIPN has problem when the connection is closed but
        // still has data in the internal buffer.
        // Delay 100ms to make sure LACPIPN internal buffer is empty before the connection is closed.
        req.tv_sec = 0;
        req.tv_nsec = CPS_FTP_Connection::DELAY_NS * 100;
        nanosleep(&req, &rem);
    }
    else
    {
        len = readBytes;
        bytes = 0;
        index = 0;

        // Transfer the data read from file
        while (len > 0)
        {
#if _MORE_TRACE
            TRACE((LOG_LEVEL_INFO, "Sending data with %ld bytes to peer", 0, len));
#endif
            bytes = ::send(m_dataSocket, buffer + index, len, MSG_NOSIGNAL | MSG_DONTWAIT);
            //TRACE(("Sent data with %ld bytes to peer", 0, bytes));

            if (bytes == -1)
            {
                if (errno == EAGAIN)
                {
                    m_retryTimes++;                    
                    if (m_retryTimes >= CPS_FTP_Connection::MAX_RETRIES)
                    {
#if _MORE_TRACE
                        TRACE((LOG_LEVEL_WARN, "Can't send %d bytes data at this time, peer buffer may be full", 0, len));
#else
                        newTRACE((LOG_LEVEL_WARN, "Can't send %d bytes data at this time, peer buffer may be full", 0, len));
#endif
                        m_retryTimes = 0;
                    }
                    
                    m_bufile->moveSubfilePtr(m_byteTransferred);
                    //lseek(m_fileFD, m_byteTransferred, SEEK_SET);

                    // Since peer buffer is not ready, delay 5ms
                    req.tv_sec = 0;
                    req.tv_nsec = CPS_FTP_Connection::DELAY_NS * 5;
                    nanosleep(&req, &rem);

                    break;
                };

                // The end point may be closed
#if _MORE_TRACE
                TRACE((LOG_LEVEL_ERROR, "Failed to %d bytes send data: %d - %s", 0, len, errno, strerror(errno)));
#else
                newTRACE((LOG_LEVEL_ERROR, "Failed to %d bytes send data: %d - %s", 0, len, errno, strerror(errno)));
#endif
                EVENT((CPS_FTP_Events::datasock_send_fail, 0, m_dataSocket, errno, strerror(errno)));

                reply = "425 Connection closed; transfer aborted.";
                break;
            }

            if (len - bytes != 0)
            {
                m_retryTimes++;
                if (m_retryTimes >= CPS_FTP_Connection::MAX_RETRIES)
                {
#if _MORE_TRACE
                    TRACE((LOG_LEVEL_WARN, "%d bytes data is not fully sent in 1 round. Sent: %d", 0, len, bytes));
#else
                    newTRACE((LOG_LEVEL_WARN, "%d bytes data is not fully sent in 1 round. Sent: %d", 0, len, bytes));
#endif
                }
            }
            else
            {
                m_retryTimes = 0;
            }

            m_byteTransferred += bytes;
            index += bytes;
            len   = len - bytes;
        }

        // The data transferring is not finished, need to continue
        m_dataTransferEvent.setEvent();
    }

    // Deallocate buffer
    delete[] buffer;

    if (reply.length() > 0)
    {
        // The end point may be closed
#if _MORE_TRACE
        TRACE((LOG_LEVEL_INFO, "Transfer done. Byte transferred: %lu", 0, m_byteTransferred));
#else
        newTRACE((LOG_LEVEL_INFO, "Transfer done. Byte transferred: %lu", 0, m_byteTransferred));
#endif
        // Transfer done
        this->sendControlData(reply);

        // Rest the state to LOG after failure or done
        m_state = LOGGED;

        // Close subfile after transferring
        m_bufile->closeSubfile();

        this->closeDataConnection();
    }
}

/**
 * Close data socket
 *
 * @param N/A
 * @return N/A
 */
void CPS_FTP_Connection::closeDataConnection(void)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Connection::closeDataConnection(void) - %d", 0, m_dataSocket));

    // Notify Server that data connection is closing
    m_notifier(shared_from_this(), DATA_SOCKET_CLOSING);

    // Close data socket
    if (m_dataSocket != -1)
    {
        close(m_dataSocket);
        m_dataSocket = -1;
    }

    m_byteTransferred = 0;
}

/**
 * Parse the input FTP command then call the corresponding handler
 *
 * @param cmd FTP command to be parsed
 * @return N/A
 */
void CPS_FTP_Connection::parseFtpCmd(const string& cmd)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Connection::parseFtpCmd(%s)", 0, cmd.c_str()));

    string code = "";
    string argument = "";
    boost::regex re("^([A-Z]{4})( (.*))?$");
    boost::cmatch matches;

    try
    {
        // Get the FTP command code & argument
        if (boost::regex_search(cmd.c_str(), matches, re, boost::match_posix))
        {
            code = string(matches[1].first, matches[1].second);
            argument = string(matches[3].first, matches[3].second);
        }
    }
    catch (...)
    {
        // Failed to parse, consider as UNKNOWN
        this->doUNKOWN(cmd);

        return;
    }

    if (code == "USER")
    {
        this->doUSER(argument);
    }
    else if (code == "PASS")
    {
        this->doPASS(argument);
    }
    else if (code == "SYST") // New command code not in DS
    {
        this->doSYST(argument);
    }
    else if (code == "TYPE")
    {
        this->doTYPE(argument);
    }
    else if (code == "PORT")
    {
        this->doPORT(argument);
    }
    else if (code == "RETR")
    {
        this->doRETR(argument);
    }
    else if (code == "ABOR")
    {
        this->doABOR(argument);
    }
    else if (code == "QUIT")
    {
        this->doQUIT(argument);
    }
    else // default
    {
        this->doUNKOWN(argument);
    }
}

/**
 * Handler of USER command
 *
 * @param argument The input argument
 * @return N/A
 */
void CPS_FTP_Connection::doUSER(const string& argument)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Connection::doUSER(%s)", 0, argument.c_str()));

    string reply = "";

    if (argument == "CP_LAB")
    {
        if (m_state == INIT)
        {
            reply = "500 Not connected";
        }
        else if (m_state == CONNECTED || m_state == USER)
        {
            m_state = USER;
            reply = "331 Password required";
        }
        else
        {
            reply = "503 Already logged in";
        }
    }
    else
    {
        reply = "530 Login failed";
        m_state = CONNECTED;
    }

    this->sendControlData(reply);

    TRACE((LOG_LEVEL_INFO, "USER Reply: %s", 0, reply.c_str()));
}

/**
 * Handler of PASS command
 *
 * @param argument The input argument
 * @return N/A
 */
void CPS_FTP_Connection::doPASS(const string& argument)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Connection::doPASS(%s)", 0, argument.c_str()));

    string reply = "";

    if (m_state == USER)
    {
        // Password must be ""
        if (argument != "")
        {
            reply = "530 Login failed";
            m_state = CONNECTED;
        }
        else
        {
            reply = "230 User logged in";
            m_state = LOGGED;
        }
    }
    else
    {
        reply = "503 Bad sequence of commands";
    }

    this->sendControlData(reply);

    TRACE((LOG_LEVEL_INFO, "PASS Reply: %s", 0, reply.c_str()));
}

/**
 * Handler of SYST command
 *
 * @param argument The input argument
 * @return N/A
 */
void CPS_FTP_Connection::doSYST(const string& argument)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Connection::doSYST(%s)", 0, argument.c_str()));

    string reply = "";

    if (argument != "")
    {
        reply = "501 Syntax error in parameters or arguments";
    }
    else
    if (m_state < LOGGED)
    {
        reply = "530 Not logged in";
    }
    else
    {
        reply = "215 UNIX Type: APG43L";
    }

    this->sendControlData(reply);

    TRACE((LOG_LEVEL_INFO, "SYST Reply: %s", 0, reply.c_str()));
}

/**
 * Handler of TYPE command
 *
 * @param argument The input argument
 * @return N/A
 */
void CPS_FTP_Connection::doTYPE(const string& argument)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Connection::doTYPE(%s)", 0, argument.c_str()));

    string reply = "";

    if (m_state < LOGGED)
    {
        reply = "530 Not logged in";
    }
    else if (argument != "I")
    {
        reply = "501 Syntax error in parameters or arguments";
    }
    else
    {
        reply = "200 Type set to image";
    }

    this->sendControlData(reply);

    TRACE((LOG_LEVEL_INFO, "TYPE Reply: %s", 0, reply.c_str()));
}

/**
 * Handler of PORT command
 *
 * @param argument The input argument
 * @return N/A
 */
void CPS_FTP_Connection::doPORT(const string& argument)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Connection::doPORT(%s)", 0, argument.c_str()));

    string reply = "";

    do
    {
        if (m_state < LOGGED)
        {
            reply = "530 Not logged in";
            break;
        }

        if (m_state > LOGGED)
        {
            reply = "503 Bad sequence of commands";
            break;
        }

        using namespace boost;
        // Parse the argument n1,...,n6 by using boost token
        // n1.n2.n3.n4 = ip
        // n5*256+n6 = port
        tokenizer<escaped_list_separator<char> > tok(argument);
        string parts[6] = "";

        int i = 0;
        for (tokenizer<escaped_list_separator<char> >::iterator it=tok.begin(); it != tok.end(); ++it)
        {
            parts[i++] = *it;
        }

        if (i != 6)
        {
            reply = "501 Syntax error in parameters or arguments";
            break;
        }

        // Obtain the IP
        m_dataAddr = inet_addr((parts[0] + "." + parts[1] + "." + parts[2] + "." + parts[3]).c_str());

        // obtain the port
        try
        {
            m_dataPort = lexical_cast<unsigned int>(parts[4]) * 256 + lexical_cast<unsigned int>(parts[5]);
        }
        catch (bad_lexical_cast &)
        {
            m_dataPort = 0;
            reply = "501 Syntax error in parameters or arguments";
            break;
        }

        // Mark the state as PORT
        m_state = PORT;
        reply = "200 PORT command successful";
    }
    while (false);

    this->sendControlData(reply);

    TRACE((LOG_LEVEL_INFO, "PORT Reply: %s - IP: %u Port: %u", 0, reply.c_str(), m_dataAddr, m_dataPort));
}

/**
 * Handler of RETR command
 *
 * @param argument The input argument
 * @return N/A
 */
void CPS_FTP_Connection::doRETR(const string& argument)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Connection::doRETR(%s)", 0, argument.c_str()));

    string reply = "";
    string generation = "";
    string subfile    = "";
    boost::regex re("^RELFSW([0-9]{1,3})-R([0-9]{1,3})$");
    boost::cmatch matches;

    do
    {
        if (m_state < PORT)
        {
            reply = "503 Bad sequence of commands";
            break;
        }

        try
        {
            // Check the format of the file name whether it matches RELFSWn-Rm
            // then get the 'm' & 'n'
            if (boost::regex_search(argument.c_str(), matches, re, boost::match_posix))
            {
               generation = string(matches[1].first, matches[1].second);
               subfile    = string(matches[2].first, matches[2].second);
            }
            else
            {
               reply = "550 File " + argument + " not found";

               // Require PORT, RETR commands again
               m_state = LOGGED;
               break;
            }
        }
        catch(...)
        {
        	// Failed to parse, consider as UNKNOWN
            this->doUNKOWN(argument);
            return;
        }

        bool found = false;

        try
        {
            uint16_t fileNum    = boost::lexical_cast<uint16_t>(generation);
            uint16_t subfileNum = boost::lexical_cast<uint16_t>(subfile);
            CPS_BUAP_BUFILE_RETCODE ret = CPS_BUAP_BUFILE_OK;

            // Only attach when subfile is R0
            // The file will be detach when the control connection is closed or failure
            // In theory, the connection will be closed after the subfile R5 is transferred.
            // This is the behavior only applicable for FTPC in IPNA
            if (subfileNum == 0)
            {
                m_bufile.reset(new CPS_BUAP_BUFile());
                ret = m_bufile->attachFile(fileNum);
            }

            if (ret == CPS_BUAP_BUFILE_OK)
            {
                if (m_bufile.get() == NULL)
                {
                    TRACE((LOG_LEVEL_WARN, "File %s hasn't been attached", 0, argument.c_str()));
                }
                else
                {
                    ret = m_bufile->openSubfile(subfileNum);
                    if (ret == CPS_BUAP_BUFILE_OK)
                    {
                        found = true;
                    }
                }
            }

            if (ret != CPS_BUAP_BUFILE_OK)
            {
                TRACE((LOG_LEVEL_ERROR, "Failed to Open file %s: - %s", 0, argument.c_str(), m_bufile->getLastErrorText()));
                EVENT((CPS_FTP_Events::datafile_open_fail, 0, argument.c_str(), ret, m_bufile->getLastErrorText()));

                // Detach the attached file & closed the opened subfile
                m_bufile->detachFile();
                m_bufile.reset();
            }
        }
        catch (boost::bad_lexical_cast &)
        {
            TRACE((LOG_LEVEL_ERROR, "Failed to Open file %s due to generation or subfile is not a number ", 0, argument.c_str()));
            found = false;
        }

        if (found == false)
        {
            reply = "550 File " + argument + " not found";
            // Require PORT, RETR commands again
            m_state = LOGGED;
            break;
        }

        string realSBC = string(m_bufile->getAttachedFileName()) + "-R" + subfile;
        TRACE((LOG_LEVEL_INFO, "Opened file %s by using buap api", 0, realSBC.c_str()));

        if (!this->openDataConnection())
        {
            reply = "425 Can't open data connection";
            m_state = LOGGED;

            // Detach the attached file & closed the opened subfile
            m_bufile->detachFile();
            m_bufile.reset();

            break;
        }

        reply = "150 Binary data connection for file " + realSBC + " opened";
        m_state = RETRIEVING;

        // Notify data connection available to server
        m_notifier(shared_from_this(), DATA_SOCKET_CONNECTED);

        // Start to transfer file data
        m_byteTransferred = 0;
		m_retryTimes = 0;
        this->transferData();
    }
    while (false);

    this->sendControlData(reply);

    TRACE((LOG_LEVEL_INFO, "RETR Reply: %s", 0, reply.c_str()));
}

/**
 * Handler of ABOR command
 *
 * @param argument The input argument
 * @return N/A
 */
void CPS_FTP_Connection::doABOR(const string& argument)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Connection::doABOR(%s)", 0, argument.c_str()));

    string reply = "221 Abort sucessful";

    if (argument != "")
    {
        reply = "501 Syntax error in parameters or arguments";
    }
    else if (m_state == RETRIEVING)
    {
        this->closeDataConnection();
    }

    if (m_state > LOGGED)
    {
        m_state = LOGGED;
    }

    this->sendControlData(reply);
}

/**
 * Handler of QUIT command
 *
 * @param argument The input argument
 * @return N/A
 */
void CPS_FTP_Connection::doQUIT(const string& argument)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Connection::doQUIT(%s)", 0, argument.c_str()));

    string reply = "";

    if (argument != "")
    {
        reply = "501 Syntax error in parameters or arguments";
    }
    else
    {
        reply = "221 Goodbye";
    }

    this->sendControlData(reply);

    if (argument == "")
    {
        // Stop the connection
        this->stop();
    }

    TRACE((LOG_LEVEL_INFO, "QUIT Reply: %s", 0, reply.c_str()));
}

/**
 * Handler of invalid command
 *
 * @param argument The input argument
 * @return N/A
 */
void CPS_FTP_Connection::doUNKOWN(const string& argument)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Connection::doUNKOWN(%s)", 0, argument.c_str()));

    this->sendControlData("502 Command not implemented");
}

/**
 * Handle incoming data from data socket
 *
 * @param N/A
 * @return N/A
 */
void CPS_FTP_Connection::handleDataPacket()
{
    newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Connection::HandleDataPacket()", 0));

    // Currently do nothing
}

/**
 * Send the reply to the peer (client)
 *
 * @param data The reply to be sent
 * @return N/A
 */
void CPS_FTP_Connection::sendControlData(const string& data)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Connection::sendControlData(%s)", 0, data.c_str()));

    string dataToSend = data + "\n";
    ssize_t len       = dataToSend.length();
    ssize_t index     = 0;
    ssize_t result    = 0;

    while (len > 0)
    {
        result = ::send(m_controlSocket, dataToSend.c_str() + index, len, MSG_NOSIGNAL);

        if (result == -1)
        {
            // The end point may be closed
            TRACE((LOG_LEVEL_ERROR, "Failed to send data: %d - %s", 0, errno, strerror(errno)));
            EVENT((CPS_FTP_Events::ctrlsock_send_fail, 0 , m_controlSocket, errno, strerror(errno)));

            return;
        }

        index += result;
        len    = len - result;
    }
}

/**
 * Subscribe notification handler for the connection change
 *
 * @param[in] subscriberHandler Handler (callback) to call when the connection has sth to notify
 * @return notification (or key) of the subscription
 */
Notification CPS_FTP_Connection::registerNotification(Notifier::slot_function_type subscriberHandler)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Connection::registerNotification()", 0));

    return m_notifier.connect(subscriberHandler);
}

/**
 * Un-subscribe notification handler
 *
 * @param[in] subscriber The key of the subscription to be removed
 * @return N/A
 */
void CPS_FTP_Connection::unregisterNotification(Notification subscriber)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Connection::unregisterNotification(Notification subscriber)", 0));

    subscriber.disconnect();
}

/**
 * Stop the connection
 *
 * @param  N/A
 * @return N/A
 * Note: This function should NOT be called by itself
 */
void CPS_FTP_Connection::stop()
{
    newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Connection::stop()", 0));

    if (m_bufile.get() != NULL)
    {
        // Close file & data connection
        m_bufile->detachFile();
        // Reset the bufile due to no use
        m_bufile.reset();
    }

    // Stop the data connection first
    if (m_dataSocket != -1)
    {
        this->closeDataConnection();
    }

    if (m_controlSocket != -1)
    {
        // Notify the connection closing event
        m_notifier(shared_from_this(), CONTROL_SOCKET_CLOSING);

        close(m_controlSocket);
        m_controlSocket = -1;
    }

    m_state = INIT;
}
