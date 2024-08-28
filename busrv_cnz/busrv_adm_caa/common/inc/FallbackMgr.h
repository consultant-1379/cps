#ifndef _FALLBACKMGR_H_
#define _FALLBACKMGR_H_
/*
NAME
	FALLBACKMGR -
LIBRARY 3C++
PAGENAME FALLBACKMGR
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE FallbackMgr.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION
	Class handling the fallback SBC selection.

ERROR HANDLING
	-

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
	2002-07-04 by U/Y/SF Anders Gillgren (qabgill, tag: ag)

LINKAGE
	-

SEE ALSO
	-

Revision history
----------------
2002-07-04 qabgill created
2003-01-08 uablan  minor update
2010-05-23 xdtthng modified for SSI
2012-12-24 xdtthng Updated for APG43L BC

*/

#include "SBCId.h"
//#include "ACS_CS_API.h"
//#include "Config.h"			// For Version 4 CLUSTER
//#include "DSDMsg.h"           // This is only for CLUSTER

#include "GASrmMsg.h"

#include <vector>

using namespace std;

typedef vector<SBCId> SBCIdVector;

class FallbackMgr {
//foos
public:
	FallbackMgr(CPID cpId=0xFFFF);
	SBCId currLoadId(CPID cpId=0xFFFF) const;
	bool lastLoadableSBC(CPID cpId=0xFFFF) const;
	void updateLoadTime(CPID cpId=0xFFFF);
//attr
private:
	 SBCIdVector m_vec;
};
//
// inlines
//===========================================================================

///////////////////////////////////////////////////////////////////////////////

class FallbackMgr_V4 {
//foos
public:
	FallbackMgr_V4(const Config::CLUSTER&);
	SBCId currLoadId(const Config::CLUSTER&);
	bool lastLoadableSBC(u_int16) const;
	void updateLoadTime(const Config::CLUSTER&);
	friend ostream& operator<<(ostream&, const FallbackMgr_V4&);

	void setSBCReloadFile(int fn, GASrmMsg::ERROR_CODE& errorCode);

//attr
private:

	bool setSRMReloadFile(int fn);
	void alignReloadFile(DateTime);

	SBCIdVector m_vec;

    u_int16     m_ffr;      // read from busrv.dat
	u_int16     m_sfr;      // read from busrv.dat
    u_int16     m_sfrStart; // start of sfr
    
    u_int16     m_srmReloadFile;	// read from busrv.dat
    bool        m_startReloadSeq;	// read from busrv.dat

};
//
// inlines
//===========================================================================

#endif // ifndef _FALLBACKMGR_H_
