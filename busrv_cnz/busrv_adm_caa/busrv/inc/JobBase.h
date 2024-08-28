#ifndef _JOBBASE_H
#define _JOBBASE_H
/*
NAME
	JOBBASE -
LIBRARY 3C++
PAGENAME JOBBASE
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE JobBase.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION
	Base class for (most) Jobs. Created when Job was changed into an
	interface.

  This class implements most required functions for Job classes.

ERROR HANDLING
	-

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
	2002-02-27 by U/Y/SF Anders Gillgren (qabgill, tag: ag)

LINKAGE
	-

SEE ALSO
	-

*/

//#include "AutoHandle.h"
#include "BUPDefs.h"
#include "Job.h"

//
// Default catch used in derived Job execute methods
//----------------------------------------------------------------------

#if 0
#define JOB_CATCH() \
catch(const Win32Exception& x) { \
	Globals::instance().eventRep().write(x); \
	m_exitcode = BUPMsg::INTERNAL_ERROR; \
} \
catch(const Exception& x) { \
	Globals::instance().eventRep().write(x); \
	m_exitcode = BUPMsg::INTERNAL_ERROR; \
} \
catch(...) { \
	Globals::instance().eventRep().write("Unknown exception caught."); \
	m_exitcode = BUPMsg::INTERNAL_ERROR; \
}
#endif

class JobBase : public Job {
// types
public:
	enum { INVALID_JOB_ID = 0 }; // never use 0 for a job id

	class JobQueueState
	{
		public:
		JobQueueState() : m_next(JQS_NORMAL), m_sbc(0xFF), m_helper(NULL) {}
		void reset() {
			m_next = JQS_NORMAL;
			m_sbc = 0xFF;
		}

		void reset(JQS_STATE state, u_int16 sbc, Job* helper = NULL) {
			m_next = state;
			m_sbc = sbc;
			m_helper = helper;
		}

		JQS_STATE	m_next;
		u_int16		m_sbc;
		Job*		m_helper;
	};

public:
	virtual ~JobBase() { }
	// access
	long jobId() const { return m_jobId; }
	virtual void abort() { } // set abort event, no-op here, override if needed
	virtual bool isAborted() const { return  false; } // have we aborted? -:-

	virtual void executeSpecial(u_int16 sbc);
	virtual void executeNonOp(u_int16) {}
	virtual void resetState();
	virtual void resetState(JQS_STATE state, u_int16 sbc, Job* job = NULL);
	virtual JQS_STATE getState(u_int16& sbc);
	virtual Job* getHelpJob() { return m_state.m_helper; }

	bool completed() const { return m_completed; }
	bool autodelete() const { return m_autodelete; }
	// this func is only used by JobThread for cleanup
	void autodelete(bool flag) { m_autodelete = flag; }
	u_int32 exitcode() const { return m_exitcode; }
protected:
	JobBase(bool autodelete = false);
private:
	JobBase(const JobBase& );
	JobBase& operator=(const JobBase& );
protected:
	bool m_autodelete;
	bool m_completed;
	long m_jobId;
	u_int32 m_exitcode;
	JobQueueState m_state;
	static long s_jobIdCounter;
};
//
// inlines
//===========================================================================

#endif
