//******************************************************************************
//
// NAME
//      %File_name:CPS_BUAP_MSG_dynamic_parameters.C%
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
//      This class is used to read the dynamic parameters.
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


#include "CPS_BUAP_Trace.h"
#include "CPS_BUAP_MSG_dynamic_parameters.h"


//******************************************************************************
//	CPS_BUAP_MSG_dynamic_parameters()
//******************************************************************************
CPS_BUAP_MSG_dynamic_parameters::CPS_BUAP_MSG_dynamic_parameters()
{
  newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_MSG_dynamic_parameters::CPS_BUAP_MSG_dynamic_parameters()", 0))
};

//******************************************************************************
//	CPS_BUAP_MSG_dynamic_parameters()
//******************************************************************************
CPS_BUAP_MSG_dynamic_parameters::CPS_BUAP_MSG_dynamic_parameters(unsigned short msgID)
{
  newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_MSG_dynamic_parameters::CPS_BUAP_MSG_dynamic_parameters(msgID=%d)", 0,msgID))

  UserData1() = msgID;
};

//******************************************************************************
//	~CPS_BUAP_MSG_dynamic_parameters()
//******************************************************************************
CPS_BUAP_MSG_dynamic_parameters::~CPS_BUAP_MSG_dynamic_parameters()
{
  newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_MSG_dynamic_parameters::~CPS_BUAP_MSG_dynamic_parameters()", 0))
};

//******************************************************************************
//	Encode()
//******************************************************************************
void CPS_BUAP_MSG_dynamic_parameters::Encode(CPS_BUAP_Buffer& userBuffer)
{
  newTRACE(("CPS_BUAP_MSG_dynamic_parameters::Encode()", 0))

  userBuffer.Reset();

  m_Validity    >> userBuffer;
  m_State       >> userBuffer;
  m_CTA         >> userBuffer;
  m_NumFFR      >> userBuffer;
  m_NumSFR      >> userBuffer;
  m_NOld        >> userBuffer;
  m_KOld        >> userBuffer;
  m_NCT         >> userBuffer;
  m_EXPcentury  >> userBuffer;
  m_EXPyear     >> userBuffer;
  m_EXPmonth    >> userBuffer;
  m_EXPday      >> userBuffer;
  m_EXPhour     >> userBuffer;
  m_EXPminute   >> userBuffer;
  m_LPF         >> userBuffer;
  m_Omission    >> userBuffer;
};

//******************************************************************************
//	Decode()
//******************************************************************************
void CPS_BUAP_MSG_dynamic_parameters::Decode(CPS_BUAP_Buffer& userBuffer)
{
  newTRACE(("CPS_BUAP_MSG_dynamic_parameters::Decode()", 0))

  userBuffer.Reset();

  m_Validity    << userBuffer;
  m_State       << userBuffer;
  m_CTA         << userBuffer;
  m_NumFFR      << userBuffer;
  m_NumSFR      << userBuffer;
  m_NOld        << userBuffer;
  m_KOld        << userBuffer;
  m_NCT         << userBuffer;
  m_EXPcentury  << userBuffer;
  m_EXPyear     << userBuffer;
  m_EXPmonth    << userBuffer;
  m_EXPday      << userBuffer;
  m_EXPhour     << userBuffer;
  m_EXPminute   << userBuffer;
  m_LPF         << userBuffer;
  m_Omission    << userBuffer;
};
