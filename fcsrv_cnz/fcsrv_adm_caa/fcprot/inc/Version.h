#ifndef _VERSION_H_
#define _VERSION_H_
/*
  NAME
  VERSION -
  LIBRARY 3C++
  PAGENAME VERSION
  HEADER  CPS
  LEFT_FOOTER Ericsson Utvecklings AB
  INCLUDE Version.H

  COPYRIGHT
  COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

  The Copyright to the computer program(s) herein is the property of
  Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
  copied only with the written permission from Ericsson Utvecklings AB
  or in accordance with the terms and conditions stipulated in the
  agreement/contract under which the program(s) have been supplied.

  DESCRIPTION
  General version class with major and minor numbering.

  ERROR HANDLING
  -

  DOCUMENT NO
  190 89-CAA 109 1410

  AUTHOR
  	  2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on Version.h of FCSRV's Windows version

  LINKAGE
  -

  SEE ALSO
  -

*/

#include "PlatformTypes.h"

#include <ostream>

using namespace std;

#pragma pack(push, 1)

class Version {
//foos
public:
	Version(u_int16 major = 0, u_int16 minor = 0) { set(major, minor); }
	// access
	u_int16 major() const { return m_major; }
	u_int16 minor() const { return m_minor; }
	void set(u_int16 major, u_int16 minor) { m_major = major; m_minor = minor; }
	void major(u_int16 val) { m_major = val; }
	void minor(u_int16 val) { m_minor = val; }
	// funx
	bool operator==(Version rhs) const { return m_major == rhs.m_major && m_minor == rhs.m_minor; }
	bool operator!=(Version rhs) const { return !(*this == rhs); }
	bool operator>(Version rhs) const { return m_major > rhs.m_major || m_minor > rhs.m_minor; }
	bool operator>=(Version rhs) const { return ((*this > rhs) || (*this == rhs)); }
	bool operator<=(Version rhs) const { return !(*this > rhs); }
	bool operator<(Version rhs) const { return !(*this >= rhs); }
	friend ostream& operator<<(ostream& , Version );
//attr
private:
	u_int16 m_major;
	u_int16 m_minor;
};
//
// inlines
//===========================================================================
#pragma pack(pop)

#endif
