//******************************************************************************
// NAME
// CPS_Parameters.cpp
//
// COPYRIGHT Ericsson AB, Sweden 2012.
// All rights reserved.
//
// The Copyright to the computer program(s) herein 
// is the property of Ericsson AB, Sweden.
// The program(s) may be used and/or copied only with 
// the written permission from Ericsson AB or in 
// accordance with the terms and conditions stipulated in the 
// agreement/contract under which the program(s) have been 
// supplied.
//
// DESCRIPTION
// Handling of Permenant Exchange Parameters for BUSRV
//
// DOCUMENT NO
// 190 89-CAA 109 0387
//
// AUTHOR
// 2012-04-18 by DEK/XDTTHNG
//
// SEE ALSO
// -
//
// Revision history
// ----------------
//
//******************************************************************************

#include "CPS_Parameters.h"
#include "PlatformTypes.h"
#include "CPS_BUSRV_Trace.h"

const char* CPS_Parameters::s_Params[] = {
    "cpBackupAndReloadConfigId=1",
    "ftpDumpRoot",
    "ftpLoadRoot",
    "maxNumberOfSimultaneousLoad",
    "maxNumberOfSimultaneousDump",
    "MinReloadSupervisionTime",
    "MaxLoadingSupervisionTime",
    "MaxDumpingSupervisionTime"
};

int CPS_Parameters::get(PARAM_TYPE param, int defaultValue)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_Parameters::get(%d, %d)", 0, param, defaultValue));
    u_int32 value = 0;
    ACS_CC_ReturnType result = m_phaObj.getParameter(s_Params[DN_BACKUP_RELOAD], 
        s_Params[param], &value);
    TRACE((LOG_LEVEL_INFO, "get() for <%s> returns <%d> value <%d>", 0, s_Params[param], result, value));
    return result == ACS_CC_SUCCESS ? value : defaultValue;
}

string CPS_Parameters::get(PARAM_TYPE param, const char* defaultStr)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_Parameters::get(%d, %s)", 0, param, defaultStr));
    char value[256]={0};
    *value = '\0';
    ACS_CC_ReturnType result = m_phaObj.getParameter<256>(s_Params[DN_BACKUP_RELOAD], 
        s_Params[param],value);
    TRACE((LOG_LEVEL_INFO, "get() for <%s> returns <%d> value <%s>", 0, s_Params[param], result, value));
    return result == ACS_CC_SUCCESS ? string(value) : string(defaultStr);
}
