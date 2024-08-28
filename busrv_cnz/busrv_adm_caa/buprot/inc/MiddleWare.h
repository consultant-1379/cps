#ifndef _MIDDLEWARE_H_
#define _MIDDLEWARE_H_
/*
NAME
	MIDDLEWARE -
LIBRARY 3C++
PAGENAME MIDDLEWARE
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE MiddleWare.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION
	-

ERROR HANDLING
	-

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
	2003-01-15 by UKY/SF Lillemor Pettersson (uablan)

LINKAGE
	-

SEE ALSO
	Memory layout must match "File Format Specification"/Backup Protocol".

*/
#pragma once

#include "PlexString.h"
#include <ostream>

using namespace std;

//#pragma pack(push, 1)
//
// Middleware data within special segment.
//========================================================================
//
// inlines
//===========================================================================
//#pragma pack(pop)

typedef PlexString<35>   MiddleWare;
ostream& operator<<(ostream& os, const MiddleWare& mware);

#endif
