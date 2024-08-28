#ifndef _SBCINFOJOB_H_
#define _SBCINFOJOB_H_
/*
NAME
	SBCINFOJOB -
LIBRARY 3C++
PAGENAME SBCINFOJOB
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE SBCInfoJob.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.
	
	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION
	Executes the BUP command SBC_INFO.

ERROR HANDLING
	-

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
	2002-02-05 by UAB/KY/SK Anders Gillgren (qabgill, tag: ag)

LINKAGE
	-

SEE ALSO
	
Revision history
----------------
2002-02-05 qabgill Created
2002-02-20 uablan updated the class

*/
#include "BUPJob.h"

class SBCInfoJob : public BUPJob {
//foos
public:
	SBCInfoJob(const BUPMsg& msg) : BUPJob(msg) { }
	virtual ~SBCInfoJob() { }
	virtual void execute();
private:
private:
	SBCInfoJob(const SBCInfoJob& rhs);
	SBCInfoJob& operator=(const SBCInfoJob& rhs);
//attr
private:
};


//
// inlines
//====================================================================
class SBCInfoJob_V4 : public BUPJob {
//foos
public:
	SBCInfoJob_V4(const BUPMsg& msg) : BUPJob(msg) { }
	virtual ~SBCInfoJob_V4() { }
	virtual void execute();

private:
	SBCInfoJob_V4(const SBCInfoJob_V4& rhs);
	SBCInfoJob_V4& operator=(const SBCInfoJob_V4& rhs);
//attr
private:
};


#endif
