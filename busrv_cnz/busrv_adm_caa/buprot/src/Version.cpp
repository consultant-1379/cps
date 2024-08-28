/*
NAME
	File_name:Version.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

The Copyright to the computer program(s) herein is the property of Ericsson
Utvecklings AB, Sweden.
The program(s) may be used and/or copied only with the written permission from
Ericsson Utvecklings AB or in accordance with the terms and conditions
stipulated in the agreement/contract under which the program(s) have been
supplied.

DESCRIPTION
	General version class with major and minor numbering.

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
	2002-05-31 by U/Y/SF Anders Gillgren (qabgill, tag :ag)

SEE ALSO
	-

Revision history
----------------
2002-05-31 qabgill Created

*/


#include "Version.h"
//
// friend
//===========================================================================
ostream& operator<<(ostream& os, Version v) {
	return os << dec << "<" << v.major() << "><" << v.minor() << ">";
}

