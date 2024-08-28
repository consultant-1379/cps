//******************************************************************************
//
// NAME
//      %File_name:CPS_BUAP_TEXT.C%
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
//	190 89-CAA 109 0082

// AUTHOR 
// 	1997/08/06 by U/B/SD David Wade

// SEE ALSO 
// 	<Related classes, (abstract) baseclasses, friend classes, ...>
//
//******************************************************************************

#include <string.h>
#include "CPS_BUAP_Trace.h"
#include "CPS_BUAP_TEXT.h"


//******************************************************************************
//	Constructor()
//******************************************************************************
CPS_BUAP_TEXT::CPS_BUAP_TEXT()
{
  newTRACE(("CPS_BUAP_TEXT::CPS_BUAP_TEXT()", 0))

  m_Value[0] = 0;
};


//******************************************************************************
//	operator<<
//******************************************************************************
CPS_BUAP_TEXT& CPS_BUAP_TEXT::operator<<(const char string[])
{
  newTRACE(("CPS_BUAP_TEXT::operator<<", 0))

  strcpy(m_Value, string);

  return *this;
};


//******************************************************************************
//	operator>>
//******************************************************************************
CPS_BUAP_TEXT& CPS_BUAP_TEXT::operator>>(char string[])
{
  newTRACE(("CPS_BUAP_TEXT::operator>>", 0))

  strcpy(string, m_Value);

  return *this;
};


//******************************************************************************
//	operator<<
//******************************************************************************
CPS_BUAP_TEXT& CPS_BUAP_TEXT::operator<<(CPS_BUAP_Buffer& buf)
{
  newTRACE(("CPS_BUAP_TEXT::operator<<", 0))
 
  int l_length = buf.Length();

  for (int i=0; i<l_length; i++)
    {
      buf >> m_Value[i];
    }

  m_Value[l_length] = 0;

  return *this;
};


//******************************************************************************
//	operator>>
//******************************************************************************
CPS_BUAP_TEXT& CPS_BUAP_TEXT::operator>>(CPS_BUAP_Buffer& buf)
{
  newTRACE(("CPS_BUAP_TEXT::operator>>", 0))

  unsigned char l_length = strlen(m_Value);

  for (int i=0; i<l_length; i++)
    {
      buf << m_Value[i];
    }

  return *this;
};
