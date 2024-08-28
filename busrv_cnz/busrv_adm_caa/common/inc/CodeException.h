#ifndef _CODEEXCEPTION_H_
#define _CODEEXCEPTION_H_
/*
NAME
	CODEEXCEPTION - 
LIBRARY 3C++
PAGENAME CODEEXCEPTION
HEADER	CPS  
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE CodeException.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2001. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the 
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION 
	Exception class

ERROR HANDLING
	-

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR 
	2001-04-03 by U/Y/SF Anders Gillgren (qabgill, tag: ag)

Revision history
----------------
2001-04-03 qabgill Created
2006-08-09 uablan  Add FMS_PHYSICAL_FILE_ERROR to ERR_CODE.


LINKAGE
	-

SEE ALSO 
	
*/

#include "Exception.h"

#include <sstream>

using namespace std;
//
// 1. create CodeException 2. dump contents if _DEBUG build 3. throw
//----------------------------------------------------------------------------
#define THROW_XCODE(msg, code) { stringstream ss; ss << msg; CodeException x(ss.str(), (code), __FILE__, __LINE__); \
	DUMP_THROW_X(x)
#define THROW_XCODE_LOG(msg, code) { stringstream ss; ss << msg; CodeException x(ss.str(), (code), __FILE__, __LINE__); \
	EventReporter::instance().write(x); \
	DUMP_THROW_X(x)
//
// The only difference between this class and the base Exception is that all
// the code are from ERROR_CODE (see ErrorInfo.h)
//----------------------------------------------------------------------------
class CodeException : public Exception {
// types
public:
	enum ERR_CODE {
		FIRST_ERR_CODE = 0,
		INTERNAL_ERROR = FIRST_ERR_CODE,
		CONFIGURATION_ERROR,
		ILLEGAL_VALUE,
		BAD_BACKUPINFO,
		BUINFO_MISSING,
		BUINFO_ACCESS_DENIED,
		BAD_DATA_FILE,
		REQ_SBC_MISSING,
		REQ_FILE_MISSING,
		SBC_RESERVE_FAILED,
		TMP_SBC_EXISTS,
		WSA_STARTUP_FAILED,
		TCP_INIT_FAILED,
		TCP_ERROR,
		FMS_CREATE_FILE_FAILED,
		FMS_DELETE_FILE_FAILED,
		FMS_PHYSICAL_FILE_ERROR,
		FMS_UNKNOWN_ERROR,
                FAILED_BUINFO_CORRECTED,
                FAILED_BUINFO_NOT_CORRECTED,
                CHECKSUM_UNKNOWN_ERROR,
		CP_APG43_CONFIG_ERROR,
		LAST_ERR_CODE = CP_APG43_CONFIG_ERROR
	};
	enum { NOF_ERR_CODES = LAST_ERR_CODE - FIRST_ERR_CODE + 1 };
	static const char* ERR_CODE_NAME[NOF_ERR_CODES];
//foos
public:
	CodeException(const string& msg, ERR_CODE code, const char* file, int line);
	~CodeException() { }
	virtual const char* name() const { return ERR_CODE_NAME[m_errcode]; }
//attr
private:
};

#endif
