
//******************************************************************************
// 
// .NAME
//  	CPS_BUAP_MSG - JTP message base class
// .LIBRARY 3C++
// .PAGENAME CPS_BUAP_MSG
// .HEADER  CPS  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE CPS_BUAP_MSG.H

// .COPYRIGHT
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 1997.
//  All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson Utvecklings AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.

// .DESCRIPTION 
// 	This class acts as a base class for all messages sent
//	over JTP.  The operators >> and << are defined for the
//	encoding and decoding of the message to a buffer.

// .ERROR HANDLING
//
//	General rule:
//	The error handling is specified for each method.
//
//	No methods initiate or send error reports unless specified.

// DOCUMENT NO
//  190 89-CAA 109 1412

// AUTHOR 
// 	1997/08/06 by U/B/SD David Wade

// .LINKAGE
//	-

// .SEE ALSO 
// 	-
//  101010 xchihoa     Ported to Linux for APG43L.
//******************************************************************************

#ifndef CPS_BUAP_MSG_H
#define CPS_BUAP_MSG_H

#include "CPS_BUAP_Buffer.h"

class CPS_BUAP_MSG
{
public:

  CPS_BUAP_MSG();
  // Description:
  // 	Class constructor 
  // Parameters: 
  //    -
  // Return value: 
  //    -
  // Additional information:
  //    -


  CPS_BUAP_MSG(unsigned short msgID);
  // Description:
  // 	Class constructor.  msgID is assigned to userdata1.
  // Parameters: 
  //    msgID			value used to preset userdata1, in
  // Return value: 
  //    -
  // Additional information:
  //    -


  virtual ~CPS_BUAP_MSG();
  // Description:
  // 	Class destructor
  // Parameters: 
  //    -
  // Return value: 
  //    -
  // Additional information:
  //    -


  unsigned short& UserData1();
  // Description:
  // 	Method to return a reference to the userdata1 field
  // Parameters: 
  //    -
  // Return value: 
  //    unsigned short&		reference to userdata1 value
  // Additional information:
  //    -


  unsigned short& UserData2();
  // Description:
  // 	Method to return a reference to the userdata2 field
  // Parameters: 
  //    -
  // Return value: 
  //    unsigned short&		reference to userdata2 value
  // Additional information:
  //    -


  virtual void Encode(CPS_BUAP_Buffer& userBuffer);
  // Description:
  // 	Method used to encode the message to a buffer.
  // Parameters: 
  //    userBuffer&		reference to buffer to store encoded message, in
  // Return value: 
  //    -
  // Additional information:
  //    -


  virtual void Decode(CPS_BUAP_Buffer& userBuffer);
  // Description:
  // 	Method used to decode the message from a buffer.
  // Parameters: 
  //    userBuffer&		reference to buffer containing encoded message, in
  // Return value: 
  //    -
  // Additional information:
  //    -


protected:
  unsigned short   m_UserData1;		// JTP message userdata 1
  unsigned short   m_UserData2;		// JTP message userdata 2

private:
};

#endif
