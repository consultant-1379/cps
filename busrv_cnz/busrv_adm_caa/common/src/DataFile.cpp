/*
NAME
	File_name:DataFile.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

The Copyright to the computer program(s) herein is the property of Ericsson
Utvecklings AB, Sweden.
The program(s) may be used and/or copied only with the written permission from
Ericsson Utvecklings AB or in accordance with the terms and conditions
stipulated in the agreement/contract under which the program(s) have been
supplied.

DESCRIPTION
	Class handling busrv.dat, which includes reload & cmdlog data.

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
	2002-05-13 by U/Y/SF Anders Gillgren (qabgill, tag :ag)

SEE ALSO
	-

Revision history
----------------
2002-05-13 qabgill Created
2002-11-04 uablan  Add ACS Trace
2003-01-08 uablan  Change from expire to last loaded
2003-01-21 uablan  Added loadingInProgress and startReloadSeq Items and functions.
2006-03-02 uablan  Remove loadingInProgress. (HG62821)
2009-09-01 xdgthng SSI project
*/

#include <iomanip>
#include <sstream>
#include <string>

#include "DataFile.h"
#include "ParseIni.h"
#include "LinuxException.h"
#include "ACS_CS_API.h"
#include "CPS_Utils.h"

#include "BUPMsg.h"

#include "CPS_BUSRV_Trace.h"
#include "CriticalSection.h"

#if 0 
#include <assert.h>
#endif 

using namespace ParseINIFunx;

// static inits

const char* DataFile::SECT_LOAD = "Load";
const char* DataFile::SECT_DUMP = "Dump";
const char* DataFile::SECT_CMDLOG = "Command Log";
const char* DataFile::SECT_SWITCH = "SBC Switch";
const char* DataFile::SECT_SRM_RELOAD = "SRM Reload";
const char* DataFile::SECT_LOAD_INFORMATION = "Loading Information";

const char* DataFile::ITEM_SRM_RELOAD_SBC = "SRM Reload SBC";
const char* DataFile::ITEM_SRM_RELOAD_SBC_YEAR = "SRM Reload SBC - Year";
const char* DataFile::ITEM_SRM_RELOAD_SBC_MONTH = "SRM Reload SBC - Month";
const char* DataFile::ITEM_SRM_RELOAD_SBC_DAY = "SRM Reload SBC - Day";
const char* DataFile::ITEM_SRM_RELOAD_SBC_HOUR = "SRM Reload SBC - Hour";
const char* DataFile::ITEM_SRM_RELOAD_SBC_MIN = "SRM Reload SBC - Minute";
const char* DataFile::ITEM_SRM_RELOAD_SBC_SEC = "SRM Reload SBC - Second";
const char* DataFile::ITEM_SRM_RELOAD_SBC_DAYLIGHT = "SRM Reload SBC - Daylight";

const char* DataFile::ITEM_SWITCH = "Ongoing SBC Switch";

const char* DataFile::ITEM_LAST_LOADED_SBC = "Last loaded SBC";
const char* DataFile::ITEM_LAST_LOADED_TIME_FOR_FALLBACK_YEAR = "Last loaded time for fallback - Year";
const char* DataFile::ITEM_LAST_LOADED_TIME_FOR_FALLBACK_MONTH = "Last loaded time for fallback - Month";
const char* DataFile::ITEM_LAST_LOADED_TIME_FOR_FALLBACK_DAY = "Last loaded time for fallback - Day";
const char* DataFile::ITEM_LAST_LOADED_TIME_FOR_FALLBACK_HOUR = "Last loaded time for fallback - Hours";
const char* DataFile::ITEM_LAST_LOADED_TIME_FOR_FALLBACK_MIN = "Last loaded time for fallback - Minute";
const char* DataFile::ITEM_LAST_LOADED_TIME_FOR_FALLBACK_SEC = "Last loaded time for fallback - Second";
const char* DataFile::ITEM_LAST_LOADED_TIME_FOR_FALLBACK_DAYLIGHT = "Last loaded time for fallback - Daylight";

const char* DataFile::ITEM_LOADING = "Ongoing Load";
const char* DataFile::ITEM_LOADING_CPS = "Loading CPs";

const char* DataFile::ITEM_LOADING_START_TIME_YEAR = "Start time for ongoing load - Year";
const char* DataFile::ITEM_LOADING_START_TIME_MONTH = "Start time for ongoing load - Month";
const char* DataFile::ITEM_LOADING_START_TIME_DAY = "Start time for ongoing load - Day";
const char* DataFile::ITEM_LOADING_START_TIME_HOUR = "Start time for ongoing load - Hours";
const char* DataFile::ITEM_LOADING_START_TIME_MIN = "Start time for ongoing load - Minute";
const char* DataFile::ITEM_LOADING_START_TIME_SEC = "Start time for ongoing load - Second";
const char* DataFile::ITEM_LOADING_START_TIME_DAYLIGHT = "Start time for ongoing load - Daylight";
const char* DataFile::ITEM_LOADING_ONGOING_SBCID = "Ongoing Load SBCId";

const char* DataFile::ITEM_DUMPING = "Ongoing Dump";
const char* DataFile::ITEM_DUMPING_CPS = "Dumping CPs";
const char* DataFile::ITEM_DEFAULT_CPS = "0 1";

const char* DataFile::ITEM_DUMPING_START_TIME_YEAR = "Start time for ongoing dump - Year";
const char* DataFile::ITEM_DUMPING_START_TIME_MONTH = "Start time for ongoing dump - Month";
const char* DataFile::ITEM_DUMPING_START_TIME_DAY = "Start time for ongoing dump - Day";
const char* DataFile::ITEM_DUMPING_START_TIME_HOUR = "Start time for ongoing dump - Hours";
const char* DataFile::ITEM_DUMPING_START_TIME_MIN = "Start time for ongoing dump - Minute";
const char* DataFile::ITEM_DUMPING_START_TIME_SEC = "Start time for ongoing dump - Second";
const char* DataFile::ITEM_DUMPING_START_TIME_DAYLIGHT = "Start time for ongoing dump - Daylight";
const char* DataFile::ITEM_DUMPING_ONGOING_SBCID = "Ongoing Dump SBCId";
const char* DataFile::ITEM_DUMPEND_QUEUED_SBCID = "DumpEnd Queued SBCId";

const char* DataFile::ITEM_START_OF_RELOAD_SEQUENCE = "Start of reload sequence";
const char* DataFile::ITEM_CURRENT = "Current";

const ParseINIFunx::PARSE_TIME_KEY DataFile::LastLoadedTimeForFallBack_Key = {
    ITEM_LAST_LOADED_TIME_FOR_FALLBACK_YEAR,
    ITEM_LAST_LOADED_TIME_FOR_FALLBACK_MONTH,
    ITEM_LAST_LOADED_TIME_FOR_FALLBACK_DAY,
    ITEM_LAST_LOADED_TIME_FOR_FALLBACK_HOUR,
    ITEM_LAST_LOADED_TIME_FOR_FALLBACK_MIN,
    ITEM_LAST_LOADED_TIME_FOR_FALLBACK_SEC,
    ITEM_LAST_LOADED_TIME_FOR_FALLBACK_DAYLIGHT
};

const ParseINIFunx::PARSE_TIME_KEY DataFile::StartTimeForOngoingLoad_Key = {
    ITEM_LOADING_START_TIME_YEAR,
    ITEM_LOADING_START_TIME_MONTH,
    ITEM_LOADING_START_TIME_DAY,
    ITEM_LOADING_START_TIME_HOUR,
    ITEM_LOADING_START_TIME_MIN,
    ITEM_LOADING_START_TIME_SEC,
    ITEM_LOADING_START_TIME_DAYLIGHT
};

const ParseINIFunx::PARSE_TIME_KEY DataFile::StartTimeForOngoingDump_Key = {
    ITEM_DUMPING_START_TIME_YEAR,
    ITEM_DUMPING_START_TIME_MONTH,
    ITEM_DUMPING_START_TIME_DAY,
    ITEM_DUMPING_START_TIME_HOUR,
    ITEM_DUMPING_START_TIME_MIN,
    ITEM_DUMPING_START_TIME_SEC,
    ITEM_DUMPING_START_TIME_DAYLIGHT
};

const ParseINIFunx::PARSE_TIME_KEY DataFile::SrmReloadSBC_Key = {
    ITEM_SRM_RELOAD_SBC_YEAR,
    ITEM_SRM_RELOAD_SBC_MONTH,
    ITEM_SRM_RELOAD_SBC_DAY,
    ITEM_SRM_RELOAD_SBC_HOUR,
    ITEM_SRM_RELOAD_SBC_MIN,
    ITEM_SRM_RELOAD_SBC_SEC,
    ITEM_SRM_RELOAD_SBC_DAYLIGHT
};

const int DataFile::BitVector::s_wordSize = sizeof(u_int32) * 8;

// There is ONLY one CriticalSection for DataFile
CriticalSection& DataFile::CS_DataFile()
{
    // This is to ensure that s_cs is constructed exactly once on first use
    static CriticalSection s_cs;
    return s_cs;
}

void DataFile::BitVector::set(const char* s)
{
	//newTRACE(("DataFile::BitVector::set(%s)", 0, s));

	istringstream is(s);
	is >> hex >> m_data.d32[1] >> m_data.d32[0];
	if (is.fail()) {
		//TRACE(("DataFile::BitVector::set fails", 0));
		clearAllBits();
	}
	//else
	//	TRACE(("DataFile::BitVector::set OK", 0));
}


ostream &operator<<(ostream& os, const DataFile::BitVector& bv)
{
    ios::fmtflags flg(os.flags());
	os << hex << setw(8) << setfill('0') << 
			bv.m_data.d32[1] << " " 
			<< setw(8) <<bv.m_data.d32[0] << dec;
    os.flags(flg);
			
	return os;
}

DataFile::BitVector& DataFile::loadingCPs()
{
	static BitVector s_loadingCPs;
	return s_loadingCPs;
}

DataFile::BitVector& DataFile::dumpingCPs()
{
	static BitVector s_dumpingCPs;
	return s_dumpingCPs;
}

DataFile::BitVector& DataFile::readLoadVector()
{
	AutoCS a(CS_DataFile());
	return loadingCPs();
}

DataFile::BitVector& DataFile::readDumpVector()
{
	AutoCS a(CS_DataFile());
	return dumpingCPs();
}

//
// Read the information about loading and dumping stored on the disk into memory
// at start up of BUSRV.
// This is the beginning of all, thus there will be no Critical Section control
//
//==================================================================================
void DataFile::initIntActData(const Config::CLUSTER& t)
{
	newTRACE((LOG_LEVEL_INFO, "DataFile::initIntActData(CLUSTER)", 0));

	BitVector& load = loadingCPs();
	BitVector& dump = dumpingCPs();
	char buff[25];

	if (GetPrivateProfileString(SECT_DUMP, ITEM_DUMPING_CPS, ITEM_DEFAULT_CPS, buff, 25, 
		Config::instance().datFile(t).c_str()) ) {

		//TRACE(("Dumping CPs buffer is: 0x%s", 0, buff));
		dump.set(buff);
	}
	
	if (GetPrivateProfileString(SECT_LOAD, ITEM_LOADING_CPS, ITEM_DEFAULT_CPS, buff, 25, 
		Config::instance().datFile(t).c_str()) ) {

		//TRACE(("Loading CPs buffer is: 0x%s", 0, buff));
		load.set(buff);
	}

	ostringstream dumpOut;
	dumpOut << dump;
	TRACE((LOG_LEVEL_INFO, "Dumping CPs are: 0x%s", 0, dumpOut.str().c_str()));

	ostringstream loadOut;
	loadOut << load;
	TRACE((LOG_LEVEL_INFO, "Loading CPs are: 0x%s", 0, loadOut.str().c_str()));
}



//
// static
//===========================================================================
void DataFile::srmReloadSBC(SBCId sbcId, const Config::CLUSTER& t) {

	newTRACE((LOG_LEVEL_INFO, "DataFile::srmReloadSBC(%d, CLUSTER)", 0, sbcId.id()));

	AutoCS a(CS_DataFile());
	using cps_utils::to_string;
	//char buf[25];
	//itoa(sbcId.id(), buf, 10);
	if(!WritePrivateProfileString(SECT_SRM_RELOAD, ITEM_SRM_RELOAD_SBC, to_string(sbcId.id()).c_str(),
		Config::instance().datFile(t).c_str())) {
		THROW_LINUX_MSG("Failed to write " << ITEM_SRM_RELOAD_SBC << " in " <<
			Config::instance().datFile(t).c_str());
	}
}

//
// static
//===========================================================================
void DataFile::srmReloadSBCTime(DateTime dt, const Config::CLUSTER& t) {

	newTRACE((LOG_LEVEL_INFO, "DataFile::srmReloadSBCTime(%d, %d, %d, %d, %d, CLUSTER)", 0, dt.year(), dt.month(), dt.day(), dt.hour(),dt.minute()));

	AutoCS a(CS_DataFile());
	
	#if 0
	char buf[25];
	string fs = Config::instance().datFile(t);
	const char *fn = fs.c_str();

	// The following does not work under MS VS .net 2003
	//const char* fn = Config::instance().datFile(t).c_str();
	TRACE(("Full file name is %s", 0, fn));

	if(!WritePrivateProfileString(SECT_SRM_RELOAD, ITEM_SRM_RELOAD_SBC_YEAR,
			itoa(dt.year(), buf, 10), fn) ||
		!WritePrivateProfileString(SECT_SRM_RELOAD, ITEM_SRM_RELOAD_SBC_MONTH,
			itoa(dt.month(), buf, 10), fn) ||
		!WritePrivateProfileString(SECT_SRM_RELOAD, ITEM_SRM_RELOAD_SBC_DAY,
			itoa(dt.day(), buf, 10), fn) ||
		!WritePrivateProfileString(SECT_SRM_RELOAD, ITEM_SRM_RELOAD_SBC_HOUR,
			itoa(dt.hour(), buf, 10), fn) ||
		!WritePrivateProfileString(SECT_SRM_RELOAD, ITEM_SRM_RELOAD_SBC_MIN,
			itoa(dt.minute(), buf, 10), fn) ||
		!WritePrivateProfileString(SECT_SRM_RELOAD, ITEM_SRM_RELOAD_SBC_SEC,
			itoa(dt.second(), buf, 10), fn) ||
		!WritePrivateProfileString(SECT_SRM_RELOAD, ITEM_SRM_RELOAD_SBC_DAYLIGHT,
			itoa(dt.daylight(), buf, 10), fn)) {
		THROW_WIN32_MSG("Failed to write srm reload time setting in " <<
			fn);
	}
	#endif
	
    if (!WritePrivateProfile(SECT_SRM_RELOAD, SrmReloadSBC_Key, dt, Config::instance().datFile(t).c_str())) {
    	TRACE((LOG_LEVEL_ERROR, "Failed to write; throw exception", 0));
		THROW_LINUX_MSG("Failed to write srm reload sbc time in " <<
			Config::instance().datFile(t).c_str());
	}
}

//
// static
//===========================================================================
DateTime DataFile::srmReloadSBCTime(const Config::CLUSTER& t) 
{
	newTRACE((LOG_LEVEL_INFO, "DateTime DataFile::srmReloadSBCTime(const Config::CLUSTER)", 0));
	AutoCS a(CS_DataFile());
	return GetPrivateProfile(SECT_SRM_RELOAD, SrmReloadSBC_Key, 
	       Config::instance().datFile(t).c_str());
}

// static
//===========================================================================
SBCId DataFile::srmReloadSBC(const Config::CLUSTER& t) {

	newTRACE((LOG_LEVEL_INFO, "DataFile::srmReloadSBC(Config::CLUSTER)", 0));

	AutoCS a(CS_DataFile());
	SBCId sbcId(static_cast<u_int16>(GetPrivateProfileInt(SECT_SRM_RELOAD, ITEM_SRM_RELOAD_SBC, 0xFF,
		Config::instance().datFile(t).c_str())));
	TRACE((LOG_LEVEL_INFO, "srmReloadSBC return: %d", 0, sbcId.id()));
	return sbcId;
}


void DataFile::setLoading(CPID cpId, const Config::CLUSTER& t)
{
	AutoCS a(CS_DataFile());
	BitVector &v = loadingCPs();
	v.setBit(cpId);
	ostringstream vOut;
	vOut << v;
	WritePrivateProfileString(SECT_LOAD, ITEM_LOADING_CPS, vOut.str().c_str(), 
		Config::instance().datFile(t).c_str());
}


//
// static
//===========================================================================
void DataFile::lastLoadedSBC(SBCId sbcId, CPID cpId)
{
    newTRACE((LOG_LEVEL_INFO, "DataFile::lastLoadedSBC(%d, %d)", 0, sbcId.id(), cpId));

    if (!WritePrivateProfile(SECT_LOAD, ITEM_LAST_LOADED_SBC, sbcId.id(), 
    	Config::instance().datFile(cpId).c_str())) {
    	TRACE((LOG_LEVEL_ERROR, "Failed to write; throw exception", 0));
		THROW_LINUX_MSG("Failed to write " << ITEM_LAST_LOADED_SBC << " in " <<
			Config::instance().datFile(cpId).c_str());
    }
}
//
// static
//===========================================================================
void DataFile::lastLoadedSBC(SBCId sbcId, const Config::CLUSTER& t) {

	newTRACE((LOG_LEVEL_INFO, "DataFile::lastLoadedSBC(%d, CLUSTER)", 0, sbcId.id()));

	AutoCS a(CS_DataFile());
	if(!WritePrivateProfile(SECT_LOAD, ITEM_LAST_LOADED_SBC, sbcId.id(),
		Config::instance().datFile(t).c_str())) {
    	TRACE((LOG_LEVEL_ERROR, "Failed to write; throw exception", 0));
		THROW_LINUX_MSG("Failed to write " << ITEM_LAST_LOADED_SBC << " in " <<
			Config::instance().datFile(t).c_str());
	}
}
//
// static
//===========================================================================
SBCId DataFile::lastLoadedSBC(CPID cpId)
{
    newTRACE((LOG_LEVEL_INFO, "DataFile::lastLoadedSBC(%d)", 0, cpId));

    SBCId sbcId(static_cast<u_int16>(GetPrivateProfileInt(SECT_LOAD, ITEM_LAST_LOADED_SBC, 0, Config::instance().datFile(cpId).c_str())));

    assert(sbcId.valid());
    TRACE((LOG_LEVEL_INFO, "lastLoadedSBC return: %d", 0, sbcId.id()));

    return sbcId;
}
//
//
// static
//===========================================================================
SBCId DataFile::lastLoadedSBC(const Config::CLUSTER& t) {

	newTRACE((LOG_LEVEL_INFO, "DataFile::lastLoadedSBC(Config::CLUSTER)", 0));

	AutoCS a(CS_DataFile());
	SBCId sbcId(static_cast<u_int16>(GetPrivateProfileInt(SECT_LOAD, ITEM_LAST_LOADED_SBC, 0,
		Config::instance().datFile(t).c_str())));
	assert(sbcId.valid());
	TRACE((LOG_LEVEL_INFO, "lastLoadedSBC return: %d", 0, sbcId.id()));
	return sbcId;
}
//
//
// static
//===========================================================================
void DataFile::lastLoadedTimeForFallback(DateTime dt, CPID cpId)
{
    newTRACE((LOG_LEVEL_INFO, "DataFile::lastLoadedTimeForFallback(%d, %d, %d, %d, %d, %d)", 0,
               dt.year(), dt.month(), dt.day(), dt.hour(),dt.minute(), cpId));

    if (!WritePrivateProfile(SECT_LOAD, LastLoadedTimeForFallBack_Key, dt, 
    	Config::instance().datFile(cpId).c_str())) {
    	TRACE((LOG_LEVEL_ERROR, "Failed to write; throw exception", 0));
		THROW_LINUX_MSG("Failed to write lastLoaded time for fallback in " <<
			Config::instance().datFile(cpId).c_str());
	}
}
//
//
// static
//===========================================================================
void DataFile::lastLoadedTimeForFallback(DateTime dt, const Config::CLUSTER& t)
{
    newTRACE((LOG_LEVEL_INFO, "DataFile::lastLoadedTimeForFallback(%d, %d, %d, %d, %d, CLUSTER)", 0,
               dt.year(), dt.month(), dt.day(), dt.hour(),dt.minute()));

    if (!WritePrivateProfile(SECT_LOAD, LastLoadedTimeForFallBack_Key, dt,
    	Config::instance().datFile(t).c_str())) {
    	TRACE((LOG_LEVEL_ERROR, "Failed to write; throw exception", 0));
		THROW_LINUX_MSG("Failed to write lastLoaded time for fallback in " <<
			Config::instance().datFile(t).c_str());
	}
}
//
// static
//===========================================================================
DateTime DataFile::lastLoadedTimeForFallback(CPID cpId)
{
    newTRACE((LOG_LEVEL_INFO, "DataFile::lastLoadedTimeForFallback(%d)", 0, cpId));

    return GetPrivateProfile(SECT_LOAD, LastLoadedTimeForFallBack_Key, Config::instance().datFile(cpId).c_str());
}
// 
// static
//===========================================================================
DateTime DataFile::lastLoadedTimeForFallback(const Config::CLUSTER& t) {

	newTRACE((LOG_LEVEL_INFO, "DataFile::lastLoadedTimeForFallback(Config::CLUSTER)", 0));

	AutoCS a(CS_DataFile());
	//DateTime dt;
	//TRACE(("lastLoadedTimeForFallback return: %02d%02d%02d %d:%d",0, dt.year(), dt.month(), dt.day(), dt.hour(),dt.minute()));
	//return dt;
    return GetPrivateProfile(SECT_LOAD, LastLoadedTimeForFallBack_Key, Config::instance().datFile(t).c_str());
}
//
//
// static
//===========================================================================
void DataFile::startReloadSeq(bool start, CPID cpId)
{
    newTRACE((LOG_LEVEL_INFO, "DataFile::startReloadSeq(%d, %d)", 0, start, cpId));

    if (!WritePrivateProfile(SECT_LOAD, ITEM_START_OF_RELOAD_SEQUENCE, int(start), 
    	Config::instance().datFile(cpId).c_str())) {
    	TRACE((LOG_LEVEL_ERROR, "Failed to write; throw exception", 0));
		THROW_LINUX_MSG("Failed to write " << ITEM_START_OF_RELOAD_SEQUENCE << " in " <<
			Config::instance().datFile(cpId).c_str());
    }
}
// 
//
// static
//===========================================================================
void DataFile::startReloadSeq(bool start, const Config::CLUSTER& t) {

	newTRACE((LOG_LEVEL_INFO, "DataFile::startReloadSeq(%d, CLUSTER)", 0, start));


	AutoCS a(CS_DataFile());
	if(!WritePrivateProfile(SECT_LOAD, ITEM_START_OF_RELOAD_SEQUENCE, int(start),
		Config::instance().datFile(t).c_str())) {
    	TRACE((LOG_LEVEL_ERROR, "Failed to write; throw exception", 0));
		THROW_LINUX_MSG("Failed to write " << ITEM_START_OF_RELOAD_SEQUENCE << " in " <<
			Config::instance().datFile(t).c_str());
	}
}

//
// static
//===========================================================================
bool DataFile::startReloadSeq(CPID cpId)
{
    newTRACE((LOG_LEVEL_INFO, "DataFile::startReloadSeq(%d)", 0, cpId));

// thng: This is the base of 09B; a very strange piece of code ??? !!!!!!
#if 0
    if (GetPrivateProfileInt(SECT_LOAD, ITEM_START_OF_RELOAD_SEQUENCE, 0, Config::instance().datFile(cpId).c_str()))
    {
        return true;
    }
    else
    {
        return false;
    }
#endif
    
    return GetPrivateProfileInt(SECT_LOAD, ITEM_START_OF_RELOAD_SEQUENCE, 0, 
    		Config::instance().datFile(cpId).c_str()) > 0;
}
//
//
// static
//===========================================================================
bool DataFile::startReloadSeq(const Config::CLUSTER& t) {

	newTRACE((LOG_LEVEL_INFO, "DataFile::startReloadSeq(CLUSTER)", 0));

	/*
	if (GetPrivateProfileInt(SECT_LOAD, ITEM_START_OF_RELOAD_SEQUENCE, 0,
		Config::instance().datFile(t).c_str()))
		return true;
	else
		return false;
	*/
	AutoCS a(CS_DataFile());
	bool res = GetPrivateProfileInt(SECT_LOAD, ITEM_START_OF_RELOAD_SEQUENCE, 0,
		Config::instance().datFile(t).c_str()) > 0;
	
	// BT
	// **************
	//TRACE(("Sleeping", 0));
	//Sleep(10000);
	// **************
	//TRACE(("Wake up", 0));

	return res;
}

//
// static
//===========================================================================
void DataFile::startLoading(CPID cpId)
{
    newTRACE((LOG_LEVEL_INFO, "DataFile::startLoading(%d)", 0, cpId));

    //WritePrivateProfile(SECT_LOAD, ITEM_LOADING, 1, Config::instance().datFile(cpId).c_str());
    
    // Set Ongoing Load = 1
	if(!WritePrivateProfile(SECT_LOAD, ITEM_LOADING, 1, Config::instance().datFile(cpId).c_str())) {
    	TRACE((LOG_LEVEL_ERROR, "Failed to write; throw exception", 0));
		THROW_LINUX_MSG("Failed to write " << ITEM_LOADING << " in " << Config::instance().datFile(cpId).c_str());
	}

    // set current time as start time
    DateTime time;
    time.now();
    startTimeForOngoingLoad(time, cpId);
}
//
// static
//===========================================================================
void DataFile::startLoading(const Config::CLUSTER& t) {

	newTRACE((LOG_LEVEL_INFO, "DataFile::startLoading(CLUSTER)", 0));

	//CS_DataFile().enter();
	// To do: remove this comment at the end of the project
	// Can not remmeber why this hang in 09B. Here we try it again
	AutoCS a(CS_DataFile());
	if(!WritePrivateProfile(SECT_LOAD, ITEM_LOADING, 1, Config::instance().datFile(t).c_str())) {
		//CS_DataFile().leave();
    	TRACE((LOG_LEVEL_ERROR, "Failed to write; throw exception", 0));
		THROW_LINUX_MSG("Failed to write " << ITEM_LOADING << " in " << Config::instance().datFile(t).c_str());
	}
	//CS_DataFile().leave();

	// set current time as start time
	DateTime time;
	time.now();
	startTimeForOngoingLoad(time,t);
}
//
//
// static
//===========================================================================
bool DataFile::loading(const Config::CLUSTER& t) {

	newTRACE((LOG_LEVEL_INFO, "DataFile::loading(CLUSTER)", 0));

	AutoCS a(CS_DataFile());
	return GetPrivateProfileInt(SECT_LOAD, ITEM_LOADING, 0, Config::instance().datFile(t).c_str()) == 1;

}
//
// static
//===========================================================================
void DataFile::endLoading(CPID cpId)
{
    newTRACE((LOG_LEVEL_INFO, "DataFile::endLoading(%d)", 0, cpId));

    // Set Ongoing Load = 0
    if (!WritePrivateProfile(SECT_LOAD, ITEM_LOADING, 0, Config::instance().datFile(cpId).c_str())) {
    	TRACE((LOG_LEVEL_ERROR, "Failed to write; throw exception", 0));
		THROW_LINUX_MSG("Failed to write " << ITEM_LOADING << " in " <<
			Config::instance().datFile(cpId).c_str());
	}
			
    // Reset start time
    DateTime time;
    time.setNull();
    startTimeForOngoingLoad(time, cpId);
}
//
// static
//===========================================================================
void DataFile::endLoading(const Config::CLUSTER& t) {

	newTRACE((LOG_LEVEL_INFO, "DataFile::endLoading(CLUSTER)", 0));

	AutoCS a(CS_DataFile());	
	if(!WritePrivateProfile(SECT_LOAD, ITEM_LOADING, 0,Config::instance().datFile(t).c_str())) {
			THROW_LINUX_MSG("Failed to write " << ITEM_LOADING << " in " <<
			Config::instance().datFile(t).c_str());
	}

	// Reset start time
	DateTime time;
	time.setNull();
	startTimeForOngoingLoad(time, t);
}

//
// static
//===========================================================================
bool DataFile::loading(CPID cpId)
{
    newTRACE((LOG_LEVEL_INFO, "DataFile::loading(%d)", 0, cpId));

// thng for Linux: This will be removed. A strange piece of code, isn't it??
#if 0
    if (GetPrivateProfileInt(SECT_LOAD, ITEM_LOADING, 0, Config::instance().datFile(cpId).c_str()) == 1)
    {
        return true;
    }
    else
    {
        return false;
    }
#endif
	return GetPrivateProfileInt(SECT_LOAD, ITEM_LOADING, 0, Config::instance().datFile(cpId).c_str()) == 1;
}

//
// static
//===========================================================================
void DataFile::startTimeForOngoingLoad(DateTime dt, CPID cpId)
{
    newTRACE((LOG_LEVEL_INFO, "DataFile::startTimeForOngoingLoad(%d, %d, %d, %d, %d, %d)", 0,
              dt.year(), dt.month(), dt.day(), dt.hour(),dt.minute(), cpId));

    if (!WritePrivateProfile(SECT_LOAD, StartTimeForOngoingLoad_Key, dt, Config::instance().datFile(cpId).c_str())) {
    	TRACE((LOG_LEVEL_ERROR, "Failed to write; throw exception", 0));
		THROW_LINUX_MSG("Failed to write start time for ongoing load in " <<
			Config::instance().datFile(cpId).c_str());
	}

}
//
//
// static
//===========================================================================
void DataFile::startTimeForOngoingLoad(DateTime dt, const Config::CLUSTER& t) {

	newTRACE((LOG_LEVEL_INFO, "DataFile::startTimeForOngoingLoad(%d, %d, %d, %d, %d, CLUSTER)", 0, dt.year(), dt.month(), dt.day(), dt.hour(),dt.minute()));

	AutoCS a(CS_DataFile());
    if (!WritePrivateProfile(SECT_LOAD, StartTimeForOngoingLoad_Key, dt, Config::instance().datFile(t).c_str())) {
    	TRACE((LOG_LEVEL_ERROR, "Failed to write; throw exception", 0));
		THROW_LINUX_MSG("Failed to write start time for ongoing load in " <<
			Config::instance().datFile(t).c_str());
    }
}
//
// static
//===========================================================================
DateTime DataFile::startTimeForOngoingLoad(CPID cpId)
{
    newTRACE((LOG_LEVEL_INFO, "DataFile::startTimeForOngoingLoad(%d)", 0, cpId));

    DateTime dt = GetPrivateProfile(SECT_LOAD, StartTimeForOngoingLoad_Key, Config::instance().datFile(cpId).c_str());
    TRACE((LOG_LEVEL_INFO, "startTimeForOngoingLoad return: %02d%02d%02d %d:%d",0, dt.year(), dt.month(), dt.day(), dt.hour(),dt.minute()));
    return dt;
}
//
// static
//===========================================================================
DateTime DataFile::startTimeForOngoingLoad(const Config::CLUSTER& t) {

	newTRACE((LOG_LEVEL_INFO, "DataFile::startTimeForOngoingLoad(CLUSTER)", 0));

	AutoCS a(CS_DataFile());
    DateTime dt = GetPrivateProfile(SECT_LOAD, StartTimeForOngoingLoad_Key, Config::instance().datFile(t).c_str());
	TRACE((LOG_LEVEL_INFO, "startTimeForOngoingLoad return: %02d%02d%02d %d:%d",0, dt.year(), dt.month(), dt.day(), dt.hour(),dt.minute()));
	return dt;
}
//
//
// static
//===========================================================================
SBCId DataFile::onGoingLoadSBCID(const Config::CLUSTER& t) {

	newTRACE((LOG_LEVEL_INFO, "DataFile::onGoingLoadSBCID(CLUSTER)", 0));

	AutoCS a(CS_DataFile());
	SBCId sbcId(static_cast<u_int16>(GetPrivateProfileInt(SECT_LOAD, ITEM_LOADING_ONGOING_SBCID, 128,
		Config::instance().datFile(t).c_str())));
	assert(sbcId.valid());
	TRACE((LOG_LEVEL_INFO, "onGoingLoadSBCID return: %d: ", 0, sbcId.id()));
	return sbcId;
}
//
//
//===========================================================================
void DataFile::startLoadingSBCID(SBCId sbc, const Config::CLUSTER& t) {

	newTRACE((LOG_LEVEL_INFO, "DataFile::startLoadingSBCID(CLUSTER)", 0));
	
	AutoCS a(CS_DataFile());
	if(!WritePrivateProfile(SECT_LOAD, ITEM_LOADING_ONGOING_SBCID, sbc.id(), Config::instance().datFile(t).c_str())) {
		TRACE((LOG_LEVEL_ERROR, "Failed to write; throw exception", 0));
		THROW_LINUX_MSG("Failed to write " << ITEM_LOADING_ONGOING_SBCID << " in " << Config::instance().datFile(t).c_str());
	}
	// set current time as start time
	DateTime time;
	time.now();
	startTimeForOngoingLoad(time,t);
}
//
//
//===========================================================================
void DataFile::endLoadingSBCID(const Config::CLUSTER& t) {

	newTRACE((LOG_LEVEL_INFO, "DataFile::endLoadingSBCID(CLUSTER)", 0));
	AutoCS a(CS_DataFile());
	if(!WritePrivateProfile(SECT_LOAD, ITEM_LOADING_ONGOING_SBCID, 128, Config::instance().datFile(t).c_str())) {
		TRACE((LOG_LEVEL_ERROR, "Failed to write; throw exception", 0));
		THROW_LINUX_MSG("Failed to write " << ITEM_LOADING_ONGOING_SBCID << " in " << Config::instance().datFile(t).c_str());
	}
	DateTime time;
	time.setNull();
	startTimeForOngoingLoad(time,t);
}
//
// static
//===========================================================================
void DataFile::lastLoadedSbcInfo(SBCId sbcId, const Config::CLUSTER& t) {

	newTRACE((LOG_LEVEL_INFO, "DataFile::lastLoadedSbcInfo(%d, CLUSTER)", 0, sbcId.id()));

	AutoCS a(CS_DataFile());
	if(!WritePrivateProfile(SECT_LOAD_INFORMATION, ITEM_LAST_LOADED_SBC, sbcId.id(),
		Config::instance().datFile(t).c_str())) {
		THROW_LINUX_MSG("Failed to write " << ITEM_LAST_LOADED_SBC << " in " <<
			Config::instance().datFile(t).c_str());
	}
}
//
//
// static
//===========================================================================
SBCId DataFile::lastLoadedSbcInfo(const Config::CLUSTER& t) {

	newTRACE((LOG_LEVEL_INFO, "DataFile::lastLoadedSbcInfo(Config::CLUSTER)", 0));

	AutoCS a(CS_DataFile());
	SBCId sbcId(static_cast<u_int16>(GetPrivateProfileInt(SECT_LOAD_INFORMATION, ITEM_LAST_LOADED_SBC, 0,
		Config::instance().datFile(t).c_str())));
	assert(sbcId.valid());
	TRACE((LOG_LEVEL_INFO, "lastLoadedSbcInfo return: %d", 0, sbcId.id()));
	return sbcId;
}
//
// static
// Set the dump flag and set the the start date and time
//===========================================================================
void DataFile::startDumping(CPID cpId)
{
    newTRACE((LOG_LEVEL_INFO, "DataFile::startDumping(%d)", 0, cpId));

    // Set Ongoing dump = 1
    if (!WritePrivateProfile(SECT_DUMP, ITEM_DUMPING, 1, Config::instance().datFile(cpId).c_str())) {
    	TRACE((LOG_LEVEL_ERROR, "Failed to write; throw exception", 0));
		THROW_LINUX_MSG("Failed to write " << ITEM_DUMPING << " in " << Config::instance().datFile(cpId).c_str());
	}

    // set current time as start time
    DateTime time;
    time.now();
    startTimeForOngoingDump(time, cpId);
}
//
// static
// Set the dump flag and set the the start date and time
//===========================================================================
void DataFile::startDumping(const Config::CLUSTER& t) {

	newTRACE((LOG_LEVEL_INFO, "DataFile::startDumping(CLUSTER)", 0));

	AutoCS a(CS_DataFile());
	if(!WritePrivateProfile(SECT_DUMP, ITEM_DUMPING, 1, Config::instance().datFile(t).c_str())) {
		THROW_LINUX_MSG("Failed to write " << ITEM_DUMPING << " in " << Config::instance().datFile(t).c_str());
	}
	
	// set current time as start time
	DateTime time;
	time.now();
	startTimeForOngoingDump(time,t);
}
//
//
//===========================================================================
void DataFile::startDumpingSBCID(SBCId sbc, const Config::CLUSTER& t) {

	newTRACE((LOG_LEVEL_INFO, "DataFile::startSBCIDDumping(CLUSTER)", 0));
	AutoCS a(CS_DataFile());
	if(!WritePrivateProfile(SECT_DUMP, ITEM_DUMPING_ONGOING_SBCID, sbc.id(), Config::instance().datFile(t).c_str())) {
		TRACE((LOG_LEVEL_ERROR, "Error DataFile::startSBCIDDumping(CLUSTER) %s", 0, Config::instance().datFile(t).c_str()));
		THROW_LINUX_MSG("Failed to write " << ITEM_DUMPING_ONGOING_SBCID << " in " << Config::instance().datFile(t).c_str());
	}
}
//
// static
//===========================================================================
void DataFile::endDumping(CPID cpId)
{
    newTRACE((LOG_LEVEL_INFO, "DataFile::endDumping(%d)", 0, cpId));

    // Set Ongoing dump = 0
    if (!WritePrivateProfile(SECT_DUMP, ITEM_DUMPING, 0, Config::instance().datFile(cpId).c_str())) {
    	TRACE((LOG_LEVEL_ERROR, "Failed to write; throw exception", 0));
		THROW_LINUX_MSG("Failed to write " << ITEM_DUMPING << " in " << Config::instance().datFile(cpId).c_str());
    }

    // Reset start time
    DateTime time;
    time.setNull();
    startTimeForOngoingDump(time, cpId);
}
//
//
// static
//===========================================================================
void DataFile::endDumping(const Config::CLUSTER& t) {

	newTRACE((LOG_LEVEL_INFO, "DataFile::endDumping(CLUSTER)", 0));

	// Write "0" for no dump 
	char buf[25] = "0\0";
	
	//CS_DataFile().enter();
	// To do: remove this comment. Can remember why it hang
	AutoCS a(CS_DataFile());
	if(!WritePrivateProfileString(SECT_DUMP, ITEM_DUMPING, buf,
		Config::instance().datFile(t).c_str())) {
			//CS_DataFile().leave();
    		TRACE((LOG_LEVEL_ERROR, "Failed to write; throw exception", 0));
			THROW_LINUX_MSG("Failed to write " << ITEM_DUMPING << " in " <<
				Config::instance().datFile(t).c_str());
	}
	//CS_DataFile().leave();

	// Reset start time
	DateTime time;
	time.setNull();
	startTimeForOngoingDump(time,t);
}
//
//
// static
//===========================================================================
bool DataFile::dumping(CPID cpId)
{
    newTRACE((LOG_LEVEL_INFO, "DataFile::dumping(%d)", 0, cpId));
#if 0
    if (GetPrivateProfileInt(SECT_DUMP, ITEM_DUMPING, 0, Config::instance().datFile(cpId).c_str()) == 1)
    {
        return true;
    }
    else
    {
        return false;
    }
#endif
	return GetPrivateProfileInt(SECT_DUMP, ITEM_DUMPING, 0, Config::instance().datFile(cpId).c_str()) == 1;
}
//
// static
//===========================================================================
bool DataFile::dumping(const Config::CLUSTER& t) {

	newTRACE((LOG_LEVEL_INFO, "DataFile::dumping(CLUSTER)", 0));

	//if (GetPrivateProfileInt(SECT_DUMP, ITEM_DUMPING, 0, Config::instance().datFile(t).c_str()) == 1)
	//	return true;
	//else
	//	return false;

	AutoCS a(CS_DataFile());
	//bool res = GetPrivateProfileInt(SECT_DUMP, ITEM_DUMPING, 0, 
	//	Config::instance().datFile(t).c_str()) == 1;
	//return res;
	return GetPrivateProfileInt(SECT_DUMP, ITEM_DUMPING, 0, 
		Config::instance().datFile(t).c_str()) == 1;
}
//
//
// static
//===========================================================================
void DataFile::endDumpingSBCID(const Config::CLUSTER& t) {

	newTRACE((LOG_LEVEL_INFO, "DataFile::endDumpingSBCID(CLUSTER)", 0));

	AutoCS a(CS_DataFile());
	if(!WritePrivateProfile(SECT_DUMP, ITEM_DUMPING_ONGOING_SBCID, 0,
		Config::instance().datFile(t).c_str())) {
    	TRACE((LOG_LEVEL_ERROR, "Failed to write; throw exception", 0));
		THROW_LINUX_MSG("Failed to write " << ITEM_DUMPING_ONGOING_SBCID << " in " <<
			Config::instance().datFile(t).c_str());
	}
}
//
//
// static
//===========================================================================
void DataFile::startTimeForOngoingDump(DateTime dt, CPID cpId)
{
    newTRACE((LOG_LEVEL_INFO, "DataFile::startTimeForOngoingDump(%d, %d, %d, %d, %d, %d)", 0, dt.year(), dt.month(), dt.day(), dt.hour(),dt.minute(), cpId));

    if (!WritePrivateProfile(SECT_DUMP, StartTimeForOngoingDump_Key, dt, Config::instance().datFile(cpId).c_str())) {
    	TRACE((LOG_LEVEL_ERROR, "Failed to write; throw exception", 0));
		THROW_LINUX_MSG("Failed to write start time for ongoing dump in " <<
			Config::instance().datFile(cpId).c_str());
	}
}
//
//
// static
//===========================================================================
void DataFile::startTimeForOngoingDump(DateTime dt, const Config::CLUSTER& t) {

	newTRACE((LOG_LEVEL_INFO, "DataFile::startTimeForOngoingDump(%d, %d, %d, %d, %d, CLUSTER)", 0, dt.year(), dt.month(), dt.day(), dt.hour(),dt.minute()));

	AutoCS a(CS_DataFile());
	
    if (!WritePrivateProfile(SECT_DUMP, StartTimeForOngoingDump_Key, dt, Config::instance().datFile(t).c_str())) {
    	TRACE((LOG_LEVEL_ERROR, "Failed to write; throw exception", 0));
		THROW_LINUX_MSG("Failed to write start time for ongoing dump in " <<
			Config::instance().datFile(t).c_str());
	}
}
//
//
// static
//===========================================================================
DateTime DataFile::startTimeForOngoingDump(CPID cpId)
{
    newTRACE((LOG_LEVEL_INFO, "DataFile::startTimeForOngoingDump(%d)", 0, cpId));
	
    return GetPrivateProfile(SECT_DUMP, StartTimeForOngoingDump_Key, Config::instance().datFile(cpId).c_str());;
}
//
// static
//===========================================================================
DateTime DataFile::startTimeForOngoingDump(const Config::CLUSTER& t) {

	newTRACE((LOG_LEVEL_INFO, "DataFile::startTimeForOngoingDump(CLUSTER)", 0));

	DateTime dt;
	AutoCS a(CS_DataFile());
	
    return GetPrivateProfile(SECT_DUMP, StartTimeForOngoingDump_Key, Config::instance().datFile(t).c_str());;
}
//
// static
//===========================================================================
u_int32 DataFile::currCmdLog(CPID cpId) {

	newTRACE((LOG_LEVEL_INFO, "DataFile::currCmdLog(%d)", 0, cpId));
 
	return GetPrivateProfileInt(SECT_CMDLOG, ITEM_CURRENT, 0,
		Config::instance().datFile(cpId).c_str());
}
//
// static
//===========================================================================
void DataFile::currCmdLog(u_int32 current, CPID cpId) {

	newTRACE((LOG_LEVEL_INFO, "DataFile::currCmdLog(%d, %d)", 0, current, cpId));

	//ostringstream ostr;
	//ostr << current;
	using cps_utils::to_string;
	if(!WritePrivateProfileString(SECT_CMDLOG, ITEM_CURRENT, to_string(current).c_str(),
		Config::instance().datFile(cpId).c_str())) {
    	TRACE((LOG_LEVEL_ERROR, "Failed to write; throw exception", 0));
		THROW_LINUX_MSG("Failed to write " << ITEM_CURRENT << " in " <<
			Config::instance().datFile(cpId).c_str());
	}
}
//
// static
//===========================================================================
SBCId DataFile::onGoingDumpSBCID(const Config::CLUSTER& t) {

	newTRACE((LOG_LEVEL_INFO, "DataFile::onGoingDumpSBCID(CLUSTER)", 0));

	AutoCS a(CS_DataFile());
	SBCId sbcId(static_cast<u_int16>(GetPrivateProfileInt(SECT_DUMP, ITEM_DUMPING_ONGOING_SBCID, 128,
		Config::instance().datFile(t).c_str())));
	assert(sbcId.valid());
	TRACE((LOG_LEVEL_INFO, "onGoingDumpSBCID return: %d", 0, sbcId.id()));
	return sbcId;
}
//
//
// NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
// For protocol interaction
//
//
//============================================================================
void DataFile::onGoingDumpLoad(BUACTREC& load, BUACTREC& dump)
{
	newTRACE((LOG_LEVEL_INFO, "DataFile::onGoingDumpLoad()", 0));

	load.reserve(Config::instance().maxNoOfSimLoad());
	dump.reserve(Config::instance().maxNoOfSimDump());

	ACS_CS_API_IdList cpList;
	ACS_CS_API_CP * CP = ACS_CS_API::createCPInstance();

	ACS_CS_API_NS::CS_API_Result res = CP->getCPList(cpList);

	AutoCS a(CS_DataFile());
	if (res != ACS_CS_API_NS::Result_Success) {
		ACS_CS_API::deleteCPInstance(CP);
		throw BUPMsg::INTERNAL_ERROR;
	}

	// Rebuild loading/dumping in core information
	BitVector &loadVec = loadingCPs();
	BitVector &dumpVec = dumpingCPs();
	loadVec.clearAllBits();
	dumpVec.clearAllBits();

	BUPAIR rec;
	u_int16 id;
	for (u_int32 i = 0; i < cpList.size(); i++) {

		CPID cpId = cpList[i];
		if (cpId > 63)
			continue;

		const Config::CLUSTER t(cpId, true);

		id = GetPrivateProfileInt(SECT_DUMP, ITEM_DUMPING_ONGOING_SBCID, 128,
				Config::instance().datFile(t).c_str());
		if (id < 128) {
			//TRACE(("CP %d dumping on SBCID: %d", 0, cpId, id));
			rec.second = cpId;
			rec.first = id;
			dump.push_back(rec);
			dumpVec.setBit(cpId);
		}
		id = GetPrivateProfileInt(SECT_LOAD, ITEM_LOADING_ONGOING_SBCID, 128,
				Config::instance().datFile(t).c_str());
		if (id < 128) {
			//TRACE(("CP %d loadning SBCID: %d", 0, cpId, id));
			rec.second = cpId;
			rec.first = id;
			load.push_back(rec);
			loadVec.setBit(cpId);
		}
	}	
	ACS_CS_API::deleteCPInstance(CP);

	//const Config::CLUSTER t;
	//ostringstream dOut;
	//dOut << dumpVec;
	//WritePrivateProfileString(SECT_DUMP, ITEM_DUMPING_CPS, dOut.str().c_str(), 
	//	Config::instance().datFile(t).c_str());

	//ostringstream lOut;
	//lOut << loadVec;
	//WritePrivateProfileString(SECT_LOAD, ITEM_LOADING_CPS, lOut.str().c_str(), 
	//	Config::instance().datFile(t).c_str());
}
//
//============================================================================
void DataFile::onGoingLoad(BUACTREC& load)
{
	newTRACE((LOG_LEVEL_INFO, "DataFile::onGoingLoad()", 0));

	ACS_CS_API_IdList cpList;
	ACS_CS_API_CP * CP = ACS_CS_API::createCPInstance();

	ACS_CS_API_NS::CS_API_Result res = CP->getCPList(cpList);

	AutoCS a(CS_DataFile());
	if (res != ACS_CS_API_NS::Result_Success) {
		ACS_CS_API::deleteCPInstance(CP);
		throw BUPMsg::INTERNAL_ERROR;
	}

	load.reserve(Config::instance().maxNoOfSimLoad());
	BitVector &loadVec = loadingCPs();
	loadVec.clearAllBits();

	BUPAIR rec;
	u_int16 id;
	for (u_int32 i = 0; i < cpList.size(); i++) {

		CPID cpId = cpList[i];
		if (cpId > 63)
			continue;

		Config::CLUSTER t(cpId, true);

		id = GetPrivateProfileInt(SECT_LOAD, ITEM_LOADING_ONGOING_SBCID, 128,
				Config::instance().datFile(t).c_str());
		if (id < 128) {
			//TRACE(("CP %d loadning SBCID: %d", 0, cpId, id));
			rec.second = cpId;
			rec.first = id;
			load.push_back(rec);
			loadVec.setBit(cpId);
		}
	}	
	ACS_CS_API::deleteCPInstance(CP);

	//const Config::CLUSTER t;
	//ostringstream lOut;
	//lOut << loadVec;
	//WritePrivateProfileString(SECT_LOAD, ITEM_LOADING_CPS, lOut.str().c_str(), 
	//	Config::instance().datFile(t).c_str());
}
//
//
//============================================================================
void DataFile::onGoingDump(BUACTREC& dump)
{
	newTRACE((LOG_LEVEL_INFO, "DataFile::onGoingDump()", 0));

	ACS_CS_API_IdList cpList;
	ACS_CS_API_CP * CP = ACS_CS_API::createCPInstance();

	ACS_CS_API_NS::CS_API_Result res = CP->getCPList(cpList);

	AutoCS a(CS_DataFile());
	if (res != ACS_CS_API_NS::Result_Success) {
		ACS_CS_API::deleteCPInstance(CP);
		throw BUPMsg::INTERNAL_ERROR;
	}

	dump.reserve(Config::instance().maxNoOfSimDump());
	BitVector &dumpVec = dumpingCPs();
	dumpVec.clearAllBits();

	BUPAIR rec;
	u_int16 id;
	for (u_int32 i = 0; i < cpList.size(); i++) {

		CPID cpId = cpList[i];
		if (cpId > 63)
			continue;

		Config::CLUSTER t(cpId, true);

		id = GetPrivateProfileInt(SECT_DUMP, ITEM_DUMPING_ONGOING_SBCID, 128,
				Config::instance().datFile(t).c_str());
		if (id < 128) {
			//TRACE(("CP %d dumping on SBCID: %d", 0, cpId, id));
			rec.first = id;
			rec.second = cpId;
			dump.push_back(rec);
			dumpVec.setBit(cpId);
		}
	}	
	ACS_CS_API::deleteCPInstance(CP);

	//const Config::CLUSTER t;
	//ostringstream dOut;
	//dOut << dumpVec;
	//WritePrivateProfileString(SECT_DUMP, ITEM_DUMPING_CPS, dOut.str().c_str(), 
	//	Config::instance().datFile(t).c_str());
}

void DataFile::clearLoading(CPID cpId, const Config::CLUSTER&t)
{
	AutoCS a(CS_DataFile());
	BitVector &v = loadingCPs();
	v.clearBit(cpId);
	ostringstream vOut;
	vOut << v;
	WritePrivateProfileString(SECT_LOAD, ITEM_LOADING_CPS, vOut.str().c_str(),
		Config::instance().datFile(t).c_str());
}

void DataFile::setDumping(CPID cpId, const Config::CLUSTER& t)
{
	AutoCS a(CS_DataFile());
	BitVector &v = dumpingCPs();
	v.setBit(cpId);
	ostringstream vOut;
	vOut << v;
	WritePrivateProfileString(SECT_DUMP, ITEM_DUMPING_CPS, vOut.str().c_str(),
		Config::instance().datFile(t).c_str());
}

void DataFile::clearDumping(CPID cpId, const Config::CLUSTER& t)
{
	AutoCS a(CS_DataFile());
	BitVector &v = dumpingCPs();
	v.clearBit(cpId);
	ostringstream vOut;
	vOut << v;
	WritePrivateProfileString(SECT_DUMP, ITEM_DUMPING_CPS, vOut.str().c_str(),
		Config::instance().datFile(t).c_str());
}
//
// static
//===========================================================================
SBCId DataFile::dumpEndQueued(const Config::CLUSTER& t) {

	newTRACE((LOG_LEVEL_INFO, "DataFile::dumpEndQueued(CLUSTER)", 0));

	AutoCS a(CS_DataFile());
	SBCId sbcId(static_cast<u_int16>(GetPrivateProfileInt(SECT_DUMP, ITEM_DUMPEND_QUEUED_SBCID, 128,
		Config::instance().datFile(t).c_str())));
	assert(sbcId.valid());
	TRACE((LOG_LEVEL_INFO, "dumpEndQueued return: %d", 0, sbcId.id()));
	return sbcId;
}
//
// static
//===========================================================================
void DataFile::dumpEndQueued(u_int16 sbcid, const Config::CLUSTER& t) {

	newTRACE((LOG_LEVEL_INFO, "DataFile::dumpEndQueued(CLUSTER)", 0));

	//char buf[25];
	//itoa(sbcid, buf, 10);
	//ostringstream buf;
	//buf << sbcid;
	using cps_utils::to_string;
	AutoCS a(CS_DataFile());
	if(!WritePrivateProfileString(SECT_DUMP, ITEM_DUMPEND_QUEUED_SBCID, to_string(sbcid).c_str(),
		Config::instance().datFile(t).c_str())) {
		THROW_LINUX_MSG("Failed to write " << ITEM_DUMPEND_QUEUED_SBCID << " in " <<
			Config::instance().datFile(t).c_str());
	}
}

//
//
//============================================================================
void DataFile::dumpEndQueued(BUACTREC& dumpEndQueue)
{
	newTRACE((LOG_LEVEL_INFO, "DataFile::dumpEndQueued(BUACTREC&)", 0));

	dumpEndQueue.reserve(4);

	ACS_CS_API_IdList cpList;
	ACS_CS_API_CP * CP = ACS_CS_API::createCPInstance();

	ACS_CS_API_NS::CS_API_Result res = CP->getCPList(cpList);

	AutoCS a(CS_DataFile());
	if (res != ACS_CS_API_NS::Result_Success) {
		ACS_CS_API::deleteCPInstance(CP);
		throw BUPMsg::INTERNAL_ERROR;
	}

	BUPAIR rec;
	u_int16 id;
	for (u_int32 i = 0; i < cpList.size(); i++) {

		CPID cpId = cpList[i];
		if (cpId > 63)
			continue;

		Config::CLUSTER t(cpId, true);

		id = GetPrivateProfileInt(SECT_DUMP, ITEM_DUMPEND_QUEUED_SBCID, 128,
				Config::instance().datFile(t).c_str());
		if (id < 128) {
			TRACE((LOG_LEVEL_INFO, "CP %d dump end queued for SBCID: %d", 0, cpId, id));
			rec.first = id;
			rec.second = cpId;
			dumpEndQueue.push_back(rec);
		}
	}
	ACS_CS_API::deleteCPInstance(CP);
}

//
//
//============================================================================
void DataFile::dumpEndQueued(u_int16 sbcId)
{
	newTRACE((LOG_LEVEL_INFO, "DataFile::dumpEndQueued(sbcId)", 0));

	ACS_CS_API_IdList cpList;
	ACS_CS_API_CP * CP = ACS_CS_API::createCPInstance();

	ACS_CS_API_NS::CS_API_Result res = CP->getCPList(cpList);

	//char buf128[25];
	//itoa(128, buf128, 10);
	//char buf0[25] = "0\0";
	//ostringstream buf128;
	//buf128 << 128;
	
	AutoCS a(CS_DataFile());
	if (res != ACS_CS_API_NS::Result_Success) {
		ACS_CS_API::deleteCPInstance(CP);
		throw BUPMsg::INTERNAL_ERROR;
	}

	BUPAIR rec;
	u_int16 id;
	for (u_int32 i = 0; i < cpList.size(); i++) {

		CPID cpId = cpList[i];
		if (cpId > 63)
			continue;

		const Config::CLUSTER tn(cpId, true);
		id = GetPrivateProfileInt(SECT_DUMP, ITEM_DUMPING_ONGOING_SBCID, 128,
				Config::instance().datFile(tn).c_str());
		if (id == sbcId) {
			// Clear DumpEnd Queue mark on specific sbcId
			if(!WritePrivateProfileString(SECT_DUMP, ITEM_DUMPEND_QUEUED_SBCID, "128",
				Config::instance().datFile(tn).c_str())) {
				THROW_LINUX_MSG("Failed to write " << ITEM_DUMPEND_QUEUED_SBCID << " in " <<
					Config::instance().datFile(tn).c_str());
			}
			// Clear DumpBegin mark
			if(!WritePrivateProfileString(SECT_DUMP, ITEM_DUMPING, "0",
				Config::instance().datFile(tn).c_str())) {
				THROW_LINUX_MSG("Failed to write " << ITEM_DUMPING << " in " <<
					Config::instance().datFile(tn).c_str());
			}
			// Clear DumpBegin mark on specific sbcId
			if(!WritePrivateProfileString(SECT_DUMP, ITEM_DUMPING_ONGOING_SBCID, "128",
				Config::instance().datFile(tn).c_str())) {
				THROW_LINUX_MSG("Failed to write " << ITEM_DUMPING_ONGOING_SBCID << " in " <<
					Config::instance().datFile(tn).c_str());
			}
		}
	}
}
//
//
// static
//===========================================================================
void DataFile::endSbcSwitch(const Config::CLUSTER& t) {

	newTRACE((LOG_LEVEL_INFO, "DataFile::endSbcSwitch(CLUSTER)", 0));

	// Write "0" for no dump
	char buf[25] = "0\0";
	AutoCS a(CS_DataFile());
	for (int i = 0; i < 3; i++) {
		if(WritePrivateProfileString(SECT_SWITCH, ITEM_SWITCH, buf,
			Config::instance().datFile(t).c_str()))
			break;
	}
}
//
//
// static
//===========================================================================
void DataFile::startSbcSwitch(const Config::CLUSTER& t) {

	newTRACE((LOG_LEVEL_INFO, "DataFile::startSbcSwitch(CLUSTER)", 0));

	// Write "0" for no dump
	char buf[25] = "1\0";
	AutoCS a(CS_DataFile());
	WritePrivateProfileString(SECT_SWITCH, ITEM_SWITCH, buf,
		Config::instance().datFile(t).c_str());
}
//
// static
//===========================================================================
bool DataFile::onGoingSbcSwitch(const Config::CLUSTER& t) {

	newTRACE((LOG_LEVEL_INFO, "DataFile::onGoingSbcSwitch(CLUSTER)", 0));

	AutoCS a(CS_DataFile());
	//bool res = GetPrivateProfileInt(SECT_SWITCH, ITEM_SWITCH, 0,
	//	Config::instance().datFile(t).c_str()) > 0;
	//return res;

	return GetPrivateProfileInt(SECT_SWITCH, ITEM_SWITCH, 0,
		Config::instance().datFile(t).c_str()) > 0;
}

