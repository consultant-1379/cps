#ifndef _LISTSBCSJOB_H_
#define _LISTSBCSJOB_H_
/*
NAME
	LISTSBCSJOB -
LIBRARY 3C++
PAGENAME LISTSBCSJOB
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE ListSBCsJob.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.
	
	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION
	Executes the BUP command LIST_SBCS.

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

// fwd
class BUPMsg;

class ListSBCsJob : public BUPJob {
//foos
public:
	ListSBCsJob(const BUPMsg& msg) : BUPJob(msg) { }
	virtual void execute();
private:
	ListSBCsJob(const ListSBCsJob& rhs);
	ListSBCsJob& operator=(const ListSBCsJob& rhs);
//attr
private:

};


class ListSBCsJob_V4 : public BUPJob {
//foos
public:
    ListSBCsJob_V4(const BUPMsg& msg) : BUPJob(msg) { }
    virtual void execute();
private:
	// Compilers generate these automatically
    ListSBCsJob_V4(const ListSBCsJob_V4& rhs);
    ListSBCsJob_V4& operator=(const ListSBCsJob_V4& rhs);
//attr
private:

};

#endif
