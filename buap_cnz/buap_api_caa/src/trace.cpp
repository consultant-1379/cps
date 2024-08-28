/***************************************************
 *
 *  @file trace.cpp
 *   xngudan 
 *
****************************************************/

#include <trace.h>

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <iostream>


int	CPS_BUAP_Lib_Trace::s_nestTraceLevel = 0;
int	CPS_BUAP_Lib_Trace::s_maxNestLevel = 23;

ACS_TRA_trace CPS_BUAP_Lib_Trace::s_trcb((char*)"libcps_buap", (char*)"C180");       // Trace object, used to be known as trace control block
CPS_BUAP_CS CPS_BUAP_Lib_Trace::s_cs;

CPS_BUAP_Lib_Trace::~CPS_BUAP_Lib_Trace()
{
	AutoCS a(s_cs);
	if (!m_newTrace)
	   --s_nestTraceLevel;
}


//******************************************************************************
//	newTrace
//******************************************************************************
void CPS_BUAP_Lib_Trace::newTrace(const char *fmt, int dummy, ...)
{
	char       l_outText[256];
	char       l_rawText[256];     
	va_list    l_args;
	
	bool traceable = isTraceable();

    // At this point, either or both Trace or Log is On
    // Need to check one by one	   	
    {
	    AutoCS a(s_cs);
	if (traceable)
            m_nestTraceLevel = m_newTrace? m_newTrace = false, s_nestTraceLevel++ : s_nestTraceLevel;
    }
	
    // Collect arguments once for either or both trace and log
    va_start(l_args, dummy);
    vsprintf(l_rawText, fmt, l_args);
    va_end(l_args);

    // Only Trace if Trace is On
    int l_index;    
    if (traceable) 
    {	
    	l_index = sprintf(l_outText, "<%d> %*s", m_nestTraceLevel, (m_nestTraceLevel + 1)*2, " ");
    	sprintf(&l_outText[l_index], "%s", l_rawText);

        s_trcb.ACS_TRA_event(1, l_outText);
    }
}

//******************************************************************************
//	Trace()
//******************************************************************************
void CPS_BUAP_Lib_Trace::Trace(const char *fmt, int dummy, ...) 
{
	char       l_outText[1024];
	char       l_lineout[256];
	char*      l_pLine;
	char*      last;
	   	

	va_list l_args;
	va_start(l_args, dummy);
	vsprintf(l_outText, fmt, l_args);
	va_end(l_args);

    // Only Trace if Trace is On
	if (isTraceable()) {	
    	l_pLine = strtok_r(l_outText, "\n", &last);
    	while (l_pLine) {
    		sprintf(l_lineout, "<%d> %*s %s", m_nestTraceLevel, m_nestTraceLevel*2+1, " ", l_pLine);          
            s_trcb.ACS_TRA_event(1, l_lineout);
    	    l_pLine = strtok_r(NULL, "\n", &last);
        }
    }

}


