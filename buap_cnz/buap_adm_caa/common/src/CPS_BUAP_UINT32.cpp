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
//  CPS_BUAP_UINT32.cpp
//
//  DESCRIPTION 
//  This class acts as a container for an AXE unsigned 32 bit
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
// 990804 BIR Created.
//  101010 xchihoa     Ported to Linux for APG43L.
//******************************************************************************

#include "CPS_BUAP_Linux.h"
#include <netinet/in.h>

#include "CPS_BUAP_Trace.h"
#include "CPS_BUAP_UINT32.h"

 
//******************************************************************************
//      Constructor()
//******************************************************************************
CPS_BUAP_UINT32::CPS_BUAP_UINT32()
{
  //newTRACE(("CPS_BUAP_UINT32::CPS_BUAP_UINT32()", 0))
 
  m_Value = 0;
};
 
 
//******************************************************************************
//      Constructor()
//******************************************************************************
CPS_BUAP_UINT32::CPS_BUAP_UINT32(unsigned long value)
{
  //newTRACE(("CPS_BUAP_UINT32::CPS_BUAP_UINT32(value=%d)", 0, value))
 
  m_Value = value;
};
 
 
//******************************************************************************
//      operator<<
//******************************************************************************
CPS_BUAP_UINT32& CPS_BUAP_UINT32::operator<<(unsigned long value)
{
  //newTRACE(("CPS_BUAP_UINT32::operator<< %u", 0, value))
 
  m_Value = value;
 
  return *this;
};
 
 
//******************************************************************************
//      operator>>
//******************************************************************************
CPS_BUAP_UINT32& CPS_BUAP_UINT32::operator>>(unsigned long& value)
{
  //newTRACE(("CPS_BUAP_UINT32::operator>> %u", 0, m_Value))
 
  value = m_Value;
 
  return *this;
};
 

//******************************************************************************
//	operator<<
//******************************************************************************
CPS_BUAP_UINT32& CPS_BUAP_UINT32::operator<<(CPS_BUAP_Buffer& buf)
{
  //newTRACE(("CPS_BUAP_UINT32::operator<<", 0))

  long net_long;

  char *pChar = (char *) &net_long;

  //:ag1 bugfix - swapped order, endian-conversion is in ntohl
  buf >> pChar[3];
  buf >> pChar[2];
  buf >> pChar[1];
  buf >> pChar[0];

  //
  // ensure the bytes are in host byte order
  //
  m_Value = ntohl(net_long);

  return *this;
};


//******************************************************************************
//	operator>>
//******************************************************************************
CPS_BUAP_UINT32& CPS_BUAP_UINT32::operator>>(CPS_BUAP_Buffer& buf)
{
  //newTRACE(("CPS_BUAP_UINT32::operator>>", 0))

  //
  // ensure the bytes are in network byte order
  //
  long net_long = htonl(m_Value);

  char *pChar = (char *) &net_long;

  //:ag1 bugfix - swapped order, endian-conversion is in htonl
  buf << pChar[3];
  buf << pChar[2];
  buf << pChar[1];
  buf << pChar[0];

  return *this;
};
