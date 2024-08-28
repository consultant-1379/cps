
//******************************************************************************
// 
// .NAME
//  	CPS_BUAP_OPT_UINIT32 - Container for OPT unsigned 32 bit integer
// .LIBRARY 3C++
// .PAGENAME CPS_BUAP_OPT_UINT32
// .HEADER  CPS  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE CPS_BUAP_OPT_UINT32.H

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
// 	This class acts as a container for an OPT unsigned 32 bit
//      integer.  The class has input/output operators defined 
//      that enable the integer value to be written to and read
//      from a CPS_BUAP_Buffer in the byte order consistant with OPT.

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


#ifndef CPS_BUAP_OPT_UINT32_H
#define CPS_BUAP_OPT_UINT32_H

#include "CPS_BUAP_Buffer.h"

class CPS_BUAP_OPT_UINT32
{
public:

  CPS_BUAP_OPT_UINT32();
  // Description:
  // 	Class constructor
  // Parameters: 
  //    -
  // Return value: 
  //    -
  // Additional information:
  //    -

  CPS_BUAP_OPT_UINT32(unsigned long value);
  // Description:
  // 	Class constructor
  // Parameters: 
  //    value			value, in
  // Return value: 
  //    -
  // Additional information:
  //    -

  const unsigned long& operator()(){ return m_Value; };
  // Description:
  // 	Operator to return a reference to the value
  // Parameters: 
  //    -
  // Return value: 
  //    unsigned long&		reference to actual value
  // Additional information:
  //    -

  CPS_BUAP_OPT_UINT32& operator<<(unsigned long val);
  // Description:
  // 	Input operator
  // Parameters: 
  //    val			character to be added to the buffer, in
  // Return value: 
  //    CPS_BUAP_OPT_UINT32&	reference to this class
  // Additional information:
  //    -

  CPS_BUAP_OPT_UINT32& operator>>(unsigned long& val);
  // Description:
  // 	Ouput operator
  // Parameters: 
  //    val&			character reference, out 
  // Return value: 
  //    CPS_BUAP_OPT_UINT32&	reference to this class
  // Additional information:
  //    -

  CPS_BUAP_OPT_UINT32& operator<<(CPS_BUAP_Buffer& buf);
  // Description:
  // 	Input operator, for reading value from a buffer
  // Parameters: 
  //    CPS_BUAP_Buffer&	reference to buffer, in 
  // Return value: 
  //    CPS_BUAP_OPT_UINT32& 	reference to this class
  // Additional information:
  //    - 

  CPS_BUAP_OPT_UINT32& operator>>(CPS_BUAP_Buffer& buf);
  // Description:
  // 	Output operator, for writing value to a buffer
  // Parameters: 
  //    CPS_BUAP_Buffer&	reference to buffer, in 
  // Return value: 
  //    CPS_BUAP_OPT_UINT32& 	reference to this class
  // Additional information:
  //    - 


protected:

private:
  enum {notValid = 0, isValid=1};
  unsigned char m_Valid;	// validity of value
  unsigned long m_Value;	// actual numeric value
};

#endif
