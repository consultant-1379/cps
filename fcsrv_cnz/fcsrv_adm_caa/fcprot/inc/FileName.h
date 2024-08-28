#ifndef _FILENAME_H_
#define _FILENAME_H_
/*
  NAME
  FTPPATH -
  LIBRARY 3C++
  PAGENAME FTPPATH
  HEADER  CPS
  LEFT_FOOTER Ericsson Utvecklings AB
  INCLUDE FileName.H

  COPYRIGHT
  COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

  The Copyright to the computer program(s) herein is the property of
  Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
  copied only with the written permission from Ericsson Utvecklings AB
  or in accordance with the terms and conditions stipulated in the
  agreement/contract under which the program(s) have been supplied.

  DESCRIPTION
  Filename as seen by the CP (over FTP).

  ERROR HANDLING
  -

  DOCUMENT NO
  190 89-CAA 109 1410

  AUTHOR
	2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on FileName.h of FCSRV's Window version

  LINKAGE
  -

  SEE ALSO
  Memory layout must match "FCP Protocol".

*/
#include <ostream>
#include <cstring>

using namespace std;

#pragma pack (push, 1)
//
// used in FtpFileSet
//========================================================================
class FileName {
// types
public:
	enum {
		MAX_FILENAME_LEN = 31
	};
public:
	FileName(const char* filename = NULL) {
		set(filename);
	}
	// access
	//const char* dir() const { return m_dir; }
	const char* filename() const { return m_filename; }
	// modify
	void set(const char* filename = NULL);
	// No need for friendship declaration here
	//friend ostream& operator<<(ostream& , const FileName& );
	// funx
	ostream& print(ostream&) const;

private:
	char m_filename[MAX_FILENAME_LEN + 1];
};
//
// inlines
//===========================================================================
inline void FileName::set(const char* filename) {

	//newTRACE(("FileName::set(%s)", 0, filename));

	//assert(!filename || filename[0] == '\0' || strlen(filename) <= MAX_FILENAME_LEN);

	memset(m_filename, 0, MAX_FILENAME_LEN + 1);

	if(filename) {
		size_t filenameLen = strlen(filename);
		if(filenameLen && filenameLen <= MAX_FILENAME_LEN) {
			strcpy(m_filename, filename);
		}
	}
}

inline ostream& FileName::print(ostream& os) const {
	os << "<";
	int i = 0;
	for(; i < FileName::MAX_FILENAME_LEN + 1; ++i)
		os << (m_filename[i] ? m_filename[i] : ' ');
	return os << ">";
}


ostream& operator<<(ostream& , const FileName& );

#pragma pack(pop)

#endif

