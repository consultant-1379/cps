#ifndef _CPS_BUSRV_Trace_H_
#define _CPS_BUSRV_Trace_H_
/*
NAME
	File_name:CPS_BUSRV_Trace.cpp

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
	2010-12-31 by DEK/xdtthng

SEE ALSO
	-

Revision history
----------------
2002-10-10 uabmnst Created
2010-12-31 xdtthng Remove global object; 
           Follow DR, only collect trace data if traceable;
           Keep exact functionality as Windows version

*/

#include <ACS_TRA_trace.h>
#include <ACS_TRA_Logging.h>

#include "boost/scoped_ptr.hpp"
#include "CriticalSection.h"

class CPS_BUSRV_Trace
{
public:

	CPS_BUSRV_Trace();
	~CPS_BUSRV_Trace();
	void newTrace(ACS_TRA_LogLevel log_level, const char *fmt, int dummy, ...);
	void Trace(ACS_TRA_LogLevel log_level, const char *fmt, int dummy, ...);
	
	static bool isTraceable();
	static bool isLogable();
	static bool isTraceLogable();
	static void Initialise();
	static void Terminate();
	 
private:

	CPS_BUSRV_Trace(const CPS_BUSRV_Trace&);
	CPS_BUSRV_Trace& operator=(const CPS_BUSRV_Trace&);

	static int         s_nestTraceLevel;     // call nesting level
	static int         s_nestLogLevel; 
	static int         s_maxNestLevel;       // maximum call nesting level for trace output
	static CriticalSection s_cs;
	
	int				  m_nestTraceLevel;
	int				  m_nestLogLevel;
	bool              m_newTrace;
	bool              m_newLog;
	
	static boost::scoped_ptr<ACS_TRA_trace> s_trcb; 
    static boost::scoped_ptr<ACS_TRA_Logging> s_logcb;

};


inline
CPS_BUSRV_Trace::CPS_BUSRV_Trace() : m_nestTraceLevel(0), m_nestLogLevel(0), m_newTrace(true), m_newLog(true)
{
}

inline
bool CPS_BUSRV_Trace::isTraceable()
{
    return s_nestTraceLevel <= s_maxNestLevel && s_trcb->isOn();
}

inline
bool CPS_BUSRV_Trace::isLogable()
{
    return s_nestLogLevel <= s_maxNestLevel;
}

inline
bool CPS_BUSRV_Trace::isTraceLogable()
{
    return (s_nestTraceLevel <= s_maxNestLevel && s_trcb->isOn()) || (s_nestLogLevel <= s_maxNestLevel);
}

#define initTRACE()								\
        CPS_BUSRV_Trace::Initialise();

#define isTRACEABLE()                           \
        CPS_BUSRV_Trace::isTraceLogable()
        
#define newTRACE(p1)							\
        CPS_BUSRV_Trace Trace;                  \
        if(CPS_BUSRV_Trace::isTraceLogable()) { \
            Trace.newTrace p1;                  \
        }
 
#define TRACE(p1)								\
        if(CPS_BUSRV_Trace::isTraceLogable()) { \
            Trace.Trace p1;                     \
        }                 

#define termTRACE()								\
        CPS_BUSRV_Trace::Terminate();

#endif

