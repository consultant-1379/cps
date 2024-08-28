//******************************************************************************
// COPYRIGHT Ericsson Utvecklings AB, Sweden 2011.
// All rights reserved.
//
// The Copyright to the computer program(s) herein 
// is the property of Ericsson Utvecklings AB, Sweden.
// The program(s) may be used and/or copied only with 
// the written permission from Ericsson Utvecklings AB or in 
// accordance with the terms and conditions stipulated in the 
// agreement/contract under which the program(s) have been 
// supplied.
// 
// NAME
// CPS_BUAP_FILEMGR_Service.h
//
// DESCRIPTION 
// Header file for CPS_BUAP_FILEMGR_Service.cpp.
//
// DOCUMENT NO
// 190 89-CAA 109 1414
//
// AUTHOR 
//  XDT/DEK XNGUDAN
//
//******************************************************************************
// === Revision history ===
// 2011-10-12   XNGUDAN    Create
//******************************************************************************

#ifndef CPS_BUAP_FILEMGR_Service_H
#define CPS_BUAP_FILEMGR_Service_H

#include "ACS_APGCC_ApplicationManager.h"
#include "CPS_BUAP_FILEMGR.h"
#include "boost/thread.hpp"

#include <cstring>

class CPS_BUAP_FILEMGR_Service : public ACS_APGCC_ApplicationManager
{
public:
    static const std::string BUFILEMGR_DEF_DAEMON_NAME;

public:
    CPS_BUAP_FILEMGR_Service(const std::string& daemonName, CPS_BUAP_FILEMGR& server);
    virtual ~CPS_BUAP_FILEMGR_Service();

    void run();

    ACS_APGCC_ReturnType performStateTransitionToActiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
    ACS_APGCC_ReturnType performStateTransitionToPassiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
    ACS_APGCC_ReturnType performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
    ACS_APGCC_ReturnType performStateTransitionToQuiescedJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
    ACS_APGCC_ReturnType performComponentHealthCheck(void);
    ACS_APGCC_ReturnType performComponentTerminateJobs(void);
    ACS_APGCC_ReturnType performComponentRemoveJobs (void);
    ACS_APGCC_ReturnType performApplicationShutdownJobs(void);

private:

    void start();
    void stop();

private:
    // BUFTPD Server
    CPS_BUAP_FILEMGR& m_server;

    // Threads
    boost::thread m_server_thread;

    // Stop Flag
    bool m_stop;

    // Active Flag;
    bool m_active;
};

#endif
