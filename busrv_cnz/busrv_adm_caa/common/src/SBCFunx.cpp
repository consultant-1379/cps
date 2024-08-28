/*
.NAME
	SBCFUNX -
.LIBRARY 3C++
.PAGENAME SBCFUNX
.HEADER  CPS
.LEFT_FOOTER Ericsson Utvecklings AB
.INCLUDE SBCList.h

.COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2001. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

.DESCRIPTION
   Functions on a SBC.

.ERROR HANDLING
   -

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
   2002-04-03 by UAB/UKY/SF Lillemor Pettersson (uablan)

.LINKAGE
	-

.SEE ALSO

*/

#include "stdafx.h"

#include "SBCFunx.h"

#include "APBackupInfo.h"
#include "buap.h"
#include "CodeException.h"
#include "Config.h"
#include "filefunx.h"
#include "Win32Exception.h"

#include <assert.h>
#include <iostream>
//
// rename a RELFSWi to RELFSWj
// 
// throws: CodeExceptions and Win32Exceptions
//
void SBCFunx::rename(const u_int16 curr_backupId, const u_int16 backupId)
{
	string backupDir = Config::instance().sbcDir(curr_backupId); // may throw config error
	string newbackupDir = Config::instance().sbcDir(backupId);  // -"-
	if (MoveFile(backupDir.c_str(), newbackupDir.c_str())) 
	{
		THROW_WIN32_CODE(GetLastError());
	}
}
//
// rename a RELFSWi to RELFSWX/Y
// 
// throws: CodeExceptions and Win32Exceptions
//
void SBCFunx::rename(const u_int16 curr_backupId, const char *name)
{
	string backupDir = Config::instance().sbcDir(curr_backupId); 
	string newbackupDir = Config::instance().relvolumsw();
	newbackupDir.append(name);
	if(MoveFile(backupDir.c_str(), newbackupDir.c_str()))
	{
		THROW_WIN32_CODE(GetLastError());
	}
}
//
// rename a RELFSWX/Y to RELFSWi
// 
// throws: CodeExceptions and Win32Exceptions
//
void SBCFunx::rename(const char *name, const u_int16 backupId)
{
	string backupDir = Config::instance().relvolumsw();
	backupDir.append(name);
	string newbackupDir = Config::instance().sbcDir(backupId); 
	if (MoveFile(backupDir.c_str(), newbackupDir.c_str()))
	{
		THROW_WIN32_CODE(GetLastError());
	}
}
//
// reserve the existing subfiles in a SBC
//
// throws: CodeExceptions
//
void SBCFunx::reserve(u_int16 backupId, SBCHandles &h_sbc)
{
	BackupInfo::Info info;

	try
	{
		// get path to SBC
		string backupDir = Config::instance().sbcDir(backupId); 

		// look at buinfo to se which files to reserve
		string subfilename = backupDir + buap::FILENAME[BUPDefs::BACKUP_INFO];

		// map buinfo into the APBackupInfo object
		APBackupInfo bi(backupDir.c_str());
		if(bi.consistencyCheck())
			info = *bi.info(); // copy info part
		else
			throw BUPMsg::BAD_BACKUPINFO_FILE;

		if (info.sddSectorInfo.nofSegments > 0)
		{
			subfilename = backupDir + buap::FILENAME[BUPDefs::SDD];
			h_sbc.sdd = CreateFile(subfilename.c_str(), GENERIC_READ | GENERIC_WRITE,0, 
											NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (h_sbc.sdd == INVALID_HANDLE_VALUE)
			{
				THROW_WIN32("Open failed for sdd", GetLastError());
			}
		}

		if (info.ldd1SectorInfo.nofSegments > 0)
		{
			subfilename = backupDir + buap::FILENAME[BUPDefs::LDD1];
			h_sbc.ldd1 = CreateFile(subfilename.c_str(), GENERIC_READ | GENERIC_WRITE,0, 
										NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (h_sbc.ldd1 == INVALID_HANDLE_VALUE)
			{
				THROW_WIN32("Open failed for ldd1", GetLastError());
			}
		}

		if (info.ldd2SectorInfo.nofSegments > 0)
		{
			subfilename = backupDir + buap::FILENAME[BUPDefs::LDD2];
			h_sbc.ldd2 = CreateFile(subfilename.c_str(), GENERIC_READ | GENERIC_WRITE,0, 
											NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (h_sbc.ldd2 == INVALID_HANDLE_VALUE)
			{
				THROW_WIN32("Open failed for ldd2", GetLastError());
			}
		}

		if (info.psSectorInfo.nofSegments > 0)
		{
			subfilename = backupDir + buap::FILENAME[BUPDefs::PS];
			h_sbc.ps = CreateFile(subfilename.c_str(), GENERIC_READ | GENERIC_WRITE,0, 
											NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (h_sbc.ps == INVALID_HANDLE_VALUE)
			{
				THROW_WIN32("Open failed for ps", GetLastError());
			}
		}

		if (info.ldd1SectorInfo.nofSegments > 0)
		{
			subfilename = backupDir + buap::FILENAME[BUPDefs::RS];
			h_sbc.rs = CreateFile(subfilename.c_str(), GENERIC_READ | GENERIC_WRITE,0, 
											NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (h_sbc.rs == INVALID_HANDLE_VALUE)
			{
				THROW_WIN32("Open failed for rs", GetLastError());
			}
		}
	}
	catch(Win32Exception& x) {
		(void) x;
		switch (x.errcode()){
			case ERROR_FILE_NOT_FOUND: 
				throw CodeException::REQ_FILE_MISSING;
				break;
			case ERROR_ACCESS_DENIED:
				throw BUPMsg::DIR_RESERVE_FAILED;
				break;				
			default:
				throw BUPMsg::INTERNAL_ERROR;
				break;
		}
	}
}

void SBCFunx::unreserve(SBCHandles &h_sbc)
{
	h_sbc.sdd.close();
	h_sbc.ldd1.close();
	h_sbc.ldd2.close();
	h_sbc.ps.close();
	h_sbc.rs.close();
}
//
// 
//====================================================================
void SBCFunx::getSBCDateTime(u_int16 backupId, Date &date, Time &time)
{

	u_int16 centuary;
	BackupInfo::Info info;

	// Get output time for backup and convert it to a Date and a Time object
	//-----------------------------------------------------------------------
	string backupDir = Config::instance().sbcDir(backupId); // may throw config error  (codeExe) 
	if(!FileFunx::dirExists(backupDir.c_str()))
		throw CodeException::REQ_SBC_NOT_FOUND;

	// Mapp buinfo into the APBackupInfo object
	APBackupInfo bi(backupDir.c_str()); // may throw bad buinfo file (codeEx.)
	if(bi.consistencyCheck())
		info = *bi.info(); // copy info part
	else
		throw CodeException::BAD_BACKUPINFO_FILE;
	BUPDefs::DateTime outputTime = bi.info()->sddSectorInfo.outputTime;

	if (outputTime.year < 70)
		centuary = 20;
	else
		centuary =19;

	date = Date(centuary, outputTime.year, outputTime.month, outputTime.day);
	time = Time(outputTime.hour, outputTime.minute, outputTime.second);
}

//
// Copy the content of an SBC to another
//
void SBCFunx::copySBC(u_int16 sourceSBCId, u_int16 destSBCId, bool overwrite)
{
	printf("Not yet implemented\n");
	printf("sourceId : %d, destId %d\n", sourceSBCId, destSBCId);
}
