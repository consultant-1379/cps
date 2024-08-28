/*
NAME
	File_name:Config.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

The Copyright to the computer program(s) herein is the property of Ericsson
Utvecklings AB, Sweden.
The program(s) may be used and/or copied only with the written permission from
Ericsson Utvecklings AB or in accordance with the terms and conditions
stipulated in the agreement/contract under which the program(s) have been
supplied.

DESCRIPTION
	FCSRV configuration data.

DOCUMENT NO
	190 89-CAA 109 0670

AUTHOR
	2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on Config.cpp of FCSRV in Windows

SEE ALSO
	-

Revision history
2011-11-15 xtuudoo Create based on Config.hpp of BUSRV
2012-08-28 xngudan Update for APG43L project

*/

#include "CPS_FCSRV_Trace.h"
#include "PlatformTypes.h"
#include "ACS_CS_API.h"
#include "ACS_DSD_Server.h"
#include "CPS_Parameters.h"
#include "CPS_Utils.h"
#include "LinuxException.h"
#include "EventReporter.h"
#include "FCPFunx.h"
#include "Config.h"
#include "FileFunx.h"
#include "CPS_Utils.h"
#include "CodeException.h"
#include "CriticalSection.h"

#include <stdio.h>
#include <set>
#include <netdb.h>
#include <sys/stat.h>
#include "boost/scoped_array.hpp"

#include "ACS_APGCC_CommonLib.h"


// misc. constants
//
const char* Config::FCSRV_ADD_ON = "/fcsrv/";
const char* Config::TMP_ADD_ON = "ftp/";
const char* Config::DATFILE_ADD_ON = "fcsrv.dat";
const char* Config::LOGICAL_NAME_CPS_DATA = "CPS_DATA";
const char* Config::CONF_ADD_ON = "conf/";
const char* Config::CLUSTER_ADD_ON = "/CLUSTER";
//
// singleton staticsconst char* Config::CLUSTER_ADD_ON = "/CLUSTER";
//
Config Config::s_instance;
bool Config::s_initialized = false;
//
// ctor (private)
//===========================================================================
Config::Config() { }
//
//
//===========================================================================
void Config::init() {


	//newTRACE((LOG_LEVEL_INFO, "Config::init()", 0));

	m_simAp = UNDEF;

	s_initialized = true;
}
//
// Return the path for symbolic name listed in GetDataDiskPath.dat
//===========================================================================
string Config::getPath(const char* logicalName, CPID cpId) const {
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
			if (FCPFunx::convertCPIdtoCPName(cpId, name))
				return string("");
			dest[len] = '/';
			strcpy(&dest[++len], name);
	    }

	    TRACE((LOG_LEVEL_INFO, "Config::getPath() returns = %s", 0, dest));
	    return string(dest);
}

//
// Return the path for symbolic name listed in GetDataDiskPath.dat
//===========================================================================
string Config::getPath(const char* logicalName, const CLUSTER&) const {
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


CriticalSection& Config::CS_Config() const {
	// This is to ensure that s_cs is constructed exactly one on first use
	static CriticalSection s_cs;
	return s_cs;
}

//
// Return FTP load path /data/cps/data/<cp>/fcsrv/ftp/
//===========================================================================
string Config::ftpReadDir(CPID cpId) const {

	newTRACE((LOG_LEVEL_INFO, "Config::ftpReadDir(CPID = %d)", 0, cpId));

	CPS_Parameters cps;
	string ftpReadDir = cps.get(CPS_Parameters::FTP_DUMP_ROOT, "/CPSDUMP/");

	if (Config::instance().isMultipleCP() && cpId != (CPID) 0xFFFF){
		// get default name for CP with cpId
		char name[10];
		name[0] = '/';
		if (int res = FCPFunx::convertCPIdtoCPName(cpId, &name[1])){
			TRACE((LOG_LEVEL_ERROR, "Convert cpid %d to cp name failed. err: %d", 0, cpId, res));
			THROW_XCODE_LOG("Convert CPId to CP name failed: %d", CodeException::CP_APG43_CONFIG_ERROR);
		}

		ftpReadDir.append(name);
	}
	else if (!Config::instance().isMultipleCP() && cpId == (CPID) 0xFFFF){
		// ONE CP System
		TRACE((LOG_LEVEL_WARN, "ONE CP System", 0));
	}
	else if (Config::instance().isMultipleCP() && cpId == (CPID) 0xFFFF) {
		// The System is faulty, CP = Multi CP system and APG = One CP system
		THROW_XCODE_LOG(" CP = Multi CP system and APG = One CP system: ", CodeException::CP_APG43_CONFIG_ERROR);
	}
	else if (!Config::instance().isMultipleCP() && cpId != (CPID) 0xFFFF) {
		// The System is faulty, CP = One CP system and APG = Multi CP system
		THROW_XCODE_LOG(" CP = One CP system and APG = Multi CP system: ", CodeException::CP_APG43_CONFIG_ERROR);
	}
	else	// The System is faulty, this is not a valid option
		THROW_XCODE_LOG(" The CP and APG is faulty: ", CodeException::CP_APG43_CONFIG_ERROR);


	ftpReadDir.append(FCSRV_ADD_ON);
	ftpReadDir.append(TMP_ADD_ON);
	TRACE((LOG_LEVEL_INFO, "ftpReadDir = %s", 0, ftpReadDir.c_str()));

	return ftpReadDir;
}

//
// Return FTP load path /data/cps/data/cluster/fcsrv/ftp/
//===========================================================================
string Config::ftpReadDir(const CLUSTER& t) const {

	newTRACE((LOG_LEVEL_INFO, "Config::ftpReadDir(CLUSTER)", 0));

	string ftpReadDir = getPath(LOGICAL_NAME_CPS_DATA, t);
	ftpReadDir.append(CLUSTER_ADD_ON);
	ftpReadDir.append(FCSRV_ADD_ON);
	ftpReadDir.append(TMP_ADD_ON);
	TRACE((LOG_LEVEL_INFO, "ftpReadDir = %s", 0, ftpReadDir.c_str()));

	return ftpReadDir;
}

//
// create FTP dump path /data/cps/data/<cp>/fcsrv/ftp/
//===========================================================================
string Config::ftpWriteDir(CPID cpId) const {
	newTRACE((LOG_LEVEL_INFO, "Config::ftpWriteDir(CPID %d)", 0, cpId));

        CPS_Parameters cps;
        string ftpWriteDir = cps.get(CPS_Parameters::FTP_DUMP_ROOT, "/CPSDUMP/");

	if (Config::instance().isMultipleCP() && cpId != (CPID) 0xFFFF){
		// get default name for CP with cpId
		char name[10];
	        name[0] = '/';	
		if (int res = FCPFunx::convertCPIdtoCPName(cpId, &name[1])){
			TRACE((LOG_LEVEL_ERROR, "Convert cpid %d to cp name failed. err: %d", 0, cpId, res));
			THROW_XCODE_LOG("Convert CPId to CP name failed: %d", CodeException::CP_APG43_CONFIG_ERROR);
		}
		ftpWriteDir.append(name);
	}
	else if (!Config::instance().isMultipleCP() && cpId == (CPID) 0xFFFF){
		// ONE CP System
		TRACE((LOG_LEVEL_WARN, "ONE CP System", 0));
	}
	else if (Config::instance().isMultipleCP() && cpId == (CPID) 0xFFFF) {
		// The System is faulty, CP = Multi CP system and APG = One CP system
		THROW_XCODE_LOG(" CP = Multi CP system and APG = One CP system: ", CodeException::CP_APG43_CONFIG_ERROR);
	}
	else if (!Config::instance().isMultipleCP() && cpId != (CPID) 0xFFFF) {
		// The System is faulty, CP = One CP system and APG = Multi CP system
		THROW_XCODE_LOG(" CP = One CP system and APG = Multi CP system: ", CodeException::CP_APG43_CONFIG_ERROR);
	}
	else	// The System is faulty, this is not a valid option
		THROW_XCODE_LOG(" The CP and APG is faulty: ", CodeException::CP_APG43_CONFIG_ERROR);

	ftpWriteDir.append(FCSRV_ADD_ON);
	ftpWriteDir.append(TMP_ADD_ON);
        TRACE((LOG_LEVEL_INFO, "ftpWriteDir = %s", 0, ftpWriteDir.c_str()));

	return ftpWriteDir;
}

//
// create FTP dump path /data/cps/data/<cp>/fcsrv/ftp/
//===========================================================================
string Config::ftpWriteDir(const CLUSTER& t) const {
	newTRACE((LOG_LEVEL_INFO, "Config::ftpWriteDir(CLUSTER)", 0));

	string ftpWriteDir = getPath(LOGICAL_NAME_CPS_DATA, t);
	ftpWriteDir.append(FCSRV_ADD_ON);
	ftpWriteDir.append(TMP_ADD_ON);
	return ftpWriteDir;
}

//
// createn ftpDir path /data/cps/data/<cp>/fcsrv/ftp/
//===========================================================================
string Config::ftpDir(CPID cpId) const {
	newTRACE((LOG_LEVEL_INFO, "Config::ftpDir(CPID = %d)", 0, cpId));
	string ftpPath = "";
	ftpPath = getPath(LOGICAL_NAME_CPS_DATA, cpId);
	ftpPath.append(FCSRV_ADD_ON);
	ftpPath.append(TMP_ADD_ON);
	TRACE((LOG_LEVEL_INFO, "ftpDir = %s", 0, ftpPath.c_str()));

	return ftpPath;
}

//
// createn ftpDir path /data/cps/data/cluster/fcsrv/ftp/
//===========================================================================
string Config::ftpDir(const CLUSTER& t) const {
	newTRACE((LOG_LEVEL_INFO, "Config::ftpDir(CLUSTER)", 0));
	string ftpPath = "";
	ftpPath = getPath(LOGICAL_NAME_CPS_DATA, t);
	ftpPath.append(CLUSTER_ADD_ON);
	ftpPath.append(FCSRV_ADD_ON);
	ftpPath.append(TMP_ADD_ON);
	TRACE((LOG_LEVEL_INFO, "ftpDir = %s", 0, ftpPath.c_str()));

	return ftpPath;
}

//
//
//===========================================================================
bool Config::isMultipleCP() const {
	newTRACE((LOG_LEVEL_INFO, "Config::isMultipleCP()", 0));

	// Use static variable in order to make sure this function is only called one time until
	// the main program exits
	static bool multCPSys = false;
	static bool isCalled = false;

	if (!isCalled) {
		ACS_CS_API_NS::CS_API_Result res = ACS_CS_API_NetworkElement::isMultipleCPSystem(multCPSys);
		if (res != ACS_CS_API_NS::Result_Success){
			TRACE((LOG_LEVEL_ERROR, "Config::isMultipleCP: failed to get info is multiple CP System, error %d", 0, res));
			THROW_XCODE_LOG("Failed to get system configuration: ", CodeException::CP_APG43_CONFIG_ERROR);
		}
		isCalled = true;
	}

	return multCPSys;
}


#if 0
//
// Get the listening IP Addresses
//===========================================================================
bool Config::getIPListeningAddr(vector<string> &ip) {
	newTRACE(("Config::getIPListeningAddr()", 0));

    bool retCode = false;
    ACS_DSD_Server s(acs_dsd::SERVICE_MODE_INET_SOCKET_PRIVATE);
    ACS_DSD_Node my_node;
    s.get_local_node(my_node);

    string apName(my_node.node_name);

    // convert to capital letters.
    CPS::toUpper(apName);

    for (int apnr = 0; apnr < NO_OF_AP; apnr++)
    {
         for (int apside = 0; apside < 2; apside++)
         {
             if (APNAMES[apnr][apside].compare(apName)==0)
             {
                 ip.push_back(APIP[apnr][apside][0]);
                 ip.push_back(APIP[apnr][apside][1]);
                 retCode=true;
                 break;
             }
         }

         if (retCode == true)
             break;
    }

    return retCode;
}

#endif

//
// Get fcsrv port number from parameter file
//===========================================================================
int Config::fcsrvPort() const {
	//newTRACE((LOG_LEVEL_INFO, "Config::fcsrvPort()", 0));

	struct servent *serv = NULL;
	const int DEF_PORT = 14016;

	serv = getservbyname("fcsrv", "tcp");
	if (serv == NULL) {
		//TRACE((LOG_LEVEL_WARN, "Failed to obtain the fcsrv port. User default: %d", 0, DEF_PORT));
		return DEF_PORT;
	}

	return ntohs(serv->s_port);
}
//
//
//============================================================================
void Config::createDirStructure(CPID cpId)
{
    newTRACE((LOG_LEVEL_INFO, "Config::createDirStructure(CPID = %d)", 0, cpId));

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

    // create /data/cps/data/<cp>/fcsrv/ftp/
    //-------------------------------------
    //bool result = FileFunx::createDirectories(tmpRoot(cpId), 755);
    string tmpDir = ftpDir(cpId);
    bool result = FileFunx::createDirectories(tmpDir);

    if (result == false)
    {
        TRACE((LOG_LEVEL_ERROR, "createDirStructure(%d): Error creating ftp path %d.", 0, cpId, errno));
        THROW_XCODE("Error creating ftp path.", CodeException::INTERNAL_ERROR);
    }

    if (::chmod(tmpDir.c_str(), 0777) != 0)
    {
        TRACE((LOG_LEVEL_ERROR, "createDirStructure(%d): Error changing permssion of ftp path %d.", 0, cpId, errno));
        THROW_XCODE("Error changing permission of ftp path.", CodeException::INTERNAL_ERROR);
    }

	// --------------------------------------------
    // TODO: Set compression attribute to folder??
	// --------------------------------------------

    // create /data/cps/data/<cp>/fcsrv/conf
    //-------------------------------------
    result = FileFunx::createDirectories(confFcsrvDir(cpId));

    if (result == false)
    {
        TRACE((LOG_LEVEL_ERROR, "createDirStructure(%d): Error creating conf path %d.", 0, cpId, errno));
        THROW_XCODE("Error creating conf path.", CodeException::INTERNAL_ERROR);
    }

	// --------------------------------------------
    // TODO: Set compression attribute to folder??
	// --------------------------------------------
}

//
//
//============================================================================
void Config::createDirStructure(const CLUSTER& t)
{
    newTRACE((LOG_LEVEL_INFO, "Config::createDirStructure(const CLUSTER&)", 0));

    // create /data/cps/data/CLUSTER/fcsrv/ftp/
    //-------------------------------------
    string tmpDir = ftpDir(t);

    bool result = FileFunx::createDirectories(tmpDir);

    if (result == false)
    {
        TRACE((LOG_LEVEL_ERROR, "createDirStructure(CLUSTER): Error creating ftp path %d.", 0, errno));
        THROW_XCODE("Error creating ftp path.", CodeException::INTERNAL_ERROR);
    }

    if (::chmod(tmpDir.c_str(), 0777) != 0)
    {
        TRACE((LOG_LEVEL_ERROR, "createDirStructure(CLUSTER): Error changing permssion of ftp path %d.", 0, errno));
        THROW_XCODE("Error changing permission of ftp path.", CodeException::INTERNAL_ERROR);
    }

	// --------------------------------------------
    // TODO: Set compression attribute to folder??
	// --------------------------------------------

    // create /data/cps/data/CLUSTER/fcsrv/conf
    //-------------------------------------
    result = FileFunx::createDirectories(confFcsrvDir(t));

    if (result == false)
    {
        TRACE((LOG_LEVEL_ERROR, "createDirStructure(CLUSTER): Error creating conf path %d.", 0, errno));
        THROW_XCODE("Error creating conf path.", CodeException::INTERNAL_ERROR);
    }

	// --------------------------------------------
    // TODO: Set compression attribute to folder??
	// --------------------------------------------
}

string Config::tmpRoot(CPID cpId) const
{
    newTRACE((LOG_LEVEL_INFO, "Config::tmpRoot(CPID = %d)", 0, cpId));

    string tmpPath = getPath(LOGICAL_NAME_CPS_DATA, cpId);
    tmpPath.append(FCSRV_ADD_ON);
    tmpPath.append(TMP_ADD_ON);
    TRACE((LOG_LEVEL_INFO, "tmpRoot = %s", 0, tmpPath.c_str()));

    return tmpPath;
}

//
// Return /data/cps/data/<cp>/fcsrv/conf/
//==============================================================
string Config::confFcsrvDir(CPID cpId) const
{
    newTRACE((LOG_LEVEL_INFO, "Config::confFcsrvDir(CPID = %d)", 0, cpId));

    string tmpPath = getPath(LOGICAL_NAME_CPS_DATA, cpId);
    tmpPath.append(FCSRV_ADD_ON);
    tmpPath.append(CONF_ADD_ON);
    TRACE((LOG_LEVEL_INFO, "confFcsrvDir = %s", 0, tmpPath.c_str()));

    return tmpPath;
}


//
// Return /data/cps/data/cluster/fcsrv/conf/
//==============================================================
string Config::confFcsrvDir(const CLUSTER& t) const
{
    newTRACE((LOG_LEVEL_INFO, "Config::confFcsrvDir(CLUSTER)", 0));

    string tmpPath = getPath(LOGICAL_NAME_CPS_DATA, t);
    tmpPath.append(CLUSTER_ADD_ON);
    tmpPath.append(FCSRV_ADD_ON);
    tmpPath.append(CONF_ADD_ON);
    TRACE((LOG_LEVEL_INFO, "confFcsrvDir = %s", 0, tmpPath.c_str()));

    return tmpPath;
}
