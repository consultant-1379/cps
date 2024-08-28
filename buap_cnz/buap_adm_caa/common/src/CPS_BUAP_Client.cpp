//******************************************************************************
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 1999.
//  All rights reserved.
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
//  CPS_BUAP_Client.cpp 
// 
//  DESCRIPTION 
//	This class implements the interface to an application based on the
//      CPS_BUAP_Server class.
//	When instantiated it connects to the destinator given
//	in the constructor.  Messages may then be sent and responses
//	received via the RXTX routine.
//
//	The destructor when called will end the conversation.
//
//  DOCUMENT NO
//  190 89-CAA 109 1412
//
//  AUTHOR 
// 	1999/07/17 by UAB/B/SF Birgit Berggren
// 101010 xchihoa     Ported to Linux for APG43L.
//
//******************************************************************************
// === Revision history ===
// 990717 BIR PA1 Created.
// 061003 uablan  Added [] after delete in destructor.
// 090819 XTBAKLU Added support for new JTP/DSD, on APZ17
//                When initiating a JTP connection the destination node must be 
//                specified to the same node (THISNODE = AP) , otherwise JTP/DSD will 
//                default to the MASTER CP.
//******************************************************************************
#include "CPS_BUAP_Linux.h"

#include <string.h>
#include <cerrno>

#include "CPS_BUAP_Events.h"
#include "CPS_BUAP_Trace.h"
#include "CPS_BUAP_Client.h"

//******************************************************************************
//	CPS_BUAP_Client()
//******************************************************************************
CPS_BUAP_Client::CPS_BUAP_Client(const char *destinatorName, int timeoutValue,
        int numRetries)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_Client::CPS_BUAP_Client(%s, %d, %d)",
                    0, destinatorName, timeoutValue, numRetries))

    unsigned short l_dummy = 0;
    unsigned short l_userData1 = 0;
    unsigned short l_userData2 = 0;

    m_NumRetries = numRetries;
    m_TimeoutValue = timeoutValue;
    m_DestinatorName = new char[strlen(destinatorName) + 1];

    strcpy(m_DestinatorName, destinatorName);

    //m_pConversation = new ACS_JTP_Conversation(m_DestinatorName, 256);
    m_pConversation.reset(new ACS_JTP_Conversation(m_DestinatorName, 256));

    m_pConversation->setTimeOut(m_TimeoutValue);
    m_pConversation->setNoOfTries(m_NumRetries);

    // Create a Node to be used with jexinitreq(). This is needed in a system
    // running the DSD2 since the default behavior is to connect to the MASTER CP.
    // Lets connect to the JTP-BULOADER published on THIS AP.
    ACS_JTP_Conversation::JTP_Node pNode;
    //pNode = new ACS_JTP_Conversation::JTP_Node;
    pNode.system_id = ACS_JTP_Conversation::SYSTEM_ID_THIS_NODE;

    //****************************************************************************
    // Request conversation initiation.
    //****************************************************************************
    TRACE((LOG_LEVEL_INFO, "calling jexinitreq", 0))
    if (!m_pConversation->jexinitreq(&pNode, l_userData1, l_userData2))
    {
       TRACE((LOG_LEVEL_ERROR, "ERROR: cannot connect to %s", 0, m_DestinatorName));
        EVENT((CPS_BUAP_Events::jexinitreq_fail, 0, m_DestinatorName))
        //delete pNode;
        THROW("jexinitreq failed");
    }

    //*************************************************************************
    // Fetch data from conversation initiation confirmation.
    //*************************************************************************
    TRACE((LOG_LEVEL_INFO, "calling jexinitconf", 0))
    if (!m_pConversation->jexinitconf(l_userData1, l_userData2, l_dummy))
    {
       TRACE((LOG_LEVEL_ERROR, "ERROR: jexinitconf failed on %s", 0, m_DestinatorName));
        EVENT((CPS_BUAP_Events::jexinitconf_fail, 0, m_DestinatorName))
        //delete pNode;
        THROW("jexinitconf failed");
    }

    //if (pNode)
    //{
    //    delete pNode;
    //    pNode = 0;
    //}
}

//******************************************************************************
//	~CPS_BUAP_Client()
//******************************************************************************
CPS_BUAP_Client::~CPS_BUAP_Client()
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_Client::~CPS_BUAP_Client()", 0))

    //TODO:
#if 0
    if (m_pConversation.get())
    {
	TRACE((LOG_LEVEL_INFO, "CPS_BUAP_Client - Handle = %d", 0, (int) m_pConversation->getHandle()));
	if (m_pConversation->State() >= ACS_JTP_Conversation::StateConnected)
 	{
	    if(m_pConversation->jexdiscreq(0, 0, 0))
	    	TRACE((LOG_LEVEL_INFO, "CPS_BUAP_Client::~CPS_BUAP_Client() - call jexdiscreq", 0))
	}
    }
#endif
/*
    if(m_pConversation)
    {
       delete m_pConversation;
       m_pConversation = 0;
    }
*/
    if(m_DestinatorName)
    { 
	delete[] m_DestinatorName;
	m_DestinatorName = 0;
    }
}

//******************************************************************************
//	TXRX()
//******************************************************************************
void CPS_BUAP_Client::TXRX(CPS_BUAP_MSG& txMsg, CPS_BUAP_MSG& rxMsg)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_Client::TXRX()", 0))

    unsigned short l_userData1 = 0;
    unsigned short l_userData2 = 0;
    CPS_BUAP_Buffer l_txBuffer(256);

    unsigned short l_rxLength;
    char *l_rxData;

    int pollResult;
    HANDLE hJTPConversation;

    txMsg.Encode(l_txBuffer);

    //****************************************************************************
    // Fetch JTP handle for usage in select().
    //****************************************************************************
    hJTPConversation = m_pConversation->getHandle();

    //****************************************************************************
    // Send data.
    //****************************************************************************
    TRACE((LOG_LEVEL_INFO, "calling jexdatareq", 0))
    if (!m_pConversation->jexdatareq(txMsg.UserData1(), txMsg.UserData2(),
          l_txBuffer.Length(), l_txBuffer.Data()))
    {
        EVENT((CPS_BUAP_Events::jexdatareq_fail, 0, m_DestinatorName))
        THROW("jexdatareq failed");
    }

    //****************************************************************************
    // Wait for reply.
    //****************************************************************************
    TRACE((LOG_LEVEL_INFO, "calling select() with %ums timeout", 0, 15000))

    fd_set rfds;
    timeval    timeout;

    FD_ZERO(&rfds);
    FD_SET(hJTPConversation, &rfds);
    timeout.tv_sec = 15;
    timeout.tv_usec = 0;

    pollResult = select(hJTPConversation + 1, &rfds, NULL, NULL, &timeout);

    if (pollResult == -1)   //FAILED
    {
        //*************************************************************************
        // Unsuccessful, disconnect conversation and terminate.
        //*************************************************************************
        EVENT((CPS_BUAP_Events::WaitForSingleObject_fail, 0,m_DestinatorName, errno))

        if (!m_pConversation->jexdiscreq(l_userData1, l_userData2, 0))
        {
            EVENT((CPS_BUAP_Events::jexdiscreq_fail, 0, m_DestinatorName))
            THROW("jexdiscreq failed");
        }

        THROW("select() failed");
    }

    else if (pollResult == 0)    //TIMEOUT
    {
        //*************************************************************************
        // Unsuccessful, diconnect conversation and terminate.
        //*************************************************************************
        EVENT((CPS_BUAP_Events::WaitForSingleObject_timeout, 0, m_DestinatorName, 15000))

        if (!m_pConversation->jexdiscreq(l_userData1, l_userData2, 0))
        {
            EVENT((CPS_BUAP_Events::jexdiscreq_fail, 0, m_DestinatorName))
            THROW("jexdiscreq failed");
        }

        THROW("select() failed");
    }

    else if( !FD_ISSET(hJTPConversation, &rfds) )
    {
        //*************************************************************************
        // Something exceptional happened, diconnect conversation and terminate.
        //*************************************************************************
        EVENT((CPS_BUAP_Events::WaitForSingleObject_fail, 0,m_DestinatorName, errno))

        if (!m_pConversation->jexdiscreq(l_userData1, l_userData2, 0))
        {
            EVENT((CPS_BUAP_Events::jexdiscreq_fail, 0, m_DestinatorName))
            THROW("jexdiscreq failed");
        }

        THROW("select() failed");
    }

    //*************************************************************************
    // Successful, continue execution.
    //*************************************************************************


    //****************************************************************************
    // Receive conversation data.
    //***************************************************************************
    TRACE((LOG_LEVEL_INFO, "calling jexdataind", 0))
    if (!m_pConversation->jexdataind(rxMsg.UserData1(), rxMsg.UserData2(),
            l_rxLength, l_rxData))
    {
        EVENT((CPS_BUAP_Events::jexdataind_fail, 0, m_DestinatorName))
        THROW("jexdataind failed");
    }

    CPS_BUAP_Buffer l_rxBuffer(l_rxLength, l_rxData);

    rxMsg.Decode(l_rxBuffer);
}
