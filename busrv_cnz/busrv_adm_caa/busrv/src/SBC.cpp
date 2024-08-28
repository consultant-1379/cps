/*
NAME
	File_name:SBC.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

The Copyright to the computer program(s) herein is the property of Ericsson
Utvecklings AB, Sweden.
The program(s) may be used and/or copied only with the written permission from
Ericsson Utvecklings AB or in accordance with the terms and conditions
stipulated in the agreement/contract under which the program(s) have been
supplied.

DESCRIPTION
	Handles misc. funx relating to the SBC dir and fileset.

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
	2002-05-17 by U/Y/SF Anders Gillgren (qabgill, tag :ag)

SEE ALSO
	-

Revision history
----------------
2002-05-17 qabgill Created
2002-10-31 uablan  Add ACS_Trace

*/


#include "SBC.h"

#include "APBackupInfo.h"
#include "CodeException.h"
#include "Config.h"
#include "DateTime.h"
#include "EventReporter.h"
#include "FileFunx.h"
#include "LinuxException.h"
#include "CPS_BUSRV_Trace.h"

#ifndef LOCAL_BUILD
#include "EnvFMS.h"
#include "fms_cpf_file.h"
#endif

#include <cassert>
//
// ctor 1
//===========================================================================
#ifdef LOCAL_BUILD
SBC::SBC() : m_sbcDir(false)
{
#else
SBC::SBC() : m_sbcDir(true)
{
#endif
	newTRACE((LOG_LEVEL_INFO, "SBC::SBC()", 0));
}

//
// ctor 2
//===========================================================================
#ifdef LOCAL_BUILD
SBC::SBC(SBCId sbcId, CPID cpId, bool reserveDir) : m_sbcDir(sbcId, false, reserveDir, cpId )
{
#else
SBC::SBC(SBCId sbcId, CPID cpId, bool reserveDir) : m_sbcDir(sbcId, true, reserveDir, cpId )
{
#endif
	newTRACE((LOG_LEVEL_INFO, "SBC::SBC(%d, %d, %d)", 0, sbcId.id(), reserveDir, cpId));
}

SBC::~SBC()
{
    newTRACE((LOG_LEVEL_INFO, "SBC::~SBC()", 0));
}
//
//
//===========================================================================
size_t SBC::fileCnt() const {
	return 0; //TODO:ag impl if needed
}

//
// swap two valid SBCs
//===========================================================================
void SBC::swap(SBC& o, SBCId tmpId, bool overwriteTmp, CPID cpId)
{

	newTRACE((LOG_LEVEL_INFO, "SBC::swap(%d, %d, %d, %d)", 0, o.id().id(), tmpId.id(), overwriteTmp, cpId));

	assert(!m_sbcDir.id().isTmp() && !o.id().isTmp());
	string tmpDir = Config::instance().sbcDir(tmpId, cpId);

	// Check tmp file
#ifdef LOCAL_BUILD
	if (FileFunx::dirExists(tmpDir.c_str())) {
#else
	if (EnvFMS::dirExists(tmpId.id(), cpId)) {
#endif			
		if(!overwriteTmp) {
			TRACE((LOG_LEVEL_ERROR, "Temporary directory %s already exists", 0, tmpDir.c_str()));
			THROW_XCODE("Temporary directory (" << tmpDir.c_str() << ") already exists.", CodeException::TMP_SBC_EXISTS);
		}
		else {
#ifdef LOCAL_BUILD
			FileFunx::cleanDir(tmpDir.c_str());
#else
			EnvFMS::cleanDir(tmpId.id(), cpId);
#endif
		}
	}

	// move other to tmp
	SBCId destId = o.id();
	o.move(tmpId, false, cpId);

	// move "me" to other
	SBCId myId = id();
	move(destId, false, cpId);

	// move other to "old me"
	o.move(myId, false, cpId);
}
//
//
// 1 == greater(later), 0 == equal, -1 == less
//===========================================================================
int SBC::cmpDateTime(const SBC& o, CPID cpId) const
{

	newTRACE((LOG_LEVEL_INFO, "SBC::cmpDateTime(%d, %d)", 0, o.id().id(), cpId));

	APBackupInfo bi(dir(cpId).c_str());
	APBackupInfo biOther(o.dir(cpId).c_str());
	
	// use sdd sector for time compare
	return bi.info()->sddSectorInfo.outputTime().cmp(biOther.info()->sddSectorInfo.outputTime());
}
//
//
//===========================================================================
SBC::BI_QUERY SBC::biQuery(CPID cpId) const {

	newTRACE((LOG_LEVEL_INFO, "SBC::biQuery(%d)", 0, cpId));

	BI_QUERY query = BQ_UNKNOWN_ERROR;
	try {
		APBackupInfo bi(dir(cpId).c_str());
		if(BackupInfo_V4::BEC_NO_ERROR == bi.valid())
			query = BQ_NO_ERROR;
	}
	catch(const CodeException& x) {
		switch(x.errcode()) {
		case CodeException::BAD_BACKUPINFO:
			query = BQ_CONTENT_ERROR;
			break;
		case CodeException::BUINFO_ACCESS_DENIED:
			query = BQ_ACCESS_ERROR;
			break;
		case CodeException::BUINFO_MISSING:
			query = BQ_FILE_NOT_FOUND;
			break;
		default:
			// leave BQ_UNKNOWN_ERROR
			break;
		}
	}
	return query;
}

/////////////////////////////////////////////////////////////////////////////

//
// ctor 1
//===========================================================================
SBC_V4::SBC_V4() : m_sbcDir(true) { 
    newTRACE((LOG_LEVEL_INFO, "SBC_V4::SBC_V4()", 0));
}

//
// ctor 2
//===========================================================================
SBC_V4::SBC_V4(SBCId sbcId, const Config::CLUSTER &c, bool reserveDir)
: m_sbcDir(sbcId, c, true, reserveDir ), m_cluster(c) { 
    newTRACE((LOG_LEVEL_INFO, "SBC_V4::SBC_V4(%d, , CLUSTER, %d)", 0, sbcId.id(), reserveDir));
}

//
//
//===========================================================================
size_t SBC_V4::fileCnt() const {
    return 0; //XXX:ag impl if needed
}

//
// swap two valid SBCs
//===========================================================================
void SBC_V4::swap(SBC_V4& o, SBCId tmpId, bool overwriteTmp) {

    newTRACE((LOG_LEVEL_INFO, "SBC_V4::swap(%d, %d, %d)", 0, o.id().id(), tmpId.id(),overwriteTmp));

    assert(!m_sbcDir.id().isTmp() && !o.id().isTmp());
    string tmpDir = Config::instance().sbcDir(tmpId, m_cluster);

    // check tmp file
    if(EnvFMS::dirExists(tmpId.id(), m_cluster)) {
        if(!overwriteTmp) {
            THROW_XCODE("Temporary directory (" << tmpDir.c_str() << ") already exists.", CodeException::TMP_SBC_EXISTS);
        }
        else
            EnvFMS::cleanDir(tmpId.id(), m_cluster);
    }

    // move other to tmp
    SBCId destId = o.id();
    o.move(tmpId, false);

    // move "me" to other
    SBCId myId = id();
    move(destId, false);

    // move other to "old me"
    o.move(myId, false);
}
//
// 1 == greater(later), 0 == equal, -1 == less
//===========================================================================
int SBC_V4::cmpDateTime(const SBC_V4& o) const {

    newTRACE((LOG_LEVEL_INFO, "SBC_V4::cmpDateTime(%d)", 0, o.id().id()));

    APBackupInfo bi(dir().c_str());
    APBackupInfo biOther(o.dir().c_str());
    
    // use sdd sector for time compare
    return bi.info()->sddSectorInfo.outputTime().cmp(
        biOther.info()->sddSectorInfo.outputTime());
}

//
//
//===========================================================================
SBC_V4::BI_QUERY SBC_V4::biQuery() const {

    newTRACE((LOG_LEVEL_INFO, "SBC_V4::biQuery()", 0));

    BI_QUERY query = BQ_UNKNOWN_ERROR;
    try {
        APBackupInfo bi(dir().c_str());
        if(BackupInfo_Base::BEC_NO_ERROR == bi.valid())
            query = BQ_NO_ERROR;
    }
    catch(const CodeException& x) {
        switch(x.errcode()) {
        case CodeException::BAD_BACKUPINFO:
            query = BQ_CONTENT_ERROR;
            break;
        case CodeException::BUINFO_ACCESS_DENIED:
            query = BQ_ACCESS_ERROR;
            break;
        case CodeException::BUINFO_MISSING:
            query = BQ_FILE_NOT_FOUND;
            break;
        default:
            // leave BQ_UNKNOWN_ERROR
            break;
        }
    }
    return query;
}


