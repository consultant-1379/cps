#ifndef _DUMPENDJOB_H_
#define _DUMPENDJOB_H_
/*
NAME
	DUMPENDJOB -
LIBRARY 3C++
PAGENAME DUMPENDJOB
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE DumpEndJob.H

COPYRIGHT
   COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.
   
   The Copyright to the computer program(s) herein is the property of
   Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
   copied only with the written permission from Ericsson Utvecklings AB
   or in accordance with the terms and conditions stipulated in the
   agreement/contract under which the program(s) have been supplied.

DESCRIPTION
   Executes the BUP command DUMP_END.

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
#pragma once

#include "BUPJob.h"
#include "Config.h"

#include <string>

using namespace std;

class BUPMsg;

class DumpEndJob : public BUPJob {
//foos
public:
   DumpEndJob(const BUPMsg& msg) : BUPJob(msg) { }
	virtual ~DumpEndJob() { }
	virtual void execute();

protected:
    void createMarker(const SBCId& sbcId, const char* fn, CPID cpId=0xFFFF);
    
private:
    DumpEndJob(const DumpEndJob& rhs);
    DumpEndJob& operator=(const DumpEndJob& rhs);
    
    void moveFile(SBCId sbcId, const string& srcfile,
        const string& destfile, bool fmsCall, CPID cpId);

};
//
// inlines
//====================================================================

class DumpEndMoveFile {
public:
	DumpEndMoveFile() {}
	~DumpEndMoveFile() {}
	void operator()(const string& src, const string& dest);
};

class DumpEndJob_V4 : public BUPJob {

public:
    DumpEndJob_V4(const BUPMsg& msg) : BUPJob(msg), m_sbcId(128) { }
    virtual ~DumpEndJob_V4() { }
    virtual void execute();
	virtual void executeSpecial(u_int16 sbc);	// Wrap around execute() for BUP interactionn
	virtual void executeNonOp(u_int16 sbc);		// Just returns DumpEnd_Rsp message

protected:
    void createMarker(const SBCId& sbcId, const char* fn, const Config::CLUSTER&);
    
private:
    DumpEndJob_V4(const DumpEndJob_V4& rhs);
    DumpEndJob_V4& operator=(const DumpEndJob_V4& rhs);
    
    void moveFile(SBCId sbcId, const string& srcfile,
		const string& destfile, bool fmsCall, const Config::CLUSTER&);

	u_int16	m_sbcId;
    
};

#endif // ifndef _DUMPENDJOB_H_
