#ifndef _BUPFUNX_H_
#define _BUPFUNX_H_
/*
NAME
	NBUPFunx -
LIBRARY 3C++
PAGENAME BUPFunx
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE 

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2001. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION
   BU functions used by different jobs.

ERROR HANDLING
   -

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
   2002-04-03 by YK/SF Lillemor Pettersson (uablan)

LINKAGE
	-

SEE ALSO

*/
#pragma once

#include "buap.h"
#include "BUPMsg.h"
#include "FtpFileSet.h"
#include "PlatformTypes.h"
#include "EventReporter.h"
#include "ACS_CS_API.h"

#include <string>
#include <fstream>

using namespace buap;

namespace BUPFunx {
	//
	// copy files belong to extent into fileset
	//========================================================================
	void setFiles(FtpFileSet& , buap::EXTENT , const string& path, bool useLDD1);

	// copy files belong to extent into fileset (for Cluster SSI)
	//========================================================================
	void setFiles(FtpFileSet_V4& , buap::EXTENT , const string& path, u_int32 useSubFiles);

	//
	// convert cpid to cpname
	//========================================================================
	int convertCPIdtoCPName(CPID cpid, char * name);
	//
	// convert cpname to cpid
	//========================================================================
	int convertCPNametoCPId(const char* cpname, CPID &cpid);
	//
	//convert cpName/CP group to a list of cp id's
	//========================================================================
	int CPNameorGrptoListofCPid(const char* cpname, ACS_CS_API_IdList& cpList);
	//
	// cast or log & throw
	//========================================================================
	template<typename T>
	const T* safe_bup_ptr_cast(const BUPMsg& msg) {
		const T* cmd = bup_ptr_cast<T>(msg);
		if(!cmd) {
			EventReporter::instance().write("Internal error when reading BUP message");
			throw BUPMsg::INTERNAL_ERROR;
		}
		return cmd;
	}
	//
	// create params file (in current dir)
	//========================================================================
	template<typename T>
	void writeParams(T t) {
		std::ofstream outf(PARAMS_FILE);
		if(!outf.good()) {
			EventReporter::instance().write("Failed to create tmp parameter file.");
			throw BUPMsg::INTERNAL_ERROR;
		}
		t->writeParams(outf);
		outf.close();
	}
};

#endif
