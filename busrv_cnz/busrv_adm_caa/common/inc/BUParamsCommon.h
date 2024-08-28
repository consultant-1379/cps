/*
NAME
   File_name: BUParamsCommon.h

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

#ifndef BUPARAMSCOMMON_H_
#define BUPARAMSCOMMON_H_

#include "PlatformTypes.h"

#include <string>
#include <map>


class BUParamsCommon
{
public:
    enum IMM_CP_TYPE
    {
        CP_SINGLE = 0,
        CP_SPX1,
        CP_SPX2,
        CP_CLUSTER,
        CP_NONE
    };
   
    struct BUParam
    {
        u_int32     ffr;
        u_int32     sfr;
        u_int32     supTime;

        // Resetting to default value for reading
        void reset() {
        	ffr = 0;
        	sfr = 0;
        	supTime = 60;
        }
    };
    
    enum BUPARAM_FLAG
    {
        FLG_FFR = 1,
        FLG_SFR = 2,
        FLG_ST  = 4
    };

    static const std::string IMM_IMPL_NAME;
    static const std::string IMM_CLN_NONCLASSIC;
    static const std::string IMM_CLN_CLUSTER;
    static const std::string IMM_CLN_PARENT;
    static const std::string IMM_ATTR_NONCLASSIC_ID;
    static const std::string IMM_ATTR_CLUSTER_ID;
    static const std::string IMM_ATTR_FFR;
    static const std::string IMM_ATTR_SFR;
    static const std::string IMM_ATTR_ST;

    enum CP_ID
    {
        CPID_DEF  = 65535,
        CPID_SPX1 = 1001,
        CPID_SPX2 = 1002        
    };


    enum BUPARAM_ERR
    {
        // OM ERR
        UNINITIALIZED_OM = 1001,
        UNSUPPORTED_SYSTEM,
        OBJID_OBTAINING_FAILURE,
        SYSTYPE_CHECKING_FAILURE,

        // OI ERR
        INVALID_OBJ_NAME = 2001,
        INVALID_ATTR,
        INVALID_FFR,
        FFR_NOT_IN_RANGE,
        INVALID_SFR,
        SFR_NOT_IN_RANGE,
        INVALID_SUPTIME,
        BUPARAM_OBTAINING_FAILURE,
        UNKNOWN
    };

public:
    static const std::string& cpTypeToObjectName(IMM_CP_TYPE cpType);
    static u_int16 cpTypeToCPID(IMM_CP_TYPE cpType);
    static IMM_CP_TYPE objectDNToCpType(const std::string& objDN);
    static const std::string& errorCodeToString(BUPARAM_ERR errorCode);
    static const std::string& getParentDN();
    
private:
    static std::map<BUPARAM_ERR, std::string> errorCodeToStringList;
    static std::map<IMM_CP_TYPE, std::string> cpTypeToObjectNameList;
};
#endif
