#ifndef _EVENTREPORTER_H_
#define _EVENTREPORTER_H_
/*
NAME
	EVENTREPORTER - 
LIBRARY 3C++
PAGENAME EVENTREPORTER
HEADER  CPS  
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE EventReporter.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2001. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the 
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION 
   Singleton event & alarm reporting class, encapsulates ACS_AEH_EvReport.

.ERROR HANDLING
   -

DOCUMENT NO
   190 89-CAA 109 0387

AUTHOR 
   2001-11-27 by U/Y/SF Anders Gillgren (qabgill, tag: ag)

.LINKAGE
	-

.SEE ALSO 
   -

*/

#include "acs_aeh_evreport.h"
#include "CriticalSection.h"
#include "ErrorEvent.h"

// fwd
class Exception;
//
#define LOG_GENERAL_EVENT(msg) { stringstream ss; ss << msg; \
	EventReporter::instance().write(ss.str()); \
}
//
// 
//===========================================================================
class EventReporter {
public:
	static EventReporter& instance() { return s_instance; }
// funx	
	void write(const Exception& );
	long write(const ErrorEvent& );
	long write(const string& msg, long code = ErrorEvent::ERROR_CODE_BASE,
		ErrorEvent::REF_OBJECT refObject = ErrorEvent::SOFTWARE);
private:
	EventReporter() { }
	EventReporter(const EventReporter&);
	EventReporter& operator=(const EventReporter&);
// attr
private:
	static EventReporter s_instance;
	CriticalSection m_cs;
	acs_aeh_evreport m_evRep;
};
//
// inlines
//===========================================================================
//
//
//===========================================================================
inline void EventReporter::write(const Exception& x) {
	ErrorEvent ee(x); // convert from Exception to ErrorEvent
	write(ee); // call to derived class
}

#endif
