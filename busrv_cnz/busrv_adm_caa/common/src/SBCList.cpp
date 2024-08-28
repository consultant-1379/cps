/*
NAME
	SBCLIST -
LIBRARY 3C++
PAGENAME SBCLIST
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
   List of available System Backup Copies (directories (files in APZ term.))
	in current directory.

ERROR HANDLING
   -

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
   2001-12-03 by U/Y/SF Anders Gillgren (qabgill, tag: ag)

LINKAGE
	-

SEE ALSO
	-
	Revision history
	----------------
	2002-10-31 uablan  Added ACS_Trace
*/


#include "Config.h"
#include "SBCList.h"

#include "CPS_BUSRV_Trace.h"
#include "CodeException.h"
#include "FileFunx.h"
#include "BUPFunx.h"
#include "SBCId.h"
#include "TmpCurrDir.h"
#include "LinuxException.h"

#ifndef LOCAL_BUILD					
#include "EnvFMS.h"
#include "fms_cpf_file.h"
#include "fms_cpf_fileiterator.h"
#endif

#include <iostream>
#include <cerrno>
#include <cstdio>
#include <cassert>

//
// ctor 1
//
SBCList::SBCList() {
	newTRACE((LOG_LEVEL_INFO, "SBCList::SBCList()", 0));

	memset(&m_dat, 0xFF, sizeof(m_dat));
	m_dat.count = 0;
	m_range = 0;
	memset(m_sbcList, 0, 128*sizeof(int));
}
//
// ctor 2
//
SBCList::SBCList(SBCId::SBC_RANGE range, bool firstSeqOnly, CPID cpId) : m_range(range) 
{
	newTRACE((LOG_LEVEL_INFO, "SBCList::SBCList(%d, %d)", 0, range, firstSeqOnly));

	update(range, firstSeqOnly, cpId);
}

//
//=============================================================================
SBCList::SBCList(SBCId::SBC_RANGE range, bool firstSeqOnly, const Config::CLUSTER& t) {
	newTRACE((LOG_LEVEL_INFO, "SBCList::SBCList(CLUSTER)", 0));

	update(range, firstSeqOnly, t);
	m_range = 0;
}

//
// get sbcId for position <idx>
//
const SBCId& SBCList::sbcId(u_int32 idx) const {

	newTRACE((LOG_LEVEL_INFO, "SBCList::sbcId(%d)", 0, idx));

	assert(idx < m_dat.count);
	return m_dat.sbcId[idx];
}

//
// Scan all relfsw0 to relfsw128 files and note files that exist
//
void SBCList::scanCpfFiles(const char* cpName)
{
	newTRACE((LOG_LEVEL_INFO, "scanCpfFiles(%s) starts", 0, cpName));

	TRACE((LOG_LEVEL_INFO, "Start scanning for 128 SBCs", 0));
	memset(m_sbcList, 0, 128*sizeof(int));
	SBCId sbcId;
	FMS_CPF_FileIterator iter("", false, cpName);
	FMS_CPF_FileIterator::FMS_CPF_FileData fData;
	while(iter.getNext(fData)) {

		sbcId.id(fData.fileName);
		if (!fData.valid || !sbcId.valid())
			continue;
		m_sbcList[sbcId.id()] = 1;

	}
	TRACE((LOG_LEVEL_INFO, "Finish scanning for 128 SBCs", 0));

	TRACE((LOG_LEVEL_INFO, "scanCpfFiles(%s) returns", 0, cpName));
}

//
// read CPS root again
//
void SBCList::update(SBCId::SBC_RANGE range, bool firstSeqOnly, const Config::CLUSTER& tCluster)
{
	newTRACE((LOG_LEVEL_INFO, "SBCList::update(%d, %d, CLUSTER)", 0, range, firstSeqOnly));

	//char buf1[10];
	try {
		// reset
		memset(&m_dat, 0xFF, sizeof(m_dat));
		m_dat.count = 0;
	}
	catch(LinuxException& x) {
		THROW_LINUX_MSG("SBCList::update(), Failed to reset memory with memset");
	}

	try {
	
		string dir;
		dir = Config::instance().relvolumsw(tCluster);
		TmpCurrDir currDir(dir); // may throw

#ifndef LOCAL_BUILD
		// May throw FMS exceptions
		scanCpfFiles(Config::CLUSTER_STR);
#else
		char buf1[10];
#endif


		int start = range == SBCId::SECOND ? SBCId::SFR_FIRST_ID : SBCId::FFR_FIRST_ID;
		int stop = range == SBCId::FIRST ? SBCId::FFR_LAST_CLUSTER_ID : SBCId::LAST_ID;

		// start from 0 unless range == SECOND_SBC_RANGE
		for(u_int16 i = start; i <= stop; ++i) {
#ifdef LOCAL_BUILD
			sprintf(buf1, "RELFSW%d", i); // change string to constant (SBCId), friend?
			if(FileFunx::dirExists(buf1)) {
#else
			//if (EnvFMS::dirExists(SBCId(static_cast<u_int16>(i)), file, tCluster)) {
			if (m_sbcList[i]) {
#endif
				m_dat.sbcId[m_dat.count++] = i;
				
				if(i == SBCId::FFR_LAST_CLUSTER_ID && firstSeqOnly && range == SBCId::COMPLETE)
					i = SBCId::SFR_FIRST_ID - 1; // need -1 since ++i will execute
				
			} 
			else {
				// if first sequence only, we need to jump to SFR, or break altogether
				if(firstSeqOnly) {
					if(range == SBCId::COMPLETE && i <= SBCId::FFR_LAST_CLUSTER_ID)
						i = SBCId::SFR_FIRST_ID - 1; // need -1 since ++i will execute
					else break;
				}
			}
		}
	}
	catch(LinuxException& x) {
		THROW_XCODE("Failed to find sbc root dir \"relvolumsw\": " << x.what(),
			CodeException::CONFIGURATION_ERROR);
	}
	catch(FMS_CPF_Exception& x) {
		stringstream str;
		str << "SBCList::update, FMS Errorcode : " << x.errorCode() << ", Text: " << x.errorText() << ", Details: "
	    	<< x.detailInfo() << endl;
		TRACE((LOG_LEVEL_ERROR, "%s", 0, str.str().c_str()));
		throw;
	}
}
//
//
//=============================================================================
void SBCList::update(SBCId::SBC_RANGE range, bool firstSeqOnly, CPID cpId) {

	newTRACE((LOG_LEVEL_INFO, "SBCList::update(%d, %d, %d)", 0, range, firstSeqOnly, cpId));

	try {
		// reset
		memset(&m_dat, 0xFF, sizeof(m_dat));
		m_dat.count = 0;
	}
	catch(...) {
		stringstream str;
		str << "SBCList::update(), Failed to reset memory with memset with errno : " << errno;
		TRACE((LOG_LEVEL_ERROR, "%s", 0, str.str().c_str()));
		THROW_LINUX("Failed to reset memory with memset", errno);
	}
	
	try {

		string dir;
		dir = Config::instance().relvolumsw(cpId);
		TmpCurrDir currDir(dir);// may throw

		// start from 0 unless range == SECOND_SBC_RANGE
		int start = range == SBCId::SECOND ? SBCId::SFR_FIRST_ID : SBCId::FFR_FIRST_ID;
		int stop = range == SBCId::FIRST ? SBCId::FFR_LAST_ID : SBCId::LAST_ID;

#ifndef LOCAL_BUILD
		char tname[10];
		tname[0] = '\0';
		if (cpId != 0xFFFF) {
			if (BUPFunx::convertCPIdtoCPName(cpId, tname)) {
				THROW_XCODE_LOG("Cannot convert CPID to CP Name", CodeException::INTERNAL_ERROR);
			}
		}
		// FMS_CPF_File file("tmpFile", tname);
		// May throw FMS exceptions
		scanCpfFiles(tname);

#else
		char buf1[10];
#endif
		
       for(u_int16 i = start; i <= stop; ++i) {
			
#ifdef LOCAL_BUILD					
			sprintf(buf1, "RELFSW%d", i); // change string to constant (SBCId), friend?
			if(FileFunx::dirExists(buf1)) {
#else
			//if (EnvFMS::dirExists(SBCId(static_cast<u_int16>(i)), file, cpId, tname)) {
			if (m_sbcList[i]) {
#endif
				m_dat.sbcId[m_dat.count++] = i;
			}
			else {
				// if first sequence only, we need to jump to SFR, or break altogether
				if(firstSeqOnly) {
					if (range == SBCId::COMPLETE && i < SBCId::SFR_FIRST_ID)
						i = SBCId::SFR_FIRST_ID - 1; // need -1 since ++i will execute
					else break;
				}
			}
		}
	}
	catch(LinuxException& x) {
		stringstream str;
		str << "SBCList::update(), Failed to find sbc root dir \"relvolumsw\" : " << x.what();
		str << " with CodeException::CONFIGURATION_ERROR : " << CodeException::CONFIGURATION_ERROR;
		TRACE((LOG_LEVEL_ERROR, "%s", 0, str.str().c_str()));
		THROW_XCODE("Failed to find sbc root dir \"relvolumsw\": " << x.what(),
			CodeException::CONFIGURATION_ERROR);
	}
#ifndef LOCAL_BUILD					
	catch(FMS_CPF_Exception& x) {
		stringstream ss;
		ss << "SBCList::update(), FMS Errorcode: " << x.errorCode() << ", Text: " << x.errorText()
		   << ", Details: " << x.detailInfo().c_str();
		TRACE((LOG_LEVEL_ERROR, "%s", 0, ss.str().c_str()));
		//EventReporter::instance().write(ss.str());
		throw;
	}
#endif
	TRACE((LOG_LEVEL_INFO, "SBCList::update() returns with m_dat.count = <%d>", 0, m_dat.count));
}
//
//
//
ostream& operator<<(ostream& os, const SBCList& sbc) {
	os << "SBCList count: " << sbc.m_dat.count << "\nItems: ";

	//copy(&sbc.m_dat.sbcId[0], &sbc.m_dat.sbcId[SBCId::LAST_ID - SBCId::FIRST_ID + 1], 
	copy(&sbc.m_dat.sbcId[0], &sbc.m_dat.sbcId[sbc.m_dat.count], 
			ostream_iterator<SBCId>(os, " ")); 
	os << endl;
	return os;
}
