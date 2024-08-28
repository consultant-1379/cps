/*
NAME
	File_name:JobBase.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

The Copyright to the computer program(s) herein is the property of Ericsson
Utvecklings AB, Sweden.
The program(s) may be used and/or copied only with the written permission from
Ericsson Utvecklings AB or in accordance with the terms and conditions
stipulated in the agreement/contract under which the program(s) have been
supplied.

DESCRIPTION
	Base class for (most) Jobs. Created when Job was changed into an
	interface.

  This class implements most required functions for Job classes.

DOCUMENT NO
	190 89-CAA 109 1410

AUTHOR
	2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on JobBase.cpp of FCSRV in Windows

SEE ALSO
	-

Revision history
----------------


*/

#include "CPS_FCSRV_Trace.h"
#include "JobBase.h"
//
// static init
//===========================================================================
long JobBase::s_jobIdCounter = JobBase::INVALID_JOB_ID;
//
// ctor
//===========================================================================
JobBase::JobBase(bool autodelete /* = false */)
: m_autodelete(autodelete), m_state() {

	newTRACE((LOG_LEVEL_INFO, "JobBase::JobBase(%d)", 0, autodelete));

	m_exitcode = 0;
	m_completed = false;

	// thread safe incr. of static variable
	m_jobId = ++s_jobIdCounter;
}

void JobBase::executeSpecial(u_int16)
{
	newTRACE((LOG_LEVEL_INFO, "JobBase::executeSpecial(u_int16)", 0));
}

void JobBase::resetState()
{
	m_state.reset();
}

Job::JQS_STATE JobBase::getState(u_int16& sbc)
{
	newTRACE((LOG_LEVEL_INFO, "JobBase::getState()", 0));
	sbc = m_state.m_sbc;
	TRACE((LOG_LEVEL_INFO, "JobBase::getState() returns sbc=%d, next state=%d", 0, sbc, m_state.m_next));
	return m_state.m_next;
}

void JobBase::resetState(JQS_STATE state, u_int16 sbc, Job* job)
{
	newTRACE((LOG_LEVEL_INFO, "JobBase::resetState(JQS_STATE=%d, sbc=%d)", 0, state, sbc));
	m_state.reset(state, sbc, job);
}
