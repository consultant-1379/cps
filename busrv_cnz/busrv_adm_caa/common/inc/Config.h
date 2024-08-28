#ifndef _CONFIG_H_
#define _CONFIG_H_
/*
NAME
	CONFIG -
LIBRARY 3C++
PAGENAME CONFIG
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE Config.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2002-2023. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION
	BUAP/BUSRV configuration data.
	The class uses eager evalutation (values are read as soon
	as the class is used).
	Implemented as a singleton (use the instance() method to get
	a const reference to the class instance.

ERROR HANDLING
	-

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
	2002-05-06 by U/Y/SF Anders Gillgren (qabgill, tag: ag)

LINKAGE
	-

SEE ALSO
	-

Revision history
----------------
2002-05-06 qabgill Created
2003-01-08 uablan  Change m_onlySBCInFFR to m_sbcToReloadFromFFR, remove dateLimit,
                   change m_sbcTOReloadFromFFR/SFR to int (from bool).
2004-06-24 uablan  Remove of MIN_CLEANUP_DELAY, Cleanup Delay.
2010-11-16 xquydao Update for APG43L project
2011-02-28 xquydao Add isClassicCP method
2015-12-17 egiaava Add vNIC speed handling for vAPZ
2023-01-01 xkomala Added isValidBladeCpid and isValidSpxCpid functions for IA20800
*/

#include "buap.h"
#include "DateTime.h"
//#include "Key.h"
#include "PlatformTypes.h"
#include "SBCId.h"
#include "EventReporter.h"
#include "ParseIni.h"

#include <string>

#include "ACS_CS_API.h"

class CriticalSection;
class Key;

using namespace buap;
using namespace std;


#define _LEGACY_BUP3_SUPPORTED
//#undef _LEGACY_BUP3_SUPPORTED

class Config {
// types
private:
	struct ItemSZ {
		const char* section;
		const char* name;
		const char* defVal;
	};
	struct ItemDW {
		const char* section;
		const char* name;
		int defVal;
	};

public:
	enum {useLDD1 = 1, usePS = 2, useRS = 4 };

public:
	//
	// keys, sections, items & such
	//
	// ini file sections
	static const char* SECT_DEBUG;
	static const char* SECT_CMDLOG;
	static const char* SECT_RELOAD;
	static const char* SECT_VNIC;
	// ini file items
	// [General]
	static const ItemDW ITEM_BUSRV_PORT;
	// [Reload]
	static const ItemDW ITEM_SUPERVISION_TIME;
	static const ItemDW ITEM_SBC_RELOAD_FROM_FFR;
	static const ItemDW ITEM_SBC_RELOAD_FROM_SFR;
	// [Command Log]
	static const ItemDW ITEM_CMDLOG_HANDLING;
	// [VNIC]
	static const ItemDW ITEM_BACKUP_VSPEED;
	//
	// add-ons & common constants
	//
	static const char* CONFIG_FILE;
	static const char* TMP_ADD_ON; 			// add to CPS data
	static const char* RELVOLUMSW_ADD_ON; 	// add to CPF Root
	static const char* DATFILE_ADD_ON; 		// add to cps data
	static const char* BUSRV_ADD_ON; 		// add to CPS data
	static const char* CONF_ADD_ON; 		// add to CPS conf
	static const char* CPF_ADD_ON; 			// add CPF Root
	static const char* CLUSTER_ADD_ON; 		// add CLUSTER
	static const char* CLUSTER_STR; 		// cluster literal string
	
	static const char* LOGICAL_NAME_CPS_DATA;
	static const char* LOGICAL_NAME_FMS_DATA;
	static const char* DSD_SERVICE_NAME;
	static const char* DSD_SERVICE_DOMAIN;
    static const char* MarkerFileName;
    static const char* VCONFIG_FILE;

	enum {
		FTP_PATH_MAX_DIR_LEN = 63,
		FTP_PATH_MAX_DIR_LEN_V4 = 95,
		FTP_PATH_MAX_FILENAME_LEN = 31
	};

	enum {
		MIN_SUPERVISION_TIME = 30,
		DEFAULT_SUPERVISION_TIME = 60,
		MAX_SUPERVISION_TIME = 2400
	};

	enum {
		AUTO = 0,
		MAN = 1
	};

	enum {
		UNDEF = -1,
		SIMAP_ALL_IP_ADDRESS = 0,
		APG_ONE_IP_ADDRESS = 0,
		APG_TWO_IP_ADDRESS = 1
	};

	enum {
		DEFAULT_BACKUP_VSPEED = 1000
	};

	class CLUSTER 
	{
	public:
		CLUSTER(CPID id = 0xffff, bool usename = false) : m_cpId(id), m_useCPNameInPath(usename) {};
		~CLUSTER() {};
				
		string	getCPName() const;
		bool	useCPNameInPath() const {return m_useCPNameInPath; }
	protected:
		CPID	m_cpId;
		bool	m_useCPNameInPath;
	};


//foos
public:
	static Config& instance() {
		if(!s_initialized)
			s_instance.init();
		return s_instance;
	}
// access
	// [General] 
	bool isMultipleCP() const;
	bool isClassicCP() const;
	bool isVAPZ() const;
        bool isValidBladeCpid(CPID cpId); //TR_IA20800
        bool isValidSpxCpid(CPID cpId); //TR_IA20800


	int busrvPort() const;

	// For One CP System, there is no need for the following
	int maxNoOfSimDump() const;
	int maxNoOfSimLoad() const;
	int noOngoingDump() const;
	int noOngoingLoad() const;
	int noOngoingDump(const CLUSTER&) const;
	int noOngoingLoad(const CLUSTER&) const;
	
	// [OM Profile handling]
	u_int16 getAPZProfile() const;

	CriticalSection& CS_Config() const;

	// [Paths]
	string cpfRoot(CPID cpId=0xFFFF) const;
	string cpfRoot(const CLUSTER &) const;

	string confBusrvDir(CPID cpId=0xFFFF) const;
	string confBusrvDir(const CLUSTER&) const;

	string confFile(CPID cpId=0xFFFF) const { return confBusrvDir(cpId) + CONFIG_FILE; }
	string confFile(const CLUSTER& t) const { return confBusrvDir(t) + CONFIG_FILE; }

	string dataBusrvDir(CPID cpId=0xFFFF) const;
	string dataBusrvDir(const CLUSTER&) const;

	string datFile(CPID cpId=0xFFFF) const { return dataBusrvDir(cpId) + DATFILE_ADD_ON; }
	string datFile(const CLUSTER& t) const { return dataBusrvDir(t) + DATFILE_ADD_ON; }

	string ftpLoadDir(SBCId sbcId, CPID cpId=0xFFFF) const;
	string ftpLoadDir(SBCId sbcId, const CLUSTER&) const;

	string ftpDumpDir(const Key& key, CPID cpId=0xFFFF) const;
	string ftpDumpDir(const Key& key, const CLUSTER&) const;
	//string ftpDumpDir(const Key_V4& key, const CLUSTER&) const;

	string relvolumsw(CPID cpId=0xFFFF) const;
	string relvolumsw(const CLUSTER&) const;

	string sbcDir(SBCId sbcId, CPID cpId=0xFFFF) const;
	string sbcDir(SBCId sbcId, const CLUSTER&) const;

	string tmpDir(const Key& key, CPID cpId=0xFFFF) const;
	string tmpDir(const Key& key, const CLUSTER&) const;
	//string tmpDir(const Key_V4& key, const CLUSTER&) const;

	string tmpRoot(CPID cpId=0xFFFF) const;
	string tmpRoot(const CLUSTER&) const;

	// [Debug]
	int isSimAp() const { return m_simAp; }

	// [Reload]
	unsigned int getSupervisionTime(CPID cpId=0xFFFF) const; 
	unsigned int getSupervisionTime(const CLUSTER&) const;

	int getSbcToReloadFromFFR(CPID cpId=0xFFFF) const; 
	int getSbcToReloadFromFFR(const CLUSTER&) const;

	int getSbcToReloadFromSFR(CPID cpId=0xFFFF) const; 
	int getSbcToReloadFromSFR(const CLUSTER&) const;

	// [Command Log]
	int getCmdLogHandling(CPID cpId=0xFFFF) const;
	//int getCmdLogHandling(const CLUSTER&) const;
// modify
	// [Debug]
	void simAp(unsigned int val) { m_simAp = val; }
	// [Reload]
	void supervisionTime(unsigned int val, CPID cpId=0xFFFF);
	void supervisionTime(unsigned int val, const CLUSTER&);

	void sbcToReloadFromFFR(int val, CPID cpId=0xFFFF);
	//void sbcToReloadFromFFR(int val, const CLUSTER&);

	void sbcToReloadFromSFR(int val, CPID cpId=0xFFFF);
	//void sbcToReloadFromSFR(int val, const CLUSTER&);
	//
	// [Command Log]
	void cmdLogHandling(int val, CPID cpId=0xFFFF);
	//void cmdLogHandling(int val, const CLUSTER&);

    // general funx
	void save(unsigned int sup, int ffr, int sfr, CPID cpId=0xFFFF) const;
	void save(unsigned int sup, int ffr, int sfr, const CLUSTER&) const;
	void save() const;

	// This helps with the "set file pointer", later invalidated by
	// Ola's algorithm. Thus not needed. Delete later on
	//void saveFFR(int ffr, const CLUSTER& t) const {
	//	write(ITEM_SBC_RELOAD_FROM_FFR, ffr, t);
	//}
	//void saveSFR(int sfr, const CLUSTER& t) const {
	//	write(ITEM_SBC_RELOAD_FROM_SFR, sfr, t);
	//}

	void dump() const;

	void retreive() { s_initialized = false; };

	void createDirStructure(CPID cpId=0xFFFF); 
	void createDirStructure(const CLUSTER&);

	// This is to create a single Ftp structure
	void createFtpDirStructure(const char*);
	void createDataBusrvDirStructure(const CLUSTER&);
	//void createConfBusrvDirStructure(const CLUSTER&);

	// Protocol Interaction case
	bool isSbcClusterRange(SBCId) const;
	bool isSbcClusterRange(u_int16) const;
	bool isSbcBladeRange(u_int16) const;

	string getPath(const char*, CPID cpId=0xFFFF) const;
	string getPath(const char*, const CLUSTER&) const;
	
	string getCPSPath(CPID cpId=0xFFFF) const;
	string getCPSPath(const CLUSTER&) const;
	string getFMSPath(CPID cpId=0xFFFF) const;
	string getFMSPath(const CLUSTER&) const;

	string getCPSDataPath() const;

	// Validate supervision time
	void validate(int sup) const;

        //TODO: Just for testing
        void crashing(int at, CPID cpId=0xFFFF);
        void crashing(int at, const CLUSTER&);

#ifdef _APGUSERSVC_
        static void setCPSUserId(const unsigned int cpsUserId);
        static unsigned int getCPSUserId() { return s_cpsUserId; };
#endif

private:
	Config();
	Config(const Config& rhs);
	Config& operator=(const Config& rhs);
	void init();

	string read(const ItemSZ&, CPID cpId=0xFFFF) const;
	string read(const ItemSZ&, const CLUSTER&) const;

	u_int32 read(const ItemDW&, CPID cpId=0xFFFF) const;
	u_int32 read(const ItemDW&, const CLUSTER&) const;

	void write(const ItemSZ&, const string& , CPID cpId=0xFFFF) const;
	void write(const ItemSZ&, const string& , const CLUSTER&) const;

	void write(const ItemDW&, u_int32, CPID cpId=0xFFFF) const;
	void write(const ItemDW&, u_int32, const CLUSTER&) const;
	//void createDirStructure();
//attr
private:
	// [Debug]
	unsigned int m_simAp; // listen on/accept from ANY network

	static bool s_initialized; // have we read the values?
	static Config s_instance;

#ifdef _APGUSERSVC_
        static unsigned int s_cpsUserId;
#endif

};
//
// inlines
//===========================================================================
//
//
//============================================================================
/*inline void Config::write(const ItemSZ& item, const string& val) const {
	if(WritePrivateProfileString(item.section, item.name, val.c_str(), m_configFile.c_str()) == false)
		EventReporter::instance().write("Config::write WritePrivateProfileString failed.");
}*/
//
//
//============================================================================
/*inline void Config::write(const ItemDW& item, DWORD val) const {
	char buf[25];
	if (WritePrivateProfileString(item.section, item.name, itoa(val, buf, 10),m_configFile.c_str()) == false)
		EventReporter::instance().write("Config::write WritePrivateProfileString failed.");
}*/

//
//
//============================================================================
inline void Config::write(const ItemSZ& item, const string& val, CPID cpId) const
{
	if(ParseINIFunx::WritePrivateProfileString(item.section, item.name, val.c_str(), confFile(cpId).c_str()) == false)
	{
		EventReporter::instance().write("Config::write WritePrivateProfileString failed.");
	}
}
//
//
//============================================================================
inline void Config::write(const ItemSZ& item, const string& val, const CLUSTER& t) const
{
	if (ParseINIFunx::WritePrivateProfileString(item.section, item.name, val.c_str(), confFile(t).c_str()) == false)
		EventReporter::instance().write("Config::write WritePrivateProfileString failed.");
}
//
//
//============================================================================
inline void Config::write(const ItemDW& item, u_int32 val, CPID cpId) const
{
	if (ParseINIFunx::WritePrivateProfile(item.section, item.name, val, confFile(cpId).c_str()) == false)
	{
		EventReporter::instance().write("Config::write WritePrivateProfile failed.");
	}
}
//
//
//============================================================================
inline void Config::write(const ItemDW& item, u_int32 val, const CLUSTER& t) const
{
	if (ParseINIFunx::WritePrivateProfile(item.section, item.name, val, confFile(t).c_str()) == false)
	{
		EventReporter::instance().write("Config::write WritePrivateProfile failed.");
	}
}


inline bool Config::isSbcClusterRange(SBCId sbcId) const
{
	u_int16 id = sbcId.id();
	//return 0 <= id && id <= 29 || 100 <= id && id <= 127;
	return (id <= SBCId::FFR_LAST_CLUSTER_ID) ||
			(SBCId::SFR_FIRST_ID <= id && id <= SBCId::LAST_ID);
}

inline bool Config::isSbcClusterRange(u_int16 id) const
{
	//return 0 <= id && id <= 29 || 100 <= id && id <= 127;
	return (id <= SBCId::FFR_LAST_CLUSTER_ID) ||
			(SBCId::SFR_FIRST_ID <= id && id <= SBCId::LAST_ID);
}

inline bool Config::isSbcBladeRange(u_int16 id) const
{
	//return 30 <= id && id <= 99;
	return SBCId::BLADE_RANGE_FIRST <= id && id <= SBCId::BLADE_RANGE_LAST;
}

#endif
