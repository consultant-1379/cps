/*
NAME
	BUPFUNX -
LIBRARY 3C++
PAGENAME BUPFUNX
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE SBCList.h

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
   2002-04-03 by UAB/UKY/SF Lillemor Pettersson (uablan)

LINKAGE
	-

SEE ALSO

*/


#include "BUPFunx.h"
#include "FtpFileSet.h"
#include "SBCFile.h"
//#include "Config.h"
#include "CPS_BUSRV_Trace.h"

#include <assert.h>
#include <iostream>
#include <cctype>

enum {useLDD1 = 1, usePS = 2, useRS = 4 };
//
// utility function
//===========================================================================
void BUPFunx::setFiles(FtpFileSet& files, buap::EXTENT extent, const std::string& path,
							  bool use_LDD1) {
	newTRACE((LOG_LEVEL_INFO, "BUPFunx::setFiles(files, %d, %s)", 0, extent, path.c_str()));

	const char* pc = path.c_str();
	switch(extent) {
	case SMALL:
		files.set(pc, SBCFile::name(SBCFile::BACKUP_INFO), pc, SBCFile::name(SBCFile::SDD));
		break;
	case SMALL_AND_LARGE:
		files.set(pc, SBCFile::name(SBCFile::BACKUP_INFO), 
			pc, SBCFile::name(SBCFile::SDD), 
			pc, SBCFile::name(use_LDD1 ? SBCFile::LDD1 : SBCFile::LDD2));
		break;
	case ALL:
		files.set(pc, SBCFile::name(SBCFile::BACKUP_INFO), 
			pc, SBCFile::name(SBCFile::SDD), 
			pc, SBCFile::name(use_LDD1 ? SBCFile::LDD1 : SBCFile::LDD2),
			pc, SBCFile::name(SBCFile::PS), 
			pc, SBCFile::name(SBCFile::RS));
		break;
	default:
		assert(!"Unreachable code");
		break;
	}
}

//
// utility function (SSI), SPX BUP V4 using BUINFO V3
//===========================================================================
void BUPFunx::setFiles(FtpFileSet_V4& files, buap::EXTENT extent, const std::string& path,
							  u_int32 useSubFiles) {
	newTRACE((LOG_LEVEL_INFO, "(SSI) BUPFunx::setFiles(files, %d, %s)", 0, extent, path.c_str()));

	const char* pc = path.c_str();
	switch(extent) {
	case SMALL:
		files.set(pc, SBCFile::name(SBCFile::BACKUP_INFO), pc, SBCFile::name(SBCFile::SDD));
		break;
	case SMALL_AND_LARGE:
		files.set(pc, SBCFile::name(SBCFile::BACKUP_INFO), 
			pc, SBCFile::name(SBCFile::SDD), 
			pc, SBCFile::name(useSubFiles & useLDD1 ? SBCFile::LDD1 : SBCFile::LDD2));
		break;
		// No diference between NEW_SMALL_AND_LARGE and ALL in SSI, except
		// NEW_SMALL_AND_LARGE is autodump to RELFSW0 only
	case ALL:
	case NEW_SMALL_AND_LARGE:
		files.set(pc, SBCFile::name(SBCFile::BACKUP_INFO), 
			pc,	SBCFile::name(SBCFile::SDD), 
			pc, SBCFile::name(useSubFiles & useLDD1 ? SBCFile::LDD1 : SBCFile::LDD2),
			pc, SBCFile::name(useSubFiles & usePS ? SBCFile::PS : SBCFile::PS2), 
			pc, SBCFile::name(useSubFiles & useRS ? SBCFile::RS : SBCFile::RS2));
		TRACE((LOG_LEVEL_INFO, "BUPFunx::setFiles: &useLDD1=%d, &usePS=%d, &useRS=%d", 0, useSubFiles & useLDD1, useSubFiles & usePS, useSubFiles & useRS));
		TRACE((LOG_LEVEL_INFO, "BUPFunx::setFiles: useSubFiles=%d ", 0, useSubFiles));
		break;
	default: 
		assert(!"Unreachable code");
		break;
	}
}


//convert cpId to cpName
int BUPFunx::convertCPIdtoCPName(CPID cpid, char* name){
	newTRACE((LOG_LEVEL_INFO, "BUPFunx::convertCPIdtoCPName(%d)", 0, cpid));

	ACS_CS_API_Name  cpName;
	ACS_CS_API_NS::CS_API_Result res = ACS_CS_API_NetworkElement::getDefaultCPName(cpid, cpName);

	if (res != ACS_CS_API_NS::Result_Success){
		TRACE((LOG_LEVEL_ERROR, "BUPFunx::convertCPIdtoCPName: getDefaultCPname() failed with error %d", 0, res));
		return res;
	}

	size_t len = cpName.length(); 
	res = cpName.getName(name, len);

	if (res != ACS_CS_API_NS::Result_Success){
		TRACE((LOG_LEVEL_ERROR, "BUPFunx::convertCPIdtoCPName: getName() failed with error %d", 0, res));
		return res;
	}

	// This might be removed
	for (char *p = name; (*p = ::tolower(*p)); ++p) ;

	TRACE((LOG_LEVEL_INFO, "BUPFunx::convertCPIdtoCPName: CP name = %s", 0, name));

	return 0;
}


//convert cpName to cpId
int BUPFunx::convertCPNametoCPId(const char* cpname, CPID  &cpid){
	newTRACE((LOG_LEVEL_INFO, "BUPFunx::convertCPNametoCPId(%s)", 0, cpname));

	ACS_CS_API_CP * CP = ACS_CS_API::createCPInstance();
	if (CP) {
		ACS_CS_API_Name name(cpname);
		// get CP id for cpname
		ACS_CS_API_NS::CS_API_Result res = CP->getCPId(name, cpid);

		TRACE((LOG_LEVEL_INFO, "BUPFunx::convertCPNametoCPId: result = %d", 0, res));

		ACS_CS_API::deleteCPInstance(CP);
		
		//if (res == ACS_CS_API_NS::Result_Success){
		//	return 0;
		//}
		//else {
		//	return res;
		//}
		
		return res == ACS_CS_API_NS::Result_Success ? 0 : res;
	}
	ACS_CS_API::deleteCPInstance(CP);
	return -1;

}

//convert cpName/CP group to a list of cp id's
int BUPFunx::CPNameorGrptoListofCPid(const char* cpname, ACS_CS_API_IdList& cpList){
	newTRACE((LOG_LEVEL_INFO, "BUPFunx::CPNameorGrptoListofCPid(%s)", 0, cpname));

	ACS_CS_API_CPGroup * cpGroup = ACS_CS_API::createCPGroupInstance();

	if (cpGroup) {
		ACS_CS_API_Name name(cpname);

		//Get list of CP ids for group or name
		ACS_CS_API_NS::CS_API_Result res = cpGroup->getGroupMembers(name, cpList);

		ACS_CS_API::deleteCPGroupInstance(cpGroup);
		//if (res == ACS_CS_API_NS::Result_Success) {
		//	return 0;
		//}
		//else {
		//	return res;
		//}
		return res == ACS_CS_API_NS::Result_Success ? 0 : res;
	}
	ACS_CS_API::deleteCPGroupInstance(cpGroup);
	return -1;
}

