#ifndef _LOADENDJOB_H_
#define _LOADENDJOB_H_
/*
NAME
	LOADENDJOB -
LIBRARY 3C++
PAGENAME LOADENDJOB
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE LoadEndJob.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.
	
	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION
	Executes the BUP command LOAD_END.

ERROR HANDLING
	-

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
	2002-02-05 by UAB/KY/SK Anders Gillgren (qabgill, tag: ag)

LINKAGE
	-

SEE ALSO
	

*/
#include "BUPJob.h"

class BUPMsg;

class LoadEndJob : public BUPJob {
//foos
public:
	LoadEndJob(const BUPMsg& msg) : BUPJob(msg) { }
	virtual ~LoadEndJob() { }
	virtual void execute();
private:
	LoadEndJob(const LoadEndJob& rhs);
	LoadEndJob& operator=(const LoadEndJob& rhs);
//attr
private:

};
//
// inlines
//====================================================================

class LoadEndJob_V4 : public BUPJob {
//foos
public:
	LoadEndJob_V4(const BUPMsg& msg) : BUPJob(msg) { }
	virtual ~LoadEndJob_V4() { }
	virtual void execute();

private:
	//void executeBupVer3();
	LoadEndJob_V4(const LoadEndJob_V4& rhs);
	LoadEndJob_V4& operator=(const LoadEndJob_V4& rhs);
//attr
private:

};

#endif
