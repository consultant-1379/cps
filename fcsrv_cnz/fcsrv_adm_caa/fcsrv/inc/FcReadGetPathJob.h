#ifndef _FCREADGETPATHJOB_H_
#define _FCREADGETPATHJOB_H_
/*
  NAME
  FCREADGETPATHJOB -
  LIBRARY 3C++
  PAGENAME FCREADGETPATHJOB
  HEADER  CPS
  LEFT_FOOTER Ericsson Utvecklings AB
  INCLUDE FcReadGetPathJob.h

  COPYRIGHT
  COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

  The Copyright to the computer program(s) herein is the property of
  Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
  copied only with the written permission from Ericsson Utvecklings AB
  or in accordance with the terms and conditions stipulated in the
  agreement/contract under which the program(s) have been supplied.

  DESCRIPTION
  Executes the FCP command FC_READ_GET_PATH (configuration).

  ERROR HANDLING
  -

  DOCUMENT NO
  190 89-CAA 109 0670

  AUTHOR
  2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on FcReadGetPathJob.h of FCSRV in Windows

  LINKAGE
  -

  SEE ALSO
  -
  
  Revision history
  ----------------
  2006-11-28	uabmnst Created from FCSRV's ReadConfJob.h
  2011-11-15    xtuudoo Ported to APG43L
  2012-09-11    xngudan Updated for Cluster session
	

*/
#include "FCPJob.h"

class FcReadGetPathJob : public FCPJob {
//foos
public:
	FcReadGetPathJob(const FCPMsg& msg) : FCPJob(msg) { }
	virtual ~FcReadGetPathJob() { }
	virtual void execute();
private:
	FcReadGetPathJob(const FcReadGetPathJob& rhs);
	FcReadGetPathJob& operator=(const FcReadGetPathJob& rhs);
//attr
private:

};


class FcReadGetPathJob_V4 : public FCPJob {
//foos
public:
	FcReadGetPathJob_V4(const FCPMsg& msg) : FCPJob(msg) { }
	virtual ~FcReadGetPathJob_V4() { }
	virtual void execute();
private:
	FcReadGetPathJob_V4(const FcReadGetPathJob_V4& rhs);
	FcReadGetPathJob_V4& operator=(const FcReadGetPathJob_V4& rhs);
//attr
private:

};

//
// inlines
//===========================================================================

#endif

