#ifndef _UNKNOWNJOB_H_
#define _UNKNOWNJOB_H_
/*
NAME
	UNKNOWNJOB -
LIBRARY 3C++
PAGENAME UNKNOWNJOB
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE UnknownJob.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION
	Handles unknown primitives received in BUP.

ERROR HANDLING
	-

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
	2002-03-13 by U/Y/SF Anders Gillgren (qabgill, tag: ag)

LINKAGE
	-

SEE ALSO
	-

*/
#include "BUPJob.h"

class UnknownJob : public BUPJob {
//foos
public:
	UnknownJob(const BUPMsg& msg) : BUPJob(msg) { }
	virtual void execute();
private:
	UnknownJob(const UnknownJob& rhs);
	UnknownJob& operator=(const UnknownJob& rhs);
//attr
private:

};
//
// inlines
//===========================================================================
class UnknownTestJob : public BUPJob {
//foos
public:
	UnknownTestJob(const BUPMsg& msg) : BUPJob(msg) { }
	virtual void execute();
private:
	UnknownTestJob(const UnknownTestJob& rhs);
	UnknownTestJob& operator=(const UnknownTestJob& rhs);
//attr
private:

};

#endif
