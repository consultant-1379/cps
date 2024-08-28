//******************************************************************************
//
// NAME
//      %File_name:CPS_BUAP_MSG_delayed_measures.C%
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


#include "CPS_BUAP_MSG_IDs.h"
#include "CPS_BUAP_Trace.h"
#include "CPS_BUAP_RPF.h"
#include "CPS_BUAP_MSG_delayed_measures.h"


//******************************************************************************
//	CPS_BUAP_MSG_delayed_measures()
//******************************************************************************
CPS_BUAP_MSG_delayed_measures::CPS_BUAP_MSG_delayed_measures()
{
  newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_MSG_delayed_measures::CPS_BUAP_MSG_delayed_measures()", 0))

  UserData1() = CPS_BUAP_MSG_ID_read_dmeasures;
};

//******************************************************************************
//	~CPS_BUAP_MSG_delayed_measures()
//******************************************************************************
CPS_BUAP_MSG_delayed_measures::~CPS_BUAP_MSG_delayed_measures()
{
  newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_MSG_delayed_measures::~CPS_BUAP_MSG_delayed_measures()", 0))
};

//******************************************************************************
//	Encode()
//******************************************************************************
void CPS_BUAP_MSG_delayed_measures::Encode(CPS_BUAP_Buffer& userBuffer)
{
  newTRACE(("CPS_BUAP_MSG_delayed_measures::Encode()", 0))

  userBuffer.Reset();

  m_Validity    >> userBuffer;

  for (int i=0; i<MAX_MEASURES; i++)
    {
      m_Measures[i]    >> userBuffer;
    }
};

//******************************************************************************
//	Decode()
//******************************************************************************
void CPS_BUAP_MSG_delayed_measures::Decode(CPS_BUAP_Buffer& userBuffer)
{
  newTRACE(("CPS_BUAP_MSG_delayed_measures::Decode()", 0))

  userBuffer.Reset();

  m_Validity    << userBuffer;

  for (int i=0; i<MAX_MEASURES; i++)
    {
      m_Measures[i]    << userBuffer;
    }
};
