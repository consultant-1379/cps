
//******************************************************************************
// 
// .NAME
//  	CPS_BUAP_MSG_delayed_measures - AP->AP Message Delayed Measures
// .LIBRARY 3C++
// .PAGENAME CPS_BUAP_MSG_delayed_measures
// .HEADER  CPS  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE CPS_BUAP_MSG_delayed_measures.H

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
//	This class is used read the delayed measures
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

#ifndef CPS_BUAP_MSG_delayed_measures_H
#define CPS_BUAP_MSG_delayed_measures_H

#include "CPS_BUAP_UINT8.h"
#include "CPS_BUAP_OPT_UINT8.h"

#include "CPS_BUAP_RPF.h"
#include "CPS_BUAP_MSG.h"


class CPS_BUAP_MSG_delayed_measures : public CPS_BUAP_MSG
{
public:

  CPS_BUAP_UINT8      m_Validity;		// Validity of delayed measures (read only)
  CPS_BUAP_OPT_UINT8  m_Measures[MAX_MEASURES];	// Delayed measure values (1=true, 0=false)


  CPS_BUAP_MSG_delayed_measures();
  // Description:
  // 	Class constructor
  // Parameters: 
  //	-
  // Return value: 
  //	-
  // Additional information:
  //	-


  virtual ~CPS_BUAP_MSG_delayed_measures();
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
