#ifndef _WRITEALOGDATAJOB_H_
#define _WRITEALOGDATAJOB_H_
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

//
// inlines
//===========================================================================

class WriteALogDataJob_V4 : public BUPJob {
//foos
public:
	WriteALogDataJob_V4(const BUPMsg& msg) : BUPJob(msg) { }
	virtual ~WriteALogDataJob_V4() { }
	virtual void execute();
private:
	WriteALogDataJob_V4(const WriteALogDataJob_V4& rhs);
	WriteALogDataJob_V4& operator=(const WriteALogDataJob_V4& rhs);
//attr
private:

};
//
// inlines
//===========================================================================

#endif
