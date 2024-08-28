/*
NAME
	File_name:SBCId.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

The Copyright to the computer program(s) herein is the property of Ericsson
Utvecklings AB, Sweden.
The program(s) may be used and/or copied only with the written permission from
Ericsson Utvecklings AB or in accordance with the terms and conditions
stipulated in the agreement/contract under which the program(s) have been
supplied.

DESCRIPTION
	SBC or Backup identity.

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
	2002-05-31 by U/Y/SF Anders Gillgren (qabgill, tag :ag)

SEE ALSO
	-

Revision history
----------------
2002-05-31 qabgill Created
2002-10-31 uablan  Added ACS_Trace
2010-05-23 xdtthng Modified for SSI

*/

#include <cstdio>
#include <istream>
#include <cctype>
#include <cstring>
#include <boost/lexical_cast.hpp>
#include "SBCId.h"
#include "CPS_BUSRV_Trace.h"

//static
const char* SBCId::DIR_BASE_NAME = "RELFSW";
const size_t SBCId::DIR_BASE_NAME_LEN = strlen(SBCId::DIR_BASE_NAME);

void SBCId::id(const string& name)
{
	m_id = MAX_NOF_SBCS;
	m_filler = 0;
	
    if (name.length() <= DIR_BASE_NAME_LEN)
        return;
    
    string::size_type idx;
    idx = name.find(DIR_BASE_NAME);
       
    if (idx == string::npos || idx)
        return;       
                
    try {
        m_id = boost::lexical_cast<u_int16>(name.substr(idx + DIR_BASE_NAME_LEN));
    }
    catch (...) {
        // Do nothing
    }
}
//
// friend
//===========================================================================
ostream& operator<<(ostream& os, SBCId id) {
	return os << "<" << id.id() << "><" << id.m_filler << ">";
}
//
// friend
//===========================================================================
istream& operator>>(istream& is, SBCId& sbcId) {
	char c[4];
	is >> c[0] >> sbcId.m_id >> c[1] >> c[2] >> sbcId.m_filler >> c[3];
	return is;
}
//
//
//===========================================================================
string SBCId::dirName() const {

	newTRACE((LOG_LEVEL_INFO, "SBCId::dirName()", 0));

	char buf[8];	
	sprintf(buf, "%d", m_id);
	if(isTmp()) {
		int ch = m_id - MAX_NOF_SBCS;
		// The exact meaning of temporary file
		if (ch == 'X' || ch == 'Y') {
			buf[0] = static_cast<char>(ch);
			buf[1] = '\0';
		}
	}
	return string(DIR_BASE_NAME) + buf;
}
