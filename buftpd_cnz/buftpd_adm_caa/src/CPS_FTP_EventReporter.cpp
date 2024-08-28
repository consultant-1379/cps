/*
NAME
    File_name:CPS_FTP_EventReporter.cpp

Ericsson Utvecklings AB

    COPYRIGHT Ericsson Utvecklings AB, Sweden 1999, 2011. All rights reserved.

    The Copyright to the computer program(s) herein is the property of
    Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
    copied only with the written permission from Ericsson Utvecklings AB
    or in accordance with the terms and conditions stipulated in the
    agreement/contract under which the program(s) have been supplied.

DESCRIPTION
    This class is use for sending the AP Event

DOCUMENT NO
    190 89-CAA 109 1415

AUTHOR
    2003-03-31 by EAB/UKY/SF Mona Notterkvist (uabmnst, tag :mnst)
    2011-14-07 by XDT/DEK/XQUYDAO

SEE ALSO
    -

Revision history
----------------
2003-03-31 uabmnst Created
2011-14-07 xquydao Updated for APG43L

*/
#include "CPS_FTP_EventReporter.h"
#include "CPS_FTP_Trace.h"
#include "CPS_FTP_CS.h"

#include <cstdio>
#include <cstdlib>
#include <cstdarg>

#include <sys/types.h>
#include <unistd.h>

CPS_FTP_EventReporter CPS_FTP_EventReporter::s_instance;
char CPS_FTP_EventReporter::s_processName[32] = {0};

void CPS_FTP_EventReporter::send(const CPS_FTP_EventStruct &eventStruct, int dummy, ...)
{
	newTRACE((LOG_LEVEL_WARN, "CPS_FTP_EventReporter::Send()", 0));
	AutoCS a(s_cs);

    char problemData[256] = {0};

    if (s_processName[0] == 0)
    {
        pid_t pid = getpid();
        sprintf(s_processName, "%s:%u", "cps_buftpd", pid);
    }

    va_list l_args;
    va_start(l_args, dummy);
    vsprintf(problemData, eventStruct.formatString, l_args);
    va_end(l_args);

    TRACE((LOG_LEVEL_WARN, "Sending event %ld - %s", 0, eventStruct.specificProblem, eventStruct.problemText));

    int ret = m_evhr.sendEventMessage(s_processName, eventStruct.specificProblem,
                                      eventStruct.percSeverity, eventStruct.probableCause,
                                      eventStruct.objClassOfReference, eventStruct.objectOfReference,
                                      eventStruct.problemText, problemData);

    if (ret != ACS_AEH_ok)
    {
        TRACE((LOG_LEVEL_ERROR, "Failed to send event: %d - %s", 0, m_evhr.getError(), m_evhr.getErrorText()));
    }
}
