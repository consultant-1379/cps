#ifndef _LOADBEGINJOB_H_
#define _LOADBEGINJOB_H_
/*
NAME
	LOADBEGINJOB -
LIBRARY 3C++
PAGENAME LOADBEGINJOB
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE LoadBeginJob.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.
	
	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION
	Executes the BUP command LOAD_BEGIN.

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


class LoadBeginJob : public BUPJob {
//foos
public:
	LoadBeginJob(const BUPMsg& msg) : BUPJob(msg) { }
	virtual ~LoadBeginJob() { }
	virtual void execute();
private:
	LoadBeginJob(const LoadBeginJob& rhs);
	LoadBeginJob& operator=(const LoadBeginJob& rhs);
//attr
private:

};

//
// inlines
//====================================================================


class LoadBeginJob_V4 : public BUPJob {
//foos
public:
    LoadBeginJob_V4(const BUPMsg& msg) : BUPJob(msg) { }
    virtual ~LoadBeginJob_V4() { }
    virtual void execute();
private:
	//void executeBupVer3();
    LoadBeginJob_V4(const LoadBeginJob_V4& rhs);
    LoadBeginJob_V4& operator=(const LoadBeginJob_V4& rhs);
//attr
private:

};


#endif
