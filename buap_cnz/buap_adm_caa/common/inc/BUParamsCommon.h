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
 *  BUParamsCommon.h
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

#ifndef BUPARAMSCOMMON_H_
#define BUPARAMSCOMMON_H_

#include <string>
#include <stdint.h>
#include <map>
const unsigned char c_Valid                 = 1;  // Boolean true value
const unsigned char c_Invalid               = 0;  // Boolean false value

const int MAX_DELAYED = 8;

class BUParamsCommon
{
public:

    /****************************************************************************
     *                  GENERAL DEFINITION
     * **************************************************************************
     */
    enum SPARAM_ERR
    {
        INVALID_SYS_CONF = 101,
        INVALID_OBJ_DN,
        INVALID_ATTR,
        INVALID_VALUE,
        VALUE_NOT_IN_RANGE,
        INVALID_SUPTIME,
        INVALID_FFR,
        INVALID_SFR,
        INVALID_CLH,
        INVALID_NTCA,
        INVALID_NTCZ,
        INVALID_LOG,
        INVALID_DATE,
        INVALID_ACTION,
        NO_INITIALIZED,
        NO_REQUESTUPDATED,
        SYNC_FAILED,
        ACTION_INITFAILED,
        ACTION_SYNCFAILED
    };

    static const std::string IMM_IMPL_PARENT_NAME;
    static const std::string IMM_PARENT_OBJ_DN;
    static const std::string IMM_PARENT_CLASS_NAME;
    static const std::string IMM_ATTR_ISVALID;
    static const std::string IMM_CLASSIC_CLASS_NAME;
    static const std::string IMM_ATTR_CLASSIC_RDN;
    static const std::string IMM_CLASSIC_OBJ_DN;
    static const std::string IMM_PARENT_CLASSIC_OBJ_DN;
    static const std::string IMM_ATTR_ID;


    static const std::string IMM_ATTR_STATIC;
    static const std::string IMM_ATTR_DYNAMIC;
    static const std::string IMM_ATTR_CMDLOGREF;
    static const std::string IMM_ATTR_DMEASURES;
    static const std::string IMM_ATTR_LASTRELOAD;

    static const std::string IMM_ATTR_STATIC_VALID;
    static const std::string IMM_ATTR_DYNAMIC_VALID;
    static const std::string IMM_ATTR_CMDLOGREF_VALID;
    static const std::string IMM_ATTR_DMEASURES_VALID;
    static const std::string IMM_ATTR_LASTRELOAD_VALID;

    static const std::string IMM_ACT_RESET;

    static const std::string VALIDITY_FLAG;
    
    /***************************************************************************
     *                     ACTION ERROR MESSAGES
     ***************************************************************************/

//    static const std::string ACTION_INITFAILED;
//    static const std::string ACTION_SYNCFAILED;
    static const std::string INITFAILED_MSG;
    static const std::string SYNCFAILED_MSG; 

    /****************************************************************************
     *                  STATIC PARAMETERS
     * **************************************************************************
     */

    enum
    {
        FLG_CLH = 1,
        FLG_INCL1 = 2,
        FLG_INCL2 = 4,
        FLG_INCLA = 8,
        FLG_LOAZ  = 16,
        FLG_NTAZ  = 32,
        FLG_NTCZ  = 64,
        FLG_SUP   = 128
    };

    struct  StaticParam_
    {
        int32_t      clh;
        int32_t      incl1;
        uint32_t     incl2;
        std::string  incla;
        int32_t      loaz;
        uint32_t     ntaz;
        uint32_t     ntcz;
        uint32_t     sup;
    };


    static const std::string IMM_SP_OBJ_DN;
    static const std::string IMM_IMPL_NAME;
    static const std::string IMM_CLASS_SP_NAME_;
    static const std::string IMM_CLASS_SP_NAME;
    static const std::string IMM_ATTR_CLH;
    static const std::string IMM_ATTR_INCL1;
    static const std::string IMM_ATTR_INCL2;
    static const std::string IMM_ATTR_INCLA;
    static const std::string IMM_ATTR_IV;;
    static const std::string IMM_ATTR_LOAZ;
    static const std::string IMM_ATTR_NTAZ;
    static const std::string IMM_ATTR_NTCZ;
    static const std::string IMM_ATTR_SUP;


    /****************************************************************************
     *                  STATIC PARAMETERS
     * **************************************************************************
     */

    /****************************************************************************
     *                  COMMANDLOGREFERENCES
     * **************************************************************************
     */
    struct CommmandLogReference_
    {
        uint32_t current;
        uint32_t main;
        uint32_t prepared;
    };

    static const std::string IMM_CMDLOG_OBJ_DN;
    static const std::string IMM_IMPL_CMDLOG_NAME;
    static const std::string IMM_CLASS_CMDLOG_NAME_;
    static const std::string IMM_CLASS_CMDLOG_NAME;
    static const std::string IMM_ATTR_CMDLOG_RDN;
    static const std::string IMM_ATTR_CURRENT;
    static const std::string IMM_ATTR_MAIN;
    static const std::string IMM_ATTR_PREPARED;


    /****************************************************************************
     *                  COMMANDLOGREFERENCES
     * **************************************************************************
     */


    /****************************************************************************
     *                  DELAYEDMEASURES
     * **************************************************************************
     */
    struct DelayedMeasure_
    {
        int32_t dm[MAX_DELAYED];
    };

    static const std::string IMM_DM_OBJ_DN;
    static const std::string IMM_IMPL_DM_NAME;
    static const std::string IMM_CLASS_DM_NAME_;
    static const std::string IMM_CLASS_DM_NAME;
    static const std::string IMM_ATTR_DM_RDN;
    static const std::string IMM_ATTR_DM1;
    static const std::string IMM_ATTR_DM2;
    static const std::string IMM_ATTR_DM3;
    static const std::string IMM_ATTR_DM4;
    static const std::string IMM_ATTR_DM5;
    static const std::string IMM_ATTR_DM6;
    static const std::string IMM_ATTR_DM7;
    static const std::string IMM_ATTR_DM8;
    static const std::string IMM_ATTR_DM;


    /****************************************************************************
     *                  DELAYEDMEASURES
     * **************************************************************************
     */

    /****************************************************************************
     *                  DYNAMICPARAMS
     * **************************************************************************
     */
    struct DynamicParam_
    {
        uint32_t state;
        uint32_t cta;
        uint32_t cffr;
        uint32_t csfr;
        uint32_t fold;
        uint32_t sold;
        uint32_t nct;
        uint32_t lpf;
        int32_t omission;
        std::string exp;
    };

    static const std::string IMM_DP_OBJ_DN;
    static const std::string IMM_IMPL_DP_NAME;
    static const std::string IMM_CLASS_DP_NAME_;
    static const std::string IMM_CLASS_DP_NAME;
    static const std::string IMM_ATTR_DP_RDN;
    static const std::string IMM_ATTR_CFFR;
    static const std::string IMM_ATTR_CSFR;
    static const std::string IMM_ATTR_CTA;
    static const std::string IMM_ATTR_EXP;
    static const std::string IMM_ATTR_FOLD;
    static const std::string IMM_ATTR_LPF;
    static const std::string IMM_ATTR_NCT;
    static const std::string IMM_ATTR_OMISSION;
    static const std::string IMM_ATTR_SOLD;
    static const std::string IMM_ATTR_STATE;


    /****************************************************************************
     *                  DYNAMICPARAMS
     * **************************************************************************
     */

    /****************************************************************************
     *                  LASTAUTORELOAD
     * **************************************************************************
     */
    struct LastAutoReload_
    {
        int32_t     clh;
        uint32_t    gfile;
        int32_t     lpgf;
        uint32_t    nct;
        uint32_t    sup;
        int32_t     omission;
        uint32_t    alog;
        std::string opsrs;
        std::string odss;
        std::string odsl;
    };

    static const std::string IMM_LAR_OBJ_DN;
    static const std::string IMM_IMPL_LAR_NAME;
    static const std::string IMM_CLASS_LAR_NAME_;
    static const std::string IMM_CLASS_LAR_NAME;
    static const std::string IMM_ATTR_LAR_RDN;
    static const std::string IMM_ATTR_ALOG;
    static const std::string IMM_ATTR_GFILE;
    static const std::string IMM_ATTR_LPGF;
    static const std::string IMM_ATTR_ODSL;
    static const std::string IMM_ATTR_ODSS;
    static const std::string IMM_ATTR_OPSRS;
    static const std::string IMM_ATTR_LAR_CLH;
    static const std::string IMM_ATTR_LAR_SUP;
    static const std::string IMM_ATTR_LAR_NCT;
    static const std::string IMM_ATTR_LAR_OMISSION;


    /****************************************************************************
     *                  LASTAUTORELOAD
     * **************************************************************************
     */

    /**
     * @Brief Method to convert error code
     *
     * @param errorCode
     * @return error code string
     */
    static std::string errorCodeToString(BUParamsCommon::SPARAM_ERR errorCode);

private:
    //error code list
    static std::map<BUParamsCommon::SPARAM_ERR, std::string> errorCodeToStringList;
};


#endif /* BUPARAMSCOMMON_H_ */
