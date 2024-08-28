#ifndef _SBCFILE_H_
#define _SBCFILE_H_
/*
NAME
	SBCFILE -
LIBRARY 3C++
PAGENAME SBCFILE
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE SBCFile.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION
	Handles file identities and names.

ERROR HANDLING
	-

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
	2002-05-31 by U/Y/SF Anders Gillgren (qabgill, tag: ag)

LINKAGE
	-

SEE ALSO
	-

Revision history
----------------
2002-05-31 qabgill Created
2010-05-23 xdtthng Modified for SSI
*/
#include <ostream>

using namespace std;
//
//
//===========================================================================
class SBCFile {
// types & class data
public:
	enum ID {
		FIRST_ID = 0,
		BACKUP_INFO = FIRST_ID,
		SDD,
		LDD1,
		LDD2,
		PS,
		RS,
		PS2,
		RS2,
		LAST_ID = RS2,
		INVALID_ID
	};
	enum {
		NUMBER_OF_IDS = LAST_ID + 1 - FIRST_ID,
	};
	static const char* FILENAME[NUMBER_OF_IDS];
//foos
public:
	SBCFile(ID id = INVALID_ID) : m_id(id) { }
	ID id() const { return m_id; }
	void id(ID val) { m_id = val; }
	// funx
	bool valid() const { return m_id >= FIRST_ID && m_id <= LAST_ID; }
	static const char* name(SBCFile );
	const char* name() const { return name(*this); }
	bool operator==(SBCFile& rhs) const { return m_id == rhs.m_id; }
	operator ID() const { return m_id; } // cast-operator
	friend ostream& operator<<(ostream& , SBCFile );
//attr
private:
	ID m_id;
};
//
// inlines
//===========================================================================

#endif
