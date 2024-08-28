#ifndef _FCPFUNX_H_
#define _FCPFUNX_H_

/*
NAME
	NBUPFunx -
LIBRARY 3C++
PAGENAME FCPFunx
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION
   FC functions used by different jobs.

ERROR HANDLING
   -

DOCUMENT NO
	190 89-CAA 109 0670

AUTHOR
   2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on FCPFunx.h of Windows version

LINKAGE
	-

SEE ALSO
	-

*/

#include "fcap.h"
#include "FCPMsg.h"
#include "FtpFileSet.h"
#include "PlatformTypes.h"
#include "ACS_CS_API.h"
#include "EventReporter.h"
#include <string>
#include <fstream>
using namespace fcap;

namespace FCPFunx {
	//
	// copy file into fileset
	//========================================================================
	void setFiles(FtpFileSet& files, const std::string& path, const char* fileName);

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
	const T* safe_fcp_ptr_cast(const FCPMsg& msg) {
		const T* cmd = fcp_ptr_cast<T>(msg);
		if(!cmd) {
			EventReporter::instance().write("Internal error when reading FCP message");
			throw FCPMsg::INTERNAL_ERROR;
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
			throw FCPMsg::INTERNAL_ERROR;
		}
		t->writeParams(outf);
		outf.close();
	}
};

#endif
