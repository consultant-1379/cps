
/*
NAME
	File_name:fcap.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

The Copyright to the computer program(s) herein is the property of Ericsson
Utvecklings AB, Sweden.
The program(s) may be used and/or copied only with the written permission from
Ericsson Utvecklings AB or in accordance with the terms and conditions
stipulated in the agreement/contract under which the program(s) have been
supplied.

DESCRIPTION
	Global defines, constants etc. common to Function Change AP programs.

DOCUMENT NO
	190 89-CAA 109 0670

AUTHOR
	2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on fcap of FCSRV in Windows
	and BUSRV's buap.cpp

SEE ALSO
	-

Revision history
----------------

*/

#include "fcap.h"

#include <iomanip>

#ifdef FCSRV
const char* fcap::APP_NAME = "CPS_FCSRV";
#endif

// add other program names here

const char* fcap::PARAMS_FILE = "params";
//
// calc a 16-bit checksum on each W8 from the given start address
//
u_int16 fcap::csumW16(const u_int8* pW8, u_int64 size) {
	u_int16 checksum = 0;
	for(u_int64	i = 0; i < size; ++i)
		checksum += pW8[i];
	return checksum;
}
//
// calc a W32 checksum on each W32 from the given start address
//===========================================================================
u_int32 fcap::csumW32(const u_int32* pW32, u_int64 size) {

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

//
// global helper
/*Later
//===========================================================================
ostream& fcap::operator<<(ostream& os, const SYSTEMTIME& st) {
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
ostream& fcap::operator<<(ostream& os, const FILETIME& ft) {
	LARGE_INTEGER li;
	li.LowPart = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;
	unsigned __int64 ui64 = li.QuadPart;
	os << "<" << ui64 << ">";
	return os;
}*/
