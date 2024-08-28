#ifndef _CPS_FCSRV_Trace_H_
#define _CPS_FCSRV_Trace_H_
/*
NAME
	File_name:CPS_FCSRV_Trace.cpp

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
	190 89-CAA 109 1410

AUTHOR
	2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on WCPS_FCSRV_Trace.h of Windows version

SEE ALSO
	-

Revision history
----------------
111115 xtuudoo Create based on CPS_BUSRV_Trace.h
120830 xngudan Update - logging to file

*/

#include "CriticalSection.h"
#include "ACS_TRA_trace.h"
#include "ACS_TRA_Logging.h"
#include "boost/scoped_ptr.hpp"


class CPS_FCSRV_Trace
{
public:

	CPS_FCSRV_Trace();
	~CPS_FCSRV_Trace();
	//void newTrace(const char *fmt, int dummy, ...);
	void newTrace(int iSeverity, const char *fmt, int dummy, ...);
	void Trace(int iSeverity, const char *fmt, int dummy, ...);
	//void Trace(const char *fmt, int dummy, ...);

	static bool isTraceable();
	static bool isLogable();
	static bool isTraceLogable();
	static void Initialise();
	static void Terminate();

private:
	// noncopyable
	CPS_FCSRV_Trace(const CPS_FCSRV_Trace&);
	CPS_FCSRV_Trace& operator=(const CPS_FCSRV_Trace&);

	static int        s_nestTraceLevel;          // call nesting level
	static int        s_nestLogLevel;
	static int        s_maxNestLevel;       // maximum call nesting level for trace output
	static CriticalSection s_cs;

	int				  m_nestTraceLevel;
	int               m_nestLogLevel;
	bool              m_newTrace;
	bool              m_newLog;

	static boost::scoped_ptr<ACS_TRA_trace> s_trcb; // trace control block
	static boost::scoped_ptr<ACS_TRA_Logging> s_logcb;
};

inline
CPS_FCSRV_Trace::CPS_FCSRV_Trace() : m_nestTraceLevel(0), m_nestLogLevel(0),
			m_newTrace(true), m_newLog(true)
{
}

inline
bool CPS_FCSRV_Trace::isTraceable()
{
    return (s_nestTraceLevel <= s_maxNestLevel) && s_trcb->isOn()
    ;
}

inline
bool CPS_FCSRV_Trace::isLogable()
{
    return s_nestLogLevel <= s_maxNestLevel;
}

inline
bool CPS_FCSRV_Trace::isTraceLogable()
{
    return (s_nestTraceLevel <= s_maxNestLevel && s_trcb->isOn()) ||
    		(s_nestLogLevel <= s_maxNestLevel);
}

#define initTRACE()								\
        CPS_FCSRV_Trace::Initialise();

#define isTRACEABLE()							\
        CPS_FCSRV_Trace::isTraceLogable()

#define newTRACE(p1)							\
        CPS_FCSRV_Trace Trace;                  \
        if (isTRACEABLE()) {                    \
            Trace.newTrace p1;                  \
        }

#define TRACE(p1)								\
        if (isTRACEABLE()) {                    \
            Trace.Trace p1;                     \
        }

#define termTRACE()								\
        CPS_FCSRV_Trace::Terminate();

#endif

