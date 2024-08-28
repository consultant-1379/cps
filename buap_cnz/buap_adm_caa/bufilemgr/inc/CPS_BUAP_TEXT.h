//******************************************************************************
// 
// .NAME
//  	CPS_BUAP_TEXT - Container for AXE unsigned 8 bit integer
// .LIBRARY 3C++
// .PAGENAME CPS_BUAP_TEXT
// .HEADER  CPS  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE CPS_BUAP_TEXT.H

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
// 	This class acts as a container for an AXE unsigned 8 bit
//      integer.  The class has input/output operators defined 
//      that enable the integer value to be written to and read
//      from a CPS_BUAP_Buffer in the byte order consistant with AXE.

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
// 	-

//******************************************************************************

#ifndef CPS_BUAP_TEXT_H
#define CPS_BUAP_TEXT_H

#include "CPS_BUAP_Buffer.h"


class CPS_BUAP_TEXT
{
public:

  CPS_BUAP_TEXT();
  // Description:
  // 	Class constructor
  // Parameters: 
  //    ch			value, in
  // Return value: 
  //    -
  // Additional information:
  //    -


  const char* operator()(){ return m_Value; };
  // Description:
  // 	Operator to return a reference to the value
  // Parameters: 
  //    -
  // Return value: 
  //    unsigned char&		reference to actual value
  // Additional information:
  //    -


  CPS_BUAP_TEXT& operator<<(const char string[]);
  // Description:
  // 	Input operator
  // Parameters: 
  //    ch			character to be added to the buffer, in
  // Return value: 
  //    CPS_BUAP_TEXT&	reference to this class
  // Additional information:
  //    -

  CPS_BUAP_TEXT& operator>>(char string[]);
  // Description:
  // 	Ouput operator
  // Parameters: 
  //    ch&			character reference, out 
  // Return value: 
  //    CPS_BUAP_TEXT&	reference to this class
  // Additional information:
  //    -

  CPS_BUAP_TEXT& operator<<(CPS_BUAP_Buffer& buf);
  // Description:
  // 	Input operator, for reading value from a buffer.
  // Parameters: 
  //    CPS_BUAP_Buffer&	reference to buffer, in 
  // Return value: 
  //    CPS_BUAP_TEXT& 	reference to this class
  // Additional information:
  //    - 


  CPS_BUAP_TEXT& operator>>(CPS_BUAP_Buffer& buf);
  // Description:
  // 	Output operator, for writing value to a buffer.
  // Parameters: 
  //    CPS_BUAP_Buffer&	reference to buffer, in 
  // Return value: 
  //    CPS_BUAP_TEXT& 	reference to this class
  // Additional information:
  //    - 


protected:

private:
  char m_Value[256];	// actual value
};

#endif
