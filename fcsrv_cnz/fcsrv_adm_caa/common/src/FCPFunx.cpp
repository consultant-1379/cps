
/*
NAME
	FCPFUNX -
LIBRARY 3C++
PAGENAME FCPFUNX
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE FCPFunx.h

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
   2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on FCPFunx.cpp of FCSRV in Windows
   and BUSRV's BUPFunx.cpp

LINKAGE
	-

SEE ALSO
	-

Fevision history
----------------


*/

#include "FCPFunx.h"
#include "FtpFileSet.h"
#include "CPS_FCSRV_Trace.h"
#include "Config.h"

#include <assert.h>
#include <iostream>


void FCPFunx::setFiles(FtpFileSet& files, const std::string& path, const char* fileName) {
	newTRACE((LOG_LEVEL_INFO, "FCPFunx::setFiles(files, %s, %s)", 0, path.c_str(), fileName));

	const char* pc = path.c_str();
	files.set(pc, fileName);

}

//convert cpId to cpName
int FCPFunx::convertCPIdtoCPName(CPID cpid, char* name){
	newTRACE((LOG_LEVEL_INFO, "FCPFunx::convertCPIdtoCPName(%d)", 0, cpid));

	ACS_CS_API_Name  cpName;
	ACS_CS_API_NS::CS_API_Result res = ACS_CS_API_NetworkElement::getDefaultCPName(cpid, cpName);

	if (res != ACS_CS_API_NS::Result_Success){
		TRACE((LOG_LEVEL_ERROR, "FCPFunx::convertCPIdtoCPName: get CP name failed with error %d", 0, res));
		return res;
	}

	size_t len = cpName.length();
	res = cpName.getName(name, len);

	if (res != ACS_CS_API_NS::Result_Success){
		TRACE((LOG_LEVEL_ERROR, "FCPFunx::convertCPIdtoCPName: getName failed with error %d", 0, res));
		return res;
	}
	
	// Convert to lower case
	for (char *p = name; (*p = ::tolower(*p)); ++p) ;

	TRACE((LOG_LEVEL_INFO, "FCPFunx::convertCPIdtoCPName: CP name = %s", 0, name));

	return 0;
}

//convert cpName to cpId
int FCPFunx::convertCPNametoCPId(const char* cpname, CPID  &cpid){
	newTRACE((LOG_LEVEL_INFO, "FCPFunx::convertCPNametoCPId(%s)", 0, cpname));

	ACS_CS_API_CP * CP = ACS_CS_API::createCPInstance();
	if (CP) {
		ACS_CS_API_Name name(cpname);
		// get CP id for cpname
		ACS_CS_API_NS::CS_API_Result res = CP->getCPId(name, cpid);

		ACS_CS_API::deleteCPInstance(CP);

		return res == ACS_CS_API_NS::Result_Success ? 0 : res;
	}
	ACS_CS_API::deleteCPInstance(CP);
	return -1;
}
//convert cpName/CP group to a list of cp id's
int FCPFunx::CPNameorGrptoListofCPid(const char* cpname, ACS_CS_API_IdList& cpList){
	newTRACE((LOG_LEVEL_INFO, "FCPFunx::CPNameorGrptoListofCPid(%s)", 0, cpname));

	ACS_CS_API_CPGroup * cpGroup = ACS_CS_API::createCPGroupInstance();

	if (cpGroup) {
		ACS_CS_API_Name name(cpname);

		//Get list of CP ids for group or name
		ACS_CS_API_NS::CS_API_Result res = cpGroup->getGroupMembers(name, cpList);

		ACS_CS_API::deleteCPGroupInstance(cpGroup);

		return res == ACS_CS_API_NS::Result_Success ? 0 : res;
	}
	ACS_CS_API::deleteCPGroupInstance(cpGroup);
	return -1;
}
