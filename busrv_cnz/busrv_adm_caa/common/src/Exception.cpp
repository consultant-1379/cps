/*
File_name:Exception.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2000. All rights reserved.

	The Copyright to the computer program(s) herein is the property of Ericsson
	Utvecklings AB, Sweden.
	The program(s) may be used and/or copied only with the written permission from
	Ericsson Utvecklings AB or in accordance with the terms and conditions
	stipulated in the agreement/contract under which the program(s) have been
	supplied.

DESCRIPTION
	Base class for exceptions.

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR 
	2000-12-18 by U/B/SF Anders Gillgren (qabgill, tag :ag)

SEE ALSO 
	GetLastError, WSAGetLastError, etc.

Revision history
----------------
2000-12-18 Created

*/

#include "Exception.h"

#include <sstream>
#include <iostream>
//
// protected ctor
//----------------------------------------------------------------------------
Exception::Exception(const string& msg, int errcode, const char* file, int line)
	                : m_msg(msg), m_errcode(errcode), m_file(file), m_line(line)
{
}
//
// virtual, write content to OutputDebugString
//----------------------------------------------------------------------------
void Exception::dump() const
{
	stringstream ss;
	ss << name() << " <" << m_msg << "> " << m_file << ":" << m_line << endl;
	
	// Send it to console for Linux
	// Will be removed later on
	//OutputDebugString(ss.str().c_str());
	std::cout << ss.str() << std::endl;
}
