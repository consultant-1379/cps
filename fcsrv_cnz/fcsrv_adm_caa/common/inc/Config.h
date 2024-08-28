
#ifndef _CONFIG_H_
#define _CONFIG_H_
/*
NAME
	CONFIG -
LIBRARY 3C++
PAGENAME CONFIG
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE Config.h

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION
	FCSRV configuration data.
	The class uses eager evalutation (values are read as soon
	as the class is used).
	Implemented as a singleton (use the instance() method to get
	a const reference to the class instance.

ERROR HANDLING
	-

DOCUMENT NO
	190 89-CAA 109 0670

AUTHOR
	2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on Config.h of Windows version

LINKAGE
	-

SEE ALSO
	-

Revision history
----------------
2011-11-15 xtuudoo Created
2012-09-11 xngudan Updated for CLUSTER session
*/
#pragma once

#include "fcap.h"
#include "PlatformTypes.h"
#include "EventReporter.h"
#include "ACS_CS_API.h"

#include <assert.h>
#include <string>
#include <vector>

class CriticalSection;

using namespace fcap;
using namespace std;

#if 0
const int NO_OF_AP = 16;
const std::string APNAMES[NO_OF_AP][2] =
{
    {"AP1A","AP1B"},
    {"AP2A","AP2B"},
    {"AP3A","AP3B"},
    {"AP4A","AP4B"},
    {"AP5A","AP5B"},
    {"AP6A","AP6B"},
    {"AP7A","AP7B"},
    {"AP8A","AP8B"},
    {"AP9A","AP9B"},
    {"AP10A","AP10B"},
    {"AP11A","AP11B"},
    {"AP12A","AP12B"},
    {"AP13A","AP13B"},
    {"AP14A","AP14B"},
    {"AP15A","AP15B"},
    {"AP16A","AP16B"}
};

// The AP IP-addresses are fixed according to the tables below.
const std::string APIP[NO_OF_AP][2][2] =
{
    {{"192.168.169.1","192.168.170.1"},{"192.168.169.2","192.168.170.2"}},
    {{"192.168.169.3","192.168.170.3"},{"192.168.169.4","192.168.170.4"}},
    {{"192.168.169.5","192.168.170.5"},{"192.168.169.6","192.168.170.6"}},
    {{"192.168.169.7","192.168.170.7"},{"192.168.169.8","192.168.170.8"}},
    {{"192.168.169.9","192.168.170.9"},{"192.168.169.10","192.168.170.10"}},
    {{"192.168.169.11","192.168.170.11"},{"192.168.169.12","192.168.170.12"}},
    {{"192.168.169.13","192.168.170.13"},{"192.168.169.14","192.168.170.14"}},
    {{"192.168.169.15","192.168.170.15"},{"192.168.169.16","192.168.170.16"}},
    {{"192.168.169.17","192.168.170.17"},{"192.168.169.18","192.168.170.18"}},
    {{"192.168.169.19","192.168.170.19"},{"192.168.169.20","192.168.170.20"}},
    {{"192.168.169.21","192.168.170.21"},{"192.168.169.22","192.168.170.22"}},
    {{"192.168.169.23","192.168.170.23"},{"192.168.169.24","192.168.170.24"}},
    {{"192.168.169.25","192.168.170.25"},{"192.168.169.26","192.168.170.26"}},
    {{"192.168.169.27","192.168.170.27"},{"192.168.169.28","192.168.170.28"}},
    {{"192.168.169.29","192.168.170.29"},{"192.168.169.30","192.168.170.30"}},
    {{"192.168.169.31","192.168.170.31"},{"192.168.169.32","192.168.170.32"}}
};

#endif

class Config {
public:
	//
	// keys, sections, items & such
	//
	// ini file sections
	static const char* SECT_DEBUG;
	//
	// add-ons & common constants
	//
	static const char* TMP_ADD_ON;			// add to CPS data
	static const char* DATFILE_ADD_ON;		// add to cps data
	static const char* FCSRV_ADD_ON;		// add to CPS data
	static const char* LOGICAL_NAME_CPS_DATA;
	static const char* CONF_ADD_ON;
	static const char* CLUSTER_ADD_ON;

	enum {
		UNDEF = -1,
		SIMAP_ALL_IP_ADDRESS = 0,
		APG_ONE_IP_ADDRESS = 0,
		APG_TWO_IP_ADDRESS = 1
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
	int fcsrvPort() const;

#if 0
	bool getIPListeningAddr(vector<string> &ip);
#endif

	CriticalSection& CS_Config() const;
	// [Paths]
	string ftpReadDir(CPID cpId=0xFFFF) const;
	string ftpReadDir(const CLUSTER&) const;
	string ftpWriteDir(CPID cpId=0xFFFF) const;
	string ftpWriteDir(const CLUSTER&) const;
	string ftpDir(CPID cpId=0xFFFF) const;
	string ftpDir(const CLUSTER&) const;
	string tmpRoot(CPID cpId=0xFFFF) const;
	string confFcsrvDir(CPID cpId) const;
	string confFcsrvDir(const CLUSTER&) const;

	// [Debug]
	unsigned int isSimAp() const { return m_simAp; }

// modify
	// [Debug]
	void simAp(unsigned int val) { m_simAp = val; }
	// general funx
	void retreive() { s_initialized = false; };
	void createDirStructure(CPID cpId=0xFFFF);
	void createDirStructure(const CLUSTER&);

private:
	Config();
	Config(const Config& rhs);
	Config& operator=(const Config& rhs);
	void init();
	string getPath(const char*, CPID cpId=0xFFFF) const;
	string getPath(const char*, const CLUSTER&) const;

//attr
private:
	// [Debug]
	unsigned int m_simAp; // listen on/accept from ANY network

	static bool s_initialized; // have we read the values?
	static Config s_instance;
};

#endif
