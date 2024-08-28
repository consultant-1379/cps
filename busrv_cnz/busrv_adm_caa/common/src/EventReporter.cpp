/*
NAME
   File_name:EventReporter.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2001. All rights reserved.

The Copyright to the computer program(s) herein is the property of Ericsson
Utvecklings AB, Sweden.
The program(s) may be used and/or copied only with the written permission from
Ericsson Utvecklings AB or in accordance with the terms and conditions
stipulated in the agreement/contract under which the program(s) have been
supplied.

DESCRIPTION
   Event & alarm reporting class, encapsulates ACS_AEH_EvReport.

DOCUMENT NO
   190 89-CAA 109 0387

AUTHOR 
   2001-11-27 by U/Y/SF Anders Gillgren (qabgill, tag :ag)

SEE ALSO 
   -

Revision history
----------------
2001-09-17 Created
2002-10-31 Added ACS_TRACE :lan

*/

#include "EventReporter.h"

#include "buap.h"
#include "CPS_BUSRV_Trace.h"
#include <sstream>

#include <iostream>
using namespace std;

//
// static inits
//
EventReporter EventReporter::s_instance;
//
//
//===========================================================================
long EventReporter::write(const string& msg, long code /* = ErrorEvent::ERROR_CODE_BASE */,
	ErrorEvent::REF_OBJECT refObject /* = ErrorEvent::SOFTWARE */) {

	newTRACE((LOG_LEVEL_INFO, "EventReporter::write(%s, %d, ErrorEvent::REF_OBJECT refObject", 0, msg.c_str(), code));

	ErrorEvent ee(msg, code, refObject); // create ErrorEvent from params
	return write(ee);
}
//
// AP (ACS_AEH) impl.
//===========================================================================
long EventReporter::write(const ErrorEvent& ee) {
	
	newTRACE((LOG_LEVEL_INFO, "EventReporter::write(const ErrorEvent& ee)", 0));
	
	m_cs.enter();

	ACS_AEH_ReturnType err = m_evRep.sendEventMessage(ee.processName(),
	                                                  ee.specificProblem(),
                                                      ee.severity(),
                                                      ee.probableCause(),
                                                      ee.objectClassOfReference(),
                                                      ee.objectOfReference(),
                                                      ee.problemData(),
                                                      ee.problemText());

	if (err == ACS_AEH_error) {
		TRACE((LOG_LEVEL_ERROR, "acs_aeh_EH_EvReport error, code: %d", 0, m_evRep.getError()));
		TRACE((LOG_LEVEL_ERROR, "ACS error description: %s", 0, m_evRep.getErrorText()));
	}

	m_cs.leave();
		
	return 0L;
}
