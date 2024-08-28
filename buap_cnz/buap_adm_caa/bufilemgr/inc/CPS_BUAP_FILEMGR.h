//******************************************************************************
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 2011.
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
//  NAME
//  CPS_BUAP_FILEMGR.h
//
//  DESCRIPTION 
//    This class, which inherits from CPS_BUAP_SERVER,
//    implements the controlling logic for the FILEMGR process.
//
//  ERROR HANDLING
//
//    General rule:
//    If and error is detected then a CPS_BUAP_Exception will be thrown
//
//    No methods initiate or send error reports unless specified.
//
//  DOCUMENT NO
//    190 89-CAA 109 1414
//
//  AUTHOR 
//     1999/05/13 by UAB/B/SF Birgit Berggren
//
//******************************************************************************
// === Revision history ===
// 990513 BIR PA1 Created.
// 990608 BIR PA2 Updated.
// 990617 BIR PA3 Updated after review.
// 110912  DANH   Updated (migrated from Win2k3 to Linux)
//******************************************************************************
//
#ifndef CPS_BUAP_FILEMGR_H_
#define CPS_BUAP_FILEMGR_H_

#include "ACS_JTP.h"

#include "CPS_BUAP_EventFD.h"
#include "CPS_BUAP_Exception.h"
#include "CPS_BUAP_Server.h"
#include "CPS_BUAP_MSG_text.h"
#include "CPS_BUAP_DateTime.h"

#include "boost/thread/mutex.hpp"
#include "boost/thread/condition_variable.hpp"
#include "boost/thread/recursive_mutex.hpp"

using namespace CPS_BUAP;
//=============================================================================
// Constants
//=============================================================================
#define ABORT_EVENT               0
#define COMMAND_THREAD            1

#define IDLE                      0   //  Command state, used in m_CmdResult
#define COMMAND_ORDERED           1   //  Command state, used in m_CmdResult

//=============================================================================
// FILEMGR prototypes
//=============================================================================


class CPS_BUAP_FILEMGR: public CPS_BUAP_Server
{

public:

    CPS_BUAP_FILEMGR();
    // Description:
    //     Class constructor
    // Parameters:
    //    -
    // Return value:
    //    -
    // Additional information:
    //    -


    virtual ~CPS_BUAP_FILEMGR();
    // Description:
    //     Class destructor
    // Parameters:
    //    -
    // Return value:
    //    -
    // Additional information:
    //    -


    void Run();
    // Description:
    //     Method implementing the main control loop
    // Parameters:
    //    -
    // Return value:
    //    -
    // Additional information:
    //    -


    void GetTime(const std::string &filename, DateTime &time);
    // Description:
    //     Method for reading the time from an R0 file
    // Parameters:
    //    filename        physical filename
    //    time            time to be returned
    // Return value:
    //    -
    // Additional information:
    //    -


    void stop();
    // Description:
    //     Method for stop the bufilemgr server
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
    CPS_BUAP_FILEMGR(const CPS_BUAP_FILEMGR&);
    // Description:
    //     Copy constructor
    // Parameters:
    //    -
    // Return value:
    //    -
    // Additional information:
    //    Declared to disallow copying

    CPS_BUAP_FILEMGR& operator=(const CPS_BUAP_FILEMGR&);
    // Description:
    //     Assignment operator
    // Parameters:
    //    -
    // Return value:
    //    -
    // Additional information:
    //    Declared to disallow copying

private:

    DWORD m_FaultCode;              // Used in CmdThread for result of
                                    // command execution
    DWORD m_CmdResult;              // Used in MainThread for result of
                                    // command
    char m_Command[16];             // Used in result printout
    char m_CommandOrder[16];        // The order to the commandthread
    ACS_JTP_Service *m_pService;    // Pointer to JTP service object

    CPS_BUAP_EventFD m_commandThreadEvent;      // Event raise for command execution

    CPS_BUAP_EventFD m_stopEvent;               // Stop Event

    bool m_running;                    // Return state of server

    boost::mutex      m_mutex;        // For condition variable
    boost::condition_variable m_condition;

private:

    void RXTX(HANDLE handle, unsigned short rxUserData1,
            unsigned short rxUserData2, CPS_BUAP_Buffer &rxBuffer,
            unsigned short &txUserData1, unsigned short &txUserData2,
            CPS_BUAP_Buffer &txBuffer);
    // Description:
    //     Method used to encode the message fields into a buffer.
    // Parameters:
    //    handle            JTP handle message received on, in
    //    rxUserData1        received userdata1, in
    //    rxUserData2        received userdata2, in
    //    rxBuffer        received buffer, in
    //    txUserData1        userdata1 to be transmitted, out
    //    txUserData1        userdata2 to be transmitted, out
    //    txBuffer        user buffer to be transmitted, out
    // Return value:
    //    -
    // Additional information:
    //    -


    DWORD Process_thread_control(DWORD objectHandle);
    // Description:
    //    This method is called for to find out whether a command has been
    //    executed or if the service has been aborted by operator.
    // Parameters:
    //    objectHandle            index in NTObjectArray
    // Return value:
    //    result                  values: NO_ERROR = OK, continue
    //                                    SERVICE_ABORTED = terminate the main
    //                                    control loop
    // Additional information:
    //    -


    ///////////////////////
    //    CP Messages    //
    ///////////////////////

    void Process_new_command(HANDLE handle, CPS_BUAP_MSG_text &rxMsg,
            CPS_BUAP_MSG **txMsg);
    // Description:
    //     Method used to encode the message fields into a buffer.
    // Parameters:
    //    handle            JTP handle on which message was received
    //    rxMsg                CPS_BUAP_MGS_text instance where the command to
    //                            be executed is hold
    //    txMsg                   Reference to a CPS_BUAP_MSG instance where the
    //                            response to the ordered command will be stored
    // Return value:
    //    -
    // Additional information:
    //    -

    bool Process_command(const char command[]);
    // Description:
    //    Method that starts command execution.
    // Parameters:
    //    command                 Command to be executed
    // Return value:
    //    -
    // Additional information:
    //    -

    void Process_SYNIC(const int low, const int high, const char tempName[]);
    // Description:
    //    Method that implements command SYNIC.
    // Parameters:
    //    low                     lowest RELFSW number in actual file range (FFR
    //                            or SFR)
    //    high                    highest RELFSW number in actual file range (FFR
    //                            or SFR)
    //    tempName                name of the temporary RELFSW file (RELFSWX for
    //                            FFR, RELFWSY for SFR)
    // Return value:
    //    -
    // Additional information:
    //    -

    void Process_SYTUC(const int low, const int high, const char tempName[]);
    // Description:
    //    Method that implements command SYTUC.
    // Parameters:
    //    low                     lowest RELFSW number in actual file range (FFR
    //                            or SFR)
    //    high                    highest RELFSW number in actual file range (FFR
    //                            or SFR)
    //    tempName                name of the temporary RELFSW file (RELFSWX for
    //                            FFR, RELFWSY for SFR)
    // Return value:
    //    -
    // Additional information:
    //    -

    void Process_SYBII();
    // Description:
    //    Method that implements command SYBII.
    // Parameters:
    //    -
    // Return value:
    //    -
    // Additional information:
    //    -

    void Process_SYSFT();
    // Description:
    //    Method that implements command SYSFT.
    // Parameters:
    //    -
    // Return value:
    //    -
    // Additional information:
    //    -

    void Process_new_buffer(CPS_BUAP_MSG **txMsg);
    // Description:
    //     Method used when the CP asks for the result of the ordered command.
    // Parameters:
    //    txMsg                   Reference to a CPS_BUAP_MSG instance where the
    //                            response will be stored
    // Return value:
    //    -
    // Additional information:
    //    iEventFd
    //    iEventFd
    //    -


    bool initiateJTPService();
    // Description:
    //  Method to start ACS_JTP Service
    // Parameters:
    //    -
    // Return value
    //    -
    // Additional information:
    //    -

};

#endif
