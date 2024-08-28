#ifndef _APZHWVARIANT_H_
#define _APZHWVARIANT_H_
/*
NAME
	EXCHANGEID -
LIBRARY 3C++
PAGENAME EXCHANGEID
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE ExchangeId.H

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
	190 89-CAA 109 0387

AUTHOR
	2002-05-31 by U/Y/SF Anders Gillgren (qabgill, tag: ag)

LINKAGE
	-

SEE ALSO
	Memory layout must match "File Format Specification"/Backup Protocol".

*/
#pragma once

//#include "PlatformTypes.h"
//#include <ostream>

#include "PlexString.h"
#include <ostream>

using namespace std;

//#pragma pack(push, 1)
//
// Exchange identity data within special segment.
//========================================================================
//
// inlines
//===========================================================================

//#pragma pack(pop)


typedef PlexString<3>  APZHwVariant;
ostream& operator<<(ostream& os, const APZHwVariant&);

#endif
