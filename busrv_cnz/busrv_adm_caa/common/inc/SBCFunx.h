#ifndef _SBCFUNX_H_
#define _SBCFUNX_H_
/*
NAME
	SBCFunx -
LIBRARY 3C++
PAGENAME SBCFunx
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
   Functions on a SBC.

ERROR HANDLING
   -

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
   2002-04-03 by UAB/UKY/SF Lillemor Pettersson (uablan)

SEE ALSO

*/

#include "BUPMsg.h"
#include "PlatformTypes.h"
#include "autohandle.h"
#include "bu_date.h"
#include "bu_time.h"


namespace SBCFunx {
// types

	enum {
		FIRST_BACKUP_ID = 0,
		LAST_BACKUP_ID = 127,
		FFR_FIRST_BACKUP_ID = FIRST_BACKUP_ID,
		FFR_LAST_BACKUP_ID = 99,
		SFR_FIRST_BACKUP_ID = 100,
		SFR_LAST_BACKUP_ID = LAST_BACKUP_ID
	};

	struct SBCHandles {
		AutoHandle	sdd;
		AutoHandle	ldd1;
		AutoHandle	ldd2;
		AutoHandle	ps;
		AutoHandle	rs;
	};

	void rename(const u_int16 curr_backupId, const u_int16 backupId);
	void rename(const u_int16 curr_backupId, const char *name);
	void rename(const char *name, const u_int16 backupId);

	void reserve(u_int16 backupId, SBCHandles &h_sbc);
	void unreserve(SBCHandles &h_sbc);

	void getSBCDateTime(u_int16 backupId, Date &date, Time &time);

	void copySBC(u_int16 sourceSBCId, u_int16 destSBCId, bool overwrite);

};

#endif
