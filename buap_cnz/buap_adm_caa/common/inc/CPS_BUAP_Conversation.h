//******************************************************************************
//
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
//  CPS_BUAP_Conversation.H  
//
//  DESCRIPTION 
//	Class contains all data about a conversation. This includes
//	the handles overwhich the conversation is taking place
//	and pointers to the JTP_Service and JTP_Conversation.
//
//	When the conversation is being used to listen for new JTP
//	conversations, the member pointer to the JTP_Conversation will
//	be null.  When the conversation is for an active JTP conversation
//	the member pointer the JTP_Conversation will not be null.
//
//  DOCUMENT NO
//  190 89-CAA 109 1412
//
//  AUTHOR 
// 	1999/03/24 by UAB/B/SF Birgit Berggren
//
//
//******************************************************************************
// === Revision history ===
// 990324 BIR PA1 Created.
// 990618 BIR PA2 Updated after review. myTester:Check if t=0 added.
// 990906 BIR PA3 myTester() removed. (Due to correction of Poll routine in 
//                Server.cpp not used anymore.
//  101010 xchihoa     Ported to Linux for APG43L.
//******************************************************************************

#ifndef CPS_BUAP_Conversation_H
#define CPS_BUAP_Conversation_H

#include "CPS_BUAP_Linux.h"
#include "ACS_JTP.h"
#include "boost/shared_ptr.hpp"

class CPS_BUAP_Server;

typedef boost::shared_ptr<ACS_JTP_Conversation> ACS_JTP_Conversation_Ptr;
//typedef boost::shared_ptr<ACS_JTP_Service> ACS_JTP_Service_Ptr;

class CPS_BUAP_Conversation
{

public:
  HANDLE                   m_handle;

  CPS_BUAP_Conversation(HANDLE hJTPhandle,
                        ACS_JTP_Service *pJTPservice);

  CPS_BUAP_Conversation(HANDLE hJTPhandle, 
                        ACS_JTP_Service *pJTPservice, 
                        ACS_JTP_Conversation_Ptr pJTPconversation);
  // Description:
  // 	Class constructor
  // Parameters: 
  //    hJTPhandle	        handle associated with this conversation
  //    pJTPservice		pointer to the associated JTP service object
  //    pJTPconversation	pointer to the associated JTP conversation object.
  // Return value: 
  //    -
  // Additional information:
  //    -


  ~CPS_BUAP_Conversation();
  // Description:
  // 	Class destructor.  Invokes destructor for JTP conversation object
  // Parameters: 
  //    -
  // Return value: 
  //    -
  // Additional information:
  //    -


  int operator==(const CPS_BUAP_Conversation& conversation);
  // Description:
  // 	Equality operator    
  // Parameters: 
  //    conversation		reference to conversation comparee 
  // Return value: 
  //    int			1=equal, 0=unequal
  // Additional information:
  //    -


  friend class CPS_BUAP_Server;
  friend class CPS_BUAP_Loader;


  CPS_BUAP_Conversation(const CPS_BUAP_Conversation& );
  //:ag1
  // Description:
  // 	Copy constructor
  // Parameters: 
  //	-
  // Return value: 
  //	-
  // Additional information:
  //	Declared to disallow copying

//protected:

//private:
  CPS_BUAP_Conversation& operator=(const CPS_BUAP_Conversation& );
  //:ag1
  // Description:
  // 	Assignment operator
  // Parameters: 
  //	-
  // Return value: 
  //	-
  // Additional information:
  //	Declared to disallow copying

private:
  ACS_JTP_Conversation_Ptr m_pJTPconversation;	// pointer to JTP conversation
  ACS_JTP_Service         *m_pJTPservice;		// pointer to JTP service

};

typedef boost::shared_ptr<CPS_BUAP_Conversation> CPS_BUAP_Conversation_Ptr;

#endif
