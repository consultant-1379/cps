#ifndef _FCWRITEGETPATHJOB_H_
#define _FCWRITEGETPATHJOB_H_
/*
  NAME
  FCWRITEGETPATHJOB -
  LIBRARY 3C++
  PAGENAME FCWRITEGETPATHJOB
  HEADER  CPS
  LEFT_FOOTER Ericsson Utvecklings AB
  INCLUDE FcWriteGetPathJob.h

  COPYRIGHT
  COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

  The Copyright to the computer program(s) herein is the property of
  Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
  copied only with the written permission from Ericsson Utvecklings AB
  or in accordance with the terms and conditions stipulated in the
  agreement/contract under which the program(s) have been supplied.

  DESCRIPTION
  Executes the FCP command FC_WRITE_GET_PATH (configuration).

  ERROR HANDLING
  -

  DOCUMENT NO
  190 89-CAA 109 0670

  AUTHOR
  2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on FcWriteGetPathJob.h of FCSRV in Windows

  LINKAGE
  -

  SEE ALSO
  -
  
  Revision history
  ----------------
  2011-11-15 xtuudoo Created
  2012-09-11 xngudan Updated for Cluster session

*/
#include "FCPJob.h"

class FcWriteGetPathJob : public FCPJob {
//foos
public:
	FcWriteGetPathJob (const FCPMsg& msg) : FCPJob(msg) { }
	virtual ~FcWriteGetPathJob() { }
	virtual void execute();
private:
	FcWriteGetPathJob(const FcWriteGetPathJob& rhs);
	FcWriteGetPathJob& operator=(const FcWriteGetPathJob& rhs);
//attr
private:

};

class FcWriteGetPathJob_V4 : public FCPJob {
//foos
public:
	FcWriteGetPathJob_V4 (const FCPMsg& msg) : FCPJob(msg) { }
	virtual ~FcWriteGetPathJob_V4() { }
	virtual void execute();
private:
	FcWriteGetPathJob_V4(const FcWriteGetPathJob_V4& rhs);
	FcWriteGetPathJob_V4& operator=(const FcWriteGetPathJob_V4& rhs);
//attr
private:

};

//
// inlines
//===========================================================================

#endif

