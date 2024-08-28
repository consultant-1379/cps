#ifndef _ENVINIT_H_
#define _ENVINIT_H_
/*
NAME
	ENVINIT - 
LIBRARY 3C++
PAGENAME ENVINIT
HEADER  CPS  
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE EnvInit.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the 
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION 
   Initialisation of the process depending on the environment - APG-40

ERROR HANDLING
   -

DOCUMENT NO
   190 89-CAA 109 1410

AUTHOR 
   2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on EnvInit.h of Windows version

LINKAGE
	-

SEE ALSO 
	-

*/

namespace EnvInit {
	enum {
			ap_command,
			ap_service
	};

	// called during process startup, typically in main()
	bool init(const char* appname, int prtype);
};

#endif
