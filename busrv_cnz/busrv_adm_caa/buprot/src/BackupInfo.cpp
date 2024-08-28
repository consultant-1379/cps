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
		BackupInfo.cxx

LINKAGE
	-

SEE ALSO

Revision history
----------------
2002-05-02 qabgill Created
2002-11-04 uablan  Add ACS Trace
2005-05-19 uablan  Added more MiddleWare info: UPBBloadModules & PCIHloadModules to BackupInfo.
2005-08-23 uablan  Changed MiddleWare info in buinfo struct to be general.
2006-06-26 uablan  Added more trace points.
2010-05-23 xdtthng Modified for SSI

*/

#include "BackupInfo.h"
#include "CPS_BUSRV_Trace.h"

#include <stdio.h>
#include <cstring> // strncpy


//////////////////////////////////////////////////////////////////////////////


//
// Check validity of the BackupInfo data.
//===========================================================================
BackupInfo_Base::BI_ERROR_CODE BackupInfo_Base::valid() const {

    newTRACE((LOG_LEVEL_INFO, "BackupInfo_Base::BI_ERROR_CODE BackupInfo_Base::valid()", 0));

	BackupInfo_Base::BI_ERROR_CODE ec = BackupInfo_Base::BEC_NO_ERROR;

    const BackupInfo_Base::Info_Base* i = info();
    if(i == 0) {
        ec = BEC_NO_DATA;
        TRACE((LOG_LEVEL_ERROR, "BackupInfo_Base.valid() error: NO_DATA\n", 0));
    }
    if(ec == BEC_NO_ERROR && i->checksum != calcCSum()) {
        ec = BEC_BAD_CSUM;
        //TRACE(("BackupInfo.valid() error: BAD_CSUM ", 0));
        TRACE((LOG_LEVEL_ERROR, "BackupInfo.valid() error: BAD_CSUM. i->checksum=0x%X, calcCSum()=0x%X ", 0, i->checksum, calcCSum()));
    }
    // This is perhaps only occurs during porting BC
    // Consider to remove for the final production; the same applies to V4
    if(ec == BEC_NO_ERROR && m_size != totalSize()) {
        ec = BEC_BAD_SIZE;
        TRACE((LOG_LEVEL_ERROR, "BackupInfo.valid() error: BAD_SIZE ", 0));
    }
    Version currVersion(BackupInfo_Base::VERSION_MAJOR, BackupInfo_Base::VERSION_MINOR);
    if (ec == BEC_NO_ERROR && i->version != currVersion) {
        ec = BEC_BAD_VERSION;
        TRACE((LOG_LEVEL_ERROR, "BackupInfo.valid() error: BAD_VERSION ", 0));
    }
	// sddSector must exists for Version 3 
	if(ec == BEC_NO_ERROR && !i->sddSectorInfo.outputTime().valid()) {
		ec = BEC_BAD_SDD_DATETIME;
		TRACE((LOG_LEVEL_ERROR, "BackupInfo.valid() error: BAD_SDD_DATETIME - %d-%d-%d %d:%d", 0, i->sddSectorInfo.outputTime().year(),
			i->sddSectorInfo.outputTime().month(),i->sddSectorInfo.outputTime().day(),i->sddSectorInfo.outputTime().hour(),
			i->sddSectorInfo.outputTime().minute()));
	}
    if (ec == BEC_NO_ERROR) {
        ec = valid_others();
    }

    TRACE((LOG_LEVEL_INFO, "BackupInfo.valid() ec: %d", 0, ec));
    return ec;
}
//===========================================================================
BackupInfo_Base::BI_ERROR_CODE BackupInfo_Base::valid_others() const {

    newTRACE((LOG_LEVEL_INFO, "BackupInfo_Base::BI_ERROR_CODE BackupInfo_Base::valid_others()", 0));

	BackupInfo_V4::BI_ERROR_CODE ec = BEC_NO_ERROR;

    const BackupInfo_Base::Info_Base* i = info();

    // TODO: Auto dump impr. Need to check if ADMARKER & adMarker buinfo exist ?? 
#if 0
    if(ec == BEC_NO_ERROR && !i->ldd1SectorInfo.nofSegments()) {
        ec = BEC_NO_LDD1_SECTORS;
        TRACE((LOG_LEVEL_ERROR, "BackupInfo.valid() error: NO_LDD1_SECTORS\n", 0));
    }


    if(ec == BEC_NO_ERROR && !i->psSectorInfo.nofSegments()) {
        ec = BEC_NO_PS_SECTORS;
        TRACE((LOG_LEVEL_ERROR, "BackupInfo.valid() error: NO_PS_SECTORS\n", 0));
    }
    if(ec == BEC_NO_ERROR && !i->rsSectorInfo.nofSegments()) {
        ec = BEC_NO_RS_SECTORS;
        TRACE((LOG_LEVEL_ERROR, "BackupInfo.valid() error: NO_RS_SECTORS\n", 0));
    }
#endif


#if 0
    if(ec == BEC_NO_ERROR && !i->ldd1SectorInfo.outputTime().valid()) {
        ec = BEC_BAD_LDD1_DATETIME;
        TRACE((LOG_LEVEL_ERROR, "BackupInfo.valid() error: BAD_LDD1_DATETIME ", 0, i->ldd1SectorInfo.outputTime().year(),
            i->ldd1SectorInfo.outputTime().month(),i->ldd1SectorInfo.outputTime().day(),i->ldd1SectorInfo.outputTime().hour(),
            i->ldd1SectorInfo.outputTime().minute()));
    }
#endif

    if(ec == BEC_NO_ERROR && (i->ldd1SectorInfo.nofSegments() ?
        !i->ldd1SectorInfo.outputTime().valid() : !i->ldd1SectorInfo.outputTime().null())) {
        ec = BEC_BAD_LDD1_DATETIME;
        TRACE((LOG_LEVEL_ERROR, "BackupInfo.valid() error: BAD_LDD1_DATETIME ", 0, i->ldd1SectorInfo.outputTime().year(),
            i->ldd1SectorInfo.outputTime().month(),i->ldd1SectorInfo.outputTime().day(),i->ldd1SectorInfo.outputTime().hour(),
            i->ldd1SectorInfo.outputTime().minute()));
    }


    if(ec == BEC_NO_ERROR && (i->ldd2SectorInfo.nofSegments() ?
            !i->ldd2SectorInfo.outputTime().valid() :
            !i->ldd2SectorInfo.outputTime().null())) {
        ec = BEC_BAD_LDD2_DATETIME;
        TRACE((LOG_LEVEL_ERROR, "BackupInfo.valid() error: BAD_LDD2_DATETIME ", 0, i->ldd2SectorInfo.outputTime().year(),
            i->ldd2SectorInfo.outputTime().month(),i->ldd2SectorInfo.outputTime().day(),i->ldd2SectorInfo.outputTime().hour(),
            i->ldd2SectorInfo.outputTime().minute()));
    }

#if 0
    if(ec == BEC_NO_ERROR && !i->psSectorInfo.outputTime().valid()) {
        ec = BEC_BAD_PS_DATETIME;
        TRACE((LOG_LEVEL_ERROR, "BackupInfo.valid() error: BAD_PS_DATETIME ", 0, i->psSectorInfo.outputTime().year(),
            i->psSectorInfo.outputTime().month(),i->psSectorInfo.outputTime().day(),i->psSectorInfo.outputTime().hour(),
            i->psSectorInfo.outputTime().minute()));
    }
    if(ec == BEC_NO_ERROR && !i->rsSectorInfo.outputTime().valid()) {
        ec = BEC_BAD_RS_DATETIME;
        TRACE((LOG_LEVEL_ERROR, "BackupInfo.valid() error: BAD_RS_DATETIME ", 0, i->rsSectorInfo.outputTime().year(),
            i->rsSectorInfo.outputTime().month(),i->rsSectorInfo.outputTime().day(),i->rsSectorInfo.outputTime().hour(),
            i->rsSectorInfo.outputTime().minute()));
    }
#endif

    if(ec == BEC_NO_ERROR && (i->psSectorInfo.nofSegments() ?
            !i->psSectorInfo.outputTime().valid() :
            !i->psSectorInfo.outputTime().null())) {
        ec = BEC_BAD_PS_DATETIME;
        TRACE((LOG_LEVEL_ERROR, "BackupInfo.valid() error: BAD_PS_DATETIME ", 0, i->psSectorInfo.outputTime().year(),
            i->psSectorInfo.outputTime().month(),i->psSectorInfo.outputTime().day(),i->psSectorInfo.outputTime().hour(),
            i->psSectorInfo.outputTime().minute()));
    }

    if(ec == BEC_NO_ERROR && (i->rsSectorInfo.nofSegments() ?
            !i->rsSectorInfo.outputTime().valid() :
            !i->rsSectorInfo.outputTime().null())) {
        ec = BEC_BAD_RS_DATETIME;
        TRACE((LOG_LEVEL_ERROR, "BackupInfo.valid() error: BAD_RS_DATETIME ", 0, i->rsSectorInfo.outputTime().year(),
            i->rsSectorInfo.outputTime().month(),i->rsSectorInfo.outputTime().day(),i->rsSectorInfo.outputTime().hour(),
            i->rsSectorInfo.outputTime().minute()));
    }

    if(ec == BEC_NO_ERROR && i->exchangeId.nofCharacters() > ExchangeId::MAX_LENGTH) {
        ec = BEC_BAD_EXCHANGE_ID;
        TRACE((LOG_LEVEL_ERROR, "BackupInfo.valid() error: BAD_EXCHANGE_ID : %s", 0, i->exchangeId.character()));
    }
    if(ec == BEC_NO_ERROR && i->cmdLogFile1 > BUPDefs::MAX_CMDLOG_FILE) {
        ec = BEC_BAD_CMDLOG1_NUM;
        TRACE((LOG_LEVEL_ERROR, "BackupInfo.valid() error: BAD_CMDLOG1_NUM : %d", 0, i->cmdLogFile1));
    }
    if(ec == BEC_NO_ERROR && i->cmdLogFile2 > BUPDefs::MAX_CMDLOG_FILE) {
        ec = BEC_BAD_CMDLOG2_NUM;
        TRACE((LOG_LEVEL_ERROR, "BackupInfo.valid() error: BAD_CMDLOG2_NUM : %d", 0, i->cmdLogFile2));
    }
    if(ec == BEC_NO_ERROR && i->mw1.nofCharacters() > MiddleWare::MAX_LENGTH) {
        ec = BEC_BAD_MW1;
        TRACE((LOG_LEVEL_ERROR, "BackupInfo.valid() error: BEC_BAD_MW1: To long string : %s", 0, i->mw1.character()));
    }
    if(ec == BEC_NO_ERROR && i->mw2.nofCharacters() > MiddleWare::MAX_LENGTH) {
        ec = BEC_BAD_MW2;
        TRACE((LOG_LEVEL_ERROR, "BackupInfo.valid() error: BEC_BAD_MW2: To long string : %s", 0, i->mw2.character()));
    }
    if(ec == BEC_NO_ERROR && i->mw3.nofCharacters() > MiddleWare::MAX_LENGTH) {
        ec = BEC_BAD_MW3;
        TRACE((LOG_LEVEL_ERROR, "BackupInfo.valid() error: BEC_BAD_MW3: To long string : %s", 0, i->mw3.character()));
    }
    if(ec == BEC_NO_ERROR && i->mw4.nofCharacters() > MiddleWare::MAX_LENGTH) {
        ec = BEC_BAD_MW4;
        TRACE((LOG_LEVEL_ERROR, "BackupInfo.valid() error: BEC_BAD_MW4: To long string : %s", 0, i->mw4.character()));
    }
    if(ec == BEC_NO_ERROR && i->mw5.nofCharacters() > MiddleWare::MAX_LENGTH) {
        ec = BEC_BAD_MW5;
        TRACE((LOG_LEVEL_ERROR, "BackupInfo.valid() error: BEC_BAD_MW5; To long string : %s", 0, i->mw5.character()));
    }
    if(ec == BEC_NO_ERROR && i->mw6.nofCharacters() > MiddleWare::MAX_LENGTH) {
        ec = BEC_BAD_MW6;
        TRACE((LOG_LEVEL_ERROR, "BackupInfo.valid() error: BEC_BAD_MW6: To long string : %s", 0, i->mw6.character()));
    }
    if(ec == BEC_NO_ERROR && i->mw7.nofCharacters() > MiddleWare::MAX_LENGTH) {
        ec = BEC_BAD_MW7;
        TRACE((LOG_LEVEL_ERROR, "BackupInfo.valid() error: BEC_BAD_MW7: To long string : %s", 0, i->mw7.character()));
    }
    if(ec == BEC_NO_ERROR && i->apzHwVariant.nofCharacters() > APZHwVariant::MAX_LENGTH) {
        ec = BEC_BAD_APZ_HW_VARIANT;
        TRACE((LOG_LEVEL_ERROR, "BackupInfo.valid() error: BEC_BAD_APZ_HW_VARIANT: To long string : %s", 0, i->apzHwVariant.character()));
    }
    return ec;
}
//
//
//===========================================================================
u_int32 BackupInfo_Base::headerCnt(const SBCFile& file) const {

    newTRACE((LOG_LEVEL_INFO, "BackupInfo::headerCnt(%s)", 0, file.name()));

    u_int32 cnt = 0;
 	Info *i = reinterpret_cast<Info_Base*>(m_pdata);
    switch(file.id()) {
        case SBCFile::SDD:
            cnt = i->sddSectorInfo.nofSegments();
            break;
        case SBCFile::LDD1:
            cnt = i->ldd1SectorInfo.nofSegments();
            break;
        case SBCFile::LDD2:
            cnt = i->ldd2SectorInfo.nofSegments();
            break;
        case SBCFile::PS:
            cnt = i->psSectorInfo.nofSegments();
            break;
        case SBCFile::RS:
            cnt = i->rsSectorInfo.nofSegments();
            break;
        default:
        	break;
     }
    return cnt;
}
//
// Calculate total size for data sector
//===========================================================================
u_int64 BackupInfo_Base::sectorSizeW32s(const SBCFile& file) const {

    newTRACE((LOG_LEVEL_INFO, "BackupInfo::sectorSizeW32s(%s)", 0, file.name()));

    u_int64 size = 0;
    u_int32 segmentCnt = 0;
 	Info *i = reinterpret_cast<Info_Base*>(m_pdata);
    const void* pv = NULL;
    switch (file.id()) {
    case SBCFile::SDD:
        segmentCnt = i->sddSectorInfo.nofSegments();
        pv = sddHeader();
        break;
    case SBCFile::LDD1:
        segmentCnt = i->ldd1SectorInfo.nofSegments();
        TRACE((LOG_LEVEL_INFO, "BackupInfo::sectorSizeW32s: LDD1 SegmentCnt %d\n", 0,segmentCnt));
        pv = ldd1Header();
        break;
    case SBCFile::LDD2:
        segmentCnt = i->ldd2SectorInfo.nofSegments();
        pv = ldd2Header();
        break;
    case SBCFile::PS:
        segmentCnt = i->psSectorInfo.nofSegments();
        pv = psHeader();
        break;
    case SBCFile::RS:
        segmentCnt = i->rsSectorInfo.nofSegments();
        pv = rsHeader();
        break;
     default:
        assert(!"Unreachable code!?");
        break;
    }
    if(file == SBCFile::SDD) {
        const IndirectSegmentHeader* hdr = reinterpret_cast<const IndirectSegmentHeader*>(pv);
        for(u_int32 i = 0; i < segmentCnt; ++i)
            size += hdr[i].blockSize();
    }
    else {
        const DirectSegmentHeader* hdr = reinterpret_cast<const DirectSegmentHeader*>(pv);
        for(u_int32 i = 0; i < segmentCnt; ++i)
        {
            size += hdr[i].blockSize();
            
            if (isTRACEABLE()) {
            	std::stringstream ss;
            	ss << "BackupInfo::sectorSizeW32s: SegmentHeader[" << i << "].blocksize : "
               	   <<  hdr[i].blockSize() << "\n"
            	   << "BackupInfo::sectorSizeW32s: Totalsize : " << size << "\n";
            	TRACE((LOG_LEVEL_INFO, "%s", 0, ss.str().c_str()));
            }
        }
    }
    return size;
}
//
// friend
//===========================================================================
ostream& operator<<(ostream& os, const BackupInfo_Base& bi) {

    if(bi.m_pdata != NULL) {
		bi.print(os);
        bi.printDynamicArea(os);
    }
	return os;
}

ostream& BackupInfo_Base::print(ostream &os ) const {

     const Info_Base* pinfo = reinterpret_cast<const Info_Base*>(m_pdata);
     os << "<BACKUP INFORMATION CONTENTS:>\n"
        << "<BACKUP DATA>\n"
        << "Checksum ...................: <0x" << hex << info()->checksum << ">\n"
        << "Version.....................: " << pinfo->version << '\n'
        << "APZ version.................: " << pinfo->apzVersion << '\n'
        << "Exchange identity...........: " << pinfo->exchangeId << '\n'
        << "Generation..................: <0x" << hex << pinfo->generation << ">\n"
    
        << "Sector Info SDD.............: " << pinfo->sddSectorInfo << '\n'
        << "Sector Info LDD1............: " << pinfo->ldd1SectorInfo << '\n'
        << "Sector Info LDD2............: " << pinfo->ldd2SectorInfo << '\n'
        << "Sector Info PS..............: " << pinfo->psSectorInfo << '\n'
        << "Sector Info RS..............: " << pinfo->rsSectorInfo << "\n\n"

        << "Command log file 1..........: " << pinfo->cmdLogFile1 << '\n'
        << "Command log file 2..........: " << pinfo->cmdLogFile2 << "\n\n";

	 u_int16 type = info()->apzVersion.type();
	 u_int16 ver = info()->apzVersion.version();
	 bool apz14 = type == 214;
	 bool apz12 = type == 212 && (ver == 50 || ver == 55 || ver == 60);
	 if (apz14 || apz12) {

             os << "CPHW Dump...................: " << pinfo->mw1 << '\n'
                << "MBC Firmware on RPHMI.......: " << pinfo->mw2 << '\n'
                << "UPBB Load Modules...........: " << pinfo->mw3 << '\n'
                << "CPSB Load Modules...........: " << pinfo->mw4 << '\n'
                << "PCIH Load Modules...........: " << pinfo->mw5 << '\n'
                << "PLEX Engine Dump............: " << pinfo->mw6 << '\n'
                << "Config file for CPSB........: " << pinfo->mw7 << "\n\n";
        }
        else { // apz21240 or apz classic

             os << "Executing Micro Program.....: " << pinfo->mw1 << '\n'
                << "Micro Program in PROM.......: " << pinfo->mw2 << '\n'
                << "MBC Firmware................: " << pinfo->mw3 << '\n'
                << "System Boot Image...........: " << pinfo->mw4 << '\n'
                << "PLEX Engine Dump............: " << pinfo->mw5 << '\n'
                << "UPBB Load Modules...........: " << pinfo->mw6 << '\n'
                << "PCIH Load Modules...........: " << pinfo->mw7 << "\n\n";
        }
	return os;
}


ostream& BackupInfo_Base::printDynamicArea(ostream &os ) const {
                        
	   const Info_Base* pinfo = reinterpret_cast<const Info_Base*>(m_pdata);
       if(pinfo->sddSectorInfo.nofSegments() > 0) {
           const IndirectSegmentHeader* ish = sddHeader();
           os << "<HEADER/HEADERS SDD>\n"
               "............: <BAN><BNR><SIZEW32 (SIZEW8)><0xCHECKSUM>\n\n";
           for(unsigned int i = 0; i < pinfo->sddSectorInfo.nofSegments(); i++) {
               os << "Header[" << i << "]...............: " << ish[i] << '\n';
           }
           os << "\n";
       }
       else
           os << "<THERE ARE NO SDD SEGMENTS>\n\n";
       
       printDSHs(os, "LDD1", pinfo->ldd1SectorInfo.nofSegments(),ldd1Header());
       printDSHs(os, "LDD2", pinfo->ldd2SectorInfo.nofSegments(),ldd2Header());
       printDSHs(os, "PS", pinfo->psSectorInfo.nofSegments(), psHeader());
       printDSHs(os, "RS", pinfo->rsSectorInfo.nofSegments(), rsHeader());

    return os;
}
//
//
//
ostream& BackupInfo_Base::printDSHs(ostream& os, const char* name, u_int32 cnt, const DirectSegmentHeader* dsh) const {
    if(cnt > 0) {

        os << "<HEADER/HEADERS " << name << ">\n"
                "............: <0xADDRESS  (ADDRESS )><SIZE_W32 (SIZE_W8   )><0xCHECKSUM>\n\n";
        for(unsigned int i = 0; i < cnt; i++) {
            os << "Header[" << i << "]...: " << dsh[i] << '\n';
        }
        os << endl;
    }
    else
        os << "<THERE ARE NO " << name << " SEGMENTS>\n\n";
    return os;
}



/////////////////////////////////////////////////////////////////////////////
// class BackupInfo_V4


//
// Check validity of the BackupInfo data.
//===========================================================================
BackupInfo_Base::BI_ERROR_CODE BackupInfo_V4::valid() const {

    newTRACE((LOG_LEVEL_INFO, "BackupInfo_V4::BI_ERROR_CODE BackupInfo::valid()", 0));

    BI_ERROR_CODE ec = BEC_NO_ERROR;

    const Info_V4* i = reinterpret_cast<const Info_V4*>(m_pdata);

	if(i == 0) {
        ec = BEC_NO_DATA;
        TRACE((LOG_LEVEL_ERROR, "BackupInfo_V4.valid() error: NO_DATA\n", 0));
    }
    if(ec == BEC_NO_ERROR && i->checksum != calcCSum()) {
        ec = BEC_BAD_CSUM;
        //TRACE(("BackupInfo.valid() error: BAD_CSUM ", 0));
        TRACE((LOG_LEVEL_ERROR, "BackupInfo_V4.valid() error: BAD_CSUM. i->checksum=0x%X, calcCSum()=0x%X ", 0, i->checksum, calcCSum()));
    }
    if(ec == BEC_NO_ERROR && m_size != totalSize()) {
        ec = BEC_BAD_SIZE;
        TRACE((LOG_LEVEL_ERROR, "BackupInfo_V4.valid() error: BAD_SIZE ", 0));
    }
    
    Version currVersion(BackupInfo_V4::VERSION_MAJOR2, BackupInfo_V4::VERSION_MINOR2);  
    if (ec == BEC_NO_ERROR && i->version != currVersion) {
        ec = BEC_BAD_VERSION;
        TRACE((LOG_LEVEL_ERROR, "BackupInfo_V4.valid() error: BAD_VERSION ", 0));
    }
 
	if(ec == BEC_NO_ERROR && !i->sddSectorInfo.outputTime().valid()) {
        ec = BEC_BAD_SDD_DATETIME;
        TRACE((LOG_LEVEL_ERROR, "BackupInfo.valid() error: BAD_SDD_DATETIME - %d-%d-%d %d:%d", 0, i->sddSectorInfo.outputTime().year(),
            i->sddSectorInfo.outputTime().month(),i->sddSectorInfo.outputTime().day(),i->sddSectorInfo.outputTime().hour(),
            i->sddSectorInfo.outputTime().minute()));
	}
    if (ec == BEC_NO_ERROR) 
		ec = BackupInfo_Base::valid_others();

    if (ec == BEC_NO_ERROR) 
		ec = valid_others();

    TRACE((LOG_LEVEL_INFO, "BackupInfo_V4.valid() ec: %d", 0, ec));
    return ec;
}
//
// Check validity of the BackupInfo data.
//===========================================================================
BackupInfo_Base::BI_ERROR_CODE BackupInfo_V4::valid_others() const {

    newTRACE((LOG_LEVEL_INFO, "BackupInfo_V4::BI_ERROR_CODE BackupInfo::valid_others()", 0));

    BI_ERROR_CODE ec = BEC_NO_ERROR;
    const Info_V4* i = reinterpret_cast<const Info_V4*>(m_pdata);

    if(ec == BEC_NO_ERROR && (i->ps2SectorInfo.nofSegments() ?
            !i->ps2SectorInfo.outputTime().valid() :
            !i->ps2SectorInfo.outputTime().null())) {
        ec = BEC_BAD_PS2_DATETIME;
        TRACE((LOG_LEVEL_ERROR, "BackupInfo.valid() error: BAD_PS2_DATETIME ", 0, i->ps2SectorInfo.outputTime().year(),
            i->ps2SectorInfo.outputTime().month(),i->ps2SectorInfo.outputTime().day(),i->ps2SectorInfo.outputTime().hour(),
            i->ps2SectorInfo.outputTime().minute()));
    }

    if(ec == BEC_NO_ERROR && (i->rs2SectorInfo.nofSegments() ?
            !i->rs2SectorInfo.outputTime().valid() :
            !i->rs2SectorInfo.outputTime().null())) {
        ec = BEC_BAD_RS2_DATETIME;
        TRACE((LOG_LEVEL_ERROR, "BackupInfo.valid() error: BAD_RS2_DATETIME ", 0, i->rs2SectorInfo.outputTime().year(),
            i->rs2SectorInfo.outputTime().month(),i->rs2SectorInfo.outputTime().day(),i->rs2SectorInfo.outputTime().hour(),
            i->rs2SectorInfo.outputTime().minute()));
    }

	return ec;
}
//
//
//===========================================================================
u_int32 BackupInfo_V4::headerCnt(const SBCFile& file) const {

    newTRACE((LOG_LEVEL_INFO, "BackupInfo::headerCnt(%s)", 0, file.name()));

    u_int32 cnt = 0;
	Info_V4 *i = reinterpret_cast<Info_V4*>(m_pdata);
    switch(file.id()) {
        case SBCFile::SDD:
            cnt = i->sddSectorInfo.nofSegments();
            break;
        case SBCFile::LDD1:
            cnt = i->ldd1SectorInfo.nofSegments();
            break;
        case SBCFile::LDD2:
            cnt = i->ldd2SectorInfo.nofSegments();
            break;
        case SBCFile::PS:
            cnt = i->psSectorInfo.nofSegments();
            break;
        case SBCFile::RS:
            cnt = info_V4()->rsSectorInfo.nofSegments();
            break;
        case SBCFile::PS2:
            cnt = i->ps2SectorInfo.nofSegments();
            break;
        case SBCFile::RS2:
            cnt = i->rs2SectorInfo.nofSegments();
            break;
        default:
        	break;
    }
    return cnt;
}
//
// Calculate total size for data sector
//===========================================================================
u_int64 BackupInfo_V4::sectorSizeW32s(const SBCFile& file) const {

    newTRACE((LOG_LEVEL_INFO, "BackupInfo::sectorSizeW32s(%s)", 0, file.name()));

    u_int64 size = 0;
    u_int32 segmentCnt = 0;
	Info_V4 *i = reinterpret_cast<Info_V4*>(m_pdata);
    const void* pv = NULL;
    switch (file.id()) {
    case SBCFile::SDD:
        segmentCnt = i->sddSectorInfo.nofSegments();
        pv = sddHeader();
        break;
    case SBCFile::LDD1:
        segmentCnt = i->ldd1SectorInfo.nofSegments();
        TRACE((LOG_LEVEL_INFO, "BackupInfo::sectorSizeW32s: LDD1 SegmentCnt %d\n", 0,segmentCnt));
        pv = ldd1Header();
        break;
    case SBCFile::LDD2:
        segmentCnt = i->ldd2SectorInfo.nofSegments();
        pv = ldd2Header();
        break;
    case SBCFile::PS:
        segmentCnt = i->psSectorInfo.nofSegments();
        pv = psHeader();
        break;
    case SBCFile::RS:
        segmentCnt = i->rsSectorInfo.nofSegments();
        pv = rsHeader();
        break;
    case SBCFile::PS2:
        segmentCnt = i->ps2SectorInfo.nofSegments();
        TRACE((LOG_LEVEL_INFO, "BackupInfo::sectorSizeW32s: PS2 SegmentCnt %d\n", 0,segmentCnt));
        pv = ps2Header();
        break;
    case SBCFile::RS2:
        segmentCnt = i->rs2SectorInfo.nofSegments();
        TRACE((LOG_LEVEL_INFO, "BackupInfo::sectorSizeW32s: RS2 SegmentCnt %d\n", 0,segmentCnt));
        pv = rs2Header();
        break;
     default:
        assert(!"Unreachable code!?");
        break;
    }
    if(file == SBCFile::SDD) {
        const IndirectSegmentHeader* hdr = reinterpret_cast<const IndirectSegmentHeader*>(pv);
        for(u_int32 i = 0; i < segmentCnt; ++i)
            size += hdr[i].blockSize();
    }
    else {
        const DirectSegmentHeader* hdr = reinterpret_cast<const DirectSegmentHeader*>(pv);
        for(u_int32 i = 0; i < segmentCnt; ++i)
        {
            TRACE((LOG_LEVEL_INFO, "BackupInfo::sectorSizeW32s: SegmentHeader[%d].blocksize : %l\n", 0,i,hdr[i].blockSize()));
            size += hdr[i].blockSize();
            TRACE((LOG_LEVEL_INFO, "BackupInfo::sectorSizeW32s: Totalsize : %l\n", 0,size));
        }
    }
    return size;
}
//
///////////////////////////////////////////////////////////////////////////////
ostream& operator<<(ostream& os, const BackupInfo_V4& bi) {


    if(bi.m_pdata != NULL) {
        bi.print(os);
		bi.printDynamicArea(os);
    }
	return os;
}

ostream& BackupInfo_V4::print(ostream &os ) const {

	// BUINFO Version 3 fix area
    BackupInfo_Base::print(os); 

    const Info_V4* pinfo = reinterpret_cast<const Info_V4*>(m_pdata);

    // BUINFO Version 4 fix area
    os  << "DS Generation ..............: <0x" << hex << info_V4()->dsGeneration << ">\n"
		<< "LDD1 OM Profile ............: " << dec << pinfo->ldd1OMProfile << "\n"
		<< "     APZ Profile ...........: " << pinfo->ldd1APZProfile << "\n"
		<< "     APT Profile ...........: " << pinfo->ldd1APTProfile << "\n"
		<< "     CP ID .................: " << (int)pinfo->ldd1CpId << "\n"
		<< "LDD2 OM Profile ............: " << dec << pinfo->ldd2OMProfile << "\n"
		<< "     APZ Profile ...........: " << pinfo->ldd2APZProfile << "\n"
		<< "     APT Profile ...........: " << pinfo->ldd2APTProfile << "\n"
		<< "     CP ID .................: " << (int)pinfo->ldd2CpId << "\n"
		<< "\n"
        << "Sector Info PS2.............: " << pinfo->ps2SectorInfo << '\n'
        << "Sector Info RS2.............: " << pinfo->rs2SectorInfo << "\n\n"
		<< endl;

	return os;
}

ostream& BackupInfo_V4::printDynamicArea(ostream &os ) const {
	
	// BUINFO Version 3 dynamic area
    BackupInfo_Base::printDynamicArea(os);     

	// BUINFO Version 4 dynamic area
    const Info_V4* pinfo = reinterpret_cast<const Info_V4*>(m_pdata);
    printDSHs(os, "PS2", pinfo->ps2SectorInfo.nofSegments(), ps2Header());
    printDSHs(os, "RS2", pinfo->rs2SectorInfo.nofSegments(), rs2Header());    
	
	return os;
}

// This will need to be moved to appropriate location later
// For the time being, it is convinient to have them here

void BackupInfo_Base::replace(const BackupInfo_Base* biNew, bool smallAndLarge)
{
	newTRACE((LOG_LEVEL_INFO, "BackupInfo_Base::replace(const BackupInfo_Base* biNew, %d)", 0, smallAndLarge));

	assert(valid() == BackupInfo_Base::BEC_NO_ERROR);

	int diff = (biNew->headerSize(SBCFile::SDD) - headerSize(SBCFile::SDD)) + 
		(smallAndLarge ? (biNew->headerSize(SBCFile::LDD1) - headerSize(SBCFile::LDD2)) : 0);
	
	u_int32 ts = totalSize() + diff;
	u_int8* pdata = new u_int8[ts];
	
	// copy info part
	//memcpy(pdata, m_pdata, getInfoSize());
	memcpy(pdata, m_pdata, sizeof(BackupInfo_Base::Info));

	BackupInfo_Base biTmp(pdata, ts, false, false);

	Info_Base* biTmpInfo = biTmp.info();
	const Info_Base* biNewInfo = biNew->info();
	Info_Base* thisInfo  = reinterpret_cast<Info_Base*>(m_pdata);


	// overwrite w. new sddSectorInfo
	biTmpInfo->sddSectorInfo = biNewInfo->sddSectorInfo;
	// overwrite w. new exchangeId
	biTmpInfo->exchangeId = biNewInfo->exchangeId;

	if(smallAndLarge) {
		// overwrite w. new ldd1/ldd2 SectorInfo + cmdlogfile + exchangeId
		biTmpInfo->ldd1SectorInfo = biNewInfo->ldd1SectorInfo;
		biTmpInfo->cmdLogFile1 = biNewInfo->cmdLogFile1;
		biTmpInfo->ldd2SectorInfo = thisInfo->ldd1SectorInfo;
		biTmpInfo->cmdLogFile2 = thisInfo->cmdLogFile1;
	}

	// copy sdd headers
	memcpy(biTmp.sddHeader(), biNew->sddHeader(), biTmp.headerSize(SBCFile::SDD));

	// copy ldd1 headers from new or old file to tmp
	memcpy(biTmp.ldd1Header(), smallAndLarge ? biNew->ldd1Header() : ldd1Header(),
		biTmp.headerSize(SBCFile::LDD1));

	// copy ldd2 headers from old ldd1/ldd2 to tmp
	memcpy(biTmp.ldd2Header(), smallAndLarge ? ldd1Header() : ldd2Header(),
		biTmp.headerSize(SBCFile::LDD2));

	// copy PS & RS headers
	memcpy(biTmp.psHeader(), psHeader(),
		biTmp.headerSize(SBCFile::PS) + biTmp.headerSize(SBCFile::RS));

	// update csum
	biTmpInfo->checksum = biTmp.calcCSum();

	//m_backupInfo->setBuf(biTmp.m_backupInfo);
	delete [] m_pdata; 
	m_pdata = pdata;

	assert(valid() == BackupInfo_Base::BEC_NO_ERROR);

}


void BackupInfo_Base::replace(const BackupInfo_Base* biNew, bool smallAndLarge, int progress) 
{
    newTRACE((LOG_LEVEL_INFO, "Recovery; BackupInfo_Base::replace(const BackupInfo_Base* biNew, %d, %d)", 0, smallAndLarge, progress));
    
    Info_Base* thisInfo  = reinterpret_cast<Info_Base*>(m_pdata);
    const Info_Base* biNewInfo = biNew->info();    

    // This should not be here    
    if (!biNewInfo->sddSectorInfo.nofSegments() && !thisInfo->ldd1SectorInfo.nofSegments())
        return;
    
    bool adMarker = (progress & MarkerCreated) == MarkerCreated;        
    bool sddUpdated = (progress & SDDmoved) == SDDmoved;    
    bool lddUpdated = smallAndLarge && ((progress & LDD1renamed) == LDD1renamed);
    
    int diff = sddUpdated ? biNew->headerSize(SBCFile::SDD) - headerSize(SBCFile::SDD) : 0;
    diff -= lddUpdated ? headerSize(SBCFile::LDD2) : 0;
        
    u_int32 ts = totalSize() + diff;
    u_int8* pdata = new u_int8[ts];
    memcpy(pdata, m_pdata, sizeof(BackupInfo_Base::Info));
    BackupInfo_Base biTmp(pdata, ts, false, false);
    Info_Base* biTmpInfo = biTmp.info();

    if (sddUpdated) {
        biTmpInfo->sddSectorInfo = biNewInfo->sddSectorInfo;
        biTmpInfo->exchangeId = biNewInfo->exchangeId;
    }

    SectorInfo nullSector;    
    if(lddUpdated) {
        // overwrite w. new ldd1/ldd2 SectorInfo + cmdlogfile + exchangeId
        biTmpInfo->ldd1SectorInfo = nullSector;
        biTmpInfo->cmdLogFile1 = 0;
        biTmpInfo->ldd2SectorInfo = thisInfo->ldd1SectorInfo;
        biTmpInfo->cmdLogFile2 = thisInfo->cmdLogFile1;
    }

    if (adMarker || sddUpdated || lddUpdated)
        biTmpInfo->admarker = 1;
        
    memcpy(biTmp.sddHeader(), sddUpdated? biNew->sddHeader() : sddHeader(), 
        biTmp.headerSize(SBCFile::SDD));
    
    // TODO: copy ldd1 headers from old ldd1 to tmp
    memcpy(biTmp.ldd1Header(), ldd1Header(), biTmp.headerSize(SBCFile::LDD1));

    // copy ldd2 headers from old ldd1/ldd2 to tmp
    memcpy(biTmp.ldd2Header(), lddUpdated ? ldd1Header() : ldd2Header(),
        biTmp.headerSize(SBCFile::LDD2));

    // copy PS & RS headers
    memcpy(biTmp.psHeader(), psHeader(),
        biTmp.headerSize(SBCFile::PS) + biTmp.headerSize(SBCFile::RS));

    // update csum
    biTmpInfo->checksum = biTmp.calcCSum();

    //m_backupInfo->setBuf(biTmp.m_backupInfo);
    delete [] m_pdata; 
    m_pdata = pdata;

}


void BackupInfo_V4::replace(const BackupInfo_Base* biNew, bool smallAndLarge)
{
	newTRACE((LOG_LEVEL_INFO, "BackupInfo_V4::replace(const BackupInfo_Base* biNew, %d)", 0, smallAndLarge));

	assert(valid() == BackupInfo_Base::BEC_NO_ERROR);

	int diff = biNew->headerSize(SBCFile::SDD) - headerSize(SBCFile::SDD);
	TRACE((LOG_LEVEL_INFO, "size of sdd diff %d", 0, diff));
	if (smallAndLarge) {
		int psDiff = biNew->headerSize(SBCFile::PS) - headerSize(SBCFile::PS2);
		int rsDiff = biNew->headerSize(SBCFile::RS) - headerSize(SBCFile::RS2);
		int lddDiff = biNew->headerSize(SBCFile::LDD1) - headerSize(SBCFile::LDD2);
		//cout << "psDiff : " << psDiff << endl;
		//cout << "rsDiff : " << rsDiff << endl;
		//cout << "lddDiff : " << lddDiff << endl;
		diff += psDiff + rsDiff + lddDiff;

		TRACE((LOG_LEVEL_INFO, "size of ps diff %d", 0, psDiff));
		TRACE((LOG_LEVEL_INFO, "size of rs diff %d", 0, rsDiff));
		TRACE((LOG_LEVEL_INFO, "size of ldd diff %d", 0, lddDiff));
		TRACE((LOG_LEVEL_INFO, "size of tot diff %d", 0, diff));

	}
	u_int32 ts = totalSize() + diff;
	u_int8* pdata = new u_int8[ts];
	TRACE((LOG_LEVEL_INFO, "Cur file size %d", 0, totalSize()));
	TRACE((LOG_LEVEL_INFO, "New file size %d", 0, totalSize() + diff));

	// copy info part
	memcpy(pdata, m_pdata, sizeof(BackupInfo_V4::Info_V4));
	
	BackupInfo_V4 biTmp(pdata, ts, false, false);

	Info_V4* biTmpInfo = biTmp.info_V4();
	const Info_V4* biNewInfo = biNew->info_V4();
	Info_V4* thisInfo  = reinterpret_cast<Info_V4*>(m_pdata);

	// overwrite w. new sddSectorInfo
	biTmpInfo->sddSectorInfo = biNewInfo->sddSectorInfo;
	// overwrite w. new exchangeId
	biTmpInfo->exchangeId = biNewInfo->exchangeId;

	if(smallAndLarge) {
		// overwrite w. new ldd1/ldd2 SectorInfo + cmdlogfile + exchangeId
		biTmpInfo->ldd1SectorInfo = biNewInfo->ldd1SectorInfo;
		biTmpInfo->cmdLogFile1 =	biNewInfo->cmdLogFile1;
		biTmpInfo->ldd2SectorInfo = thisInfo->ldd1SectorInfo;
		biTmpInfo->cmdLogFile2 =	thisInfo->cmdLogFile1;

		biTmpInfo->psSectorInfo = biNewInfo->psSectorInfo;
		biTmpInfo->rsSectorInfo = biNewInfo->rsSectorInfo;
		biTmpInfo->ps2SectorInfo = thisInfo->psSectorInfo;
		biTmpInfo->rs2SectorInfo = thisInfo->rsSectorInfo;

		biTmpInfo->dsGeneration = biNewInfo->dsGeneration;

		biTmpInfo->ldd1APTProfile = biNewInfo->ldd1APTProfile;
		biTmpInfo->ldd1APZProfile = biNewInfo->ldd1APZProfile;
		biTmpInfo->ldd1OMProfile =  biNewInfo->ldd1OMProfile;
		biTmpInfo->ldd1CpId =		biNewInfo->ldd1CpId;

		biTmpInfo->ldd2APTProfile = thisInfo->ldd1APTProfile;
		biTmpInfo->ldd2APZProfile = thisInfo->ldd1APZProfile;
		biTmpInfo->ldd2OMProfile =  thisInfo->ldd1OMProfile;
		biTmpInfo->ldd2CpId =		thisInfo->ldd1CpId;
	}

	// copy sdd headers
	memcpy(biTmp.sddHeader(), biNew->sddHeader(), biTmp.headerSize(SBCFile::SDD));

	// copy ldd1 headers from new or old file to tmp
	memcpy(biTmp.ldd1Header(), smallAndLarge ? biNew->ldd1Header() : ldd1Header(),
		biTmp.headerSize(SBCFile::LDD1));

	// copy ldd2 headers from old ldd1/ldd2 to tmp
	memcpy(biTmp.ldd2Header(), smallAndLarge ? ldd1Header() : ldd2Header(),
		biTmp.headerSize(SBCFile::LDD2));

	// copy PS & RS headers
	memcpy(biTmp.psHeader(), smallAndLarge ? biNew->psHeader() : psHeader(), 
		biTmp.headerSize(SBCFile::PS) + biTmp.headerSize(SBCFile::RS));

	// copy PS2 & RS2 headers
	memcpy(biTmp.ps2Header(), smallAndLarge ? psHeader() : ps2Header(), 
		biTmp.headerSize(SBCFile::PS2) + biTmp.headerSize(SBCFile::RS2));

	// update csum
	biTmpInfo->checksum = biTmp.calcCSum();
	
	delete [] m_pdata; 
	m_pdata = pdata;

	//if (valid() != BackupInfo_Base::BEC_NO_ERROR)
	//	cout << "Wrong in replace" << endl;

	assert(valid() == BackupInfo_Base::BEC_NO_ERROR);

	//cout << "End of replace" << endl;
}

void BackupInfo_V4::replace(const BackupInfo_Base* biNew, bool smallAndLarge, int progress)
{
    newTRACE((LOG_LEVEL_INFO, "Recovery; BackupInfo_V4::replace(const BackupInfo_Base* biNew, %d, %d)", 0, smallAndLarge, progress));

    Info_V4* thisInfo = reinterpret_cast<Info_V4*>(m_pdata);
    const Info_V4* biNewInfo = biNew->info_V4();
    
    if (!thisInfo->ldd1SectorInfo.nofSegments())    
       return;
    bool adMarker = (progress & MarkerCreated) == MarkerCreated;    
    bool sddUpdated = (progress & SDDmoved) == SDDmoved;
    bool lddUpdated = smallAndLarge && ((progress & LDD1renamed) == LDD1renamed);
    bool psUpdated = smallAndLarge && ((progress & PSrenamed) == PSrenamed);
    bool rsUpdated = smallAndLarge && ((progress & RSrenamed) == RSrenamed);
   
    int diff = sddUpdated ? biNew->headerSize(SBCFile::SDD) - headerSize(SBCFile::SDD) : 0;
    diff -= lddUpdated ? ((progress & LDD1moved) ? (headerSize(SBCFile::LDD2) - biNew->headerSize(SBCFile::LDD1)) : headerSize(SBCFile::LDD2)) : 0;
    diff -= psUpdated ? ((progress & PSmoved) ? (headerSize(SBCFile::PS2) - biNew->headerSize(SBCFile::PS)) : headerSize(SBCFile::PS2)) : 0;
    diff -= rsUpdated ? ((progress & RSmoved) ? (headerSize(SBCFile::RS2) - biNew->headerSize(SBCFile::RS)) : headerSize(SBCFile::RS2)) : 0;

    u_int32 ts = totalSize() + diff;
    u_int8* pdata = new u_int8[ts];
    memcpy(pdata, m_pdata, sizeof(BackupInfo_V4::Info_V4));
    BackupInfo_V4 biTmp(pdata, ts, false, false);
    Info_V4* biTmpInfo = biTmp.info_V4();

/*
    if (sddUpdated) {
        biTmpInfo->sddSectorInfo = biNewInfo->sddSectorInfo;
        biTmpInfo->exchangeId = biNewInfo->exchangeId;
    }
*/
    biTmpInfo->sddSectorInfo = biNewInfo->sddSectorInfo;
    biTmpInfo->exchangeId = biNewInfo->exchangeId;
    
    SectorInfo nullSector;
    if(lddUpdated) {
        // overwrite w. new ldd1/ldd2 SectorInfo + cmdlogfile + exchangeId
        biTmpInfo->ldd1SectorInfo = progress & LDD1moved ? biNewInfo->ldd1SectorInfo : nullSector;
        biTmpInfo->cmdLogFile1 = progress & LDD1moved ? biNewInfo->cmdLogFile1 : 0;
        biTmpInfo->ldd2SectorInfo = thisInfo->ldd1SectorInfo;
        biTmpInfo->cmdLogFile2 = thisInfo->cmdLogFile1;
    }

    if(psUpdated) {
        // overwrite w. new ldd1/ldd2 SectorInfo + cmdlogfile + exchangeId
        biTmpInfo->psSectorInfo = progress & PSmoved ? biNewInfo->psSectorInfo : nullSector;
        biTmpInfo->ps2SectorInfo = thisInfo->psSectorInfo;
    }
    
    if(rsUpdated) {
        // overwrite w. new ldd1/ldd2 SectorInfo + cmdlogfile + exchangeId
        biTmpInfo->rsSectorInfo = progress & RSmoved ? biNewInfo->rsSectorInfo : nullSector;
        biTmpInfo->rs2SectorInfo = thisInfo->rsSectorInfo;
    }

    if(smallAndLarge) {

        biTmpInfo->dsGeneration = biNewInfo->dsGeneration;

        biTmpInfo->ldd1APTProfile = biNewInfo->ldd1APTProfile;
        biTmpInfo->ldd1APZProfile = biNewInfo->ldd1APZProfile;
        biTmpInfo->ldd1OMProfile =  biNewInfo->ldd1OMProfile;
        biTmpInfo->ldd1CpId =        biNewInfo->ldd1CpId;

        biTmpInfo->ldd2APTProfile = thisInfo->ldd1APTProfile;
        biTmpInfo->ldd2APZProfile = thisInfo->ldd1APZProfile;
        biTmpInfo->ldd2OMProfile =  thisInfo->ldd1OMProfile;
        biTmpInfo->ldd2CpId =        thisInfo->ldd1CpId;
    }

    if ( adMarker || sddUpdated || lddUpdated || psUpdated || rsUpdated)
        biTmpInfo->admarker = 1;

    // copy sdd headers
    memcpy(biTmp.sddHeader(), sddUpdated ? biNew->sddHeader() : sddHeader(), biTmp.headerSize(SBCFile::SDD));

    // copy ldd1 headers from new or old file to tmp
    memcpy(biTmp.ldd1Header(), smallAndLarge && (progress & LDD1moved) ? biNew->ldd1Header() : ldd1Header(), 
                              biTmp.headerSize(SBCFile::LDD1));

    // copy ldd2 headers from old ldd1/ldd2 to tmp
    memcpy(biTmp.ldd2Header(), lddUpdated ? ldd1Header() : ldd2Header(),  
        biTmp.headerSize(SBCFile::LDD2));

    // copy PS & RS headers
    memcpy(biTmp.psHeader(), smallAndLarge && (progress & PSmoved) ? biNew->psHeader() : psHeader(), 
        biTmp.headerSize(SBCFile::PS) + biTmp.headerSize(SBCFile::RS));

    // copy PS2 & RS2 headers
    memcpy(biTmp.ps2Header(), (psUpdated || rsUpdated) ? psHeader() : ps2Header(), 
        biTmp.headerSize(SBCFile::PS2) + biTmp.headerSize(SBCFile::RS2));

    // update csum
    biTmpInfo->checksum = biTmp.calcCSum();

    delete [] m_pdata; 
    m_pdata = pdata;

    assert(valid() == BackupInfo_Base::BEC_NO_ERROR);
}


#if 0
void BackupInfo_V4::removeLDD1()
{
    newTRACE((LOG_LEVEL_INFO, "BackupInfo_V4::removeLDD1()", 0));
    assert(valid() == BackupInfo_Base::BEC_NO_ERROR);
    
    int lddDiff = headerSize(SBCFile::LDD1);
    TRACE((LOG_LEVEL_INFO, "XXXXXXXX size of ldd diff %d", 0, lddDiff));
    u_int32 ts = totalSize() - lddDiff;
    u_int8* pdata = new u_int8[ts];
    
    TRACE((LOG_LEVEL_INFO, "XXXXXXXXXX Cur file size %d", 0, totalSize()));
    TRACE((LOG_LEVEL_INFO, "XXXXXXXXX New file size %d", 0, totalSize() - lddDiff));
    
   // copy info part
    memcpy(pdata, m_pdata, sizeof(BackupInfo_V4::Info_V4));
    
    BackupInfo_V4 biTmp(pdata, ts, false, false);

    Info_V4* biTmpInfo = biTmp.info_V4(); 

    biTmpInfo->ldd1SectorInfo.nofSegments(0);
    biTmpInfo->ldd1SectorInfo.outputTime().setNull();
    biTmpInfo->filler[0] = 1;
    
    //copy header
    // copy sdd headers
    memcpy(biTmp.sddHeader(), sddHeader(), biTmp.headerSize(SBCFile::SDD));

    // copy ldd1 headers from new or old file to tmp
    memcpy(biTmp.ldd1Header(), ldd1Header(),
        biTmp.headerSize(SBCFile::LDD1));

    // copy ldd2 headers from old ldd1/ldd2 to tmp
    memcpy(biTmp.ldd2Header(), ldd2Header(),
        biTmp.headerSize(SBCFile::LDD2));

    // copy PS & RS headers
    memcpy(biTmp.psHeader(), psHeader(), 
        biTmp.headerSize(SBCFile::PS) + biTmp.headerSize(SBCFile::RS));

    // copy PS2 & RS2 headers
    memcpy(biTmp.ps2Header(), ps2Header(), 
        biTmp.headerSize(SBCFile::PS2) + biTmp.headerSize(SBCFile::RS2));

    // update csum
    biTmpInfo->checksum = biTmp.calcCSum();

    delete [] m_pdata;
    m_pdata = pdata;

    assert(valid() == BackupInfo_Base::BEC_NO_ERROR);
}

#endif

///////////////////////////////////////////////////////////////////////////////
