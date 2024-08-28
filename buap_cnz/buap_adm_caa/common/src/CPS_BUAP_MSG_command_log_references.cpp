//******************************************************************************
//
// NAME
//      %File_name:CPS_BUAP_MSG_command_log_references.C%
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
//      This class is used for the AP message to read/write the command
//      log references.  It is derived from CPS_BUAP_MSG.

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
#include "CPS_BUAP_MSG_command_log_references.h"


//******************************************************************************
//	CPS_BUAP_MSG_command_log_references()
//******************************************************************************
CPS_BUAP_MSG_command_log_references::CPS_BUAP_MSG_command_log_references()
{
  newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_MSG_command_log_references::CPS_BUAP_MSG_command_log_references()", 0))
};

//******************************************************************************
//	CPS_BUAP_MSG_command_log_references()
//******************************************************************************
CPS_BUAP_MSG_command_log_references::CPS_BUAP_MSG_command_log_references(unsigned short msgID)
{
  newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_MSG_command_log_references::CPS_BUAP_MSG_command_log_references(msgID=%d)", 0, msgID))

  UserData1() = msgID;
};

//******************************************************************************
//	~CPS_BUAP_MSG_command_log_references()
//******************************************************************************
CPS_BUAP_MSG_command_log_references::~CPS_BUAP_MSG_command_log_references()
{
  newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_MSG_command_log_references::~CPS_BUAP_MSG_command_log_references()", 0))
};

//******************************************************************************
//	Encode()
//******************************************************************************
void CPS_BUAP_MSG_command_log_references::Encode(CPS_BUAP_Buffer& userBuffer)
{
  newTRACE(("CPS_BUAP_MSG_command_log_references::Encode()", 0))

  userBuffer.Reset();

  m_Validity      >> userBuffer;
  m_Current       >> userBuffer;
  m_Prepared      >> userBuffer;
  m_Main          >> userBuffer;
};

//******************************************************************************
//	Decode()
//******************************************************************************
void CPS_BUAP_MSG_command_log_references::Decode(CPS_BUAP_Buffer& userBuffer)
{
  newTRACE(("CPS_BUAP_MSG_command_log_references::Decode()", 0))

  userBuffer.Reset();

  m_Validity      << userBuffer;
  m_Current       << userBuffer;
  m_Prepared      << userBuffer;
  m_Main          << userBuffer;
};
