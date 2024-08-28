/*
NAME
   File_name:FCPJob.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

   The Copyright to the computer program(s) herein is the property of Ericsson
   Utvecklings AB, Sweden.
   The program(s) may be used and/or copied only with the written permission from
   Ericsson Utvecklings AB or in accordance with the terms and conditions
   stipulated in the agreement/contract under which the program(s) have been
   supplied.

DESCRIPTION
   Base class and "letter" class for all FCP Command Jobs.

DOCUMENT NO
   190 89-CAA 109 1410

AUTHOR
   2011-11-27 by XDT/DEK Tu, Do Hoang (xtuudoo) based on FCPJob.cpp of FCSRV in Windows

SEE ALSO
   Base class Job & and all deriving classes.

Revision history
----------------
2002-02-05 qabgill Created
2002-10-20 uabmnst Add ACSTrace
*/


#include "FCPJob.h"

#include "fcap.h"
#include "CPS_FCSRV_Trace.h"
#include "CodeException.h"
#include "EventReporter.h"
#include "LinuxException.h"
#include <sstream>

using namespace std;
//
// only for derived classes
//
FCPJob::FCPJob(const FCPMsg& msg) : m_msg(msg) {
	newTRACE((LOG_LEVEL_INFO, "FCPJob::FCPJob(const FCPMsg& msg)", 0));
}
//
// called by derived classes to check for correct version of FCP
//=========================================================================
bool FCPJob::verifyVersion() {

//mnst		TRACE("\n" << m_msg << "\n\n");
	newTRACE((LOG_LEVEL_INFO, "FCPJob::verifyVersion()", 0));
	// verify
	const FCPMsg::ReqHeader& hdr = *m_msg.requestHeader();

	// THN: TODO: the following are really constant
	// Declare them as constant if we have time
	Version currVersion(FCPMsg::VERSION_MAJOR, FCPMsg::VERSION_MINOR);
	if(currVersion != hdr.version()) {
		ostringstream ss;
		ss << "Unsupported Backup protocol on client: " << hdr.version() << '\0';
		EventReporter::instance().write(ss.str(), ErrorEvent::CONFIGURATION);
		m_exitcode = FCPMsg::UNSUP_FCPROT_VERSION;
		return false;
	}
	return true;
}
//
// write response to response message buffer
// the response must be placed in the m_msg buffer prior to this call
//=========================================================================
void FCPJob::response(FCPMsg& rspMsg) {
	newTRACE((LOG_LEVEL_INFO, "FCPJob::response(FCPMsg& rspMsg)", 0));
	memcpy(rspMsg.addr(), m_msg.addr(), FCPMsg::MSG_SIZE_IN_BYTES);
}
