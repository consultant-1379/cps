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
//  CPS_BUAP_Client.H
//
//  DESCRIPTION 
//	This class implements the interface to an application based on the
//      CPS_BUAP_Server class.
//	When instantiated it connects to the destinator given
//	in the constructor.  Messages may then be sent and responses
//	received via the RXTX routine.
//
//	The destructor when called will end the conversation.
//  DOCUMENT NO
//  190 89-CAA 109 1412
//
//  AUTHOR 
// 	1999/07/17 by UAB/B/SF Birgit Berggren
//
//******************************************************************************
// === Revision history ===
// 990717 BIR PA1 Created.
//  101010 xchihoa     Ported to Linux for APG43L.
//******************************************************************************

#ifndef CPS_BUAP_Client_H
#define CPS_BUAP_Client_H

//#include <ACS_JTP.H>
#include <ACS_JTP.h>

#include "CPS_BUAP_MSG.h"
#include "CPS_BUAP_Exception.h"

#include "boost/shared_ptr.hpp"

class CPS_BUAP_Client
{
public:

  CPS_BUAP_Client( const char *destinatorName,
                         int   timeoutValue,
                         int   numRetries);
  // Description:
  // 	Class constructor
  // Parameters: 
  //	destinatorName		Name of the destinator
  //	timeoutValue		Timeout value
  //	numRetries		Number of retries
  // Return value: 
  //	-
  // Additional information:
  //	-


  virtual ~CPS_BUAP_Client();
  // Description:
  // 	Class destructor
  // Parameters: 
  //	-
  // Return value: 
  //	-
  // Additional information:
  //	-


  void TXRX(CPS_BUAP_MSG& txMsg, CPS_BUAP_MSG& rxMsg);
  // Description:
  // 	Method used to transmit and receive a reply from the parmgr process
  // Parameters: 
  //	txMsg			Message to be sent to parmgr
  //	rxMsg			Message received in response
  // Return value: 
  //	-
  // Additional information:
  //	-


private:
  CPS_BUAP_Client(const CPS_BUAP_Client& );
  //:ag1
  // Description:
  // 	Copy constructor
  // Parameters: 
  //	-
  // Return value: 
  //	-
  // Additional information:
  //	Declared to disallow copying

  CPS_BUAP_Client& operator=(const CPS_BUAP_Client& );
  //:ag1
  // Description:
  // 	Assignment operator
  // Parameters: 
  //	-
  // Return value: 
  //	-
  // Additional information:
  //	Declared to disallow copying


protected:

private:
  char                 *m_DestinatorName;	// Pointer to destinator name
  int                   m_TimeoutValue;		// Timeout value
  int                   m_NumRetries;		// Number of retries
  boost::shared_ptr<ACS_JTP_Conversation> m_pConversation;	// Pointer to JTP conversation object
};

#endif
