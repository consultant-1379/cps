#ifndef _SBCLIST_H_
#define _SBCLIST_H_
/*
NAME
	SBCLIST -
LIBRARY 3C++
PAGENAME SBCLIST
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE 

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2001. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION
   List of available System Backup Copies (directories (files in APZ term.))
	in current directory.

ERROR HANDLING
   -

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
   2001-12-03 by U/Y/SF Anders Gillgren (qabgill, tag: ag)

LINKAGE
	-

SEE ALSO

*/
#pragma once


#include <ostream>
#include <string>

using namespace std;

#include "Config.h"
#include "BUPMsg.h"
#include "PlatformTypes.h"
#include "SBCId.h"
//#include "ACS_CS_API.h"
class FallbackMgr_V4;
class FallbackMgr;
//class SBCSwitchJob_V4;

class SBCList {
// types
public:
	struct Data {
		u_int32 count;
		SBCId sbcId[SBCId::LAST_ID - SBCId::FIRST_ID + 1];
	};
//
public:
	SBCList();
	SBCList(SBCId::SBC_RANGE , bool firstSeqOnly, CPID cpId=0xFFFF);
	
	SBCList(SBCId::SBC_RANGE , bool firstSeqOnly, const Config::CLUSTER&);
	
	u_int32 count() const {return m_dat.count; }	// how many entries
	const SBCId& sbcId(u_int32 idx) const; 			// get backupId for position <idx>
	SBCId* sbcIds() { return m_dat.sbcId; }
	void update(SBCId::SBC_RANGE , bool firstSeqOnly, CPID cpid=0xFFFF ); // read CPS root again
	void update(SBCId::SBC_RANGE , bool firstSeqOnly, const Config::CLUSTER&);
		
	friend ostream& operator<<(ostream& , const SBCList& );
	friend class FallbackMgr_V4;		// For setting SRM Reload File pointer
	friend class SBCSwitchJob_V4;		// Load and switch interaction
	friend class FallbackMgr;

private:
	Data	m_dat;
	u_int32	m_range;
	int		m_sbcList[128];
	
	//static const string RELVOLUMSW;
	static const char* RELVOLUMSW;

	void scanCpfFiles(const char* cpName);
};

#endif
