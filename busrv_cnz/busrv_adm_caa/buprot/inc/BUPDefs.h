#ifndef _BUPDEFS_H
#define _BUPDEFS_H
/*
NAME
	BUPDEFS -
LIBRARY 3C++
PAGENAME BUPDEFS
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE BUPDefs.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION
	Declaratons, data types, etc., used in BUP (Backup Protocol).
	
	Note: This file follows the AP coding standards and practice, but is
	also compiled into the APZ-VM binary.

  The main differences between CP and AP coding conventions are
  1) constants, enums etc. are all upper-case w. underscores, as in
	  "MAX_PATH", not MaxPath
  2) Typenames and templates are allowed. (We have only light use of
	  these facilities in the code.)
  3) The BUSRV program, specifically, has a preference rule to use the
	  Win32 API rather than the standard C-library.

ERROR HANDLING
	-

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR 
	2002-02-26 by UAB/KY/SK Anders Gillgren (qabgill, tag: ag), based on BUDefs.hxx

LINKAGE
	-

SEE ALSO 

*/
#pragma once
//
// Imported Interfaces, Types & Definitions
//===========================================================================
#pragma once

#include "PlatformTypes.h"

#include <assert.h>
#include <string.h>
//
// Exported Types & Definitions
//===========================================================================
#pragma pack(push, 1)
//
//
//===========================================================================
namespace BUPDefs {
	//
	//
	//========================================================================
	enum EXTENT {
		FIRST_EXTENT = 0,
		SMALL = FIRST_EXTENT,
		SMALL_AND_LARGE,
		ALL,
		NEW_SMALL_AND_LARGE,
		LAST_EXTENT = NEW_SMALL_AND_LARGE,
		INVALID_EXTENT
	};
	enum {
		NUMBER_OF_EXTENTS = LAST_EXTENT + 1 - FIRST_EXTENT,
		NUMBER_OF_EXTENT_NAMES = NUMBER_OF_EXTENTS + 1
	};
	//
	// Misc.
	//========================================================================
	enum {
		MAX_CMDLOG_FILE = 9999999
	}; 
};

#pragma pack(pop)

#endif
