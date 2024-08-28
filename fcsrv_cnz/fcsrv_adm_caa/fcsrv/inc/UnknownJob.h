#ifndef _UNKNOWNJOB_H_
#define _UNKNOWNJOB_H_
/*
  NAME
  UNKNOWNJOB -
  LIBRARY 3C++
  PAGENAME UNKNOWNJOB
  HEADER  CPS
  LEFT_FOOTER Ericsson Utvecklings AB
  INCLUDE UnknownJob.h

  COPYRIGHT
  COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

  The Copyright to the computer program(s) herein is the property of
  Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
  copied only with the written permission from Ericsson Utvecklings AB
  or in accordance with the terms and conditions stipulated in the
  agreement/contract under which the program(s) have been supplied.

  DESCRIPTION
  Handles unknown primitives received in FCP.

  ERROR HANDLING
  -

  DOCUMENT NO
  190 89-CAA 109 0670

  AUTHOR
  2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on UnknownJob.h of FCSRV in Windows

  LINKAGE
  -

  SEE ALSO
  -
  
  Revision history
  ----------------

*/
#include "FCPJob.h"

class UnknownJob : public FCPJob {
//foos
public:
	UnknownJob(const FCPMsg& msg) : FCPJob(msg) { }
	virtual void execute();
private:
	UnknownJob(const UnknownJob& rhs);
	UnknownJob& operator=(const UnknownJob& rhs);
//attr
private:

};
//
// inlines
//===========================================================================

#endif
