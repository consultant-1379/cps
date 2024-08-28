//*****************************************************************************
// COPYRIGHT Ericsson Utvecklings AB, Sweden 1999, 2011
// All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson Utvecklings AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.
//
//  NAME
//  CPS_BUAP_LoaderClient.cpp     
//
//  DESCRIPTION 
//  Interface for clients towards the m_conversation server
//
//  DOCUMENT NO
//  CXA 110 4958
//
//  AUTHOR 
//  990806 UAB/B/SF Birgit Berggren
//
//*****************************************************************************
// === Revision history ===
// 990806 BIR     Created.
// 090819 XTBAKLU Added support for new JTP/DSD, on APZ17
//                When initiating a JTP connection the destination node must be 
//                specified to the same node (THISNODE = AP) , otherwise JTP/DSD will 
//                default to the MASTER CP.
// 101215 xchihoa Port to Linux for APG43Ls
// 110809 xquydao Updated for APG43L
//*****************************************************************************

#include "CPS_BUAP_LoaderClient.h"
#include "CPS_BUAP_Loader_MsgIDs.h"
#include "trace.h"

#include <cassert>
#include <cstdio>

#define STR_DESTINATOR_NAME  "BULOADER"
#define JTPBUFSIZE           512

//*****************************************************************************
//      CPS_BUAP_LoaderClient()
//*****************************************************************************
CPS_BUAP_LoaderClient::CPS_BUAP_LoaderClient() :
   m_conversation((char*) STR_DESTINATOR_NAME, JTPBUFSIZE)
{
    newTRACE(("CPS_BUAP_LoaderClient::CPS_BUAP_LoaderClient()", 0));
    m_lastErrorText[0] = '\0';
}

//*****************************************************************************
//      connect()
//*****************************************************************************
bool CPS_BUAP_LoaderClient::connect()
{
    newTRACE(("CPS_BUAP_LoaderClient::connect()", 0));
    ushort ru1 = 0, ru2 = 0, r = 0;

    // Create a Node to be used with jexinitreq(). This is needed in a system
    // running the DSD2 since the default behavior is to connect to the MASTER CP.
    // Lets connect to the JTP-BULOADER published on THIS AP.
    ACS_JTP_Conversation::JTP_Node node;
    node.system_id = ACS_JTP_Conversation::SYSTEM_ID_THIS_NODE;

    // Initialize the connection
    if (!m_conversation.jexinitreq(&node, CPS_BUAP_Loader_MsgId_ConnectClient, 0))
    {
        m_conversation.jexdiscind(ru1, ru2, r); // Gets error

        TRACE(("Failed to connect to buloader. jexdiscind: %d, %d, %d", 0, ru1, ru2, r));
        sprintf(m_lastErrorText, "LoaderClient: Failed to connect to buloader. jexdiscind: %d, %d, %d", ru1, ru2, r);

        return false;
    }

    m_conversation.setTimeOut(30);
    m_conversation.jexinitconf(ru1, ru2, r);

    return true;
}

//*****************************************************************************
//      disconnect()
//*****************************************************************************
void CPS_BUAP_LoaderClient::disconnect()
{
    newTRACE(("CPS_BUAP_LoaderClient::disconnect()", 0));

    if (m_conversation.jexdiscreq(0, 0, 0) == false)
    {
        TRACE(("Disconnect failed.", 0));
        sprintf(m_lastErrorText, "LoaderClient: %s", "Failed to disconnect to buloader");
    }
}

//*****************************************************************************
//      isConnected()
//*****************************************************************************
bool CPS_BUAP_LoaderClient::isConnected()
{
    newTRACE(("CPS_BUAP_LoaderClient::isConnected()", 0));

    return (m_conversation.State() >= ACS_JTP_Conversation::StateConnected) ? true : false;
}

//*****************************************************************************
// getFileName()
//*****************************************************************************
bool CPS_BUAP_LoaderClient::getFileName(char* fileName) // fileName: in/out
{
    newTRACE(("CPS_BUAP_LoaderClient::getFileName(%s)", 0, fileName));

    ushort ru1 = 0, ru2 = 0, rMsgLen = 0;
    char* rMsg = 0;

    if (!sendJTPReq(CPS_BUAP_Loader_MsgId_getFn, 0, strlen(fileName) + 1, fileName, ru1, ru2, rMsgLen, rMsg))
    {
        TRACE(("Failed to send JTP req for file name", 0));
        return false;
    }

    switch (ru1)
    {
    case CPS_BUAP_Loader_MsgId_getFn_ok:
        TRACE(("Got file name <%s> U1: %d", 0, rMsg, ru1));
        break;

    case CPS_BUAP_Loader_MsgId_getFn_failed:
    default:
        TRACE(("Failed to obtain file name from buloader. U1: %d", 0, ru1));
        return false;
        break;
    }

    strcpy(fileName, rMsg);
    return true;
}

//*****************************************************************************
//      reportCmdLog()
//*****************************************************************************
bool CPS_BUAP_LoaderClient::reportCmdLog(R0Info &data)
{
    newTRACE(("CPS_BUAP_LoaderClient::reportCmdLog()", 0));

    ushort ru1 = 0, ru2 = 0, rMsgLen = 0;
    char* rMsg = 0;

    char msg[JTPBUFSIZE];
    memcpy(msg, &data, sizeof(data));
    ushort msgLen = sizeof(data);

    if (!sendJTPReq(CPS_BUAP_Loader_MsgId_reportCmdLog, 0, msgLen, msg, ru1, ru2, rMsgLen, rMsg))
    {
        TRACE(("Failed to send JTP req for cmd log report", 0));
        return false;
    }

    switch (ru1)
    {
    case CPS_BUAP_Loader_MsgId_reportCmdLog_ok:
        break;

    case CPS_BUAP_Loader_MsgId_reportCmdLog_failed:
    default:
        TRACE(("Failed to report command log. err: %d %d", 0, ru1, ru2));
        return false;
    }

    return true;
}

//*****************************************************************************
//      sendJTPReq()
//*****************************************************************************
bool CPS_BUAP_LoaderClient::sendJTPReq(ushort u1, ushort u2, ushort msgLen,
      char* msg, ushort& ru1, ushort& ru2, ushort& rMsgLen, char*& rMsg)
{
    newTRACE(("CPS_BUAP_LoaderClient::sendJTPReq()", 0));

    //  Send conversation data
    if (!m_conversation.jexdatareq(u1, u2, msgLen, msg))
    {
        TRACE(("Failed to send JTP request. jexdatareq: %d, %d, %d, <%s>", 0, u1, u2, msgLen, msg));
        sprintf(m_lastErrorText, "LoaderClient: Failed to send JTP request. jexdatareq: %d, %d, %d, <%s>", u1, u2, msgLen, msg);
        return 0;
    }

    fd_set rfds;
    timeval timeout;
    int hJTPConversation = m_conversation.getHandle();

    FD_ZERO(&rfds);
    FD_SET(hJTPConversation, &rfds);
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    int pollResult = select(hJTPConversation + 1, &rfds, NULL, NULL, &timeout);

    if (pollResult == -1) //FAILED
    {
        //RepEvent(g_bufilelog, EVENTLOG_WARNING_TYPE, "JTP Wait failed: %d", pollResult);
        TRACE(("Failed to select: %d", 0, errno));
        sprintf(m_lastErrorText, "LoaderClient: Failed to select in sendJTPReq(%d, %d, %d, <%s>). Error: %d",
                u1, u2, msgLen, (msgLen == 0) ? "NULL" : msg, errno);
        return false;
    }
    else if (pollResult == 0) //TIMEOUT
    {
        TRACE(("Select time out", 0));
        sprintf(m_lastErrorText, "LoaderClient: Failed to select in sendJTPReq(%d, %d, %d, <%s>). Error: timeout",
                u1, u2, msgLen, (msgLen == 0) ? "NULL" : msg);
        return false;
    }
    else if (!FD_ISSET(hJTPConversation, &rfds))
    {
        TRACE(("Select return with unrecognized event!!!", 0));
        sprintf(m_lastErrorText, "LoaderClient: Failed to select in sendJTPReq(%d, %d, %d, <%s>). Error: unrecognized event",
                u1, u2, msgLen, (msgLen == 0) ? "NULL" : msg);

        return false;
    }

    //*************************************************************************
    // Successful, continue execution.
    //*************************************************************************
    //  Read answer
    if (m_conversation.jexdataind(ru1, ru2, rMsgLen, rMsg) == false)
    {
        ushort r = 0;
        m_conversation.jexdiscind(ru1, ru2, r); // Gets error

        TRACE(("Failed to receive JTP reply. jexdiscind: %d, %d, %d", 0, ru1, ru2, r));
        sprintf(m_lastErrorText, "LoaderClient: Failed to receive JTP reply in select in sendJTPReq(%d, %d, %d, <%s>). jexdiscind: %d, %d, %d",
                u1, u2, msgLen, (msgLen == 0) ? "NULL" : msg, ru1, ru2, r);

        return false;
    }

    return true;
}

//*****************************************************************************
//      getLastErrorText()
//*****************************************************************************
const char* CPS_BUAP_LoaderClient::getLastErrorText()
{
    return m_lastErrorText;
}
