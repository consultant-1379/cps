/*
NAME
    File_name:FCService.cpp

Ericsson Utvecklings AB

    COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

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
    2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on FCService.cpp of FCSRV in Windows
    and BUSRV's BUService.cpp


SEE ALSO
    -

Revision history
----------------


*/

#include "CriticalSection.h"
#include "FCService.h"
#include "CPS_FCSRV_Trace.h"
#include "EventReporter.h"

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <cerrno>

#include "boost/thread/mutex.hpp"
#include "boost/thread/condition_variable.hpp"

#include <boost/thread/thread.hpp>


const char *FCService::fcDaemonName = "cps_fcsrvd";

FCService::FCService(const char* daemonName) : ACS_APGCC_ApplicationManager(daemonName), m_server(), m_jobThread(m_server), m_stop(true), m_active(false)
{
    //newTRACE((LOG_LEVEL_INFO, "FCService::FCService(%s)", 0, daemonName));

    m_server.set(&m_jobThread);
}

FCService::~FCService()
{
   //newTRACE((LOG_LEVEL_INFO, "FCService::~FCService()", 0));
}

void FCService::run()
{
    newTRACE((LOG_LEVEL_INFO, "FCService::run()", 0));

    ACS_APGCC_HA_ReturnType errorCode = ACS_APGCC_HA_SUCCESS;

    // Start service & event loop for AMF events
    TRACE((LOG_LEVEL_INFO, "FCService::run() start amf service loop", 0));
    if (isTRACEABLE()) {
        std::ostringstream ss;
        ss << "FCService::run() thread id "
           << boost::this_thread::get_id();
        TRACE((LOG_LEVEL_INFO, "%s", 0, ss.str().c_str()));
    }
    
    errorCode = this->activate();
    TRACE((LOG_LEVEL_INFO, "FCService::run() amf service loop returns", 0));

    if (errorCode == ACS_APGCC_HA_FAILURE)
    {
        TRACE((LOG_LEVEL_ERROR, "HA Activation Failed", 0));
    }

    if (errorCode == ACS_APGCC_HA_FAILURE_CLOSE)
    {
        TRACE((LOG_LEVEL_ERROR, "Failed to Gracefully close", 0));
    }

    if (errorCode == ACS_APGCC_HA_SUCCESS)
    {
        TRACE((LOG_LEVEL_INFO, "Gracefully closed", 0));
    }
    TRACE((LOG_LEVEL_INFO, "FCService::run() returns", 0));
}

void FCService::start()
{
    newTRACE((LOG_LEVEL_INFO, "FCService::start()", 0));

    m_stop = false;

    // Order to run
    // 1. JobThread Thread (Job thread should be started before BU Server Thread
    // 2. FCServer Thread

    // Start the JobThread in different thread
    m_jobThread_thread = boost::thread(boost::bind(&JobThread::run, &m_jobThread));

    // Wait until JobThread started before execute
    m_jobThread.waitUntilRunning();

    // Start the FCServer in different thread
    m_server_thread = boost::thread(boost::bind(&FCServer::run, &m_server));

    // Wait until FCServer started in another thread before execute
    m_server.waitUntilRunning();
}

void FCService::stop()
{
    newTRACE((LOG_LEVEL_INFO, "FCService::stop()", 0));

    // Stop the BU Server in case it has not been stopped
    if (!m_stop)
    {
        m_server.stop();
    }
    else
    {
        TRACE((LOG_LEVEL_INFO, "FCSRV Server was already stopped.", 0));
    }

    m_stop = true;

    // Wait for all the threads exit before itself exits
    m_jobThread_thread.join();
    m_server_thread.join();

    TRACE((LOG_LEVEL_INFO, "FCService::stop() returns", 0));
}

ACS_APGCC_ReturnType FCService::performStateTransitionToActiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
    newTRACE((LOG_LEVEL_INFO, "FCService::performStateTransitionToActiveJobs(%d)", 0, previousHAState));

    ACS_APGCC_ReturnType rc = ACS_APGCC_SUCCESS;

    if (isTRACEABLE()) {
        std::ostringstream ss;
        ss << "FCService::performStateTransitionToActiveJobs() thread id "
           << boost::this_thread::get_id();
        TRACE((LOG_LEVEL_INFO, "%s", 0, ss.str().c_str()));
    }
    
    m_active = true;
    // Previous state is ACTIVE, no need to process again
    if(ACS_APGCC_AMF_HA_ACTIVE == previousHAState)
    {
        return rc;
    }

    // Become active, should start server now
    this->start();

    return rc;
}

ACS_APGCC_ReturnType FCService::performStateTransitionToPassiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
    newTRACE((LOG_LEVEL_INFO, "FCService::performStateTransitionToPassiveJobs(%d)", 0, previousHAState));

    m_active = false;

    if (isTRACEABLE()) {
        std::ostringstream ss;
        ss << "FCService::performStateTransitionToPassiveJobs() thread id "
           << boost::this_thread::get_id();
        TRACE((LOG_LEVEL_INFO, "%s", 0, ss.str().c_str()));
    }
    // Do nothing

    return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType FCService::performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
    newTRACE((LOG_LEVEL_INFO, "FCService::performStateTransitionToQueisingJobs(%d)", 0, previousHAState));

    m_active = false;
    this->stop();

    if (isTRACEABLE()) {
        std::ostringstream ss;
        ss << "FCService::performStateTransitionToQueisingJobs() thread id "
           << boost::this_thread::get_id();
        TRACE((LOG_LEVEL_INFO, "%s", 0, ss.str().c_str()));
    }
    
    return ACS_APGCC_SUCCESS;

}
ACS_APGCC_ReturnType FCService::performStateTransitionToQuiescedJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
    newTRACE((LOG_LEVEL_INFO, "FCService::performStateTransitionToQuiescedJobs(%d)", 0, previousHAState));

    m_active = false;
    this->stop();

    if (isTRACEABLE()) {
        std::ostringstream ss;
        ss << "FCService::performStateTransitionToQuiescedJobs() thread id "
           << boost::this_thread::get_id();
        TRACE((LOG_LEVEL_INFO, "%s", 0, ss.str().c_str()));
    }
    
    return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType FCService::performComponentHealthCheck(void)
{
	static u_int32 count = 0;
	bool trace = ++count % 30 == 0;	
	if (trace) {
    	newTRACE((LOG_LEVEL_INFO, "FCService::performComponentHealthCheck()", 0));
    }

    ACS_APGCC_ReturnType rc = ACS_APGCC_SUCCESS;

#if 0

    // Check whether FC Server is running then respond accordingly
    if (m_active && !m_server.isRunning(trace))
    {
        rc = ACS_APGCC_FAILURE;
        newTRACE(("HealthCheck failed. Server is not running", 0));
        EventReporter::instance().write("HealthCheck failed. Server is not running");
    }

#endif

    return rc;
}

ACS_APGCC_ReturnType FCService::performComponentTerminateJobs(void)
{
    newTRACE((LOG_LEVEL_INFO, "FCService::performComponentTerminateJobs(void)", 0));

    // Application has received terminate component callback due to
    // LOCK-INST admin opreration perform on SU.

    this->stop();

    return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType FCService::performComponentRemoveJobs(void)
{
    newTRACE((LOG_LEVEL_INFO, "FCService::performComponentRemoveJobs(void)", 0));

    // Application has received Removal callback. State of the application
    // is neither Active nor Standby. This is with the result of LOCK admin operation
    // performed on our SU.

    if (isTRACEABLE()) {
        std::ostringstream ss;
        ss << "FCService::performComponentRemoveJobs() thread id "
           << boost::this_thread::get_id();
        TRACE((LOG_LEVEL_INFO, "%s", 0, ss.str().c_str()));
    }
    
    // Stop the BU server
    this->stop();

    return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType FCService::performApplicationShutdownJobs(void)
{
    newTRACE((LOG_LEVEL_INFO, "FCService::performApplicationShutdownJobs(void)", 0));

    this->stop();

    return ACS_APGCC_SUCCESS;
}
