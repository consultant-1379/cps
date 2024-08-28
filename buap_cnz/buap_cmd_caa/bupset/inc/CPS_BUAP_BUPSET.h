
//******************************************************************************
// 
// .NAME
//  	CPS_BUAP_BUPSET - Class containing functionality of bupset command
// .LIBRARY 3C++
// .PAGENAME CPS_BUAP_BUPSET
// .HEADER  CPS  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE CPS_BUAP_BUPSET.H

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
//	This class contains the methods required to implement the
//	bupset command.  The first method to be used is Initialise
//	which will create a client of parmgr and initiate a 
//	conversation.  Next, the ParseCommandLine method should 
//	be called.  Finally the ExecuteCommand method should be
//	called.  If an error occurs at any time an exception will
//	be thrown.

// .ERROR HANDLING
//
//	General rule:
//	Any error encountered will result in a CPS_BUAP_Exception
//	being thrown.

// DOCUMENT NO
//	190 89-CAA 109 0082

// AUTHOR 
// 	1997/08/06 by U/B/SD David Wade

// .LINKAGE
//	-

// .SEE ALSO 
// 	-

//******************************************************************************


#ifndef CPS_BUAP_BUPSET_H
#define CPS_BUAP_BUPSET_H

#include "acs_apgcc_omhandler.h"
#include "BUParamsCommon.h"
#include <string>
//#include "CPS_BUAP_Exception.h"
//#include "CPS_BUAP_MSG_static_parameters.h"
//#include "CPS_BUAP_Client.h"

class CPS_BUAP_BUPSET
{
  
public:

  enum eError
  {
    eNoError          = 0,
    eSyntaxError      = 2,
    eInvalidValue     = 3,
    eNoServer         = 16,
    eLoadInProgress   = 17,
    eCPFSerror        = 18,
    eUnknown          = 100,
    eIllegalCommand   = 115
  };

  CPS_BUAP_BUPSET();
  // Description:
  // 	Class constructor
  // Parameters: 
  //    -
  // Return value: 
  //    -
  // Additional information:
  //    -


  virtual ~CPS_BUAP_BUPSET();
  // Description:
  // 	Class destructor
  // Parameters: 
  //    -
  // Return value: 
  //    -
  // Additional information:
  //    -


  //void Initialise();
  // Description:
  // 	Method to create a client of the parmgr process and initiate
  //	a conversation.
  // Parameters: 
  //    -
  // Return value: 
  //    -
  // Additional information:
  //    May throw CPS_BUAP_Exception


  //void ParseCommandLine(int argc, char** argv);
  // Description:
  // 	Method to process the command line options.  Each option passed
  //	is checked.  If any option is found invalid an exception will be
  //	thrown.
  // Parameters: 
  //    argc			Count of the number of command line options
  //	argv			Array of string pointers to the actual options
  // Return value: 
  //    -
  // Additional information:
  //    May throw CPS_BUAP_Exception


  //void ExecuteCommand();
  // Description:
  // 	Method used to execute the command.  If an error is detected this
  //	method will throw an exception.
  // Parameters: 
  //    -
  // Return value: 
  //    -
  // Additional information:
  //    May throw CPS_BUAP_Exception

  /*
   * @brief Function
   */
  bool init();
  bool update();

  void setCLH(uint32_t clh);
  void setINCL1(uint32_t incl1);
  void setINCL2(uint32_t incl2);
  void setINCLA(std::string &incla);
  void setLOAZ(uint32_t loaz);
  void setNTAZ(uint32_t ntaz);
  void setNTCZ(uint32_t ntcz);
  void setSUP(uint32_t sup);

  //bool get(BUParamsCommon::StaticParam_ &param);
  bool doReset();

  int getLastError();
  const std::string& getLastErrorText();

protected:

private:

  CPS_BUAP_BUPSET(const CPS_BUAP_BUPSET& );
  //:ag1
  // Description:
  // 	Copy constructor
  // Parameters: 
  //	-
  // Return value: 
  //	-
  // Additional information:
  //	Declared to disallow copying

  CPS_BUAP_BUPSET& operator=(const CPS_BUAP_BUPSET& );
  //:ag1
  // Description:
  // 	Assignment operator
  // Parameters: 
  //	-
  // Return value: 
  //	-
  // Additional information:
  //	Declared to disallow copying

private:

  //int                             m_Reset;
  //int                             m_Force;
  //CPS_BUAP_Client                *m_pClient;		// pointer to client
  //CPS_BUAP_MSG_static_parameters  m_StaticPars;		// Message sent to parmgr

  int32_t m_clh;
  int32_t m_incl1;
  uint32_t m_incl2;
  std::string m_incla;
  //uint32_t m_isValid;
  //std::string m_lastReloadFile;
  //std::string m_lastReloadTIme;
  int32_t m_loaz;
  uint32_t m_ntaz;
  uint32_t m_ntcz;
  uint32_t m_sup;

  int m_updateFlag;
  bool m_initialized;
  int m_lastErrorCode;
  std::string m_lastErrorText;

  OmHandler m_omHandler;
};

#endif
