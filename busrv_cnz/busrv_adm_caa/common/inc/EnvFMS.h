#ifndef _ENVFMS_H_
#define _ENVFMS_H_
/*
NAME
	ENVFMS - 
LIBRARY 3C++
PAGENAME ENVFMS
HEADER  CPS  
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE EnvFMS.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2001. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the 
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION 
   Encapsulates FMS calls. This is done for two purposes.
   1) Makes it easier to remove FMS.
   2) Allows us to fake the FMS calls when running in "Local test" mode.

ERROR HANDLING
   -

DOCUMENT NO
   190 89-CAA 109 0387

AUTHOR 
   2001-09-17 by U/Y/SF Anders Gillgren (qabgill, tag: ag)

LINKAGE
	-

SEE ALSO 
   -

*/
#pragma once

#include "PlatformTypes.h"
#include "SBCId.h"
#include "ACS_CS_API.h"
#include "Config.h"

#include <string>

class FMS_CPF_File;

using namespace std;
//
//
//===========================================================================
class EnvFMS {
public:
	// all ops on "subfiles"
	static void createFile(SBCId , const char* filename,CPID cpid=0xFFFF);
	static void rename(SBCId, const char* src, const char* dest, CPID cpid=0xFFFF);
	static bool dirExists(SBCId, CPID cpid=0xFFFF);
	static bool dirExists(SBCId, FMS_CPF_File&, CPID cpid=0xFFFF);
	static bool dirExists(SBCId, FMS_CPF_File&, CPID, const char*);
	static size_t cleanDir(SBCId, CPID cpid=0xFFFF);	
	static void deleteFile(SBCId, const char* filename, CPID cpid=0xFFFF);
	static bool fileExists(SBCId, const char* filename, CPID cpid=0xFFFF);

	static void createFile(SBCId , const char* filename, const Config::CLUSTER&);
	static void rename(SBCId, const char* src, const char* dest, const Config::CLUSTER&);
	static bool dirExists(SBCId, const Config::CLUSTER&);
	static bool dirExists(SBCId, FMS_CPF_File&, const Config::CLUSTER&);
	static size_t cleanDir(SBCId, const Config::CLUSTER&);
	static bool fileExists(SBCId, const char* filename, const Config::CLUSTER&);
	static void deleteFile(SBCId, const char* filename, const Config::CLUSTER&);

private:
	static size_t delSubFile(const char* fileName, const char* cpName = "");
};

#endif
