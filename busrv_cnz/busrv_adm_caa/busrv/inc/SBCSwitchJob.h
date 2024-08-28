#ifndef _SBCSWITCHJOB_H_
#define _SBCSWITCHJOB_H_
/*
NAME
	SBCSWITCHJOB -
LIBRARY 3C++
PAGENAME SBCSWITCHJOB
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE SBCSwitchJob.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.
	
	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION
	Executes the BUP command SBC_SWITCH.

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
#include "Config.h"
#include "BUPJob.h"
#include "SBCId.h"

#if HAVE_ACS_CS
#include "ACS_CS_API.h"
#endif

// fwd
class SBCList;

class SBCSwitchJob : public BUPJob {
//foos
public:
	SBCSwitchJob(const BUPMsg& msg) : BUPJob(msg) { }
	virtual ~SBCSwitchJob() { }
	virtual void execute();
private:
	SBCSwitchJob(const SBCSwitchJob& rhs);
	SBCSwitchJob& operator=(const SBCSwitchJob& rhs);
	void switchSBCs(SBCId::SBC_RANGE range, BUPMsg::SWITCH_OPERATION operation, CPID cpId);
	void switchHighLow(SBCId tmpId, const SBCList& , CPID cpId);
	void switchRolldown(SBCId tmpId, const SBCList& sbc, CPID cpId);
//attr
private:

};
//
// inlines
//====================================================================

class SBCSwitchJob_V4 : public BUPJob {
//foos
public:
    SBCSwitchJob_V4(const BUPMsg& msg) : BUPJob(msg) { }
    virtual ~SBCSwitchJob_V4() { }
    virtual void execute();
private:
    SBCSwitchJob_V4(const SBCSwitchJob_V4& rhs);
    SBCSwitchJob_V4& operator=(const SBCSwitchJob_V4& rhs);
    void switchSBCs(SBCId::SBC_RANGE range, BUPMsg::SWITCH_OPERATION operation, CPID cpId);
    void switchHighLow(SBCId tmpId, const SBCList&, CPID cpId);
    void switchRolldown(SBCId tmpId, const SBCList& sbc);
//attr
private:

};

#endif

