
//******************************************************************************
// 
// .NAME
//  	CPS_BUAP_MSG_text - AP->AP Message Command Log References
// .LIBRARY 3C++
// .PAGENAME CPS_BUAP_MSG_text
// .HEADER  CPS  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE CPS_BUAP_MSG_text.H

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
//	This class is used for the AP message to read/write the command
//	log references.  It is derived from CPS_BUAP_MSG. 

// .ERROR HANDLING
//
//	General rule:
//	The error handling is specified for each method.
//
//	No methods initiate or send error reports unless specified.

// DOCUMENT NO
//	190 89-CAA 109 0082

// AUTHOR 
// 	1997/08/06 by U/B/SD David Wade

// .LINKAGE
//	-

// .SEE ALSO 
// 	CPS_BUAP_MSG

//******************************************************************************

#ifndef CPS_BUAP_MSG_text_H
#define CPS_BUAP_MSG_text_H

#include "CPS_BUAP_TEXT.h"

#include "CPS_BUAP_MSG.h"

class CPS_BUAP_MSG_text : public CPS_BUAP_MSG
{
public:

  CPS_BUAP_TEXT            m_Text;	// text

  CPS_BUAP_MSG_text();
  // Description:
  // 	Class constructor
  // Parameters: 
  //	-
  // Return value: 
  //	-
  // Additional information:
  //	-

  CPS_BUAP_MSG_text(unsigned short msgID);
  // Description:
  // 	Class constuctor with message identity
  // Parameters: 
  //    msgId			identity of message (userdata1), in 
  // Return value: 
  //	-
  // Additional information:
  //	-

  virtual ~CPS_BUAP_MSG_text();
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
