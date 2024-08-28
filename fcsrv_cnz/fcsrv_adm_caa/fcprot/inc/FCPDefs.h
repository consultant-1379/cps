
#ifndef _FCPDEFS_H
#define _FCPDEFS_H
/*
  NAME
  FCPDEFS -
  LIBRARY 3C++
  PAGENAME FCPDEFS
  HEADER  CPS
  LEFT_FOOTER Ericsson Utvecklings AB
  INCLUDE FCPDefs.h

  COPYRIGHT
  COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

  The Copyright to the computer program(s) herein is the property of
  Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
  copied only with the written permission from Ericsson Utvecklings AB
  or in accordance with the terms and conditions stipulated in the
  agreement/contract under which the program(s) have been supplied.

  DESCRIPTION
  Declaratons, data types, etc., used in BUP (Backup Protocol).
	
  Note: This file follows the AP coding standards and practice, but is
  also compiled into the APZ-VM binary.

  ERROR HANDLING
  -

  DOCUMENT NO
  190 89-CAA 109 0670

  AUTHOR 
  2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo), based on FCPDefs.h of FCSRV's Window version

  LINKAGE
  -

  SEE ALSO 
  
  Revision history
  ----------------

*/
//
// Imported Interfaces, Types & Definitions
//===========================================================================

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
namespace FCPDefs {
	//
	//
	//========================================================================
	enum EXTENT {
		FIRST_EXTENT = 0,
		SMALL = FIRST_EXTENT,
		SMALL_AND_LARGE,
		ALL,
		LAST_EXTENT = ALL,
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
