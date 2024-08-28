
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
  COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

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
  190 89-CAA 109 1410

  AUTHOR
 	 2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on FtpPath.h of FCSRV's Windows version

  LINKAGE
  -

  SEE ALSO
  Memory layout must match "Backup Protocol".

  Revision history
  ----------------
  2011-11-15 xtuudoo Created
  2012-09-11 xngudan Updated for Cluster

*/

#include <ostream>
#include <stdio.h>
#include <cstring>

using namespace std;

#pragma pack (push, 1)
//   
// used in FtpFileSet
//========================================================================
template<int MAX_DIR_LEN, int MAX_FILENAME_LEN>
class FtpPath_Base {

public:
	FtpPath_Base(const char* dir = NULL, const char* filename = NULL) {
		set(dir, filename);
	}
	// access
	const char* dir() const { return m_dir; }
	const char* filename() const { return m_filename; }
	bool empty() const { return (!m_dir || m_dir[0] == 0); }
	// modify
	void set(const char* dir = NULL, const char* filename = NULL);
	// funx
	//friend ostream& operator<<(ostream& , const FtpPath& );
	ostream& print(ostream&) const;

private:
	char m_dir[MAX_DIR_LEN + 1];
	char m_filename[MAX_FILENAME_LEN + 1];
};

template<int MAX_DIR_LEN, int MAX_FILENAME_LEN>
inline
void FtpPath_Base<MAX_DIR_LEN, MAX_FILENAME_LEN>::set(const char* dir /* = NULL */, const char* filename /* = NULL */) {

	//newTRACE(("FtpPath::set(%s, %s)", 0, dir, filename));

	//assert(!dir || dir[0] == '\0' || strlen(dir) <= MAX_DIR_LEN);
	//assert(!filename || filename[0] == '\0' || strlen(filename) <= MAX_FILENAME_LEN);
	//assert((!dir && !filename) || (dir && filename));

	memset(m_dir, 0,MAX_DIR_LEN + 1);
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

template<int MAX_DIR_LEN, int MAX_FILENAME_LEN>
inline
ostream& FtpPath_Base<MAX_DIR_LEN, MAX_FILENAME_LEN>::print(ostream& os) const {
	os << "<";
	int i = 0;
	for(; i < MAX_DIR_LEN + 1; ++i)
		os << (m_dir[i] ? m_dir[i] : ' ');
	os << ">\n<";

	for(i = 0; i < MAX_FILENAME_LEN + 1; ++i)
		os << (m_filename[i] ? m_filename[i] : ' ');

	return os << ">";
}

typedef FtpPath_Base<63, 31> FtpPath;
// This is new FtpPath of 96 char long
typedef FtpPath_Base<95, 31> FtpPath_V4;

ostream& operator<<(ostream& , const FtpPath& );
ostream& operator<<(ostream& , const FtpPath_V4& );
//
// inlines
//===========================================================================

#pragma pack(pop)

#endif
