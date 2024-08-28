
#ifndef _FCPJob_H_
#define _FCPJob_H_
/*
  NAME
  FCPJob -
  LIBRARY 3C++
  PAGENAME FCPJob
  HEADER  CPS
  LEFT_FOOTER Ericsson Utvecklings AB
  INCLUDE FCPJob.h

  COPYRIGHT
  COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.
   
  The Copyright to the computer program(s) herein is the property of
  Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
  copied only with the written permission from Ericsson Utvecklings AB
  or in accordance with the terms and conditions stipulated in the
  agreement/contract under which the program(s) have been supplied.

  DESCRIPTION
  Base class for all FCP Command Jobs.

  ERROR HANDLING
  -

  DOCUMENT NO
  190 89-CAA 109 0670

  AUTHOR
  2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on FCPJob.h of FCSRV in Windows


  LINKAGE
  -

  SEE ALSO
  Base class Job & and all deriving classes.
  
  Revision history
  ----------------

*/

#include "JobBase.h"
#include "FCPMsg.h"

class FCPJob:public JobBase{
//foos
public:
    virtual ~FCPJob() { };
	virtual void response(FCPMsg& );
protected:
	bool verifyVersion();
	FCPJob(const FCPMsg& );
	FCPMsg& msg() { return m_msg; }
private:
    FCPJob(const FCPJob& rhs);
    FCPJob& operator=(const FCPJob& rhs);
//attr
private:
	FCPMsg m_msg;
};
//
// inlines
//====================================================================

#endif
