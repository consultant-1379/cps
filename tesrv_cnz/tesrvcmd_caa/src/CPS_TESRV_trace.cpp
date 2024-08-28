/*
NAME
	File_name:CPS_TESRV_trace.cpp

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
	190 89-CAA 109 0387

AUTHOR
	2002-10-10 by EAB/KY/SF Mona (uabmnst, tag :mnst)
	2010-09-11 by DEK/xdtthng

SEE ALSO
	-

Revision history
----------------
2002-10-10 uabmnst Created
2010-12-31 xdtthng Remove global object; 
           Follow DR, only collect trace data if traceable;
           Keep exact functionality as Windows version

*/


#include <cstdio>
#include <cstdlib>
#include <cstdarg>

#include "CPS_TESRV_trace.h"

int	CPS_TESRV_trace::s_nestLevel     = 0;
int	CPS_TESRV_trace::s_maxNestLevel  = 12;
CriticalSection CPS_TESRV_trace::s_cs;

boost::scoped_ptr<ACS_TRA_trace> CPS_TESRV_trace::s_trcb;


//******************************************************************************
//	newTrace()
//******************************************************************************
void CPS_TESRV_trace::newTrace(const char *fmt, int dummy, ...)
{
	char     l_traceText[256];
	va_list  l_args;
	
    {
	    AutoCS a(s_cs);
        m_nestLevel = m_newTrace? m_newTrace = false, s_nestLevel++ : s_nestLevel;
    }
	int l_index = sprintf(l_traceText, "<%d> %*s", m_nestLevel, m_nestLevel*2, " ");

	va_start(l_args, dummy);
	vsprintf(&l_traceText[l_index], fmt, l_args);
	va_end(l_args);

	s_trcb->ACS_TRA_event(1, l_traceText);

#ifdef HAVE_CONSOLE_TRA
	puts(l_traceText);
#endif
    
}

//******************************************************************************
//	Trace()
//******************************************************************************
void CPS_TESRV_trace::Trace(const char *fmt, int dummy, ...)
{
	char     l_traceText[1024];
	char     l_lineout[256];
	char     *l_pLine;
	char	   *last;
	va_list  l_args;

	sprintf(l_traceText, "<%d> %*s", m_nestLevel, m_nestLevel*2+1, " ");

	va_start(l_args, dummy);
	vsprintf(l_traceText, fmt, l_args);
	va_end(l_args);

	l_pLine = strtok_r(l_traceText, "\n", &last);
	//l_pLine = strtok(l_traceText, "\n");

	while (l_pLine) {
		sprintf(l_lineout, "<%d> %*s %s", m_nestLevel, m_nestLevel*2+1, " ", l_pLine);
          
		s_trcb->ACS_TRA_event(1, l_lineout);

#ifdef HAVE_CONSOLE_TRA
	    puts(l_lineout);         
#endif
	    l_pLine = strtok_r(NULL, "\n", &last);
	    //l_pLine = strtok(NULL, "\n");
    }
}

//******************************************************************************
//	Terminate()
//******************************************************************************
void CPS_TESRV_trace::Terminate()
{
    s_trcb.reset();
}

//******************************************************************************
//	Initialise()
//******************************************************************************
void CPS_TESRV_trace::Initialise()
{
	
	s_trcb.reset(new ACS_TRA_trace("", "C180"));

	char *l_envptr = getenv("CPS_TESRV_MAXNESTLEVEL");
	if (l_envptr)
	{
		int level = atoi(l_envptr);
		s_maxNestLevel = ((level > 0) ? level : s_maxNestLevel);
	}
}
