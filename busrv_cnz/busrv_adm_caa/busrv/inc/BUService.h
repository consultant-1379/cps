/*
NAME
    File_name:BUService.h

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

#ifndef BUSERVICE_H_
#define BUSERVICE_H_

#include "ACS_APGCC_ApplicationManager.h"
#include "BUServer.h"
#include "JobThread.h"
#include "BUParamsOI.h"
#include "GAServer.h"

#include "boost/thread.hpp"


class BUService : public ACS_APGCC_ApplicationManager
{
public:
    static const char *buDaemonName;

public:
    BUService(const char* daemonName);
    virtual ~BUService();

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
    // BU Server
    BUServer m_server;

    // BU JobThread
    JobThread m_jobThread;

    // BUParam OI
    BUParamsOI m_oi;

    // Threads
    boost::thread m_jobThread_thread;
    boost::thread m_server_thread;
    boost::thread m_oi_thread;

    // Stop Flag
    bool m_stop;

    // Active Flag;
    bool m_active;
    GAServer m_gaServer;
};

#endif /* BUSERVICE_H_ */
