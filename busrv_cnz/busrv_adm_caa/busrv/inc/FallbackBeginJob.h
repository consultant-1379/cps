#ifndef _FALLBACKBEGINJOB_H_
#define _FALLBACKBEGINJOB_H_
/*
NAME
	FALLBACKBEGINJOB -
LIBRARY 3C++
PAGENAME FALLBACKBEGINJOB
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE FallbackBeginJob.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION
	Executes the BUP command FALLBACK_BEGIN.

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

class FallbackBeginJob : public BUPJob {
//foos
public:
	FallbackBeginJob(const BUPMsg& msg) : BUPJob(msg) { }
	virtual ~FallbackBeginJob() { }
	virtual void execute();
private:
	FallbackBeginJob(const FallbackBeginJob& rhs);
	FallbackBeginJob& operator=(const FallbackBeginJob& rhs);
//attr
private:

};

//
// inlines
//===========================================================================

class FallbackBeginJob_V4 : public BUPJob {
//foos
public:
	FallbackBeginJob_V4(const BUPMsg& msg) : BUPJob(msg) { }
	virtual ~FallbackBeginJob_V4() { }
	virtual void execute();
private:
	//void executeBupVer3();
	FallbackBeginJob_V4(const FallbackBeginJob_V4& rhs);
	FallbackBeginJob_V4& operator=(const FallbackBeginJob_V4& rhs);
//attr
private:

};

#endif

