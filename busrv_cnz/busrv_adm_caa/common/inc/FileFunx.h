#ifndef _FILEFUNX_H_
#define _FILEFUNX_H_
/*
NAME
	FILEFUNX - 
Ericsson Utvecklings AB

	COPYRIGHT Ericsson Utvecklings AB, Sweden 2001. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the 
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION 
   namespaced utility functions for file handling
	mainly used to enforce the error handling by using the Exception classes
	1. first comes specific utility functions for the AP code
	2. and then simple wrappers around existing Win32 functions, but w.
	   error handling using Exceptions. These functions follow the Win32
		naming conventions with leading capital letter. I have appended an X
		to the Win32 function name to indicate the extension to the corre-
		spondig Win32 function.
		In some cases simplified versions are supplied. These functions have
		X2 appended to the corresponding Win32 name.

ERROR HANDLING
	throws Win32Exception on errors

DOCUMENT NO
   190 89-CAA 109 0387

AUTHOR 
   2001-02-07 by U/B/SF Anders Gillgren (qabgill, tag: ag)
   2010-12-14 xquydao Update for APG43L project

SEE ALSO 
   - 	
*/

#include "Exception.h"
#include "PlatformTypes.h"

#include <cassert>
#include <fcntl.h>
#include <string>

class DateTime;
class Job;


namespace FileFunx {

	//
	// Get file time
	//=========================================================================
	int getWriteFileTime(const char* fileName, DateTime& dateTime);

	//
	// dir or die
	// TODO: change the mode to other number if it's not appropriate
	//=========================================================================
	void createDirX(const char* pc, mode_t mode = 0755);

	//
	// Change the ownership of path for user "root" to groupName
	// ============================================================================
	void chown(const char* path, const char* groupName = "FMSUSRGRP");
	//
	// Create recursive directories if fails check errno
	// ============================================================================
	bool createDirectories(const std::string& path, mode_t mode = 0755);
	
	//
	// Wrapper for Create or Open File
	//=========================================================================
	int CreateFileX(const char* fileName, int flags = O_RDWR | O_CREAT, mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	//
	// just a check
	//=========================================================================
	bool dirExists(const char* pc);

	//
	// Delete a file
	//=========================================================================
	bool DeleteFileX(const char* fileName);

	//
	// delete multiple files in dir
	//=========================================================================
	void deleteFiles(const char* path, const char** files, size_t cnt);

	//
	//
	//=========================================================================
	bool SetCurrentDirectoryX(const char* fileName);

	//
	// check if the file is 0 bytes (w/o opening it)
	//=========================================================================
	bool empty(const char* fileName);

	//
	// Remove files with Regular Expression pattern
	// then the directory itself if rmdir is true
	// return nof files not deleted
	//=========================================================================
	size_t cleanDir(const char* dir, const char* regex_pattern = ".*", bool del_dir = true);

	//
	// get file size as u_int64
	//=========================================================================
	u_int64 getFileSize(const char* fileName);

	//
	// Check file exist
	//=========================================================================
	bool fileExists(const char* fileName);

	//
	// Wrapper for write file
	//=========================================================================
	bool WriteFileX(int fd, const void* buffer, size_t count);

	//
	// Wrapper for read file
	//=========================================================================
	bool ReadFileX(int fd, void* buffer, size_t count);

	//
	// Wrapper for close file
	//=========================================================================
	bool CloseFileX(int fd);

	//
	// Wrapper for moving file or directory
	//=========================================================================
	bool MoveFileX(const char* old_path, const char* new_path);

	//
	// calculate W32 checksum on data area for each W32 word
	//=========================================================================
	u_int32 segmentCSum(int fd, u_int64 nofW32Words, Job* = 0);


#if 0

    //
    // same as previous, except that bytes written is handled automatically
    //=========================================================================
    BOOL WriteFileX2(HANDLE hfile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite);

	//
	// simplified version
	//=========================================================================
	BOOL ReadFileX2(HANDLE hfile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead);

#endif
};


#endif
