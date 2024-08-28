/*
NAME
    File_name:CPS_FTP_Trace.cpp

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
    190 89-CAA 109 1415

AUTHOR
    2011-14-07 by XDT/DEK/XQUYDAO


SEE ALSO
    -

Revision history
----------------
2011-14-07 xquydao Created (Most of the content is taken from BUSRV/CPS_BUSRV_Trace.cpp)

*/

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <string>

#include "CPS_FTP_Trace.h"

int CPS_FTP_Trace::s_nestLevel     = 0;
int CPS_FTP_Trace::s_maxNestLevel  = 12;
CPS_FTP_CS CPS_FTP_Trace::s_cs;

boost::scoped_ptr<ACS_TRA_trace> CPS_FTP_Trace::s_trcb;
boost::scoped_ptr<ACS_TRA_Logging> CPS_FTP_Trace::s_logcb;


//******************************************************************************
//  newTrace()
//******************************************************************************
void CPS_FTP_Trace::newTrace(int iSeverity, const char *fmt, int dummy, ...)
{
    int      num = 256;
    char     l_traceText[num];
    va_list  l_args;

    {
        AutoCS a(s_cs);
        m_nestLevel = m_newTrace? m_newTrace = false, s_nestLevel++ : s_nestLevel;
    }
    //  "TRACE", "DEBUG", "INFO ", "WARN ", "ERROR", "FATAL"
    std::string aStrSeverityText[] = { "T", "D", "I", "W", "E", "F" };

    int l_index = snprintf(l_traceText, num, "<%d> %*s<%s>", m_nestLevel, m_nestLevel*2, " ", aStrSeverityText[iSeverity].c_str());

    va_start(l_args, dummy);
    vsnprintf(&l_traceText[l_index], num - l_index, fmt, l_args);
    va_end(l_args);

    // Log to file with trace level by default
    s_logcb->Write(l_traceText, (ACS_TRA_LogLevel)iSeverity);

    // Log to trautil when the BUFTPD trace definition is turned on
    if (s_trcb->isOn())
    {
        s_trcb->ACS_TRA_event(1, l_traceText);
    }

#ifdef HAVE_CONSOLE_TRA
    puts(l_traceText);
#endif

}

//******************************************************************************
//  Trace()
//******************************************************************************
void CPS_FTP_Trace::Trace(int iSeverity, const char *fmt, int dummy, ...)
{
    int num  = 256;
    char     l_traceText[num];
    va_list  l_args;
    //  "TRACE", "DEBUG", "INFO ", "WARN ", "ERROR", "FATAL"
    std::string aStrSeverityText[] = { "T", "D", "I", "W", "E", "F" };

    int l_index = snprintf(l_traceText, num, "<%d> %*s<%s>", m_nestLevel, m_nestLevel * 2 + 1, " ", aStrSeverityText[iSeverity].c_str());

    va_start(l_args, dummy);
    vsnprintf(&l_traceText[l_index], num - l_index, fmt, l_args);
    va_end(l_args);

    // Log to file with trace level by default
    s_logcb->Write(l_traceText, (ACS_TRA_LogLevel)iSeverity);

    // Log to trautil when the BUFTPD trace definition is turned on
    if (s_trcb->isOn())
    {
        s_trcb->ACS_TRA_event(1, l_traceText);
    }

#ifdef HAVE_CONSOLE_TRA
    puts(l_lineout);
#endif
}

//******************************************************************************
//  Terminate()
//******************************************************************************
void CPS_FTP_Trace::Terminate()
{
    s_trcb.reset();
    s_logcb.reset();
}

//******************************************************************************
//  Initialise()
//******************************************************************************
void CPS_FTP_Trace::Initialise()
{
    s_trcb.reset(new ACS_TRA_trace("", "C180"));
    s_logcb.reset(new ACS_TRA_Logging());
    s_logcb->Open("BUFTPD");

    char *l_envptr = getenv("CPS_BUFTPD_MAXNESTLEVEL");
    if (l_envptr)
    {
        int level = atoi(l_envptr);
        s_maxNestLevel = ((level > 0) ? level : s_maxNestLevel);
    }
}
