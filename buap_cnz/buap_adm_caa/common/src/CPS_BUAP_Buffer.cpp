
//******************************************************************************
//
// NAME
//      %File_name:CPS_BUAP_Buffer.C%
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
//      This class implments a buffer.  Operators are defined
//      for adding byte values to and removing them from the
//      buffer. Other methods are defined for resetting (emptying)
//      and returning the length of the buffer.
//
//      Note that two constructors are provided.

// DOCUMENT NO
//  190 89-CAA 109 1412

// AUTHOR 
// 	1997/08/06 by U/B/SD David Wade

// SEE ALSO 
// 	<Related classes, (abstract) baseclasses, friend classes, ...>
//
//******************************************************************************
// === Revision history ===
// 970806 David Wade  Created.
// 061003 uablan      Added [] after delete in destructor.
// 101010 xchihoa     Ported to Linux for APG43L.
//******************************************************************************

#include <assert.h>
#include "CPS_BUAP_Trace.h"
#include "CPS_BUAP_Buffer.h"


//******************************************************************************
//	CPS_BUAP_Buffer()
//******************************************************************************
CPS_BUAP_Buffer::CPS_BUAP_Buffer(int size)
  : m_Size(size), m_Buffer(new char[m_Size]), m_Delete(1)
{
  newTRACE(("CPS_BUAP_Buffer::CPS_BUAP_Buffer(size=%d)", 0, size))
  m_Index = 0;
  m_Length = 0;
};

//******************************************************************************
//	CPS_BUAP_Buffer()
//******************************************************************************
CPS_BUAP_Buffer::CPS_BUAP_Buffer(int size, char *buffer)
  : m_Size(size), m_Buffer(buffer), m_Delete(0)
{
  newTRACE(("CPS_BUAP_Buffer::CPS_BUAP_Buffer(size=%d, buffer=%p)", 0,size, buffer))

  // This form of constructor used when creating a buffer from an already
  // encoded byte array.  So set m_Length to show the number of encoded bytes.
  m_Index  = 0; 
  m_Length = size; 
};

//******************************************************************************
//	~CPS_BUAP_Buffer()
//******************************************************************************
CPS_BUAP_Buffer::~CPS_BUAP_Buffer()
{
  newTRACE(("CPS_BUAP_Buffer::~CPS_BUAP_Buffer()", 0))

  // if buffer was allocated by this class then 
  if (m_Delete)
    {
      TRACE(("Deleting buffer", 0))
      delete [] m_Buffer;
    }
};


//******************************************************************************
//	operator<<
//******************************************************************************
CPS_BUAP_Buffer& CPS_BUAP_Buffer::operator<<(char ch)
{
  //newTRACE(("CPS_BUAP_Buffer::operator<<(buffer[%3d] << %02x)", 0, m_Index, ch))
  assert(m_Index < m_Size);
  m_Buffer[m_Index++] = ch;
  m_Length = m_Index;
  return *this;
};

//******************************************************************************
//	operator>>
//******************************************************************************
CPS_BUAP_Buffer& CPS_BUAP_Buffer::operator>>(char &ch)
{
  //newTRACE(("CPS_BUAP_Buffer::operator(buffer[%3d] >> %02x)", 0, m_Index, m_Buffer[m_Index]))
  assert(m_Index < m_Size);
  ch  = m_Buffer[m_Index++];
  return *this;
};

//******************************************************************************
//	Data()
//******************************************************************************
char * CPS_BUAP_Buffer::Data()
{
  newTRACE(("CPS_BUAP_Buffer::Data()", 0))
  return (char *)m_Buffer;
};


//******************************************************************************
//	Reset()
//******************************************************************************
void CPS_BUAP_Buffer::Reset()
{
  newTRACE(("CPS_BUAP_Buffer::Reset()", 0))
  m_Index  = 0;
};

//******************************************************************************
//	Length()
//******************************************************************************
int CPS_BUAP_Buffer::Length()
{
  newTRACE(("CPS_BUAP_Buffer::Length(%d)", 0, m_Length))
  return (m_Length);
};
