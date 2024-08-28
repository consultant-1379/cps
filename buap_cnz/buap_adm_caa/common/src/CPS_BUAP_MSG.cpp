//******************************************************************************
//
// NAME
//      %File_name:CPS_BUAP_MSG.C%
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
//      This class acts as a base class for all messages sent
//      over JTP.  The operators >> and << are defined for the
//      encoding and decoding of the message to a buffer.

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
#include "CPS_BUAP_Exception.h"

#include "CPS_BUAP_MSG.h"


//******************************************************************************
//	CPS_BUAP_MSG()
//******************************************************************************
CPS_BUAP_MSG::CPS_BUAP_MSG(): m_UserData1(0), m_UserData2(0)
{
  newTRACE(("CPS_BUAP_MSG::CPS_BUAP_MSG()", 0))
}

//******************************************************************************
//	CPS_BUAP_MSG()
//******************************************************************************
CPS_BUAP_MSG::CPS_BUAP_MSG(unsigned short msgID)
{
  newTRACE(("CPS_BUAP_MSG::CPS_BUAP_MSG(msgID=%d)", 0, msgID))

  m_UserData1=msgID;
  m_UserData2=1;
};

//******************************************************************************
//	~CPS_BUAP_MSG()
//******************************************************************************
CPS_BUAP_MSG::~CPS_BUAP_MSG()
{
  newTRACE(("CPS_BUAP_MSG::~CPS_BUAP_MSG()", 0))
};

//******************************************************************************
//	UserData1()
//******************************************************************************
unsigned short& CPS_BUAP_MSG::UserData1()
{
  newTRACE(("CPS_BUAP_MSG::UserData1()", 0))

  return m_UserData1;
};

//******************************************************************************
//	UserData2()
//******************************************************************************
unsigned short& CPS_BUAP_MSG::UserData2()
{
  newTRACE(("CPS_BUAP_MSG::UserData2()", 0))

  return m_UserData2;
};

//******************************************************************************
//	Encode()
//******************************************************************************
void CPS_BUAP_MSG::Encode(CPS_BUAP_Buffer& userBuffer)
{
  newTRACE(("CPS_BUAP_MSG::Encode()", 0))

  // Must reset the buffer index
  userBuffer.Reset();
}

//******************************************************************************
//	Decode()
//******************************************************************************
void CPS_BUAP_MSG::Decode(CPS_BUAP_Buffer& userBuffer)
{
  newTRACE(("CPS_BUAP_MSG::Decode()", 0))

  // Must reset the buffer index
  userBuffer.Reset();
}
