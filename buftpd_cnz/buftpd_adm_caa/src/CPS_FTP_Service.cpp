/*
NAME
    File_name:CPS_FTP_Service.cpp

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
    190 89-CAA 109 1415

AUTHOR
    2011-08-19 by XDT/DEK xquydao


SEE ALSO
    -

Revision history
----------------
2011-10-19 xquydao Created

*/
#include "CPS_FTP_Service.h"
#include "CPS_FTP_Trace.h"
#include "CPS_FTP_EventReporter.h"
#include "CPS_FTP_Events.h"

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <cerrno>

#include "boost/thread/mutex.hpp"
#include "boost/thread/condition_variable.hpp"

#include <boost/thread/thread.hpp>


const std::string CPS_FTP_Service::BUFTP_DEF_DAEMON_NAME = "cps_buftpd";

CPS_FTP_Service::CPS_FTP_Service(const string& daemonName, CPS_FTP_Server& server) :
        ACS_APGCC_ApplicationManager(daemonName.c_str()), m_server(server), m_stop(false), m_active(false)
{
   // newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Service::CPS_FTP_Service(%s)", 0, daemonName.c_str()));
}

CPS_FTP_Service::~CPS_FTP_Service()
{
   // newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Service::~CPS_FTP_Service()", 0));
}

void CPS_FTP_Service::run()
{
    newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Service::run()", 0));

    ACS_APGCC_HA_ReturnType errorCode = ACS_APGCC_HA_SUCCESS;

    // Start service & event loop for AMF events
    // performStateTransitionToActive/PassiveJobs will be called
    // after this
    errorCode = this->activate();

    TRACE((LOG_LEVEL_INFO, "CPS_FTP_Service::run() amf service loop returns", 0));

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

    TRACE((LOG_LEVEL_INFO, "CPS_FTP_Service::run() returns", 0));
}

void CPS_FTP_Service::start()
{
    newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Service::start()", 0));

    m_stop = false;

    // Start the BUFTP Server in different thread
    m_server_thread = boost::thread(boost::bind(&CPS_FTP_Server::run, &m_server));

    // Wait until CPS_FTP_Server started in another thread before executing
    m_server.waitUntilRunning();
}

void CPS_FTP_Service::stop()
{
    newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Service::stop()", 0));

    // Stop the BUFTP Server in case it has not been stopped
    if (!m_stop)
    {
        m_server.stop();
    }
    else
    {
        TRACE((LOG_LEVEL_INFO, "BUFTP Server was already stopped.", 0));
    }

    m_stop = true;

    // Wait for all the threads exit before itself exits
    m_server_thread.join();
    TRACE((LOG_LEVEL_INFO, "CPS_FTP_Service::stop() returns", 0));
}

ACS_APGCC_ReturnType CPS_FTP_Service::performStateTransitionToActiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Service::performStateTransitionToActiveJobs(%d)", 0, previousHAState));

    ACS_APGCC_ReturnType rc = ACS_APGCC_SUCCESS;

    m_active = true;
    // Previous state is ACTIVE, no need to process again
    if (ACS_APGCC_AMF_HA_ACTIVE == previousHAState)
    {
        TRACE((LOG_LEVEL_WARN, "Previous state is ACTIVE, no need to activate", 0));
        return rc;
    }

    // Become active, should start server now
    this->start();

    return rc;
}

ACS_APGCC_ReturnType CPS_FTP_Service::performStateTransitionToPassiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Service::performStateTransitionToPassiveJobs(%d)", 0, previousHAState));

    // Set the state to passive
    m_active = false;

    // Do nothing. Before it gets to this state, it already got into QueisingJob or QuiescedJobs

    return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType CPS_FTP_Service::performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Service::performStateTransitionToQueisingJobs(%d)", 0, previousHAState));

    m_active = false;
    this->stop();

    return ACS_APGCC_SUCCESS;

}

ACS_APGCC_ReturnType CPS_FTP_Service::performStateTransitionToQuiescedJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Service::performStateTransitionToQuiescedJobs(%d)", 0, previousHAState));

    m_active = false;
    this->stop();

    return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType CPS_FTP_Service::performComponentHealthCheck(void)
{
    newTRACE((LOG_LEVEL_TRACE, "CPS_FTP_Service::performComponentHealthCheck()", 0));

    ACS_APGCC_ReturnType rc = ACS_APGCC_SUCCESS;

    // Check whether BUFTP Server is running then respond accordingly
    /* From HA DR, this fucntion needs to return ACS_APGCC_SUCCESS
    if (m_active && !m_server.isRunning())
    {
        rc = ACS_APGCC_FAILURE;
        TRACE(("HealthCheck failed. Server is not running", 0));
        EVENT((CPS_FTP_Events::service_healthcheck_fail, 0));
    }*/

    return rc;
}

ACS_APGCC_ReturnType CPS_FTP_Service::performComponentTerminateJobs(void)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Service::performComponentTerminateJobs(void)", 0));

    // Application has received terminate component callback due to
    // LOCK-INST admin opreration perform on SU.

    this->stop();

    return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType CPS_FTP_Service::performComponentRemoveJobs(void)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Service::performComponentRemoveJobs(void)", 0));

    // Application has received Removal callback. State of the application
    // is neither Active nor Standby. This is with the result of LOCK admin operation
    // performed on our SU.

    // Stop the BUFTP Server
    this->stop();

    return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType CPS_FTP_Service::performApplicationShutdownJobs(void)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_FTP_Service::performApplicationShutdownJobs(void)", 0));

    this->stop();

    return ACS_APGCC_SUCCESS;
}
