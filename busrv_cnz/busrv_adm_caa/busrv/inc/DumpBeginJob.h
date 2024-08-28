#ifndef _DUMPBEGINJOB_H_
#define _DUMPBEGINJOB_H_
/*
NAME
	DUMPBEGINJOB -
LIBRARY 3C++
PAGENAME DUMPBEGINJOB
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE DumpBeginJob.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.
	
	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION
	Executes the BUP command DUMP_BEGIN.

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
class BUPMsg;

class DumpBeginJob : public BUPJob {
//foos
public:
	DumpBeginJob(const BUPMsg& msg) : BUPJob(msg) { }
	virtual ~DumpBeginJob() { }
	virtual void execute();
private:
	DumpBeginJob(const DumpBeginJob& rhs);
	DumpBeginJob& operator=(const DumpBeginJob& rhs);

//attr
private:
};
//
// inlines
//====================================================================


class DumpBeginJob_V4 : public BUPJob {
//foos
public:
	DumpBeginJob_V4(const BUPMsg& msg) : BUPJob(msg) { }
	virtual ~DumpBeginJob_V4() { }
	virtual void execute();
private:
	//void executeBupVer3();
	DumpBeginJob_V4(const DumpBeginJob_V4& rhs);
	DumpBeginJob_V4& operator=(const DumpBeginJob_V4& rhs);

//attr
private:
};


#endif // ifndef _DUMPBEGINJOB_H_

