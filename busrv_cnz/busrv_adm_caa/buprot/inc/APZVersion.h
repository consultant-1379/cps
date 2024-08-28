#ifndef _APZVERSION_H_
#define _APZVERSION_H_
/*
NAME
	APZVERSION -
LIBRARY 3C++
PAGENAME APZVERSION
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE APZVersion.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION
	APZ Version from OS-arean in CP.

ERROR HANDLING
	-

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
	2002-05-31 by U/Y/SF Anders Gillgren (qabgill, tag: ag)

LINKAGE
	-

SEE ALSO
	

*/
#pragma once

#include "PlatformTypes.h"

#include <ostream>

using namespace std;

#pragma pack(push, 1)

class APZVersion {
//foos
public:
	APZVersion(u_int16 type = 0, u_int16 version = 0, u_int16 revision = 0,
				  u_int16 prodNum = 0);
	// access
	u_int16 type() const { return m_type; }
	u_int16 version() const { return m_version; }
	u_int16 revision() const { return m_revision; }
	u_int16 prodNumInfo() const { return m_prodNumInfo; }
	// modify
	void type(u_int16 val) { m_type = val; }
	void version(u_int16 val) { m_version = val; }
	void revision(u_int16 val) { m_revision = val; }
	void prodNumInfo(u_int16 val) { m_prodNumInfo = val; }
	void set(u_int16 type, u_int16 version, u_int16 revision, u_int16 prodNum);
	// funx
	friend ostream& operator<<(ostream& , const APZVersion& );
//attr
private:
	u_int16 m_type;
	u_int16 m_version;
	u_int16 m_revision;
	u_int16 m_prodNumInfo;
};
//
// inlines
//===========================================================================
inline APZVersion::APZVersion(u_int16 type /* = 0 */, u_int16 version /* = 0 */,
										u_int16 revision /* = 0 */, u_int16 prodNum /* = 0 */)
: m_type(type), m_version(version), m_revision(revision), m_prodNumInfo(prodNum) {

}
//
inline void APZVersion::set(u_int16 type, u_int16 version, u_int16 revision,
	u_int16 prodNum) {
	m_type = type; m_version = version, m_revision = revision; m_prodNumInfo = prodNum;
}
 
#pragma pack(pop)

#endif
