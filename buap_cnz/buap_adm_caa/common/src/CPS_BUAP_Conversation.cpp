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
//  CPS_BUAP_Conversation.cpp  
//
//  DESCRIPTION 
//  Class contains all data about a conversation. This includes
//  the handles overwhich the conversation is taking place
//  and pointers to the JTP_Service and JTP_Conversation.
//
//  When the conversation is being used to listen for new JTP
//  conversations, the member pointer to the JTP_Conversation will
//  be null.  When the conversation is for an active JTP conversation
//  the member pointer the JTP_Conversation will not be null.

//  DOCUMENT NO
//  190 89-CAA 109 1412

//  AUTHOR 
//  1999/03/24 by UAB/B/SF Birgit Berggren
//  101010 xchihoa     Ported to Linux for APG43L.
//
//******************************************************************************
// === Revision history ===
// 990324 BIR PA1 Created.
// 990618 BIR PA2 Updated after review. myTester:Check if t=0 added.
// 990906 BIR PA3 myTester() removed. (Due to correction of Poll routine in 
//                Server.cpp not used anymore.
//******************************************************************************

#include "CPS_BUAP_Linux.h"

#include "CPS_BUAP_Trace.h"
#include "CPS_BUAP_Conversation.h"


//******************************************************************************
//	CPS_BUAP_Conversation()
//******************************************************************************
CPS_BUAP_Conversation::CPS_BUAP_Conversation(HANDLE hJTPhandle, ACS_JTP_Service *pJTPservice):
    m_handle(hJTPhandle), m_pJTPservice(pJTPservice)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_Conversation::CPS_BUAP_Conversation(handle=%x)", 0, hJTPhandle));
    m_pJTPconversation.reset(); 
}

CPS_BUAP_Conversation::CPS_BUAP_Conversation(HANDLE hJTPhandle, ACS_JTP_Service *pJTPservice, ACS_JTP_Conversation_Ptr pJTPconversation):
    m_handle(hJTPhandle), m_pJTPconversation(pJTPconversation), m_pJTPservice(pJTPservice)
{
  newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_Conversation::CPS_BUAP_Conversation(handle=%x)", 0, hJTPhandle));

 // m_pJTPconversation = pJTPconversation;
 // m_pJTPservice      = pJTPservice;
 // m_handle           = hJTPhandle;
}


//******************************************************************************
//	operator==
//******************************************************************************
int CPS_BUAP_Conversation::operator==(const CPS_BUAP_Conversation& conversation)
{
  newTRACE(("CPS_BUAP_Conversation::operator==()", 0))

  return (m_handle == conversation.m_handle);
}


//******************************************************************************
//	~CPS_BUAP_Conversation()
//******************************************************************************
CPS_BUAP_Conversation::~CPS_BUAP_Conversation()
{
  newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_Conversation::~CPS_BUAP_Conversation() for handle %x", 0, m_handle))

  //if (m_pJTPconversation)
  //   delete m_pJTPconversation;
}


