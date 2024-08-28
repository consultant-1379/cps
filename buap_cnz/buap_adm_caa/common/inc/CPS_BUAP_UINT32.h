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
//  CPS_BUAP_UINT32.H
//
//  DESCRIPTION 
// 	This class acts as a container for an AXE unsigned 32 bit
//  integer.  The class has input/output operators defined 
//  that enable the integer value to be written to and read
//  from a CPS_BUAP_Buffer in the byte order consistant with AXE.
//
//  DOCUMENT NO
//  190 89-CAA 109 1412
//
//  AUTHOR 
// 	1999/08/04 by UAB/B/SF Birgit Berggren
//
//******************************************************************************
// === Revision history ===
// 990804 BIR PA1 Created.
//  101010 xchihoa     Ported to Linux for APG43L.
//******************************************************************************


#ifndef CPS_BUAP_UINT32_H
#define CPS_BUAP_UINT32_H

#include "CPS_BUAP_Buffer.h"

class CPS_BUAP_UINT32
{
public:

  CPS_BUAP_UINT32();
  // Description:
  //    Class constructor
  // Parameters:
  //    -
  // Return value:
  //    -
  // Additional information:
  //    -
 
  CPS_BUAP_UINT32(unsigned long value);
  // Description:
  //    Class constructor
  // Parameters:
  //    value                   value, in
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

  CPS_BUAP_UINT32& operator<<(unsigned long val);
  // Description:
  //    Input operator
  // Parameters:
  //    val                     character to be added to the buffer, in
  // Return value:
  //    CPS_BUAP_UINT32&        reference to this class
  // Additional information:
  //    -
 
  CPS_BUAP_UINT32& operator>>(unsigned long& val);
  // Description:
  //    Ouput operator
  // Parameters:
  //    val&                    character reference, out
  // Return value:
  //    CPS_BUAP_UINT32&        reference to this class
  // Additional information:
  //    -

  CPS_BUAP_UINT32& operator<<(CPS_BUAP_Buffer& buf);
  // Description:
  // 	Input operator, for reading value from a buffer
  // Parameters: 
  //    CPS_BUAP_Buffer&	reference to buffer, in 
  // Return value: 
  //    CPS_BUAP_UINT32& 	reference to this class
  // Additional information:
  //    - 

  CPS_BUAP_UINT32& operator>>(CPS_BUAP_Buffer& buf);
  // Description:
  // 	Output operator, for writing value to a buffer
  // Parameters: 
  //    CPS_BUAP_Buffer&	reference to buffer, in 
  // Return value: 
  //    CPS_BUAP_UINT32& 	reference to this class
  // Additional information:
  //    - 


protected:

private:
  unsigned long m_Value;	// actual numeric value
};

#endif
