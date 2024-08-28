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
//  190 89-CAA 109 1029
//
//  AUTHOR 
//  990913 UAB/B/SF Birgit Berggren
//	010327 UAB/Y/SF Birgit Berggren
//
//*****************************************************************************
// === Revision history ===
// 990913 BIR PA1 Created.
// 991105 BIR PA2 Correction of TRACE printout.
// 010327 BIR PB1 Hanging after reload from MS.
//*****************************************************************************

#include <windows.h>
#include <winbase.h>                         // Sleep()
 
#include <stdio.h>
#include <stdlib.h>

#pragma warning (disable : 4786 ) //:ag1 debug info truncated

#include "CPS_BUAP_types.H"                 // R0Info        
#include "CPS_BUAP_Loader_MsgIDs.H"
#include "CPS_BUAP_Trace.H"
#include "CPS_BUAP_Events.H"
#include "CPS_BUAP_Loader_Global.H"
#include "CPS_BUAP_Loader.H"

using namespace std;
//*****************************************************************************
//	CPS_BUAP_Loader()
//*****************************************************************************
CPS_BUAP_Loader::CPS_BUAP_Loader(PHANDLE hAbortEventPtr)
    : m_pService(0),
      loadSession(0),
      noOfNTHandles(0)

{    
   newTRACE(("CPS_BUAP_Loader::CPS_BUAP_Loader()", 0))
   
   CPS_BUAP_Conversation *l_pConversation;
   int noOfServerHandles;
   HANDLE *serverHandles;
   
   
   //***************************************************************************
   // Initiate the array of thread control objects.
   //***************************************************************************
   TRACE(("initiating hNTObjectArray(BULOADER)", 0))
   hNTObjectArray[ABORT_EVENT] = (HANDLE) *hAbortEventPtr;  // There's always a 
   noOfNTHandles = 1;                           // handle to the AbortEvent
      
 
   //***************************************************************************
   // Start the BULOADER server.
   //***************************************************************************
   TRACE(("new ACS_JTP_Service(BULOADER)", 0))

   m_pService = new ACS_JTP_Service("BULOADER");

   // Try to connect to JTP Name Service. Make three attempts.
   int i = 0;
   while (++i <= 3)
     {
       if ((m_pService->jidrepreq() == true ))
	 {
	   TRACE(("Server registred in DSD", 0))
	   break;
	 }
       else
	 {
	   TRACE(("Unable to connect to DSD. %d", 0, i))
	   Sleep(100); //:ag - changed from 10000, which is too long
	 }
     }	  
   if (i > 3)	
     //*************************************************************************
     // No answer from DSD, give up.
     //*************************************************************************
     {
       delete m_pService;
       EVENT((CPS_BUAP_Events::jidrepreq_fail, 0, "BULOADER"))
       THROW("unable to register BULOADER server in JTP DSD");
     }  
     
  //****************************************************************************
  // Get JTP handles for service and link them into conversation list.
  //****************************************************************************
  m_pService->getHandles(noOfServerHandles, serverHandles);
    
  for (int m=0; m<noOfServerHandles; m++)
    {
     l_pConversation = new CPS_BUAP_Conversation(serverHandles[m], 
                                                 m_pService, 
                                                 (ACS_JTP_Conversation *)0);
   
     addConversation(l_pConversation);
   }
  
} 


//*****************************************************************************
//	~CPS_BUAP_Loader()
//*****************************************************************************
CPS_BUAP_Loader::~CPS_BUAP_Loader()
{
  newTRACE(("CPS_BUAP_Loader::~CPS_BUAP_Loader()", 0))
    
  CPS_BUAP_Conversation_LIST_ITERATOR  l_iter;
  for (l_iter=m_ConversationList.begin(); l_iter != m_ConversationList.end();  l_iter++)		 
  {			 
	  delete *l_iter;	
	  TRACE(("~CPS_BUAP_Loader delete l_iter", 0));
  }

//  m_ConversationList.clear();
  if (m_pService != 0){
    delete m_pService;
	TRACE(("~CPS_BUAP_Loader delete m_pService", 0));
  }
  if (loadSession != 0){
	delete loadSession;
	TRACE(("~CPS_BUAP_Loader delete loadSession", 0));
  }
  TRACE(("End of ~CPS_BUAP_Loader", 0));
}   


//******************************************************************************
//	addConversation()
//******************************************************************************
void CPS_BUAP_Loader::addConversation(CPS_BUAP_Conversation* conversation)
{
  newTRACE(("CPS_BUAP_Loader::addConversation() on handle %x", 0, conversation->m_handle))

  m_ConversationList.push_back(conversation);
}


//*****************************************************************************
//	run()
//*****************************************************************************
void CPS_BUAP_Loader::run()
{
 newTRACE(("CPS_BUAP_Loader::run()", 0))

 while (!poll());
 
}
//*****************************************************************************
//	poll()
//*****************************************************************************
DWORD CPS_BUAP_Loader::poll()
{
 newTRACE(("CPS_BUAP_Loader::poll()", 0))
 
 CPS_BUAP_Conversation_LIST_ITERATOR  l_iter=m_ConversationList.begin();
  
 DWORD l_entries, pollResult;
 HANDLE hTempArray[MAX_TEMP_OBJECTS];    
  
  try
	{
	 l_entries = m_ConversationList.size();
	     
     //************************************************************************
     // Fetch thread control handles.        
     //************************************************************************
     for (DWORD i=0; i<noOfNTHandles; i++) 
         hTempArray[i] = hNTObjectArray[i]; 
  
         //********************************************************************
         // Fetch the JTP handles.
         //********************************************************************

         for (DWORD j=0; j<l_entries; j++, l_iter++)  
             {
				hTempArray[noOfNTHandles + j] = (*l_iter)->m_handle;
             } 
      //***********************************************************************
      // Wait until one of the objects has been signaled.
      //***********************************************************************
      TRACE(("calling WaitForMultipleObjects with %d NThandles and %d JTPhandles", 0, noOfNTHandles, l_entries))
  
      pollResult= WaitForMultipleObjects((noOfNTHandles+l_entries), 
                                        (const HANDLE*) hTempArray,  
                                         FALSE, 
                                         INFINITE);
  
      if (pollResult == WAIT_FAILED)
         //********************************************************************
         // Failure, send event and terminate thread.
         //********************************************************************
         {
          EVENT((CPS_BUAP_Events::WaitForMultipleObjects_fail, 0, GetLastError()));
          THROW("WaitForMultipleObjects failed");
         }
         
      //***********************************************************************
      // The result of WaitForMultipleObjects is WAIT_OBJECT_0 + index in 
      // TempArray, only the latter is of interest.
      //***********************************************************************
      pollResult -=WAIT_OBJECT_0;   
                
	  if (pollResult >= (noOfNTHandles + l_entries))
         //********************************************************************
         // Invalid handle no.
         //********************************************************************
       {
        EVENT((CPS_BUAP_Events::WaitForMultipleObjects_fail, 0, INVALID_HANDLE));
        THROW("WaitForMultipleObjects failed, invalid handle");
        }
    
      //***********************************************************************
      // Successful, find the signaled object.
      //***********************************************************************             
      if (pollResult < noOfNTHandles)
         //********************************************************************
         // It's one of the thread control objects. 
         //********************************************************************
         return(Process_thread_control(pollResult));

      else
	  {
         //********************************************************************
         // It's one of the JTP handles, find out whether it's a server handle
         // or a conversation handle.
         //********************************************************************
		 l_iter = m_ConversationList.begin();
         for (DWORD k=0; k<l_entries; k++, l_iter++)
		 {
             if (pollResult == (noOfNTHandles + k))
			 {
                if ((*l_iter)->m_pJTPconversation==0)
				{
                    newConversation(*l_iter);
                }
                else
				{
                    receiveMsg(*l_iter);
				}

			 } //if
            
		 } // for
         
	  } // else  
                                     
      //***********************************************************************
      // None of the handles has to be reset manually.
      //***********************************************************************
       
      } //try
  
 catch (...)
 {
	throw;
 }

 return NO_ERROR;
	 
} // poll

//*****************************************************************************
//	newConversation()
//*****************************************************************************
void CPS_BUAP_Loader::newConversation(CPS_BUAP_Conversation *l_pConversation)

{
  newTRACE(("CPS_BUAP_Loader::newConversation() on handle %x", 0, l_pConversation->m_handle))
  
  CPS_BUAP_Conversation_LIST_ITERATOR  l_iter=m_ConversationList.begin();
 
  //****************************************************************************
  // Create a conversation object.
  //****************************************************************************
  ACS_JTP_Conversation    *l_pJTPconversation = new ACS_JTP_Conversation;
  unsigned short U1(0), U2(0);

  //****************************************************************************
  // Accept incoming call from remote side.
  //****************************************************************************
  TRACE(("calling JTP_Service.accept", 0))
  l_pConversation->m_pJTPservice->accept(NULL, l_pJTPconversation);
  
  //****************************************************************************
  // Check that incoming call is a conversation.
  //****************************************************************************
  if (l_pJTPconversation->State() == ACS_JTP_Conversation::StateConnected)
    {
       //***********************************************************************
       // Conversation initiation indication.
       //***********************************************************************
       TRACE(("calling jexinitind", 0))
       if (!l_pJTPconversation->jexinitind(U1, U2))
         {
           EVENT((CPS_BUAP_Events::jexinitind_fail, 0))
           delete l_pJTPconversation;
         }
       else
         {
           //*******************************************************************
           // Job initiation response.
           //*******************************************************************
           TRACE(("calling jexinitrsp", 0))
           if (!l_pJTPconversation->jexinitrsp(U1, U2, 0))
             {
               EVENT((CPS_BUAP_Events::jexinitrsp_fail, 0))
               delete l_pJTPconversation;
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
				 for (int n=0; n<m_ConversationList.size(); n++, l_iter++)
				 {
					 if ((*l_iter)->m_pJTPconversation !=0) {
						m_ConversationList.remove(*l_iter);
						delete *l_iter;
					 }
				 }
			  }
			   
			   //***************************************************************
			   // Get a handle for the conversation and link it into          
			   // conversation list.
               //***************************************************************
               CPS_BUAP_Conversation *l_pNewConversation;
               l_pNewConversation = new CPS_BUAP_Conversation(l_pJTPconversation->getHandle(),
                                                              l_pConversation->m_pJTPservice,
                                                              l_pJTPconversation);
                                                           
               m_ConversationList.push_back(l_pNewConversation);
             }
         }
    }


}

//******************************************************************************
//	receiveMsg()
//******************************************************************************
void CPS_BUAP_Loader::receiveMsg(CPS_BUAP_Conversation *l_pConversation)

{
  newTRACE(("CPS_BUAP_Loader::receiveMsg()", 0))
  unsigned short U1(0), U2(0), R(0), len(0);
  char* msg = NULL;
	 
  //****************************************************************************
  // Recieve message.
  //****************************************************************************
  TRACE(("calling jexdataind", 0))
  if (!l_pConversation->m_pJTPconversation->jexdataind(U1, U2, len, msg))
    {
      //************************************************************************
      // No data to fetch, the client has probably requested end of 
      // conversation. Delete conversation objects.
      //************************************************************************
      TRACE(("conversation ended on handle %x", 0, l_pConversation->m_handle))
	  m_ConversationList.remove(l_pConversation);
	  delete l_pConversation;
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
    	TRACE(("Client wants filename for >%s<", 0, msg))

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
        loadSession = new CPS_BUAP_LoadingSession();

        U1 = CPS_BUAP_Loader_MsgId_getFn_ok;
    	try
		{
	      loadSession->getFn(name);	
		}
        catch (...)
		{
	      U1 = CPS_BUAP_Loader_MsgId_getFn_failed;
		}
	    U2 = 0;                       // For JTP message
        len = name.length()+1;        // For JTP message
        strcpy(msg, name.data());     //:ag1 bugfix - For JTP message
    	TRACE(("New name is >%s<, len is %d\n", 0,msg, len))
        }
        break;
      
      case CPS_BUAP_Loader_MsgId_reportCmdLog:
        //**********************************************************************
        // Client reports R0 data
        //**********************************************************************
        TRACE(("Client reports command log data", 0))
        U1 = CPS_BUAP_Loader_MsgId_reportCmdLog_ok; 
        try 
		{
	      loadSession->reportCmdlog(*(R0Info*)msg);
		}
        catch (...)
		{
	      U1 = CPS_BUAP_Loader_MsgId_reportCmdLog_failed;
		}	
        U2 = 0;                       // For JTP message
    	len = 0;                      // For JTP message
	
        break;
      
      case CPS_BUAP_Loader_MsgId_CPLoadDone:
    	//**********************************************************************
    	// Got signal that CP conciders loading is done.
        //**********************************************************************
	    TRACE(("Loading Done received", 0))
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
	     }
	     catch (...)
	     {
	       EVENT((CPS_BUAP_Events::cannot_write_LRP, 0));
	     }
	     delete loadSession;
	     loadSession = 0;

	   }
		//************************************************************************
		// Prepare answer to CP 
		//************************************************************************
														// UABBIR Hanging after
														//        MS-reload of CP
		U1 = CPS_BUAP_Loader_MsgId_CPLoadDone_ack;		// UABBIR 010327
		U2 = 0;                       // For JTP message   UABBIR 010327
	    len = 0;                      // For JTP message   UABBIR 010327

		break;   
    
    default:
      EVENT((CPS_BUAP_Events::invalid_message, 0, U1));
      //************************************************************************
      // Delete conversation objects and continue. 
      //************************************************************************
	  l_pConversation->m_pJTPconversation->jexdiscreq(0, 0, 0);
      TRACE(("conversation ended on handle %x", 0, l_pConversation->m_handle))
      m_ConversationList.remove(l_pConversation);
	  delete l_pConversation;
      return;
      break;
      
    } // switch
	   
    //**************************************************************************
    // Send acknowledge/result signal.
    //**************************************************************************
    TRACE(("Sending reply: %d", 0,U1))
    
    if (!l_pConversation->m_pJTPconversation->jexdatareq(U1, U2, len, msg))
        {
          EVENT((CPS_BUAP_Events::jexdatareq_fail, 0))
          m_ConversationList.remove(l_pConversation);
		  delete l_pConversation;
         }
    
   } //else
   
}   

//******************************************************************************
//	Process_thread_control()
//******************************************************************************
DWORD CPS_BUAP_Loader::Process_thread_control(DWORD objectHandle)
 
 {
  newTRACE(("CPS_BUAP_Loader::Process_thread_control", 0))
  
  DWORD result = NO_ERROR;

  switch(objectHandle)
  {
     case ABORT_EVENT:
     {
       TRACE(("CPS_BUAP_Loader::ABORT_EVENT", 0))
      
        //**********************************************************************
        // So far there is only one thread control event.
        //**********************************************************************
        result = SERVICE_ABORTED;
        break;
         
     } //case ABORT_EVENT
     
         
     default:
     {
     //Kan det intr„ffa????????????? I s† fall har noOfNTObjects felaktigt v„rde
     //och det borde uppt„ckts tidigare. 
     }
   }//switch
    
   return result;
 } 
 
