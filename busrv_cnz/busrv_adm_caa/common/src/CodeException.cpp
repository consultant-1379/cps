/*
NAME
   File_name:CodeException.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2001. All rights reserved.

The Copyright to the computer program(s) herein is the property of Ericsson
Utvecklings AB, Sweden.
The program(s) may be used and/or copied only with the written permission from
Ericsson Utvecklings AB or in accordance with the terms and conditions
stipulated in the agreement/contract under which the program(s) have been
supplied.

.DESCRIPTION
	Exception class

DOCUMENT NO
   190 89-CAA 109 0387

AUTHOR 
   2001-04-03 by U/Y/SF Anders Gillgren (qabgill, tag :ag)

SEE ALSO 
   
Revision history
----------------
2001-04-03 Created

*/

#include "CodeException.h"

#include <assert.h>

//
// static
// 
const char* CodeException::ERR_CODE_NAME[CodeException::NOF_ERR_CODES] = {
	"Configuration error.",
	"Invalid or missing backupinfo file.",
	"Invalid or missing data file.",
	"Required directory not found.",
	"Failed to initialise TCP (WSAStartup).",
	"Failed to initialise TCP.",
	"Unspecified TCP error."
};
//
// ctor
//----------------------------------------------------------------------------
CodeException::CodeException(const string& msg, ERR_CODE errcode, const char* file, int line)
: Exception(msg, errcode, file, line) {
	// if anyone casts an errcode in...
	assert(errcode >= FIRST_ERR_CODE && errcode <= LAST_ERR_CODE);
}
