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
// CPS_BUAP_Service.h
//
// DESCRIPTION
// Header file for CPS_BUAP_Service.cpp.
//
// DOCUMENT NO
// 190 89-CAA 109 1413
//
// AUTHOR
// 120102 XDT/DEK XNGUDAN
//
//******************************************************************************
// === Revision history ===
// 120102 XNGUDAN Created.
// 120327 XNGUDAN Updated with the Object Implementer.
//******************************************************************************


#ifndef CPS_BUAP_Service_H
#define CPS_BUAP_Service_H

#include "ACS_APGCC_ApplicationManager.h"
#include "CPS_BUAP_PARMGR.h"
#include "CPS_BUAP_Loader.h"
#include "CPS_BUAP_FILEMGR.h"
#include "CPS_BUAP_Config.h"
#include "CPS_BUAP_Trace.h"
#include "CPS_BUAP_Events.h"
#include "CPS_BUAP_PARMGR_Global.h"
#include "CPS_BUAP_Linux.h"
#include "StaticParamsOI.h"
#include "ClassicOI.h"

#include <boost/thread/thread.hpp>
#include <string>

class CPS_BUAP_Service : public ACS_APGCC_ApplicationManager
{
public:
    static const std::string BUAP_DEF_DAEMON_NAME;

public:
    /**
     * @brief: Constructor of BUAP service
     *
     * @param: daemonName
     * @param: parServer
     * @param: loadServer
     * @param: fileServer
     */
    CPS_BUAP_Service(const std::string& daemonName, CPS_BUAP_PARMGR& parServer, CPS_BUAP_Loader& loadServer,
                     CPS_BUAP_FILEMGR& fileServer, StaticParamsOI& staticParams, ClassicOI& classic);

    /**
     * @brief: Destructor
     *
     */
    virtual ~CPS_BUAP_Service();


    /**
     * Start service & event loop for AMF events
     *
     * @param:  N/A
     * @return: N/A
     */
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

    /**
     * Start BUAP Server
     *
     * @param:  N/A
     * @return: N/A
     */
    void start();

    /**
     * Stop BUPAP Server
     *
     * @param:  N/A
     * @return: N/A
     */
    void stop();

private:
    // BUAP Server
    CPS_BUAP_PARMGR&   m_parServer;
    CPS_BUAP_Loader&   m_loadServer;
    CPS_BUAP_FILEMGR&  m_fileServer;
    StaticParamsOI&    m_staticParamsOI;
    ClassicOI&         m_classicOI;

    // Threads
    boost::thread m_parServerThread;
    boost::thread m_loadServerThread;
    boost::thread m_fileServerThread;
    boost::thread m_staticParamsThread;
    boost::thread m_classicThread;

    // Stop Flag
    bool m_stop;

    // Active Flag;
    bool m_active;
};


#endif /* CPS_BUAP_Service.h */
