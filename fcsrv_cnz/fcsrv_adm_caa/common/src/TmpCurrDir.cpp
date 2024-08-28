/*
NAME
   File_name:TmpCurrDir.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

The Copyright to the computer program(s) herein is the property of Ericsson
Utvecklings AB, Sweden.
The program(s) may be used and/or copied only with the written permission from
Ericsson Utvecklings AB or in accordance with the terms and conditions
stipulated in the agreement/contract under which the program(s) have been
supplied.

DESCRIPTION
   This class will temporarily set the "current directory",
	and restore the old current dir when the instance leaves
	scope (in the destructor).

 DOCUMENT NO
   ??? ??-??? ??? ???? (190 89-CAA 109 0082)

AUTHOR
   2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on TmpCurrDir.cpp of FCSRV in Windows

SEE ALSO
   -

Revision history
----------------
2002-01-30 qabgill Created
2002-11-04 uablan  Add ACS_Trace
2010-10-20 xquydao updated for APG43L project
*/

#include "TmpCurrDir.h"
#include "FileFunx.h"
#include "CPS_FCSRV_Trace.h"
#include "LinuxException.h"

#include <unistd.h>
#include <climits>

//
// ctor
//===========================================================================
TmpCurrDir::TmpCurrDir(const string& newdir) : m_olddir("")
{
	newTRACE((LOG_LEVEL_INFO, "TmpCurrDir::TmpCurrDir(%s)", 0, newdir.c_str()));

	char curr[PATH_MAX];

	if (getcwd(curr, PATH_MAX) == NULL)
	{
	    THROW_LINUX_MSG("Get current working directory failed");
	    return;
	}

	m_olddir = curr;

	FileFunx::SetCurrentDirectoryX(newdir.c_str());
}

//
// dtor
//===========================================================================
TmpCurrDir::~TmpCurrDir()
{
	newTRACE((LOG_LEVEL_INFO, "TmpCurrDir::~TmpCurrDir()", 0));

	// THNG: Need to put it back. This throws exception all the time
	// Removing does not really help
	FileFunx::SetCurrentDirectoryX(m_olddir.c_str());
}
