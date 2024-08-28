#ifndef _SEGMENTHEADER_H_
#define _SEGMENTHEADER_H_
/*
NAME
	SEGMENTHEADER -
LIBRARY 3C++
PAGENAME SEGMENTHEADER
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE SegmentHeader.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION
	Direct- and Indirect Segment descriptors.

ERROR HANDLING
	-

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
	2002-05-31 by U/Y/SF Anders Gillgren (qabgill, tag: ag)

LINKAGE
	-

SEE ALSO
	Memory layout must match "File Format Specification"/"Backup Protocol".

*/
#pragma once

#include "PlatformTypes.h"

#include <ostream>

using namespace std;

#pragma pack(push, 1)
//
// Header for blocks using BAN and BN for lookup - SDD
//========================================================================
class IndirectSegmentHeader {
//foos
public:
	IndirectSegmentHeader() { m_ban = m_bnr = 0; m_blockSize = 0; m_checksum = 0; }
	// access	
	u_int16 ban() const { return m_ban; }
	u_int16 bnr() const { return m_bnr; }
	u_int64 blockSize() const { return m_blockSize; }
	u_int32 checksum() const { return m_checksum; }
	//virtual const void* pdata() const { return &m_ban; }
	//virtual size_t sizeW8() const { return sizeof(m_dat); }
	// modify
	void ban(u_int16 val) { m_ban = val; }
	void bnr(u_int16 val) { m_bnr = val; }
	void blockSize(u_int64 val) { m_blockSize = val; }
	void checksum(u_int32 val) { m_checksum = val; }
	// funx
	friend ostream& operator<<(ostream& , const IndirectSegmentHeader& );
	//attr
private:
	u_int16 m_ban;			// Base address number
	u_int16 m_bnr;			// Block number
	u_int64 m_blockSize;	// number of W32's
	u_int32 m_checksum;
};
//   
// Header for blocks using direct memory addressing for lookup - LDD, PS, RS
//========================================================================
class DirectSegmentHeader {
// types
public:
//foos
public:
	DirectSegmentHeader() { m_startAddress = m_blockSize = 0; m_checksum = 0; }
	// access	
	u_int64 startAddress() const { return m_startAddress; }
	u_int64 blockSize() const { return m_blockSize; }
	u_int32 checksum() const { return m_checksum; }
	//virtual const void* pdata() const { return &m_dat; }
	//virtual size_t sizeW8() const { return sizeof(m_dat); }
	// modify
	void startAddress(u_int64 val) { m_startAddress = val; }
	void blockSize(u_int64 val) { m_blockSize = val; }
	void checksum(u_int32 val) { m_checksum = val; }
	// funx
	friend ostream& operator<<(ostream& , const DirectSegmentHeader& );
	//attr
private:
	u_int64 m_startAddress;
	u_int64 m_blockSize;	// number of W32's
	u_int32 m_checksum;
};
//
// inlines
//===========================================================================

#pragma pack(pop)
#endif
