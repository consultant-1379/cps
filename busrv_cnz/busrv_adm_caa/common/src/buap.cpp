/*
NAME
	File_name:buap.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

The Copyright to the computer program(s) herein is the property of Ericsson
Utvecklings AB, Sweden.
The program(s) may be used and/or copied only with the written permission from
Ericsson Utvecklings AB or in accordance with the terms and conditions
stipulated in the agreement/contract under which the program(s) have been
supplied.

DESCRIPTION
	Global defines, constants etc. common to Backup AP programs.

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
	2002-03-12 by U/Y/SF Anders Gillgren (qabgill, tag :ag)

SEE ALSO
	-

Revision history
----------------
2002-03-12 qabgill Created
2003-01-27 uablan  Added APP_NAME for CPS_BUPLS and CPS_BUPDEF.
2003-02-12 uablan  Added APP_NAME for CPS_BUPIDLS

*/

#include "buap.h"

#include <iomanip>

#ifdef BUSRV
const char* buap::APP_NAME = "CPS_BUSRV";
#endif
#ifdef BUPDEF
const char* buap::APP_NAME = "CPS_BUPDEF";
#endif
#ifdef BUPLS
const char* buap::APP_NAME = "CPS_BUPLS";
#endif
#ifdef BUPIDLS
const char* buap::APP_NAME = "CPS_BUPIDLS";
#endif
#ifdef BIREAD
const char* buap::APP_NAME = "biread";
#endif
#ifdef RX2SBC
const char* buap::APP_NAME = "rx2sbc";
#endif
// add other program names here

const char* buap::PARAMS_FILE = "params";
//
// calc a 16-bit checksum on each W8 from the given start address
//
u_int16 buap::csumW16(const u_int8* pW8, u_int64 size) {
	u_int16 checksum = 0;
	for(u_int64	i = 0; i < size; ++i)
		checksum += pW8[i];
	return checksum;
}
//
// calc a W32 checksum on each W32 from the given start address
//===========================================================================
u_int32 buap::csumW32(const u_int32* pW32, u_int64 size) {

	union W32AsW8 {
		u_int32 w32;
		u_int8 w8[sizeof(u_int32)];
	};

	u_int32 checksum = 0;
	u_int64 i = 0;
	for( ; i < (size / sizeof(u_int32)); ++i)
		checksum += pW32[i];

	W32AsW8 item;
	item.w32 = 0;
	const u_int8* pW8 = reinterpret_cast<const u_int8*>(&pW32[i]);
	for(u_int32 j = 0; j < (size % sizeof(u_int32)); ++j)
		item.w8[j] = pW8[j];

	checksum += item.w32;
	return checksum;
}

#if 0
//
// global helper
//===========================================================================
ostream& buap::operator<<(ostream& os, const SYSTEMTIME& st) {
	os << "<" << dec << setfill('0')
		<< setw(2) << static_cast<short>(st.wYear) << "-"
		<< setw(2) << static_cast<short>(st.wMonth) << "-"
		<< setw(2) << static_cast<short>(st.wDay) << " "
		<< setw(2) << static_cast<short>(st.wHour) << ":"
		<< setw(2) << static_cast<short>(st.wMinute) << ":"
		<< setw(2) << static_cast<short>(st.wSecond) << " - "
		<< setfill(' ') << ">";
	return os;
}
//
// global helper
//===========================================================================
ostream& buap::operator<<(ostream& os, const FILETIME& ft) {
	LARGE_INTEGER li;
	li.LowPart = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;
	unsigned __int64 ui64 = li.QuadPart;
	os << "<" << ui64 << ">";
	return os;
}
#endif
