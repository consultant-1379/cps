/*
NAME
   File_name:ErrorEvent.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2001. All rights reserved.

The Copyright to the computer program(s) herein is the property of Ericsson
Utvecklings AB, Sweden.
The program(s) may be used and/or copied only with the written permission from
Ericsson Utvecklings AB or in accordance with the terms and conditions
stipulated in the agreement/contract under which the program(s) have been
supplied.

.DESCRIPTION
   Used to hook up from BUSRV error codes/handling to the ACS_AEH event & alarm 
	functionality.
	Create an instance of this class from an Exception, and you'll have the data
	that ACS_AEH_EvReporter expects.

DOCUMENT NO
   190 89-CAA 109 0387

AUTHOR 
   2001-09-18 by U/Y/SF Anders Gillgren (qabgill, tag :ag)

SEE ALSO 
   EventReporter


Revision history
----------------
2001-09-18 Created

*/


#include "ErrorEvent.h"

#include "SystemInfo.h"
#include "buap.h"
#include <cstdio>

//
// static inits
//----------------------------------------------------------------------
const char* ErrorEvent::SEVERITY_NAME[ErrorEvent::NUMBER_OF_SEVERITIES] = {
	"A1",
	"A2",
	"A3",
	"O1",
	"O2",
	"CEASING",
	"EVENT"
};
//
//
//
const char* ErrorEvent::REF_OBJECT_NAME[ErrorEvent::NUMBER_OF_REF_OBJECTS] = {
	"Unknown/1",
	"Configuration/1",
	"Software/1",
};
const char* ErrorEvent::PROBLEM_TEXT = "";
char ErrorEvent::PROCESS_NAME[] = "";
char ErrorEvent::OBJECT_CLASS_OF_REFERENCE[4] = "APZ";
//
// each ERROR_CODE has a SEVERITY rating (all EVENT now, but could be updated)
//
//
// global functions
//----------------------------------------------------------------------
ostream& operator<<(ostream& os, const ErrorEvent& ee) {
	os << "Event: " << ee.processName() <<
		"\n\tSpecific event: " << ee.specificProblem() <<
		"\n\tSeverity: " << ee.severity() <<
		"\n\tProbable cause: " << ee.probableCause() <<
		"\n\tObject class of reference: " << ee.objectClassOfReference() <<
		"\n\tObject of reference: " << ee.objectOfReference() <<
		"\n\tProblem data: " << ee.problemData() <<
		"\n\tProblem text: " << ee.problemText() << endl;
	return os;
}
//
// member functions
//----------------------------------------------------------------------
//
// Get process name and PID as string
//----------------------------------------------------------------------
const char* ErrorEvent::processName() const {
	if(!strlen(PROCESS_NAME)) {
		// we only need to do this once
		sprintf(PROCESS_NAME, "%s:%d", SystemInfo::getCurrentProcessName().c_str(), SystemInfo::getCurrentProcessID());
	}
	return PROCESS_NAME;
}
