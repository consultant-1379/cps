#ifndef _READCMDLOGDATAJOB_H_
#define _READCMDLOGDATAJOB_H_
/*
NAME
	READCMDLOGDATAJOB -
LIBRARY 3C++
PAGENAME READCMDLOGDATAJOB
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE ReadCmdLogDataJob.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION
	Executes the BUP command READ_CMDLOG_DATA.

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

#ifdef _LEGACY_BUP3_SUPPORTED
class ReadCmdLogDataJob : public BUPJob {
//foos
public:
	ReadCmdLogDataJob(const BUPMsg& msg) : BUPJob(msg) { }
	virtual ~ReadCmdLogDataJob() { }
	virtual void execute();
private:
	ReadCmdLogDataJob(const ReadCmdLogDataJob& rhs);
	ReadCmdLogDataJob& operator=(const ReadCmdLogDataJob& rhs);
//attr
private:

};
//
// inlines
//===========================================================================
#endif

#if 0
class ReadCmdLogDataJob_V4 : public BUPJob {
//foos
public:
	ReadCmdLogDataJob_V4 (const BUPMsg& msg) : BUPJob(msg) { }
	virtual ~ReadCmdLogDataJob_V4 () { }
	virtual void execute();
private:
	ReadCmdLogDataJob_V4 (const ReadCmdLogDataJob_V4& rhs);
	ReadCmdLogDataJob_V4& operator=(const ReadCmdLogDataJob_V4& rhs);
//attr
private:

};
//
// inlines
//===========================================================================
class ReadCmdLogDataJob_LEV4 : public BUPJob {
//foos
public:
	ReadCmdLogDataJob_LEV4 (const BUPMsg& msg) : BUPJob(msg) { }
	virtual ~ReadCmdLogDataJob_LEV4 () { }
	virtual void execute();
private:
	ReadCmdLogDataJob_LEV4 (const ReadCmdLogDataJob_LEV4& rhs);
	ReadCmdLogDataJob_LEV4& operator=(const ReadCmdLogDataJob_LEV4& rhs);
//attr
private:

};
//
// inlines
//===========================================================================
#endif
#endif
