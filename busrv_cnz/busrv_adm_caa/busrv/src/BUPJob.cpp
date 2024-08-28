/*
NAME
   File_name:BUPJob.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

   The Copyright to the computer program(s) herein is the property of Ericsson
   Utvecklings AB, Sweden.
   The program(s) may be used and/or copied only with the written permission from
   Ericsson Utvecklings AB or in accordance with the terms and conditions
   stipulated in the agreement/contract under which the program(s) have been
   supplied.

DESCRIPTION
   Base class and "letter" class for all BUP Command Jobs.

DOCUMENT NO
   190 89-CAA 109 0387

AUTHOR
   2002-02-05 by UAB/KY/SK Anders Gillgren (qabgill, tag :ag)

SEE ALSO
   Base class Job & and all deriving classes.

Revision history
----------------
2002-02-05 qabgill Created
2002-10-20 uabmnst Add ACSTrace
*/


#include "BUPJob.h"

#include "buap.h"
#include "CPS_BUSRV_Trace.h"
#include "CodeException.h"
#include "EventReporter.h"

#include <sstream>

using namespace std;
//
// only for derived classes
//
BUPJob::BUPJob(const BUPMsg& msg) : m_msg(msg) {
	newTRACE((LOG_LEVEL_INFO, "BUPJob::BUPJob(const BUPMsg& msg)", 0));
}
//
// called by derived classes to check for correct version of BUP
//=========================================================================
bool BUPJob::verifyVersion() {

//mnst		TRACE("\n" << m_msg << "\n\n");
	newTRACE((LOG_LEVEL_INFO, "BUPJob::verifyVersion()", 0));
	// verify
	const BUPMsg::ReqHeader& hdr = *m_msg.requestHeader();

	// THN: TODO: the following are really constant
	// Declare them as constant if we have time
	Version currVersion(BUPMsg::VER_MAJOR, BUPMsg::VER_MINOR);
	Version currVersion2(BUPMsg::VER_MAJOR2, BUPMsg::VER_MINOR2);
	if((currVersion != hdr.version()) && (currVersion2 != hdr.version())) {
		ostringstream ss;
		ss << "Unsupported Backup protocol on client: " << hdr.version() << '\0';
		EventReporter::instance().write(ss.str(), ErrorEvent::CONFIGURATION);
		m_exitcode = BUPMsg::UNSUP_BUPROT_VERSION;
		return false;
	}
	return true;
}
//
// write response to response message buffer
// the response must be placed in the m_msg buffer prior to this call
//=========================================================================
void BUPJob::response(BUPMsg& rspMsg) {
	newTRACE((LOG_LEVEL_INFO, "BUPJob::response(BUPMsg& rspMsg)", 0));
	memcpy(rspMsg.addr(), m_msg.addr(), BUPMsg::MSG_SIZE_IN_BYTES);
}
