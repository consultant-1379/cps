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
//  CPS_BUAP_Server.cpp
//
//  DESCRIPTION 
//  This class multiplexes JTP conversations. It does this by
//  maintaining a list of active conversations. Each conversation
//  in the list is associated with a JTP handle. 
//  The function WaitForMultipleObjects is called.
//  The class also syncronizes the threads within the process. 
//
//  DOCUMENT NO
//  190 89-CAA 109 1412
//
//  AUTHOR 
// 	1999/05/07 by UAB/B/SF Birgit Berggren
//
//
//******************************************************************************
// === Revision history ===
// 990513 BIR PA1 Created.
// 990607 BIR PA2 Updated.
// 990617 BIR PA3 Updated after review.
// 990906 BIR PA4 Correction of the Poll routine: finding signaled JTP handle.
// 991105 BIR PA5 Correction of TRACE printout.
// 101010 xchihoa     Ported to Linux for APG43L.
//******************************************************************************
#include "CPS_BUAP_Linux.h"

#include <stdio.h>
#include <stdlib.h>
#include <cerrno>

#include "CPS_BUAP_Trace.h"
#include "CPS_BUAP_Events.h"
#include "CPS_BUAP_Exception.h"

#include "CPS_BUAP_Server.h"

using namespace std;
//******************************************************************************
//	CPS_BUAP_Server()
//******************************************************************************
CPS_BUAP_Server::CPS_BUAP_Server(): noOfNTHandles(0), m_deriveName("")
{
	// Change for Tracing Order
    //newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_Server::CPS_BUAP_Server()", 0));

    for (int i = 0; i < MAX_NT_OBJECTS; ++i)
    {
        hNTObjectArray[i] = -1;
    }
}

//******************************************************************************
//	~CPS_BUAP_Server()
//******************************************************************************
CPS_BUAP_Server::~CPS_BUAP_Server()
{
	// Change for Tracing Order
    //newTRACE((LOG_LEVEL_INFO, "%s - CPS_BUAP_Server::~CPS_BUAP_Server()", 0, m_deriveName.c_str()))

    if (!m_ConversationList.empty())
    {
        // delete all conversations from the list
        //TRACE(("%s - Delete all conversation from the list", 0, m_deriveName.c_str()));
        for (CPS_BUAP_Conversation_LIST_ITERATOR iConversation =
                m_ConversationList.begin(); iConversation
                != m_ConversationList.end(); ++iConversation)
        {
            //delete *iConversation;
	    (*iConversation).reset();
        }
    }

    m_ConversationList.clear();
}

//******************************************************************************
//	AddConversation()
//******************************************************************************
void CPS_BUAP_Server::AddConversation(CPS_BUAP_Conversation_Ptr conversation)
{
    newTRACE((LOG_LEVEL_INFO, "%s - CPS_BUAP_Server::AddConversation() on handle %x", 0, m_deriveName.c_str(), conversation->m_handle))

    m_ConversationList.push_back(conversation);
    TRACE((LOG_LEVEL_INFO, "%s - CPS_BUAP_Server::AddConversation() end", 0, m_deriveName.c_str()));
}

//******************************************************************************
//	NewConversation()
//******************************************************************************
void CPS_BUAP_Server::NewConversation(CPS_BUAP_Conversation_Ptr l_pConversation)
{
    newTRACE((LOG_LEVEL_INFO, "%s - CPS_BUAP_Server::NewConversation() on handle %x", 0, m_deriveName.c_str(), l_pConversation->m_handle))

    //ACS_JTP_Conversation *l_pJTPconversation = new ACS_JTP_Conversation;
    ACS_JTP_Conversation_Ptr l_pJTPconversation(new ACS_JTP_Conversation);
    unsigned short U1 = 0;
    unsigned short U2 = 0;

    TRACE((LOG_LEVEL_INFO, "%s - calling JTP_Service.accept", 0, m_deriveName.c_str()))
    l_pConversation->m_pJTPservice->accept(NULL, l_pJTPconversation.get());

    if (l_pJTPconversation->State() == ACS_JTP_Conversation::StateConnected)
    {

        TRACE((LOG_LEVEL_INFO, "%s - calling jexinitind", 0, m_deriveName.c_str()))
        if (!l_pJTPconversation->jexinitind(U1, U2))
        {
            EVENT((CPS_BUAP_Events::jexinitind_fail, 0))
            //delete l_pJTPconversation;
        }
        else
        {
            TRACE((LOG_LEVEL_INFO, "%s - calling jexinitrsp", 0, m_deriveName.c_str()))
            if (!l_pJTPconversation->jexinitrsp(U1, U2, 0))
            {
                EVENT((CPS_BUAP_Events::jexinitrsp_fail, 0))
                //delete l_pJTPconversation;
            }
            else
            {
                // successfully sent confirmation
                CPS_BUAP_Conversation_Ptr l_pNewConversation(new CPS_BUAP_Conversation(
                        l_pJTPconversation->getHandle(),
                        l_pConversation->m_pJTPservice, l_pJTPconversation));
                AddConversation(l_pNewConversation);
                //m_ConversationList.push_back(l_pNewConversation);

            }
        }
    }
    TRACE((LOG_LEVEL_INFO, "%s - CPS_BUAP_Server::NewConversation() end", 0, m_deriveName.c_str()))
}

//******************************************************************************
//	OldConversation()
//******************************************************************************
void CPS_BUAP_Server::OldConversation(CPS_BUAP_Conversation_Ptr l_pConversation)
{
    newTRACE((LOG_LEVEL_INFO, "%s - CPS_BUAP_Server::OldConversation() for handle %x", 0, m_deriveName.c_str(), l_pConversation->m_handle))

    unsigned short rxUserData1 = 0;
    unsigned short rxUserData2 = 0;
    unsigned short rxLength;
    char *rxUserBuffer;

    unsigned short txUserData1 = 0;
    unsigned short txUserData2 = 0;

    TRACE((LOG_LEVEL_INFO, "%s - calling jexdataind", 0, m_deriveName.c_str()))
    if (!l_pConversation->m_pJTPconversation->jexdataind(rxUserData1,
            rxUserData2, rxLength, rxUserBuffer))
    {
        TRACE((LOG_LEVEL_INFO, "%s - conversation ended on handle %x", 0, m_deriveName.c_str(), l_pConversation->m_handle))

        m_ConversationList.remove(l_pConversation);
        //delete l_pConversation;

    }
    else
    {
        CPS_BUAP_Buffer rxBuffer(rxLength, rxUserBuffer);
        CPS_BUAP_Buffer txBuffer(256);

        RXTX(l_pConversation->m_handle, rxUserData1, rxUserData2, rxBuffer,
                txUserData1, txUserData2, txBuffer);

        TRACE((LOG_LEVEL_INFO, "%s - calling jexdatareq(UserData1=%d, UserData2=%d, Length=%d, ptr)",
                        0, m_deriveName.c_str(), txUserData1, txUserData2, txBuffer.Length()))

        if (!l_pConversation->m_pJTPconversation->jexdatareq(txUserData1,
                txUserData2, txBuffer.Length(), txBuffer.Data()))
        {
            EVENT((CPS_BUAP_Events::jexdatareq_fail, 0))

            m_ConversationList.remove(l_pConversation);
            //delete l_pConversation;
        }
    }
}

//******************************************************************************
//	Poll()
//******************************************************************************
DWORD CPS_BUAP_Server::Poll()
{
    newTRACE((LOG_LEVEL_INFO, "%s - CPS_BUAP_Server::Poll()", 0, m_deriveName.c_str()))

    CPS_BUAP_Conversation_LIST_ITERATOR l_iter = m_ConversationList.begin();

    DWORD l_entries;
    int pollResult;
    HANDLE hTempArray[MAX_TEMP_OBJECTS] = {-1};

    try
    {

        l_entries = m_ConversationList.size();
        int iTotalHandles = l_entries + noOfNTHandles;

        //****************************************************************************
        // Fetch thread control handles.
        //****************************************************************************
        for (DWORD i = 0; i < noOfNTHandles; i++)
            hTempArray[i] = hNTObjectArray[i];

        //****************************************************************************
        // Fetch the JTP handles.
        //****************************************************************************
        for (DWORD i = 0; i < l_entries; i++, l_iter++)
            hTempArray[noOfNTHandles + i] = (*l_iter)->m_handle;

        //****************************************************************************
        // Wait until one of the objects has been signaled.
        //****************************************************************************

        int iMaxFD = 0;
        fd_set rfds;
        FD_ZERO(&rfds);

        for (int i = 0; i < iTotalHandles; i++)
        {
            FD_SET(hTempArray[i], &rfds);
            if (iMaxFD < hTempArray[i])
                iMaxFD = hTempArray[i];
        }

        TRACE((LOG_LEVEL_INFO, "%s - calling select() with %d NThandles and %d JTPhandles", 0, m_deriveName.c_str(), noOfNTHandles, l_entries));

        pollResult = select(iMaxFD + 1, &rfds, NULL, NULL, NULL);

        TRACE((LOG_LEVEL_INFO, "%s - select() return with pollResult = %d, iMaxFD = %d, iTotalHandles = %d", 0, m_deriveName.c_str(),
                        pollResult, iMaxFD, iTotalHandles));

        if (pollResult == -1)
        {
            //TODO: check the returned error.
            if (errno == EINTR)
            {
                return NO_ERROR;
            }

            //********************************************************************
            // Failure, send event and terminate thread.
            //********************************************************************
            EVENT((CPS_BUAP_Events::WaitForMultipleObjects_fail, 0, errno));
            TRACE((LOG_LEVEL_ERROR, "%s - calling select() failed with error = %s", 0, m_deriveName.c_str(), strerror(errno)));
            THROW("select() failed");
        }

        //***********************************************************************
        // Successful, find the signaled object.
        //***********************************************************************
        for (int i = 0; i < iTotalHandles; i++)
        {
            if (FD_ISSET(hTempArray[i],&rfds) && (i < (int) noOfNTHandles))
            {
                //********************************************************************
                // It's one of the thread control objects.
                //********************************************************************
                return (Process_thread_control(hTempArray[i]));
            }
            else if (FD_ISSET(hTempArray[i],&rfds))
            {
                //********************************************************************
                // It's one of the JTP handles, find out whether it's a server handle
                // or a conversation handle.
                //********************************************************************
                CPS_BUAP_Conversation_LIST_ITERATOR l_iterTmp;
                l_iter = m_ConversationList.begin();
                for (DWORD j = 0; j < l_entries; j++)
                {

                    l_iterTmp = l_iter;
                    l_iter++;

                    if (i == (int) (noOfNTHandles + j))
                    {
                        //*********************************************************************
                        // Check if this is a server (server has no JTP conversation).
                        //*********************************************************************
                        if ((*l_iterTmp)->m_pJTPconversation.get() == 0)
                        {
                            NewConversation(*l_iterTmp);
                        }
                        else
                        {
                            OldConversation(*l_iterTmp);
                        }
                    }
                }
            }
        }

        //************************************************************************
        // None of the handles has to be reset manually.
        //************************************************************************

    } //try


    catch (...)
    {
        TRACE((LOG_LEVEL_WARN, "%s - CPS_BUAP_Server::Poll(): catching some error!!!", 0, m_deriveName.c_str()));
        EVENT((CPS_BUAP_Events::death_of_server, 0));
        THROW("Death of Server");
        //return ERROR_EXCEPTION;
    }

    return NO_ERROR;

} //Poll


//******************************************************************************
//	OnDisconnect()
//******************************************************************************
/* xchihoa: This is a virtual function, no need to implement it with empty {}
 void CPS_BUAP_Server::OnDisconnect(HANDLE handle)
 {
 newTRACE(("CPS_BUAP_Server::OnDisconnect()", 0))

 //
 // This routine should be overloaded in a derived class to be useful.
 // Use the parameter to stop unused warning
 HANDLE l_handle = handle;
 }
 */
