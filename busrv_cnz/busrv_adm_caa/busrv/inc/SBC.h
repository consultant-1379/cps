#ifndef _SBC_H_
#define _SBC_H_
/*
NAME
	SBC -
LIBRARY 3C++
PAGENAME SBC
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE SBC.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION
	Handles misc. funx relating to the SBC dir and fileset.

ERROR HANDLING
	-

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
	2002-05-17 by U/Y/SF Anders Gillgren (qabgill, tag: ag)
	2010-12-14 xquydao Update for APG43L project

LINKAGE
	-

SEE ALSO
	-

*/
#include "buap.h"
#include "PlatformTypes.h"
#include "Config.h"
#include "SBCDir.h"
#include "SBCId.h"

#if HAVE_ACS_CS
#include "ACS_CS_API.h"
#endif

#include <string>

using namespace buap;
using namespace std;

class SBC {
// types
public:
	enum BI_QUERY {
		BQ_NO_ERROR = 0,
		BQ_UNKNOWN_ERROR,
		BQ_CONTENT_ERROR,
		BQ_ACCESS_ERROR,
		BQ_FILE_NOT_FOUND
	};
//foos
public:
	SBC();
	SBC(SBCId sbcId, CPID cpId=0xFFFF, bool reserve = false);
	~SBC();
	// access & query
	bool reserved() const { return m_sbcDir.reserved(); }
	SBCId id() const { return m_sbcDir.id(); }
	bool dirExists(CPID cpId=0xFFFF) const { return m_sbcDir.dirExists(cpId); }
	string dir(CPID cpId=0) const { return m_sbcDir.dir(cpId); }
	size_t fileCnt() const;
	// modify
	void id(SBCId sbcId, CPID cpId=0xFFFF) { m_sbcDir.id(sbcId, cpId); }
	// funx
	// see BI_QUERY above
	BI_QUERY biQuery(CPID cpId=0xFFFF) const;
	// 1 == this greater(later) than other, 0 == equal, -1 == older
	int cmpDateTime(const SBC& o, CPID cpId=0xFFFF) const;
	// returns true if file is reserved after op
	bool reserve(CPID cpId=0xFFFF) { return m_sbcDir.reserve(cpId); } 
	void unreserve() { m_sbcDir.unreserve(); }
	void swap(SBC& o, SBCId tmpId, bool overwriteTmp, CPID cpId=0xFFFF);
	void move(SBCId sbcId, bool overwrite, CPID cpId=0xFFFF) { m_sbcDir.move(sbcId, overwrite, cpId); }
	// not included in PRA1
	// void diskCopy(SBCId sbcId, bool overwrite, HANDLE abortEvent);
private:
	SBC(const SBC& rhs);
	SBC& operator=(const SBC& rhs);
//attr
private:
	SBCDir m_sbcDir;
};
//
//
// inlines
//===========================================================================

class SBC_V4 {
// types
public:
    enum BI_QUERY {
        BQ_NO_ERROR,
        BQ_UNKNOWN_ERROR,
        BQ_CONTENT_ERROR,
        BQ_ACCESS_ERROR,
        BQ_FILE_NOT_FOUND
    };
//foos
public:
    SBC_V4();
	SBC_V4(SBCId sbcId, const Config::CLUSTER&, bool reserve = false);
    // access & query
    bool reserved() const { return m_sbcDir.reserved(); }
    SBCId id() const { return m_sbcDir.id(); }
    bool dirExists() const { return m_sbcDir.dirExists() ; }
    string dir() const { return m_sbcDir.dir(); }
    size_t fileCnt() const;
    // modify
    void id(SBCId sbcId) { m_sbcDir.id(sbcId); }
    // funx
    // see BI_QUERY above
    BI_QUERY biQuery() const;
    // 1 == this greater(later) than other, 0 == equal, -1 == older
    int cmpDateTime(const SBC_V4& o) const;
    // returns true if file is reserved after op
    bool reserve() { return m_sbcDir.reserve(8); } 
    void unreserve() { m_sbcDir.unreserve(); }
    void swap(SBC_V4& o, SBCId tmpId, bool overwriteTmp);
    void move(SBCId sbcId, bool overwrite) { m_sbcDir.move(sbcId, overwrite); }
    // not included in PRA1
    // void diskCopy(SBCId sbcId, bool overwrite, HANDLE abortEvent);
private:
    SBC_V4(const SBC_V4& rhs);
    SBC_V4& operator=(const SBC_V4& rhs);
//attr
private:
    SBCDir_V4 m_sbcDir;
	const Config::CLUSTER m_cluster;
};
//
// inlines
//===========================================================================
#endif
