/*
NAME
  File_name:FileFunx.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

	The Copyright to the computer program(s) herein is the property of Ericsson
	Utvecklings AB, Sweden.
	The program(s) may be used and/or copied only with the written permission from
	Ericsson Utvecklings AB or in accordance with the terms and conditions
	stipulated in the agreement/contract under which the program(s) have been
	supplied.

DESCRIPTION
   global (static member) utility function for file handling. mostly wrappers
	around Win32 function with exception based error handling & asserts.

DOCUMENT NO
   190 89-CAA 109 1410

AUTHOR
   2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on FileFunx.cpp of FCSRV in Windows
	and BUSRV's FIleFunx.cpp
SEE ALSO
	-

Revision history
----------------

*/

#include "FileFunx.h"
#include "CPS_FCSRV_Trace.h"
#include "TmpCurrDir.h"
#include "LinuxException.h"
#include "fcap.h"
#include "CodeException.h"
#include "Job.h"
#include "DateTime.h"
#include "EventReporter.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <ctime>
#include <cstring>
#include <cstdio>
#include <climits>
#include <cerrno>
#include <pwd.h>
#include <grp.h>

#include "boost/regex.hpp"

class UMask
{
public:
    UMask() {
        m_mode = umask(0);
    }
    ~UMask() {
        umask(m_mode);
    }
private:
    mode_t  m_mode;
};

//
//
// Return non zero if able to fetch the time
//----------------------------------------------------------------------
int FileFunx::getWriteFileTime(const char* fileName, DateTime& dateTime)
{
    newTRACE((LOG_LEVEL_INFO, "FileFunx::getWriteFileTime(%s)", 0, fileName));

    struct stat fileStat;

    if (stat(fileName, &fileStat) != 0)
    {
        THROW_LINUX_MSG("Failed to fetch time of file: " << fileName << endl);
    }

    struct tm* tmData = localtime(&fileStat.st_mtime);

    if (tmData == NULL)
    {
        THROW_LINUX_MSG("Can't convert to valid time of file: " << fileName << endl);
    }

    dateTime.systemtime(*tmData);

	// Succeed to fetch the time
	return 1;
}

//
//
// Change the ownership of path for user "root" to groupName
// ============================================================================
void FileFunx::chown(const char* path, const char* groupName)
{
	newTRACE((LOG_LEVEL_INFO, "FileFunx::chown()", 0));

	ostringstream ss;
	ss << "chown of path <" << path << "> to group <" << groupName << "> ";
    struct passwd *pwd;
    struct group *grp;
    pwd = getpwnam("root");
    grp = getgrnam(groupName);
    if (::chown(path, pwd->pw_uid, grp->gr_gid)) {
        ss << "failed";
        TRACE((LOG_LEVEL_ERROR, "%s", 0, ss.str().c_str()));
        EventReporter::instance().write(ss.str().c_str());
    }
    else {
        ss << "successful";
        TRACE((LOG_LEVEL_INFO, "%s", 0, ss.str().c_str()));
    }
}

//
//
//----------------------------------------------------------------------
void FileFunx::createDirX(const char* pc, mode_t mode)
{
	newTRACE((LOG_LEVEL_INFO, "FileFunx::createDirX(%s)", 0, pc));

    UMask a;
	if (mkdir(pc, mode) != 0)
	{
	    THROW_LINUX_MSG("Create directory (" << pc << ") failed");
	}
}
//
// Create recursive directories if fails check errno
// ============================================================================
bool FileFunx::createDirectories(const string& path, mode_t mode)
{
    string::size_type search_pos = 0;
    string::size_type result_pos = 0;
    int result;
    UMask a;

    do
    {
        result_pos = path.find("/", search_pos);

        if (result_pos == string::npos)
        {
            // can't find '/'
            break;
        }

        result = mkdir(path.substr(0, result_pos + 1).c_str(), mode);

        if (result == -1 && errno != EEXIST)
        {
            // Can't create directory
            break;
        }

        search_pos = result_pos + 1;
    }
    while (true);

    // Check if directory exist
    struct stat buf;
    int org_errno = errno;

    if (stat(path.c_str(), &buf) == 0)
    {
        return true;
    }

    errno = org_errno;

    // Check errno for error details
    return false;
}

//
//
//----------------------------------------------------------------------
bool FileFunx::dirExists(const char* pc)
{
    newTRACE((LOG_LEVEL_INFO, "FileFunx::dirExists(%s)", 0, pc));

    struct stat fileStat;

    if (stat(pc, &fileStat) != 0)
    {
        if (errno != ENOENT) // ENOENT: No such file or directory
        {
            THROW_LINUX("Directory exists query on " << pc << " failed", errno);
        }
		TRACE((LOG_LEVEL_INFO, "FileFunx::dirExists() return 0", 0));
        return false;
    }
	TRACE((LOG_LEVEL_INFO, "FileFunx::dirExists() return 1", 0));
    return true;
}

//
//
// Throws on error unless the error is just that the file doesn't exist
//----------------------------------------------------------------------
bool FileFunx::DeleteFileX(const char* fileName)
{
    newTRACE((LOG_LEVEL_INFO, "FileFunx::DeleteFileX(%s)", 0, fileName));

    if(remove(fileName) != 0)
    {
        if (errno != ENOENT) // ENOENT: No such file or directory
        {
            THROW_LINUX("Failed to delete file (" << fileName << ")", errno);
        }

        return false;
    }

    return true;
}


//
// Doesn't care if the files exist, accepts some failures (read-only, no access, etc.)
//----------------------------------------------------------------------
void FileFunx::deleteFiles(const char* path, const char** files, size_t cnt)
{
    newTRACE((LOG_LEVEL_INFO, "FileFunx::deleteFiles(%s, const char** files, %d)", 0, path, cnt));

    if (cnt == 0)
        return;

    assert(path && strlen(path));

    // Make sure we have at least one valid fileName (could loop cnt here)
    assert(files && *files && strlen(files[0]));

    TmpCurrDir currDir(path);

    for (size_t i = 0; i < cnt; ++i) {
        DeleteFileX(files[i]);
    }
}

//
//
//----------------------------------------------------------------------
bool FileFunx::SetCurrentDirectoryX(const char* lpPathName)
{
    newTRACE((LOG_LEVEL_INFO, "FileFunx::SetCurrentDirectoryX(%s)", 0, lpPathName));

    if (chdir(lpPathName) != 0)
    {
    	// The improvement is removed. However, please do not delete the commented out code.
    	//
        //if (errno != ENOENT && errno != ENOTDIR)
        //{
            THROW_LINUX_MSG("Set current directory (" << lpPathName << ") failed");
        //}
        //return false;
    }

    return true;
}

//
// Check if the file is 0 bytes (w/o opening it)
//----------------------------------------------------------------------
bool FileFunx::empty(const char* fileName)
{
    newTRACE((LOG_LEVEL_INFO, "FileFunx::empty(%s)", 0, fileName));

    struct stat fileStat;

    if (stat(fileName, &fileStat) != 0)
    {
        THROW_LINUX_MSG("File operation failed");
        return false;
    }

    return (fileStat.st_size == 0) ? true : false;
}

//
// cleanDir - delete all files matching <pattern> in a directory, and then
// the directory itself.
// no special action is taken if a delete fails, other errors will throw.
// return: number of files not deleted (not counting "." & "..")
//----------------------------------------------------------------------
size_t FileFunx::cleanDir(const char* dir, const char* regex_pattern, bool del_dir /* = true */)
{
	newTRACE((LOG_LEVEL_INFO, "FileFunx::cleanDir(%s, %s)", 0, dir, regex_pattern));

	assert(dir);
	assert(strlen(dir));
	assert(regex_pattern);
	assert(strlen(regex_pattern));
	assert(dir[strlen(dir) - 1] == '/');

	DIR *dp;
	struct dirent *ep;
	size_t failCnt = 0;
	char buf[PATH_MAX];
	string pattern = regex_pattern;
	if (pattern == "*") {
	   pattern = ".*";
    }
	const boost::regex expression(pattern);

	dp = opendir(dir);

    if (dp != NULL)
    {
        while ((ep = readdir(dp)) != NULL)
        {
            if (ep->d_type == DT_DIR)
            {
                // Only interest on files
                continue;
            }


            sprintf(buf, "%s%s", dir, ep->d_name);

            try
            {
                // Check whether file name matches the pattern before delete
                if (boost::regex_match(buf, expression) == false)
                {
                    continue;
                }
            }
            catch(...)
            {
                TRACE((LOG_LEVEL_WARN, "regex_match failed(%s, %s)", 0, buf, regex_pattern));
                continue;
            }

            if (DeleteFileX(buf) == false)
            {
                TRACE((LOG_LEVEL_WARN, "Failed to delete %s", 0, buf));
                failCnt++;
            }
        }

        if (closedir(dp) != 0)
        {
            THROW_LINUX_MSG("Close dir operation failed");
        }
    }
    else
    {
        THROW_LINUX_MSG("Open dir operation failed");
    }

    if (del_dir)
    {
        if (rmdir(dir) != 0)
        {
            THROW_LINUX_MSG("Delete dir operation failed");
        }
    }

    return failCnt;
}

//
// Get file size as u_int64
//----------------------------------------------------------------------
u_int64 FileFunx::getFileSize(const char* fileName)
{
    newTRACE((LOG_LEVEL_INFO, "FileFunx::getFileSize(%s)", 0, fileName));

    assert(fileName != NULL);

    struct stat fileStat;

    if (stat(fileName, &fileStat) != 0)
    {
        THROW_LINUX_MSG("File operation failed");
    }

    return fileStat.st_size;
}

//
// Check file exist
//----------------------------------------------------------------------
bool FileFunx::fileExists(const char* fileName)
{
    newTRACE((LOG_LEVEL_INFO, "FileFunx::dirExists(%s)", 0, fileName));

    struct stat fileStat;

    if (stat(fileName, &fileStat) != 0)
    {
        if (errno != ENOENT) // ENOENT: No such file or directory
        {
            THROW_LINUX("File exists query on " << fileName << " failed", errno);
        }
        return false;
    }

    return true;
}

//
// Create File if sucecced return file descriptor otherwise throw exception
// Note: flags is defined in #include <fcntl.h> such as O_RDONLY,...
//-------------------------------------------------------------------------
int FileFunx::CreateFileX(const char* fileName, int flags, mode_t mode)
{
    newTRACE((LOG_LEVEL_INFO, "FileFunx::CreateFileX(%s, %d)", 0, fileName, flags));

    UMask a;
    int fd = open(fileName, flags, mode);

    if (fd == -1)
    {
    	TRACE((LOG_LEVEL_ERROR, "Failed to create file; throw exception", 0));
        THROW_LINUX_MSG("Failed to create file: " << fileName << endl);
    }

    return fd;
}

//
// Write to file
//----------------------------------------------------------------------
bool FileFunx::WriteFileX(int fd, const void* buffer, size_t count)
{
    newTRACE((LOG_LEVEL_INFO, "FileFunx::WriteFileX(%d, %u)", 0, fd, count));
    assert(buffer != NULL);

    ssize_t num;
    size_t numWrite = 0;
    const char* pBuf = (const char*) buffer;

    do
    {
        num = write(fd, pBuf + numWrite, count - numWrite);

        if (num == -1)
        {
            THROW_LINUX_MSG("Write file operation failed");
        }

        numWrite += num;
    }
    while (numWrite < count);

    return true;
}

//
// Read data from file
//----------------------------------------------------------------------
bool FileFunx::ReadFileX(int fd, void* buffer, size_t count)
{
    //newTRACE(("FileFunx::ReadFileX(%d, %u)", 0, fd, count));
    // Avoid tracing, unless temporarily turned on for debugging

    assert(buffer != NULL);

    ssize_t num;
    size_t numRead = 0;
    char* pBuf = (char*) buffer;

    do
    {
        num = read(fd, pBuf + numRead, count - numRead);

        if (num == -1)
        {
            THROW_LINUX_MSG("Read file operation failed");
        }

        if (num == 0) // EOF
        {
            break;
        }

        numRead += num;
    }
    while (numRead < count);

    return (numRead == count) ? true : false;
}
//
// Close File
//----------------------------------------------------------------------
bool FileFunx::CloseFileX(int fd)
{
    newTRACE((LOG_LEVEL_INFO, "FileFunx::CloseFileX(%d)", 0, fd));

    return (close(fd) == 0) ? true: false;
}

//
// Wrapper for moving file or directory
//=========================================================================
bool FileFunx::MoveFileX(const char* old_path, const char* new_path)
{
    newTRACE((LOG_LEVEL_INFO, "FileFunx::MoveFileX(%s %s)", 0, old_path, new_path));

    if (rename(old_path, new_path) != 0)
    {
        if (errno != EISDIR && errno != ENOENT && errno != ENOTEMPTY && errno != EEXIST)
        {
            THROW_LINUX_MSG("Move file operation failed");
        }

        return false;
    }

    return true;
}

//
// calculate W32 checksum on data area for each W32 word
//=========================================================================
u_int32 FileFunx::segmentCSum(int fd, u_int64 nofW32Words, Job* currJob)
{
	newTRACE((LOG_LEVEL_INFO, "FileFunx::segmentCSum(%d, %d)", 0, fd, nofW32Words));
	
	u_int32 csum = 0;
	const size_t BUF_SIZE = 65536;
	char buf[BUF_SIZE];
	
	if (nofW32Words) {
		u_int32 loopCnt = static_cast<u_int32>((nofW32Words * 4) / BUF_SIZE);

		for(u_int32 i = 0; i < loopCnt; ++i) {
			if(ReadFileX(fd, buf, BUF_SIZE)) {
				csum += fcap::csumW32(reinterpret_cast<u_int32*>(buf), BUF_SIZE);
			}
			else {
				THROW_XCODE("Unexpected end-of-file error.", CodeException::BAD_DATA_FILE);
			}
			if (currJob && currJob->isAborted())
			     return 0;
		}

		u_int32 lastBlockSize = static_cast<u_int32>((nofW32Words * 4) % BUF_SIZE);

		if (lastBlockSize) {
			if(ReadFileX(fd, buf, lastBlockSize)) {
				csum += fcap::csumW32(reinterpret_cast<u_int32*>(buf), lastBlockSize);
			}
			else {
				THROW_XCODE("Unexpected end-of-file error.", CodeException::BAD_DATA_FILE);
			}
		}
	}
	return csum;
}

#if 0
//
// Win32 wrapper w. exception error handling
//----------------------------------------------------------------------
BOOL FileFunx::WriteFileX2(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite) {
	newTRACE(("FileFunx::WriteFileX2(%d, LPCVOID lpBuffer, %d", 0, hFile, nNumberOfBytesToWrite));
	DWORD written = 0;
	return WriteFileX(hFile, lpBuffer, nNumberOfBytesToWrite, &written, NULL);
}
//
// Win32 wrapper w. exception error handling
// throws on
// 1) general error
// 2) if bytesRead isn't eq to bytesToRead, unless bytesToRead == 0
// returns FALSE at EOF
//----------------------------------------------------------------------
BOOL FileFunx::ReadFileX2(HANDLE hfile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead) {
	assert(hfile != INVALID_HANDLE_VALUE);
	assert(lpBuffer);

	DWORD read = 0;
	if(!ReadFile(hfile, lpBuffer, nNumberOfBytesToRead, &read, NULL) ||
		(nNumberOfBytesToRead != read && read != 0)) {
		THROW_WIN32_CODE("Read file failed.");
	}
	return read ? TRUE : FALSE;
}
//
#endif
