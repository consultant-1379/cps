/*
NAME
   File_name:EventReporter.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

The Copyright to the computer program(s) herein is the property of Ericsson
Utvecklings AB, Sweden.
The program(s) may be used and/or copied only with the written permission from
Ericsson Utvecklings AB or in accordance with the terms and conditions
stipulated in the agreement/contract under which the program(s) have been
supplied.

DESCRIPTION
   Event & alarm reporting class, encapsulates ACS_AEH_EvReport.

DOCUMENT NO
	190 89-CAA 109 1410

AUTHOR 
   2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on EventReporter.cpp of FCSRV in Windows

SEE ALSO 
   -

Revision history
----------------

*/

#include "EventReporter.h"
#include "PlatformTypes.h"
#include "fcap.h"
#include "CPS_FCSRV_Trace.h"
#include <sstream>

using namespace std;

#ifdef _DEBUG
#include <iostream> // for tracing
#endif
//
// static inits
//
EventReporter EventReporter::s_instance;
//
//
//===========================================================================
long EventReporter::write(const string& msg, long code /* = ErrorEvent::ERROR_CODE_BASE */,
	ErrorEvent::REF_OBJECT refObject /* = ErrorEvent::SOFTWARE */) {

	newTRACE((LOG_LEVEL_ERROR, "EventReporter::write(%s, %d, ErrorEvent::REF_OBJECT refObject", 0, msg.c_str(), code));

	ErrorEvent ee(msg, code, refObject); // create ErrorEvent from params
	return write(ee);
}
//
// AP (ACS_AEH) impl.
//===========================================================================
long EventReporter::write(const ErrorEvent& ee) {
	
	newTRACE((LOG_LEVEL_ERROR, "EventReporter::write(const ErrorEvent& ee)", 0));
	
	m_cs.enter();

	int32_t err = m_evRep.sendEventMessage(ee.processName(),
										   ee.specificProblem(),
										   ee.severity(),
										   ee.probableCause(),
										   ee.objectClassOfReference(),
										   ee.objectOfReference(),
										   ee.problemData(),
										   ee.problemText());

	if(err == ACS_AEH_error) {
		int32_t errcode = m_evRep.getError();
		TRACE((LOG_LEVEL_ERROR, "ACS_AEH_EvReport error, code: %d", 0, errcode));
		TRACE((LOG_LEVEL_ERROR, "ACS error description: %s", 0, m_evRep.getErrorText()));
	}

	m_cs.leave();
		
	return 0L;
}
