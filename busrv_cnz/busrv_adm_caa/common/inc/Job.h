#ifndef _JOB_H_
#define _JOB_H_

#pragma once
/*
NAME
	JOB - 
LIBRARY 3C++
PAGENAME JOB
HEADER  CPS  
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE Job.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2001. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the 
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION 
   Interface definition for all Job classes - pure abstract class.

ERROR HANDLING
   -

DOCUMENT NO
   190 89-CAA 109 0387

AUTHOR 
   2001-05-14 by U/Y/SF Anders Gillgren (qabgill, tag: ag)

LINKAGE
	-

.SEE ALSO
   -

*/

#include "PlatformTypes.h"

//
//
//===========================================================================
struct Job {
public:

	virtual ~Job() {}						// Memory leak without this destructor

	virtual void execute() = 0;				// run job
	virtual long jobId() const = 0;			// get JobId
	virtual void abort() = 0;				// set abort
	virtual bool isAborted() const = 0;		// are we aborting?
	virtual bool completed() const = 0;		// job done?
	virtual bool autodelete() const = 0;	// should the job be autodeleted?
	virtual void autodelete(bool ) = 0;		// set autodelete (only used by JobThread to clean up)
	virtual u_int32 exitcode() const = 0;		// what's the exit code?


	// NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
	// Backup Protocol Interaction
	//------------------------------------------------------------------------
	enum JQS_STATE { 
		JQS_NORMAL = 0,	// Normal state, returns the result to TCP thread
		JQS_RSPCONT,	// Return result to TCP thread and continue with execution
						// of the Helper in Job thread
		JQS_QUEUED,		// Can not proceed with the current job, queue it up
		JQS_FLUSH		// Flush the Job Queue
	};

	virtual void resetState() = 0;					// Reset Job Queue state
	virtual void resetState(JQS_STATE state, u_int16 sbc, Job* job = NULL) = 0;
	virtual void executeSpecial(u_int16 sbc) = 0;	// Special operation during Job Queue flushing
	virtual void executeNonOp(u_int16 sbc) = 0;		// Just return the response to client
	virtual JQS_STATE getState(u_int16& sbc) = 0;	// Read state of Job Queue
	virtual Job* getHelpJob() = 0;

};

#endif
