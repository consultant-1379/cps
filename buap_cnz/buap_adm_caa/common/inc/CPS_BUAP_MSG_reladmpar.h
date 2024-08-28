//******************************************************************************
//
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 1999
//  All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson Utvecklings AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.
//
//  ESCRIPTION 
//	This class is used to give (certain) contents of the 
//	reladmpar file.  It is derived from CPS_BUAP_MSG. 
//
// DOCUMENT NO
//  190 89-CAA 109 1412
//
// AUTHOR 
// 	1999/07/29 by UAB/B/SF Birgit Berggren
//
//******************************************************************************
// === Revision history ===
// 990729BIR PA1 Created.
//  101010 xchihoa     Ported to Linux for APG43L.
//******************************************************************************

#ifndef CPS_BUAP_MSG_reladmpar_H
#define CPS_BUAP_MSG_reladmpar_H

#include "CPS_BUAP_UINT8.h"
#include "CPS_BUAP_UINT32.h"

#include "CPS_BUAP_MSG.h"
#include "CPS_BUAP_RPF.h"

class CPS_BUAP_MSG_reladmpar : public CPS_BUAP_MSG
{
public:

  enum eResultCode 
  {
    eSuccess       =0, 
    eNotAccessable =1, 
    eFunctionBusy  =2
  };

  CPS_BUAP_UINT8      m_ResultCode;			// Result code from read
  CPS_BUAP_UINT8      m_Validities;			// Encoded validity indicators
  CPS_BUAP_UINT8      m_ManAuto;			// Manual (1) or Automatic (0)
  CPS_BUAP_UINT8      m_LRG;				// Generation file

  CPS_BUAP_UINT8      m_OPSRSyear;			// PSRS output year 0 - 99
  CPS_BUAP_UINT8      m_OPSRSmonth;			// PSRS output month
  CPS_BUAP_UINT8      m_OPSRSday;			// PSRS output day
  CPS_BUAP_UINT8      m_OPSRShour;			// PSRS output hour
  CPS_BUAP_UINT8      m_OPSRSminute;			// PSRS output minute

  CPS_BUAP_UINT8      m_ODSSyear;			// DS small output year
  CPS_BUAP_UINT8      m_ODSSmonth;			// DS small output month
  CPS_BUAP_UINT8      m_ODSSday;			// DS small output day
  CPS_BUAP_UINT8      m_ODSShour;			// DS small output hour
  CPS_BUAP_UINT8      m_ODSSminute;			// DS small output minute

  CPS_BUAP_UINT8      m_ODSLyear;			// DS large output year
  CPS_BUAP_UINT8      m_ODSLmonth;			// DS large output month
  CPS_BUAP_UINT8      m_ODSLday;			// DS large output day
  CPS_BUAP_UINT8      m_ODSLhour;			// DS large output hour
  CPS_BUAP_UINT8      m_ODSLminute;			// DS large output minute

  CPS_BUAP_UINT8      m_LRGF;				// Last possible generation file loaded
  CPS_BUAP_UINT8      m_NCT;				// Number of current truncation attempt
  CPS_BUAP_UINT8      m_Omission;			// Omission
  CPS_BUAP_UINT8      m_SUP;				// Supervision time in tens of minutes
  CPS_BUAP_UINT32     m_AssClogNum;			// Assiciate command log number
  CPS_BUAP_UINT32     m_Current;			// Current command log number
  CPS_BUAP_UINT32     m_Prepared;			// Prepared command log number
  CPS_BUAP_UINT32     m_Main;				// Mainstore command log number
  CPS_BUAP_UINT8      m_NumMeasures;		// Number of (true) delayed measures
  CPS_BUAP_UINT8      m_Measures[MAX_MEASURES];		// Delayed measures that are true


  CPS_BUAP_MSG_reladmpar();
  // Description:
  // 	Class constructor
  // Parameters: 
  //	-
  // Return value: 
  //	-
  // Additional information:
  //	-


  CPS_BUAP_MSG_reladmpar(unsigned short msgID);
  // Description:
  // 	Class constuctor with message identity
  // Parameters: 
  //    msgId			identity of message (userdata1), in 
  // Return value: 
  //	-
  // Additional information:
  //	-


  virtual ~CPS_BUAP_MSG_reladmpar();
  // Description:
  // 	Class destructor
  // Parameters: 
  // Return value: 
  //	-
  // Additional information:
  //	-


  virtual void Encode(CPS_BUAP_Buffer& userBuffer);
  // Description:
  // 	Method used to encode the message fields into a buffer.
  // Parameters: 
  //    userBuffer		Buffer into which message is to be encoded, in
  // Return value: 
  //	-
  // Additional information:
  //	-


  virtual void Decode(CPS_BUAP_Buffer& userBuffer);
  // Description:
  // 	Method used to decode a message from a buffer.
  // Parameters: 
  //    userBuffer		Buffer from which the message is to be decoded, in
  // Return value: 
  //	-
  // Additional information:
  //	-


protected:

private:

};

#endif
