/*
NAME
    File_name:BUService.cpp

Ericsson Utvecklings AB

    COPYRIGHT Ericsson Utvecklings AB, Sweden 2000. All rights reserved.

    The Copyright to the computer program(s) herein is the property of
    Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
    copied only with the written permission from Ericsson Utvecklings AB
    or in accordance with the terms and conditions stipulated in the
    agreement/contract under which the program(s) have been supplied.

DESCRIPTION
    Class implements a debug context class used in the development of the
    reset of the code. Note that no methods are to be called directly,
    rather by use of macros so that the debug code may be conditionally
    compiled out.

DOCUMENT NO
    190 89-CAA 109 1409

AUTHOR
    2010-12-01 by XDT/DEK xquydao


SEE ALSO
    -

Revision history
----------------
2010-12-01 xquydao Created
2011-01-10 xquydao Updated after review

*/

#include "CriticalSection.h"
#include "BUService.h"
#include "BUParamsOM.h"

#include "CPS_BUSRV_Trace.h"
#include "EventReporter.h"
#include "Config.h"

#include <sys/time.h>
#include <sys/types.h>

#ifdef _APGUSERSVC_

#include <unistd.h>
#include <cerrno>
#include <grp.h>

#else 

#include <sys/capability.h>

#endif

#include "boost/thread/mutex.hpp"
#include "boost/thread/condition_variable.hpp"

#include <boost/thread/thread.hpp>


const char *BUService::buDaemonName = "cps_busrvd";


BUService::BUService(const char* daemonName) : ACS_APGCC_ApplicationManager(daemonName), m_server(), m_jobThread(m_server), 
    m_stop(true), m_active(false), m_gaServer(Config::DSD_SERVICE_NAME, Config::DSD_SERVICE_DOMAIN)
{
    //newTRACE((LOG_LEVEL_INFO, "BUSRV Run_as_service ==>> BUService::BUService(%s)", 0, daemonName));
    m_server.set(&m_jobThread);
}

BUService::~BUService()
{
    //newTRACE((LOG_LEVEL_INFO, "BUService::~BUService()", 0));
}

void BUService::run()
{
    newTRACE((LOG_LEVEL_INFO, "BUService::run()", 0));

    // Start service & event loop for AMF events
    TRACE((LOG_LEVEL_INFO, "BUService::run() start amf service loop", 0));
    if (isTRACEABLE()) {
        std::ostringstream ss;
        ss << "BUService::run() thread id " 
           << boost::this_thread::get_id();
        TRACE((LOG_LEVEL_INFO, "%s", 0, ss.str().c_str()));
    }
    
    ACS_APGCC_HA_ReturnType returnCode = this->activate();
    TRACE((LOG_LEVEL_INFO, "BUService::run() amf service loop returns code <%d>", 0, returnCode));

    string msg;
    switch (returnCode) {
    case ACS_APGCC_HA_SUCCESS:
    	msg = "Gracefully closed";
    	break;
    case ACS_APGCC_HA_FAILURE:
    	msg = "HA Activation Failed";
    	break;
    default:
    	msg = "Failed to Gracefully close";
    	break;
    }
    TRACE((LOG_LEVEL_INFO, "%s", 0, msg.c_str()));
    TRACE((LOG_LEVEL_INFO, "BUService::run() returns", 0));
}

void BUService::start()
{
    newTRACE((LOG_LEVEL_INFO, "BUService::start()", 0));

    m_stop = false;

    Config::instance().save();
    // Order to run
    // 1. Create CPReload object
    // 2. JobThread Thread (Job thread should be started before BU Server Thread
    // 3. BUServer Thread
    // 4. OI

    // Create CPReloadParam & instance in IMM then sync file to IMM
    BUParamsOM om;
    if (!om.createAndSyncBUParamObject())
    {
        if (om.getLastError() != BUParamsCommon::UNSUPPORTED_SYSTEM)
        {
            TRACE((LOG_LEVEL_ERROR, "Failed to create CPReload object", 0));
            // TODO: keep running or component error restart?
        }
    }

    // Start the JobThread in different thread
    m_jobThread_thread = boost::thread(boost::bind(&JobThread::run, &m_jobThread));

    // Wait until JobThread started before execute
    m_jobThread.waitUntilRunning();

    // Start the BUServer in different thread
    m_server_thread = boost::thread(boost::bind(&BUServer::run, &m_server));

    // Wait until BUServer started in another thread before execute
    m_server.waitUntilRunning();

    // Start the OI in different thread
    m_oi_thread = boost::thread(boost::bind(&BUParamsOI::run, &m_oi));
    m_oi.waitUntilRunning();

    m_gaServer.start();
}

void BUService::stop()
{
    newTRACE((LOG_LEVEL_INFO, "BUService::stop()", 0));

    // Stop the BU Server in case it has not been stopped
    if (!m_stop)
    {
        m_server.stop();
        m_oi.stop();
    }
    else 
    {
        TRACE((LOG_LEVEL_INFO, "BU Server was already stopped.", 0));
    }

    m_stop = true;
    m_gaServer.stop();

    // Wait for all the threads exit before itself exits
    m_jobThread_thread.join();
    m_server_thread.join();
    m_oi_thread.join();
    m_gaServer.join();

    TRACE((LOG_LEVEL_INFO, "BUService::stop() returns", 0));
}

ACS_APGCC_ReturnType BUService::performStateTransitionToActiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
    newTRACE((LOG_LEVEL_INFO, "BUService::performStateTransitionToActiveJobs(%d)", 0, previousHAState));

    ACS_APGCC_ReturnType rc = ACS_APGCC_SUCCESS;

    if (isTRACEABLE()) {
        std::ostringstream ss;
        ss << "BUService::performStateTransitionToActiveJobs() thread id " 
           << boost::this_thread::get_id();
        TRACE((LOG_LEVEL_INFO, "%s", 0, ss.str().c_str()));
    }
    
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

ACS_APGCC_ReturnType BUService::performStateTransitionToPassiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
    newTRACE((LOG_LEVEL_INFO, "BUService::performStateTransitionToPassiveJobs(%d)", 0, previousHAState));

    m_active = false;

    if (isTRACEABLE()) {
        std::ostringstream ss;
        ss << "BUService::performStateTransitionToPassiveJobs() thread id " 
           << boost::this_thread::get_id();
        TRACE((LOG_LEVEL_INFO, "%s", 0, ss.str().c_str()));
    }
    // Do nothing

    return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType BUService::performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
    newTRACE((LOG_LEVEL_INFO, "BUService::performStateTransitionToQueisingJobs(%d)", 0, previousHAState));

    m_active = false;
    this->stop();

    if (isTRACEABLE()) {
        std::ostringstream ss;
        ss << "BUService::performStateTransitionToQueisingJobs() thread id " 
           << boost::this_thread::get_id();
        TRACE((LOG_LEVEL_INFO, "%s", 0, ss.str().c_str()));
    }
    
    return ACS_APGCC_SUCCESS;

}
ACS_APGCC_ReturnType BUService::performStateTransitionToQuiescedJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
    newTRACE((LOG_LEVEL_INFO, "BUService::performStateTransitionToQuiescedJobs(%d)", 0, previousHAState));

    m_active = false;
    this->stop();

    if (isTRACEABLE()) {
        std::ostringstream ss;
        ss << "BUService::performStateTransitionToQuiescedJobs() thread id " 
           << boost::this_thread::get_id();
        TRACE((LOG_LEVEL_INFO, "%s", 0, ss.str().c_str()));
    }
    
    return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType BUService::performComponentHealthCheck(void)
{
	newTRACE((LOG_LEVEL_INFO, "BUService::performComponentHealthCheck()", 0));
    ACS_APGCC_ReturnType rc = m_server.fatalError() ? ACS_APGCC_FAILURE : ACS_APGCC_SUCCESS;

    TRACE((LOG_LEVEL_INFO, "Component Health Check returns <%d>", 0, rc));

    // Check whether BU Server is running then respond accordingly
    #if 0
    if (m_active && !m_server.isRunning(trace))
    {
        rc = ACS_APGCC_FAILURE;
        newTRACE(("HealthCheck failed. Server is not running", 0));
        EventReporter::instance().write("HealthCheck failed. Server is not running");
    }
    #endif

    return rc;
}

ACS_APGCC_ReturnType BUService::performComponentTerminateJobs(void)
{
    newTRACE((LOG_LEVEL_INFO, "BUService::performComponentTerminateJobs(void)", 0));

    // Application has received terminate component callback due to
    // LOCK-INST admin opreration perform on SU.

    this->stop();

    return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType BUService::performComponentRemoveJobs(void)
{
    newTRACE((LOG_LEVEL_INFO, "BUService::performComponentRemoveJobs(void)", 0));

    // Application has received Removal callback. State of the application
    // is neither Active nor Standby. This is with the result of LOCK admin operation
    // performed on our SU.

    if (isTRACEABLE()) {
        std::ostringstream ss;
        ss << "BUService::performComponentRemoveJobs() thread id " 
           << boost::this_thread::get_id();
        TRACE((LOG_LEVEL_INFO, "%s", 0, ss.str().c_str()));
    }
    
    // Stop the BU server
    this->stop();

    return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType BUService::performApplicationShutdownJobs(void)
{
    newTRACE((LOG_LEVEL_INFO, "BUService::performApplicationShutdownJobs(void)", 0));

    this->stop();

    return ACS_APGCC_SUCCESS;
}
