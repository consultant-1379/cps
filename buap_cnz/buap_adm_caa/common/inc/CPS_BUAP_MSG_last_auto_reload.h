
//******************************************************************************
// 
// .NAME
//  	CPS_BUAP_MSG_last_auto_reload - AP->AP Message Last Auto Reload
// .LIBRARY 3C++
// .PAGENAME CPS_BUAP_MSG_last_auto_reload
// .HEADER  CPS  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE CPS_BUAP_MSG_last_auto_reload.H

// .COPYRIGHT
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
//	This class is used to read the last auto reload record.
//	It is derived from CPS_BUAP_MSG. 

// .ERROR HANDLING
//
//	General rule:
//	The error handling is specified for each method.
//
//	No methods initiate or send error reports unless specified.

// DOCUMENT NO
//  190 89-CAA 109 1412

// AUTHOR 
// 	1997/08/06 by U/B/SD David Wade

// .LINKAGE
//	-

// .SEE ALSO 
// 	CPS_BUAP_MSG
//  101010 xchihoa     Ported to Linux for APG43L.
//******************************************************************************

#ifndef CPS_BUAP_MSG_last_auto_reload_H
#define CPS_BUAP_MSG_last_auto_reload_H

#include "CPS_BUAP_UINT8.h"
#include "CPS_BUAP_OPT_UINT8.h"
#include "CPS_BUAP_OPT_UINT32.h"

#include "CPS_BUAP_MSG.h"

class CPS_BUAP_MSG_last_auto_reload : public CPS_BUAP_MSG
{
public:
  enum 
  {
    eManual = 0, 
    eAutomatic = 1
  };

  CPS_BUAP_UINT8          m_Validity;		// Validity indication of this record (read only)
  CPS_BUAP_OPT_UINT8      m_ManAuto;		// Manual (1) or Automatic (0) reload
  CPS_BUAP_OPT_UINT8      m_LRG;		// Generation file

  CPS_BUAP_OPT_UINT8      m_OPSRScentury;	// Century PSRS was output
  CPS_BUAP_OPT_UINT8      m_OPSRSyear;		// year PSRS was output
  CPS_BUAP_OPT_UINT8      m_OPSRSmonth;		// month PSRS was output
  CPS_BUAP_OPT_UINT8      m_OPSRSday;		// day PSRS was output
  CPS_BUAP_OPT_UINT8      m_OPSRShour;		// hour PSRS was output
  CPS_BUAP_OPT_UINT8      m_OPSRSminute;	// minute PSRS was output

  CPS_BUAP_OPT_UINT8      m_ODSScentury;	// century DS small dump was output
  CPS_BUAP_OPT_UINT8      m_ODSSyear;		// year DS small dump was output
  CPS_BUAP_OPT_UINT8      m_ODSSmonth;		// month DS small dump was output
  CPS_BUAP_OPT_UINT8      m_ODSSday;		// day DS small dump was output
  CPS_BUAP_OPT_UINT8      m_ODSShour;		// hour DS small dump was output
  CPS_BUAP_OPT_UINT8      m_ODSSminute;		// minute DS small dump was output

  CPS_BUAP_OPT_UINT8      m_ODSLcentury;	// century DS large was output
  CPS_BUAP_OPT_UINT8      m_ODSLyear;		// year DS large dump was output
  CPS_BUAP_OPT_UINT8      m_ODSLmonth;		// month DS large dump was output
  CPS_BUAP_OPT_UINT8      m_ODSLday;		// day DS large dump was output
  CPS_BUAP_OPT_UINT8      m_ODSLhour;		// hour DS large dump was output
  CPS_BUAP_OPT_UINT8      m_ODSLminute;		// minute DS large dump was output

  CPS_BUAP_OPT_UINT8      m_LRGF;		// Last possible generation file (true/false)
  CPS_BUAP_OPT_UINT8      m_NCT;		// Number of current truncation attempt
  CPS_BUAP_OPT_UINT8      m_Omission;		// Omission
  CPS_BUAP_OPT_UINT8      m_SUP;		// Supervision time in tens of minutes
  CPS_BUAP_OPT_UINT32     m_AssClogNum;		// Associate command log number


  CPS_BUAP_MSG_last_auto_reload();
  // Description:
  // 	Class constructor
  // Parameters: 
  //	-
  // Return value: 
  //	-
  // Additional information:
  //	-


  CPS_BUAP_MSG_last_auto_reload(unsigned short msgID);
  // Description:
  // 	Class constuctor with message identity
  // Parameters: 
  //    msgId			identity of message (userdata1), in 
  // Return value: 
  //	-
  // Additional information:
  //	-


  virtual ~CPS_BUAP_MSG_last_auto_reload();
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
