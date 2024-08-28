
//******************************************************************************
//
// NAME
//      %File_name:CPS_BUAP_MSG_static_parameters.C%
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
//      This class is used to read/write the static
//      parameters.  It is derived from CPS_BUAP_MSG.

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
#include "CPS_BUAP_MSG_static_parameters.h"


//******************************************************************************
//	CPS_BUAP_MSG_static_parameters()
//******************************************************************************
CPS_BUAP_MSG_static_parameters::CPS_BUAP_MSG_static_parameters()
{
  newTRACE(("CPS_BUAP_MSG_static_parameters::CPS_BUAP_MSG_static_parameters()", 0))
};

//******************************************************************************
//	CPS_BUAP_MSG_static_parameters()
//******************************************************************************
CPS_BUAP_MSG_static_parameters::CPS_BUAP_MSG_static_parameters(unsigned short msgID)
{
  newTRACE(("CPS_BUAP_MSG_static_parameters::CPS_BUAP_MSG_static_parameters(msgID=%d)", 0, msgID))

  UserData1() = msgID;
};

//******************************************************************************
//	~CPS_BUAP_MSG_static_parameters()
//******************************************************************************
CPS_BUAP_MSG_static_parameters::~CPS_BUAP_MSG_static_parameters()
{
  newTRACE(("CPS_BUAP_MSG_static_parameters::~CPS_BUAP_MSG_static_parameters()", 0))
};

//******************************************************************************
//	Encode()
//******************************************************************************
void CPS_BUAP_MSG_static_parameters::Encode(CPS_BUAP_Buffer& userBuffer)
{
  newTRACE(("CPS_BUAP_MSG_static_parameters::Encode()", 0))

  userBuffer.Reset();

  m_Validity      >> userBuffer;
  m_ManAuto       >> userBuffer;
  m_NTAZ          >> userBuffer;
  m_NTCZ          >> userBuffer;
  m_LOAZ          >> userBuffer;
  m_INCLAcentury  >> userBuffer;
  m_INCLAyear     >> userBuffer;
  m_INCLAmonth    >> userBuffer;
  m_INCLAday      >> userBuffer;
  m_INCL1         >> userBuffer;
  m_INCL2         >> userBuffer;
  m_SUP           >> userBuffer;

};

//******************************************************************************
//	Decode()
//******************************************************************************
void CPS_BUAP_MSG_static_parameters::Decode(CPS_BUAP_Buffer& userBuffer)
{
  newTRACE(("CPS_BUAP_MSG_static_parameters::Decode()", 0))

  userBuffer.Reset();

  m_Validity      << userBuffer;
  m_ManAuto       << userBuffer;
  m_NTAZ          << userBuffer;
  m_NTCZ          << userBuffer;
  m_LOAZ          << userBuffer;
  m_INCLAcentury  << userBuffer;
  m_INCLAyear     << userBuffer;
  m_INCLAmonth    << userBuffer;
  m_INCLAday      << userBuffer;
  m_INCL1         << userBuffer;
  m_INCL2         << userBuffer;
  m_SUP           << userBuffer;
};
