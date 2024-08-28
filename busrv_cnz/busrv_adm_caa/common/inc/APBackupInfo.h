#ifndef _APBACKUPINFO_H_
#define _APBACKUPINFO_H_
/*
NAME
	APBACKUPINFO - 
LIBRARY 3C++
PAGENAME APBACKUPINFO
HEADER  CPS  
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE APBackupInfo.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2001. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the 
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION 
   AP specific routines for BackupInfo.
	The AP specific routines have to do with the creation of BackupInfo,
	and how data is inserted.

ERROR HANDLING
   -

DOCUMENT NO
   190 89-CAA 109 0387

AUTHOR 
   2001-06-26 by U/Y/SF Anders Gillgren (qabgill, tag: ag)

LINKAGE
	-

SEE ALSO 
   BackupInfo in the buprot code

Revision history
----------------
2001-06-26 Created   qabgill
2020-05-23 xdtthng   Modified for SSI by XDT/DEK/XDTTHNG
*/
#pragma once

#include "BackupInfo.h"

#include "SegmentHeader.h"

//#include <list>

using namespace std;
using namespace BUPDefs;
//
//typedef list<IndirectSegmentHeader> ISHList;
//typedef list<DirectSegmentHeader> DSHList;



/////////////////////////////////////////////////////////////////////////

class APBackupInfo {

public:
	APBackupInfo(const char* path);

	~APBackupInfo();

	// THNG: I do not understand what it is used for and who uses this
	// APBackupInfo(u_int32 datasize); // create w. empty block of size <param>

    // Funx
	void createFile(const char* path);
	void replaceSmall(const APBackupInfo& );
	void replaceSmallAndLarge(const APBackupInfo& );
        
        void replaceSmall(const APBackupInfo&, int);
        void replaceSmallAndLarge(const APBackupInfo&, int);

	// Forwarding to BackupInfo_Base and/or BackupInfo_V4

	Version getVersion() const;
	BackupInfo_Base::BI_ERROR_CODE valid() const;

	// The use of info() and info_V4() discards polymorphic behaviour of BackupInfo
	// Return V3 as in the base product
	BackupInfo_Base::Info_Base* info();
	const BackupInfo_Base::Info_Base* info() const;

	// Return V4 Info
	BackupInfo_Base::Info_V4* info_V4();
	const BackupInfo_Base::Info_V4* info_V4() const;

    IndirectSegmentHeader* sddHeader();
    const IndirectSegmentHeader* sddHeader() const;

	DirectSegmentHeader* ldd1Header();
    const DirectSegmentHeader* ldd1Header() const;

	DirectSegmentHeader* ldd2Header();
    const DirectSegmentHeader* ldd2Header() const;
    DirectSegmentHeader* psHeader();
    const DirectSegmentHeader* psHeader() const;
    DirectSegmentHeader* rsHeader();
    const DirectSegmentHeader* rsHeader() const;

	DirectSegmentHeader* ps2Header();
    const DirectSegmentHeader* ps2Header() const;
	DirectSegmentHeader* rs2Header();
    const DirectSegmentHeader* rs2Header() const;

    u_int32 calcCSum() const; 
    u_int64 sectorSizeW32s(const SBCFile&) const;	// total size of data area
    u_int64 sectorSizeW8s(const SBCFile&) const;	// total size of data area
    u_int32 headerSize(const SBCFile& ) const;		// total size of headers for SDD/LDD1/etc
    
    u_int32 getAdMarker() const;

    friend ostream& operator<<(ostream& , const APBackupInfo& );

protected:

	BackupInfo_Base	*m_backupInfo;

private:
	APBackupInfo(const APBackupInfo& ); // not impl.
	APBackupInfo& operator=(const APBackupInfo& ); // not impl.

};
//
//
//===========================================================================
inline APBackupInfo::~APBackupInfo() 
{
	if (m_backupInfo)
		delete m_backupInfo;
	//cout << "APBackupInfo desstructor" << endl;
}
//
//
//===========================================================================
inline Version APBackupInfo::getVersion() const
{
	return m_backupInfo->getVersion();
}
//
//
//===========================================================================
inline void APBackupInfo::replaceSmall(const APBackupInfo& bi) {
	m_backupInfo->replace(bi.m_backupInfo, false);

}
//
//
//===========================================================================
inline u_int64 APBackupInfo::sectorSizeW8s(const SBCFile& fid) const {
    return sectorSizeW32s(fid) * 4;
}
//
//
//===========================================================================
inline u_int64 APBackupInfo::sectorSizeW32s(const SBCFile& fid) const {
    return m_backupInfo->sectorSizeW32s(fid);
}
//
//
//===========================================================================
inline IndirectSegmentHeader* APBackupInfo::sddHeader()
{
		return m_backupInfo->sddHeader();
}
//
//
//===========================================================================
inline const IndirectSegmentHeader* APBackupInfo::sddHeader() const
{
		return m_backupInfo->sddHeader();
}
//
//
//===========================================================================
inline DirectSegmentHeader* APBackupInfo::ldd1Header()
{
		return m_backupInfo->ldd1Header();
}
//
//
//===========================================================================
inline const DirectSegmentHeader* APBackupInfo::ldd1Header() const
{
		return m_backupInfo->ldd1Header();
}
//
//
//===========================================================================
inline DirectSegmentHeader* APBackupInfo::ldd2Header()
{
		return m_backupInfo->ldd2Header();
}
//
//
//===========================================================================
inline const DirectSegmentHeader* APBackupInfo::ldd2Header() const
{
		return m_backupInfo->ldd2Header();
}
//
//
//===========================================================================
inline DirectSegmentHeader* APBackupInfo::psHeader()
{
		return m_backupInfo->psHeader();
}
//
//
//===========================================================================
inline const DirectSegmentHeader* APBackupInfo::psHeader() const
{
		return m_backupInfo->psHeader();
}
//
//
//===========================================================================
inline DirectSegmentHeader* APBackupInfo::rsHeader()
{
		return m_backupInfo->rsHeader();
}
//
//
//===========================================================================
inline const DirectSegmentHeader* APBackupInfo::rsHeader() const
{
		return m_backupInfo->rsHeader();
}
//
//
//===========================================================================
inline DirectSegmentHeader* APBackupInfo::ps2Header()
{
		return m_backupInfo->ps2Header();
}
//
//
//===========================================================================
inline const DirectSegmentHeader* APBackupInfo::ps2Header() const
{
		return m_backupInfo->ps2Header();
}
//
//
//===========================================================================
inline DirectSegmentHeader* APBackupInfo::rs2Header()
{
		return m_backupInfo->rs2Header();
}
//
//
//===========================================================================
inline const DirectSegmentHeader* APBackupInfo::rs2Header() const
{
		return m_backupInfo->rs2Header();
}
//
//
//===========================================================================
inline u_int32 APBackupInfo::headerSize(const SBCFile& fid) const 
{
	return m_backupInfo->headerSize(fid);
}
//
//
//===========================================================================
inline u_int32 APBackupInfo::calcCSum() const 
{
	return m_backupInfo->calcCSum();
}
//
//
//===========================================================================
inline void APBackupInfo::replaceSmallAndLarge(const APBackupInfo& bi) {

	m_backupInfo->replace(bi.m_backupInfo, true);
}
//
//
//===========================================================================
inline void APBackupInfo::replaceSmall(const APBackupInfo& bi, int progress) {

        m_backupInfo->replace(bi.m_backupInfo, false, progress);
}
//
//
//===========================================================================
inline void APBackupInfo::replaceSmallAndLarge(const APBackupInfo& bi, int progress) {

        m_backupInfo->replace(bi.m_backupInfo, true, progress);
}
//
//===========================================================================
inline ostream &operator<<(ostream &os, const APBackupInfo &bi)
{
	bi.m_backupInfo->print(os);
	bi.m_backupInfo->printDynamicArea(os);
	return os;
}
//
//
//===========================================================================
inline BackupInfo_V4::BI_ERROR_CODE APBackupInfo::valid() const
{
	return m_backupInfo->valid();
}
//
// 
// This will returns version 3 which ensures compatibility with existing code
//===========================================================================
inline const BackupInfo_Base::Info_Base* APBackupInfo::info() const
{
	return m_backupInfo->info();

}
inline BackupInfo_Base::Info_Base* APBackupInfo::info()
{
	return m_backupInfo->info();

}
//
//
// Version 4 needs to use Info_V4()
//===========================================================================
inline const BackupInfo_Base::Info_V4* APBackupInfo::info_V4() const
{
	return m_backupInfo->info_V4();

}
inline BackupInfo_Base::Info_V4* APBackupInfo::info_V4()
{
	return m_backupInfo->info_V4();

}

//
//
//===========================================================================
inline u_int32 APBackupInfo::getAdMarker() const
{
	return m_backupInfo->getAdMarker();
}

#endif
