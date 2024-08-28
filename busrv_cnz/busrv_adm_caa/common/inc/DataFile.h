#ifndef _DATAFILE_H_
#define _DATAFILE_H_
/*
NAME
	DATAFILE -
LIBRARY 3C++
PAGENAME DATAFILE
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE DataFile.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION
	Class handling busrv.dat, which includes reload & cmdlog data.

ERROR HANDLING
	-

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
	2002-05-13 by U/Y/SF Anders Gillgren (qabgill, tag: ag)

LINKAGE
	-

SEE ALSO
	-

Revision history
----------------
2002-05-13 qabgill Created
2003-01-08 uablan  Change from expire to last loaded.
2003-01-21 uablan  Added LoadingInProgess Item and functions
2006-03-01 uablan  Remove LoadingInProgress flag. (HG62821)
2009-09-01 xdgthng SSI project
*/
#pragma once


#include "Config.h"
#include "buap.h"
#include "DateTime.h"
#include "SBCId.h"

#if 0
#include "ACS_CS_API.h"
#endif


namespace ParseINIFunx {
    struct PARSE_TIME_KEY;
};

// Forward Declaration
class CriticalSection;

#include <vector>

using namespace buap;


struct DataFile {

	// This is a convinient wrap around of a 64 bit integer used as bit mask for marking
	// loading or dumping activity
	// This is not intended as a bit vector concrete class offering full functionality
	class BitVector
	{
	public:

		union BVData {
			u_int64	d64;
			u_int32 d32[2];
		};

		friend ostream& operator<<(ostream&, const BitVector&);
		void set(const char*);

		BitVector() : m_dumpEndQueue(0) {m_data.d64 = 0ULL; }

		// Turn the bit on for a given CPID 
		void setBit(CPID pos) {			
			m_data.d32[pos/s_wordSize] |= 1 << pos % s_wordSize;
		}

		// Turn the bit off for a given CPID 
		void clearBit(CPID pos){
			m_data.d32[pos/s_wordSize] &= ~(1 << pos % s_wordSize);
		}

		void clearAllBits() { 
			m_data.d64 = 0ULL; 
			//m_dumpEndQueue = 0;
		}

		bool operator==(u_int64 n) { return m_data.d64 == n; }
		bool operator!=(u_int64 n) { return !operator==(n); }

		// This is the piggy back functionality. BUPJob is not serialised
		void dumpEndQueue(bool increment) { 
			if (increment) ++m_dumpEndQueue;
			else if (m_dumpEndQueue) --m_dumpEndQueue; 
		}

		u_int16 dumpEndQueue() { return m_dumpEndQueue; }

	private:

		BVData				m_data;
		u_int16				m_dumpEndQueue;
		static const int	s_wordSize;
	};

	typedef pair<u_int16, CPID> BUPAIR;

	typedef vector<BUPAIR>	BUACTREC;
	typedef vector<BUPAIR>::iterator BUACTREC_ITERATOR;

	static CriticalSection& CS_DataFile();

	static const char* SECT_LOAD;
	static const char* SECT_DUMP;
	static const char* SECT_CMDLOG;
	static const char* SECT_SWITCH;
	static const char* SECT_SRM_RELOAD;
	static const char* SECT_LOAD_INFORMATION;

	static const char* ITEM_SRM_RELOAD_SBC;
	static const char* ITEM_SRM_RELOAD_SBC_YEAR;
	static const char* ITEM_SRM_RELOAD_SBC_MONTH;
	static const char* ITEM_SRM_RELOAD_SBC_DAY;
	static const char* ITEM_SRM_RELOAD_SBC_HOUR;
	static const char* ITEM_SRM_RELOAD_SBC_MIN;
	static const char* ITEM_SRM_RELOAD_SBC_SEC;
	static const char* ITEM_SRM_RELOAD_SBC_DAYLIGHT;

	static const char* ITEM_SWITCH;

	static const char* ITEM_LAST_LOADED_SBC;
	static const char* ITEM_LAST_LOADED_TIME_FOR_FALLBACK_YEAR;
	static const char* ITEM_LAST_LOADED_TIME_FOR_FALLBACK_MONTH;
	static const char* ITEM_LAST_LOADED_TIME_FOR_FALLBACK_DAY;
	static const char* ITEM_LAST_LOADED_TIME_FOR_FALLBACK_HOUR;
	static const char* ITEM_LAST_LOADED_TIME_FOR_FALLBACK_MIN;
	static const char* ITEM_LAST_LOADED_TIME_FOR_FALLBACK_SEC;
	static const char* ITEM_LAST_LOADED_TIME_FOR_FALLBACK_DAYLIGHT;
	static const char* ITEM_START_OF_RELOAD_SEQUENCE;

	static const char* ITEM_LOADING;
	static const char* ITEM_LOADING_CPS;
	static const char* ITEM_DEFAULT_CPS;

	static const char* ITEM_LOADING_START_TIME_YEAR;
	static const char* ITEM_LOADING_START_TIME_MONTH;
	static const char* ITEM_LOADING_START_TIME_DAY;
	static const char* ITEM_LOADING_START_TIME_HOUR;
	static const char* ITEM_LOADING_START_TIME_MIN;
	static const char* ITEM_LOADING_START_TIME_SEC;
	static const char* ITEM_LOADING_START_TIME_DAYLIGHT;
	static const char* ITEM_LOADING_ONGOING_SBCID;

	static const char* ITEM_DUMPING;
	static const char* ITEM_DUMPING_CPS;

	static const char* ITEM_DUMPING_START_TIME_YEAR;
	static const char* ITEM_DUMPING_START_TIME_MONTH;
	static const char* ITEM_DUMPING_START_TIME_DAY;
	static const char* ITEM_DUMPING_START_TIME_HOUR;
	static const char* ITEM_DUMPING_START_TIME_MIN;
	static const char* ITEM_DUMPING_START_TIME_SEC;
	static const char* ITEM_DUMPING_START_TIME_DAYLIGHT;
	static const char* ITEM_DUMPING_ONGOING_SBCID;
	static const char* ITEM_DUMPEND_QUEUED_SBCID;
	static const char* ITEM_CURRENT;

	static const ParseINIFunx::PARSE_TIME_KEY LastLoadedTimeForFallBack_Key;
	static const ParseINIFunx::PARSE_TIME_KEY StartTimeForOngoingLoad_Key;
	static const ParseINIFunx::PARSE_TIME_KEY StartTimeForOngoingDump_Key;
	static const ParseINIFunx::PARSE_TIME_KEY SrmReloadSBC_Key;


    // Until Porting FallbackBegin primitive
	// SRM Reload
	static void srmReloadSBC(SBCId,const Config::CLUSTER&); 
	static void srmReloadSBCTime(DateTime ,const Config::CLUSTER&);
	static DateTime srmReloadSBCTime(const Config::CLUSTER&);

	static SBCId srmReloadSBC(const Config::CLUSTER&);

	// load
	static void lastLoadedSBC(SBCId, CPID cpId=0xFFFF);
	static void lastLoadedSBC(SBCId, const Config::CLUSTER&);

	static SBCId lastLoadedSBC(CPID cpId=0xFFFF);
	static SBCId lastLoadedSBC(const Config::CLUSTER&);

	static void lastLoadedTimeForFallback(DateTime ,CPID cpId=0xFFFF);
	static void lastLoadedTimeForFallback(DateTime ,const Config::CLUSTER&);

	static DateTime lastLoadedTimeForFallback(CPID cpId=0xFFFF);
	static DateTime lastLoadedTimeForFallback(const Config::CLUSTER&);

	static bool startReloadSeq(CPID cpId=0xFFFF);
	static bool startReloadSeq(const Config::CLUSTER&);

	static void startReloadSeq(bool start, CPID cpId=0xFFFF);
	static void startReloadSeq(bool start, const Config::CLUSTER&);

	static void startLoading(CPID cpId=0xFFFF);
	static void startLoading(const Config::CLUSTER&);

	static void endLoading(CPID cpId=0xFFFF);
	static void endLoading(const Config::CLUSTER&);

	static bool loading(CPID cpId=0xFFFF);
	static bool loading(const Config::CLUSTER&);

	static DateTime startTimeForOngoingLoad(CPID cpId=0xFFFF);
	static DateTime startTimeForOngoingLoad(const Config::CLUSTER&);

	static void startTimeForOngoingLoad(DateTime dt, CPID cpId=0xFFFF);
	static void startTimeForOngoingLoad(DateTime dt, const Config::CLUSTER&);

	static void startLoadingSBCID(SBCId, const Config::CLUSTER&);
	static void endLoadingSBCID(const Config::CLUSTER&);
	static SBCId onGoingLoadSBCID(const Config::CLUSTER&);

	// Loading Information
	static void lastLoadedSbcInfo(SBCId, const Config::CLUSTER&);
	static SBCId lastLoadedSbcInfo(const Config::CLUSTER&);

	// dump
	static void startDumping(CPID cpId=0xFFFF);
	static void startDumping(const Config::CLUSTER&);

	static void startDumpingSBCID(SBCId, const Config::CLUSTER&);

	static void endDumping(CPID cpId=0xFFFF);
	static void endDumping(const Config::CLUSTER&);

	static void endDumpingSBCID(const Config::CLUSTER&);

	static bool dumping(CPID cpId=0xFFFF);
	static bool dumping(const Config::CLUSTER&);

	static DateTime startTimeForOngoingDump(CPID cpId=0xFFFF);
	static void startTimeForOngoingDump(DateTime dt, CPID cpId=0xFFFF);

    // cmdlog
	static void currCmdLog(u_int32 , CPID cpId=0xFFFF);
	//static void currCmdLog(u_int32 , const Config::CLUSTER&);

	static u_int32 currCmdLog(CPID cpId=0xFFFF);
	//static u_int32 currCmdLog(const Config::CLUSTER&);

	static DateTime startTimeForOngoingDump(const Config::CLUSTER&);
	static void startTimeForOngoingDump(DateTime dt, const Config::CLUSTER&);

	// TTTTTTTTTTTTTTTTTTTTT
	static SBCId onGoingDumpSBCID(const Config::CLUSTER&);

    // BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
    // CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
    //
    // Until BC port
    //
	// NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
	// For protocol interaction
	//

	// The following group of functions scans the disk for loading and dumping
	// and other activity. These are very expensive operation in term of time
	// 
	static void onGoingDumpLoad(BUACTREC& load, BUACTREC& dump);
	static void onGoingDump(BUACTREC& dump);
	static void onGoingLoad(BUACTREC& load);


	static SBCId dumpEndQueued(const Config::CLUSTER&);
	static void dumpEndQueued(u_int16, const Config::CLUSTER&);
	static void dumpEndQueued(BUACTREC& dumpEndQueue);
	static void dumpEndQueued(u_int16);


	static void startSbcSwitch(const Config::CLUSTER&);
	static void endSbcSwitch(const Config::CLUSTER&);
	static bool onGoingSbcSwitch(const Config::CLUSTER&);

	// The following group of functions keeps the minimum information about loading and
	// dumping in memory. This memory will be checked first before launching the functions
	// in the above group
	// The purpose of this group of functions is to keep the loading/dumping execution time
	// as close to the base product as possible during normal operations
	// During interaction, the above group of function can not be avoided

	static void initIntActData(const Config::CLUSTER&);	// Initialise Loading/Dumping vector

	static BitVector& loadingCPs();		// Ctor on first use
	static BitVector& dumpingCPs();		// Ctor on first use
	static BitVector& readLoadVector();		// Returns Loading Vector
	static BitVector& readDumpVector();		// Returns Dumping Vector
	static void setLoading(CPID, const Config::CLUSTER&);	// Mark CPID is loading

	static void clearLoading(CPID, const Config::CLUSTER&);	// Remove CPID loading mark
	static void setDumping(CPID, const Config::CLUSTER&);	// Mark CPID is dumping
	static void clearDumping(CPID, const Config::CLUSTER&);	// Remove CPID dumping mark

	//
	// NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN

};
//
// inlines
//===========================================================================

#endif
