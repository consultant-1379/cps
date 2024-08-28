
/*
NAME
	File_name:Config.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

The Copyright to the computer program(s) herein is the property of Ericsson
Utvecklings AB, Sweden.
The program(s) may be used and/or copied only with the written permission from
Ericsson Utvecklings AB or in accordance with the terms and conditions
stipulated in the agreement/contract under which the program(s) have been
supplied.

DESCRIPTION
	BUAP configuration data.

DOCUMENT NO
//  190 89-CAA 109 1412

AUTHOR
	2003-01-07 by EAB/KY/SF Mona Ntterkvist (uabmnst, tag :mnst)

SEE ALSO
	-

Revision history
----------------
2003-01-07 uabmnst Created
2010-10-10 xchihoa Ported to Linux for APG43L
2012-07-05 xngudan Updated
*/

//#include "commondll.h"
#include "CPS_BUAP_Config.h"
#include "CPS_BUAP_Exception.h"
#include "CPS_BUAP_Events.h"
#include "CPS_BUAP_PARMGR_Global.h"
#include "CPS_BUAP_Trace.h"

#include "ACS_APGCC_CommonLib.h"
#include "acs_apgcc_paramhandling.h"

#include <set>
#include <string.h>

// registry
//

// misc. constants
//
const char* CPS_BUAP_Config::CONFIG_FILE = "reladmpar";
const char* CPS_BUAP_Config::LOGICAL_NAME_CPS_DATA = "CPS_DATA";
//
// singleton statics
//
CPS_BUAP_Config CPS_BUAP_Config::s_instance;
bool CPS_BUAP_Config::s_initialized = false;
//
// ctor (private)
//===========================================================================
CPS_BUAP_Config::CPS_BUAP_Config() {
}
//
//
//===========================================================================
void CPS_BUAP_Config::init() {
	// change for Tracing order
    //  newTRACE((LOG_LEVEL_INFO, "Config::init()", 0));

    // Get paths from registry
    m_cpsReladmpar = getPath();


    // if we get here config values are OK
    s_initialized = true;

}
//
//
//
//===========================================================================
string CPS_BUAP_Config::getPath() {

    int result = ACS_APGCC_STRING_BUFFER_SMALL;
    int ilen = 50;
    char *szPath = NULL;
    string ReturnPath = "";
    string FileName = "/reladmpar.ini";
    ACS_APGCC_CommonLib acs;


    // Try getting the datadisk path requested until we get the buffer size right
    while (ACS_APGCC_STRING_BUFFER_SMALL == result) {
        if (NULL != szPath)
            free(szPath);

        szPath = (char*) malloc(ilen*sizeof(char));
        if (NULL != szPath) {
            result = acs.GetDataDiskPath(LOGICAL_NAME_CPS_DATA, szPath, ilen);
        }
        else {
                free(szPath);
                return ReturnPath;
        }
    }

    // Condition for other error codes from GetDataDiskPath. We can do much about it
    // so return FALSE and raise an event
    if (ACS_APGCC_DNFPATH_SUCCESS != result) {
        if (NULL != szPath)
            free(szPath);
        // change for Tracing order
        //EVENT((CPS_BUAP_Events::reladmpar_fail, 0, "Failed to retrieve reladmpar path"));
        return ReturnPath;
    }



    if(szPath) {
        ReturnPath = szPath;
        ReturnPath.append(FileName);
    }
    if (NULL != szPath)
        free(szPath);
    return ReturnPath;
}

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
bool CPS_BUAP_Config::isClassicCP() const
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_Config::isClassicCP()", 0));

    static bool isClassicSys = false;
    static bool contactIMM = false;

    if (contactIMM)
    {
        // No need to fetch the info again
        // Just use the previous value
        return isClassicSys;
    }

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
        throw 1;
    }

    ret = immHandle.getClassInstances("AxeFunctions", dnList);

    if (ret != ACS_CC_SUCCESS)
    {
        TRACE((LOG_LEVEL_ERROR, "Get class instance failed: %d - %s", 0,
               immHandle.getInternalLastError(),
               immHandle.getInternalLastErrorText()));

        immHandle.Finalize();
        throw 1;
    }
    else
    {
        immHandle.Finalize();
        TRACE(("AxeFunctions DN: %s", 0, dnList[0].c_str()));
    }

   acs_apgcc_paramhandling phaObject;
   const char *pAttrName = "apzProtocolType";

    int apzProtocolTypeValue = 0;
    ret = phaObject.getParameter(dnList[0], pAttrName, &apzProtocolTypeValue);

    if(ACS_CC_SUCCESS == ret)
    {
        if( (0 == apzProtocolTypeValue) || (1 == apzProtocolTypeValue) )
        {
            isClassicSys = true;
        }
        else
        {
            TRACE((LOG_LEVEL_WARN, "This system is not Classic", 0));
        }
    }
    else
    {
       TRACE((LOG_LEVEL_ERROR, "CPS_BUAP_Config::isClassicCP(): Reading PHA param apzProtocolType failed", 0));
       throw 1;
    }

    return isClassicSys;
}

