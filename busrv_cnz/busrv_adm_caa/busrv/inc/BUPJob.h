#ifndef _BUPJOB_H_
#define _BUPJOB_H_
/*
NAME
	BUPJOB -
LIBRARY 3C++
PAGENAME BUPJOB
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE BUPJob.H

COPYRIGHT
   COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.
   
   The Copyright to the computer program(s) herein is the property of
   Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
   copied only with the written permission from Ericsson Utvecklings AB
   or in accordance with the terms and conditions stipulated in the
   agreement/contract under which the program(s) have been supplied.

DESCRIPTION
   Base class for all BUP Command Jobs.

ERROR HANDLING
   -

DOCUMENT NO
   190 89-CAA 109 0387

AUTHOR
   2002-02-05 by UAB/KY/SK Anders Gillgren (qabgill, tag: ag)

LINKAGE
   -

SEE ALSO
   Base class Job & and all deriving classes.

*/

#include "JobBase.h"
#include "BUPMsg.h"

class BUPJob : public JobBase {
//foos
public:
   virtual ~BUPJob() { }
	virtual void response(BUPMsg& );
protected:
	bool verifyVersion();
	BUPJob(const BUPMsg& );
	BUPMsg& msg() { return m_msg; }
private:
   BUPJob(const BUPJob& rhs);
   BUPJob& operator=(const BUPJob& rhs);
//attr
private:
	BUPMsg m_msg;
};
//
// inlines
//====================================================================

#endif
