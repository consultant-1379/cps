
//******************************************************************************
// 
// .NAME
//  	CPS_BUAP_MSG_static_parameters - AP->AP Message Static Parameters
// .LIBRARY 3C++
// .PAGENAME CPS_BUAP_MSG_static_parameters
// .HEADER  CPS  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE CPS_BUAP_MSG_static_parameters.H

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
//	This class is used to read/write the static
//	parameters.  It is derived from CPS_BUAP_MSG. 

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

#ifndef CPS_BUAP_MSG_static_parameters_H
#define CPS_BUAP_MSG_static_parameters_H

#include "CPS_BUAP_UINT8.h"
#include "CPS_BUAP_OPT_UINT8.h"

#include "CPS_BUAP_MSG.h"


class CPS_BUAP_MSG_static_parameters : public CPS_BUAP_MSG
{
public:
  enum {eAutomatic = 0, eManual = 1};

  CPS_BUAP_UINT8  m_Validity;		// Validity of static parameters (read only)
  CPS_BUAP_OPT_UINT8  m_ManAuto;		// Manual (1) or Automatic (0)
  CPS_BUAP_OPT_UINT8  m_NTAZ;		// Number of truncation attempts
  CPS_BUAP_OPT_UINT8  m_NTCZ;		// Number of truncated commands
  CPS_BUAP_OPT_UINT8  m_LOAZ;		// Log subfile omission attempt
  
  CPS_BUAP_OPT_UINT8  m_INCLAcentury;	// INCLA century
  CPS_BUAP_OPT_UINT8  m_INCLAyear;		// INCLA year
  CPS_BUAP_OPT_UINT8  m_INCLAmonth;		// INCLA month
  CPS_BUAP_OPT_UINT8  m_INCLAday;		// INCLA day

  CPS_BUAP_OPT_UINT8  m_INCL1;		// Files in first range included, 0=RELSFW0, 1=all
  CPS_BUAP_OPT_UINT8  m_INCL2;		// Files in second range included
  CPS_BUAP_OPT_UINT8  m_SUP;		// Supervision time in tens of minutes


  CPS_BUAP_MSG_static_parameters();
  // Description:
  // 	Class constructor
  // Parameters: 
  //	-
  // Return value: 
  //	-
  // Additional information:
  //	-


  CPS_BUAP_MSG_static_parameters(unsigned short msgID);
  // Description:
  // 	Class constuctor with message identity
  // Parameters: 
  //    msgId			identity of message (userdata1), in 
  // Return value: 
  //	-
  // Additional information:
  //	-


  virtual ~CPS_BUAP_MSG_static_parameters();
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
