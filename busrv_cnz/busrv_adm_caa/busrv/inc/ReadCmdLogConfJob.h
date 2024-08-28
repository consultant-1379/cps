#ifndef _READCMDLOGCONFJOB_H_
#define _READCMDLOGCONFJOB_H_
/*
NAME
	READCMDLOGCONFJOB -
LIBRARY 3C++
PAGENAME READCMDLOGCONFJOB
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE ReadCmdLogConfJob.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION
	Executes the BUP command READ_CMDLOG_CONF.

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

class ReadCmdLogConfJob : public BUPJob {
//foos
public:
	ReadCmdLogConfJob(const BUPMsg& msg) : BUPJob(msg) { }
	virtual ~ReadCmdLogConfJob() { }
	virtual void execute();
private:
	ReadCmdLogConfJob(const ReadCmdLogConfJob& rhs);
	ReadCmdLogConfJob& operator=(const ReadCmdLogConfJob& rhs);
//attr
private:

};

#if 0
//
// inlines
//===========================================================================

class ReadCmdLogConfJob_V4 : public BUPJob {
//foos
public:
	ReadCmdLogConfJob_V4(const BUPMsg& msg) : BUPJob(msg) { }
	virtual ~ReadCmdLogConfJob_V4() { }
	virtual void execute();
private:
	ReadCmdLogConfJob_V4(const ReadCmdLogConfJob_V4& rhs);
	ReadCmdLogConfJob_V4& operator=(const ReadCmdLogConfJob_V4& rhs);
//attr
private:

};
//
// inlines
//===========================================================================

class ReadCmdLogConfJob_LEV4 : public BUPJob {
//foos
public:
	ReadCmdLogConfJob_LEV4(const BUPMsg& msg) : BUPJob(msg) { }
	virtual ~ReadCmdLogConfJob_LEV4() { }
	virtual void execute();
private:
	ReadCmdLogConfJob_LEV4(const ReadCmdLogConfJob_LEV4& rhs);
	ReadCmdLogConfJob_LEV4& operator=(const ReadCmdLogConfJob_LEV4& rhs);
//attr
private:

};
//
// inlines
//===========================================================================
#endif

#endif
