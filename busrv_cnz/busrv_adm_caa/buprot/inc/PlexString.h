#ifndef _PLEXSTRING_H_
#define _PLEXSTRING_H_
/*
NAME
	PLEXSTRING -
LIBRARY 3C++
PAGENAME PLEXSTRING
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE PLEXSTRING.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION
	Id used in APZ.

ERROR HANDLING
	-

DOCUMENT NO
	190 89-CAA 109 

AUTHOR
	2010-06-30 XDT/DEK XDTTHNG

LINKAGE
	-

SEE ALSO
	Memory layout must match "File Format Specification"/Backup Protocol".

*/
#pragma once

#include "PlatformTypes.h"
#include <string>
#include <iostream>
#include <iomanip>

using namespace std;

#pragma pack(push, 1)
//
// Plex String in AP.
//==========================================================================
template <u_int8 MAX_LEN>
class PlexString {
public:
	enum { MAX_LENGTH = MAX_LEN };

	PlexString() { set(0, 0); }
	PlexString(u_int8 nofCharacters, const u_int8* character) { set(nofCharacters, character); }
	u_int8 nofCharacters() const { return m_nofCharacters; }
	const u_int8* character() const { return m_character; }
	void set(u_int8 nofCharacters, const u_int8* character);

	string toStr() const { return string(reinterpret_cast<const char*>(m_character), m_nofCharacters);}
	ostream& print(ostream& ) const;
private:
	u_int8 m_nofCharacters;
	u_int8 m_character[MAX_LENGTH]; // may not be 0-terminated
};

template <u_int8 MAX_LEN>
void PlexString<MAX_LEN>::set(u_int8 nofCharacters, const u_int8* character) {

	assert(nofCharacters <= MAX_LENGTH);
	m_nofCharacters = nofCharacters <= MAX_LENGTH ? nofCharacters : 0;
	memset(m_character, 0, MAX_LENGTH);
	memcpy(m_character, character, m_nofCharacters);
}

template<u_int8 MAX_LEN>
ostream& PlexString<MAX_LEN>::print(ostream& os) const {

	os << std::dec << "<" << static_cast<short>(m_nofCharacters) << "><";
	for(int i = 0; i < MAX_LENGTH; ++i)
		os << (m_character[i] == 0 ? ' ' : static_cast<char>(m_character[i]));
	os << ">";
	return os;
}

#pragma pack(pop)

//
// This template is intended to generate the following
// typedef PlexString<23>   ExchangeId;
// typedef PlexString<35>   MiddleWare;
// typedef PlexString<255>  ALogData;
//

#endif
