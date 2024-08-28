#ifndef _FCAP_H_
#define _FCAP_H_
/*
NAME
	BUAP -
LIBRARY 3C++
PAGENAME BUAP
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE buap.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION
	Global defines, constants etc. common to Backup AP programs -
	All in the buap namespace.

ERROR HANDLING
	-

DOCUMENT NO
	190 89-CAA 109 1410

AUTHOR
	2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on fcap.h of Windows version

LINKAGE
	-

SEE ALSO
	-

*/
#pragma once

#include "FCPDefs.h"
#include "PlatformTypes.h"

#include <assert.h>
#include <iostream>
#include <sstream>
using namespace std;

#if 0

//------------------------------------------------------------------------
//#ifdef _DEBUG
  mnst	#define TRACE(x) { stringstream ss;
	ss << x; OutputDebugString(ss.str().c_str()); cout << ss.str();}
//}
//#else
//----------------------------------------------------------------------------
// uncomment first line and remove the other lines for non-tracing release build
//#define TRACE(x) { }
//#define TRACE(x) { }
//----------------------------------------------------------------------------
//#endif

#endif

namespace fcap {
	using namespace FCPDefs; // import FCPDefs namespace
	extern const char* APP_NAME;
	extern const char* PARAMS_FILE;
// calc a W16 checksum on each W8 from the given start address
	u_int16 csumW16(const u_int8*, u_int64);
// calc a W32 checksum on each W32 from the given start address
	u_int32 csumW32(const u_int32*, u_int64);
	// helpers (__int64 not supported by ios before VC 7.0 release)

//#if _MSC_VER < 1300
//	ostream& operator<<(ostream& os, const __int64& value);
//	ostream& operator<<(ostream& os, const unsigned __int64& value);
//#endif

    // Linux porting note: thng
    // When we cross them, we will bring them back to life. In the mean time,
    // they are out of the way

	// helpers
//	ostream& operator<<(ostream& , const SYSTEMTIME& );
//	ostream& operator<<(ostream& , const FILETIME& );
};

//
// inlines
//===========================================================================

#endif

