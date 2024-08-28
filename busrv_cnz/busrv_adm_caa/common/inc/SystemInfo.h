#ifndef _SYSTEMINFO_H_
#define _SYSTEMINFO_H_
/*
NAME
	SYSTEMINFO -
LIBRARY 3C++
PAGENAME SYSTEMINFO
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
   -.

ERROR HANDLING
   -

DOCUMENT NO
	190 89-CAA 109 0582

AUTHOR
   2006-05-17 by EAB/AZA/DG Lillemor Pettersson (uablan)

SEE ALSO

Revision history
----------------
2010-02-23 xquydao Update for APG43L project


*/

#include "PlatformTypes.h"
#include <string>

namespace SystemInfo {
const u_int32 DEFAULT_BANDWIDTH = 1000;
//
// get the current band width for the CP-AP link
//=========================================================================
int getCurrentCPAPBandWidth(u_int32 &bandWidth);

int getCurrentProcessID(void);
std::string getCurrentProcessName(void);


};
#endif
