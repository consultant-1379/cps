/*
  NAME
  File_name:FCPJobFactory.cpp

  COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

  The Copyright to the computer program(s) herein is the property of Ericsson
  Utvecklings AB, Sweden.
  The program(s) may be used and/or copied only with the written permission from
  Ericsson Utvecklings AB or in accordance with the terms and conditions
  stipulated in the agreement/contract under which the program(s) have been
  supplied.

  DESCRIPTION
  Creates Job that corresponds to an incoming FCPMsg (FCP command).

  DOCUMENT NO
  190 89-CAA 109 0670

  AUTHOR
  2011-11-15 by XDT/DEK xtuudoo based on FCPJobFactory.cpp of FCSRV in Windows

  SEE ALSO
  -

  Revision history
  ----------------
  2006-11-28	uablan Created from BUSRV's BUPJobFactory.cpp


*/



#include "fcap.h"
#include "FCPJobFactory.h"

#include "UnknownJob.h"

#include "CodeException.h"
#include "CPS_FCSRV_Trace.h"
// include all jobs
#include "FcWriteGetPathJob.h"
#include "FcReadGetPathJob.h"
#include "FcRemoveJob.h"
#include "FcRemoveAllJob.h"
#include "FcSizeJob.h"
#include <iostream>
//
// ctor
//====================================================================
FCPJobFactory::FCPJobFactory(const FCPMsg& msg) {
	newTRACE((LOG_LEVEL_INFO, "FCPJobFactory::FCPJobFactory(%d)", 0, (int)msg.msgHeader()->primitive()));

	switch((int)msg.msgHeader()->primitive()) {
	case FCPMsg::FC_WRITE_GET_PATH:
		TRACE((LOG_LEVEL_INFO, "FC_WRITE_GET_PATH received",0));
		m_job = new FcWriteGetPathJob(msg);
		break;
	case FCPMsg::FC_READ_GET_PATH:
		TRACE((LOG_LEVEL_INFO, "FC_READ_GET_PATH received",0));
		m_job = new FcReadGetPathJob(msg);
		break;
	case FCPMsg::FC_REMOVE:
		TRACE((LOG_LEVEL_INFO, "FC_REMOVE received", 0));
		m_job = new FcRemoveJob(msg);
		break;
	case FCPMsg::FC_REMOVE_ALL:
		TRACE((LOG_LEVEL_INFO, "FC_REMOVE_ALL received", 0));
		m_job = new FcRemoveAllJob(msg);
		break;
	case FCPMsg::FC_SIZE:
		TRACE((LOG_LEVEL_INFO, "FC_SIZE received", 0));
		m_job = new FcSizeJob(msg);
		break;
	default:
		TRACE((LOG_LEVEL_WARN, "Unknown FCP primitive: %d ", 0, msg.msgHeader()->primitive()));
		m_job = new UnknownJob(msg);
		break;
	}

	TRACE((LOG_LEVEL_INFO, "FCPJobFactory::FCPJobFactory() returns", 0));
}

