/*
NAME
    File_name:CPS_BUAP_Service.cpp

Ericsson Utvecklings AB

    COPYRIGHT Ericsson Utvecklings AB, Sweden 2012. All rights reserved.

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
    2012-01-03 by XDT/DEK XNGUDAN


SEE ALSO
    -

Revision history
----------------
2012-01-03 XNGUDAN Created
2012-03-27 XNGUDAN Updated with Object Implementer

*/

#include "CPS_BUAP_Service.h"

#include <cerrno>
#include <boost/bind.hpp>


const std::string CPS_BUAP_Service::BUAP_DEF_DAEMON_NAME = "cps_buapd";

//**********************************************************************************************
// CPS_BUAP_Service()
//**********************************************************************************************
CPS_BUAP_Service::CPS_BUAP_Service(const std::string& daemonName, CPS_BUAP_PARMGR& parServer,
        CPS_BUAP_Loader& loadServer, CPS_BUAP_FILEMGR& fileServer, StaticParamsOI& staticParams,
        ClassicOI& classic) :
        ACS_APGCC_ApplicationManager(daemonName.c_str()), m_parServer(parServer), m_loadServer(loadServer),
        m_fileServer(fileServer), m_staticParamsOI(staticParams), m_classicOI(classic),
        m_stop(true), m_active(false)
{
	// change for Tracing order
    //newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_Service::CPS_BUAP_Service(%s)", 0, daemonName.c_str()));

}

//**********************************************************************************************
// ~CPS_BUAP_Service()
//**********************************************************************************************
CPS_BUAP_Service::~CPS_BUAP_Service()
{
	// change for Tracing order
    //newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_Service::~CPS_BUAP_Service()", 0));

}

//**********************************************************************************************
// run()
//**********************************************************************************************
void CPS_BUAP_Service::run()
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_Service::run()", 0));

    ACS_APGCC_HA_ReturnType errorCode = ACS_APGCC_HA_SUCCESS;

    // Start service & event loop for AMF events
    // performStateTransitionToActive/PassiveJobs will be called
    // after this

    errorCode = this->activate();

    TRACE((LOG_LEVEL_INFO, "CPS_BUAP_Service::run() amf service loop returns", 0));

    if (errorCode == ACS_APGCC_HA_FAILURE)
    {
        TRACE((LOG_LEVEL_WARN, "HA Activation Failed", 0));
    }

    if (errorCode == ACS_APGCC_HA_FAILURE_CLOSE)
    {
        TRACE((LOG_LEVEL_WARN, "Failed to Gracefully close", 0));
    }

    if (errorCode == ACS_APGCC_HA_SUCCESS)
    {
        TRACE((LOG_LEVEL_INFO, "Gracefully closed", 0));
    }

    TRACE((LOG_LEVEL_INFO, "CPS_BUAP_Service::run() returns", 0));

}

//**********************************************************************************************
// start()
//**********************************************************************************************
void CPS_BUAP_Service::start()
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_Service::start()", 0));

    m_stop = false;

    // Start Classic OI in different thread
    m_classicThread = boost::thread(boost::bind(&ClassicOI::run, &m_classicOI));
    // Wait until Classic OI started in another thread before executing
    m_classicOI.waitUntilRunning();

    // Start StaticOI in different thread
    m_staticParamsThread = boost::thread(boost::bind(&StaticParamsOI::run, &m_staticParamsOI));
    // Wait until Static OI started in another thread before executing
    m_staticParamsOI.waitUntilRunning();

    // Start CPS_BUAP_PARMGR Server in different thread
    m_parServerThread = boost::thread(boost::bind(&CPS_BUAP_PARMGR::Run, &m_parServer));
    // Wait until CPS_BUAP_PARMGR server started in another thread before executing
    m_parServer.waitUntilRunning();


    // Start CPS_BUAP_Loader Server in different thread
    m_loadServerThread = boost::thread(boost::bind(&CPS_BUAP_Loader::Run, &m_loadServer));
    // Wait until CPS_BUAP_PARMGR server started in another thread before executing
    m_loadServer.waitUntilRunning();


    // Start CPS_BUAP_FILEMGR Server in different thread
    m_fileServerThread = boost::thread(boost::bind(&CPS_BUAP_FILEMGR::Run, &m_fileServer));
    // Wait until CPS_BUAP_PARMGR server started in another thread before executing
    m_fileServer.waitUntilRunning();

}

//**********************************************************************************************
// stop()
//**********************************************************************************************
void CPS_BUAP_Service::stop()
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_Service::stop()", 0));

    // Stop the BUAP Server in case it has not been stopped
    if (!m_stop)
    {
        m_fileServer.stop();
        m_loadServer.stop();
        m_parServer.stop();
        m_staticParamsOI.stop();
        m_classicOI.stop();
    }
    else 
    {
        TRACE((LOG_LEVEL_INFO, "BUAP Server was already stopped.", 0));
    }

    m_stop = true;

    // Wait for all the threads exit before itself exits
    m_fileServerThread.join();
    m_loadServerThread.join();
    m_parServerThread.join();
    m_staticParamsThread.join();
    m_classicThread.join();

    TRACE((LOG_LEVEL_INFO, "CPS_BUAP_Service::stop() returns", 0));
}

//**********************************************************************************************
// performStateTransitionToActiveJobs()
//**********************************************************************************************
ACS_APGCC_ReturnType CPS_BUAP_Service::performStateTransitionToActiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_Service::performStateTransitionToActiveJobs(%d)", 0, previousHAState));

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

//**********************************************************************************************
// performStateTransitionToPassiveJobs()
//**********************************************************************************************
ACS_APGCC_ReturnType CPS_BUAP_Service::performStateTransitionToPassiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_Service::performStateTransitionToPassiveJobs(%d)", 0, previousHAState));

    // Set the state to passive
    m_active = false;

    // Do nothing

    return ACS_APGCC_SUCCESS;
}

//**********************************************************************************************
// performStateTransitionToQueisingJobs()
//**********************************************************************************************
ACS_APGCC_ReturnType CPS_BUAP_Service::performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_Service::performStateTransitionToQueisingJobs(%d)", 0, previousHAState));

    m_active = false;
    this->stop();

    return ACS_APGCC_SUCCESS;

}

//**********************************************************************************************
// performStateTransitionToQuiescedJobs()
//**********************************************************************************************
ACS_APGCC_ReturnType CPS_BUAP_Service::performStateTransitionToQuiescedJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_Service::performStateTransitionToQuiescedJobs(%d)", 0, previousHAState));

    m_active = false;
    this->stop();

    return ACS_APGCC_SUCCESS;
}

//**********************************************************************************************
// performComponentHealthCheck()
//**********************************************************************************************
ACS_APGCC_ReturnType CPS_BUAP_Service::performComponentHealthCheck(void)
{
    newTRACE(("CPS_BUAP_Service::performComponentHealthCheck()", 0));

    ACS_APGCC_ReturnType rc = ACS_APGCC_SUCCESS;

#if 0
    // Check whether PARMGR Server is running then respond accordingly
    if (m_active && !(m_parServer.isRunning() && m_loadServer.isRunning() && m_fileServer.isRunning()))
    {
        rc = ACS_APGCC_FAILURE;
        newTRACE(("HealthCheck failed. Server is not running", 0));

    }

#endif

    return rc;
}

//**********************************************************************************************
// performComponentTerminateJobs()
//**********************************************************************************************
ACS_APGCC_ReturnType CPS_BUAP_Service::performComponentTerminateJobs(void)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_Service::performComponentTerminateJobs(void)", 0));

    // Application has received terminate component callback due to
    // LOCK-INST admin opreration perform on SU.

    this->stop();

    return ACS_APGCC_SUCCESS;
}

//**********************************************************************************************
// performComponentRemoveJobs()
//**********************************************************************************************
ACS_APGCC_ReturnType CPS_BUAP_Service::performComponentRemoveJobs(void)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_Service::performComponentRemoveJobs(void)", 0));

    // Application has received Removal callback. State of the application
    // is neither Active nor Standby. This is with the result of LOCK admin operation
    // performed on our SU.

    // Stop the BUAP Service
    this->stop();

    return ACS_APGCC_SUCCESS;
}

//**********************************************************************************************
// performApplicationShutdownJobs()
//**********************************************************************************************
ACS_APGCC_ReturnType CPS_BUAP_Service::performApplicationShutdownJobs(void)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_Service::performApplicationShutdownJobs(void)", 0));

    this->stop();

    return ACS_APGCC_SUCCESS;
}

