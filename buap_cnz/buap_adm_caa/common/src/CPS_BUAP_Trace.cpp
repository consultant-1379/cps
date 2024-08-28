//******************************************************************************
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 1999.
//  All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson Utvecklings AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.
//
//  NAME
//  CPS_BUAP_Trace.cpp 
//
//  DESCRIPTION 
//  Class implements a debug context class used in the
//  development of the reset of the code.  Note that no
//  methods are to be called directly, rather by use of
//  macros so that the debug code may be conditionally
//  compiled out.
//
//  DOCUMENT NO
//  190 89-CAA 109 1412
//
//  AUTHOR 
// 	1999/05/31 by UAB/B/SF Birgit Berggren
//
//******************************************************************************
// === Revision history ===
// 990531 BIR PA1 Created.
// 990617 BIR PA2 Updated after review.
// 991123 BIR PA3 "syslog" replaced by "TRA_LOG"(an own kind of NT
//                Event Log)
//  101010 xchihoa     Ported to Linux for APG43L.
//  120404 xngudan     Logging to file.
//******************************************************************************
#include "CPS_BUAP_Trace.h"

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <iostream>

using namespace std;

int	CPS_BUAP_Trace::s_nestTraceLevel = 0;
int	CPS_BUAP_Trace::s_nestLogLevel = 0;
int	CPS_BUAP_Trace::s_maxNestLevel = 23;
CPS_BUAP_CS CPS_BUAP_Trace::s_cs;

boost::scoped_ptr<ACS_TRA_trace> CPS_BUAP_Trace::s_trcb;       // Trace object, used to be known as trace control block
boost::scoped_ptr<ACS_TRA_Logging> CPS_BUAP_Trace::s_logcb;    // Log object

//******************************************************************************
/*
// Log level for trace log
enum ACS_TRA_LogLevel
{
	LOG_LEVEL_TRACE,
	LOG_LEVEL_DEBUG,
	LOG_LEVEL_INFO,
	LOG_LEVEL_WARN,
	LOG_LEVEL_ERROR,
	LOG_LEVEL_FATAL
};
*/


CPS_BUAP_Trace::~CPS_BUAP_Trace()
{
	AutoCS a(s_cs);
	if (!m_newTrace)
	   --s_nestTraceLevel;
	if (!m_newLog)
	   --s_nestLogLevel;
}


//******************************************************************************
//	newTrace
//******************************************************************************
void CPS_BUAP_Trace::newTrace(ACS_TRA_LogLevel logLevel, const char *fmt, int dummy, ...)
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
        //s_logcb->Write(l_outText, LOG_LEVEL_TRACE);
        s_logcb->Write(l_outText, logLevel);
    }	
   
}

void CPS_BUAP_Trace::newTrace(const char *fmt, int dummy, ...)
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
        s_logcb->Write(l_outText, LOG_LEVEL_TRACE);
        //s_logcb->Write(l_outText, logLevel);
    }

}



//******************************************************************************
//	Trace()
//******************************************************************************
void CPS_BUAP_Trace::Trace(ACS_TRA_LogLevel logLevel, const char *fmt, int dummy, ...) 
{
	char       l_outText[1024];
	char       l_lineout[256];
	char*      l_pLine;
	char*      last;
	   	
    // At this point, either or both Trace or Log is On	   	
	// Collect all arguments once for either or both Trace or Log	

	va_list l_args;
	va_start(l_args, dummy);
	vsprintf(l_outText, fmt, l_args);
	va_end(l_args);

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
	   l_pLine = strtok_r(l_outText, "\n", &last);
    	while (l_pLine) {
        	sprintf(l_lineout, "<%d> %*s %s", m_nestLogLevel, m_nestLogLevel*2+1, " ", l_pLine);          
            s_logcb->Write(l_lineout, logLevel);
            //s_logcb->Write(l_lineout, LOG_LEVEL_TRACE);
            l_pLine = strtok_r(NULL, "\n", &last);
        }
    }
}

void CPS_BUAP_Trace::Trace(const char *fmt, int dummy, ...)
{
        char       l_outText[1024];
        char       l_lineout[256];
        char*      l_pLine;
        char*      last;

    // At this point, either or both Trace or Log is On
        // Collect all arguments once for either or both Trace or Log

        va_list l_args;
        va_start(l_args, dummy);
        vsprintf(l_outText, fmt, l_args);
        va_end(l_args);

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
           l_pLine = strtok_r(l_outText, "\n", &last);
        while (l_pLine) {
                sprintf(l_lineout, "<%d> %*s %s", m_nestLogLevel, m_nestLogLevel*2+1, " ", l_pLine);
            //s_logcb->Write(l_lineout, logLevel);
            s_logcb->Write(l_lineout, LOG_LEVEL_TRACE);
            l_pLine = strtok_r(NULL, "\n", &last);
        }
    }
}


//******************************************************************************
//	Terminate()
//******************************************************************************
void CPS_BUAP_Trace::Terminate()
{
    s_trcb.reset();
    s_logcb.reset();
}

//******************************************************************************
//	Initialise()
//******************************************************************************
void CPS_BUAP_Trace::Initialise()
{
	
	s_trcb.reset(new ACS_TRA_trace("cps_buapd", "C180"));
	s_logcb.reset(new ACS_TRA_Logging());
	s_logcb->Open("BUAP");	

	char *l_envptr = getenv("CPS_BUAP_MAXNESTLEVEL");
	if (l_envptr)
	{
		int level = atoi(l_envptr);
		s_maxNestLevel = ((level > 0) ? level : s_maxNestLevel);
	}
}

