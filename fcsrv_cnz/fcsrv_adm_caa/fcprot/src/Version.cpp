/*
  NAME
  File_name:Version.cpp

  COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

  The Copyright to the computer program(s) herein is the property of Ericsson
  Utvecklings AB, Sweden.
  The program(s) may be used and/or copied only with the written permission from
  Ericsson Utvecklings AB or in accordance with the terms and conditions
  stipulated in the agreement/contract under which the program(s) have been
  supplied.

  DESCRIPTION
  General version class with major and minor numbering.

  DOCUMENT NO
  190 89-CAA 109 1410

  AUTHOR
  2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on Version.cpp of FCSRV in Windows

  SEE ALSO
  -

  Revision history
  ----------------

*/

#include "Version.h"
//
// friend
//===========================================================================
ostream& operator<<(ostream& os, Version v) {
	return os << dec << "<" << v.major() << "><" << v.minor() << ">";
}

