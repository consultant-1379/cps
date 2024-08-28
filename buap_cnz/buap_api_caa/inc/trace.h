/*
 *   @file trace.h
 *   xngudan	
 *
 */

#ifndef TRACE_H
#define TRACE_H

#include "ACS_TRA_trace.h"
#include "CPS_BUAP_CS.h"

class CPS_BUAP_Lib_Trace
{
public:

        CPS_BUAP_Lib_Trace();
        ~CPS_BUAP_Lib_Trace();
        void newTrace(const char *fmt, int dummy, ...);
        void Trace(const char *fmt, int dummy, ...);

        static bool isTraceable();

private:

        CPS_BUAP_Lib_Trace(const CPS_BUAP_Lib_Trace&);
        CPS_BUAP_Lib_Trace& operator=(const CPS_BUAP_Lib_Trace&);

        static int         s_nestTraceLevel;     // call nesting level
        static int         s_maxNestLevel;       // maximum call nesting level for trace output

        int                               m_nestTraceLevel;
        bool              m_newTrace;

        static ACS_TRA_trace s_trcb;
	static CPS_BUAP_CS s_cs;
};


inline
CPS_BUAP_Lib_Trace::CPS_BUAP_Lib_Trace() : m_nestTraceLevel(0), m_newTrace(true)
{
}

inline
bool CPS_BUAP_Lib_Trace::isTraceable()
{
    return s_nestTraceLevel <= s_maxNestLevel;
}


#define isTRACEABLE()                           \
        CPS_BUAP_Lib_Trace::isTraceable()

#define newTRACE(p1)                                                    \
        CPS_BUAP_Lib_Trace Trace;                  \
        if(CPS_BUAP_Lib_Trace::isTraceable()) { \
            Trace.newTrace p1;                  \
        }
#define TRACE(p1)								\
        if(CPS_BUAP_Lib_Trace::isTraceable()) { \
            Trace.Trace p1;                     \
        }

#endif //TRACE_H
