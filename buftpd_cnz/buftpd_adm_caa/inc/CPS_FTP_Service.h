/*
NAME
    File_name:CPS_FTP_Service.h

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
2011-08-19 xquydao Created

*/

#ifndef CPS_FTP_SERVICE_H_
#define CPS_FTP_SERVICE_H_

#include "ACS_APGCC_ApplicationManager.h"
#include "CPS_FTP_Server.h"
#include "boost/thread.hpp"

#include <string>

class CPS_FTP_Service : public ACS_APGCC_ApplicationManager
{
public:
    static const std::string BUFTP_DEF_DAEMON_NAME;

public:
    CPS_FTP_Service(const std::string& daemonName, CPS_FTP_Server& server);
    virtual ~CPS_FTP_Service();

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
    // BUFTPD Server
    CPS_FTP_Server& m_server;

    // Threads
    boost::thread m_server_thread;

    // Stop Flag
    bool m_stop;

    // Active Flag;
    bool m_active;
};

#endif /* CPS_FTP_Service_H_ */
