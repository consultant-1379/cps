#ifndef _FCPJOBFACTORY_H_
#define _FCPJOBFACTORY_H_
/*
NAME
	FCPJOBFACTORY -
LIBRARY 3C++
PAGENAME FCPJOBFACTORY
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE FCPJobFactory.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.
	
	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION
	Creates Job that corresponds to an incoming FCPMsg (FCP command).

ERROR HANDLING
	-

DOCUMENT NO
	190 89-CAA 109 1410

AUTHOR
	2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on FCPJobFactory.h of FCSRV in Windows

LINKAGE
	-

SEE ALSO
	-

*/
#include "FCPJob.h"
#include "FCPMsg.h"
// fwd


class FCPJobFactory : public Job {
//foos
public:
	FCPJobFactory(const FCPMsg& );
	long jobId() const { return m_job->jobId(); }
	bool autodelete() const { return m_job->autodelete(); }
	void autodelete(bool flag) { m_job->autodelete(flag); }
	u_int32 exitcode() const { return m_job->exitcode(); }
	void abort() { m_job->abort(); }
	bool isAborted() const { return m_job->isAborted(); }
	bool completed() const { return m_job->completed(); }
	virtual void execute() { m_job->execute(); }
	virtual void response(FCPMsg& msg) { m_job->response(msg); }
	virtual ~FCPJobFactory() { delete m_job; }

	virtual void executeSpecial(unsigned short sbc) { m_job->executeSpecial(sbc); }
	virtual void executeNonOp(unsigned short sbc) { m_job->executeNonOp(sbc); }
	virtual void resetState() { m_job->resetState(); }
	virtual JQS_STATE getState(u_int16& sbc) { return m_job->getState(sbc); }
	virtual void resetState(JQS_STATE state, u_int16 sbc, Job* job) {
		m_job->resetState(state, sbc, job);
	}
	virtual Job* getHelpJob() { return m_job->getHelpJob(); }

private:
	FCPJobFactory(const FCPJobFactory& rhs);
	FCPJobFactory& operator=(const FCPJobFactory& rhs);
//attr
private:
	FCPJob* m_job; // the actual Job
};
//
// inlines
//====================================================================

#endif


