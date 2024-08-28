#ifndef _FTPFILESET_H_
#define _FTPFILESET_H_
/*
  NAME
  FTPFILESET -
  LIBRARY 3C++
  PAGENAME FTPFILESET
  HEADER  CPS
  LEFT_FOOTER Ericsson Utvecklings AB
  INCLUDE FtpFileSet.H

  COPYRIGHT
  COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

  The Copyright to the computer program(s) herein is the property of
  Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
  copied only with the written permission from Ericsson Utvecklings AB
  or in accordance with the terms and conditions stipulated in the
  agreement/contract under which the program(s) have been supplied.

  DESCRIPTION
  SBC file set as seen by the CP (over FTP).

  ERROR HANDLING
  -

  DOCUMENT NO
  190 89-CAA 109 1410

  AUTHOR
  2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on FtpFileSet.h of Windows version

  LINKAGE
  -

  SEE ALSO
  Memory layout must match "File Format Specification".

  Revision history
  ----------------
  2011-11-15 xtuudoo Created
  2012-09-11 xngudan Updated for Cluster
*/

#include "FtpPath.h"

#include <ostream>

using namespace std;

#pragma pack(push, 1)

//   
// Fileset used in BUPMsg
//========================================================================
template<class FTP_PATH>
class FtpFileSet_Base {
public:
	FtpFileSet_Base() { }
	void set(const char* fcfileDir, const char* fcfileFilename);
// access
	const FTP_PATH& fcfile() const { return m_fcfile; }

// modify
	void fcfile(const char* dir, const char* filename) { m_fcfile.set(dir, filename); }
	
	void fcfile(const FTP_PATH& path) { m_fcfile = path; }

// funx
	size_t count() const;
	void reset();
	//friend ostream& operator<<(ostream& , const FtpFileSet_Base<FTP_PATH>& );
	ostream& print(ostream&) const;

private:
	FTP_PATH m_fcfile;
};

//
// inlines
//===========================================================================
//
template<class FTP_PATH>
inline void FtpFileSet_Base<FTP_PATH>::set(const char* fcfileDir, const char* fcfileFilename) {
	if(count())
		reset();
	m_fcfile.set(fcfileDir, fcfileFilename);
}
//
template<class FTP_PATH>
inline size_t FtpFileSet_Base<FTP_PATH>::count() const {
	size_t cnt = (m_fcfile.empty() ? 0 : 1);
	return cnt;
}
//
template<class FTP_PATH>
inline void FtpFileSet_Base<FTP_PATH>::reset() {
	m_fcfile.set(0, 0);
}

template<class FTP_PATH>
inline
ostream& FtpFileSet_Base<FTP_PATH>::print(ostream& os) const {
	return os << fcfile() << "\n";
}

typedef FtpFileSet_Base<FtpPath> FtpFileSet;
typedef FtpFileSet_Base<FtpPath_V4> FtpFileSet_V4;

ostream& operator<<(ostream& , const FtpFileSet& );
ostream& operator<<(ostream& , const FtpFileSet_V4& );

#pragma pack(pop)
#endif

