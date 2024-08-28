#ifndef _WRITECMDLOGDATAJOB_H_
#define _WRITECMDLOGDATAJOB_H_
/*
NAME
	WRITECMDLOGDATAJOB -
LIBRARY 3C++
PAGENAME WRITECMDLOGDATAJOB
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE WriteCmdLogDataJob.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION
	Executes the BUP command WRITE_CMDLOG_DATA.

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

class WriteCmdLogDataJob : public BUPJob {
//foos
public:
	WriteCmdLogDataJob(const BUPMsg& msg) : BUPJob(msg) { }
	virtual ~WriteCmdLogDataJob() { }
	virtual void execute();
private:
	WriteCmdLogDataJob(const WriteCmdLogDataJob& rhs);
	WriteCmdLogDataJob& operator=(const WriteCmdLogDataJob& rhs);
//attr
private:

};
//
// inlines
//===========================================================================

class WriteCmdLogDataJob_V4 : public BUPJob {
//foos
public:
	WriteCmdLogDataJob_V4(const BUPMsg& msg) : BUPJob(msg) { }
	virtual ~WriteCmdLogDataJob_V4() { }
	virtual void execute();
private:
	WriteCmdLogDataJob_V4(const WriteCmdLogDataJob_V4& rhs);
	WriteCmdLogDataJob_V4& operator=(const WriteCmdLogDataJob_V4& rhs);

private:

};
#endif

