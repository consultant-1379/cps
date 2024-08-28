/*
NAME
  File_name:SytemInfo.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2001. All rights reserved.

	The Copyright to the computer program(s) herein is the property of Ericsson
	Utvecklings AB, Sweden.
	The program(s) may be used and/or copied only with the written permission from
	Ericsson Utvecklings AB or in accordance with the terms and conditions
	stipulated in the agreement/contract under which the program(s) have been
	supplied.

DESCRIPTION
    global (static member) utility function for extrating system info. 

DOCUMENT NO
   190 89-CAA 109 0583

AUTHOR
   2006-05-17 by EAB/AZA/DG Lillemor Pettersson (uablan)

SEE ALSO
	-

Revision history
----------------
2006-05-17 uablan  Created
2010-12-14 xquydao Update for APG43L project
2015-12-17 egiaava Add vNIC speed handling for vAPZ

*/

#include "SystemInfo.h"
#include "boost/regex.hpp"
#include "CPS_BUSRV_Trace.h"

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <linux/sockios.h>
#include <linux/if.h>
//#include <net/if.h>
#include <linux/ethtool.h>

#include <sys/types.h>
#include <unistd.h>

#include <cerrno>
#include <iostream>
#include <fstream>
#include <fstream>
#include <string>
#include "Config.h"


using namespace std;

const string BOND_INTF_PATH    = "/proc/net/bonding/bond0";
const string BOND_INTF_PATTERN = "Currently Active Slave: (eth\\d+)";

const string DEF_INTF = "eth3";
const int    DEF_INTF_SPEED    = 1000;

bool GetBondActiveIntferace(string& ethName)
{
    newTRACE((LOG_LEVEL_INFO, "GetBondActiveIntferace(string& ethName)", 0));

    std::string line;
    bool found = false;
    ifstream bondFile(BOND_INTF_PATH.c_str());
    boost::regex re(BOND_INTF_PATTERN);    
    boost::cmatch matches;

    // Read bond interface attribute
    if (!bondFile.is_open())
    {
        TRACE((LOG_LEVEL_ERROR, "Open %s failed", 0, BOND_INTF_PATH.c_str()));
        return false;
    }
    
    while (bondFile.good())
    {
        getline(bondFile, line);
        
        try
        {
            // Search the pattern & get the active intf name
            if (boost::regex_search(line.c_str(), matches, re, boost::match_default))
            {
                ethName = string(matches[1].first, matches[1].second);
                found = true;
                TRACE((LOG_LEVEL_INFO, "Found active interface face: %s", 0, ethName.c_str()));
                break;
            }
        }
        catch (...)
        {
            TRACE((LOG_LEVEL_ERROR, "Grep active interface name failed", 0));
            break;
        }        
    }
    
    bondFile.close();

    return found;
}

// Return link speed of specified ethernet interface
bool GetEthLinkSpeed(const string& ethName, u_int32& speed)
{
    newTRACE((LOG_LEVEL_INFO, "GetEthLinkSpeed(%s, u_int32& speed)", 0, ethName.c_str()));

    if (Config::instance().isVAPZ())
    {
    	string filename(Config::instance().getCPSDataPath());
		filename.append(Config::instance().VCONFIG_FILE);
    	speed = ParseINIFunx::GetPrivateProfileInt(Config::ITEM_BACKUP_VSPEED.section, Config::ITEM_BACKUP_VSPEED.name, Config::ITEM_BACKUP_VSPEED.defVal,
			filename.c_str());
    	TRACE((LOG_LEVEL_INFO, "Obtained link vspeed: %d", 0, speed));
    	return true;
    }
    else
    {
    	int fd, ret;
    	struct ifreq ifReq;
    	struct ethtool_cmd eCmd;

    	// Copy ethernet interface name to ifReq
    	memset(&ifReq, 0, sizeof(ifReq));
    	ethName.copy(ifReq.ifr_name, ethName.length());

    	// Open control socket because ioctl needs to work on opened socket
    	fd = socket(AF_INET, SOCK_DGRAM, 0);

    	if (fd < 0)
    	{
    		TRACE((LOG_LEVEL_ERROR, "Obtain link speed failed at step opening socket", 0));
    		return false;
    	}

    	// Pass the "get link speed" command to eth tool driver
    	eCmd.cmd = ETHTOOL_GSET;
    	ifReq.ifr_data = (char*) &eCmd;
    	ret = ioctl(fd, SIOCETHTOOL, &ifReq);

    	if (ret != 0)
    	{
    		TRACE((LOG_LEVEL_ERROR, "Obtain link speed failed at step ioctl", 0));
    		close(fd);
    		return false;
    	}

    	// The speed is in Mbps
    	speed = eCmd.speed;
    	TRACE((LOG_LEVEL_INFO, "Obtained link speed: %d", 0, speed));
    	close(fd);

    	return true;
    }
}

// Returns the bandwidth of the CP-AP link in Mbps
int SystemInfo::getCurrentCPAPBandWidth(u_int32 &bandWidth)
{
    newTRACE((LOG_LEVEL_INFO, "SystemInfo::getCurrentCPAPBandWidth(u_int32 &bandWidth)", 0));

#if 0
    string ethName;

    if (!GetBondActiveIntferace(ethName))
    {
        TRACE(("Can't find the active interface name", 0));
        return 1;
    }
#endif

    return GetEthLinkSpeed(DEF_INTF, bandWidth) ? 0 : 1;
}

// Returns the process name from process id
// Returns null string in case not found
string GetProcessName(pid_t pid)
{
    newTRACE((LOG_LEVEL_INFO, "GetProcessName(%d)", 0, pid));

    stringstream ss;
    string line;
    ss << "/proc/" << pid << "/cmdline";

    // Read file /proc/<pid>/cmdline to findout process name
    ifstream stream(ss.str().c_str(), ifstream::in);

    while (stream.good())
    {
        // Get the first line splitted by '\0'
        getline(stream, line, '\0');
        TRACE((LOG_LEVEL_INFO, "Found line: %s", 0, line.c_str()));
        break;
    }

    // Close the stream
    stream.close();

    // Get the last string before '/'
    size_t pos = line.find_last_of('/') + 1;
    string processName = line.substr(pos);

    TRACE((LOG_LEVEL_INFO, "Found ProcessName: %s", 0, processName.c_str()));

    return processName;
}

int SystemInfo::getCurrentProcessID(void)
{
    return getpid();
}

// Returns the current process name
string SystemInfo::getCurrentProcessName(void)
{
    newTRACE((LOG_LEVEL_INFO, "SystemInfo::getCurrentProcessName(void)", 0));

    return GetProcessName(getpid());
}
