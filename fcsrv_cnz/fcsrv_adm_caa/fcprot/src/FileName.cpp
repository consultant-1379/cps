/*
  NAME
  File_name:FileName.cpp

  COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

  The Copyright to the computer program(s) herein is the property of Ericsson
  Utvecklings AB, Sweden.
  The program(s) may be used and/or copied only with the written permission from
  Ericsson Utvecklings AB or in accordance with the terms and conditions
  stipulated in the agreement/contract under which the program(s) have been
  supplied.

  DESCRIPTION
  Filename as seen by the CP (over FTP).

  DOCUMENT NO
  190 89-CAA 109 0670

  AUTHOR
  2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on FileName.cpp of FCSRV in Windows

  SEE ALSO
  Memory layout must match "File Format Specification".

  Revision history
  ----------------
  -

*/

//#include "PlatformTypes.h"
#include "FileName.h"


//
//===========================================================================

//
// friend
//===========================================================================
ostream& operator<<(ostream& os, const FileName& fileName) {
	return fileName.print(os);
}

