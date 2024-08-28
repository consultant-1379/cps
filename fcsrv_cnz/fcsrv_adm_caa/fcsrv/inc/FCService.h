/*
NAME
    File_name:FCService.h

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
    2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on FCService.h of FCSRV in Windows
    and  BUSRV's BUService.h


SEE ALSO
    -

Revision history
----------------

*/

#ifndef FCSERVICE_H_
#define FCSERVICE_H_

#include "ACS_APGCC_ApplicationManager.h"
#include "FCServer.h"
#include "JobThread.h"
#include "boost/thread.hpp"

class FCService : public ACS_APGCC_ApplicationManager
{
public:
    static const char *fcDaemonName;

public:
    FCService(const char* daemonName);
    virtual ~FCService();

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
    // FC Server
    FCServer m_server;

    // Thread for FC JobThread
    JobThread m_jobThread;

    // Threads
    boost::thread m_jobThread_thread;
    boost::thread m_server_thread;

    // Stop Flag
    bool m_stop;

    // Active Flag;
    bool m_active;
};

#endif /* FCSERVICE_H_ */
