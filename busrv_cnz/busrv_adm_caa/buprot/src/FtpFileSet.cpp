/*
NAME
	File_name:FtpFileSet.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

The Copyright to the computer program(s) herein is the property of Ericsson
Utvecklings AB, Sweden.
The program(s) may be used and/or copied only with the written permission from
Ericsson Utvecklings AB or in accordance with the terms and conditions
stipulated in the agreement/contract under which the program(s) have been
supplied.

DESCRIPTION
	SBC file set as seen by the CP (over FTP).

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
	2002-05-31 by U/Y/SF Anders Gillgren (qabgill, tag :ag)

SEE ALSO
	Memory layout must match "File Format Specification".

Revision history
----------------
2002-05-31 qabgill Created
2010-05-23 xdtthng Modified for SSI

*/


#include "FtpFileSet.h"
//
// friend
//===========================================================================
ostream& operator<<(ostream& os, const FtpFileSet& ffs) {
	return ffs.print(os);
}

//===========================================================================
ostream& operator<<(ostream& os, const FtpFileSet_V4& ffs) {
	return ffs.print(os);
}
