
//******************************************************************************
// 
// .NAME
//  	CPS_BUAP_MSG_dynamic_parameters - AP->AP Message for dynamic parameters
// .LIBRARY 3C++
// .PAGENAME CPS_BUAP_MSG_dynamic_parameters
// .HEADER  CPS  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE CPS_BUAP_MSG_dynamic_parameters.H

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
//	This class is used to read the dynamic parameters.
//	It is derived from CPS_BUAP_MSG. 

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
// 	CPS_BUAP_MSG
//  101010 xchihoa     Ported to Linux for APG43L.
//******************************************************************************

#ifndef CPS_BUAP_MSG_dynamic_parameters_H
#define CPS_BUAP_MSG_dynamic_parameters_H

#include "CPS_BUAP_UINT8.h"
#include "CPS_BUAP_OPT_UINT8.h"

#include "CPS_BUAP_MSG.h"

class CPS_BUAP_MSG_dynamic_parameters : public CPS_BUAP_MSG
{
public:

  CPS_BUAP_UINT8  m_Validity;		// Validity indication of dynamic parameters (read only)
  CPS_BUAP_OPT_UINT8  m_State;		// State, current state of loading, 1 - 6
  CPS_BUAP_OPT_UINT8  m_CTA;		// Current truncation attempt
  CPS_BUAP_OPT_UINT8  m_NumFFR;		// File number in FFR
  CPS_BUAP_OPT_UINT8  m_NumSFR;		// File number in SFR
  CPS_BUAP_OPT_UINT8  m_NOld;		// Highest file number in FFR
  CPS_BUAP_OPT_UINT8  m_KOld;		// Highest accepted file in SFR
  CPS_BUAP_OPT_UINT8  m_NCT;		// Number of truncation attempts

  // Supervision expiring time
  CPS_BUAP_OPT_UINT8  m_EXPcentury;	// Century number (-1!!! ie 19 = 20th century)
  CPS_BUAP_OPT_UINT8  m_EXPyear;	// Year (0 - 99)
  CPS_BUAP_OPT_UINT8  m_EXPmonth;	// Month (1 - 12)
  CPS_BUAP_OPT_UINT8  m_EXPday;		// Day (1 -31)
  CPS_BUAP_OPT_UINT8  m_EXPhour;	// Hour (1 -24)
  CPS_BUAP_OPT_UINT8  m_EXPminute;	// Minute (0 - 59)

  CPS_BUAP_OPT_UINT8  m_LPF;		// Last permissible file (only valid if state = 6)
  CPS_BUAP_OPT_UINT8  m_Omission;	// Omission

  CPS_BUAP_MSG_dynamic_parameters();
  // Description:
  // 	Class constructor
  // Parameters: 
  //	-
  // Return value: 
  //	-
  // Additional information:
  //	-


  CPS_BUAP_MSG_dynamic_parameters(unsigned short msgID);
  // Description:
  // 	Class constuctor with message identity
  // Parameters: 
  //    msgId			identity of message (userdata1), in 
  // Return value: 
  //	-
  // Additional information:
  //	-


  virtual ~CPS_BUAP_MSG_dynamic_parameters();
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
