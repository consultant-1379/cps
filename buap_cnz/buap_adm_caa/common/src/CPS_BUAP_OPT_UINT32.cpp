
//******************************************************************************
//
// NAME
//      %File_name:CPS_BUAP_OPT_UINT32.C%
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
//      This class acts as a container for an OPT unsigned 32 bit
//      integer.  The class has input/output operators defined
//      that enable the integer value to be written to and read
//      from a CPS_BUAP_Buffer in the byte order consistant with OPT.

// DOCUMENT NO
//  190 89-CAA 109 1412

// AUTHOR 
// 	1997/08/06 by U/B/SD David Wade

// SEE ALSO 
// 	<Related classes, (abstract) baseclasses, friend classes, ...>
//  101010 xchihoa     Ported to Linux for APG43L.
//
//******************************************************************************
#include <netinet/in.h>

#include "CPS_BUAP_Trace.h"
#include "CPS_BUAP_OPT_UINT32.h"


//******************************************************************************
//	Constructor()
//******************************************************************************
CPS_BUAP_OPT_UINT32::CPS_BUAP_OPT_UINT32()
{
  //newTRACE(("CPS_BUAP_OPT_UINT32::CPS_BUAP_OPT_UINT32()", 0))

  m_Value = 0;
  m_Valid = notValid;
};


//******************************************************************************
//	Constructor()
//******************************************************************************
CPS_BUAP_OPT_UINT32::CPS_BUAP_OPT_UINT32(unsigned long value)
{
  //newTRACE(("CPS_BUAP_OPT_UINT32::CPS_BUAP_OPT_UINT32(value=%d)", 0, value))

  m_Value = value;
  m_Valid = isValid;
};


//******************************************************************************
//	operator<<
//******************************************************************************
CPS_BUAP_OPT_UINT32& CPS_BUAP_OPT_UINT32::operator<<(unsigned long value)
{
  //newTRACE(("CPS_BUAP_OPT_UINT32::operator<<", 0))

  m_Value = value;
  m_Valid = isValid;

  return *this;
};


//******************************************************************************
//	operator>>
//******************************************************************************
CPS_BUAP_OPT_UINT32& CPS_BUAP_OPT_UINT32::operator>>(unsigned long& value)
{
  //newTRACE(("CPS_BUAP_OPT_UINT32::operator>>", 0))

  if (m_Valid)
    {
      value = m_Value;
    }

  return *this;
};

//******************************************************************************
//	operator<<
//******************************************************************************
CPS_BUAP_OPT_UINT32& CPS_BUAP_OPT_UINT32::operator<<(CPS_BUAP_Buffer& buf)
{
  //newTRACE(("CPS_BUAP_OPT_UINT32::operator<<", 0))

  long net_long;

  char *pChar = (char *) &net_long;

  buf >> (char &)m_Valid;

  if (m_Valid)
    {
      buf >> pChar[0];
      buf >> pChar[1];
      buf >> pChar[2];
      buf >> pChar[3];

      //
      // ensure the bytes are in host byte order
      //
      m_Value = ntohl(net_long);
    }

  return *this;
};


//******************************************************************************
//	operator>>
//******************************************************************************
CPS_BUAP_OPT_UINT32& CPS_BUAP_OPT_UINT32::operator>>(CPS_BUAP_Buffer& buf)
{
  //newTRACE(("CPS_BUAP_OPT_UINT32::operator>>", 0))

  //
  // ensure the bytes are in network byte order
  //
  long net_long = htonl(m_Value);

  char *pChar = (char *) &net_long;

  buf << m_Valid;

  if (m_Valid)
    {
       buf << pChar[0];
       buf << pChar[1];
       buf << pChar[2];
       buf << pChar[3];
    }

  return *this;
};
