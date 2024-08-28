/*
NAME
   File_name: BUParamsCommon.cpp

COPYRIGHT Ericsson AB, Sweden 2012. All rights reserved.

   The Copyright to the computer program(s) herein is the property of Ericsson
   Utvecklings AB, Sweden.
   The program(s) may be used and/or copied only with the written permission from
   Ericsson Utvecklings AB or in accordance with the terms and conditions
   stipulated in the agreement/contract under which the program(s) have been
   supplied.

DESCRIPTION
   Common Utilities for OI & OM.

DOCUMENT NO
   190 89-CAA 109 1409

AUTHOR
   2012-04-20 by Quyen Dao

SEE ALSO


Revision history
----------------
2012-04-20 xquydao Created

*/

#include "BUParamsCommon.h"
#include "BUPFunx.h"
#include "Config.h"
#include "CPS_BUSRV_Trace.h"
#include "boost/assign/list_of.hpp"
#include "boost/algorithm/string.hpp"
#include <sstream>

#include "ACS_APGCC_CommonLib.h"


using namespace std;

const string BUParamsCommon::IMM_IMPL_NAME          = "CPS_BUSRV_CpReloadOI";
const string BUParamsCommon::IMM_CLN_NONCLASSIC     = "AxeCpReloadDualSidedCp";
const string BUParamsCommon::IMM_CLN_CLUSTER        = "AxeCpReloadCpCluster";
const string BUParamsCommon::IMM_CLN_PARENT         = "AxeCpReloadCpReloadM";
const string BUParamsCommon::IMM_ATTR_NONCLASSIC_ID = "dualSidedCpId";
const string BUParamsCommon::IMM_ATTR_CLUSTER_ID    = "cpClusterId";
const string BUParamsCommon::IMM_ATTR_FFR           = "firstFileRange";
const string BUParamsCommon::IMM_ATTR_SFR           = "secondFileRange";
const string BUParamsCommon::IMM_ATTR_ST            = "supervisionTime";

map<BUParamsCommon::BUPARAM_ERR, std::string> BUParamsCommon::errorCodeToStringList = boost::assign::map_list_of
                                                                (BUParamsCommon::UNINITIALIZED_OM, "Uninitialized OM")
                                                                (BUParamsCommon::UNSUPPORTED_SYSTEM, "Unsupported system")
                                                                (BUParamsCommon::OBJID_OBTAINING_FAILURE, "Obtaining Instance ID failure")
                                                                (BUParamsCommon::SYSTYPE_CHECKING_FAILURE, "Checking system type failure")
                                                                (BUParamsCommon::INVALID_OBJ_NAME, "Invalid object name")
                                                                (BUParamsCommon::INVALID_ATTR, "Invalid object attribute")
                                                                (BUParamsCommon::INVALID_FFR, "Invalid FFR")
                                                                (BUParamsCommon::FFR_NOT_IN_RANGE, "FFR is not in the range defined by FMS")
                                                                (BUParamsCommon::INVALID_SFR, "Invalid SFR")
                                                                (BUParamsCommon::SFR_NOT_IN_RANGE, "SFR is not in the range defined by FMS")
                                                                (BUParamsCommon::INVALID_SUPTIME, "Invalid supervision time")
                                                                (BUParamsCommon::BUPARAM_OBTAINING_FAILURE, "Obtaining BUPARAM failure")
                                                                (BUParamsCommon::UNKNOWN, "Unknown error");
                                                                
map<BUParamsCommon::IMM_CP_TYPE, std::string> BUParamsCommon::cpTypeToObjectNameList = boost::assign::map_list_of
    (BUParamsCommon::CP_SINGLE, BUParamsCommon::IMM_ATTR_NONCLASSIC_ID + "=1")
    (BUParamsCommon::CP_SPX1, BUParamsCommon::IMM_ATTR_NONCLASSIC_ID + "=1001")
    (BUParamsCommon::CP_SPX2, BUParamsCommon::IMM_ATTR_NONCLASSIC_ID + "=1002")
    (BUParamsCommon::CP_CLUSTER, BUParamsCommon::IMM_ATTR_CLUSTER_ID + "=1")
    (BUParamsCommon::CP_NONE, "");

const string& BUParamsCommon::cpTypeToObjectName(IMM_CP_TYPE cpType)
{
    return BUParamsCommon::cpTypeToObjectNameList[cpType];
}

u_int16 BUParamsCommon::cpTypeToCPID(IMM_CP_TYPE cpType)
{
    u_int16 cpId = 0;

    switch (cpType)
    {
    case CP_SINGLE:
        cpId = CPID_DEF;
        break;

    case CP_SPX1:
        cpId = CPID_SPX1;
        break;

    case CP_SPX2:
        cpId = CPID_SPX2;
        break;

    default:
        cpId = 0;
        break;
    }

    return cpId;
}

BUParamsCommon::IMM_CP_TYPE BUParamsCommon::objectDNToCpType(const string& objDN)
{
	newTRACE((LOG_LEVEL_INFO, "BUParamsCommon::objectDNToCpType(%s)", 0, objDN.c_str()));
    BUParamsCommon::IMM_CP_TYPE cpType = BUParamsCommon::CP_NONE;

    // ObjName will have this form "<cpType>=<cpName>,CPReload=1" or other form
    // Just take the string before the first "," for the next parsing
    // If there is no "," , will take the original objName for the next parsing
    vector<string> fields;
    boost::algorithm::split(fields, objDN, boost::is_any_of(","));
    
    // Convert objectName to cpType
    string objectName(fields[0]);
    map<IMM_CP_TYPE, string>::iterator it;
    for (it = BUParamsCommon::cpTypeToObjectNameList.begin(); it != BUParamsCommon::cpTypeToObjectNameList.end(); ++it)
    {
        if ((*it).second == objectName)
        {
            cpType = (*it).first;
            break;
        }
    }
   
    return cpType;
}

const string& BUParamsCommon::errorCodeToString(BUParamsCommon::BUPARAM_ERR errorCode)
{
    return BUParamsCommon::errorCodeToStringList[errorCode];
}

const string& BUParamsCommon::getParentDN()
{
    newTRACE((LOG_LEVEL_INFO, "BUParamsCommon::getParentDN()", 0));

    static string parentDN("");
    static bool contactIMM = false;

    if (contactIMM)
    {
        // No need to fetch the info again
        // Just use the previous value
        return parentDN;
    }

    // Get the CpReload DN
    OmHandler immHandle;
    ACS_CC_ReturnType ret;
    std::vector<std::string> dnList;

    ret = immHandle.Init();
    if (ret != ACS_CC_SUCCESS)
    {
        TRACE((LOG_LEVEL_ERROR, "Init OMHandler failed: %d - %s", 0,
                immHandle.getInternalLastError(),
                immHandle.getInternalLastErrorText()));

        return parentDN;
    }

    ret = immHandle.getClassInstances(BUParamsCommon::IMM_CLN_PARENT.c_str(), dnList);

    if (ret != ACS_CC_SUCCESS)
    {
        TRACE((LOG_LEVEL_ERROR, "Get class instance failed: %d - %s", 0,
               immHandle.getInternalLastError(),
               immHandle.getInternalLastErrorText()));
    }
    else
    {
        contactIMM = true;
        parentDN = dnList[0];
        TRACE((LOG_LEVEL_INFO, "Parent DN: %s", 0, dnList[0].c_str()));
    }

    immHandle.Finalize();

    return parentDN;
}
