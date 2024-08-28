//******************************************************************************
// 
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
//  CPS_BUAP_trace.h
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
// 	1999/06/06 by UAB/B/SF Birgit Berggren
//
//******************************************************************************
// === Revision history ===
// 990531 BIR PA1 Created.
// 990617 BIR PA2 Updated after review.
// 991123 BIR PA3 "syslog" replaced by "AP_SysLog_BUAP"(an own kind of NT 
//                Event Log)
//  101010 xchihoa     Ported to Linux for APG43L.
//  120404 xngudan     Logging trace to file.
//******************************************************************************



#ifndef CPS_BUAP_Trace_H
#define CPS_BUAP_Trace_H

#include "ACS_TRA_trace.h"
#include "ACS_TRA_Logging.h"
#include "CPS_BUAP_CS.h"

#include "boost/scoped_ptr.hpp"

class CPS_BUAP_Trace
{
public:

	CPS_BUAP_Trace();
	~CPS_BUAP_Trace();
        void newTrace(ACS_TRA_LogLevel, const char *fmt, int dummy, ...);
	void newTrace(const char *fmt, int dummy, ...);
        void Trace(ACS_TRA_LogLevel, const char *fmt, int dummy, ...); 
	void Trace(const char *fmt, int dummy, ...);
	
	static bool isTraceable();
	static bool isLogable();
	static bool isTraceLogable();
	static void Initialise();
	static void Terminate();
	 
private:

	CPS_BUAP_Trace(const CPS_BUAP_Trace&);
	CPS_BUAP_Trace& operator=(const CPS_BUAP_Trace&);

	static int         s_nestTraceLevel;     // call nesting level
	static int         s_nestLogLevel; 
	static int         s_maxNestLevel;       // maximum call nesting level for trace output
	static CPS_BUAP_CS s_cs;
	
	int				  m_nestTraceLevel;
	int				  m_nestLogLevel;
	bool              m_newTrace;
	bool              m_newLog;
	
	static boost::scoped_ptr<ACS_TRA_trace> s_trcb; 
        static boost::scoped_ptr<ACS_TRA_Logging> s_logcb;

};


inline
CPS_BUAP_Trace::CPS_BUAP_Trace() : m_nestTraceLevel(0), m_nestLogLevel(0), m_newTrace(true), m_newLog(true)
{
}

inline
bool CPS_BUAP_Trace::isTraceable()
{
    return s_nestTraceLevel <= s_maxNestLevel && s_trcb->isOn();
}

inline
bool CPS_BUAP_Trace::isLogable()
{
    return s_nestLogLevel <= s_maxNestLevel;
}

inline
bool CPS_BUAP_Trace::isTraceLogable()
{
    return (s_nestTraceLevel <= s_maxNestLevel && s_trcb->isOn()) || (s_nestLogLevel <= s_maxNestLevel);
}

#define initTRACE()								\
        CPS_BUAP_Trace::Initialise();

#define isTRACEABLE()                           \
        CPS_BUAP_Trace::isTraceLogable()
        
#define newTRACE(p1)							\
        CPS_BUAP_Trace Trace;                  \
        if(CPS_BUAP_Trace::isTraceLogable()) { \
            Trace.newTrace p1;                  \
        }
 
#define TRACE(p1)								\
        if(CPS_BUAP_Trace::isTraceLogable()) { \
            Trace.Trace p1;                     \
        }                 

#define termTRACE()								\
        CPS_BUAP_Trace::Terminate();

#endif
