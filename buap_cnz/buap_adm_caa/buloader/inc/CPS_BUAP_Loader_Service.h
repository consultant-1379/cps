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
// CPS_BUAP_Loader_Service.H
//
// DESCRIPTION 
// Header file for CPS_BUAP_Loader_Service.cpp.
//
// DOCUMENT NO
// 190 89-CAA 109 1412
//
// AUTHOR 
// 990913 UAB/B/SF Mona Ntterkvist
// 101215 xchihoa  Port to Linux for APG43L
//
//******************************************************************************
// === Revision history ===
// 990913 MNST PA1 Created.
//******************************************************************************
#ifndef CPS_BUAP_Loader_Service_H
#define CPS_BUAP_Loader_Service_H

#include "ACS_APGCC_ApplicationManager.h"
#include <pthread.h>

void *runBuapLoader(void*);

class CPS_BUAP_Loader_Service : public ACS_APGCC_ApplicationManager
{
public:
	CPS_BUAP_Loader_Service(const char* daemon_name, const char* user_name);
	virtual ~CPS_BUAP_Loader_Service();

	ACS_APGCC_HA_ReturnType amfInitialize();

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

	bool m_isRunning;
	pthread_t  m_buloaderThread;
	CPS_BUAP_Loader_Service* m_cpsBUAPLoader;

};
#endif
