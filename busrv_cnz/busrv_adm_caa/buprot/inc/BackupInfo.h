#ifndef _BACKUPINFO_H_
#define _BACKUPINFO_H_
/*
NAME
	BACKUPINFO -
LIBRARY 3C++
PAGENAME BACKUPINFO
HEADER CPS
LEFT_FOOTER Ericsson Utvecklings AB

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION
	BackupInfo struct(ure) - used in BUP/FT (Backup Protocol/File Transfer).

ERROR HANDLING
	-

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
	2002-02-05 by UAB/KY/SK Anders Gillgren (qabgill, tag: ag), based on
		BackupInfo.hxx

LINKAGE
	-

SEE ALSO


Revision history
----------------
2003-02-11 uablan  Added MiddleWare to BackupInfo and changed backupinfo version to 3.0.
2005-05-19 uablan  Added more MiddleWare info: UPBBloadModules & PCIHloadModules to BackupInfo.
2005-08-23 uablan  Changed MiddleWare info in buinfo struct to be general.
2010-05-23 xdtthng Modified for SSI

*/
#pragma once

#include "buap.h"
#include "BUPDefs.h"
#include "APZVersion.h"
#include "ExchangeId.h"
#include "SBCFile.h"
#include "SectorInfo.h"
#include "SegmentHeader.h"
#include "Version.h"
#include "MiddleWare.h"
#include "APZHwVariant.h"

#include <assert.h>
#include <memory.h>
//#include <new.h>
#include <ostream>

using namespace std;

enum {
        JustStarted        = 0,
        CreatingMarker     = 1,
        MarkerCreated      = 2,
        SDDmoved           = 4,
        LDD1renamed        = 8,
        PSrenamed          = 16,
        RSrenamed          = 32,
        LDD1moved          = 64,
        PSmoved            = 128,
        RSmoved            = 256,
        BUINFOmoved        = 512
};

class BackupInfo_Base {
// types
public:
	//friend class APBackupInfo_V4;
	enum {  VERSION_MAJOR = 3, VERSION_MINOR = 0,
            VERSION_MAJOR2 = 4, VERSION_MINOR2 = 0};

    enum BI_ERROR_CODE {
        BEC_NO_ERROR = 0,
        BEC_NO_DATA,
        BEC_BAD_CSUM,
        BEC_BAD_VERSION,
        BEC_BAD_SDD_DATETIME,
        BEC_BAD_LDD1_DATETIME,
        BEC_BAD_LDD2_DATETIME,
        BEC_BAD_PS_DATETIME,
        BEC_BAD_RS_DATETIME,
        BEC_NO_LDD1_SECTORS,
        BEC_NO_PS_SECTORS,
        BEC_NO_RS_SECTORS,
        BEC_BAD_EXCHANGE_ID,
        BEC_BAD_CMDLOG1_NUM,
        BEC_BAD_CMDLOG2_NUM,
        BEC_BAD_MW1, // exMicroProg for APZ21240, CPHWdump for APZ21250
        BEC_BAD_MW2, // microProgInProm for APZ21240 , mbcFWrphmi for APZ21250
        BEC_BAD_MW3, // mbcFirmware for APZ21240, UPBBloadModules for APZ 21250
        BEC_BAD_MW4, // sysBootImage for APZ21240, CPSBloadModules for APZ21250
        BEC_BAD_MW5, // plexEngineDump for APZ21240, PCIHloadModules for APZ21250
        BEC_BAD_MW6, // UPBBloadModules for APZ21240, plexEngineDump for APZ21250
        BEC_BAD_MW7, // PCIHloadModules for APZ21240, ConfigFileCPSB for APZ21250
	BEC_BAD_PS2_DATETIME,
	BEC_BAD_RS2_DATETIME,
	BEC_BAD_SIZE,
	BEC_BAD_APZ_HW_VARIANT,
        BEC_UNKNOWN_ERROR
    };

public:
 
    struct Info_Base {
        Info_Base(bool ); // to set defaults
        u_int32 checksum;
        Version version;  // Version of the backup
        APZVersion apzVersion;
        ExchangeId exchangeId;
        u_int32 generation;
        SectorInfo sddSectorInfo;
        SectorInfo ldd1SectorInfo;
        SectorInfo ldd2SectorInfo;
        SectorInfo psSectorInfo;
        SectorInfo rsSectorInfo;
        u_int32 cmdLogFile1;
        u_int32 cmdLogFile2;
        MiddleWare mw1;  // exMicroProg for APZ21240, CPHWdump for APZ21250
        MiddleWare mw2;  // microProgInProm for APZ21240 , mbcFWrphmi for APZ21250
        MiddleWare mw3;  // mbcFirmware for APZ21240, UPBBloadModules for APZ 21250
        MiddleWare mw4;  // sysBootImage for APZ21240, CPSBloadModules for APZ21250
        MiddleWare mw5;  // plexEngineDump for APZ21240, PCIHloadModules for APZ21250
        MiddleWare mw6;  // UPBBloadModules for APZ21240, plexEngineDump for APZ21250
        MiddleWare mw7;  // PCIHloadModules for APZ21240, ConfigFileCPSB for APZ21250i

        // 5 spare 32 bi words start from here
        APZHwVariant apzHwVariant;
        u_int32 filler[3];
        u_int32 admarker;  // Auto dump marker
    };


    struct Info_V4: public Info_Base {
        // BUP 4.0 extension starts here
        Info_V4(bool ); // to set defaults
        u_int32 dsGeneration;
        u_int16 ldd1OMProfile;
        u_int16 ldd1OMProfile_filler;
        u_int16 ldd1APZProfile;
        u_int16 ldd1APZProfile_filler;
        u_int16 ldd1APTProfile;
        u_int16 ldd1APTProfile_filler;
        u_int8  ldd1CpId;
        u_int8  ldd1CpId_u8_filler[3];
        //u_int16 ldd1CpId_filler;
        u_int16 ldd2OMProfile;
        u_int16 ldd2OMProfile_filler;
        u_int16 ldd2APZProfile;
        u_int16 ldd2APZProfile_filler;
        u_int16 ldd2APTProfile;
        u_int16 ldd2APTProfile_filler;
        u_int8  ldd2CpId;
        u_int8  ldd2CpId_u8_filler[3];
        //u_int16 ldd2CpId_filler;
        SectorInfo ps2SectorInfo;
        SectorInfo rs2SectorInfo;
        u_int32 filler2[5];
    };

	// For compatibility with version 3 code
	typedef Info_Base Info;

    
//
public:
    BackupInfo_Base(u_int8* pdata, u_int32 size, bool init, bool owner);
	virtual ~BackupInfo_Base() { if(m_owner) delete[] m_pdata; }

    // raw access & modify
    const u_int8* buf() const { return m_pdata; }
    u_int8* buf() { return m_pdata; }

    virtual void replace(const BackupInfo_Base* biNew, bool smallAndLarge);
    virtual void replace(const BackupInfo_Base*, bool smallAndLarge, int);
    virtual u_int32 getAdMarker() const;
    
    // typed access & modify
	Version getVersion() const;
	virtual Info_Base* info(); 
    virtual const Info_Base* info() const;

	// These are the methods belonging to Version 4 on this fat interface
	// User code will get NULL on Version 3 objects
	// Throwinng exception will stop error escalation
    virtual Info_V4* info_V4()
		{ return NULL; } //THN TODO: throw exception
    virtual const Info_V4* info_V4() const
		{return NULL;}	 //THN TODO: throw exception
	virtual DirectSegmentHeader* ps2Header()
		{return NULL; } //THN TODO: throw exception
	virtual const DirectSegmentHeader* ps2Header() const
		{return NULL;} //THN TODO: throw exception
	virtual DirectSegmentHeader* rs2Header()
		{return NULL;} // THN TODO: throw exception
	virtual const DirectSegmentHeader* rs2Header() const
		{return NULL;} // THN TODO: throw exception

	//virtual u_int16 getInfoSize() const;
    
    virtual IndirectSegmentHeader* sddHeader();
    virtual const IndirectSegmentHeader* sddHeader() const;
    
    virtual DirectSegmentHeader* ldd1Header();
    virtual const DirectSegmentHeader* ldd1Header() const;
    virtual DirectSegmentHeader* ldd2Header();
    virtual const DirectSegmentHeader* ldd2Header() const;
    virtual DirectSegmentHeader* psHeader();
    virtual const DirectSegmentHeader* psHeader() const;
    virtual DirectSegmentHeader* rsHeader();
    virtual const DirectSegmentHeader* rsHeader() const;

// funx
    u_int32 calcCSum() const; // csum for all data - the csum itself
    virtual u_int32 headerCnt(const SBCFile& ) const; // nof headers for SDD/LDD1/etc
    u_int32 headerSize(const SBCFile& ) const; // total size of headers for SDD/LDD1/etc
    virtual u_int64 sectorSizeW32s(const SBCFile&) const; // total size of data area
    virtual u_int64 sectorSizeW8s(const SBCFile&) const; // total size of data area
    virtual u_int32 totalSize() const; // total size of BackupInfo (Info + all headers)
    
	// validation with detailed reporting
	virtual BI_ERROR_CODE valid() const; 
    virtual BI_ERROR_CODE valid_others() const;

    virtual ostream& print(ostream &) const;    
    virtual ostream& printDynamicArea(ostream &) const;    
    friend ostream& operator<<(ostream& , const BackupInfo_Base& );
    
//private:
    ostream& printDSHs(ostream& , const char* name, u_int32 cnt, const DirectSegmentHeader* ) const;
// attrib
protected:
    u_int8*     m_pdata;
    bool        m_owner;
    u_int32     m_size;
};
//
// ctor
//===========================================================================
inline BackupInfo_Base::BackupInfo_Base(u_int8* pdata, u_int32 size, bool init, bool owner /* = false */)
: m_pdata(pdata), m_owner(owner), m_size(size) {

    assert((pdata && init) || (!pdata && !init) || (pdata && !init));
    
	if(init)
		new (m_pdata) BackupInfo_Base::Info_Base(init);
}
//
//
//===========================================================================
inline Version BackupInfo_Base::getVersion() const {
	return reinterpret_cast<BackupInfo_Base::Info_Base*>(m_pdata)->version;
}
//
//
//===========================================================================
inline BackupInfo_Base::Info_Base* BackupInfo_Base::info() {
    assert(m_pdata != 0);

    return reinterpret_cast<BackupInfo_Base::Info_Base*>(m_pdata);
}
//
//
//===========================================================================
inline 
const BackupInfo_Base::Info_Base* BackupInfo_Base::info() const {
    assert(m_pdata != 0);
    return reinterpret_cast<const BackupInfo_Base::Info_Base*>(m_pdata);
}
//
//
//===========================================================================
//inline u_int16 BackupInfo_Base::getInfoSize() const
//{
//	return sizeof(Info_Base);
//}
//
//
//===========================================================================
inline IndirectSegmentHeader* BackupInfo_Base::sddHeader() {
    assert(m_pdata != 0);
    return  
        reinterpret_cast<IndirectSegmentHeader*>(&m_pdata[sizeof(BackupInfo_Base::Info_Base)]);
 }
//
inline const IndirectSegmentHeader* BackupInfo_Base::sddHeader() const {
    assert(m_pdata != 0);
    return
        reinterpret_cast<const IndirectSegmentHeader*>(&m_pdata[sizeof(BackupInfo_Base::Info_Base)]);
}
//
//
//===========================================================================
inline DirectSegmentHeader* BackupInfo_Base::ldd1Header() {
    assert(m_pdata != 0);
    
	Info_Base* pinfo = reinterpret_cast<BackupInfo_Base::Info_Base*>(m_pdata);
	IndirectSegmentHeader* pi = reinterpret_cast<IndirectSegmentHeader*>(
		&m_pdata[sizeof(BackupInfo_Base::Info_Base)]) +
		pinfo->sddSectorInfo.nofSegments();

	return reinterpret_cast<DirectSegmentHeader*>(pi);
	
	//return reinterpret_cast<DirectSegmentHeader*>(
    //    &(sddHeader()[info()->sddSectorInfo.nofSegments()]));
}
//
inline const DirectSegmentHeader* BackupInfo_Base::ldd1Header() const {
    assert(m_pdata != 0);

	Info_Base* pinfo = reinterpret_cast<BackupInfo_Base::Info_Base*>(m_pdata);
	IndirectSegmentHeader* pi = reinterpret_cast<IndirectSegmentHeader*>(
		&m_pdata[sizeof(BackupInfo_Base::Info_Base)]) +
		pinfo->sddSectorInfo.nofSegments();

	return reinterpret_cast<DirectSegmentHeader*>(pi);
	
	//return reinterpret_cast<const DirectSegmentHeader*>(
    //    &(sddHeader()[info()->sddSectorInfo.nofSegments()]));
}
//
//
//===========================================================================
inline DirectSegmentHeader* BackupInfo_Base::ldd2Header() {
    assert(m_pdata != 0);

	Info_Base* pinfo = reinterpret_cast<BackupInfo_Base::Info_Base*>(m_pdata);
	IndirectSegmentHeader* pi = reinterpret_cast<IndirectSegmentHeader*>(
		&m_pdata[sizeof(BackupInfo_Base::Info_Base)]) +
		pinfo->sddSectorInfo.nofSegments();

	return reinterpret_cast<DirectSegmentHeader*>(pi) +
		pinfo->ldd1SectorInfo.nofSegments();

	//return reinterpret_cast<DirectSegmentHeader*>(
    //    &(ldd1Header()[info()->ldd1SectorInfo.nofSegments()]));
}
//
inline const DirectSegmentHeader* BackupInfo_Base::ldd2Header() const {
    assert(m_pdata != 0);

	Info_Base* pinfo = reinterpret_cast<BackupInfo_Base::Info_Base*>(m_pdata);
	IndirectSegmentHeader* pi = reinterpret_cast<IndirectSegmentHeader*>(
		&m_pdata[sizeof(BackupInfo_Base::Info_Base)]) +
		pinfo->sddSectorInfo.nofSegments();

	return reinterpret_cast<DirectSegmentHeader*>(pi) +
		pinfo->ldd1SectorInfo.nofSegments();

    //return reinterpret_cast<const DirectSegmentHeader*>(
    //    &(ldd1Header()[info()->ldd1SectorInfo.nofSegments()]));
}
//
//
//===========================================================================
inline DirectSegmentHeader* BackupInfo_Base::psHeader() {
    assert(m_pdata != 0);

	Info_Base* pinfo = reinterpret_cast<BackupInfo_Base::Info_Base*>(m_pdata);
	IndirectSegmentHeader* pi = reinterpret_cast<IndirectSegmentHeader*>(
		&m_pdata[sizeof(BackupInfo_Base::Info_Base)]) +
		pinfo->sddSectorInfo.nofSegments();

	return reinterpret_cast<DirectSegmentHeader*>(pi) +
		pinfo->ldd1SectorInfo.nofSegments() +
		pinfo->ldd2SectorInfo.nofSegments();

	//return reinterpret_cast<DirectSegmentHeader*>(
    //    &(ldd2Header()[info()->ldd2SectorInfo.nofSegments()]));
}
//
inline const DirectSegmentHeader* BackupInfo_Base::psHeader() const {
    assert(m_pdata != 0);

	Info_Base* pinfo = reinterpret_cast<BackupInfo_Base::Info_Base*>(m_pdata);
	IndirectSegmentHeader* pi = reinterpret_cast<IndirectSegmentHeader*>(
		&m_pdata[sizeof(BackupInfo_Base::Info_Base)]) +
		pinfo->sddSectorInfo.nofSegments();

	return reinterpret_cast<DirectSegmentHeader*>(pi) +
		pinfo->ldd1SectorInfo.nofSegments() +
		pinfo->ldd2SectorInfo.nofSegments();

    //return reinterpret_cast<const DirectSegmentHeader*>(
    //    &(ldd2Header()[info()->ldd2SectorInfo.nofSegments()]));
}
//
//
//===========================================================================
inline DirectSegmentHeader* BackupInfo_Base::rsHeader() {
    assert(m_pdata != 0);
	
	Info_Base* pinfo = reinterpret_cast<BackupInfo_Base::Info_Base*>(m_pdata);
	IndirectSegmentHeader* pi = reinterpret_cast<IndirectSegmentHeader*>(
		&m_pdata[sizeof(BackupInfo_Base::Info_Base)]) +
		pinfo->sddSectorInfo.nofSegments();

	return reinterpret_cast<DirectSegmentHeader*>(pi) +
		pinfo->ldd1SectorInfo.nofSegments() +
		pinfo->ldd2SectorInfo.nofSegments() +
		pinfo->psSectorInfo.nofSegments();

	//return reinterpret_cast<DirectSegmentHeader*>(
    //    &(psHeader()[info()->psSectorInfo.nofSegments()]));
}
//
inline const DirectSegmentHeader* BackupInfo_Base::rsHeader() const {
    assert(m_pdata != 0);

	Info_Base* pinfo = reinterpret_cast<BackupInfo_Base::Info_Base*>(m_pdata);
	IndirectSegmentHeader* pi = reinterpret_cast<IndirectSegmentHeader*>(
		&m_pdata[sizeof(BackupInfo_Base::Info_Base)]) +
		pinfo->sddSectorInfo.nofSegments();

	return reinterpret_cast<DirectSegmentHeader*>(pi) +
		pinfo->ldd1SectorInfo.nofSegments() +
		pinfo->ldd2SectorInfo.nofSegments() +
		pinfo->psSectorInfo.nofSegments();
	
	//return reinterpret_cast<const DirectSegmentHeader*>(
    //    &(psHeader()[info()->psSectorInfo.nofSegments()]));
}
//
// size of backupinfo data
//===========================================================================
inline u_int32 BackupInfo_Base::totalSize() const {
    return sizeof(BackupInfo_Base::Info_Base) + headerSize(SBCFile::SDD) +
        headerSize(SBCFile::LDD1) + headerSize(SBCFile::LDD2) +
        headerSize(SBCFile::PS) + headerSize(SBCFile::RS);
}
//
// size of sector (0-*) segments in SDD/LDD1/LDD2/PS/RS
//===========================================================================
inline u_int64 BackupInfo_Base::sectorSizeW8s(const SBCFile& fid) const {
    return sectorSizeW32s(fid) * 4;
}
//
// calc new csum
//===========================================================================
inline u_int32 BackupInfo_Base::calcCSum() const {
    assert(m_pdata != 0);
    u_int32 size = m_size - sizeof(u_int32);
    const u_int32* pui32 = reinterpret_cast<const u_int32*>(m_pdata + sizeof(u_int32));
    u_int32 csum = buap::csumW32(pui32, size);

    return csum;
}
//
//
//===========================================================================
inline u_int32 BackupInfo_Base::headerSize(const SBCFile& fileId) const {
    return headerCnt(fileId) * (fileId != SBCFile::SDD ?
        sizeof(DirectSegmentHeader) : sizeof(IndirectSegmentHeader));
}
//
// to set defaults
//===========================================================================
inline BackupInfo_Base::Info_Base::Info_Base(bool init) {
    if(init) {
        checksum = 0;
        cmdLogFile1 = cmdLogFile2 = 0u;
        memset(filler, 0, sizeof(filler));
        version.set(BackupInfo_Base::VERSION_MAJOR, BackupInfo_Base::VERSION_MINOR);
        // additional inits? 0's?
        memset(static_cast<void*>(&apzVersion), 0, sizeof(apzVersion) + sizeof(exchangeId) +
            sizeof(generation) + (sizeof(SectorInfo) * 5) +
            (sizeof(cmdLogFile1) * 2) + (sizeof(mw1)*5));
    }
}

//
//
//===========================================================================
inline u_int32 BackupInfo_Base::getAdMarker() const {
    return reinterpret_cast<BackupInfo_Base::Info_Base*>(m_pdata)->admarker;
}


/****************************************************************************
 *
 * BACKUP INFO V4
 *
 ****************************************************************************/

class BackupInfo_V4: public BackupInfo_Base {

public:
  
    BackupInfo_V4(u_int8* pdata, u_int32 size, bool init, bool owner) : 
		BackupInfo_Base(pdata, size, init, owner) {};
	virtual ~BackupInfo_V4() {};
        
    virtual Info_V4* info_V4();
    virtual const Info_V4* info_V4() const;

	//virtual u_int16 getInfoSize() const;

    virtual void replace(const BackupInfo_Base* biNew, bool smallAndLarge);
    virtual void replace(const BackupInfo_Base* biNew, bool smallAndLarge, int);
    virtual u_int32 getAdMarker() const;
      
    virtual IndirectSegmentHeader* sddHeader();
    virtual const IndirectSegmentHeader* sddHeader() const;
	virtual u_int32 totalSize() const;
	virtual u_int32 headerCnt(const SBCFile& ) const; // nof headers for SDD/LDD1/etc

	virtual BI_ERROR_CODE valid() const; // validation w. detailed reporting
    virtual BI_ERROR_CODE valid_others() const;
 
    virtual DirectSegmentHeader* ldd1Header();
    virtual const DirectSegmentHeader* ldd1Header() const;
    virtual DirectSegmentHeader* ldd2Header();
    virtual const DirectSegmentHeader* ldd2Header() const;
    virtual DirectSegmentHeader* psHeader();
    virtual const DirectSegmentHeader* psHeader() const;
    virtual DirectSegmentHeader* rsHeader();
    virtual const DirectSegmentHeader* rsHeader() const;

	virtual DirectSegmentHeader* ps2Header();
    virtual const DirectSegmentHeader* ps2Header() const;
    virtual DirectSegmentHeader* rs2Header();
    virtual const DirectSegmentHeader* rs2Header() const;

	virtual u_int64 sectorSizeW32s(const SBCFile&) const; // total size of data area
    virtual u_int64 sectorSizeW8s(const SBCFile&) const; // total size of data area
 
    virtual ostream& print(ostream &) const;    
    virtual ostream& printDynamicArea(ostream &) const;    
    friend ostream& operator<<(ostream& , const BackupInfo_V4& );
};
//
//
//===========================================================================
inline BackupInfo_V4::Info_V4* BackupInfo_V4::info_V4() {
    assert(m_pdata != 0);
    return reinterpret_cast<BackupInfo_V4::Info_V4*>(m_pdata);
}
//
inline const BackupInfo_V4::Info_V4* BackupInfo_V4::info_V4() const {
    assert(m_pdata != 0);
    return reinterpret_cast<const BackupInfo_V4::Info_V4*>(m_pdata);
}
//
//
//===========================================================================
//inline u_int16 BackupInfo_V4::getInfoSize() const
//{
//	return sizeof(Info_V4);
//}
//
//
//===========================================================================
inline IndirectSegmentHeader* BackupInfo_V4::sddHeader() {
    assert(m_pdata != 0);
    return  
        reinterpret_cast<IndirectSegmentHeader*>(&m_pdata[sizeof(BackupInfo_V4::Info_V4)]);
 }
//
inline const IndirectSegmentHeader* BackupInfo_V4::sddHeader() const {
    assert(m_pdata != 0);
    return
        reinterpret_cast<IndirectSegmentHeader*>(&m_pdata[sizeof(BackupInfo_V4::Info_V4)]);
}
//
//
//===========================================================================
inline DirectSegmentHeader* BackupInfo_V4::ldd1Header() {
    assert(m_pdata != 0);
    
	Info_V4* pinfo = reinterpret_cast<BackupInfo_Base::Info_V4*>(m_pdata);
	return reinterpret_cast<DirectSegmentHeader*>(&m_pdata[sizeof(Info_V4)]) +
        pinfo->sddSectorInfo.nofSegments();
	
	//return reinterpret_cast<DirectSegmentHeader*>(
    //    &(sddHeader()[info()->sddSectorInfo.nofSegments()]));
}
//
inline const DirectSegmentHeader* BackupInfo_V4::ldd1Header() const {
    assert(m_pdata != 0);

	Info_V4* pinfo = reinterpret_cast<BackupInfo_Base::Info_V4*>(m_pdata);
	return reinterpret_cast<const DirectSegmentHeader*>(&m_pdata[sizeof(Info_V4)]) +
        pinfo->sddSectorInfo.nofSegments();
	
	//return reinterpret_cast<const DirectSegmentHeader*>(
    //    &(sddHeader()[info()->sddSectorInfo.nofSegments()]));
}
//
//
//===========================================================================
inline DirectSegmentHeader* BackupInfo_V4::ldd2Header() {
    assert(m_pdata != 0);

	Info_V4* pinfo = reinterpret_cast<BackupInfo_Base::Info_V4*>(m_pdata);

	return reinterpret_cast<DirectSegmentHeader*>(&m_pdata[sizeof(Info_V4)]) +
        pinfo->sddSectorInfo.nofSegments() +
		pinfo->ldd1SectorInfo.nofSegments();

	//return reinterpret_cast<DirectSegmentHeader*>(
    //    &(ldd1Header()[info()->ldd1SectorInfo.nofSegments()]));
}
//
inline const DirectSegmentHeader* BackupInfo_V4::ldd2Header() const {
    assert(m_pdata != 0);

	Info_V4* pinfo = reinterpret_cast<BackupInfo_Base::Info_V4*>(m_pdata);
	return reinterpret_cast<const DirectSegmentHeader*>(&m_pdata[sizeof(Info_V4)]) +
        pinfo->sddSectorInfo.nofSegments() +
		pinfo->ldd1SectorInfo.nofSegments();

    //return reinterpret_cast<const DirectSegmentHeader*>(
    //    &(ldd1Header()[info()->ldd1SectorInfo.nofSegments()]));
}
//
//
//===========================================================================
inline DirectSegmentHeader* BackupInfo_V4::psHeader() {
    assert(m_pdata != 0);

	Info_V4* pinfo = reinterpret_cast<BackupInfo_Base::Info_V4*>(m_pdata);
	return reinterpret_cast<DirectSegmentHeader*>(&m_pdata[sizeof(Info_V4)]) +
        pinfo->sddSectorInfo.nofSegments() +
		pinfo->ldd1SectorInfo.nofSegments() +
		pinfo->ldd2SectorInfo.nofSegments();
	
	//return reinterpret_cast<DirectSegmentHeader*>(
    //    &(ldd2Header()[info()->ldd2SectorInfo.nofSegments()]));
}
//
inline const DirectSegmentHeader* BackupInfo_V4::psHeader() const {
    assert(m_pdata != 0);

	Info_V4* pinfo = reinterpret_cast<BackupInfo_Base::Info_V4*>(m_pdata);
	return reinterpret_cast<const DirectSegmentHeader*>(&m_pdata[sizeof(Info_V4)]) +
        pinfo->sddSectorInfo.nofSegments() +
		pinfo->ldd1SectorInfo.nofSegments() +
		pinfo->ldd2SectorInfo.nofSegments();

    //return reinterpret_cast<const DirectSegmentHeader*>(
    //    &(ldd2Header()[info()->ldd2SectorInfo.nofSegments()]));
}
//
//
//===========================================================================
inline DirectSegmentHeader* BackupInfo_V4::rsHeader() {
    assert(m_pdata != 0);
	
	Info_V4* pinfo = reinterpret_cast<BackupInfo_Base::Info_V4*>(m_pdata);
	
	return reinterpret_cast<DirectSegmentHeader*>(&m_pdata[sizeof(Info_V4)]) +
        pinfo->sddSectorInfo.nofSegments() +
		pinfo->ldd1SectorInfo.nofSegments() +
		pinfo->ldd2SectorInfo.nofSegments() +
		pinfo->psSectorInfo.nofSegments();
	
	//return reinterpret_cast<DirectSegmentHeader*>(
    //    &(psHeader()[info()->psSectorInfo.nofSegments()]));
}
//
inline const DirectSegmentHeader* BackupInfo_V4::rsHeader() const {
    assert(m_pdata != 0);

	Info_V4* pinfo = reinterpret_cast<BackupInfo_Base::Info_V4*>(m_pdata);
	return reinterpret_cast<const DirectSegmentHeader*>(&m_pdata[sizeof(Info_V4)]) +
        pinfo->sddSectorInfo.nofSegments() +
		pinfo->ldd1SectorInfo.nofSegments() +
		pinfo->ldd2SectorInfo.nofSegments() +
		pinfo->psSectorInfo.nofSegments();
	
	//return reinterpret_cast<const DirectSegmentHeader*>(
    //    &(psHeader()[info()->psSectorInfo.nofSegments()]));
}
//
//
//===========================================================================
inline DirectSegmentHeader* BackupInfo_V4::ps2Header() {
    assert(m_pdata != 0);

	Info_V4* pinfo = reinterpret_cast<BackupInfo_Base::Info_V4*>(m_pdata);
	return reinterpret_cast<DirectSegmentHeader*>(&m_pdata[sizeof(Info_V4)]) +
        pinfo->sddSectorInfo.nofSegments() +
		pinfo->ldd1SectorInfo.nofSegments() +
		pinfo->ldd2SectorInfo.nofSegments() +
		pinfo->psSectorInfo.nofSegments() +
		pinfo->rsSectorInfo.nofSegments();

    //return reinterpret_cast<DirectSegmentHeader*>(
    //    &(rsHeader()[info_V4()->rsSectorInfo.nofSegments()]));
}
//
inline const DirectSegmentHeader* BackupInfo_V4::ps2Header() const {
    assert(m_pdata != 0);

	Info_V4* pinfo = reinterpret_cast<BackupInfo_Base::Info_V4*>(m_pdata);
	return reinterpret_cast<const DirectSegmentHeader*>(&m_pdata[sizeof(Info_V4)]) +
        pinfo->sddSectorInfo.nofSegments() +
		pinfo->ldd1SectorInfo.nofSegments() +
		pinfo->ldd2SectorInfo.nofSegments() +
		pinfo->psSectorInfo.nofSegments() +
		pinfo->rsSectorInfo.nofSegments();
	
	//return reinterpret_cast<const DirectSegmentHeader*>(
    //    &(rsHeader()[info_V4()->rsSectorInfo.nofSegments()]));
}
//
//
//===========================================================================
inline DirectSegmentHeader* BackupInfo_V4::rs2Header() {
    assert(m_pdata != 0);

	Info_V4* pinfo = reinterpret_cast<BackupInfo_Base::Info_V4*>(m_pdata);	
	return reinterpret_cast<DirectSegmentHeader*>(&m_pdata[sizeof(Info_V4)]) +
        pinfo->sddSectorInfo.nofSegments() +
		pinfo->ldd1SectorInfo.nofSegments() +
		pinfo->ldd2SectorInfo.nofSegments() +
		pinfo->psSectorInfo.nofSegments() +
		pinfo->rsSectorInfo.nofSegments() +
		pinfo->ps2SectorInfo.nofSegments();

	//return reinterpret_cast<DirectSegmentHeader*>(
    //    &(ps2Header()[info_V4()->ps2SectorInfo.nofSegments()]));
}
//
inline const DirectSegmentHeader* BackupInfo_V4::rs2Header() const {
    assert(m_pdata != 0);
	
	Info_V4* pinfo = reinterpret_cast<BackupInfo_Base::Info_V4*>(m_pdata);
	return reinterpret_cast<DirectSegmentHeader*>(&m_pdata[sizeof(Info_V4)]) +
        pinfo->sddSectorInfo.nofSegments() +
		pinfo->ldd1SectorInfo.nofSegments() +
		pinfo->ldd2SectorInfo.nofSegments() +
		pinfo->psSectorInfo.nofSegments() +
		pinfo->rsSectorInfo.nofSegments() +
		pinfo->ps2SectorInfo.nofSegments();

	//return reinterpret_cast<const DirectSegmentHeader*>(
    //    &(ps2Header()[info_V4()->ps2SectorInfo.nofSegments()]));
}
//
// size of backupinfo data
//===========================================================================
inline u_int32 BackupInfo_V4::totalSize() const {
    return 
        (sizeof(BackupInfo_V4::Info_V4) + headerSize(SBCFile::SDD) +
        headerSize(SBCFile::LDD1) + headerSize(SBCFile::LDD2) +
        headerSize(SBCFile::PS) + headerSize(SBCFile::RS) +
        headerSize(SBCFile::PS2) + headerSize(SBCFile::RS2));
}
//
// to set defaults
//===========================================================================
inline BackupInfo_V4::Info_V4::Info_V4(bool init) : BackupInfo_Base::Info_Base(init) {
    if(init) {
        version.set(BackupInfo_Base::VERSION_MAJOR2, BackupInfo_Base::VERSION_MINOR2);
        // additional inits? 0's?
    }
}
//
// size of sector (0-*) segments in SDD/LDD1/LDD2/PS/RS
//===========================================================================
inline u_int64 BackupInfo_V4::sectorSizeW8s(const SBCFile& fid) const {
    return sectorSizeW32s(fid) * 4;
}

//
//
//===========================================================================
inline u_int32 BackupInfo_V4::getAdMarker() const {
   return reinterpret_cast<BackupInfo_Base::Info_V4*>(m_pdata)->admarker; 
}

#endif
