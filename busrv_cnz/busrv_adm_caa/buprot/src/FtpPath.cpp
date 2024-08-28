/*
NAME
	File_name:FtpPath.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

The Copyright to the computer program(s) herein is the property of Ericsson
Utvecklings AB, Sweden.
The program(s) may be used and/or copied only with the written permission from
Ericsson Utvecklings AB or in accordance with the terms and conditions
stipulated in the agreement/contract under which the program(s) have been
supplied.

DESCRIPTION
	Dir and filename as seen by the CP (over FTP).

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
	2002-05-31 by U/Y/SF Anders Gillgren (qabgill, tag :ag)

SEE ALSO
	Memory layout must match "File Format Specification".

Revision history
----------------
2002-05-31 qabgill Created
2002-10-31 uablan  Added ACS_Trace

*/


#include "FtpPath.h"
//#include "CPS_BUSRV_Trace.h"
//#include <assert.h>
//
//
//
// friend
//===========================================================================
ostream& operator<<(ostream& os, const FtpPath& ftpPath) {
	return ftpPath.print(os);
}

//===========================================================================
ostream& operator<<(ostream& os, const FtpPath_V4& ftpPath) {
	return ftpPath.print(os);
}
