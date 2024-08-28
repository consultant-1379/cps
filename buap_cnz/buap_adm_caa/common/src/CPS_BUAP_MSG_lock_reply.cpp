//******************************************************************************
//
// NAME
//      %File_name:CPS_BUAP_MSG_lock_reply.C%
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
#include "CPS_BUAP_MSG_lock_reply.H"


//******************************************************************************
//	CPS_BUAP_MSG_lock_reply()
//******************************************************************************
CPS_BUAP_MSG_lock_reply::CPS_BUAP_MSG_lock_reply()
{
  newTRACE(("CPS_BUAP_MSG_lock_reply::CPS_BUAP_MSG_lock_reply()", 0))

  UserData1() = CPS_BUAP_MSG_ID_lock_ok;
};

//******************************************************************************
//	~CPS_BUAP_MSG_lock_reply()
//******************************************************************************
CPS_BUAP_MSG_lock_reply::~CPS_BUAP_MSG_lock_reply()
{
  newTRACE(("CPS_BUAP_MSG_lock_reply::~CPS_BUAP_MSG_lock_reply()", 0))
};

//******************************************************************************
//	Encode()
//******************************************************************************
void CPS_BUAP_MSG_lock_reply::Encode(CPS_BUAP_Buffer& userBuffer)
{
  newTRACE(("CPS_BUAP_MSG_lock_reply::Encode()", 0))

  userBuffer.Reset();

  m_LockSetting    >> userBuffer;
};

//******************************************************************************
//	Decode()
//******************************************************************************
void CPS_BUAP_MSG_lock_reply::Decode(CPS_BUAP_Buffer& userBuffer)
{
  newTRACE(("CPS_BUAP_MSG_lock_reply::Decode()", 0))

  userBuffer.Reset();

  m_LockSetting    << userBuffer;
};
