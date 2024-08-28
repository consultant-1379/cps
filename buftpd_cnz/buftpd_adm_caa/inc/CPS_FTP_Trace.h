/*
NAME
    File_name:CPS_FTP_Trace.h

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
2011-14-07 xquydao Created (Most of the content is taken from BUSRV/CPS_BUSRV_Trace.h)

*/
#ifndef _CPS_FTP_Trace_H_
#define _CPS_FTP_Trace_H_


#include "ACS_TRA_trace.h"
#include "ACS_TRA_Logging.h"

#include "boost/scoped_ptr.hpp"
#include "CPS_FTP_CS.h"

class CPS_FTP_Trace
{
public:

    CPS_FTP_Trace();
    ~CPS_FTP_Trace();
    void newTrace(int iSeverity, const char *fmt, int dummy, ...);
    void Trace(int iSeverity, const char *fmt, int dummy, ...);
    bool isNewTrace() const;

    static bool isTraceable();
    static void Initialise();
    static void Terminate();

private:

    CPS_FTP_Trace(const CPS_FTP_Trace&);
    CPS_FTP_Trace& operator=(const CPS_FTP_Trace&);

    static int        s_nestLevel;          // call nesting level
    static int        s_maxNestLevel;       // maximum call nesting level for trace output
    static CPS_FTP_CS s_cs;
    int               m_nestLevel;
    bool              m_newTrace;
    static boost::scoped_ptr<ACS_TRA_trace> s_trcb; // trace control block
    static boost::scoped_ptr<ACS_TRA_Logging> s_logcb; // log control block

};

inline
CPS_FTP_Trace::CPS_FTP_Trace() : m_nestLevel(0), m_newTrace(true)
{
}

inline
CPS_FTP_Trace::~CPS_FTP_Trace()
{
    AutoCS a(s_cs);
    if (!m_newTrace)
       s_nestLevel--;
}

inline
bool CPS_FTP_Trace::isNewTrace() const
{
    return m_newTrace;
}

inline
bool CPS_FTP_Trace::isTraceable()
{
    return s_nestLevel <= s_maxNestLevel;
}

#define initTRACE()                             \
        CPS_FTP_Trace::Initialise();

#define isTRACEABLE()                           \
        CPS_FTP_Trace::isTraceable()

#define newTRACE(p1)                            \
        CPS_FTP_Trace Trace;                    \
        if (isTRACEABLE()) {                    \
            Trace.newTrace p1;                  \
        }

#define TRACE(p1)                               \
        if (isTRACEABLE()) {                    \
            if (Trace.isNewTrace()) {           \
                Trace.newTrace p1;              \
            }                                   \
            else Trace.Trace p1;                \
        }

#define termTRACE()                             \
        CPS_FTP_Trace::Terminate();

#endif
