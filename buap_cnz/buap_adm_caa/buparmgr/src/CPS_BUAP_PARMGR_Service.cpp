/*
NAME
    File_name:CPS_BUAP_PARMGR_Service.cpp

Ericsson Utvecklings AB

    COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

    The Copyright to the computer program(s) herein is the property of
    Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
    copied only with the written permission from Ericsson Utvecklings AB
    or in accordance with the terms and conditions stipulated in the
    agreement/contract under which the program(s) have been supplied.

DESCRIPTION
    Class implements the AMF service registration and handling.

DOCUMENT NO
    190 89-CAA 109 1413

AUTHOR
    2011-11-01 by XDT/DEK XNGUDAN


SEE ALSO
    -

Revision history
----------------
2011-11-01 XNGUDAN Created

*/

#include "CPS_BUAP_PARMGR_Service.h"
#include "CPS_BUAP_PARMGR.h"
#include "CPS_BUAP_PARMGR_Global.h"
#include "CPS_BUAP_Trace.h"
#include "CPS_BUAP_Config.h"
#include "CPS_BUAP_Events.h"
#include "CPS_BUAP_Linux.h"

#include <cerrno>

#include <boost/thread/thread.hpp>

#ifdef _DEBUG
//:ag1
bool g_consoleMode = false;
#endif

const std::string CPS_BUAP_PARMGR_Service::BUPARMGR_DEF_DAEMON_NAME = "cps_buapparmgrd";

CPS_BUAP_PARMGR_Service::CPS_BUAP_PARMGR_Service(const string& daemonName, CPS_BUAP_PARMGR& server) :
        ACS_APGCC_ApplicationManager(daemonName.c_str()), m_server(server), m_stop(false), m_active(false)
{
    newTRACE(("CPS_BUAP_PARMGR_Service::CPS_BUAP_PARMGR_Service(%s)", 0, daemonName.c_str()));

}

CPS_BUAP_PARMGR_Service::~CPS_BUAP_PARMGR_Service()
{
    newTRACE(("CPS_BUAP_PARMGR_Service::~CPS_BUAP_PARMGR_Service()", 0));

}

/**
 * Start service & event loop for AMF events
 *
 * @param:  N/A
 * @return: N/A
 */
void CPS_BUAP_PARMGR_Service::run()
{
    newTRACE(("CPS_BUAP_PARMGR_Service::run()", 0));

    ACS_APGCC_HA_ReturnType errorCode = ACS_APGCC_HA_SUCCESS;

    // Start service & event loop for AMF events
    // performStateTransitionToActive/PassiveJobs will be called
    // after this

    errorCode = this->activate();

    TRACE(("CPS_BUAP_PARMGR_Service::run() amf service loop returns", 0));

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

    TRACE(("CPS_BUAP_PARMGR_Service::run() returns", 0));

}

/**
 * Start BUPARMGR Server
 *
 * @param:  N/A
 * @return: N/A
 */
void CPS_BUAP_PARMGR_Service::start()
{
    newTRACE(("CPS_BUAP_PARMGR_Service::start()", 0));

    m_stop = false;

    // Start the BUPARMGR Server in different thread
    m_server_thread = boost::thread(boost::bind(&CPS_BUAP_PARMGR::Run, &m_server));

    // Wait until CPS_BUAP_PARMGR server started in another thread before executing
    m_server.waitUntilRunning();
}

/**
 * Stop BUPARMGR Server
 *
 * @param:  N/A
 * @return: N/A
 */
void CPS_BUAP_PARMGR_Service::stop()
{
    newTRACE(("CPS_BUAP_PARMGR_Service::stop()", 0));

    // Stop the BUPARMGR Server in case it has not been stopped
    if (!m_stop)
    {
        m_server.stop();

    }

    m_stop = true;

    // Wait for all the threads exit before itself exits
    m_server_thread.join();

    TRACE(("CPS_BUAP_PARMGR_Service::stop() returns", 0));
}

ACS_APGCC_ReturnType CPS_BUAP_PARMGR_Service::performStateTransitionToActiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
    newTRACE(("CPS_BUAP_PARMGR_Service::performStateTransitionToActiveJobs(%d)", 0, previousHAState));

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

ACS_APGCC_ReturnType CPS_BUAP_PARMGR_Service::performStateTransitionToPassiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
    newTRACE(("CPS_BUAP_PARMGR_Service::performStateTransitionToPassiveJobs(%d)", 0, previousHAState));

    // Set the state to passive
    m_active = false;

    // Do nothing

    return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType CPS_BUAP_PARMGR_Service::performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
    newTRACE(("CPS_BUAP_PARMGR_Service::performStateTransitionToQueisingJobs(%d)", 0, previousHAState));

    m_active = false;
    this->stop();

    return ACS_APGCC_SUCCESS;

}

ACS_APGCC_ReturnType CPS_BUAP_PARMGR_Service::performStateTransitionToQuiescedJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
    newTRACE(("CPS_BUAP_PARMGR_Service::performStateTransitionToQuiescedJobs(%d)", 0, previousHAState));

    m_active = false;
    this->stop();

    return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType CPS_BUAP_PARMGR_Service::performComponentHealthCheck(void)
{
    newTRACE(("CPS_BUAP_PARMGR_Service::performComponentHealthCheck()", 0));

    ACS_APGCC_ReturnType rc = ACS_APGCC_SUCCESS;

    // Check whether PARMGR Server is running then respond accordingly
#if 0
    if (m_active && !m_server.isRunning())
    {
        rc = ACS_APGCC_FAILURE;
        TRACE(("HealthCheck failed. Server is not running", 0));

    }
#endif

    return rc;
}

ACS_APGCC_ReturnType CPS_BUAP_PARMGR_Service::performComponentTerminateJobs(void)
{
    newTRACE(("CPS_BUAP_PARMGR_Service::performComponentTerminateJobs(void)", 0));

    // Application has received terminate component callback due to
    // LOCK-INST admin opreration perform on SU.

    // Do nothing because it already done in RemoveJobs
    return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType CPS_BUAP_PARMGR_Service::performComponentRemoveJobs(void)
{
    newTRACE(("CPS_BUAP_PARMGR_Service::performComponentRemoveJobs(void)", 0));

    // Application has received Removal callback. State of the application
    // is neither Active nor Standby. This is with the result of LOCK admin operation
    // performed on our SU.

    // Stop the PARMGR Service
    this->stop();

    return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType CPS_BUAP_PARMGR_Service::performApplicationShutdownJobs(void)
{
    newTRACE(("CPS_BUAP_PARMGR_Service::performApplicationShutdownJobs(void)", 0));

    this->stop();

    return ACS_APGCC_SUCCESS;
}


