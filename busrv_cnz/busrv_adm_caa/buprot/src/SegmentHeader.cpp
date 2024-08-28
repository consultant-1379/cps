/*
NAME
	File_name:SegmentHeader.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

The Copyright to the computer program(s) herein is the property of Ericsson
Utvecklings AB, Sweden.
The program(s) may be used and/or copied only with the written permission from
Ericsson Utvecklings AB or in accordance with the terms and conditions
stipulated in the agreement/contract under which the program(s) have been
supplied.

DESCRIPTION
	Direct- and Indirect Segment descriptors.

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
	2002-05-31 by U/Y/SF Anders Gillgren (qabgill, tag :ag)

SEE ALSO
	Memory layout must match "File Format Specification"/"Backup Protocol".

Revision history
----------------
2002-05-31 qabgill Created

*/



#include "SegmentHeader.h"
#include "buap.h"
#include <iomanip>

using namespace buap;
//
// friend
//===========================================================================
ostream& operator<<(ostream& os, const IndirectSegmentHeader& ish) 
{
    ios::fmtflags flg(os.flags());
	os << setfill('0')
		<< dec << "<" << ish.ban()
		<< "><" << ish.bnr() << "><"
		<< setw(8) << ish.blockSize() << " (" << setw(10) << (ish.blockSize() * 4) << ")><0x"
		<< hex << setw(8) << ish.checksum()
		<< ">" << setfill(' ');
    os.flags(flg);
    return os;
}
//
// friend
//===========================================================================
ostream& operator<<(ostream& os, const DirectSegmentHeader& dsh) 
{
    ios::fmtflags flg(os.flags());
	os << setfill('0')
		<< hex << "<0x" << setw(8) << dsh.startAddress() << " (" << dec << setw(8) << dsh.startAddress() << ")><"
		<< setw(8) << dsh.blockSize() << " (" << setw(10) << (dsh.blockSize() * 4) << ")><0x"
		<< hex << setw(8) << dsh.checksum()
		<< dec << ">";
    os.flags(flg);
	return os;
}
