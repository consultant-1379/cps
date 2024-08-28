/*
NAME
	File_name:CPS_FCSRV_Trace.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

The Copyright to the computer program(s) herein is the property of Ericsson
Utvecklings AB, Sweden.
The program(s) may be used and/or copied only with the written permission from
Ericsson Utvecklings AB or in accordance with the terms and conditions
stipulated in the agreement/contract under which the program(s) have been
supplied.

DESCRIPTION
	FCSRV configuration data.

DOCUMENT NO
	190 89-CAA 109 0670

AUTHOR
	2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on CPS_FCSRV_Trace of FCSRV in Windows

SEE ALSO
	-

Revision history
----------------
111115 xtuudoo Create based on CPS_BUSRV_Trace.cpp
120830 xngudan Update - logging to file

*/


#include <cstdio>
#include <cstdlib>
#include <cstdarg>

#include "CPS_FCSRV_Trace.h"

int	CPS_FCSRV_Trace::s_nestTraceLevel     = 0;
int	CPS_FCSRV_Trace::s_nestLogLevel     = 0;
int	CPS_FCSRV_Trace::s_maxNestLevel  = 14;
CriticalSection CPS_FCSRV_Trace::s_cs;

boost::scoped_ptr<ACS_TRA_trace> CPS_FCSRV_Trace::s_trcb;
boost::scoped_ptr<ACS_TRA_Logging> CPS_FCSRV_Trace::s_logcb;

CPS_FCSRV_Trace::~CPS_FCSRV_Trace()
{
    AutoCS a(s_cs);
    if (!m_newTrace)
       --s_nestTraceLevel;
    if (!m_newLog)
       --s_nestLogLevel;
}

//******************************************************************************
//	newTrace()
//******************************************************************************
void CPS_FCSRV_Trace::newTrace(int iSeverity, const char *fmt, int dummy, ...)
{
    char       l_outText[256];
    char       l_rawText[256];
    va_list    l_args;

    bool traceable = isTraceable();
    bool logable = isLogable();

    // At this point, either or both Trace or Log is On
    // Need to check one by one
    {
        AutoCS a(s_cs);
        if (traceable)
            m_nestTraceLevel = m_newTrace? m_newTrace = false, s_nestTraceLevel++ : s_nestTraceLevel;
        if (logable)
            m_nestLogLevel = m_newLog? m_newLog = false, s_nestLogLevel++ : s_nestLogLevel;
    }

    // Collect arguments once for either or both trace and log
    va_start(l_args, dummy);
    vsprintf(l_rawText, fmt, l_args);
    va_end(l_args);

    // Only Trace if Trace is On
    int l_index;
    if (traceable) {
        l_index = sprintf(l_outText, "<%d> %*s", m_nestTraceLevel, (m_nestTraceLevel + 1)*2, " ");
        sprintf(&l_outText[l_index], "%s", l_rawText);
        s_trcb->ACS_TRA_event(1, l_outText);
    }

    // Only Log if log level has not exceed max level
    if (logable) {
        l_index = sprintf(l_outText, "<%d> %*s", m_nestLogLevel, (m_nestLogLevel + 1)*2, " ");
        sprintf(&l_outText[l_index], "%s", l_rawText);
        s_logcb->Write(l_outText, (ACS_TRA_LogLevel)iSeverity);
    }
}

//******************************************************************************
//	Trace()
//******************************************************************************
void CPS_FCSRV_Trace::Trace(int iSeverity, const char *fmt, int dummy, ...)
{
    char       l_outText[512];
    char       l_outText_log[512];
    char       l_lineout[1024];
    char*      l_pLine;
    char*      last;

    // At this point, either or both Trace or Log is On
    // Collect all arguments once for either or both Trace or Log
    va_list l_args;
    va_start(l_args, dummy);
    vsnprintf(l_outText, 511, fmt, l_args);
    l_outText[511] = '\0';
    va_end(l_args);

    memcpy(l_outText_log, l_outText, 511);

    // Only Trace if Trace is On
    if (isTraceable()) {
        l_pLine = strtok_r(l_outText, "\n", &last);
        while (l_pLine) {
            sprintf(l_lineout, "<%d> %*s %s", m_nestTraceLevel, m_nestTraceLevel*2+1, " ", l_pLine);
            s_trcb->ACS_TRA_event(1, l_lineout);
            l_pLine = strtok_r(NULL, "\n", &last);
        }
    }

    // Only Log if log level has not exceeded max level
    if (isLogable()) {
       l_pLine = strtok_r(l_outText_log, "\n", &last);
        while (l_pLine) {
            sprintf(l_lineout, "<%d> %*s %s", m_nestLogLevel, m_nestLogLevel*2+1, " ", l_pLine);
            s_logcb->Write(l_lineout, (ACS_TRA_LogLevel)iSeverity);
            l_pLine = strtok_r(NULL, "\n", &last);
        }
    }
}

//******************************************************************************
//	Terminate()
//******************************************************************************
void CPS_FCSRV_Trace::Terminate()
{
    s_trcb.reset();
    s_logcb.reset();
}

//******************************************************************************
//	Initialise()
//******************************************************************************
void CPS_FCSRV_Trace::Initialise()
{
	s_trcb.reset(new ACS_TRA_trace("", "C180"));
	s_logcb.reset(new ACS_TRA_Logging());
	s_logcb->Open("FCSRV");

	char *l_envptr = getenv("CPS_FCSRV_MAXNESTLEVEL");
	if (l_envptr)
	{
		int level = atoi(l_envptr);
		s_maxNestLevel = ((level > 0) ? level : s_maxNestLevel);
	}
}

