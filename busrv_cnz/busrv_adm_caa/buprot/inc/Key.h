#ifndef _KEY_H_
#define _KEY_H_
/*
NAME
	KEY -
LIBRARY 3C++
PAGENAME KEY
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE Key.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION
	Unique identifier based on a GUID/UUID.

ERROR HANDLING
	-

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
	2002-05-31 by U/Y/SF Anders Gillgren (qabgill, tag: ag)

LINKAGE
	-

SEE ALSO
	Memory layout must match "File Format Specification".

*/
#pragma once

#include <ostream>

using namespace std;

#pragma pack (push, 1)

class Key {
// types
	enum {
		// this is the original. Change it at your own risk
		LENGTH = 36,       // keylen == length of session identifier
		FILLER = 3,        // KEY_LEN + 1 + 3 == 40 (align on 4-bytes)
		
		//LENGTH = 28,
		//FILLER = 11,        // KEY_LEN + 1 + 11 == 40 (align on 4-bytes)
	};
//foos
public:
	Key(bool init = true);
	const char* c_str() const { return m_str; }
	void set(const char* );
	void newKey();
	bool operator==(const Key& rhs) const;
	bool valid() const;
	friend ostream& operator<<(ostream& os, const Key& key);
//attr
private:
	char m_str[LENGTH + 1 + FILLER];
};
/* Fake Key
//
// inlines
//===========================================================================

class Key_V4 {
// types
	enum {
		// this is the original. Change it at your own risk
		//LENGTH = 36,       // keylen == length of session identifier
		//FILLER = 3,        // KEY_LEN + 1 + 3 == 40 (align on 4-bytes)
		
		LENGTH = 28,
		FILLER = 11,        // KEY_LEN + 1 + 11 == 40 (align on 4-bytes)
	};
//foos
public:
	Key_V4(bool init = true);
	const char* c_str() const { return m_str; }
	void set(const char* );
	void newKey();
	bool operator==(const Key_V4& rhs) const;
	bool valid() const;
	friend ostream& operator<<(ostream& os, const Key_V4& key);
//attr
private:
	char m_str[LENGTH + 1 + FILLER];
};
//
// inlines
//===========================================================================
*/
#pragma pack (pop)

#endif
