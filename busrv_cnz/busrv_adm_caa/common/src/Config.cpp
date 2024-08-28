/*
NAME
	File_name:Config.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2002-2023. All rights reserved.

The Copyright to the computer program(s) herein is the property of Ericsson
Utvecklings AB, Sweden.
The program(s) may be used and/or copied only with the written permission from
Ericsson Utvecklings AB or in accordance with the terms and conditions
stipulated in the agreement/contract under which the program(s) have been
supplied.

DESCRIPTION
	BUAP/BUSRV configuration data.

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
	2002-05-06 by U/Y/SF Anders Gillgren (qabgill, tag :ag)

SEE ALSO
	-

Revision history
----------------
2002-05-06 qabgill Created
2003-01-08 uablan  Change m_onlySBCInFFR to m_sbcToReloadFromFFR, remove dateLimit,
                   change m_sbcTOReloadFromFFR/SFR to int (from bool).
2004-04-?? uablan  Change of registry entry dir from common to common_BUSRV.
2004-06-24 uablan  Remove Cleanup Delay.
2004-09-14 uablan  Add compress property to ftp directory.
2006-10-24 uablan  Add paramter handling.
2010-11-16 xquydao Update for APG43L project
2011-02-28 xquydao Add isClassicCP method
2015-12-17 egiaava Add vNIC speed handling for vAPZ
2023-01-01 xkomala Added isValidBladeCpid and isValidSpxCpid functions for IA20800 
*/


#include "CPS_BUSRV_Trace.h"
#include "Config.h"

#include <set>
#include <climits> // For PATH_MAX
#include <boost/scoped_array.hpp>

#include "CodeException.h"
#include "FileFunx.h"
#include "EventReporter.h"
#include "CPS_Parameters.h"
#include "ParseIni.h"

#include "BUPFunx.h"
#include "DataFile.h"
#include "Key.h"
#include "CriticalSection.h"
#include "ParseIni.h"
#include "ACS_APGCC_CommonLib.h"

// These 2 are for mkdir
#include <sys/stat.h>
#include <sys/types.h>

// For getservbyname
#include <netdb.h>
#include <netinet/in.h>


// ini file sections
const char* Config::SECT_RELOAD = "Reload";
const char* Config::SECT_CMDLOG = "Commando Log";
const char* Config::SECT_VNIC = "VNIC";
// ini file items
// [Reload]
const Config::ItemDW Config::ITEM_SUPERVISION_TIME = { SECT_RELOAD, "Supervision time", DEFAULT_SUPERVISION_TIME };
const Config::ItemDW Config::ITEM_SBC_RELOAD_FROM_FFR = { SECT_RELOAD, "Reload from FFR", 0 };
const Config::ItemDW Config::ITEM_SBC_RELOAD_FROM_SFR = { SECT_RELOAD, "Reload from SFR", 0 };
// [Command Log]
const Config::ItemDW Config::ITEM_CMDLOG_HANDLING = { SECT_CMDLOG, "Command Log Handling", MAN };
// [VNIC]
const Config::ItemDW Config::ITEM_BACKUP_VSPEED = { SECT_VNIC, "Backup vspeed", DEFAULT_BACKUP_VSPEED };
//
// misc. constants
//
const char* Config::CONFIG_FILE = "busrv.ini";
const char* Config::BUSRV_ADD_ON = "/busrv/";
const char* Config::CONF_ADD_ON = "conf/";
const char* Config::TMP_ADD_ON = "ftp/";
const char* Config::RELVOLUMSW_ADD_ON = "RELVOLUMSW/";
const char* Config::DATFILE_ADD_ON = "busrv.dat";
const char* Config::CPF_ADD_ON = "/cpf/";
const char* Config::CLUSTER_STR = "cluster";
const char* Config::CLUSTER_ADD_ON = "/cluster";

const char* Config::LOGICAL_NAME_CPS_DATA = "CPS_DATA";
const char* Config::LOGICAL_NAME_FMS_DATA = "FMS_DATA";

const char* Config::DSD_SERVICE_NAME = "BUSRVDSD";;
const char* Config::DSD_SERVICE_DOMAIN = "SBC";
const char* Config::MarkerFileName = "ADMARKER";
const char* Config::VCONFIG_FILE = "vConfig.ini";

//
// singleton statics
//
Config Config::s_instance;
bool Config::s_initialized = false;

#ifdef _APGUSERSVC_
unsigned int Config::s_cpsUserId = 0;
#endif

//
//
// ctor (private)
//===========================================================================
Config::Config() : m_simAp(UNDEF) { }
//
//
//===========================================================================
void Config::init()
{
	//newTRACE((LOG_LEVEL_INFO, "BUSRV Run ==>> Config::init()", 0));

	m_simAp = UNDEF;	
	s_initialized = true;
}

//
//
//===========================================================================
//
//
// Config::CLUSTER
// ============================================================================
string Config::CLUSTER::getCPName() const
{
	char name[10];
	name[0] = '/';
	bool validBladeId = m_useCPNameInPath && m_cpId <= 63;

	if (validBladeId && BUPFunx::convertCPIdtoCPName(m_cpId, &name[1]) == ACS_CS_API_NS::Result_Success) {
		return string(name);
	}

	return string("");
}
//
//
//===========================================================================
bool Config::isMultipleCP() const
{
    //newTRACE(("Config::isMultipleCP()", 0));
    static bool multCPSys = false;
    static bool contactCS = false;

	if (!contactCS) {
	    ACS_CS_API_NS::CS_API_Result res = ACS_CS_API_NetworkElement::isMultipleCPSystem(multCPSys);
	    if (res != ACS_CS_API_NS::Result_Success) {
	        //TRACE(("Config::isMultipleCP: failed to get info is multiple CP System, error %d", 0, res));
	        THROW_XCODE_LOG("Failed to get system configuration: ", CodeException::CP_APG43_CONFIG_ERROR);
	    }
	    contactCS = true;
	}
    //TRACE(("Config::isMultipleCP() returns %d", 0, multCPSys));
    return multCPSys;
}
//
//
//===========================================================================
//  Parameter   cpAndProtocolType (old name: ACS_APCONFBIN_CpAndProtocolType)
//  Format  unsignedInt
//
//  Contains information about the CP type the AP is connected to and what
//  communication protocol to use on the processor test bus when communicating
//  with the CP
//
//           Value   CP type       Protocol
//           -----   -------       --------
//           0       APZ 212 3x    SDLC
//           1       APZ 212 3x    TCP/IP
//           2       APZ 212 40    TCP/IP
//           3       APZ 212 50    TCP/IP
//           4       APZ 212 55    TCP/IP
//           4       MSC-S BC SPX  TCP/IP
bool Config::isClassicCP() const
{
    newTRACE((LOG_LEVEL_INFO, "Config::isClassicCP()", 0));

#if 0
    static bool isClassicSys = false;
    static bool contactIMM = false;

    if (contactIMM)
    {
        // No need to fetch the info again
        // Just use the previous value
        return isClassicSys;
    }
#else
    bool isClassicSys = false;
#endif

    // Get the AxeFunctions DN
    OmHandler immHandle;
    ACS_CC_ReturnType ret;
    std::vector<std::string> dnList;

    ret = immHandle.Init();
    if (ret != ACS_CC_SUCCESS)
    {
        TRACE((LOG_LEVEL_ERROR, "Init OMHandler failed: %d - %s", 0,
                immHandle.getInternalLastError(),
                immHandle.getInternalLastErrorText()));

        THROW_XCODE_LOG("Config::isClassicCP(): Reading PHA param apzProtocolType failed", CodeException::CP_APG43_CONFIG_ERROR);
    }

    ret = immHandle.getClassInstances("AxeFunctions", dnList);

    if (ret != ACS_CC_SUCCESS)
    {
        TRACE((LOG_LEVEL_ERROR, "Get class instance failed: %d - %s", 0,
               immHandle.getInternalLastError(),
               immHandle.getInternalLastErrorText()));

        immHandle.Finalize();
        THROW_XCODE_LOG("Config::isClassicCP(): Reading PHA param apzProtocolType failed", CodeException::CP_APG43_CONFIG_ERROR);
    }
    else
    {
        immHandle.Finalize();
        TRACE((LOG_LEVEL_INFO, "AxeFunctions DN: %s", 0, dnList[0].c_str()));
    }

    acs_apgcc_paramhandling phaObject;
    const char *pAttrName = "apzProtocolType";
    int32 apzProtocolTypeValue = 0;

    ret = phaObject.getParameter(dnList[0], pAttrName, &apzProtocolTypeValue);

    if (ret == ACS_CC_SUCCESS)
    {
        // Marked that value is obtained from IMM
        //contactIMM = true;

        if ((apzProtocolTypeValue == 0) || (apzProtocolTypeValue == 1))
        {
            isClassicSys = true;
        }
    }
    else
    {
        TRACE((LOG_LEVEL_ERROR, "Config::isClassicCP(): Reading PHA param apzProtocolType failed", 0));
        THROW_XCODE_LOG("Config::isClassicCP(): Reading PHA param apzProtocolType failed", CodeException::CP_APG43_CONFIG_ERROR);
    }

    return isClassicSys;
}

//------------------------------------------------------------------------------------------
// Check if vAPZ
//------------------------------------------------------------------------------------------
bool Config::isVAPZ() const
{
	ACS_CS_API_NS::CS_API_Result result;
	ACS_CS_API_CommonBasedArchitecture::ArchitectureValue arcValue;

	// Check if the is VIRTUALIZED ENV
	result = ACS_CS_API_NetworkElement::getNodeArchitecture(arcValue);
	newTRACE((LOG_LEVEL_INFO, "Config::isVAPZ()", 0));
	TRACE((LOG_LEVEL_INFO, "Config::isVAPZ(): arcValue = %d", 0, arcValue));
	return (result == ACS_CS_API_NS::Result_Success) && (arcValue == ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED);
}

//TR_IA20800 START
//------------------------------------------------------------------------------------------
// Check if valid cpid received for blade
//------------------------------------------------------------------------------------------
bool Config::isValidBladeCpid(CPID cpId)
{
         return ((cpId >= 0) && (cpId <=63));
}

//------------------------------------------------------------------------------------------
// Check if valid cpid received for SPX
//------------------------------------------------------------------------------------------
bool Config::isValidSpxCpid(CPID cpId)
{
     return ((cpId == 1001) || (cpId == 1002));
}
//TR_IA20800 END

//
// Get busrv port number from /etc/services
//===========================================================================
int Config::busrvPort() const
{
    //newTRACE((LOG_LEVEL_INFO, "Config::busrvPort()", 0));

    struct servent *serv = NULL;
    const int DEF_PORT = 1003;

    serv = getservbyname("busrv", "tcp");

    if (serv == NULL)
    {
        //TRACE((LOG_LEVEL_WARN, "Failed to obtain the busrv port. Use default: %d", 0, DEF_PORT));
        return DEF_PORT;
    }

    return ntohs(serv->s_port);
}

//
// Get max allowed no of simultaniuos cp dumps from parameter file
//===========================================================================
int Config::maxNoOfSimDump() const 
{ 
	newTRACE((LOG_LEVEL_INFO, "Config::maxNoOfSimDump()", 0));

	CPS_Parameters cps; 
	return cps.get(CPS_Parameters::MAX_SIMULTANEOUS_DUMP, 4);
}
//
// Get max allowed no of simultaniuos cp load from parameter file
//===========================================================================
int Config::maxNoOfSimLoad() const 
{ 
	newTRACE((LOG_LEVEL_INFO, "Config::maxNoOfSimLoad()", 0));

	CPS_Parameters cps; 
	return cps.get(CPS_Parameters::MAX_SIMULTANEOUS_LOAD, 4);
}
//
// Get number of ongoing cp dumps for blades
// also check if dump has been going on for to long time. Then asume that
// the CP is dead and reset the dump flag.
//===========================================================================
int Config::noOngoingDump() const { 
	newTRACE((LOG_LEVEL_INFO, "Config::noOngoingDump()", 0));

	unsigned int noOfOngoingDump = 0;
	DateTime currTime;
	currTime.now();

	ACS_CS_API_IdList cpList;
	
	ACS_CS_API_CP * CP = ACS_CS_API::createCPInstance();
	ACS_CS_API_NS::CS_API_Result res = CP->getCPList(cpList);

	if (res != ACS_CS_API_NS::Result_Success){
		if (res == ACS_CS_API_NS::Result_Failure){
			//make a second try
			res = CP->getCPList(cpList);
			if (res != ACS_CS_API_NS::Result_Success){
				ACS_CS_API::deleteCPInstance(CP);
				TRACE((LOG_LEVEL_ERROR, "Config::noOngoingDump: get CP List failed with error %d", 0, res));
				THROW_XCODE_LOG("noOngoingDump: Get list of CP's failed: %d", CodeException::CP_APG43_CONFIG_ERROR);
			}
		}
		else {
			ACS_CS_API::deleteCPInstance(CP);
			TRACE((LOG_LEVEL_ERROR, "Config::noOngoingDump: get CP List failed with error %d", 0, res));
			THROW_XCODE_LOG("noOngoingDump: Get list of CP's failed: %d", CodeException::CP_APG43_CONFIG_ERROR);
		}
	}

	// For all possible blades count number of ongoing dumps
	for (u_int32 i = 0; i < cpList.size(); ++i) {

		// check for how long the dump been ongoing.
		// if longer than 60 min, asume that the CP is dead ann reset the dumpflag for this cp

		//TRACE(("Ongoing Dump = %d on BC%d", 0, DataFile::dumping(cpList[i]), cpList[i]));		
		if ( DataFile::dumping(cpList[i]) ) {
			DateTime dumpTime = DataFile::startTimeForOngoingDump(cpList[i]);
			dumpTime.addMinutes(60);
			//TRACE(("current:%d, %d, %d, %d, %d", 0, currTime.year(), currTime.month(), currTime.day(), currTime.hour(),currTime.minute()));
			//TRACE(("dumpTime+60:%d, %d, %d, %d, %d", 0, dumpTime.year(), dumpTime.month(), dumpTime.day(), dumpTime.hour(),dumpTime.minute()));
			if ( currTime.cmp(dumpTime) > 0 )
				DataFile::endDumping(cpList[i]);
		}
		if (DataFile::dumping(cpList[i]))
			noOfOngoingDump++;;

	}
	
	ACS_CS_API::deleteCPInstance(CP);

	TRACE((LOG_LEVEL_INFO, "noOngoingDump: %d", 0, noOfOngoingDump));
	return noOfOngoingDump;
}
//
// Get number of ongoing cp dumps for blades
// also check if dump has been going on for to long time. Then asume that
// the CP is dead and reset the dump flag.
//===========================================================================
int Config::noOngoingDump(const CLUSTER&) const { 
	newTRACE((LOG_LEVEL_INFO, "Config::noOngoingDump(CLUSTER)", 0));

	unsigned int noOfOngoingDump = 0;
	DateTime currTime;
	currTime.now();

	ACS_CS_API_IdList cpList;
	
	ACS_CS_API_CP * CP = ACS_CS_API::createCPInstance();
	ACS_CS_API_NS::CS_API_Result res = CP->getCPList(cpList);

	if (res != ACS_CS_API_NS::Result_Success){
		if (res == ACS_CS_API_NS::Result_Failure){
			//make a second try
			res = CP->getCPList(cpList);
			if (res != ACS_CS_API_NS::Result_Success){
				ACS_CS_API::deleteCPInstance(CP);
				TRACE((LOG_LEVEL_ERROR, "Config::noOngoingDump: get CP List failed with error %d", 0, res));
				THROW_XCODE_LOG("noOngoingDump: Get list of CP's failed: %d", CodeException::CP_APG43_CONFIG_ERROR);
			}
		}
		else {
			ACS_CS_API::deleteCPInstance(CP);
			TRACE((LOG_LEVEL_ERROR, "Config::noOngoingDump: get CP List failed with error %d", 0, res));
			THROW_XCODE_LOG("noOngoingDump: Get list of CP's failed: %d", CodeException::CP_APG43_CONFIG_ERROR);
		}
	}
	
	CPS_Parameters cps;
	int maxDumpTime = cps.get(CPS_Parameters::MAX_DUMPING_SUPERVISION_TIME, 60);

	// For all possible blades count number of ongoing dumps
	for (u_int32 i = 0; i < cpList.size(); ++i) {

		// check for how long the dump been ongoing.
		// if longer than 60 min, asume that the CP is dead ann reset the dumpflag for this cp

		CPID tCpId = cpList[i];
		if (tCpId > 63)
			continue;

		const CLUSTER tCluster(tCpId, true);
		//TRACE(("Ongoing Dump (0/1 false/true) = %d on cpId %d", 0, DataFile::dumping(tCluster), tCpId));		
		if ( DataFile::dumping(tCluster) ) {
			DateTime dumpTime = DataFile::startTimeForOngoingDump(tCluster);
			dumpTime.addMinutes(maxDumpTime);
			//TRACE(("current:%d, %d, %d, %d, %d", 0, currTime.year(), currTime.month(), currTime.day(), currTime.hour(),currTime.minute()));
			//TRACE(("dumpTime+%d:%d, %d, %d, %d, %d", 0, maxDumpTimer, dumpTime.year(), dumpTime.month(), dumpTime.day(), dumpTime.hour(),dumpTime.minute()));
			if ( currTime.cmp(dumpTime) > 0 ) {
				DataFile::endDumping(tCluster);
				DataFile::endDumpingSBCID(tCluster);
			}
		}
		if (DataFile::dumping(tCluster))
			noOfOngoingDump++;;

	}
	
	ACS_CS_API::deleteCPInstance(CP);

	TRACE((LOG_LEVEL_INFO, "noOngoingDump returns: %d", 0, noOfOngoingDump));
	return noOfOngoingDump;
}
//
//
// Get number of ongoing cp loadings
//===========================================================================
int Config::noOngoingLoad() const { 
	newTRACE((LOG_LEVEL_INFO, "Config::noOngoingLoad()", 0));

	unsigned int noOfOngoingLoad = 0;
	DateTime currTime;
	currTime.now();

	ACS_CS_API_IdList cpList;
	
	ACS_CS_API_CP * CP = ACS_CS_API::createCPInstance();
	ACS_CS_API_NS::CS_API_Result res = CP->getCPList(cpList);

	if (res != ACS_CS_API_NS::Result_Success){
		ACS_CS_API::deleteCPInstance(CP);
		TRACE((LOG_LEVEL_ERROR, "Config::noOngoingLoad: get CP List failed with error %d", 0, res));
		THROW_XCODE_LOG("noOngoingLoad: Get list of CP's failed: %d", CodeException::CP_APG43_CONFIG_ERROR);
	}

	// For all possible blades count number of ongoing dumps
	CPID cpid;
	for (u_int32 i = 0; i <  cpList.size(); ++i) {

		// check for how long the load been ongoing.
		// if longer than 20 min, asume that the CP is dead ann reset the load flag for this cp
		cpid = cpList[i];
		if ( DataFile::loading(cpid) ) {
			DateTime loadTime = DataFile::startTimeForOngoingLoad(cpid);
			loadTime.addMinutes(20);
			if ( currTime.cmp(loadTime) > 0 )
				DataFile::endLoading(cpid);
		}
		if (DataFile::loading(cpid)) {
			//cout << "index i = " << i << " cpid " << cpid << " is loading " << endl;
			noOfOngoingLoad++;
		}
		//else
			//cout << "index i = " << i << " cpid " << cpid << " is not loading " << endl;
		
		//TRACE(("noOngoingLoad: %d", 0, noOfOngoingLoad));		
	}
	
	ACS_CS_API::deleteCPInstance(CP);

	TRACE((LOG_LEVEL_INFO, "noOfOngoingLoad: %d", 0, noOfOngoingLoad));
	return noOfOngoingLoad;
}
//
// Get number of ongoing cp loadings
//===========================================================================
int Config::noOngoingLoad(const CLUSTER&) const { 
	newTRACE((LOG_LEVEL_INFO, "Config::noOngoingLoad(const CLUSTER)", 0));

	unsigned int noOfOngoingLoad = 0;
	DateTime currTime;
	currTime.now();

	ACS_CS_API_IdList cpList;
	ACS_CS_API_CP * CP = ACS_CS_API::createCPInstance();

	ACS_CS_API_NS::CS_API_Result res = CP->getCPList(cpList);

	if (res != ACS_CS_API_NS::Result_Success){
		ACS_CS_API::deleteCPInstance(CP);
		TRACE((LOG_LEVEL_ERROR, "Config::noOngoingDump: get CP List failed with error %d", 0, res));
		THROW_XCODE_LOG("noOngoingLoad: Get list of CP's failed: %d", CodeException::CP_APG43_CONFIG_ERROR);
	}

	// For all possible blades count number of ongoing loads
	// This is default value; should read from busrv.ini for BT purposes
	CPS_Parameters cps;
	int maxLoadTime = cps.get(CPS_Parameters::MAX_LOADING_SUPERVISION_TIME, 20); 

	for (u_int32 i = 0; i <  cpList.size(); ++i) {

		// check for how long the load been ongoing.
		// if longer than 20 min, asume that the CP is dead ann reset the load flag for this cp
		CPID tCpId = cpList[i];
		if (tCpId > 63)
			continue;

		const CLUSTER tCluster(tCpId, true);
		if ( DataFile::loading(tCluster) || 
			DataFile::onGoingLoadSBCID(tCluster).id() < 128 ) {
			DateTime loadTime = DataFile::startTimeForOngoingLoad(tCluster);
			loadTime.addMinutes(maxLoadTime);
			if ( currTime.cmp(loadTime) > 0 ) {
				DataFile::endLoading(tCluster);
				DataFile::endLoadingSBCID(tCluster);
			}
		}
		if (DataFile::loading(tCluster))
			noOfOngoingLoad++;
		//TRACE(("noOngoingLoad: %d", 0, noOfOngoingLoad));		
	}
	ACS_CS_API::deleteCPInstance(CP);
	TRACE((LOG_LEVEL_INFO, "noOfOngoingLoad: %d", 0, noOfOngoingLoad));
	return noOfOngoingLoad;
}

//
// Handling APZ Profile
//===========================================================================
u_int16 Config::getAPZProfile() const
{
    //ACS_CS_API_OmProfileChange_R1 omProfile;
    
	newTRACE((LOG_LEVEL_INFO, "Config::getAPZProfile()", 0));
	TRACE((LOG_LEVEL_TRACE, "Config::getAPZProfile() work around; Read from CLUSTER config file", 0));

    // Read from internal data structure
    // If it is not there, return 100
    return ParseINIFunx::GetPrivateProfileInt("APZ Profile", "APZ Profile", 100, confFile(CLUSTER()).c_str());
}

CriticalSection& Config::CS_Config() const
{
	// This is to ensure that s_cs is constructed exactly once on first use
	static CriticalSection s_cs;
	return s_cs;
}
//
//
// Return /data/fms/data/<cp>/cpf
//===========================================================================
string Config::cpfRoot(CPID cpId) const
{
    newTRACE((LOG_LEVEL_INFO, "Config::cpfRoot(CPID = %d)", 0, cpId));

    //string tmpPath = getPath(LOGICAL_NAME_FMS_DATA, cpId);
    string tmpPath = getFMSPath(cpId);
    tmpPath.append(CPF_ADD_ON);
    TRACE((LOG_LEVEL_INFO, "cpfRoot = %s", 0, tmpPath.c_str()));

    return tmpPath;
}
//
// Return /data/fms/data/<cp>/cpf
//===========================================================================
string Config::cpfRoot(const CLUSTER& t) const 
{
	newTRACE((LOG_LEVEL_INFO, "Config::cpfRoot(CLUSTER)", 0));

	//string tmpPath = getPath(LOGICAL_NAME_FMS_DATA, t);
	string tmpPath = getFMSPath(t);
	tmpPath.append(CLUSTER_ADD_ON);
	tmpPath.append(CPF_ADD_ON);
	TRACE((LOG_LEVEL_INFO, "cpfRoot = %s", 0, tmpPath.c_str()));

	return tmpPath;
}
//
// Return /data/cps/data/<cp>/busrv/conf
//===========================================================================
string Config::confBusrvDir(CPID cpId) const
{
    newTRACE((LOG_LEVEL_INFO, "Config::confBusrvDir(CPID = %d)", 0, cpId));
 
    //string tmpPath = getPath(LOGICAL_NAME_CPS_DATA, cpId);
    string tmpPath = getCPSPath(cpId);
    tmpPath.append(BUSRV_ADD_ON);
    tmpPath.append(CONF_ADD_ON);
    TRACE((LOG_LEVEL_INFO, "confBusrvDir = %s", 0, tmpPath.c_str()));

    return tmpPath;
}
//
//
// Return /data/cps/data/<cp>/busrv/
//===========================================================================
string Config::confBusrvDir(const CLUSTER& t) const 
{
	newTRACE((LOG_LEVEL_INFO, "Config::confBusrvDir(CLUSTER)", 0));

	//string tmpPath = getPath(LOGICAL_NAME_CPS_DATA, t);
	string tmpPath = getCPSPath(t);
	tmpPath.append(CLUSTER_ADD_ON);
	tmpPath.append(BUSRV_ADD_ON);
    tmpPath.append(CONF_ADD_ON);
	TRACE((LOG_LEVEL_INFO, "confBusrvDir = %s", 0, tmpPath.c_str()));

	return tmpPath;
}
//
//
// Return /data/cps/data/<cp>/busrv/
//===========================================================================
string Config::dataBusrvDir(CPID cpId) const
{
    newTRACE((LOG_LEVEL_INFO, "Config::dataBusrvDir(CPID = %d)", 0, cpId));

    //string tmpPath = getPath(LOGICAL_NAME_CPS_DATA, cpId);
    string tmpPath = getCPSPath(cpId);
    tmpPath.append(BUSRV_ADD_ON);
    TRACE((LOG_LEVEL_INFO, "dataBusrvDir = %s", 0, tmpPath.c_str()));

    return tmpPath;
}
//
//
// Return Return /data/cps/data/cluster/busrv/
//===========================================================================
string Config::dataBusrvDir(const CLUSTER& t) const {
	newTRACE((LOG_LEVEL_INFO, "Config::dataBusrvDir(CLUSTER)", 0));
	string tmpPath = "";

	//tmpPath = getPath(LOGICAL_NAME_CPS_DATA, t);
	tmpPath = getCPSPath(t);
	tmpPath.append(CLUSTER_ADD_ON);
	tmpPath += t.getCPName();
	tmpPath.append(BUSRV_ADD_ON);
	TRACE((LOG_LEVEL_INFO, "dataBusrvDir = %s", 0, tmpPath.c_str()));

	//cout << "Config::dataBusrvDir(CLUSTER) tmpPath is: " << tmpPath << endl;

	return tmpPath;
}
//
// create FTP load path
//===========================================================================
string Config::ftpLoadDir(SBCId sbcId, CPID cpId) const
{
    newTRACE((LOG_LEVEL_INFO, "Config::ftpLoadDir(%d)", 0, sbcId.id()));

    CPS_Parameters cps;
    string ftpLoadDir = cps.get(CPS_Parameters::FTP_LOAD_ROOT, "/CPSLOAD/");;
    TRACE((LOG_LEVEL_INFO, "Virtual FTPLoadRoot: %s", 0, ftpLoadDir.c_str()));

    if (Config::instance().isMultipleCP() && cpId != 0xFFFF){
        // get default name for CP with cpId
        char name[10];
        name[0] = '/';
        if (int res = BUPFunx::convertCPIdtoCPName(cpId, &name[1])){
            TRACE((LOG_LEVEL_ERROR, "Convert cpid %d to cp name failed. err: %d", 0, cpId, res));
            THROW_XCODE_LOG("Convert CPId to CP name failed: %d", CodeException::CP_APG43_CONFIG_ERROR);
        }

        ftpLoadDir.append(name);
    }
    else if (!Config::instance().isMultipleCP() && cpId == 0xFFFF){
        // ONE CP System
        TRACE((LOG_LEVEL_INFO, "ONE CP System", 0));
    }
    else if (Config::instance().isMultipleCP() && cpId == (CPID) 0xFFFF) {
        // The System is faulty, CP = Multi CP system and APG = One CP system
        THROW_XCODE_LOG(" CP = Multi CP system and APG = One CP system: ", CodeException::CP_APG43_CONFIG_ERROR);
    }
    else if (!Config::instance().isMultipleCP() && cpId != (CPID) 0xFFFF) {
        // The System is faulty, CP = One CP system and APG = Multi CP system
        THROW_XCODE_LOG(" CP = One CP system and APG = Multi CP system: ", CodeException::CP_APG43_CONFIG_ERROR);
    }
    else    // The System is faulty, this is not a valid option
        THROW_XCODE_LOG(" The CP and APG is faulty: ", CodeException::CP_APG43_CONFIG_ERROR);

    ftpLoadDir.append(CPF_ADD_ON);
    ftpLoadDir.append(RELVOLUMSW_ADD_ON);
    ftpLoadDir.append(sbcId.dirName());
    ftpLoadDir.append("/");
    return ftpLoadDir;
}
//
// create FTP load path
//===========================================================================
string Config::ftpLoadDir(SBCId sbcId, const CLUSTER&) const 
{
	newTRACE((LOG_LEVEL_INFO, "Config::ftpLoadDir(%d, CLUSTER)", 0, sbcId.id()));

    CPS_Parameters cps;
    string ftpLoadDir = cps.get(CPS_Parameters::FTP_LOAD_ROOT, "/CPSLOAD/");;
    TRACE((LOG_LEVEL_INFO, "Virtual FTPLoadRoot: %s", 0, ftpLoadDir.c_str()));


	ftpLoadDir.append(CLUSTER_ADD_ON);
	ftpLoadDir.append(CPF_ADD_ON);
	ftpLoadDir.append(RELVOLUMSW_ADD_ON);
	ftpLoadDir.append(sbcId.dirName());
	ftpLoadDir.append("/");
	return ftpLoadDir;
}
//
//
// Create FTP dump path /data/cps/data/<cp>/busrv/ftp/<uuid>/
//===========================================================================
string Config::ftpDumpDir(const Key& key, CPID cpId) const
{
    newTRACE((LOG_LEVEL_INFO, "Config::ftpDumpDir(%s, %d)", 0, key.c_str(), cpId));

    CPS_Parameters cps;
    string ftpDumpDir = cps.get(CPS_Parameters::FTP_DUMP_ROOT, "/CPSDUMP/");
    TRACE((LOG_LEVEL_INFO, "Virtual FTPDumpRoot: %s", 0, ftpDumpDir.c_str()));

    if (Config::instance().isMultipleCP() && cpId != 0xFFFF){
        // get default name for CP with cpId
        char name[10];
        name[0] = '/';
        if (int res = BUPFunx::convertCPIdtoCPName(cpId, &name[1])){
            TRACE((LOG_LEVEL_ERROR, "Convert cpid %d to cp name failed. err: %d", 0, cpId, res));
            THROW_XCODE_LOG("Convert CPId to CP name failed: %d", CodeException::CP_APG43_CONFIG_ERROR);
        }
        ftpDumpDir.append(name);
    }
    else if (!Config::instance().isMultipleCP() && cpId == (CPID) 0xFFFF){
        // ONE CP System
        TRACE((LOG_LEVEL_INFO, "ONE CP System", 0));
    }
    else if (Config::instance().isMultipleCP() && cpId == (CPID) 0xFFFF) {
        // The System is faulty, CP = Multi CP system and APG = One CP system
        THROW_XCODE_LOG(" CP = Multi CP system and APG = One CP system: ", CodeException::CP_APG43_CONFIG_ERROR);
    }
    else if (!Config::instance().isMultipleCP() && cpId != (CPID) 0xFFFF) {
        // The System is faulty, CP = One CP system and APG = Multi CP system
        THROW_XCODE_LOG(" CP = One CP system and APG = Multi CP system: ", CodeException::CP_APG43_CONFIG_ERROR);
    }
    else    // The System is faulty, this is not a valid option
        THROW_XCODE_LOG(" The CP and APG is faulty: ", CodeException::CP_APG43_CONFIG_ERROR);

    ftpDumpDir.append(BUSRV_ADD_ON);
    ftpDumpDir.append(TMP_ADD_ON);
    ftpDumpDir.append(key.c_str());
    ftpDumpDir.append("/");
    TRACE((LOG_LEVEL_INFO, "Config::ftpDumpDir() returns %s", 0, ftpDumpDir.c_str()));
    return ftpDumpDir;
}
//
//
// Create FTP dump path /data/cps/data/cluster/<cp>/busrv/ftp/<uuid>/
//===========================================================================
string Config::ftpDumpDir(const Key& key, const CLUSTER& t) const 
{
	newTRACE((LOG_LEVEL_INFO, "Config::ftpDumpDir(%s, CLUSTER)", 0, key.c_str()));

    CPS_Parameters cps;
    string ftpDumpDir = cps.get(CPS_Parameters::FTP_DUMP_ROOT, "/CPSDUMP/");
    TRACE((LOG_LEVEL_INFO, "Virtual FTPDumpRoot: %s", 0, ftpDumpDir.c_str()));
	
	ftpDumpDir.append(CLUSTER_ADD_ON);
	ftpDumpDir += t.getCPName();
	ftpDumpDir.append(BUSRV_ADD_ON);
	ftpDumpDir.append(TMP_ADD_ON);
	ftpDumpDir.append(key.c_str());
	ftpDumpDir.append("/");

	return ftpDumpDir;

}
//
//
// Create relvolumsw path /data/fms/data/cpf/RELVOLUMSW/
//===========================================================================
string Config::relvolumsw(CPID cpId) const
{
    newTRACE((LOG_LEVEL_INFO, "Config::relvolumsw(CPID = %d)", 0, cpId));

    //string tmpPath = getPath(LOGICAL_NAME_FMS_DATA, cpId);
    string tmpPath = getFMSPath(cpId);

    tmpPath.append(CPF_ADD_ON);
    tmpPath.append(RELVOLUMSW_ADD_ON);
    TRACE((LOG_LEVEL_INFO, "relvolumsw = %s", 0, tmpPath.c_str()));

    return tmpPath;
}
//
//
// Create relvolumsw path /data/fms/data/cluster/cpf/RELVOLUMSW/
//===========================================================================
string Config::relvolumsw(const CLUSTER& t) const 
{
	newTRACE((LOG_LEVEL_INFO, "Config::relvolumsw(CLUSTER)", 0 ));
	
	//string tmpPath = getPath(LOGICAL_NAME_FMS_DATA, t);
	string tmpPath = getFMSPath(t);
	tmpPath.append(CLUSTER_ADD_ON);
	tmpPath.append(CPF_ADD_ON);
	tmpPath.append(RELVOLUMSW_ADD_ON);
	TRACE((LOG_LEVEL_INFO, "relvolumsw = %s", 0, tmpPath.c_str()));
	
	return tmpPath;
}
//
//
// Create SBC path /data/fms/data/<cp>/RELVOLUMSW/<SBC id>/
//===========================================================================
string Config::sbcDir(SBCId sbcId, CPID cpId) const
{
    newTRACE((LOG_LEVEL_INFO, "Config::sbcDir(SBCId = %d, CPID = %d)", 0, sbcId.id(), cpId));
        
    string s = relvolumsw(cpId);
    s += sbcId.dirName();
    s += "/";
    TRACE((LOG_LEVEL_INFO, "sbcDir = %s", 0, s.c_str()));
    return s;
}
//
//
// Create SBC path /data/fms/data/cluster/RELVOLUMSW/<SBC id>/
//===========================================================================
string Config::sbcDir(SBCId sbcId, const CLUSTER& t) const
{
	newTRACE((LOG_LEVEL_INFO, "Config::sbcDir(SBCId = %d, CLUSTER)", 0, sbcId.id()));
	
	string s = relvolumsw(t);
	s += sbcId.dirName();
	s += "/";

	//cout << "Config::sbcDir(SBCId sbcId, Config::CLUSTER t)" << endl;
	//cout << "sbc dir: " << s << endl;

	TRACE((LOG_LEVEL_INFO, "sbcDir = %s", 0, s.c_str()));
 	return s;
}
//
//
// Create tmpDir path /data/cps/data/<cp>/busrv/ftp/<uuid>
//===========================================================================
string Config::tmpDir(const Key& key, CPID cpId) const
{
    newTRACE((LOG_LEVEL_INFO, "Config::tmpDir(Key = %s, CPID = %d)", 0, key.c_str(), cpId));
    
    //string tmpPath = getPath(LOGICAL_NAME_CPS_DATA, cpId);
    string tmpPath = getCPSPath(cpId);
    tmpPath.append(BUSRV_ADD_ON);
    tmpPath.append(TMP_ADD_ON);
    tmpPath.append(key.c_str());
    tmpPath.append("/");
    TRACE((LOG_LEVEL_INFO, "tmpDir = %s", 0, tmpPath.c_str()));

    return tmpPath;
}
//
//
// Create tmpDir path /data/cps/data/cluster/<cp>/busrv/ftp/<uuid>
//===========================================================================
string Config::tmpDir(const Key& key, const CLUSTER& t) const 
{
	newTRACE((LOG_LEVEL_INFO, "Config::tmpDir(Key = %s, CLUSTER)", 0, key.c_str()));
	
	//string tmpPath = getPath(LOGICAL_NAME_CPS_DATA, t);
	string tmpPath = getCPSPath(t);
	tmpPath.append(CLUSTER_ADD_ON);
	tmpPath += t.getCPName();
	tmpPath.append(BUSRV_ADD_ON);
	tmpPath.append(TMP_ADD_ON);
	tmpPath.append(key.c_str());
	tmpPath.append("/");
	TRACE((LOG_LEVEL_INFO, "tmpDir = %s", 0, tmpPath.c_str()));

	return tmpPath;
}
//
//
// 
// Create tmpRoot path /data/cps/data/<cp>/busrv/ftp/
//===========================================================================
string Config::tmpRoot(CPID cpId) const
{
    newTRACE((LOG_LEVEL_INFO, "Config::tmpRoot(CPID = %d)", 0, cpId));
    
    //string tmpPath = getPath(LOGICAL_NAME_CPS_DATA, cpId);
    string tmpPath = getCPSPath(cpId);
    tmpPath.append(BUSRV_ADD_ON);
    tmpPath.append(TMP_ADD_ON);
    TRACE((LOG_LEVEL_INFO, "tmpRoot = %s", 0, tmpPath.c_str()));

    return tmpPath;
}
//
//
// 	Create tmpRoot path /data/cps/data/cluster/<cp>/busrv/ftp/
//===========================================================================
string Config::tmpRoot(const CLUSTER& t) const 
{
	newTRACE((LOG_LEVEL_INFO, "Config::tmpRoot(CLUSTER)", 0));
	
	//string tmpPath = getPath(LOGICAL_NAME_CPS_DATA, t);
	string tmpPath = getCPSPath(t);
	tmpPath.append(CLUSTER_ADD_ON);
	tmpPath += t.getCPName();
	tmpPath.append(BUSRV_ADD_ON);
	tmpPath.append(TMP_ADD_ON);
	TRACE((LOG_LEVEL_INFO, "tmpRoot = %s", 0, tmpPath.c_str()));

	return tmpPath;
}
//
//
//
//============================================================================
unsigned int Config::getSupervisionTime(CPID cpId) const
{
    return read(ITEM_SUPERVISION_TIME, cpId);
}
//
//
//
//============================================================================
unsigned int Config::getSupervisionTime(const CLUSTER& t) const {

	AutoCS a(CS_Config());
	return read(ITEM_SUPERVISION_TIME, t);
}
//
//
//============================================================================
int Config::getSbcToReloadFromFFR(CPID cpId) const
{
    return read(ITEM_SBC_RELOAD_FROM_FFR, cpId);
}
//
//
//============================================================================
int Config::getSbcToReloadFromFFR(const CLUSTER& t) const
{
	AutoCS a(CS_Config());
	return read(ITEM_SBC_RELOAD_FROM_FFR, t);
}
//
//
//============================================================================
int Config::getSbcToReloadFromSFR(CPID cpId) const
{
    return read(ITEM_SBC_RELOAD_FROM_SFR, cpId);
}
//
int Config::getSbcToReloadFromSFR(const CLUSTER& t) const
{
	AutoCS a(CS_Config());
	return read(ITEM_SBC_RELOAD_FROM_SFR, t);
}
//
//
//============================================================================
int Config::getCmdLogHandling(CPID cpId) const
{
    return read(ITEM_CMDLOG_HANDLING, cpId);
}

//
//
//============================================================================
void Config::supervisionTime(unsigned int val, CPID cpId)
{
    if(val >= MIN_SUPERVISION_TIME && val <= MAX_SUPERVISION_TIME)
    {
        write(ITEM_SUPERVISION_TIME, val, cpId);
    }
    else
    {
        THROW_XCODE("Supervision time out of bounds.", CodeException::ILLEGAL_VALUE);
    }
}

//
//
//============================================================================
void Config::sbcToReloadFromFFR(int val, CPID cpId)
{
    write(ITEM_SBC_RELOAD_FROM_FFR, val, cpId);
}

//
//
//============================================================================
void Config::sbcToReloadFromSFR(int val, CPID cpId)
{
    write(ITEM_SBC_RELOAD_FROM_SFR, val, cpId);
}
//
//
//============================================================================
void Config::cmdLogHandling(int val, CPID cpId)
{
    write(ITEM_CMDLOG_HANDLING, val ? MAN : AUTO, cpId);
}

//
//
//============================================================================
void Config::save(unsigned int sup, int ffr, int sfr, CPID cpid) const {

    // check supervision time
    validate(sup); // throws on errors

    // [Reload]
    write(ITEM_SUPERVISION_TIME, sup, cpid);
    write(ITEM_SBC_RELOAD_FROM_FFR, ffr, cpid);
    write(ITEM_SBC_RELOAD_FROM_SFR, sfr, cpid);

    // [Command Log]
    // Write(ITEM_CMDLOG_HANDLING, m_cmdLogHandling ? MAN : AUTO, cpid);
    write(ITEM_CMDLOG_HANDLING, MAN, cpid);
}
//
//
//============================================================================
void Config::save(unsigned int sup, int ffr, int sfr, const CLUSTER& t) const
{

	AutoCS a(CS_Config());
	// check supervision time
	validate(sup); // throws on errors

	// [Reload]
	write(ITEM_SUPERVISION_TIME, sup, t);
	write(ITEM_SBC_RELOAD_FROM_FFR, ffr, t);
	write(ITEM_SBC_RELOAD_FROM_SFR, sfr, t);

	// [Command Log]
//	write(ITEM_CMDLOG_HANDLING, m_cmdLogHandling ? MAN : AUTO, cpid);
	//eftersom vi inte har n��gon m��jlighet att via bupdef att s��tta cmdlog-hanteringen
	// s�� s��tter vi den default till manuel!!!
	write(ITEM_CMDLOG_HANDLING, MAN, t);
}

//
//
//============================================================================
void Config::save() const
{
	newTRACE((LOG_LEVEL_INFO, "Config::save()", 0));
	if (Config::instance().isVAPZ())
	{
		string filename(getCPSDataPath());
		filename.append(VCONFIG_FILE);

		if (!FileFunx::fileExists(filename.c_str()))
		{
			// [VNIC]
			if (!ParseINIFunx::WritePrivateProfile(ITEM_BACKUP_VSPEED.section, ITEM_BACKUP_VSPEED.name, ITEM_BACKUP_VSPEED.defVal,
					filename.c_str()))
			{
				EventReporter::instance().write("Config::write WritePrivateProfile failed.");
			}
		}
	}
}
//Config::LOGICAL_NAME_CPS_DATA+Config::CONF_ADD_ON+Config::BUSRV_ADD_ON+Config::VCONFIG_FILE

//
// values to stdout
//===========================================================================
void Config::dump() const
{
    CPS_Parameters cps;

    cout << "Application settings: " << endl
         << "  BUSRV port_____________________" << busrvPort() << endl
         << endl
         << "  CPS conf_______________________" << confBusrvDir() << endl
         << "  CPS data_______________________" << dataBusrvDir() << endl
         << "  CPF root_______________________" << cpfRoot() << endl
         << "  FTP dump root__________________" << cps.get(CPS_Parameters::FTP_DUMP_ROOT, "/CPSDUMP/") << endl
         << "  FTP load root__________________" << cps.get(CPS_Parameters::FTP_LOAD_ROOT, "/CPSLOAD/") << endl
         << "  Relvolumsw (dynamic func)______" << relvolumsw() << endl
         << endl
         << "  Supervison time________________" << getSupervisionTime() << endl
         << "  ReloadFrom FFR_________________" << getSbcToReloadFromFFR() << endl
         << "  ReloadFrom SFR_________________" << getSbcToReloadFromSFR() << endl
         << "  Cmdlog Handling______" << (getCmdLogHandling() ? "manual" : "automatic") << endl;
}
//
// THN: This is the original method, should be left untouched
// This is to handle SPXs in SSI, or BCs in non SSI during upgrade
//============================================================================
void Config::createDirStructure(CPID cpId)
{
    newTRACE((LOG_LEVEL_INFO, "Config::createDirStructure(%d)", 0, cpId));

    // Check if the system is a Multi or a One CP system and create the
    // directory structure for ether Multi including the CP level or
    // One system without CP level in the directory structure.
    // If cpId = 0xFFFF its a One CP system.
    if (Config::instance().isMultipleCP() && cpId == (CPID) 0xFFFF){
        // The System is faulty, CP = Multi CP system and APG = One CP system
        THROW_XCODE_LOG(" CP = Multi CP system and APG = One CP system: ", CodeException::CP_APG43_CONFIG_ERROR);
    }
    else if(!Config::instance().isMultipleCP() && cpId != (CPID) 0xFFFF){
        // The System is faulty, CP = One CP system and APG = Multi CP system
        THROW_XCODE_LOG(" CP = One CP system and APG = Multi CP system: ", CodeException::CP_APG43_CONFIG_ERROR);
    }

    // create /data/cps/data/<cp>/busrv/ftp/
    //-------------------------------------
    //string ftpDir = tmpRoot(cpId);

    if (FileFunx::createDirectories(tmpRoot(cpId)) == false)
    {
        TRACE((LOG_LEVEL_ERROR, "createDirStructure(%d): Error creating ftp path %d.", 0, cpId, errno));
        THROW_XCODE("Error creating ftp path.", CodeException::INTERNAL_ERROR);
    }
    //FileFunx::chown(ftpDir.c_str());

	// --------------------------------------------
    // TODO: Set compression attribute to folder??
	// --------------------------------------------

    // create /data/cps/data/<cp>/busrv/conf
    //--------------------------------------
    if (FileFunx::createDirectories(confBusrvDir(cpId)) == false)
    {
        TRACE((LOG_LEVEL_ERROR, "createDirStructure(%d): Error creating conf path %d.", 0, cpId, errno));
        THROW_XCODE("Error creating conf path.", CodeException::INTERNAL_ERROR);
    }

	// --------------------------------------------
    // TODO: Set compression attribute to folder??
	// --------------------------------------------
}
//
// THN: This is the original method, should be left untouched
// This is to handle SPXs in SSI, or BCs in non SSI during upgrade
//============================================================================
void Config::createDirStructure(const CLUSTER& t)
{
    newTRACE((LOG_LEVEL_INFO, "Config::createDirStructure(CLUSTER)", 0));
    
    // create /data/cps/data/CLUSTER/<BCx>/busrv/ftp/
    //-----------------------------------------------
	// This path depends on BCx name
	//

	// Looping through all BCs to create Ftp directory structure
	ACS_CS_API_IdList cpList;
	ACS_CS_API_CP * CP = ACS_CS_API::createCPInstance();

	ACS_CS_API_NS::CS_API_Result res = CP->getCPList(cpList);

	if (res != ACS_CS_API_NS::Result_Success) {
		ACS_CS_API::deleteCPInstance(CP);
		throw CodeException::INTERNAL_ERROR;
	}

	//cout << "CreateDirStructure(CLUSTER), Config.cpp" << endl;
	for (u_int32 i = 0; i < cpList.size(); i++) {

		CPID cpId = cpList[i];
		if (cpId > 63)
			continue;

		Config::CLUSTER t(cpId, true);
		//cout << "Creating FtpStructure: " 
		//	 << Config::instance().tmpRoot(t) << endl;;

		createFtpDirStructure(Config::instance().tmpRoot(t).c_str());

	}
	ACS_CS_API::deleteCPInstance(CP);

	// create /data/cps/data/CLUSTER/busrv/
    //-----------------------------------------------
	// This path does not depend on BCx name
	//
	createDataBusrvDirStructure(t);
	   // create /data/cps/data/CLUSTER/busrv/conf
	    //--------------------------------------
	    if (FileFunx::createDirectories(confBusrvDir(t)) == false)
	    {
	        TRACE((LOG_LEVEL_ERROR, "createDirStructure(): Error creating conf path %d.", 0, errno));
	        THROW_XCODE("Error creating conf path.", CodeException::INTERNAL_ERROR);
	    }
}   
//
//
// create /data/cps/data/CLUSTER/<BCx>/busrv/
//============================================================================
void Config::createDataBusrvDirStructure(const CLUSTER& t)
{
    newTRACE((LOG_LEVEL_INFO, "Config::createDataBusrvDirStructure(const CLUSTER&)", 0));
    
	// dataBusrvDir(t).c_str()
    if (FileFunx::createDirectories(dataBusrvDir(t)) == false)
    {
        TRACE((LOG_LEVEL_ERROR, "createDataBusrvDirStructure(const CLUSTER&): Error creating conf path %d.", 0, errno));
        THROW_XCODE("Error creating conf path.", CodeException::INTERNAL_ERROR);
    }
}
//
// Note that this create Ftp directory structure specifically
//============================================================================
void Config::createFtpDirStructure(const char *name)
{
    newTRACE((LOG_LEVEL_INFO, "Config::createFtpDirStructure(%s)", 0, name));

    if (FileFunx::createDirectories(name) == false)
    {
        TRACE((LOG_LEVEL_ERROR, "createFtpDirStructure(%s): Error creating ftp path %d.", 0, name, errno));
        THROW_XCODE("Error creating ftp path.", CodeException::INTERNAL_ERROR);
    }
    //FileFunx::chown(name);
}
//
//
// Return the path for symbolic name listed in DataDiskPath.dat
//===========================================================================
string Config::getPath(const char* logicalName, CPID cpId) const
{
    newTRACE((LOG_LEVEL_INFO, "Config::getPath(SYMBOLIC_NAME = %s, CPID = %d)", 0, logicalName, cpId));
    
    int result = ACS_APGCC_STRING_BUFFER_SMALL;
    int len = 48;
    boost::scoped_array<char> path;
            
	ACS_APGCC_CommonLib acs;
	bool isMultipleCP = Config::instance().isMultipleCP();
	char *dest = 0;
	
    // Try getting the datadisk path requested until we get the buffer size right    
    while (ACS_APGCC_STRING_BUFFER_SMALL == result) {
    	len += 10;	// Extra spaces, for cpname if needed
        path.reset(dest = new (nothrow) char[len*sizeof(char)]);
        
        if (NULL != dest) {       
        	
            if (isMultipleCP && cpId != 0xFFFF) {
            	// CS nolonger supports GetDataDiskPathForCp()
                //result = acs.GetDataDiskPathForCp(logicalName, cpId, path.get(), len);
                //TRACE(("ACS GetDataDiskPath() returns result = %d; path = %s", 0, result, path.get()));
                
                result = acs.GetDataDiskPath(logicalName, dest, len);
                TRACE((LOG_LEVEL_INFO, "ACS GetDataDiskPath() returns result = %d; path = %s", 0, result, dest));
            }
            else if (!isMultipleCP && cpId == 0xFFFF){            
                result = acs.GetDataDiskPath(logicalName, dest, len);
                TRACE((LOG_LEVEL_INFO, "ACS GetDataDiskPath() returns result = %d; path = %s", 0, result, dest));
            }
            else  if (isMultipleCP && cpId == 0xFFFF) {
                // The System is faulty, CP = Multi CP system and APG = One CP system
                THROW_XCODE_LOG(" CP = Multi CP system and APG = One CP system: ", CodeException::CP_APG43_CONFIG_ERROR);
            }
            else { //if (!isMultipleCP && cpId != 0xFFFF) 
                // The System is faulty, CP = One CP system and APG = Multi CP system
                THROW_XCODE_LOG(" CP = One CP system and APG = Multi CP system: ", CodeException::CP_APG43_CONFIG_ERROR);
            }
        }
        else {
        	TRACE((LOG_LEVEL_WARN, "Config::getPath() returns empty string; memory allocation fails", 0));
            return string("");
        }
    }

    if (result != ACS_APGCC_DNFPATH_SUCCESS) {
    	TRACE((LOG_LEVEL_WARN, "ACS GetDataDiskPath() returns result = %d; Config::getPath() returns empty string", 0, result));
        return string("");
    }

	// This is meant to be a low level char* manipulation
    if (cpId != 0xFFFF) {
    	char name[8];
		if (BUPFunx::convertCPIdtoCPName(cpId, name))
			return string("");
		dest[len] = '/';
		strcpy(&dest[++len], name);
    }
       
    TRACE((LOG_LEVEL_INFO, "Config::getPath() returns = %s", 0, dest));
    return string(dest); 
}
//
//
// Return the path for symbolic name listed in DataDiskPath.dat
//===========================================================================
string Config::getPath(const char* logicalName, const CLUSTER&) const
{
    newTRACE((LOG_LEVEL_INFO, "Config::getPath(SYMBOLIC_NAME = %s, const CLUSTER&)", 0, logicalName));
    
    int result = ACS_APGCC_STRING_BUFFER_SMALL;
    int len = 48;
    boost::scoped_array<char> path;
            
	ACS_APGCC_CommonLib acs;
	char *dest = 0;
	
    // Try getting the datadisk path requested until we get the buffer size right    
    while (ACS_APGCC_STRING_BUFFER_SMALL == result) {
    	len += 10;	// Extra spaces, for cpname if needed
        path.reset(dest = new (nothrow) char[len*sizeof(char)]);
        
        if (NULL != dest) {               	
            result = acs.GetDataDiskPath(logicalName, dest, len);
            TRACE((LOG_LEVEL_INFO, "ACS GetDataDiskPath() returns result = %d; path = %s", 0, result, dest));
        }
        else {
        	TRACE((LOG_LEVEL_WARN, "Config::getPath() returns empty string; memory allocation fails", 0));
            return string("");
        }
    }

    if (result != ACS_APGCC_DNFPATH_SUCCESS) {
    	TRACE((LOG_LEVEL_WARN, "ACS GetDataDiskPath() returns result = %d; Config::getPath() returns empty string", 0, result));
        return string("");
    }
       
    TRACE((LOG_LEVEL_INFO, "Config::getPath() returns = %s", 0, dest));
    return string(dest); 
}


//
//
//===========================================================================
string Config::getCPSPath(CPID cpId) const
{
    newTRACE((LOG_LEVEL_INFO, "Config::getCPSPath(CPID = %d)", 0, cpId));

    static bool contactAPGCC[] = {false, false, false};
    static string paths[] = {"", "", ""};

    string res = "";    
    int idx = 0;
    switch (cpId) {
    case 1001:
    case 1002:
        idx = cpId - 1000;
        break;
    default:
        idx = 0;
    }
    
    if (!contactAPGCC[idx])
    {
        res = getPath(LOGICAL_NAME_CPS_DATA, cpId);
        if (!res.empty()) {
            contactAPGCC[idx] = true;
            paths[idx] = res;
        }
    }

    TRACE((LOG_LEVEL_INFO, "Config::getCPSPath() returns = %s", 0, paths[idx].c_str()));
    return paths[idx];

}

//
//
//===========================================================================
string Config::getCPSPath(const CLUSTER& t) const
{
    newTRACE((LOG_LEVEL_INFO, "Config::getCPSPath(const CLUSTER&)", 0));

    static bool contactAPGCC = false;
    static string path = "";

    if (!contactAPGCC)
    {
        path = getPath(LOGICAL_NAME_CPS_DATA, t);
        if (!path.empty()) {
            contactAPGCC = true;
        }
    }

    TRACE((LOG_LEVEL_INFO, "Config::getCPSPath() returns = %s", 0, path.c_str()));
    return path;
}
//
//
//============================================================================
string Config::getCPSDataPath() const
{
	int result = ACS_APGCC_STRING_BUFFER_SMALL;
	int len = 48;
	char *dest = dest = new (nothrow) char[len*sizeof(char)];
	ACS_APGCC_CommonLib acs;

	newTRACE((LOG_LEVEL_INFO, "Config::getCPSDataPath()", 0));
	result = acs.GetDataDiskPath(LOGICAL_NAME_CPS_DATA, dest, len);
	TRACE((LOG_LEVEL_INFO, "ACS GetDataDiskPath() returns result = %d; path = %s", 0, result, dest));

	if (result != ACS_APGCC_DNFPATH_SUCCESS)
	{
		TRACE((LOG_LEVEL_WARN, "ACS GetDataDiskPath() returns result = %d; Config::save() using hardcoded string /data/apz/cps/data/", 0, result));
		return string("/data/apz/cps/data/");
	}
	else
	{
		return string(dest) + "/";
	}
}
//
//
//===========================================================================
string Config::getFMSPath(CPID cpId) const
{
    newTRACE((LOG_LEVEL_INFO, "Config::getFMSPath(CPID = %d)", 0, cpId));

    static bool contactAPGCC[] = {false, false, false};
    static string paths[] = {"", "", ""};

    string res = "";    
    int idx = 0;
    switch (cpId) {
    case 1001:
    case 1002:
        idx = cpId - 1000;
        break;
    default:
        idx = 0;
    }
    
    if (!contactAPGCC[idx])
    {
        res = getPath(LOGICAL_NAME_FMS_DATA, cpId);
        if (!res.empty()) {
            contactAPGCC[idx] = true;
            paths[idx] = res;
        }
    }

    TRACE((LOG_LEVEL_INFO, "Config::getFMSPath() returns = %s", 0, paths[idx].c_str()));
    return paths[idx];
}


//
//
//===========================================================================
string Config::getFMSPath(const CLUSTER& t) const
{
    newTRACE((LOG_LEVEL_INFO, "Config::getFMSPath(const CLUSTER&)", 0));

    static bool isContactAPGCC = false;
    static string dirPath = "";

    if (!isContactAPGCC)
    {
        dirPath = getPath(LOGICAL_NAME_FMS_DATA, t);
        if (!dirPath.empty()) {
            isContactAPGCC = true;
        }
    }

    TRACE((LOG_LEVEL_INFO, "Config::getFMSPath() returns = %s", 0, dirPath.c_str()));
    return dirPath;
}

//
//
//===========================================================================
u_int32 Config::read(const ItemDW& item, CPID cpId) const
{
	newTRACE((LOG_LEVEL_INFO, "Config::read()", 0));
	TRACE((LOG_LEVEL_INFO, "conf file name is %s", 0, confFile(cpId).c_str()));
    return ParseINIFunx::GetPrivateProfileInt(item.section, item.name, item.defVal, confFile(cpId).c_str());
}

//
//
//===========================================================================
string Config::read(const ItemSZ& item, CPID cpId) const
{
    char buf[PATH_MAX + 1];
    ParseINIFunx::GetPrivateProfileString(item.section, item.name, item.defVal, buf, PATH_MAX, confFile(cpId).c_str());
    return string(buf);
}
//
//
//===========================================================================
u_int32 Config::read(const ItemDW& item, const CLUSTER& t) const {
	return ParseINIFunx::GetPrivateProfileInt(item.section, item.name, item.defVal, confFile(t).c_str());
}
//
//
//===========================================================================
string Config::read(const ItemSZ& item, const CLUSTER& t) const {
	char buf[255 + 1];
	ParseINIFunx::GetPrivateProfileString(item.section, item.name, item.defVal, buf, 255, confFile(t).c_str());
	return buf;
}

//
// throws
//===========================================================================
void Config::validate(int supervisionTime) const
{

    newTRACE((LOG_LEVEL_INFO, "Config::validate(int supervisionTime = %d)", 0, supervisionTime));
    
    // This is for BT; Lower the reload supervision time
    CPS_Parameters cps;
    int minSupTime = cps.get(CPS_Parameters::MIN_RELOAD_SUPERVISION_TIME, MIN_SUPERVISION_TIME);

    if(supervisionTime < minSupTime || supervisionTime > MAX_SUPERVISION_TIME)
    {
        TRACE((LOG_LEVEL_ERROR, "Config::validate(int supervisionTime) fails", 0));
        THROW_XCODE(ITEM_SUPERVISION_TIME.name << " (" <<
        supervisionTime << ") is too " <<
        (supervisionTime < MIN_SUPERVISION_TIME ? "small." : "large."),
        CodeException::CONFIGURATION_ERROR);
    }
}
//
////TODO: To be removed
//===========================================================================
void Config::crashing(int at, CPID cpId) {
    newTRACE((LOG_LEVEL_INFO, "****** DumpEndJob; Auto dump failure simulation; crashing(%d) *******", 0, at));

    int val = ParseINIFunx::GetPrivateProfileInt("Simulated Auto Dump Failure", "Failure at", 0, Config::instance().confFile(cpId).c_str());

    if (at && at == val) {
        TRACE((LOG_LEVEL_INFO, "Auto dump failure simulation throwing exception at value = %d", 0, at));
        throw 1;
    }
    TRACE((LOG_LEVEL_INFO, "Auto dump failure simulation; no simulation action taken", 0));
}

void Config::crashing(int at, const CLUSTER& t) {
    newTRACE((LOG_LEVEL_INFO, "****** DumpEndJob; Auto dump failure simulation; crashing(%d) *******", 0, at));

    int val = ParseINIFunx::GetPrivateProfileInt("Simulated Auto Dump Failure", "Failure at", 0, Config::instance().confFile(t).c_str());

    if (at && at == val) {
        TRACE((LOG_LEVEL_INFO, "Auto dump failure simulation throwing exception at value = %d", 0, at));
        throw 1;
    }
    TRACE((LOG_LEVEL_INFO, "Auto dump failure simulation; no simulation action taken", 0));
}

#ifdef _APGUSERSVC_
//
//
//===========================================================================
void Config::setCPSUserId(const unsigned int cpsUserId) {
    s_cpsUserId = cpsUserId;
}

#endif

