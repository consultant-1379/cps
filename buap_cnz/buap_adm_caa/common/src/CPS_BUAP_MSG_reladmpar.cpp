//******************************************************************************
//
// NAME
//      %File_name:CPS_BUAP_MSG_reladmpar.C%
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
//      This class is used to give (certain) contents of the
//      reladmpar file.  It is derived from CPS_BUAP_MSG.

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
#include "CPS_BUAP_MSG_reladmpar.h"


//******************************************************************************
//	CPS_BUAP_MSG_reladmpar()
//******************************************************************************
CPS_BUAP_MSG_reladmpar::CPS_BUAP_MSG_reladmpar()
{
  newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_MSG_reladmpar::CPS_BUAP_MSG_reladmpar()", 0))
};

//******************************************************************************
//	CPS_BUAP_MSG_reladmpar()
//******************************************************************************
CPS_BUAP_MSG_reladmpar::CPS_BUAP_MSG_reladmpar(unsigned short msgID)
{
  newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_MSG_reladmpar::CPS_BUAP_MSG_reladmpar(msgID=%d)", 0, msgID))

  UserData1() = msgID;
};

//******************************************************************************
//	~CPS_BUAP_MSG_reladmpar()
//******************************************************************************
CPS_BUAP_MSG_reladmpar::~CPS_BUAP_MSG_reladmpar()
{
  newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_MSG_reladmpar::~CPS_BUAP_MSG_reladmpar()", 0))
};

//******************************************************************************
//	Encode()
//******************************************************************************
void CPS_BUAP_MSG_reladmpar::Encode(CPS_BUAP_Buffer& userBuffer)
{
  newTRACE(("CPS_BUAP_MSG_reladmpar::Encode()", 0))

  userBuffer.Reset();

  m_ResultCode   >> userBuffer;
  m_Validities   >> userBuffer;
  m_ManAuto      >> userBuffer;
  m_LRG          >> userBuffer;
  m_OPSRSyear    >> userBuffer;
  m_OPSRSmonth   >> userBuffer;
  m_OPSRSday     >> userBuffer;
  m_OPSRShour    >> userBuffer;
  m_OPSRSminute  >> userBuffer;
  m_ODSSyear     >> userBuffer;
  m_ODSSmonth    >> userBuffer;
  m_ODSSday      >> userBuffer;
  m_ODSShour     >> userBuffer;
  m_ODSSminute   >> userBuffer;
  m_ODSLyear     >> userBuffer;
  m_ODSLmonth    >> userBuffer;
  m_ODSLday      >> userBuffer;
  m_ODSLhour     >> userBuffer;
  m_ODSLminute   >> userBuffer;
  m_LRGF         >> userBuffer;
  m_NCT          >> userBuffer;
  m_Omission     >> userBuffer;
  m_SUP          >> userBuffer;
  m_AssClogNum   >> userBuffer;
  m_Current      >> userBuffer;
  m_Prepared     >> userBuffer;
  m_Main         >> userBuffer;
  m_NumMeasures  >> userBuffer;

  for (int i=0; i< m_NumMeasures(); i++)
    {
      m_Measures[i]     >> userBuffer;
    }
};

//******************************************************************************
//	Decode()
//******************************************************************************
void CPS_BUAP_MSG_reladmpar::Decode(CPS_BUAP_Buffer& userBuffer)
{
  newTRACE(("CPS_BUAP_MSG_reladmpar::Decode()", 0))

  userBuffer.Reset();

  m_ResultCode   << userBuffer;
  m_Validities   << userBuffer;
  m_ManAuto      << userBuffer;
  m_LRG          << userBuffer;
  m_OPSRSyear    << userBuffer;
  m_OPSRSmonth   << userBuffer;
  m_OPSRSday     << userBuffer;
  m_OPSRShour    << userBuffer;
  m_OPSRSminute  << userBuffer;
  m_ODSSyear     << userBuffer;
  m_ODSSmonth    << userBuffer;
  m_ODSSday      << userBuffer;
  m_ODSShour     << userBuffer;
  m_ODSSminute   << userBuffer;
  m_ODSLyear     << userBuffer;
  m_ODSLmonth    << userBuffer;
  m_ODSLday      << userBuffer;
  m_ODSLhour     << userBuffer;
  m_ODSLminute   << userBuffer;
  m_LRGF         << userBuffer;
  m_NCT          << userBuffer;
  m_Omission     << userBuffer;
  m_SUP          << userBuffer;
  m_AssClogNum   << userBuffer;
  m_Current      << userBuffer;
  m_Prepared     << userBuffer;
  m_Main         << userBuffer;
  m_NumMeasures  << userBuffer;

  for (int i=0; i< m_NumMeasures(); i++)
    {
      m_Measures[i]     << userBuffer;
    }
};
