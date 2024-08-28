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
//  CPS_BUAP_Loader.h
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
//  990906 by UAB/B/SF Birgit Berggren
//
//******************************************************************************
// === Revision history ===
// 990906 BIR PA1  Created.
// 101215 xchihoa  Port to Linux for APG43L
// 120106 xngudan  Updated
//******************************************************************************

#ifndef CPS_BUAP_Loader_H 
#define CPS_BUAP_Loader_H

#include "CPS_BUAP_EventFD.h"
#include "CPS_BUAP_Linux.h"
#include "CPS_BUAP_Exception.h"
#include "CPS_BUAP_Conversation.h"
#include "CPS_BUAP_LoadingSession.h"
#include "ACS_JTP.h"

#include <list>
#include "boost/thread/mutex.hpp"
#include "boost/thread/condition_variable.hpp"
#include "boost/thread/recursive_mutex.hpp"
#include "boost/shared_ptr.hpp"

typedef boost::shared_ptr<CPS_BUAP_Conversation> CPS_BUAP_Conversation_Ptr;
typedef std::list<CPS_BUAP_Conversation_Ptr> CPS_BUAP_Conversation_LIST;

typedef std::list<CPS_BUAP_Conversation_Ptr>::iterator
      CPS_BUAP_Conversation_LIST_ITERATOR;

//=============================================================================
// Constants
//=============================================================================
#define ABORT_EVENT               0
#define MAX_NT_OBJECT            1    // Max size of NTObjectArray
#define MAX_TEMP_OBJECTS          15   // Max size of TempArray (The handles to
                                       // the NT objects + the JTP handles)
//#define __FD_SETSIZE                100000


class CPS_BUAP_Loader
{

public:

   CPS_BUAP_Loader();
   //----------------------------------------------------------
   // Description:
   //      Class default constructor .
   //      Initializes all attributes and creates dynamic objects
   // Parameters:
   //      None
   // Return value:
   //      None
   // Additional information:
   //      None
   //----------------------------------------------------------

   ~CPS_BUAP_Loader();
   //----------------------------------------------------------
   // Description:
   //      Class destructor . Disallocate memory for the class
   // Parameters:
   //      None
   // Return value:
   //      None
   // Additional information:
   //      None
   //----------------------------------------------------------


   void Run();
   // Description:
   // 	Method implementing the main control loop
   // Parameters:
   //	-
   // Return value:
   //	-
   // Additional information:
   //	-

   void stop();
   // Description
   //   Method for stopping server
   // Parameters:
   //   -
   // Return value:
   //   -
   // Additional information:
   //   -

   bool isRunning() const;
   // Description:
   //     Method for checking status of buparmgr server
   // Parameters:
   //    -
   // Return value:
   //    -
   // Additional information:
   //

   void waitUntilRunning();
   // Description:
   //     Method for blocking the calling thread until buparmgr server finishes start up
   // Parameters:
   //    -
   // Return value:
   //    -
   // Additional information:
   //

private:

   CPS_BUAP_Loader(const CPS_BUAP_Loader&);
   //:ag1
   // Description:
   // 	Copy constructor
   // Parameters:
   //	-
   // Return value:
   //	-
   // Additional information:
   //	Declared to disallow copying

   CPS_BUAP_Loader& operator=(const CPS_BUAP_Loader&);
   //:ag1
   // Description:
   // 	Assignment operator
   // Parameters:
   //	-
   // Return value:
   //	-
   // Additional information:
   //	Declared to disallow copying


   void addConversation(CPS_BUAP_Conversation_Ptr l_pConversation);
   // Description:
   // 	Method used to add a conversation to the list.
   // Parameters:
   //    l_pConversation		Pointer to conversation object, in
   // Return value:
   //	-
   // Additional information:
   //

   void newConversation(CPS_BUAP_Conversation_Ptr l_pConversation);
   //----------------------------------------------------------
   // Description:
   // 	Establishes a new JTP connection
   // Parameters:
   //    l_pConversation		Pointer to conversation object, in
   // Return value:
   //	None
   // Additional information:
   //    None
   //----------------------------------------------------------

   DWORD poll();
   //----------------------------------------------------------
   // Description:
   // 	Method used to poll for input using WaitForMultipleObjects
   // Parameters:
   //    None
   // Return value:
   //	NO_ERROR or result of routine Process_thread_control
   // Additional information:
   //    None
   //----------------------------------------------------------

   void receiveMsg(CPS_BUAP_Conversation_Ptr l_pConversation);
   //----------------------------------------------------------
   // Description:
   // 	Recieves a JTP message from the CP
   // Parameters:
   //    l_pConversation		Pointer to conversation object, in
   // Return value:
   //	None
   // Additional information:
   //	None
   //----------------------------------------------------------

   DWORD Process_thread_control(int objectHandle);
   //----------------------------------------------------------
   // Description:
   //    This method is called for to find out whether a command has been
   //    executed or if the service has been aborted by operator.
   //    This metod must be defined in a derived class.
   // Parameters:
   //    objectHandle            index in NTObjectArray
   // Return value:
   //	result
   //
   //
   // Additional information:
   //	-
   //---------------------------------------------------------- 


   bool initiateJTPService();
   // Description:
   //  Method to start ACS_JTP Service
   // Parameters:
   //    -
   // Return value
   //    -
   // Additional information:
   //


private:
   ACS_JTP_Service*             m_pService;
   CPS_BUAP_Conversation_LIST   m_ConversationList; // List of conversations
   //boost::shared_ptr<CPS_BUAP_LoadingSession>     loadSession;
   CPS_BUAP_LoadingSession*      loadSession;

   HANDLE   hNTObjectArray[MAX_NT_OBJECT]; // Array of NT objects
                                           // used for thread control
   DWORD    noOfNTHandles; // Current number of
                           // handles in NTObjectarray

   bool                         m_running;      // Return state of server

   boost::mutex                 m_mutex;        // For condition variable
   boost::condition_variable    m_condition;

   CPS_BUAP_EventFD             m_stopEvent;

};

#endif
