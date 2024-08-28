#ifndef _FCSIZEJOB_H_
#define _FCSIZEJOB_H_
/*
  NAME
  FCSIZWJOB -
  LIBRARY 3C++
  PAGENAME FCSIZWJOB
  HEADER  CPS
  LEFT_FOOTER Ericsson Utvecklings AB
  INCLUDE FcSizeJob.h

  COPYRIGHT
  COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

  The Copyright to the computer program(s) herein is the property of
  Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
  copied only with the written permission from Ericsson Utvecklings AB
  or in accordance with the terms and conditions stipulated in the
  agreement/contract under which the program(s) have been supplied.

  DESCRIPTION
  Executes the FCP command FC_SIZE (configuration).

  ERROR HANDLING
  -

  DOCUMENT NO
  190 89-CAA 109 0670

  AUTHOR
  2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on FcSizeJob.h of FCSRV in Windows

  LINKAGE
  -

  SEE ALSO
  -
  
  Revision history
  ----------------
  2006-11-28	uabmnst Created from FCSRV's ReadConfJob.h
  2011-11-15 	xtuudoo Ported to APG43L
  2012-09-11 	xngudan Updated for Cluster session
	

*/
#include "FCPJob.h"

class FcSizeJob : public FCPJob {
//foos
public:
	FcSizeJob(const FCPMsg& msg) : FCPJob(msg) { }
	virtual ~FcSizeJob() { }
	virtual void execute();
private:
	FcSizeJob(const FcSizeJob& rhs);
	FcSizeJob& operator=(const FcSizeJob& rhs);
//attr
private:

};

#include "FCPJob.h"

class FcSizeJob_V4 : public FCPJob {
//foos
public:
	FcSizeJob_V4(const FCPMsg& msg) : FCPJob(msg) { }
	virtual ~FcSizeJob_V4() { }
	virtual void execute();
private:
	FcSizeJob_V4(const FcSizeJob_V4& rhs);
	FcSizeJob_V4& operator=(const FcSizeJob_V4& rhs);
//attr
private:

};

//
// inlines
//===========================================================================

#endif

