//******************************************************************************
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 1999
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
// DESCRIPTION 
//	This class is used to update the command
//	log references.  It is derived from CPS_BUAP_MSG. 
//
//
// DOCUMENT NO
//  190 89-CAA 109 1412
//
// AUTHOR 
// 	1999/07/29 UAB/B/SF Birgit Berggren
//
//******************************************************************************
// === Revision history ===
// 990729BIR PA1 Created.
//  101010 xchihoa     Ported to Linux for APG43L.
//******************************************************************************

#ifndef CPS_BUAP_MSG_write_clog_H
#define CPS_BUAP_MSG_write_clog_H

#include "CPS_BUAP_UINT8.h"
#include "CPS_BUAP_UINT32.h"

#include "CPS_BUAP_MSG.h"


class CPS_BUAP_MSG_write_clog : public CPS_BUAP_MSG
{
public:
  enum 
  {
    eCurrent             = 1, 
    eCurrentPrepared     = 2, 
    eCurrentMain         = 3, 
    eCurrentMainPrepared = 4, 
    eMain                = 5
  };

  CPS_BUAP_UINT8      m_ParType;		// Write or read
  CPS_BUAP_UINT8      m_ClogRefType; 	// Which references to update
  CPS_BUAP_UINT32     m_ClogRef;	   	// Command log number
 

  CPS_BUAP_MSG_write_clog();
  // Description:
  // 	Class constructor
  // Parameters: 
  //	-
  // Return value: 
  //	-
  // Additional information:
  //	-


  virtual ~CPS_BUAP_MSG_write_clog();
  // Description:
  // 	Class destructor
  // Parameters: 
  // Return value: 
  //	-
  // Additional information:
  //	-


  virtual void Encode(CPS_BUAP_Buffer& userBuffer);
  // Description:
  // 	Method used to encode the message fields into a buffer.
  // Parameters: 
  //    userBuffer		Buffer into which message is to be encoded, in
  // Return value: 
  //	-
  // Additional information:
  //	-


  virtual void Decode(CPS_BUAP_Buffer& userBuffer);
  // Description:
  // 	Method used to decode a message from a buffer.
  // Parameters: 
  //    userBuffer		Buffer from which the message is to be decoded, in
  // Return value: 
  //	-
  // Additional information:
  //	-


protected:

private:

};

#endif
