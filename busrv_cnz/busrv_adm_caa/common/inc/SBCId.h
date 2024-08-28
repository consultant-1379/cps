#ifndef _SBCID_H_
#define _SBCID_H_
/*
NAME
	SBCID -
LIBRARY 3C++
PAGENAME SBCID
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE SBCId.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION
	SBC or Backup identity.

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

*/
#pragma once

#include "PlatformTypes.h"

#include <assert.h>
#include <ostream>
#include <string>

using namespace std;

class SBCId {
// types
public:
	enum {
		FIRST_ID = 0,
		LAST_ID = 127,
		FFR_FIRST_ID = FIRST_ID,
		FFR_LAST_CLUSTER_ID = 29,  // For SSI 
		FFR_LAST_ID = 99,
		SFR_FIRST_ID = 100,
		SFR_LAST_ID = LAST_ID,
		BLADE_RANGE_FIRST = 30,
		BLADE_RANGE_LAST = 99,
		MAX_NOF_SBCS = 128
	};
	enum SBC_RANGE { FIRST, SECOND, COMPLETE };

private:
	static const char* DIR_BASE_NAME;
	static const size_t DIR_BASE_NAME_LEN;
//foos
public:
	SBCId(u_int16 id = 0) : m_id(id), m_filler(0) { }
	
	SBCId(char c) : m_id(c), m_filler(0)
	{
		// The exact meaning of temporary file
		if (c == 'X' || c == 'Y')
			m_id = MAX_NOF_SBCS + c;	
	}
	
	// access
	u_int16 id() const { return m_id; }
	// modify
	void id(u_int16 val) { m_id = val; }
	void id(const string&);
	//
	string dirName() const; // simple name, as in RELFSW100 or RELFSWX
	bool firstId(SBC_RANGE range = COMPLETE) const;
	bool valid() const { return m_id <= LAST_ID; }
	bool isTmp() const { return m_id > LAST_ID; }
	operator u_int16() const { return m_id; } // cast operator
	friend ostream& operator<<(ostream& , SBCId );
	friend istream& operator>>(istream& , SBCId& );

    bool operator!=(SBCId d) const { return m_id != d.m_id; }
    bool operator==(SBCId d) const { return m_id == d.m_id; }
    friend bool operator<(SBCId x, SBCId y); 

//attr
private:
	u_int16 m_id;
	u_int16 m_filler;
};
//
// inlines
//===========================================================================
inline bool SBCId::firstId(SBC_RANGE range /* = COMPLETE */) const {
	return ((range == COMPLETE || range == FIRST) && m_id == FFR_FIRST_ID) ||
		((range == COMPLETE || range == SECOND) && m_id == SFR_FIRST_ID);
}

inline bool operator<(SBCId x, SBCId y) 
{
    return x.m_id < y.m_id;
}

#endif
