//******************************************************************************
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson Utvecklings AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.
//
//  NAME
//  CPS-BUAP_Server.H
//
//  DESCRIPTION 
//	This class multiplexes JTP conversations. It does this by
//	maintaining a list of active conversations.  Each conversation
//	in the list is associated with a JTP handle.  
//  The function WaitForMultipleObjects is called.
//  The class also syncronizes the threads within the process. 
//      
//  DOCUMENT NO
//  190 89-CAA 109 1412
//
//  AUTHOR 
// 	1999/05/13 by UAB/B/SF Birgit Berggren
//
//******************************************************************************
// === Revision history ===
// 990513 BIR PA1 Created.
// 990607 BIR PA2 Updated.
// 990617 BIR PA3 Updated after review.
//  101010 xchihoa     Ported to Linux for APG43L.
//******************************************************************************

#ifndef CPS_BUAP_Server_H
#define CPS_BUAP_Server_H

#include <list>
#include <string>

#include "CPS_BUAP_Linux.h"
#include "CPS_BUAP_Buffer.h"
#include "CPS_BUAP_Conversation.h"
#include "boost/shared_ptr.hpp"

//typedef boost::shared_ptr<CPS_BUAP_Conversation> CPS_BUAP_Conversation_Ptr;
typedef std::list<CPS_BUAP_Conversation_Ptr> CPS_BUAP_Conversation_LIST;
typedef std::list<CPS_BUAP_Conversation_Ptr>::iterator CPS_BUAP_Conversation_LIST_ITERATOR;
//typedef std::list<CPS_BUAP_Conversation *> CPS_BUAP_Conversation_LIST;
//typedef std::list<CPS_BUAP_Conversation *>::iterator CPS_BUAP_Conversation_LIST_ITERATOR;
        
        
#define MAX_NT_OBJECTS            2   // Max size of NTObjectArray 
#define MAX_TEMP_OBJECTS          15  // Max size of TempArray (The handles to 
                                      // the NT objects + the JTP handles)
//******************************************************************************
// Internally generated error codes
//******************************************************************************
#define USER_ERROR 0x20000000

#define INVALID_HANDLE             (USER_ERROR | 0x00000003)


class CPS_BUAP_Server
{
public:

  CPS_BUAP_Server();
  // Description:
  // 	Class constructor
  // Parameters: 
  //	-
  // Return value: 
  //	-
  // Additional information:
  //	-


  virtual ~CPS_BUAP_Server();
  // Description:
  // 	Class destructor
  // Parameters: 
  //	-
  // Return value: 
  //	-
  // Additional information:
  //	-

  DWORD Poll();
  // Description:
  // 	Method used to poll for input using WaitForMultipleObjects
  // Parameters: 
  //    -
  // Return value: 
  //	NO_ERROR or result of routine Process_thread_control
  // Additional information:
  //	-


protected:

  HANDLE           hNTObjectArray[MAX_NT_OBJECTS];   // Array of NT objects 
                                                     // used for thread control
  DWORD            noOfNTHandles;                    // Current number of  
                                                     // handles in NTObjectarray
               

  CPS_BUAP_Conversation_LIST m_ConversationList;    // List of conversations

  std::string m_deriveName;

  void AddConversation(CPS_BUAP_Conversation_Ptr l_pConversation);
  // Description:
  // 	Method used to add a conversation to the list.
  // Parameters: 
  //    l_pConversation		Pointer to conversation object, in
  // Return value: 
  //	-
  // Additional information:
  //	-
  
  virtual void RXTX(HANDLE handle,
                    unsigned short  rxUserData1, 
                    unsigned short  rxUserData2, 
                    CPS_BUAP_Buffer &rxBuffer,
                    unsigned short &txUserData1, 
                    unsigned short &txUserData2, 
                    CPS_BUAP_Buffer &txBuffer) = 0;
  // Description:
  // 	Method used to process a received message and transmit the reply. This 
  // 	method is pure and must be defined in a derived class.
  // Parameters: 
  //    handle			JTP handle message received on, in
  //    rxUserData1		received userdata1, in
  //    rxUserData2		received userdata2, in
  //    rxBuffer		received buffer, in
  //    txUserData1		userdata1 to be transmitted, out
  //    txUserData1		userdata2 to be transmitted, out
  //    txBuffer		user buffer to be transmitted, out
  // Return value: 
  //	-
  // Additional information:
  //	-

  virtual DWORD Process_thread_control(DWORD objectHandle) = 0; 
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
 
private:

  CPS_BUAP_Server(const CPS_BUAP_Server& );
  //:ag1
  // Description:
  // 	Copy constructor
  // Parameters: 
  //	-
  // Return value: 
  //	-
  // Additional information:
  //	Declared to disallow copying

  CPS_BUAP_Server& operator=(const CPS_BUAP_Server& );
  //:ag1
  // Description:
  // 	Assignment operator
  // Parameters: 
  //	-
  // Return value: 
  //	-
  // Additional information:
  //	Declared to disallow copying

  void NewConversation(CPS_BUAP_Conversation_Ptr l_pConversation);
  // Description:
  // 	Method to process a new conversation
  // Parameters: 
  //    l_pConversation		Pointer to conversation object, in
  // Return value: 
  //	-
  // Additional information:
  //	-


  void OldConversation(CPS_BUAP_Conversation_Ptr l_pConversation);
  // Description:
  // 	Method to process activity on an existing conversation
  // Parameters: 
  //    l_pConversation		Pointer to conversation object, in
  // Return value: 
  //	-
  // Additional information:
  //	-



  //virtual void OnDisconnect(HANDLE handle) = 0;
  // 	Method used to pass on a disconnection event
  // Parameters: 
  //    handle			JTP handle message received on, in
  // Return value: 
  //	-
  // Additional information:
  //	-

};

#endif
