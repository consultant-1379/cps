//******************************************************************************
// COPYRIGHT Ericsson Utvecklings AB, Sweden 1999.
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
// CPS_BUAP_Loader_Service.cpp
//
// DESCRIPTION 
// This file implements the parts of the BUAP_Loader service that communicates 
// with the Windows Service Control Manager. This code starts BUAP_Loader_Main.
//
// DOCUMENT NO
// 190 89-CAA 109 1412
//
// AUTHOR 
// 990913 UAB/B/SF Mona Ntterkvist
//
//******************************************************************************
// === Revision history ===
// 990913 MNST PA1 Created.
// 991102 BIR  PA2 Setting of process priority added.
// 090825 XTBAKLU Moved AP_InitProcess() from initService() to main() in 
//                order to align with DR PP1.
// 101215 xchihoa  Port to Linux for APG43L
//******************************************************************************

//******************************************************************************
//BUAP_Loader_Service.cpp
//******************************************************************************
#include "CPS_BUAP_Loader_Service.h"
#include "CPS_BUAP_Loader.h"
#include "CPS_BUAP_Trace.h"

#include <string.h>
#include <sys/eventfd.h>


/*===================================================================
 ROUTINE: runBuapLoader
 Global thread function
 =================================================================== */
void *runBuapLoader(void*)
{
   newTRACE(("runBuapLoader()", 0));

   try
   {
      CPS_BUAP_Loader loader;
      loader.run();
   } catch (...)
   {
      TRACE((LOG_LEVEL_ERROR, "BUAP_Loader runs with Exception!!!", 0));
   }

   pthread_exit(NULL);
}

/*
 * Description: This is the class constructor.
 * @para[in]: daemon_name - name of application.
 * @para[in]: user_name - user is used to start application.
 * @return: N/A
 */
CPS_BUAP_Loader_Service::CPS_BUAP_Loader_Service(const char* daemon_name, const char* user_name):ACS_APGCC_ApplicationManager(daemon_name, user_name)
{
	m_isRunning = false;
}

/*
 * Description: This is the class destructor.
 * @para: N/A
 * @return: N/A
 */
CPS_BUAP_Loader_Service::~CPS_BUAP_Loader_Service()
{

}
/*
 * Description: This function is used to activate amf service loop.
 * @para: N/A
 * @return: The error code
 */
ACS_APGCC_HA_ReturnType CPS_BUAP_Loader_Service::amfInitialize()
{
	newTRACE(("CPS_BUAP_Loader_Service::amfInitialize()",0));

	ACS_APGCC_HA_ReturnType errorCode = ACS_APGCC_HA_SUCCESS;

	errorCode = this->activate();

	if (errorCode == ACS_APGCC_HA_FAILURE)
	{
		TRACE(("cps_buaploaderd. HA Activation Failed", 0));
	}

	if (errorCode == ACS_APGCC_HA_FAILURE_CLOSE)
	{
		TRACE(("cps_buaploaderd, HA Application Failed to Gracefully closed!!", 0));
	}

	if (errorCode == ACS_APGCC_HA_SUCCESS)
	{
		TRACE(("cps_buaploaderd, HA Application Gracefully closed!!", 0));
	}

	return errorCode;
}

/*
 * Description: This functions is used to run the application.
 * @para: N/A
 * @return: N/A
 */
void CPS_BUAP_Loader_Service::start()
{
    newTRACE(("CPS_BUAP_Loader_Service::start()",0));
    if(!m_isRunning)
    {
        //Start application thread
        int ret_val = pthread_create(&m_buloaderThread, NULL, runBuapLoader,NULL);
        if (ret_val != 0)
        {
            TRACE((LOG_LEVEL_ERROR, "Can not create Loader thread!!!", 0));
        }
        else
        {
            TRACE(("buaploaderd started!!!", 0));
        }

        m_isRunning = true;
    }
}

/*
 * Description: This functions is used to stop application.
 * @para: N/A
 * @return: N/A
 */
void CPS_BUAP_Loader_Service::stop()
{
    newTRACE(("CPS_BUAP_Loader_Service::stop()",0));

    if(m_isRunning)
    {
        // Inform Loader thread to terminate
        CPS_BUAP_Loader::setStopEvent();

        // Wait Loader thread to terminate itself
        pthread_join(m_buloaderThread, NULL);

        TRACE(("buaploaderd stopped!!!", 0));

        m_isRunning = false;
    }
}

/*
 * Description: This is a callback functions that is called from AMF when application's state changes to active.
 * @para[in]: previousHAState - the previous state of application
 * @return: ACS_APGCC_SUCCESS
 */
ACS_APGCC_ReturnType CPS_BUAP_Loader_Service::performStateTransitionToActiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	/* Check if we have received the ACTIVE State Again.
	 * This means that, our application is already Active and
	 * again we have got a callback from AMF to go active.
	 * Ignore this case anyway. This case should rarely happens
	 */

	newTRACE(("CPS_BUAP_Loader_Service::performStateTransitionToActiveJobs()",0));

	if(ACS_APGCC_AMF_HA_ACTIVE == previousHAState)
		return ACS_APGCC_SUCCESS;

	/* Our application has received state ACTIVE from AMF.
	 * Start off with the activities needs to be performed
	 * on ACTIVE
	 */

	/* Handle here what needs to be done when you are given ACTIVE State */

	TRACE(("My Application Component received ACTIVE state assignment!!!", 0));
	this->start();

	return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType CPS_BUAP_Loader_Service::performStateTransitionToPassiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{

    newTRACE(("BUService::performStateTransitionToPassiveJobs(%d)", 0, previousHAState));

    return ACS_APGCC_SUCCESS;
}

/*
 * Description: This is a callback functions that is called from AMF when application's state changes to queising.
 * @para[in]: previousHAState - the previous state of application
 * @return: ACS_APGCC_SUCCESS
 */
ACS_APGCC_ReturnType CPS_BUAP_Loader_Service::performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT /*previousHAState*/)
{
	/* We were active and now losing active state due to some shutdown admin
	 * operation performed on our SU.
	 * Inform the thread to go to "stop" state
	 */
	newTRACE(("CPS_BUAP_Loader_Service::performStateTransitionToQueisingJobs()",0));

	TRACE(("My Application Component received QUIESING state assignment!!!",0));
	this->stop();

	return ACS_APGCC_SUCCESS;
}

/*
 * Description: This is a callback functions that is called from AMF when application's state changes to queisced.
 * @para[in]: previousHAState - the previous state of application
 * @return: ACS_APGCC_SUCCESS
 */
ACS_APGCC_ReturnType CPS_BUAP_Loader_Service::performStateTransitionToQuiescedJobs(ACS_APGCC_AMF_HA_StateT /*previousHAState*/)
{
	/* We were Active and now losting Active state due to Lock admin
	 * operation performed on our SU.
	 * Inform the thread to go to "stop" state
	 */
	newTRACE(("CPS_BUAP_Loader_Service::performStateTransitionToQuiescedJobs()",0));

	TRACE(("My Application Component received QUIESCED state assignment!",0));
	this->stop();

	return ACS_APGCC_SUCCESS;
}

/*
 * Description: This is a callback functions that is called from AMF in every healthcheck interval.
 * @para: N/A
 * @return: ACS_APGCC_SUCCESS
 */
ACS_APGCC_ReturnType CPS_BUAP_Loader_Service::performComponentHealthCheck()
{
	newTRACE(("CPS_BUAP_Loader_Service::performComponentHealthCheck()",0));

	TRACE(("My Application Component received health check!",0));

	ACS_APGCC_ReturnType rc = ACS_APGCC_SUCCESS;

    return rc;
}

/*
 * Description: This is a callback functions that is called from AMF when application is terminated.
 * @para: N/A
 * @return: ACS_APGCC_SUCCESS
 */
ACS_APGCC_ReturnType CPS_BUAP_Loader_Service::performComponentTerminateJobs(void)
{
	newTRACE(("CPS_BUAP_Loader_Service::performComponentTerminateJobs()",0));

	TRACE(("My Application Component received terminate callback!",0));

	this->stop();

	return ACS_APGCC_SUCCESS;
}

/*
 * Description: This is a callback functions that is called from AMF when application is stopped.
 * @para: N/A
 * @return: ACS_APGCC_SUCCESS
 */
ACS_APGCC_ReturnType CPS_BUAP_Loader_Service::performComponentRemoveJobs (void)
{
	/* Application has received Removal callback. State of the application
	 * is neither Active nor Standby. This is with the result of LOCK admin operation
	 * performed on our SU. Terminate the thread by informing the thread to go "stop" state.
	 */
	newTRACE(("CPS_BUAP_Loader_Service::performComponentRemoveJobs()",0));

	TRACE(("Application Assignment is removed now",0));
	this->stop();

	return ACS_APGCC_SUCCESS;
}

/*
 * Description: This is a callback functions that is called from AMF when shutting down AMF.
 * @para: N/A
 * @return: ACS_APGCC_SUCCESS
 */
ACS_APGCC_ReturnType CPS_BUAP_Loader_Service::performApplicationShutdownJobs(void)
{
	newTRACE(("CPS_BUAP_Loader_Service::performApplicationShutdownJobs()",0));

	TRACE(("Shutting down the application",0));
	this->stop();

	return ACS_APGCC_SUCCESS;
}

