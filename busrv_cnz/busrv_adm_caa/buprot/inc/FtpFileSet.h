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
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

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
	190 89-CAA 109 0387

AUTHOR
	2002-05-31 by U/Y/SF Anders Gillgren (qabgill, tag: ag)

LINKAGE
	-

SEE ALSO
	Memory layout must match "File Format Specification".

Revision history
----------------
2002-05-31 Created   qabgill
2020-05-23 xdtthng   Modified for SSI
*/
#pragma once

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
	void set(const char* buinfoDir, const char* buinfoFilename,
		const char* sddDir, const char* sddFilename);
	void set(const char* buinfoDir, const char* buinfoFilename,
		const char* sddDir, const char* sddFilename,
		const char* lddDir, const char* lddFilename);
	void set(const char* buinfoDir, const char* buinfoFilename,
		const char* sddDir, const char* sddFilename,
		const char* lddDir, const char* lddFilename,
		const char* psDir, const char* psFilename,
		const char* rsDir, const char* rsFilename);
// access
	const FTP_PATH& buinfo() const { return m_buinfo; }
	const FTP_PATH& sdd() const { return m_sdd; }
	const FTP_PATH& ldd() const { return m_ldd; }
	const FTP_PATH& ps() const { return m_ps; }
	const FTP_PATH& rs() const { return m_rs; }
	int dirLength() const {return m_buinfo.dirLength(); }
// modify
	void buinfo(const char* dir, const char* filename) { m_buinfo.set(dir, filename); }
	void sdd(const char* dir, const char* filename) { m_sdd.set(dir, filename); }
	void ldd(const char* dir, const char* filename) { m_ldd.set(dir, filename); }
	void ps(const char* dir, const char* filename) { m_ps.set(dir, filename); }
	void rs(const char* dir, const char* filename) { m_rs.set(dir, filename); }

	void buinfo(const FTP_PATH& path) { m_buinfo = path; }
	void sdd(const FTP_PATH& path) { m_sdd = path; }
	void ldd(const FTP_PATH& path) { m_ldd = path; }
	void ps(const FTP_PATH& path) { m_ps = path; }
	void rs(const FTP_PATH& path) { m_rs = path; }
// funx
	size_t count() const;
	void reset();
    //friend ostream& operator<<(ostream& , const FtpFileSet_Base<FTP_PATH>& );
	ostream& print(ostream&) const;

private:
	FTP_PATH m_buinfo;
	FTP_PATH m_sdd;
	FTP_PATH m_ldd;
	FTP_PATH m_ps;
	FTP_PATH m_rs;
};


//
// inlines
//===========================================================================
//
template<class FTP_PATH>
inline 
void FtpFileSet_Base<FTP_PATH>::set(const char* buinfoDir, const char* buinfoFilename,
	const char* sddDir, const char* sddFilename) {
	if(count())
		reset();
	m_buinfo.set(buinfoDir, buinfoFilename);
	m_sdd.set(sddDir, sddFilename);
}
//
template<class FTP_PATH>
inline 
void FtpFileSet_Base<FTP_PATH>::set(const char* buinfoDir, const char* buinfoFilename,
	const char* sddDir, const char* sddFilename,
	const char* lddDir, const char* lddFilename) {
	set(buinfoDir, buinfoFilename, sddDir, sddFilename);
	m_ldd.set(lddDir, lddFilename);
}
//
template<class FTP_PATH>
inline 
void FtpFileSet_Base<FTP_PATH>::set(const char* buinfoDir, const char* buinfoFilename,
	const char* sddDir, const char* sddFilename,
	const char* lddDir, const char* lddFilename,
	const char* psDir, const char* psFilename,
	const char* rsDir, const char* rsFilename) {
	set(buinfoDir, buinfoFilename, sddDir, sddFilename, lddDir, lddFilename);
	m_ps.set(psDir, psFilename);
	m_rs.set(rsDir, rsFilename);
}
//
template<class FTP_PATH>
inline 
size_t FtpFileSet_Base<FTP_PATH>::count() const {
	size_t cnt = (m_buinfo.empty() ? 0 : 1) +
		(m_sdd.empty() ? 0 : 1) + (m_ldd.empty() ? 0 : 1) +
		(m_ps.empty() ? 0 : 1) + (m_rs.empty() ? 0 : 1);
	return cnt;
}
//
template<class FTP_PATH>
inline 
void FtpFileSet_Base<FTP_PATH>::reset() {
	m_buinfo.set(0, 0); m_sdd.set(0, 0); m_ldd.set(0, 0);
	m_ps.set(0, 0); m_rs.set(0, 0);
}

template<class FTP_PATH>
inline
ostream& FtpFileSet_Base<FTP_PATH>::print(ostream& os) const {
	return os << buinfo() << "\n" <<
		sdd() << "\n" <<
		ldd() << "\n" <<
		ps() << "\n" <<
		rs();
}

typedef FtpFileSet_Base<FtpPath> FtpFileSet;
typedef FtpFileSet_Base<FtpPath_V4> FtpFileSet_V4;

ostream& operator<<(ostream& , const FtpFileSet& );
ostream& operator<<(ostream& , const FtpFileSet_V4& );

#pragma pack(pop)
#endif
