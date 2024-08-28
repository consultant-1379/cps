#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_
/*
NAME
	EXCEPTION - 
Ericsson Utvecklings AB

	COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the 
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION 
	Base class for exceptions.

ERROR HANDLING
	-

DOCUMENT NO
	190 89-CAA 109 1410

AUTHOR 
	2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on CPS_Utils.h of Windows version

SEE ALSO 
	GetLastError, WSAGetLastError, etc.

*/
#include <string>


using namespace std;
//
// macros
// dump() if _DEBUG (do not use directly)
//
#ifdef _DEBUG
#define DUMP_THROW_X(x) x.dump(); throw x; }
#else
#define DUMP_THROW_X(x) throw x; }
#endif
// creates and Exception, and throws it (w. dump if _DEBUG build)
#define THROW_X(msg, code) { Exception x((msg), (code), __FILE__, __LINE__); DUMP_THROW_X(x)

//
//
//----------------------------------------------------------------------------
class Exception {
//foos
public:
	virtual ~Exception() { }
protected:
	Exception(const string& msg, int errcode, const char* file, int line);
public:
	// funx
	virtual void dump() const;
	// access
	virtual const char* name() const = 0;
	const string& what() const { return m_msg; }
	int errcode() const { return m_errcode; }
	const string& file() const { return m_file; }
	int line() const { return m_line; }
//attr
protected:
	string m_msg;
	int m_errcode;
	string m_file; // source file
	int m_line; // source line
};

#endif
