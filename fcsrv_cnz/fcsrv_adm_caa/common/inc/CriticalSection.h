/*
NAME
	CRITICALSECTION -
LIBRARY 3C++
PAGENAME CRITICALSECTION
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE CriticalSection.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION
   CriticalSection - Linux CRITICAL_SECTION class

	AutoCS - Uses an existing CriticalSection: enters when AutoCS is created,
	and leaves when it is destroyed/goes out of scope

ERROR HANDLING
   -

DOCUMENT NO
   190 89-CAA 109 1410

AUTHOR
   2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on CriticalSection.h Windows version

LINKAGE
	-

SEE ALSO
   -

*/

#ifndef CRITICALSECTION_H_
#define CRITICALSECTION_H_

#include "boost/thread/recursive_mutex.hpp"

class CriticalSection {

public:
	CriticalSection() : m_cs(), m_count(0) { }
	~CriticalSection() {}
	void enter() { m_cs.lock(); ++m_count; }
	void leave() { m_cs.unlock(); --m_count; }
	long int getCounter() const { return m_count; }

private:
	CriticalSection(const CriticalSection& ); // not impl.
	CriticalSection& operator=(const CriticalSection& ); // -:-
	boost::recursive_mutex  m_cs;
	long int m_count;
};


class AutoCS
{
public:
	AutoCS(CriticalSection& cs) : m_cs(cs) { m_cs.enter(); }
	~AutoCS() { m_cs.leave(); }

private:
	AutoCS(const AutoCS& );
	AutoCS& operator=(const AutoCS& );

	CriticalSection& m_cs;
};


#endif /* CRITICALSECTION_H_ */
