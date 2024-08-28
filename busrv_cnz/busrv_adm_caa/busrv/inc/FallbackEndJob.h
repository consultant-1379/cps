#ifndef _FALLBACKENDJOB_H_
#define _FALLBACKENDJOB_H_
/*
NAME
	FALLBACKENDJOB -
LIBRARY 3C++
PAGENAME FALLBACKENDJOB
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE FallbackEndJob.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION
	Executes the BUP command FALLBACK_END.

ERROR HANDLING
	-

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
	2002-05-02 by U/Y/SF Anders Gillgren (qabgill, tag: ag)

LINKAGE
	-

SEE ALSO
	

*/
#include "BUPJob.h"

// fwd
class BUPMsg;

class FallbackEndJob : public BUPJob {
//foos
public:
	FallbackEndJob(const BUPMsg& msg) : BUPJob(msg) { }
	virtual ~FallbackEndJob() { }
	virtual void execute();
private:
	FallbackEndJob(const FallbackEndJob& rhs);
	FallbackEndJob& operator=(const FallbackEndJob& rhs);
//attr
private:

};
//
// inlines
//===========================================================================

class FallbackEndJob_V4 : public BUPJob {
//foos
public:
	FallbackEndJob_V4(const BUPMsg& msg) : BUPJob(msg) { }
	virtual ~FallbackEndJob_V4() { }
	virtual void execute();
private:
	//void executeBupVer3();
	FallbackEndJob_V4(const FallbackEndJob_V4& rhs);
	FallbackEndJob_V4& operator=(const FallbackEndJob_V4& rhs);
//attr
private:

};


#endif
