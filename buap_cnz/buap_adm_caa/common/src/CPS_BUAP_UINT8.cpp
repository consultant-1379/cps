//******************************************************************************
//
// NAME
//      %File_name:CPS_BUAP_UINT8.C%
//
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
//      This class acts as a container for an AXE unsigned 8 bit
//      integer.  The class has input/output operators defined
//      that enable the integer value to be written to and read
//      from a CPS_BUAP_Buffer in the byte order consistant with AXE.

// DOCUMENT NO
//  190 89-CAA 109 1412

// AUTHOR 
// 	1997/08/06 by U/B/SD David Wade

// SEE ALSO 
// 	<Related classes, (abstract) baseclasses, friend classes, ...>
//  101010 xchihoa     Ported to Linux for APG43L.
//
//******************************************************************************


#include "CPS_BUAP_Trace.h"
#include "CPS_BUAP_UINT8.h"


//******************************************************************************
//	Constructor()
//******************************************************************************
CPS_BUAP_UINT8::CPS_BUAP_UINT8()
{
  //newTRACE(("CPS_BUAP_UINT8::CPS_BUAP_UINT8()", 0))

  m_Value = 0;
};


//******************************************************************************
//	Constructor()
//******************************************************************************
CPS_BUAP_UINT8::CPS_BUAP_UINT8(unsigned char ch)
{
  //newTRACE(("CPS_BUAP_UINT8::CPS_BUAP_UINT8(ch=%d", 0, ch))

  m_Value = ch;
};


//******************************************************************************
//	operator<<
//******************************************************************************
CPS_BUAP_UINT8& CPS_BUAP_UINT8::operator<<(unsigned char ch)
{
  //newTRACE(("CPS_BUAP_UINT8::operator<<", 0))

  m_Value = ch;

  return *this;
};


//******************************************************************************
//	operator>>
//******************************************************************************
CPS_BUAP_UINT8& CPS_BUAP_UINT8::operator>>(unsigned char& ch)
{
  //newTRACE(("CPS_BUAP_UINT8::operator>>", 0))

  ch = m_Value;

  return *this;
};


//******************************************************************************
//	operator<<
//******************************************************************************
CPS_BUAP_UINT8& CPS_BUAP_UINT8::operator<<(CPS_BUAP_Buffer& buf)
{
  //newTRACE(("CPS_BUAP_UINT8::operator<<", 0))

  buf >> (char &)m_Value;

  return *this;
};


//******************************************************************************
//	operator>>
//******************************************************************************
CPS_BUAP_UINT8& CPS_BUAP_UINT8::operator>>(CPS_BUAP_Buffer& buf)
{
  //newTRACE(("CPS_BUAP_UINT8::operator>>", 0))

  buf << (char)m_Value;

  return *this;
};
