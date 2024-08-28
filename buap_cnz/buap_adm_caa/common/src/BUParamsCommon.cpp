/*
 * COPYRIGHT Ericsson Utvecklings AB, Sweden 2011.
 * All rights reserved.
 *
 * The Copyright to the computer program(s) herein
 * is the property of Ericsson Utvecklings AB, Sweden.
 * The program(s) may be used and/or copied only with
 * the written permission from Ericsson Utvecklings AB or in
 * accordance with the terms and conditions stipulated in the
 * agreement/contract under which the program(s) have been
 * supplied.
 *
 * NAME
 *  BUParamsCommon.cpp
 *
 * DESCRIPTION
 *  -
 *
 * DOCUMENT NO
 *  190 89-CAA 109 1414
 *
 * AUTHOR
 *  XDT/DEK XNGUDAN
 *
 * === Revision history ===
 * 2012-02-20   XNGUDAN    Create
 * 2012-03-08   XNGUDAN    Update after model review
 *
 */


#include "BUParamsCommon.h"
#include <boost/assign/list_of.hpp>
#include <stdint.h>
using namespace std;
using namespace boost::assign;

// Use this marco to suppress unused variable warning
#define UNUSED(x) ((void) x)

/**************************************************************************************
 *
 *          GENERAL DEFINITION
 *
 * ************************************************************************************
 */
const string BUParamsCommon::IMM_IMPL_PARENT_NAME  = "CPS_OI_BUPARAMS_CLASSIC";
const string BUParamsCommon::IMM_PARENT_OBJ_DN     =  "apzFunctionsId=1,managedElementId=1";
//const string BUParamsCommon::IMM_CLASSIC_OBJ_DN    = "classicCpId=1";
const string BUParamsCommon::IMM_CLASSIC_OBJ_DN    = "AxeCpReloadClassicclassicCpId=1";
//const string BUParamsCommon::IMM_PARENT_CLASS_NAME = "CpReload";
const string BUParamsCommon::IMM_PARENT_CLASS_NAME = "AxeCpReloadCpReloadM";
//const string BUParamsCommon::IMM_CLASSIC_CLASS_NAME = "ClassicCp";
const string BUParamsCommon::IMM_CLASSIC_CLASS_NAME = "AxeCpReloadClassicClassicCp";
//const string BUParamsCommon::IMM_ATTR_CLASSIC_RDN = "classicCpId";
const string BUParamsCommon::IMM_ATTR_CLASSIC_RDN = "AxeCpReloadClassicclassicCpId";
//const string BUParamsCommon::IMM_PARENT_CLASSIC_OBJ_DN  = "cpReloadId=1";
const string BUParamsCommon::IMM_PARENT_CLASSIC_OBJ_DN  = "AxeCpReloadcpReloadMId=1";

const string BUParamsCommon::IMM_ATTR_ISVALID      = "isValid";

const string BUParamsCommon::IMM_ATTR_ID        = "id";

const string BUParamsCommon::IMM_ACT_RESET      = "resetStaticParams";

const string BUParamsCommon::IMM_ATTR_STATIC     = "staticParams";
const string BUParamsCommon::IMM_ATTR_DYNAMIC    = "dynamicParams";
const string BUParamsCommon::IMM_ATTR_CMDLOGREF  = "commandLogReferences";
const string BUParamsCommon::IMM_ATTR_DMEASURES  = "delayedMeasures";
const string BUParamsCommon::IMM_ATTR_LASTRELOAD = "lastReload";

const string BUParamsCommon::IMM_ATTR_STATIC_VALID     = "isStaticParamsValid";
const string BUParamsCommon::IMM_ATTR_DYNAMIC_VALID    = "isDynamicParamsValid";
const string BUParamsCommon::IMM_ATTR_CMDLOGREF_VALID  = "isCommandLogReferencesValid";
const string BUParamsCommon::IMM_ATTR_DMEASURES_VALID  = "isDelayedMeasuresValid";
const string BUParamsCommon::IMM_ATTR_LASTRELOAD_VALID = "isLastReloadValid";

const string BUParamsCommon::VALIDITY_FLAG = "Validity flags";

// Action Error Messages
//const string BUParamsCommon::ACTION_INITFAILED     = "initFailed";
//const string BUParamsCommon::ACTION_SYNCFAILED     = "syncFailed"
const string BUParamsCommon::INITFAILED_MSG        = "@ComNbi@Cannot initialize interaction with IMM";
const string BUParamsCommon::SYNCFAILED_MSG        = "@ComNbi@Cannot synchronize between IMM and reladmpar file";

/**************************************************************************************
 *
 *          STATICPARAMS DEFINITION
 *
 * ************************************************************************************
 */
//const string BUParamsCommon::IMM_SP_OBJ_DN       = "id=staticParams_0,classicCpId=1";
const string BUParamsCommon::IMM_SP_OBJ_DN       = "id=staticParams_0,AxeCpReloadClassicclassicCpId=1";
const string BUParamsCommon::IMM_IMPL_NAME       = "CPS_OI_STATIC_PARAMS";
const string BUParamsCommon::IMM_CLASS_SP_NAME_   = "StaticParams";
const string BUParamsCommon::IMM_CLASS_SP_NAME   = "AxeCpReloadClassicStaticParams";
const string BUParamsCommon::IMM_ATTR_CLH       = "commandLogMode";
const string BUParamsCommon::IMM_ATTR_INCL1     = "includeFirstFileRange";
const string BUParamsCommon::IMM_ATTR_INCL2     = "includeSecondFileRange";
const string BUParamsCommon::IMM_ATTR_INCLA     = "oldestAcceptedDate";
const string BUParamsCommon::IMM_ATTR_IV        = "isValid";
const string BUParamsCommon::IMM_ATTR_LOAZ      = "logOmissionMode";
const string BUParamsCommon::IMM_ATTR_NTAZ      = "numberOfTruncationAttempts";
const string BUParamsCommon::IMM_ATTR_NTCZ      = "numberOfTruncationCommands";
const string BUParamsCommon::IMM_ATTR_SUP       = "supervisionTime";

/**************************************************************************************
 *
 *          COMMANDLOGREFERENCES DEFINITION
 *
 * ************************************************************************************
 */
//const string BUParamsCommon::IMM_CMDLOG_OBJ_DN       = "id=commandLogReferences_0,classicCpId=1";
const string BUParamsCommon::IMM_CMDLOG_OBJ_DN       = "id=commandLogReferences_0,AxeCpReloadClassicclassicCpId=1";
const string BUParamsCommon::IMM_IMPL_CMDLOG_NAME    = "CPS_RO_CMDLOG_REFERENCES";
const string BUParamsCommon::IMM_CLASS_CMDLOG_NAME_   = "CommandLogReferences";
const string BUParamsCommon::IMM_CLASS_CMDLOG_NAME   = "AxeCpReloadClassicCommandLogReferences";
//const string BUParamsCommon::IMM_ATTR_CMDLOG_RDN     = "commandLogReferencesId";
const string BUParamsCommon::IMM_ATTR_CURRENT        = "currentSubfileNumber";
const string BUParamsCommon::IMM_ATTR_MAIN           = "mainStoreSubfileNumber";
const string BUParamsCommon::IMM_ATTR_PREPARED       = "preparedSubfileNumber";


/**************************************************************************************
 *
 *          DELAYEDMEASURES DEFINITION
 *
 * ************************************************************************************
 */
//const string BUParamsCommon::IMM_DM_OBJ_DN       = "id=delayedMeasures_0,classicCpId=1";
const string BUParamsCommon::IMM_DM_OBJ_DN       = "id=delayedMeasures_0,AxeCpReloadClassicclassicCpId=1";
const string BUParamsCommon::IMM_IMPL_DM_NAME    = "CPS_RO_DELAYED_MEASURES";
const string BUParamsCommon::IMM_CLASS_DM_NAME_   = "DelayedMeasures";
//const string BUParamsCommon::IMM_ATTR_DM_RDN     = "delayedMeasuresId";
const string BUParamsCommon::IMM_CLASS_DM_NAME   = "AxeCpReloadClassicDelayedMeasures";
const string BUParamsCommon::IMM_ATTR_DM1        = "preparedCommandLog";
const string BUParamsCommon::IMM_ATTR_DM2        = "mainStoreCommandLog";
const string BUParamsCommon::IMM_ATTR_DM3        = "currentCommandLog";
const string BUParamsCommon::IMM_ATTR_DM4        = "mainStoreStepping";
const string BUParamsCommon::IMM_ATTR_DM5        = "dumpStepping";
const string BUParamsCommon::IMM_ATTR_DM6        = "preparedStepping";
const string BUParamsCommon::IMM_ATTR_DM7        = "smallBackupArea";
const string BUParamsCommon::IMM_ATTR_DM8        = "largeBackupArea";
const string BUParamsCommon::IMM_ATTR_DM         = "delayedMeasure";



/**************************************************************************************
 *
 *          DYNAMICPARAMS DEFINITION
 *
 * ************************************************************************************
 */
//const string BUParamsCommon::IMM_DP_OBJ_DN                  = "id=dynamicParams_0,classicCpId=1";
const string BUParamsCommon::IMM_DP_OBJ_DN                  = "id=dynamicParams_0,AxeCpReloadClassicclassicCpId=1";
const string BUParamsCommon::IMM_IMPL_DP_NAME               = "CPS_RO_DYNAMIC_PARAMS";
const string BUParamsCommon::IMM_CLASS_DP_NAME_              = "DynamicParams";
//const string BUParamsCommon::IMM_ATTR_DP_RDN                = "dynamicParamsId";
const string BUParamsCommon::IMM_CLASS_DP_NAME              = "AxeCpReloadClassicDynamicParams";
const string BUParamsCommon::IMM_ATTR_CFFR                  = "currentFileNumberInFFR";
const string BUParamsCommon::IMM_ATTR_CSFR                  = "currentFileNumberInSFR";
const string BUParamsCommon::IMM_ATTR_CTA                   = "currentTruncationAttempt";
const string BUParamsCommon::IMM_ATTR_EXP                   = "expiringTimeSupervision";
const string BUParamsCommon::IMM_ATTR_FOLD                  = "highestFileNumberInFFR";
const string BUParamsCommon::IMM_ATTR_LPF                   = "lastPermissibleGeneration";
const string BUParamsCommon::IMM_ATTR_NCT                   = "numberOfTruncatedCommands";
const string BUParamsCommon::IMM_ATTR_OMISSION              = "logOmissionSetting";
const string BUParamsCommon::IMM_ATTR_SOLD                  = "highestFileNumberInSFR";
const string BUParamsCommon::IMM_ATTR_STATE                 = "loadingState";


/**************************************************************************************
 *
 *          LASTAUTORELOAD DEFINITION
 *
 * ************************************************************************************
 */
//const string BUParamsCommon::IMM_LAR_OBJ_DN                 = "id=lastAutoReload_0,classicCpId=1";
const string BUParamsCommon::IMM_LAR_OBJ_DN                 = "id=lastReload_0,AxeCpReloadClassicclassicCpId=1";
const string BUParamsCommon::IMM_IMPL_LAR_NAME              = "CPS_RO_LAST_RELOAD";
const string BUParamsCommon::IMM_CLASS_LAR_NAME_             = "LastReload";
//const string BUParamsCommon::IMM_ATTR_LAR_RDN               = "lastAutoReloadId";
const string BUParamsCommon::IMM_CLASS_LAR_NAME             = "AxeCpReloadClassicLastReload";
const string BUParamsCommon::IMM_ATTR_ALOG                  = "logSubfileNoOfLastAutoReload";
const string BUParamsCommon::IMM_ATTR_GFILE                 = "generationFileNumber";
const string BUParamsCommon::IMM_ATTR_LPGF                  = "lastPossibleGenerationFile";
const string BUParamsCommon::IMM_ATTR_ODSL                  = "dataStoreLargeDumpOutputTime";
const string BUParamsCommon::IMM_ATTR_ODSS                  = "dataStoreSmallDumpOutputTime";
const string BUParamsCommon::IMM_ATTR_OPSRS                 = "programStoreReferenceStoreOutputTime";
const string BUParamsCommon::IMM_ATTR_LAR_CLH               = "commandLogMode";
const string BUParamsCommon::IMM_ATTR_LAR_SUP               = "supervisionTime";
const string BUParamsCommon::IMM_ATTR_LAR_NCT               = "currentTruncatedCommands";
const string BUParamsCommon::IMM_ATTR_LAR_OMISSION          = "logOmissionSetting";

/**************************************************************************************
 *
 *          ERROR CODE LIST
 *
 * ************************************************************************************
 */
map<BUParamsCommon::SPARAM_ERR, string> BUParamsCommon::errorCodeToStringList = map_list_of
        (BUParamsCommon::INVALID_SYS_CONF, "Invalid system configuration")
        (BUParamsCommon::INVALID_OBJ_DN, "Invalid object DN")
        (BUParamsCommon::INVALID_ATTR, "Invalid object attribute")
        (BUParamsCommon::INVALID_VALUE, "Invalid value")
        (BUParamsCommon::VALUE_NOT_IN_RANGE, "Value is not in the range defined by system")
        (BUParamsCommon::INVALID_SUPTIME, "Invalid supervision time")
        (BUParamsCommon::INVALID_FFR, "Invalid first file range")
        (BUParamsCommon::INVALID_SFR, "Invalid second file range")
        (BUParamsCommon::INVALID_CLH, "Invalid command log mode value")
        (BUParamsCommon::INVALID_NTCA, "Invalid number of current truncation attempts")
        (BUParamsCommon::INVALID_NTCZ, "Invalid number of current truncation commands ")
        (BUParamsCommon::INVALID_LOG, "Invalid Log omission setting")
        (BUParamsCommon::INVALID_DATE, "The oldest accepted birth date is invalid")
        (BUParamsCommon::INVALID_ACTION, "@ComNbi@cannot initialize interaction with IMM")
        (BUParamsCommon::NO_INITIALIZED, "OM is not initialized")
        (BUParamsCommon::NO_REQUESTUPDATED, "No attribute requests update")
        (BUParamsCommon::SYNC_FAILED, "Synchronization between IMM and reladmpar.ini file failed")
        (BUParamsCommon::ACTION_INITFAILED, "InitFailed")
        (BUParamsCommon::ACTION_SYNCFAILED, "SyncFailed");


/**
 * @Brief Method to convert error code
 *
 * @param errorCode
 * @return error code string
 */
string BUParamsCommon::errorCodeToString(BUParamsCommon::SPARAM_ERR errorCode)
{
    return BUParamsCommon::errorCodeToStringList[errorCode];
}

