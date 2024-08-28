/*
NAME
	WIN32EXCEPTION -
LIBRARY 3C++
PAGENAME WIN32EXCEPTION
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE Win32Exception.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION
	Exception class for windows errors.
	Note that this class has two error codes, one from GetLastError, and
	one from the ERROR_CODE enum - WIN32_ERROR.

ERROR HANDLING
	-

DOCUMENT NO
	190 89-CAA 109 1410

AUTHOR
	2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on LinuxException.h of Windows version

LINKAGE
	-

SEE ALSO
	-

*/

#ifndef LINUXEXCEPTION_H_
#define LINUXEXCEPTION_H_

#include "Exception.h"

#include <sstream>

using namespace std;
//
// user provided code & message
//----------------------------------------------------------------------------
#define THROW_LINUX(msg, errcode) { stringstream ss; ss << msg; \
    LinuxException x(ss.str(), errcode, __FILE__, __LINE__); \
    DUMP_THROW_X(x)
//
// auto-fetched code, generic message
//----------------------------------------------------------------------------
#define THROW_LINUX_AUTO() { LinuxException x(__FILE__, __LINE__); DUMP_THROW_X(x)
//
// user provided code, generic message
//----------------------------------------------------------------------------
#define THROW_LINUX_CODE(errcode) { LinuxException x((errcode), __FILE__, __LINE__); \
    DUMP_THROW_X(x)
//
// auto-fetched code, user provided message
//----------------------------------------------------------------------------
#define THROW_LINUX_MSG(msg) { stringstream ss; ss << msg; \
    LinuxException x(ss.str(), __FILE__, __LINE__); \
    DUMP_THROW_X(x)
//
// auto-fetched code, user provided message, and log
//----------------------------------------------------------------------------
#define THROW_LINUX_MSG_LOG(msg) { stringstream ss; ss << msg; \
    LinuxException x(ss.str(), __FILE__, __LINE__); \
    EventReporter::instance().write(x); \
    DUMP_THROW_X(x)
//
// Linux - errno exception
//----------------------------------------------------------------------------
class LinuxException : public Exception {
//foos
public:
    LinuxException(const char* file, int line);
    LinuxException(int errcode, const char* file, int line);
    LinuxException(const string& msg, const char* file, int line);
    LinuxException(const string& msg, int errcode, const char* file, int line);
    ~LinuxException() { }
    virtual const char* name() const { return "Linux API call error."; }
private:
    void createMsg(const string& );
//attr
private:
    // -
};

#endif /* LINUXEXCEPTION_H_ */
