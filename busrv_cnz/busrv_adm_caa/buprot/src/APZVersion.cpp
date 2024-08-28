/*
NAME
	File_name:APZVersion.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

The Copyright to the computer program(s) herein is the property of Ericsson
Utvecklings AB, Sweden.
The program(s) may be used and/or copied only with the written permission from
Ericsson Utvecklings AB or in accordance with the terms and conditions
stipulated in the agreement/contract under which the program(s) have been
supplied.

DESCRIPTION
	APZ Version from OS-arean in CP.

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
	2002-05-31 by U/Y/SF Anders Gillgren (qabgill, tag :ag)

SEE ALSO
	

Revision history
----------------
2002-05-31 qabgill Created

*/


#include "APZVersion.h"
//
// friend
//===========================================================================
ostream& operator<<(ostream& os, const APZVersion& apz) {
	return os << dec << "<APZ " << apz.type() << " " << apz.version() << "/" <<
		apz.prodNumInfo() << " R" <<apz.revision() << "A>";
}

