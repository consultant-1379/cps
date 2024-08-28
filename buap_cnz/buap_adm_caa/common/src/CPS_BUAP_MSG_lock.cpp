//******************************************************************************
//
// NAME
//      %File_name:CPS_BUAP_MSG_lock.C%
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
//      This class is used read the delayed measures
//      It is derived from CPS_BUAP_MSG.

// DOCUMENT NO
//  190 89-CAA 109 1412

// AUTHOR 
// 	1997/08/06 by U/B/SD David Wade

// SEE ALSO 
// 	<Related classes, (abstract) baseclasses, friend classes, ...>
//  101010 xchihoa     Ported to Linux for APG43L.
//
//******************************************************************************


#include "CPS_BUAP_MSG_IDs.H"
#include "CPS_BUAP_Trace.H"
#include "CPS_BUAP_RPF.H"
#include "CPS_BUAP_MSG_lock.H"


//******************************************************************************
//	CPS_BUAP_MSG_lock()
//******************************************************************************
CPS_BUAP_MSG_lock::CPS_BUAP_MSG_lock()
{
  newTRACE(("CPS_BUAP_MSG_lock::CPS_BUAP_MSG_lock()", 0))

  UserData1() = CPS_BUAP_MSG_ID_lock;
};

//******************************************************************************
//	~CPS_BUAP_MSG_lock()
//******************************************************************************
CPS_BUAP_MSG_lock::~CPS_BUAP_MSG_lock()
{
  newTRACE(("CPS_BUAP_MSG_lock::~CPS_BUAP_MSG_lock()", 0))
};

//******************************************************************************
//	Encode()
//******************************************************************************
void CPS_BUAP_MSG_lock::Encode(CPS_BUAP_Buffer& userBuffer)
{
  newTRACE(("CPS_BUAP_MSG_lock::Encode()", 0))

  userBuffer.Reset();

  m_Lock    >> userBuffer;
};

//******************************************************************************
//	Decode()
//******************************************************************************
void CPS_BUAP_MSG_lock::Decode(CPS_BUAP_Buffer& userBuffer)
{
  newTRACE(("CPS_BUAP_MSG_lock::Decode()", 0))

  userBuffer.Reset();

  m_Lock    << userBuffer;
};
