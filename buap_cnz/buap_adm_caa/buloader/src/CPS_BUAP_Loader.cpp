//*****************************************************************************
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
//  CPS_BUAP_Loader.cpp
//
//  DESCRIPTION 
//  Main object for the loader process. 
//  Manages CP reload attempts. Operates upon JTP msgs.
//  The class also syncronizes the threads within the process.
//
//  DOCUMENT NO
//  190 89-CAA 109 1412
//
//  AUTHOR 
//  990913 UAB/B/SF Birgit Berggren
//	 010327 UAB/Y/SF Birgit Berggren
//
//*****************************************************************************
// === Revision history ===
// 990913 BIR PA1 Created.
// 991105 BIR PA2 Correction of TRACE printout.
// 010327 BIR PB1 Hanging after reload from MS.
// 070625 LAN     Try to connect/register to JTP/DSD until connected. (HI11967)
// 101215 xchihoa  Port to Linux for APG43L
// 120103 xngudan Updated after reviewing
//*****************************************************************************


#include "CPS_BUAP_Loader.h"
#include "CPS_BUAP_Types.h"                 // R0Info
#include "CPS_BUAP_Loader_MsgIDs.h"
#include "CPS_BUAP_Trace.h"
#include "CPS_BUAP_Events.h"
#include "CPS_BUAP_Loader_Global.h"
#include "CPS_BUAP_Linux.h"
#include "CPS_BUAP_Service.h"

#include <string.h>

//=============================================================================
// Extern variable used to report error.
//=============================================================================
extern CPS_BUAP_Service* pService;
extern bool reportErrorFlag;
extern boost::recursive_mutex mutex;

using namespace std;

//*****************************************************************************
//	CPS_BUAP_Loader()
//*****************************************************************************
CPS_BUAP_Loader::CPS_BUAP_Loader() :
    m_pService(0), loadSession(0), noOfNTHandles(0), m_running(false)
{
	// Change for Tracing Order
    //newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_Loader::CPS_BUAP_Loader()", 0));

    //***************************************************************************
    // Initiate the array of thread control objects.
    //***************************************************************************
    //TRACE(("initiating hNTObjectArray(BULOADER)", 0));
    hNTObjectArray[ABORT_EVENT] = m_stopEvent.getFd(); // There's always a
    noOfNTHandles = 1;                         // handle to the AbortEvent

    m_ConversationList.clear();

}

//*****************************************************************************
//	~CPS_BUAP_Loader()
//*****************************************************************************
CPS_BUAP_Loader::~CPS_BUAP_Loader()
{
	// Change for Tracing Order
    //newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_Loader::~CPS_BUAP_Loader()", 0));

    // Clear the conversation list
    if (!m_ConversationList.empty())
    {
        CPS_BUAP_Conversation_LIST_ITERATOR l_iter;
        for (l_iter = m_ConversationList.begin(); l_iter
                != m_ConversationList.end(); l_iter++)
        {
            //delete *l_iter;
	    (*l_iter).reset();
            //TRACE(("~CPS_BUAP_Loader delete l_iter", 0));
        }
    }

    m_ConversationList.clear();

    if (m_pService != 0)
    {
        delete m_pService;
        m_pService = 0;
        //TRACE(("~CPS_BUAP_Loader delete m_pService", 0));
    }
    if (loadSession != 0)
    {
        delete loadSession;
        loadSession = 0;
        //loadSession.reset();
        //TRACE(("~CPS_BUAP_Loader delete loadSession", 0));
    }
    //TRACE(("End of ~CPS_BUAP_Loader", 0));
}

//******************************************************************************
//	addConversation()
//******************************************************************************
void CPS_BUAP_Loader::addConversation(CPS_BUAP_Conversation_Ptr conversation)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_Loader::addConversation() on handle %x", 0, conversation->m_handle));

    m_ConversationList.push_back(conversation);
}

//*****************************************************************************
//	run()
//*****************************************************************************
void CPS_BUAP_Loader::Run()
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_Loader::Run()", 0));

    // Use condition variable to notify other thread that server is started
    {
        boost::lock_guard<boost::mutex> lock(m_mutex);
        m_running = true;
    }
    m_condition.notify_one();

    try
    {
        // Start ACS_JTP Service
        if (!this->initiateJTPService())
        {
            m_running = false;
            TRACE((LOG_LEVEL_INFO, "CPS_BUAP_Loader server stopped", 0));
            return;
        }

        TRACE((LOG_LEVEL_INFO, "CPS_BUAP_Loader server started", 0));

        while (!poll())
            ;

    }
    catch (...)
    {
        TRACE((LOG_LEVEL_ERROR, "BUAP_Loader runs with Exception!!!", 0));

        {
            // Lock the session to protect
            boost::recursive_mutex::scoped_lock scoped_lock(mutex);
            if (pService && (!reportErrorFlag))
            {
                TRACE((LOG_LEVEL_FATAL, "LOADER -Report error to AMF.", 0));
                reportErrorFlag = true;
                pService->componentReportError(ACS_APGCC_COMPONENT_RESTART);
            }
        }
    }

    m_running = false;
    TRACE((LOG_LEVEL_INFO, "CPS_BUAP_Loader server stopped gracefully", 0));
}
//*****************************************************************************
//	poll()
//*****************************************************************************
DWORD CPS_BUAP_Loader::poll()
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_Loader::poll()", 0));

    CPS_BUAP_Conversation_LIST_ITERATOR l_iter = m_ConversationList.begin();

    DWORD l_entries;
    int pollResult;
    HANDLE hTempArray[MAX_TEMP_OBJECTS];

    try
    {
        l_entries = m_ConversationList.size();

        //************************************************************************
        // Fetch thread control handles.
        //************************************************************************
        for (DWORD i = 0; i < noOfNTHandles; i++)
            hTempArray[i] = hNTObjectArray[i];

        //********************************************************************
        // Fetch the JTP handles.
        //********************************************************************
        for (DWORD j = 0; j < l_entries; j++, l_iter++)
        {
            hTempArray[noOfNTHandles + j] = (*l_iter)->m_handle;
        }

        //***********************************************************************
        // Wait until one of the objects has been signaled.
        //***********************************************************************
        int iMaxFD = 0;
        fd_set rfds;
        FD_ZERO(&rfds);

        for (DWORD k = 0; k < noOfNTHandles + l_entries; k++)
        {
            FD_SET(hTempArray[k], &rfds);
            if (iMaxFD < hTempArray[k])
                iMaxFD = hTempArray[k];
        }

        pollResult = select(iMaxFD + 1, &rfds, NULL, NULL, NULL);

        TRACE((LOG_LEVEL_INFO, "select() return with pollResult = %d: some event fired", 0, pollResult));

        if (pollResult == -1) //WAIT_FAILED)
        {
            if (errno == EINTR)
            {
                return NO_ERROR;
            }
            //********************************************************************
            // Failure, send event and terminate thread.
            //********************************************************************
            EVENT((CPS_BUAP_Events::WaitForMultipleObjects_fail, 0, errno));
            TRACE((LOG_LEVEL_ERROR, "BULOADER - calling select() failed with error = %s", 0, strerror(errno)));
            THROW("select() failed");
        }

        //***********************************************************************
        // Successful, find the signaled object.
        //***********************************************************************
        for (int i = 0; i < (int) (noOfNTHandles + l_entries); i++)
        {
            if (FD_ISSET(hTempArray[i],&rfds))
            {
                if (i < (int) noOfNTHandles)
                {
                    //********************************************************************
                    // It's one of the thread control objects.
                    //********************************************************************
                    return (Process_thread_control(hTempArray[i]));
                }
                else
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
                            if ((*l_iterTmp)->m_pJTPconversation == 0)
                            {
                                newConversation(*l_iterTmp);
                            }
                            else
                            {
                                receiveMsg(*l_iterTmp);
                            }
                        }
                    }
                }
            } // if( FD_ISSET(hTempArray[i],&rfds) )
        } //for( int i = 0; i <= (int)(noOfNTHandles + l_entries); i++)
    } //try

    catch (...)
    {
        THROW("Death of Server");
    }

    return NO_ERROR;

} // poll

//*****************************************************************************
//	newConversation()
//*****************************************************************************
void CPS_BUAP_Loader::newConversation(CPS_BUAP_Conversation_Ptr l_pConversation)

{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_Loader::newConversation() on handle %x", 0, l_pConversation->m_handle));

    CPS_BUAP_Conversation_LIST_ITERATOR l_iter = m_ConversationList.begin();

    //****************************************************************************
    // Create a conversation object.
    //****************************************************************************
    ACS_JTP_Conversation_Ptr l_pJTPconversation(new ACS_JTP_Conversation);
    unsigned short U1(0), U2(0);

    //****************************************************************************
    // Accept incoming call from remote side.
    //****************************************************************************
    TRACE((LOG_LEVEL_INFO, "calling JTP_Service.accept", 0));
    l_pConversation->m_pJTPservice->accept(NULL, l_pJTPconversation.get());

    //****************************************************************************
    // Check that incoming call is a conversation.
    //****************************************************************************
    if (l_pJTPconversation->State() == ACS_JTP_Conversation::StateConnected)
    {
        //***********************************************************************
        // Conversation initiation indication.
        //***********************************************************************
        TRACE((LOG_LEVEL_INFO, "calling jexinitind", 0));
        if (!l_pJTPconversation->jexinitind(U1, U2))
        {
            EVENT((CPS_BUAP_Events::jexinitind_fail, 0));
            //delete l_pJTPconversation;
        }
        else
        {
            //*******************************************************************
            // Job initiation response.
            //*******************************************************************
            TRACE((LOG_LEVEL_INFO, "calling jexinitrsp", 0));
            if (!l_pJTPconversation->jexinitrsp(U1, U2, 0))
            {
                EVENT((CPS_BUAP_Events::jexinitrsp_fail, 0));
                //delete l_pJTPconversation;
            }
            else
            {
                //***************************************************************
                // Successfully sent confirmation.
                //***************************************************************
                if (U1 == CPS_BUAP_Loader_MsgId_ConnectClient)
                {
                    //*************************************************************
                    // If it's a new reload all conversations in an interrupted
                    // reload, if any, must be ended.
                    //*************************************************************
                    for (unsigned int n = 0; n < m_ConversationList.size(); n++, l_iter++)
                    {
                        if ((*l_iter)->m_pJTPconversation != 0)
                        {
                            m_ConversationList.remove(*l_iter);
                            //delete *l_iter;
                        }
                    }
                }

                //***************************************************************
                // Get a handle for the conversation and link it into
                // conversation list.
                //***************************************************************
/*
                CPS_BUAP_Conversation *l_pNewConversation;
                l_pNewConversation = new CPS_BUAP_Conversation(
                        l_pJTPconversation->getHandle(),
                        l_pConversation->m_pJTPservice, l_pJTPconversation);
*/
		CPS_BUAP_Conversation_Ptr l_pNewConversation(new CPS_BUAP_Conversation(
                        l_pJTPconversation->getHandle(),
                        l_pConversation->m_pJTPservice, l_pJTPconversation));
                m_ConversationList.push_back(l_pNewConversation);
            }
        }
    }

}

//******************************************************************************
//	receiveMsg()
//******************************************************************************
void CPS_BUAP_Loader::receiveMsg(CPS_BUAP_Conversation_Ptr l_pConversation)

{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_Loader::receiveMsg()", 0));
    unsigned short U1(0), U2(0), len(0);
    char* msg = NULL;

    //****************************************************************************
    // Recieve message.
    //****************************************************************************
    TRACE((LOG_LEVEL_INFO, "calling jexdataind", 0));
    if (!l_pConversation->m_pJTPconversation->jexdataind(U1, U2, len, msg))
    {
        //************************************************************************
        // No data to fetch, the client has probably requested end of
        // conversation. Delete conversation objects.
        //************************************************************************
        TRACE((LOG_LEVEL_INFO, "conversation ended on handle %x", 0, l_pConversation->m_handle));
        m_ConversationList.remove(l_pConversation);
        //delete l_pConversation;
    }

    else
    {
        switch (U1)
        {
        case CPS_BUAP_Loader_MsgId_getFn:
        {
            //********************************************************************
            // Get correct dumpname for client.
            //********************************************************************
            string name(msg);
            TRACE((LOG_LEVEL_INFO, "Client wants filename for >%s<", 0, msg));

            if (loadSession != 0)
            {
                //******************************************************************
                // Remove old session.
                //******************************************************************
                delete loadSession;
                loadSession = 0;
            }

            //********************************************************************
            // Create new session.
            //********************************************************************
            try
            {
		loadSession = new CPS_BUAP_LoadingSession();	
                U1 = CPS_BUAP_Loader_MsgId_getFn_ok;

                loadSession->getFn(name);


            } catch (...)
            {
                U1 = CPS_BUAP_Loader_MsgId_getFn_failed;
                TRACE((LOG_LEVEL_ERROR, "Get file name failed!!!\n", 0));
            }
            U2 = 0; // For JTP message
            len = name.length() + 1; // For JTP message
            strcpy(msg, name.data()); //:ag1 bugfix - For JTP message
            TRACE((LOG_LEVEL_INFO, "New name is >%s<, len is %d\n", 0,msg, len));
        
	}
            break;

        case CPS_BUAP_Loader_MsgId_reportCmdLog:
            //**********************************************************************
            // Client reports R0 data
            //**********************************************************************
            TRACE((LOG_LEVEL_INFO, "Client reports command log data", 0))
            ;
            U1 = CPS_BUAP_Loader_MsgId_reportCmdLog_ok;
            try
            {
                loadSession->reportCmdlog(*(R0Info*) msg);
            } catch (...)
            {
                U1 = CPS_BUAP_Loader_MsgId_reportCmdLog_failed;
                TRACE((LOG_LEVEL_ERROR, "Report command log failed!!!\n", 0));
            }
            U2 = 0; // For JTP message
            len = 0; // For JTP message

            break;

        case CPS_BUAP_Loader_MsgId_CPLoadDone:
            //**********************************************************************
            // Got signal that CP conciders loading is done.
            //**********************************************************************
            TRACE((LOG_LEVEL_INFO, "Loading Done received", 0));
            if (loadSession != 0)
            {
                //********************************************************************
                // The data to be written in RELADMPAR is already stored in the
                // LoadingSession instance (sent in reportCmdLog). If the instance
                // has been deleted (eg. restart of AP) there's no data to write.
                //********************************************************************

                try
                {
                    loadSession->writeLastReloadParams();
                } catch (...)
                {
                    TRACE((LOG_LEVEL_ERROR, "Cannot write last reload params!!!\n", 0));
                    EVENT((CPS_BUAP_Events::cannot_write_LRP, 0));
                }

		//loadSession.reset();
                delete loadSession;
                loadSession = 0;

            }
            //************************************************************************
            // Prepare answer to CP
            //************************************************************************
            // UABBIR Hanging after
            //        MS-reload of CP
            U1 = CPS_BUAP_Loader_MsgId_CPLoadDone_ack; // UABBIR 010327
            U2 = 0; // For JTP message   UABBIR 010327
            len = 0; // For JTP message   UABBIR 010327

            break;

        default:
            EVENT((CPS_BUAP_Events::invalid_message, 0, U1));
	    TRACE((LOG_LEVEL_WARN, "Invalid message", 0));
            //************************************************************************
            // Delete conversation objects and continue.
            //************************************************************************
            if (!l_pConversation->m_pJTPconversation->jexdiscreq(0, 0, 0))
            {
                EVENT((CPS_BUAP_Events::jexdiscreq_fail, 0));
            }
            TRACE((LOG_LEVEL_INFO, "conversation ended on handle %x", 0, l_pConversation->m_handle));
            m_ConversationList.remove(l_pConversation);
            //delete l_pConversation;
            return;
            break;

        } // switch

        //**************************************************************************
        // Send acknowledge/result signal.
        //**************************************************************************
        TRACE((LOG_LEVEL_INFO, "Sending reply: %d", 0,U1));

        if (!l_pConversation->m_pJTPconversation->jexdatareq(U1, U2, len, msg))
        {
            EVENT((CPS_BUAP_Events::jexdatareq_fail, 0));
            m_ConversationList.remove(l_pConversation);
            //delete l_pConversation;
        }

    } //else

}

//******************************************************************************
//	Process_thread_control()
//******************************************************************************
DWORD CPS_BUAP_Loader::Process_thread_control(int objectHandle)

{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_Loader::Process_thread_control", 0));

    DWORD result = NO_ERROR;

    if (objectHandle == hNTObjectArray[ABORT_EVENT])
    {
        TRACE((LOG_LEVEL_INFO, "CPS_BUAP_Loader::ABORT_EVENT", 0));

        //**********************************************************************
        // So far there is only one thread control event.
        //**********************************************************************
        result = SERVICE_ABORTED;

        // Reset the stop event.
        m_stopEvent.resetEvent();

        // Clear the JTP Handlers list.
        if (!m_ConversationList.empty())
        {
            // delete all conversations from the list
            TRACE(("BULOADER - Delete all conversation from the list", 0));
            for (CPS_BUAP_Conversation_LIST_ITERATOR iConversation =
                    m_ConversationList.begin(); iConversation
                    != m_ConversationList.end(); ++iConversation)
            {
                //delete *iConversation;
		(*iConversation).reset();
            }
        }

        m_ConversationList.clear();

        // Server closed, clear the pointer to JTP Service
        if (m_pService)
        {
            delete m_pService;
            m_pService = 0;
        }

    	if (loadSession != 0)
    	{
            delete loadSession;
            loadSession = 0;
            //loadSession.reset();
            TRACE(("delete loadSession", 0));
    	}	

    }
    else
    {
        result = INVALID_HANDLE;
    }

    return result;
}


//******************************************************************************
//  stop()
//******************************************************************************
void CPS_BUAP_Loader::stop()
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_Loader::stop()", 0));

    m_stopEvent.setEvent();
}

//***************************************************************************
//    isRunning()
//***************************************************************************

bool CPS_BUAP_Loader::isRunning() const
{
    //newTRACE(("CPS_BUAP_Loader::isRunning() - %d", 0, (int)m_running));
    return m_running;
}

//***************************************************************************
//    waitUntilRunning()
//***************************************************************************
void CPS_BUAP_Loader::waitUntilRunning()
{
    newTRACE(("CPS_BUAP_Loader::waitUntilRunning()", 0));

    boost::unique_lock<boost::mutex> lock(m_mutex);

    while (m_running == false)
    {
        TRACE((LOG_LEVEL_INFO, "Wait until CPS_BUAP_Loader server Thread signal", 0));
        m_condition.wait(lock);
        TRACE((LOG_LEVEL_INFO, "CPS_BUAP_Loader server Thread signaled", 0));
    }
}

//***************************************************************************

bool CPS_BUAP_Loader::initiateJTPService()
{
    CPS_BUAP_Conversation_Ptr l_pConversation;

    int noOfServerHandles = 15;
    HANDLE *serverHandles;
    serverHandles = new HANDLE[15];

    bool jtpResult = false;

    //***************************************************************************
    // Start the BULOADER server.
    //***************************************************************************
    newTRACE((LOG_LEVEL_INFO, "new ACS_JTP_Service(BULOADER)", 0));

    uint64_t u;
    ssize_t num;
    bool isTrace = false;

    do
    {
        // Create a JTP to BULOADER server.
        if (m_pService)
        {
            delete m_pService;
            m_pService = 0;
        }

        m_pService = new ACS_JTP_Service((char*) "BULOADER");
        TRACE(("CPS_BUAP_Loader calling jidrepreq", 0));
        // try to connect to JTP Name Service (DSD at the end) until it not fails.
        jtpResult = m_pService->jidrepreq();

        if (jtpResult == false)
        {
            if (!isTrace)
            {
                TRACE((LOG_LEVEL_ERROR, "ACS_JTP_Service(BULOADER)->jidrepreq: failed to register to DSD", 0));
                EVENT((CPS_BUAP_Events::jidrepreq_fail, 0, "BULOADER"));
                isTrace = true;
            }

            // Check if the stop event is set.
            num = ::read(m_stopEvent.getFd(), &u, sizeof(uint64_t));
            if (num == -1)
            {
                if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
                {
                    // Not receive stop event, go to sleep.
                    ::sleep((unsigned int) 1);
                    continue;
                    //THROW("Register to DSD failed.");
                }

                TRACE((LOG_LEVEL_ERROR, "CPS_BUAP_Loader - Failed to read stop event", 0));
                if (serverHandles)
                {
                    delete [] serverHandles;
                    serverHandles = 0;
                }
                //return false;
                THROW("Reading stop event failed");
            }
            else
            {
                TRACE((LOG_LEVEL_INFO, "CPS_BUAP_Loader - Receives stop event %d ", 0, m_stopEvent.getFd()));
                m_stopEvent.resetEvent();

                if (serverHandles)
                {
                    delete [] serverHandles;
                    serverHandles = 0;
                }
                return false;
            }
        }
        else
        {
            TRACE((LOG_LEVEL_INFO, "ACS_JTP_Service(BULOADER) register successfully", 0));
        }
    }
    while (jtpResult == false);

    //****************************************************************************
    // Get JTP handles for service and link them into conversation list.
    //****************************************************************************
    m_pService->getHandles(noOfServerHandles, serverHandles);

    for (int m = 0; m < noOfServerHandles; m++)
    {
        l_pConversation.reset(new CPS_BUAP_Conversation(serverHandles[m], m_pService));

        TRACE(("Add conversation with serverHandles[%d] = %d", 0, m, serverHandles[m]));
        addConversation(l_pConversation);
    }

    if (serverHandles)
    {
        delete[] serverHandles;
        serverHandles = 0;
    }

    TRACE((LOG_LEVEL_INFO, "CPS_BUAP_Loader initiateJTPService returned true", 0));
    return true;
}
