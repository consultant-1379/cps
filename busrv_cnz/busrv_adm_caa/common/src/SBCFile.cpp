/*
NAME
	File_name:SBCFile.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

The Copyright to the computer program(s) herein is the property of Ericsson
Utvecklings AB, Sweden.
The program(s) may be used and/or copied only with the written permission from
Ericsson Utvecklings AB or in accordance with the terms and conditions
stipulated in the agreement/contract under which the program(s) have been
supplied.

DESCRIPTION
	Handles file identities and names.

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
	2002-05-31 by U/Y/SF Anders Gillgren (qabgill, tag :ag)

SEE ALSO
	-

Revision history
----------------
2002-05-31 qabgill Created
2002-11-04 uablan  Add ACS_Trace
*/


#include "SBCFile.h"
#include "CPS_BUSRV_Trace.h"

#include <assert.h>
//
//
//===========================================================================
const char* SBCFile::FILENAME[SBCFile::NUMBER_OF_IDS] = {
	"BUINFO", "SDD", "LDD1", "LDD2", "PS", "RS", "PS2", "RS2"
};
//
// static
//===========================================================================
const char* SBCFile::name(SBCFile file) {

	newTRACE((LOG_LEVEL_INFO, "SBCFile::name(SBCFile %d)", 0, file.id()));

	assert(file.valid());
	if(file.valid()) {
	    //TRACE(("SBCFile::name, file valid, returning", 0));
		return FILENAME[file.id()];
	}
	//TRACE(("SBCFile::name, file NOT valid, returning", 0));
	return "";
}
//
// friend
//===========================================================================
ostream& operator<<(ostream& os, SBCFile file) {
	return os << "<" << file.id() << "><" << (file.valid() ? file.name() : "NoName") << ">";
}

