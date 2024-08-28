#ifndef _SBC1TOSFRJOB_H_
#define _SBC1TOSFRJOB_H_
/*
NAME
	SBC1TOSFRJOB -
LIBRARY 3C++
PAGENAME SBC1TOSFRJOB
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE SBC1ToSFRJob.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.
	
	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION
	Executes the BUP command SBC1_TO_SFR.

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

class SBC1ToSFRJob : public BUPJob {
//foos
public:
	SBC1ToSFRJob(const BUPMsg& msg) : BUPJob(msg) { }
	virtual ~SBC1ToSFRJob() { }
	virtual void execute();
private:
	SBC1ToSFRJob(const SBC1ToSFRJob& rhs);
	SBC1ToSFRJob& operator=(const SBC1ToSFRJob& rhs);
	void copy(bool overwrite);
//attr
private:

};
//
// inlines
//====================================================================

#endif
