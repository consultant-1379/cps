#ifndef _SBCDIR_H_
#define _SBCDIR_H_
/*
NAME
	SBCDIR -
LIBRARY 3C++
PAGENAME SBCDIR
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE SBCDir.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION
	Low-level directory operations using either FMS or Win32 functions.

ERROR HANDLING
	-

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
	2002-05-30 by U/Y/SF Anders Gillgren (qabgill, tag: ag)
	2010-12-14 xquydao Update for APG43L project

LINKAGE
	-

SEE ALSO
	-

*/

#include "AutoFd.h"
#include "buap.h"
#include "PlatformTypes.h"
#include "Config.h"
#include "SBCId.h"
#include "BUPFunx.h"

#if HAVE_ACS_CS
#include "ACS_CS_API.h"
#endif

#ifndef LOCAL_BUILD
#include "fms_cpf_file.h"
#endif

#include <string>

using namespace buap;
using namespace std;

class SBCDir {
//foos
public:
	SBCDir(SBCId sbcId, bool useFMS, bool reserveDir = false, CPID cpid = 0xFFFF);
	SBCDir(bool useFMS); // set tmp object
	~SBCDir();
	// access & query
	SBCId id() const { return m_id; }
	bool reserved() const;
	bool dirExists(CPID cpId=0xFFFF) const;
	string dir(CPID cpId=0xFFFF) const; // get dir (L:/fms/.../RELFSWn)
	// modify
	void id(SBCId sbcId, CPID cpId=0xFFFF); // change sbcId
	// funx
	bool reserve(CPID cpId=0xFFFF);
	// TODO: Don't know why unreserve doesn't have have cpid parameter, check???
	void unreserve();
	void move(SBCId sbcId, bool overwrite, CPID cpId=0xFFFF);

private:

	bool linuxReserve(CPID cpId=0xFFFF);
	void linuxUnreserve();
	void linuxMove(SBCId sbcId, bool overwrite, CPID cpId=0xFFFF);
	bool linuxReserved() const;
#ifndef LOCAL_BUILD
	string fmsDir() const; // get fms dir (RELFSWn)
	bool fmsReserve(CPID cpid=0xFFFF);
	void fmsUnreserve();
	void fmsMove(SBCId sbcId, bool overwrite, CPID cpId=0xFFFF);
#endif

	SBCDir(const SBCDir& rhs);
	SBCDir& operator=(const SBCDir& rhs);
//attr
private:
	SBCId m_id;
	AutoFd m_hDir;
	bool m_useFMS;
	string m_lockFile;
#ifndef LOCAL_BUILD
	FMS_CPF_File* m_fmsDir;
#endif
};

ostream &operator<<(ostream &os, const SBCDir& sdir);

//
// inlines
//===========================================================================
//
inline bool SBCDir::reserve(CPID cpId)
{
#ifdef LOCAL_BUILD
	return linuxReserve(cpId);
#else
	return m_useFMS ? fmsReserve(cpId) : linuxReserve(cpId);
	//return true;
#endif
}
//
inline void SBCDir::unreserve()
{
#ifdef LOCAL_BUILD
	return linuxUnreserve();
#else
	m_useFMS ? fmsUnreserve() : linuxUnreserve();
#endif
}
// is the dir reserved?
inline bool SBCDir::reserved() const
{
#ifdef LOCAL_BUILD
	return linuxReserved();
#else
	return m_useFMS ? m_fmsDir != 0 : m_hDir.valid();
#endif
}
//
inline void SBCDir::move(SBCId sbcId, bool overwrite, CPID cpId)
{
#ifdef LOCAL_BUILD
	linuxMove(sbcId, overwrite, cpId);
#else
	m_useFMS ? fmsMove(sbcId, overwrite, cpId) : linuxMove(sbcId, overwrite, cpId);
#endif
}

//
// fms funx
//===========================================================================
#ifndef LOCAL_BUILD

inline string SBCDir::fmsDir() const { return m_id.dirName(); }
#endif


///////////////////////////////////////////////////////////////////////////////

class SBCDir_V4{
//foos
public:
	SBCDir_V4(SBCId sbcId, const Config::CLUSTER&, bool useFMS, bool reserveDir = false);
    SBCDir_V4(bool useFMS); // set tmp object
    ~SBCDir_V4();
    // access & query
    SBCId id() const { return m_id; }
    bool reserved() const;
    bool dirExists() const;
    string dir() const; // get win dir (L:/fms/.../RELFSWn)
    // modify
    void id(SBCId sbcId); // change sbcId
    // funx
    bool reserve(int);
    void unreserve();
    void move(SBCId sbcId, bool overwrite);
	ostream& print(ostream&) const;

private:
    
    #ifndef LOCAL_BUILD
    string fmsDir() const; // get fms dir (RELFSWn)
    bool fmsReserve();
    void fmsUnreserve();
    void fmsMove(SBCId sbcId, bool overwrite);
    #endif

    SBCDir_V4(const SBCDir_V4& rhs);
    SBCDir_V4& operator=(const SBCDir_V4& rhs);

private:
    SBCId m_id;
    bool m_useFMS;
    const Config::CLUSTER m_cluster;
    
#ifndef LOCAL_BUILD
    FMS_CPF_File* m_fmsDir;
#endif
};
ostream &operator<<(ostream &os, const SBCDir_V4& sdir);
//
//
//=============================================================================
inline bool SBCDir_V4::reserve(int) {
	return m_useFMS ? fmsReserve() : false;
}
//
//=============================================================================
// is the dir reserved?
inline bool SBCDir_V4::reserved() const {
	return m_useFMS ? m_fmsDir != 0 : false;

}
inline void SBCDir_V4::unreserve() {
    if (m_useFMS)
    	fmsUnreserve();
}
//
inline void SBCDir_V4::move(SBCId sbcId, bool overwrite) {
    if (m_useFMS)
    	fmsMove(sbcId, overwrite);
}
//
// fms funx
//===========================================================================
#ifndef LOCAL_BUILD
inline string SBCDir_V4::fmsDir() const { return m_id.dirName(); }
#endif

#endif
