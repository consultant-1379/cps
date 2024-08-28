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
// CPS_BUAP_FILEMGR_Service.cpp
//
// DESCRIPTION 
// -
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


#include "CPS_BUAP_FILEMGR_Service.h"

#include "CPS_BUAP_Trace.h"
#include "CPS_BUAP_Events.h"

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <cerrno>

#include "boost/thread/mutex.hpp"
#include <boost/thread/thread.hpp>


const std::string CPS_BUAP_FILEMGR_Service::BUFILEMGR_DEF_DAEMON_NAME = "cps_buapfilemgrd";

CPS_BUAP_FILEMGR_Service::CPS_BUAP_FILEMGR_Service(const string& daemonName, CPS_BUAP_FILEMGR& server) :
        ACS_APGCC_ApplicationManager(daemonName.c_str()), m_server(server), m_stop(false), m_active(false)
{
    newTRACE(("CPS_BUAP_FILEMGR_Service::CPS_BUAP_FILEMGR_Service(%s)", 0, daemonName.c_str()));
}

CPS_BUAP_FILEMGR_Service::~CPS_BUAP_FILEMGR_Service()
{
    newTRACE(("CPS_BUAP_FILEMGR_Service::~CPS_BUAP_FILEMGR_Service()", 0));

}

/**
 * Start service & event loop for AMF events
 *
 * @param:  N/A
 * @return: N/A
 */
void CPS_BUAP_FILEMGR_Service::run()
{
    newTRACE(("CPS_BUAP_FILEMGR_Service::run()", 0));

    ACS_APGCC_HA_ReturnType errorCode = ACS_APGCC_HA_SUCCESS;

    // Start service & event loop for AMF events
    // performStateTransitionToActive/PassiveJobs will be called
    // after this

    errorCode = this->activate();

    TRACE(("CPS_BUAP_FILEMGR_Service::run() amf service loop returns", 0));

    if (errorCode == ACS_APGCC_HA_FAILURE)
    {
        TRACE(("HA Activation Failed", 0));
    }

    if (errorCode == ACS_APGCC_HA_FAILURE_CLOSE)
    {
        TRACE(("Failed to Gracefully close", 0));
    }

    if (errorCode == ACS_APGCC_HA_SUCCESS)
    {
        TRACE(("Gracefully closed", 0));
    }

    TRACE(("CPS_BUAP_FILEMGR_Service::run() returns", 0));

}

/**
 * Start BUFILEMGR Server
 *
 * @param:  N/A
 * @return: N/A
 */
void CPS_BUAP_FILEMGR_Service::start()
{
    newTRACE(("CPS_BUAP_FILEMGR_Service::start()", 0));

    m_stop = false;

    // Start the BUFILEMGR Server in different thread
    m_server_thread = boost::thread(boost::bind(&CPS_BUAP_FILEMGR::Run, &m_server));

}

/**
 * Stop BUFILEMGR Server
 *
 * @param:  N/A
 * @return: N/A
 */
void CPS_BUAP_FILEMGR_Service::stop()
{
    newTRACE(("CPS_BUAP_FILEMGR_Service::stop()", 0));

    // Stop the BUFILEMGR Server in case it has not been stopped
    if (!m_stop)
    {
        m_server.stop();
    }
    m_stop = true;

    // Wait for all the threads exit before itself exits
    m_server_thread.join();

    TRACE(("CPS_BUAP_FILEMGR_Service::stop() returns", 0));
}

ACS_APGCC_ReturnType CPS_BUAP_FILEMGR_Service::performStateTransitionToActiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
    newTRACE(("CPS_BUAP_FILEMGR_Service::performStateTransitionToActiveJobs(%d)", 0, previousHAState));

    ACS_APGCC_ReturnType rc = ACS_APGCC_SUCCESS;

    m_active = true;
    // Previous state is ACTIVE, no need to process again
    if (ACS_APGCC_AMF_HA_ACTIVE == previousHAState)
    {
        return rc;
    }

    // Become active, should start server now
    this->start();

    return rc;
}

ACS_APGCC_ReturnType CPS_BUAP_FILEMGR_Service::performStateTransitionToPassiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
    newTRACE(("CPS_BUAP_FILEMGR_Service::performStateTransitionToPassiveJobs(%d)", 0, previousHAState));

    // Set the state to passive
    m_active = false;

    // Do nothing

    return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType CPS_BUAP_FILEMGR_Service::performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
    newTRACE(("CPS_BUAP_FILEMGR_Service::performStateTransitionToQueisingJobs(%d)", 0, previousHAState));

    m_active = false;
    this->stop();

    return ACS_APGCC_SUCCESS;

}

ACS_APGCC_ReturnType CPS_BUAP_FILEMGR_Service::performStateTransitionToQuiescedJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
    newTRACE(("CPS_BUAP_FILEMGR_Service::performStateTransitionToQuiescedJobs(%d)", 0, previousHAState));

    m_active = false;
    this->stop();

    return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType CPS_BUAP_FILEMGR_Service::performComponentHealthCheck(void)
{
    newTRACE(("CPS_BUAP_FILEMGR_Service::performComponentHealthCheck()", 0));

    ACS_APGCC_ReturnType rc = ACS_APGCC_SUCCESS;

    // Check whether IPNAADM Server is running then respond accordingly
/*  TODO: check
    if (m_active && !m_server.isRunning())
    {
        rc = ACS_APGCC_FAILURE;
        //TRACE(("HealthCheck failed. Server is not running", 0));

    }
*/
    return rc;
}

ACS_APGCC_ReturnType CPS_BUAP_FILEMGR_Service::performComponentTerminateJobs(void)
{
    newTRACE(("CPS_BUAP_FILEMGR_Service::performComponentTerminateJobs(void)", 0));

    // Application has received terminate component callback due to
    // LOCK-INST admin opreration perform on SU.

    // Do nothing because it already done in RemoveJobs
    return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType CPS_BUAP_FILEMGR_Service::performComponentRemoveJobs(void)
{
    newTRACE(("CPS_BUAP_FILEMGR_Service::performComponentRemoveJobs(void)", 0));

    // Application has received Removal callback. State of the application
    // is neither Active nor Standby. This is with the result of LOCK admin operation
    // performed on our SU.

    // Stop the IPNAADM Service
    this->stop();

    return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType CPS_BUAP_FILEMGR_Service::performApplicationShutdownJobs(void)
{
    newTRACE(("CPS_BUAP_FILEMGR_Service::performApplicationShutdownJobs(void)", 0));

    this->stop();

    return ACS_APGCC_SUCCESS;
}
