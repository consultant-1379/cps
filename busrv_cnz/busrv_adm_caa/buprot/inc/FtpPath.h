#ifndef _FTPPATH_H_
#define _FTPPATH_H_
/*
NAME
	FTPPATH -
LIBRARY 3C++
PAGENAME FTPPATH
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE FtpPath.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION
	Dir and filename as seen by the CP (over FTP).

ERROR HANDLING
	-

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
	2002-05-31 by U/Y/SF Anders Gillgren (qabgill, tag: ag)

LINKAGE
	-

SEE ALSO
	Memory layout must match "Backup Protocol".

*/
#pragma once

#include <ostream>
#include <cstring>

//#include "Config.h"

using namespace std;

#pragma pack (push, 1)
//   
// used in FtpFileSet
//========================================================================
template<int MAX_DIR_LEN, int MAX_FILENAME_LEN>
class FtpPath_Base {
// types
public:
	FtpPath_Base(const char* dir = NULL, const char* filename = NULL) {
		set(dir, filename);
	}
	// access
	const char* dir() const { return m_dir; }
	const char* filename() const { return m_filename; }
	bool empty() const { return (m_dir[0] == 0); }
	int dirLength() const { return MAX_DIR_LEN; }
	// modify
	void set(const char* dir = NULL, const char* filename = NULL);

    // thng: to be removed
    // There is no need for the friendship declaration here. The member function print will help	
	//friend ostream& operator<<(ostream& , const FtpPath_Base<MAX_DIR_LEN, MAX_FILENAME_LEN>& );
	
	// funx
	ostream& print(ostream&) const;

private:
	char m_dir[MAX_DIR_LEN + 1];
	char m_filename[MAX_FILENAME_LEN + 1];
};

//
// inlines
//===========================================================================
template<int MAX_DIR_LEN, int MAX_FILENAME_LEN>
void FtpPath_Base<MAX_DIR_LEN, MAX_FILENAME_LEN>::set(const char* dir, const char* filename) {

	//newTRACE(("FtpPath::set(%s, %s)", 0, dir, filename));

	//assert(!dir || dir[0] == '\0' || strlen(dir) <MAX_DIR_LEN);
	//assert(!filename || filename[0] == '\0' || strlen(filename) <= MAX_FILENAME_LEN);
	//assert((!dir && !filename) || (dir && filename));

	memset(m_dir, 0, MAX_DIR_LEN + 1);
	memset(m_filename, 0, MAX_FILENAME_LEN + 1);
	
	if(dir && filename) {
		size_t dirLen = strlen(dir);
		size_t filenameLen = strlen(filename);
		if(dirLen && filenameLen && dirLen <= MAX_DIR_LEN && filenameLen <= MAX_FILENAME_LEN) {
			//assert(dir[dirLen - 1] == '/' || dir[dirLen - 1] == '\\');
			strcpy(m_dir, dir);
			strcpy(m_filename, filename);
		}
	}
}
//
//===========================================================================
template<int MAX_DIR_LEN, int MAX_FILENAME_LEN>
ostream& FtpPath_Base<MAX_DIR_LEN, MAX_FILENAME_LEN>::print(ostream &os) const {
	os << "<";
	int i = 0;
	for(; i < MAX_DIR_LEN + 1; ++i)
		os << (m_dir[i] ? m_dir[i] : ' ');
	os << ">\n<";
	for(i = 0; i < MAX_FILENAME_LEN + 1; ++i)
		os << (m_filename[i] ? m_filename[i] : ' ');
	return os << ">";
}


//typedef FtpPath_Base<63, 31> FtpPath;
typedef FtpPath_Base<63, 31> FtpPath;

// This is new FtpPath of 96 char long
typedef FtpPath_Base<95, 31> FtpPath_V4;

ostream& operator<<(ostream& , const FtpPath& );
ostream& operator<<(ostream& , const FtpPath_V4& );

#pragma pack(pop)

#endif
