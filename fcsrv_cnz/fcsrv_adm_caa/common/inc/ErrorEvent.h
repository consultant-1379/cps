#ifndef _ERROREVENT_H_
#define _ERROREVENT_H_
/*
NAME
	ERROREVENT - 
LIBRARY 3C++
PAGENAME ERROREVENT
HEADER  CPS  
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE ErrorEvent.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the 
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION 
   Used to hook up from FCSRV error codes/handling to the ACS_AEH event & alarm 
	functionality.
	Create an instance of this class from an Exception, and you'll have the data
	that ACS_AEH_EvReporter expects.

ERROR HANDLING
   -

DOCUMENT NO
   190 89-CAA 109 1410

AUTHOR 
   2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on ErrorEvent.h of Windows version

LINKAGE
	-

SEE ALSO 
	-
Revision history
----------------

*/
#include "Exception.h"

#include <cassert>
#include <string>

using namespace std;

class ErrorEvent {
// types & stuff
public:
	//
	// ERROR_CODE_BASE + (FCSRV) ERROR_CODE == Specific Problem according to ACS_AEH...
	//----------------------------------------------------------------------
	// start at 22100 for FCSRV, at 22150 for CodeExceptions
	enum { ERROR_CODE_BASE = 22100 }; 
	// range for CPS
	enum ERROR_CODE_RANGE {
		MIN_ERROR_CODE = 22000,
		MAX_ERROR_CODE = 22999
	};
	//
	// if you add something here, remember the string table!
	//----------------------------------------------------------------------
	enum SEVERITY {
		FIRST_SEVERITY = 0,
		A1 = FIRST_SEVERITY,
		A2,
		A3,
		O1,
		O2,
		CEASING,
		EVENT,
		LAST_SEVERITY = EVENT
	};
	enum { NUMBER_OF_SEVERITIES = LAST_SEVERITY + 1 - FIRST_SEVERITY };
	//
	// if you add something here, remember the string table!
	//----------------------------------------------------------------------
	enum REF_OBJECT {
		FIRST_REF_OBJECT = 0,
		UNKNOWN_REF_OBJECT = FIRST_REF_OBJECT,
		CONFIGURATION,
		SOFTWARE,
		LAST_REF_OBJECT = SOFTWARE
	};
	enum { NUMBER_OF_REF_OBJECTS = LAST_REF_OBJECT + 1 - FIRST_REF_OBJECT };
	//
	//
	//
	static const char* SEVERITY_NAME[NUMBER_OF_SEVERITIES];
	static const char* REF_OBJECT_NAME[NUMBER_OF_REF_OBJECTS];
//foos
public:
	ErrorEvent(const Exception& x) : m_msg(x.what()),
		// reduce the error code to 22100 - 22199 range
									 m_code((x.errcode() ? x.errcode() % 100 : x.errcode()) + ERROR_CODE_BASE),
									 m_probableCause(x.name() ? x.name() : ""),
									 m_refObject(SOFTWARE) {
		assert(m_code >= ERROR_CODE_BASE && m_code <= MAX_ERROR_CODE);
	}
	ErrorEvent(const string& msg, long code = 0, REF_OBJECT refObject = SOFTWARE)
		: m_msg(msg),
		  m_code(code + ERROR_CODE_BASE),
		  m_refObject(refObject) {
		assert(m_code >= ERROR_CODE_BASE && m_code <= MAX_ERROR_CODE);
	}
	// access (function names map to "AP Event Reporting" 1/10260-CXA 110 46)
	const char* processName() const;
	long specificProblem() const { return m_code; }
	const char* severity() const { return SEVERITY_NAME[EVENT]; }
	const char* probableCause() const { return m_probableCause.c_str(); }
	const char* objectClassOfReference() const { return OBJECT_CLASS_OF_REFERENCE; }
	const char* objectOfReference() const { return REF_OBJECT_NAME[m_refObject]; }
	const char* problemData() const { return m_msg.c_str(); }
	const char* problemText() const { return PROBLEM_TEXT; }
private:
// global
	friend ostream& operator<<(ostream& , const ErrorEvent& );
//attr
private:
	string m_msg;
	long m_code;
	string m_probableCause;
	REF_OBJECT m_refObject;
	static const char* PROBLEM_TEXT; // = ""
	static char PROCESS_NAME[100]; // + PID
	static char OBJECT_CLASS_OF_REFERENCE[4]; // = "APZ" (what is an object class?) 
};

#endif
