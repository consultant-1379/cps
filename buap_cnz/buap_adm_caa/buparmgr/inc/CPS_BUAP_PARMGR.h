//******************************************************************************
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 2011.
//  All rights reserved.
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson Utvecklings AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.
//
//  NAME
//  CPS_BUAP_PARMGR.h
//
//  DESCRIPTION 
//	This class, which inherits from CPS_BUAP_Server,
//	implements the controlling logic for the PARMGR process.
//
//  DOCUMENT NO
//	190 89-CAA 109 1413
//
//  AUTHOR 
// 	1999/07/26 by UAB/B/SF Birgit Berggren
//
//******************************************************************************
// === Revision history ===
// 990726 BIR PA1 Created.
// 050117 UABLAN  Remove CheckLock, m_lockHandle and Process_lock. 
//                lock handling on file reladmpar removed.
// 111101 XNGUDAN Updated (migrated from win2k3 to Linux)
//*************************************************************************
#ifndef CPS_BUAP_PARMGR_H
#define CPS_BUAP_PARMGR_H

#include <ACS_JTP.h>

#include "CPS_BUAP_EventFD.h"
#include "CPS_BUAP_Exception.h"
#include "CPS_BUAP_RPF.h"
#include "CPS_BUAP_Server.h"
#include "CPS_BUAP_MSG_static_parameters.h"
#include "CPS_BUAP_MSG_dynamic_parameters.h"
#include "CPS_BUAP_MSG_last_auto_reload.h"
#include "CPS_BUAP_MSG_command_log_references.h"
#include "CPS_BUAP_MSG_delayed_measures.h"
#include "CPS_BUAP_MSG_write_clog.h"
#include "CPS_BUAP_MSG_write_dmr.h"
#include "CPS_BUAP_MSG_write_reply.h"
#include "CPS_BUAP_MSG_reladmpar.h"
#include "CPS_BUAP_DateTime.h"

#include "boost/thread/mutex.hpp"
#include "boost/thread/condition_variable.hpp"
#include "boost/thread/recursive_mutex.hpp"

using namespace CPS_BUAP;
//=============================================================================
// Constants
//=============================================================================
#define ABORT_EVENT               0


class CPS_BUAP_PARMGR : public CPS_BUAP_Server
{

public:

  CPS_BUAP_PARMGR(const char *filename);
  // Description:
  // 	Class constructor
  // Parameters:      	   
  //	filename		Full path and name of RELADMPAR file
  // Return value: 
  //	-
  // Additional information:
  //	-


  virtual ~CPS_BUAP_PARMGR();
  // Description:
  // 	Class destructor
  // Parameters: 
  //	-
  // Return value: 
  //	-
  // Additional information:
  //	-


  void Run();
  // Description:
  // 	Method implementing the main control loop
  // Parameters: 
  //	-
  // Return value: 
  //	-
  // Additional information:
  //	-


  void stop();
  // Description:
  //     Method for stop the buparmgr server
  // Parameters:
  //    -
  // Return value:
  //    -
  // Additional information:
  //

  bool isRunning() const;
  // Description:
  //     Method for checking status of buparmgr server
  // Parameters:
  //    -
  // Return value:
  //    -
  // Additional information:
  //

  void waitUntilRunning();
  // Description:
  //     Method for blocking the calling thread until buparmgr server finishes start up
  // Parameters:
  //    -
  // Return value:
  //    -
  // Additional information:
  //

private:

  enum eWrongKey{};			                   // internal exceptions
  ACS_JTP_Service *m_pService;		           // Pointer to JTP service object
  CPS_BUAP_RPF    *m_pRPF;		               // Pointer to RELADMPAR file object

  std::string   m_fileName;
  bool          m_running;                     // Return state of server

  boost::mutex      m_mutex;                   // For condition variable
  boost::condition_variable m_condition;

  CPS_BUAP_EventFD m_stopEvent;                // Stop event

private:

  CPS_BUAP_PARMGR(const CPS_BUAP_PARMGR& );
  // Description:
  // 	Copy constructor
  // Parameters: 
  //    -
  // Return value: 
  //    -
  // Additional information:
  //    Declared to disallow copying
  CPS_BUAP_PARMGR& operator=(const CPS_BUAP_PARMGR& );
  // Description:
  // 	Assignment operator
  // Parameters: 
  //    -
  // Return value: 
  //    -
  // Additional information:
  //    Declared to disallow assignment



  virtual void RXTX(HANDLE handle,
                    unsigned short  rxUserData1, 
                    unsigned short  rxUserData2, 
                    CPS_BUAP_Buffer &rxBuffer,
                    unsigned short &txUserData1, 
                    unsigned short &txUserData2, 
                    CPS_BUAP_Buffer &txBuffer);
  // Description:
  // 	Method used to encode the message fields into a buffer.
  // Parameters: 
  //    handle			JTP handle message received on, in
  //    rxUserData1		received userdata1, in
  //    rxUserData2		received userdata2, in
  //    rxBuffer		received buffer, in
  //    txUserData1		userdata1 to be transmitted, out
  //    txUserData1		userdata2 to be transmitted, out
  //    txBuffer		user buffer to be transmitted, out
  // Return value: 
  //	-
  // Additional information:
  //	-
 
  DWORD Process_thread_control(DWORD objectHandle); 
  // Description:
  //    This method is called for to find if the service has been aborted by 
  //    operator.
  // Parameters: 
  //    objectHandle            index in NTObjectArray
  // Return value: 
  //	result                  values: NO_ERROR = OK, continue
  //                                    SERVICE_ABORTED = terminate the main
  //                                    control loop 
  // Additional information:
  //	-
 


  /////////////////////// 
  //    AP Messages    //
  ///////////////////////
 
  void Process_read_static   (CPS_BUAP_MSG **txMsg);
  // Description:
  // 	Method used to read static reload parameters from the RELADMPAR file.
  // Parameters: 
  //    txMsg                   Reference to a CPS_BUAP_MSG instance where the
  //                            static reload parameters will be stored.
  // Return value: 
  //	-
  // Additional information:
  //	-

  void Process_read_dynamic  (CPS_BUAP_MSG **txMsg);
  // Description:
  // 	Method used to read dynamic reload parameters from the RELADMPAR file.
  // Parameters: 
  //    txMsg                   Reference to a CPS_BUAP_MSG instance where the
  //                            dynamic reload parameters will be stored.
  // Return value: 
  //	-
  // Additional information:
  //	-

  void Process_read_LAR      (CPS_BUAP_MSG **txMsg);
  // Description:
  // 	Method used to read information about last automatic reload from the 
  //    RELADMPAR file.
  // Parameters: 
  //    txMsg                   Reference to a CPS_BUAP_MSG instance where the
  //                            information about last automatic reload  will 
  //                            be stored.
  // Return value: 
  //	-
  // Additional information:
  //	-

  void Process_read_clogrefs (CPS_BUAP_MSG **txMsg);
  // Description:
  // 	Method used to read command log references record in the RELADMPAR
  //    file.
  // Parameters: 
  //    txMsg                   Reference to a CPS_BUAP_MSG instance where the
  //                            command log references will be stored.
  // Return value: 
  //	-
  // Additional information:
  //	-

  void Process_read_dmeasures(CPS_BUAP_MSG **txMsg);
  // Description:
  // 	Method used to read delyed measures record in the RELADMPAR file.
  // Parameters: 
  //    txMsg                   Reference to a CPS_BUAP_MSG instance where the
  //                            delayed measures will be stored.
  // Return value: 
  //	-
  // Additional information:
  //	-

  void Process_write_static  (HANDLE handle, CPS_BUAP_MSG_static_parameters  rxMsg, CPS_BUAP_MSG **txMsg);
  // Description:
  // 	Method used to write static reload parameters in the RELADMPAR file.
  // Parameters: 
  //    handle   		JTP handle on which message received, in
  //    rxMsg                   Reference to a CPS_BUAP_MSG instance where the
  //                            static reload parameters is stored.
  //    txMsg                   Reference to a CPS_BUAP_MSG instance where the
  //                            result of the writing will be stored.
  // Return value: 
  //	-
  // Additional information:
  //	-

  void Process_write_dynamic (HANDLE handle, CPS_BUAP_MSG_dynamic_parameters rxMsg, CPS_BUAP_MSG **txMsg);
  // Description:
  // 	Method used to write dynamic reload parameters in the RELADMPAR file.
  // Parameters: 
  //    handle   		JTP handle on which message received, in
  //    rxMsg                   Reference to a CPS_BUAP_MSG instance where the
  //                            dynaic reload parameters is stored.
  //    txMsg                   Reference to a CPS_BUAP_MSG instance where the
  //                            result of the writing will be stored.
  // Return value: 
  //	-
  // Additional information:
  //	-

  void Process_write_LAR     (HANDLE handle, CPS_BUAP_MSG_last_auto_reload   rxMsg, CPS_BUAP_MSG **txMsg);
  // Description:
  // 	Method used to write last automatic reload information in the RELADMPAR 
  //    file.
  // Parameters: 
  //    handle   		JTP handle on which message received, in
  //    rxMsg                   Reference to a CPS_BUAP_MSG instance where the
  //                            last automatic reload information is stored.
  //    txMsg                   Reference to a CPS_BUAP_MSG instance where the
  //                            result of the writing will be stored.
  // Return value: 
  //	-
  // Additional information:
  //	-


  /////////////////////// 
  //    CP Messages    //
  ///////////////////////
 
  void Process_write_clog    (HANDLE handle, CPS_BUAP_MSG_write_clog     rxMsg, CPS_BUAP_MSG **txMsg);
  // Description:
  // 	Method used to write the command log references in the RELADMPAR file.
  // Parameters: 
  //    handle			JTP handle on which message received, in
  //    rxMsg                   Reference to a CPS_BUAP_MSG instance where the
  //                            the command log references are stored.
  //    txMsg                   Reference to a CPS_BUAP_MSG instance where the
  //                            result of the writing will be stored.
  // Return value: 
  // Return value: 
  //	-
  // Additional information:
  //	-

  void Process_write_dmr     (HANDLE handle, CPS_BUAP_MSG_write_dmr      rxMsg, CPS_BUAP_MSG **txMsg);
  // Description:
  // 	Method used to write the delayed measurs information in the RELADMPAR 
  //    file.
  // Parameters: 
  //    handle			JTP handle on which message received, in
  //    rxMsg                   Reference to a CPS_BUAP_MSG instance where the
  //                            the delayed measures information is stored.
  //    txMsg                   Reference to a CPS_BUAP_MSG instance where the
  //                            result of the writing will be stored.
  // Return value: 
  //	-
  // Additional information:
  //	-

  void Process_read_reladmpar(CPS_BUAP_MSG **txMsg);
  // Description:
  // 	Method used to read parameters from the RELADMPAR file.
  // Parameters: 
  //    txMsg                   Reference to a CPS_BUAP_MSG instance where the
  //                            parameters will be stored.
  // Return value: 
  //	-
  // Additional information:
  //	-

  virtual void OnDisconnect(HANDLE handle);
  // 	Method used to pass on a disconnection event
  // Parameters: 
  //    handle			JTP handle message received on, in
  // Return value: 
  //	-
  // Additional information:
  //	-

  void SetupDynamicParameters();
  // 	Method used to setup the dynamic parameters after a write
  //	to the static parameters record.
  // Parameters: 
  //	-
  // Return value: 
  //	-
  // Additional information:
  //	-

  int HighestFile(int low, int high, DateTime &INCLA);
  // 	Method used to determine the highest consecutive Dump
  //	within a range that is newer than INCLA
  // Parameters: 
  //    low                     lowest RELFSW number in actual file range (FFR
  //                            or SFR)
  //    high                    highest RELFSW number in actual file range (FFR
  //                            or SFR)
  //    INCLA                   oldest accepted date for creation of the dump.
  // Return value: 
  //	Highest accepted file number
  // Additional information:
  //	-

  bool initiateJTPService();
  // Description:
  //  Method to start ACS_JTP Service
  // Parameters:
  //    -
  // Return value
  //    -
  // Additional information:
  //

};

#endif
