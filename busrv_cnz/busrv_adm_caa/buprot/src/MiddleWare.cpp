/*
NAME
	File_name:MiddleWare.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

The Copyright to the computer program(s) herein is the property of Ericsson
Utvecklings AB, Sweden.
The program(s) may be used and/or copied only with the written permission from
Ericsson Utvecklings AB or in accordance with the terms and conditions
stipulated in the agreement/contract under which the program(s) have been
supplied.

DESCRIPTION
	-

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
	2003-01-15 by UKY/SF Lillemor Pettersson (uablan)

SEE ALSO
	Memory layout must match "File Format Specification".

Revision history
----------------
2003-01-15 uablan	 Created
*/

#include "MiddleWare.h"

//
// 
//===========================================================================
ostream& operator<<(ostream& os, const MiddleWare& mware) {
	return mware.print(os);
}

