#ifndef _CPS_TESRV_trace_H_
#define _CPS_TESRV_trace_H_
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

#include "ACS_TRA_trace.h"

#include "boost/scoped_ptr.hpp"
#include "CriticalSection.h"

class CPS_TESRV_trace
{
public:

	CPS_TESRV_trace();
	~CPS_TESRV_trace();
	void newTrace(const char *fmt, int dummy, ...); 
	void Trace(const char *fmt, int dummy, ...);
	bool isNewTrace();
	
	static bool isTraceable();
	static void Initialise();
	static void Terminate();
	 
private:

	CPS_TESRV_trace(const CPS_TESRV_trace&);
	CPS_TESRV_trace& operator=(const CPS_TESRV_trace&);

	static int        s_nestLevel;          // call nesting level
	static int        s_maxNestLevel;       // maximum call nesting level for trace output
	static CriticalSection s_cs;
	int				  m_nestLevel;
	bool              m_newTrace;
	static boost::scoped_ptr<ACS_TRA_trace> s_trcb; // trace control block

};

inline
CPS_TESRV_trace::CPS_TESRV_trace() : m_nestLevel(0), m_newTrace(true)
{
}

inline
CPS_TESRV_trace::~CPS_TESRV_trace()
{
	AutoCS a(s_cs);
	if (!m_newTrace)
	   s_nestLevel--;
}

inline
bool CPS_TESRV_trace::isNewTrace()
{
    return m_newTrace;
}

inline
bool CPS_TESRV_trace::isTraceable()
{
    return s_nestLevel <= s_maxNestLevel && s_trcb->isOn();
}

#define initTRACE()								\
        CPS_TESRV_trace::Initialise();

#define isTRACEABLE()							\
        CPS_TESRV_trace::isTraceable()
        
#define newTRACE(p1)							\
        CPS_TESRV_trace Trace;                  \
        if (isTRACEABLE()) {                    \
            Trace.newTrace p1;                  \
        }

#define TRACE(p1)								\
        if (isTRACEABLE()) {                    \
            if (Trace.isNewTrace()) {           \
                Trace.newTrace p1;              \
            }                                   \
            else Trace.Trace p1;                \
        }

#define termTRACE()								\
        CPS_TESRV_trace::Terminate();

#endif

