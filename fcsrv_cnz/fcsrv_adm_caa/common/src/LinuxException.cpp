/*
NAME
  File_name:LinuxException.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

	The Copyright to the computer program(s) herein is the property of Ericsson
	Utvecklings AB, Sweden.
	The program(s) may be used and/or copied only with the written permission from
	Ericsson Utvecklings AB or in accordance with the terms and conditions
	stipulated in the agreement/contract under which the program(s) have been
	supplied.

DESCRIPTION
   global (static member) utility function for file handling. mostly wrappers
	around Win32 function with exception based error handling & asserts.

DOCUMENT NO
   190 89-CAA 109 1410

AUTHOR
   2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on BUSRV's LinuxException.cpp
SEE ALSO
	-

Revision history
----------------

*/

#include "LinuxException.h"

#include <cassert>
#include <sstream>
#include <cerrno>

//
// ctor 1 - default msg, pick up error code
//----------------------------------------------------------------------
LinuxException::LinuxException(const char* file,
                               int line) : Exception("", errno, file, line)
{
    createMsg("");
}

//
// ctor 2 - default msg, error code as parameter
//----------------------------------------------------------------------
LinuxException::LinuxException(int errcode,
                               const char* file,
                               int line) : Exception("", errcode, file, line)
{
    createMsg("");
}

//
// ctor 3 - msg as parameter - pick up error code
//----------------------------------------------------------------------
LinuxException::LinuxException(const string& msg,
                               const char* file,
                               int line) : Exception("", errno, file, line)
{
    createMsg(msg);
}

//
// ctor 4 - msg & error code as parameters
//----------------------------------------------------------------------
LinuxException::LinuxException(const string& msg,
                               int errcode,
                               const char* file,
                               int line) : Exception("", errcode, file, line)
{
    createMsg(msg);
}

//
// create message
//----------------------------------------------------------------------
void LinuxException::createMsg(const string& msg)
{
    stringstream ss;

    ss << msg << (msg.empty() ? "" : " --- ") << "Linux Error: " << errcode() << endl;
    m_msg = ss.str();
}
