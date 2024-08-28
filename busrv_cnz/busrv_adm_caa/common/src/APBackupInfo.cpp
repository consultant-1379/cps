/*
NAME
   File_name:APBackupInfo.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2001. All rights reserved.

The Copyright to the computer program(s) herein is the property of Ericsson
Utvecklings AB, Sweden.
The program(s) may be used and/or copied only with the written permission from
Ericsson Utvecklings AB or in accordance with the terms and conditions
stipulated in the agreement/contract under which the program(s) have been
supplied.

.DESCRIPTION
   AP specific routines for BackupInfo.

DOCUMENT NO
   190 89-CAA 109 0387

AUTHOR 
   2001-06-26 by U/Y/SF Anders Gillgren (qabgill, tag :ag)

SEE ALSO 
   BackupInfo in the apcp_common code.


Revision history
----------------
2001-06-26 Created   qabgill
2010-05-23 xdtthng   Modified for SSI by XDT/DEK/xdtthng

*/


#include "APBackupInfo.h"

//#include "AutoHandle.h"
#include "buap.h"
#include "CodeException.h"
#include "CPS_BUSRV_Trace.h"
#include "FileFunx.h"
#include "SBCFile.h"
#include "LinuxException.h"
#include "Config.h"

#include <cstdio>
#include <assert.h>

#include <limits.h>
#include <string>
#include <fstream>

#include <error.h>

using namespace buap;
using namespace std;

// This is only meant to be used here
//
class AutoIfstream : public ifstream
{
public:
    AutoIfstream() : ifstream() {}
    AutoIfstream(const char *n, ios_base::openmode mode = ios_base::in) : ifstream(n, mode) {}
    ~AutoIfstream() { close(); }
};


/////////////////////////////////////////////////////////////////////////////////


APBackupInfo::APBackupInfo(const char* path) : m_backupInfo(NULL)
{
	newTRACE((LOG_LEVEL_INFO, "APBackupInfo::APBackupInfo(%s)", 0, path));	//mnst

	try {
		string filename;
		if(path) {
			assert(path[strlen(path) - 1] == '/' || path[strlen(path) - 1] == '\\');
			filename = path; 
            filename += SBCFile::name(SBCFile::BACKUP_INFO);
		}
		else
			filename = SBCFile::name(SBCFile::BACKUP_INFO);

		struct stat buffer;             // Won't compile without struct even in C++
        int res = stat(filename.c_str(), &buffer);    // On success returns 0 else -1

		if (res == -1) {
            int error = errno;
		    TRACE((LOG_LEVEL_ERROR, "Backup info does not exist; errno <%d>", 0, error));
			THROW_LINUX("Backup info does not exist", error);
		}
		
		// allocate data
		if(buffer.st_size < static_cast<off_t>(sizeof(BackupInfo_Base::Info))) {
		    TRACE((LOG_LEVEL_ERROR, "Invalid BackupInfo file size, %d", 0, buffer.st_size));
			THROW_XCODE("Invalid BackupInfo file size.", CodeException::BAD_BACKUPINFO);
		}

		u_int8 *pdata = new u_int8[buffer.st_size];
		AutoIfstream file(filename.c_str(), ios::in|ios::binary);
				
		file.read(reinterpret_cast<char *> (pdata), buffer.st_size);
		
		// read data
		//if(!FileFunx::ReadFileX2(hfile, pdata, size)) {
		if (file.fail() || file.bad()) {
			delete [] pdata;
			THROW_XCODE("Unexpected EOF in BackupInfo.", CodeException::BAD_BACKUPINFO);
		}

		//cout << "THNG: inside APBackupInfo constructor, filename is: " << filename << endl;

		// This is needed to decide if BUINFO version 3 or version 4 is created
        BackupInfo_Base::Info_Base *bi = reinterpret_cast<BackupInfo_Base::Info_Base *>(pdata);
		
		// THN: The commented out line is wrong.
		// Todo: take care of Version(4, 0) if deemed not a good practice
		//if (Config::instance().getAPZProfile() > 0 && bi->version == Version(4, 0)) {
		if (bi->version >= Version(4, 0)) {

			// THNG: 
			//cout << "THNG: here version 4 created" << endl;
			m_backupInfo = new BackupInfo_V4(pdata, buffer.st_size, false, true);
			//cout << *m_backupInfo << endl;
		}
		else {
		
		// Only use Version 3 of BUINFO for now
			m_backupInfo = new BackupInfo_Base(pdata, buffer.st_size, false, true);

		}
	}
	catch(const CodeException& x) {
	    //cout << "Catching code exception here in APBackup Info and rethrow" << endl;
		throw x; // rethrow
	}
	
	
	catch(const LinuxException& x) {
        //TRACE(("LinuxException::errocode <%d>", 0, x.errcode()));
		switch(x.errcode()) {
		case ENOENT: // Win32 ERROR_FILE_NOT_FOUND:
		
			//THROW_XCODE("BUINFO not found.", CodeException::BAD_BACKUPINFO);
			//cout << "catch ENOENT, rethrow CodeException::BUINFO_MISSING " << CodeException::BUINFO_MISSING << endl;
			THROW_XCODE("BUINFO not found.", CodeException::BUINFO_MISSING);
			break;
		#if 0
		case ERROR_PATH_NOT_FOUND: // fall thru
			THROW_XCODE("SBC not found.", CodeException::REQ_SBC_MISSING);
			break;
		case ERROR_ACCESS_DENIED:
			THROW_XCODE("BUINFO locked by other user.", CodeException::BUINFO_ACCESS_DENIED);
			break;
		#endif
		default:
			TRACE((LOG_LEVEL_ERROR, "Unhandled LinuxException in %s : %d", 0, x.file().c_str(), x.line()));
			throw x;
			break;
		}
	}
	
	
	catch(...) {
		// unknown error
		THROW_XCODE("Unhandled error.", CodeException::INTERNAL_ERROR);
	}
	
	// Could be remove for final production.
	TRACE((LOG_LEVEL_INFO, "ctor APBackupInfo::APBackupInfo() returns", 0));
}

//
// write current data to new file
// <path> = relative or absolute path to location of buinfo.dat
// <path> must be NULL, "", or it must a valid path that ends with a "/" or "\\"
//===========================================================================
void APBackupInfo::createFile(const char* path) {

	newTRACE((LOG_LEVEL_INFO, "APBackupInfo::createFile(%s)", 0, path));

#ifdef _DEBUG
	cout << *this;
#endif

	// append name to path
	string filename = path;
	filename += SBCFile::name(SBCFile::BACKUP_INFO);
	// open file
	//AutoHandle handle = FileFunx::CreateFileX(filename.c_str(), GENERIC_WRITE, 0, NULL,
	//	CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		
	ofstream file(filename.c_str(), ios::out|ios::binary);
	file.write(reinterpret_cast<char*>(m_backupInfo->buf()), m_backupInfo->totalSize());
	// write data
	//FileFunx::WriteFileX2(handle, m_backupInfo->buf(), 
	//	m_backupInfo->totalSize());
}

/*
// This is an issue. We need to know the version of the buinfo
// Sine this is not a file, we can not read the version from the disk
APBackupInfo::APBackupInfo(u_int32 datasize) : m_backupInfo(NULL) 
{

	newTRACE(("APBackupInfo::APBackupInfo(%d)", 0, datasize));	//mnst

	assert(datasize >= sizeof(BackupInfo_V4::Info));
	u_int8 *pdata = new u_int8[datasize];
	// yes, we'll delete the buffer

	// 0-fill the buffer
	ZeroMemory(pdata, datasize);
	m_backupInfo = new BackupInfo_V4(pdata, true, false);
	
	// set current version
	m_backupInfo->info()->version.major(BackupInfo_V4::VERSION_MAJOR2);
	m_backupInfo->info()->version.minor(BackupInfo_V4::VERSION_MINOR2);
}
*/
