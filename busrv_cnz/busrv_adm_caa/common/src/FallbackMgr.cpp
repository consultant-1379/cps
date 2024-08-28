/*
NAME
	File_name:FallbackMgr.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

The Copyright to the computer program(s) herein is the property of Ericsson
Utvecklings AB, Sweden.
The program(s) may be used and/or copied only with the written permission from
Ericsson Utvecklings AB or in accordance with the terms and conditions
stipulated in the agreement/contract under which the program(s) have been
supplied.

DESCRIPTION
	Class handling the fallback SBC selection.

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
	2002-07-04 by U/Y/SF Anders Gillgren (qabgill, tag :ag)

SEE ALSO
	-

Revision history
----------------
2002-07-04 qabgill Created
2003-01-08 uabalan Added code for the function and added ACS_TRACE
2010-05-23 xdtthng Modified for SSI
2012-12-24 xdtthng Modified for APG43L BC
*/


#include "Config.h"
#include "FallbackMgr.h"

#include "CPS_BUSRV_Trace.h"
#include "CodeException.h"
#include "DataFile.h"
#include "SBCList.h"
#include "LinuxException.h"

#include <algorithm>
#include <iterator>
#include <boost/bind.hpp>

//
// ctor
//===========================================================================
FallbackMgr::FallbackMgr(CPID cpId) {

	newTRACE((LOG_LEVEL_INFO, "FallbackMgr::FallbackMgr(%d)", 0, cpId));

	// Make sure that we get the latest values from file.
	Config::instance().retreive();

	// Make a list of existing SBCs (firstSequence) in FFR and (SFR)	
	SBCList sbcFFRList(SBCId::FIRST, true, cpId);
	SBCList sbcSFRList(SBCId::SECOND, true, cpId);

	// RELSFW0 must exit
	if (sbcFFRList.count() && (sbcFFRList.sbcId(0) == SBCId((u_int16) 0)))
	{

		// calculate the real number of sbc's to be in vector
		size_t sbcFFRCount = Config::instance().getSbcToReloadFromFFR(cpId) + 1;
		size_t sbcSFRCount = 0;
		if (Config::instance().getSbcToReloadFromSFR(cpId))
			sbcSFRCount = Config::instance().getSbcToReloadFromSFR(cpId) - 99;

		// Get the specified SBC from the two lists into a vector
		// Files in reload sequence, which do no longer exist at reload instance,
		// are not considered as part of the reload sequence
		m_vec.reserve(sbcFFRCount + sbcSFRCount + 1);
		back_insert_iterator<SBCIdVector> ins(m_vec);
		remove_copy_if(&sbcFFRList.m_dat.sbcId[0], &sbcFFRList.m_dat.sbcId[sbcFFRCount], ins,
					!boost::bind<bool>(not_equal_to<u_int16>(),
							boost::bind<u_int16>(&SBCId::id, _1),
							0xffff));
		remove_copy_if(&sbcSFRList.m_dat.sbcId[0], &sbcSFRList.m_dat.sbcId[sbcSFRCount], ins,
				!boost::bind<bool>(not_equal_to<u_int16>(),
						boost::bind<u_int16>(&SBCId::id, _1),
						0xffff));

		TRACE((LOG_LEVEL_ERROR, "Reload vector with size <%d> as", 0, m_vec.size()));
		stringstream ss;
		copy(m_vec.begin(), m_vec.end(), ostream_iterator<u_int16>(ss, " "));
		TRACE((LOG_LEVEL_ERROR, "<%s>", 0, ss.str().c_str()));
	}
	else
	{
		//XXX
		assert(!"Missing SBC 0 (RELFSW0)");
		// To Do: log to EAH
		TRACE((LOG_LEVEL_ERROR, "FallbackMgr::FallbackMgr(); Missing SBC 0 (RELFSW0)", 0));
		throw BUPMsg::SBC_NOT_FOUND;
	}
}
//
//
//===========================================================================
SBCId FallbackMgr::currLoadId(CPID cpId) const {

	newTRACE((LOG_LEVEL_INFO, "FallbackMgr::currLoadId(%d)", 0, cpId));

	if (DataFile::startReloadSeq(cpId))  // is set to true by bupdef
	{
		DataFile::startReloadSeq(false, cpId);
		return SBCId((u_int16) 0);
	}
	else 
	{
		//Check if fallbacktime has expired
		DateTime now;
		DateTime lastLoadTime = DataFile::lastLoadedTimeForFallback(cpId);
        DateTime expireTime = lastLoadTime;
        
		//expireTime.addMinutes(Config::instance().getSupervisionTime(cpId));
		// Expanding the above for debugging purposes
		u_int32 supTime = Config::instance().getSupervisionTime(cpId);
		u_int16 lastLoadSbcId = DataFile::lastLoadedSBC(cpId).id();
		TRACE((LOG_LEVEL_INFO, "Supervision time is <%d>", 0, supTime));
		ostringstream ostime;
		ostime << "Last fallback attempt of <RELFSW" << lastLoadSbcId << "> at " << lastLoadTime;
		TRACE((LOG_LEVEL_INFO, "%s", 0, ostime.str().c_str()));
		expireTime.addMinutes(supTime);
		TRACE((LOG_LEVEL_INFO, "Expire time <%02d:%02d:%02d>", 0, expireTime.hour(), expireTime.minute(), expireTime.second()));

		now.now();
		TRACE((LOG_LEVEL_INFO, "Current time <%02d:%02d:%02d>", 0, now.hour(), now.minute(), now.second()));
		if (now.cmp(expireTime) > 0)
			return SBCId((u_int16) 0); // time has expire - return relfsw0

		// Reset the start off reload sequence flag
		DataFile::startReloadSeq(false, cpId);

		// Get next SBC in list after lastLoadedSBC or if it is the last SBC get current SBC
		SBCId lastId = DataFile::lastLoadedSBC(cpId);
		unsigned int i = 0;
		while ( m_vec[i].id() <= lastId.id() && i < m_vec.size()-1 )
			++i;

		TRACE((LOG_LEVEL_INFO, "m_vec size = %d, m_vec[%d] = %d", 0, m_vec.size(), i, m_vec[i].id()));
		return m_vec[i];
	}
}
//
//
//===========================================================================
bool FallbackMgr::lastLoadableSBC(CPID cpId) const {

	newTRACE((LOG_LEVEL_INFO, "FallbackMgr::lastLoadableSBC(%d)", 0, cpId));

	// Check if last loaded SBC is last in the list
	return (m_vec.back() == DataFile::lastLoadedSBC(cpId));
}
//
//
//===========================================================================
void FallbackMgr::updateLoadTime(CPID cpId) {

	newTRACE((LOG_LEVEL_INFO, "FallbackMgr::updateLoadTime(%d)", 0, cpId));

	DateTime now;
	now.now();
	DataFile::lastLoadedTimeForFallback(now, cpId);
}

///////////////////////////////////////////////////////////////////////////////
//
// ctor
//===========================================================================
FallbackMgr_V4::FallbackMgr_V4(const Config::CLUSTER &tg) : m_vec(), m_sfrStart(0), 
				m_srmReloadFile(0xFF), m_startReloadSeq(false) {

	newTRACE((LOG_LEVEL_INFO, "FallbackMgr_V4::FallbackMgr_V4(CLUSTER at CP %s)", 0, tg.getCPName().c_str()));

	// Make sure that we get the latest values from file.
	Config::instance().retreive();

	// Make a list of existing SBCs (firstSequence) in FFR and (SFR)
	SBCList ffr(SBCId::FIRST, true, tg);
	SBCList sfr(SBCId::SECOND, true, tg);

	// RELSFW0 must exit
	if(ffr.count() && (ffr.sbcId(0) == SBCId((u_int16) 0))) {

		m_ffr = Config::instance().getSbcToReloadFromFFR(tg);
		m_sfr = Config::instance().getSbcToReloadFromSFR(tg);
		m_startReloadSeq = DataFile::startReloadSeq(tg);
		m_srmReloadFile = DataFile::srmReloadSBC(tg);

		// calculate the real number of sbc's to be in vector
		u_int16 sbcSFRCount = 0;
		if (m_sfr) {
			sbcSFRCount = m_sfr - SBCId::FFR_LAST_ID;
			m_sfrStart = sfr.m_dat.sbcId[0].id();    
		}

		// Get the specified SBC from the two lists into a vector
		// Files in reload sequence, which do no longer exist at reload instance,
		// are not considered as part of the reload sequence
		m_vec.reserve(m_ffr + 1 + sbcSFRCount);
		back_insert_iterator<SBCIdVector> ins(m_vec);
		remove_copy_if(&ffr.m_dat.sbcId[0], &ffr.m_dat.sbcId[m_ffr + 1], ins,
				!boost::bind<bool>(not_equal_to<u_int16>(),
						boost::bind<u_int16>(&SBCId::id, _1),
						0xffff));
		remove_copy_if(&sfr.m_dat.sbcId[0], &sfr.m_dat.sbcId[sbcSFRCount], ins,
				!boost::bind<bool>(not_equal_to<u_int16>(),
						boost::bind<u_int16>(&SBCId::id, _1),
						0xffff));

		TRACE((LOG_LEVEL_ERROR, "Reload vector with size <%d> as", 0, m_vec.size()));
		stringstream ss;
		copy(m_vec.begin(), m_vec.end(), ostream_iterator<u_int16>(ss, " "));
		TRACE((LOG_LEVEL_ERROR, "<%s>", 0, ss.str().c_str()));
	}
	else {
		//XXX
		//assert(!"Missing SBC 0 (RELFSW0)");		// To Do: log to EAH
		TRACE((LOG_LEVEL_ERROR, "FallbackMgr_V4::FallbackMgr_V4(); Missing SBC 0 (RELFSW0)", 0));
		throw BUPMsg::SBC_NOT_FOUND;

	}
}

SBCId FallbackMgr_V4::currLoadId(const Config::CLUSTER &tg) {

	newTRACE((LOG_LEVEL_INFO, "FallbackMgr_V4::currLoadId(CLUSTER)", 0));
	const Config::CLUSTER tc;

	// The current time
	DateTime now;	
	now.now();
	TRACE((LOG_LEVEL_INFO, "now time : %02d%02d", 0, now.hour(), now.minute()));

	// Check if the SRM Reload file has expired when it is used for the first time
	// The SRM Reload File time stamp is at CLUSTER/BCx level
	u_int16 lastLoadedSBC = DataFile::lastLoadedSBC(tg);
    if (m_startReloadSeq) {
		DataFile::startReloadSeq(false, tc);	// At CLUSTER level
		DataFile::startReloadSeq(false, tg);	// At CLUSER/BCx level
		m_startReloadSeq  =  false;

		if (m_srmReloadFile != 0xFF && lastLoadedSBC <= m_srmReloadFile) { 
			// Check if time expired
			DateTime srmTime = DataFile::srmReloadSBCTime(tg);
			DateTime srmExpireTime(srmTime.year(), srmTime.month(),srmTime.day(), srmTime.daylight(),
						srmTime.hour(), srmTime.minute(), srmTime.second());
			srmExpireTime.addMinutes(Config::instance().getSupervisionTime(tc));
			TRACE((LOG_LEVEL_INFO, "SRM expire time : %02d%02d", 0, srmExpireTime.hour(), srmExpireTime.minute()));
			if (now.cmp(srmExpireTime) > 0) {
				TRACE((LOG_LEVEL_WARN, "SRM Time expired, return relfsw0", 0));
				m_srmReloadFile = 0xFF;
				DataFile::srmReloadSBC(m_srmReloadFile, tg);
				return (u_int16)0; // time has expire - return relfsw0
			}
			return SBCId(m_srmReloadFile);
		}
		else
			return (u_int16)0;
	}		

	//Check if fallbacktime has expired
	DateTime lastLoadTime = DataFile::lastLoadedTimeForFallback(tg);
	DateTime expireTime(lastLoadTime.year(), lastLoadTime.month(),lastLoadTime.day(),lastLoadTime.daylight(),
								lastLoadTime.hour(),lastLoadTime.minute(),lastLoadTime.second());

	// This is configuration data under CLUSTER rather than BCx directory
	expireTime.addMinutes(Config::instance().getSupervisionTime(tc));
	TRACE((LOG_LEVEL_INFO, "expire time : %02d%02d", 0, expireTime.hour(), expireTime.minute()));

	if (now.cmp(expireTime) > 0) {
		TRACE((LOG_LEVEL_WARN, "Time expired, return relfsw0", 0));
		m_srmReloadFile = 0xFF;
		DataFile::srmReloadSBC(m_srmReloadFile, tg);
		return (u_int16)0; // time has expire - return relfsw0
	}

	// Reset the start off reload sequence flag
	DataFile::startReloadSeq(false, tc);
	DataFile::startReloadSeq(false, tg);

	// Get next SBC in list after lastLoadedSBC or if it is the last SBC get current SBC
	unsigned int i = 0;
	while ( m_vec[i].id() <= lastLoadedSBC && i < m_vec.size()-1 )
		++i;

	TRACE((LOG_LEVEL_INFO, "m_vec size = %d, m_vec[%d] = %d", 0, m_vec.size(), i, m_vec[i].id()));
	return m_vec[i];
	
}

//===========================================================================
bool FallbackMgr_V4::lastLoadableSBC(u_int16 lastLoadedSBC) const {

	newTRACE((LOG_LEVEL_INFO, "FallbackMgr_V4::lastLoadableSBC(%d)", 0, lastLoadedSBC));
	bool res = m_vec.back() == SBCId((u_int16) lastLoadedSBC);
	TRACE((LOG_LEVEL_INFO, "FallbackMgr_V4::lastLoadableSBC(%d) returns %d", 0, lastLoadedSBC, res));
	return res;
}
//
//===========================================================================
void FallbackMgr_V4::updateLoadTime(const Config::CLUSTER &tg) {

	newTRACE((LOG_LEVEL_INFO, "FallbackMgr::updateLoadTime(CLUSTER)", 0));

	DateTime now;
	now.now();
	Config::CLUSTER tc;
	DataFile::lastLoadedTimeForFallback(now, tc);
	DataFile::lastLoadedTimeForFallback(now, tg);
}

ostream& operator<<(ostream& os, const FallbackMgr_V4& man)
{
    os << "<ffr: " << man.m_ffr << ">";
    os << "<sfr: " << man.m_sfr << ">";
    copy(man.m_vec.begin(), man.m_vec.end(), ostream_iterator<SBCId>(os));
    
    return os;
}

// Implement Ola's algorithm of SRM interaction with BUSRV Fallback Manager
bool FallbackMgr_V4::setSRMReloadFile(int fn)
{
        
    // Search for fn is in the reload range
    const SBCId reloadFile(static_cast<u_int16>(fn));   
    if (binary_search(m_vec.begin(), m_vec.end(), reloadFile)) {
        m_srmReloadFile = fn;   // Accept SRM reload file setting
        return true;            // and we are done   
    }
    
    // At this point, the setting of SRM reload file has failed
    // Search for special conditions which can approve the srm reload file
    
    // Strange requirements lead to weirdo looking code
    // Note every if statement returns on its own
    bool res = false;
	m_srmReloadFile = 0xFF;

    // Neither FFR nor SFR specified
    if (m_ffr == 0 && m_sfr == 0) {    
        if (fn != 0 && fn != 1)
            return false;
            
        // Only accept fn == 0 or fn == 1 
        // It is treated as fn == 0 for both cases
        m_srmReloadFile = 0;
        return true;
    }
    
    // FFR specified and SFR not specified
    if (m_ffr != 0 && m_sfr == 0) {
        if (fn == m_ffr + 1) {
            m_srmReloadFile = m_ffr;
            return true;
        }
        return false;
    }

    // Two cases are covered here 
    // a) No FFR, but SFR specified
    // b) Both FFR and SFR specified   
    if (fn == m_ffr + 1) {
        m_srmReloadFile = m_sfrStart;
        return true;
    }
    else if (fn == m_sfr + 1) {
        m_srmReloadFile = m_sfr;  
        return true;
    }
 
    // Set SRM Reload File has failed. The search for all extra conditions which
    // can approve the srm reload file also failed
    // Finally, it just fails here   
    return res;    
}


void FallbackMgr_V4::setSBCReloadFile(int fn, GASrmMsg::ERROR_CODE& errorCode)
{
	newTRACE((LOG_LEVEL_INFO, "FallbackMgr::setSBCReloadFile(%d)", 0, fn));

	errorCode = GASrmMsg::RC_FILE_OUT_OF_RANGE;
	try {
		const Config::CLUSTER tc;
		Config::instance().createDirStructure(tc);

		DateTime now;
		now.now();

		bool res = setSRMReloadFile(fn);			// Attempt to set the new srm reload file
		if (res) {									// If successfull
			DataFile::startReloadSeq(true, tc);		// Start reload sequence
			DataFile::lastLoadedSBC(SBCId(static_cast<u_int16 >(0)), tc);	// Reset last loaded sbc
			DataFile::srmReloadSBCTime(now, tc);
			alignReloadFile(now);
			errorCode = GASrmMsg::RC_OK;
			// Logging
			ostringstream fptr;
			fptr << "Successful FallbackMgr::setSBCReloadFile(" << fn << ") at " << now;
			TRACE((LOG_LEVEL_INFO, "%s", 0, fptr.str().c_str()));
			EventReporter::instance().write(fptr.str().c_str());
		}
		DataFile::srmReloadSBC(SBCId(m_srmReloadFile), tc);
	}
	catch (...) {
		errorCode = GASrmMsg::RC_INTERNAL_ERROR;
	}
}


void FallbackMgr_V4::alignReloadFile(DateTime now)
{
	newTRACE((LOG_LEVEL_INFO, "FallbackMgr::alignReloadFile(DateTime now)", 0));
	try {
		ACS_CS_API_IdList cpList;
		ACS_CS_API_CP * CP = ACS_CS_API::createCPInstance();

		ACS_CS_API_NS::CS_API_Result res = CP->getCPList(cpList);

		if (res != ACS_CS_API_NS::Result_Success) {
			ACS_CS_API::deleteCPInstance(CP);
			throw BUPMsg::INTERNAL_ERROR;
		}
		DateTime nullTime;
		nullTime.setNull();
		for (u_int32 i = 0; i < cpList.size(); i++) {
			CPID cpId = cpList[i];
			if (cpId > 63) continue;
			const Config::CLUSTER t(cpId, true);

			DataFile::startReloadSeq(true, t);
			DataFile::lastLoadedSBC(SBCId(static_cast<u_int16 >(0)), t);	// Reset last loaded sbc
			DataFile::lastLoadedTimeForFallback(nullTime, t);
			DataFile::srmReloadSBC(SBCId(m_srmReloadFile), t);
			DataFile::srmReloadSBCTime(now, t);
		}
		ACS_CS_API::deleteCPInstance(CP);
	}
	catch (...) {
		throw BUPMsg::INTERNAL_ERROR;
	}
}

