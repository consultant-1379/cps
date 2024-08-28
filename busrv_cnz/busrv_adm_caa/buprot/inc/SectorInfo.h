#ifndef _SECTORINFO_H_
#define _SECTORINFO_H_
/*
NAME
	SECTORINFO -
LIBRARY 3C++
PAGENAME SECTORINFO
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE SectorInfo.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION
	Sector (0-* segments) descriptor.

ERROR HANDLING
	-

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
	2002-05-31 by U/Y/SF Anders Gillgren (qabgill, tag: ag)

LINKAGE
	-

SEE ALSO
	Memory layout must match "File Format Specification"/"Backup Protocol".

*/
#pragma once

#include "DateTime.h"

#include <assert.h>
#include <ostream>

using namespace std;

#pragma pack (push, 1)

class SectorInfo {
//foos
public:
	SectorInfo() : m_nofSegments(0) { }
	SectorInfo(u_int32 nofSegments, const DateTime& dt);
// access
	u_int32 nofSegments() const { return m_nofSegments; }
	const DateTime& outputTime() const { return m_outputTime; }
	DateTime& outputTime() { return m_outputTime; }
// modify	
	void nofSegments(u_int32 val) { m_nofSegments = val; }
	void outputTime(const DateTime& val) { m_outputTime = val; }
// funx
	//friend ostream& operator<<(ostream& os, const SectorInfo& si);
	ostream& print(ostream& os) const {
	    os << dec << "<" << m_nofSegments << "><" << m_outputTime << ">";
	    return os;
	}
	
private:
	u_int32 m_nofSegments;
	DateTime m_outputTime;
};
#pragma pack (pop)
//
// inlines
//===========================================================================
inline SectorInfo::SectorInfo(u_int32 nofSegments, const DateTime& dt)
: m_nofSegments(nofSegments), m_outputTime(dt) {
	assert(m_outputTime.valid() || m_outputTime.null());
}

ostream& operator<<(ostream& os, const SectorInfo&);

#endif
