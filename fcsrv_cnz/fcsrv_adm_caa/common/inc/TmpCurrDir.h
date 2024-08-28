#ifndef _TMPCURRDIR_H
#define _TMPCURRDIR_H
/*
NAME
	TMPCURRDIR -
LIBRARY 3C++
PAGENAME TMPCURRDIR
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE TmpCurrDir.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION
   This class will temporarily set the "current directory",
	and restore the old current dir when the instance leaves
	scope (in the destructor).

ERROR HANDLING
   -

DOCUMENT NO
	??? ??-??? ??? ???? (190 89-CAA 109 0082)

AUTHOR
   2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on TmpCurrDir.h of Windows version

LINKAGE
	-

SEE ALSO
 	-

*/

#include <string>

using namespace std;

class TmpCurrDir {
//foos
public:
	TmpCurrDir(const string& newdir);
	~TmpCurrDir();
//attr
private:
	string m_olddir;
};
//
// inlines
//===========================================================================

#endif
