#ifndef _READCONFJOB_H_
#define _READCONFJOB_H_
/*
NAME
	READCONFJOB -
LIBRARY 3C++
PAGENAME READCONFJOB
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE ReadConfJob.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION
	Executes the BUP command READ_CONF (configuration).

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

class ReadConfJob : public BUPJob {
//foos
public:
	ReadConfJob(const BUPMsg& msg) : BUPJob(msg) { }
	virtual ~ReadConfJob() { }
	virtual void execute();
private:
	ReadConfJob(const ReadConfJob& rhs);
	ReadConfJob& operator=(const ReadConfJob& rhs);
//attr
private:
};

//
// inlines
//===========================================================================

class ReadConfJob_V4 : public BUPJob {
//foos
public:
	ReadConfJob_V4(const BUPMsg& msg) : BUPJob(msg) { }
	virtual ~ReadConfJob_V4() { }
	virtual void execute();
private:
	ReadConfJob_V4(const ReadConfJob_V4& rhs);
	ReadConfJob_V4& operator=(const ReadConfJob_V4& rhs);
//attr
private:

};

#endif
