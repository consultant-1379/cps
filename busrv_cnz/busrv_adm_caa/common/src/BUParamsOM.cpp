/*
NAME
   File_name: BUParamsOM.cpp

COPYRIGHT Ericsson AB, Sweden 2012. All rights reserved.

   The Copyright to the computer program(s) herein is the property of Ericsson
   Utvecklings AB, Sweden.
   The program(s) may be used and/or copied only with the written permission from
   Ericsson Utvecklings AB or in accordance with the terms and conditions
   stipulated in the agreement/contract under which the program(s) have been
   supplied.

DESCRIPTION
   Object Manager (OM) for Non Classic & Cluster CPReload Params.

DOCUMENT NO
   190 89-CAA 109 1409

AUTHOR
   2012-04-20 by Quyen Dao

SEE ALSO


Revision history
----------------
2012-04-20 xquydao Created

*/

#include "BUParamsOM.h"
#include "Config.h"
#include "CPS_BUSRV_Trace.h"

#include <sstream>
#include <vector>

using namespace std;

const Config::CLUSTER g_cluster;

BUParamsOM::BUParamsOM() : m_initialized(false), m_lastErrorCode(0), m_lastErrorText("")
{
    //newTRACE((LOG_LEVEL_INFO, "BUParamsOM::BUParamsOM()", 0));

    // Init the OM
    this->init();
}

BUParamsOM::~BUParamsOM()
{
    //newTRACE((LOG_LEVEL_INFO, "BUParamsOM::~BUParamsOM()", 0));
    
    if (m_initialized)
    {
        m_omHandler.Finalize();
    }
    
    m_initialized = false;
}

bool BUParamsOM::init()
{
    //newTRACE((LOG_LEVEL_INFO, "BUParamsOM::init()", 0));
    
    m_initialized = false;

    ACS_CC_ReturnType result = m_omHandler.Init();

    if (result == ACS_CC_SUCCESS)
    {
        m_initialized = true;
    }
    else
    {
        m_lastErrorCode = m_omHandler.getInternalLastError();
        m_lastErrorText = m_omHandler.getInternalLastErrorText();
        //TRACE((LOG_LEVEL_ERROR, "Failed to init the OM: %d - %s", 0, m_lastErrorCode, m_lastErrorText.c_str()));
    }

    return m_initialized;
}

bool BUParamsOM::isInitialized() const
{
    return m_initialized;
}

bool BUParamsOM::finalize()
{
    newTRACE((LOG_LEVEL_INFO, "BUParamsOM::finalize()", 0));

    ACS_CC_ReturnType result = ACS_CC_SUCCESS;

    if (m_initialized)
    {
        result = m_omHandler.Finalize();

        if (result != ACS_CC_SUCCESS)
        {
            m_lastErrorCode = m_omHandler.getInternalLastError();
            m_lastErrorText = m_omHandler.getInternalLastErrorText();
            TRACE((LOG_LEVEL_ERROR, "Failed to finalize the OM: %d - %s", 0, m_lastErrorCode, m_lastErrorText.c_str()));
        }
    }

    m_initialized = false;

    return result == ACS_CC_SUCCESS;
}

bool BUParamsOM::createAndSyncBUParamObject()
{
    newTRACE((LOG_LEVEL_INFO, "BUParamsOM::createAndSyncBUParamObject()", 0));

    bool result = true;

    try
    {
        // Check If OM initialized
        if (!m_initialized)
        {
            TRACE((LOG_LEVEL_ERROR, "Can't create IMM object due to uninitialized OM", 0));

            m_lastErrorCode = BUParamsCommon::UNINITIALIZED_OM;
            m_lastErrorText = BUParamsCommon::errorCodeToString(BUParamsCommon::UNINITIALIZED_OM);

            return false;
        }

        // Check if system is single or multiple
        bool multi = Config::instance().isMultipleCP();
        bool isClassicCP = Config::instance().isClassicCP();

        // Check the system is Classic, One CP System or Multi CP System
        // Remove DualSidedCP object on Classic and One CP System
        if (isClassicCP || multi)
        {
            TRACE((LOG_LEVEL_INFO, "DualSidedCP object is not supported on Classic and MCP system", 0));

            m_lastErrorCode = BUParamsCommon::UNSUPPORTED_SYSTEM;
            m_lastErrorText = BUParamsCommon::errorCodeToString(BUParamsCommon::UNSUPPORTED_SYSTEM);

            // This is a permanent workaround; IO refused to implement the solution
            // it is intended to be as brief as possible
            // If the object is already deleted previously it will failed
            const string& ncdObjName = BUParamsCommon::cpTypeToObjectName(BUParamsCommon::CP_SINGLE);
            string dn = ncdObjName;
            dn += ",";
            dn += BUParamsCommon::getParentDN();
            if (m_omHandler.deleteObject(dn.c_str(),
            	ACS_APGCC_ONE) == ACS_CC_SUCCESS) {
            	TRACE((LOG_LEVEL_INFO, "BUParamsOM Workaround: Successfully remove <%s> object ", 0, ncdObjName.c_str()));
            }
            else {
            	TRACE((LOG_LEVEL_ERROR, "BUParamsOM Workaround: Failed to remove <%s> object ", 0, ncdObjName.c_str()));
            }
            
            // Continue the work for MCP; returning only for Classic CP
            if (isClassicCP)
                return false;
        }

        if (multi)
        {
            // Multi CP system then create 3 objects: 2 NonClassic (SPX1/2) + Cluster
            BUParamsCommon::IMM_CP_TYPE cp[3] = {BUParamsCommon::CP_SPX1, BUParamsCommon::CP_SPX2, BUParamsCommon::CP_CLUSTER};

            for (int i = 0; i < 3; ++i)
            {
                string objName = BUParamsCommon::cpTypeToObjectName(cp[i]);
                CPID cpId = BUParamsCommon::cpTypeToCPID(cp[i]);

                if (objName == "")
                {
                    TRACE((LOG_LEVEL_ERROR, "Failed to obtain object instance id of type %d", 0, cp[i]));

                    m_lastErrorCode = BUParamsCommon::OBJID_OBTAINING_FAILURE;
                    m_lastErrorText = BUParamsCommon::errorCodeToString(BUParamsCommon::OBJID_OBTAINING_FAILURE);

                    return false;
                }

                if (cp[i] == BUParamsCommon::CP_CLUSTER)
                {
                    result = this->createObject(objName, BUParamsCommon::IMM_CLN_CLUSTER);
                    if (result)
                    {
                        // Sync from file to IMM
                        BUParamsCommon::BUParam param;
                        param.ffr     = Config::instance().getSbcToReloadFromFFR(g_cluster);
                        param.sfr     = Config::instance().getSbcToReloadFromSFR(g_cluster);
                        param.supTime = Config::instance().getSupervisionTime(g_cluster);

                        TRACE((LOG_LEVEL_INFO, "Start to sync the object with IMM_CP_TYPE<%d>", 0, BUParamsCommon::CP_CLUSTER));
                        result = this->setBUParam(param, BUParamsCommon::CP_CLUSTER);
                    }
                }
                else
                {
                    result = this->createObject(objName, BUParamsCommon::IMM_CLN_NONCLASSIC);
                    if (result)
                    {
                        // Sync from file to IMM
                        BUParamsCommon::BUParam param;
                        param.ffr     = Config::instance().getSbcToReloadFromFFR(cpId);
                        param.sfr     = Config::instance().getSbcToReloadFromSFR(cpId);
                        param.supTime = Config::instance().getSupervisionTime(cpId);

                        TRACE((LOG_LEVEL_INFO, "Start to sync the object with IMM_CP_TYPE<%d>", 0, cp[i]));
                        result = this->setBUParam(param, cp[i]);
                    }
               }
            }
        }
        else
        {
            // Single CP system then create 1 object: NonClassic
            string objName = BUParamsCommon::cpTypeToObjectName(BUParamsCommon::CP_SINGLE);

            if (objName == "")
            {
                TRACE((LOG_LEVEL_ERROR, "Failed to obtain NonClassic object instance id", 0));

                m_lastErrorCode = BUParamsCommon::OBJID_OBTAINING_FAILURE;
                m_lastErrorText = BUParamsCommon::errorCodeToString(BUParamsCommon::OBJID_OBTAINING_FAILURE);

                return false;
            }

            // Create non classic object
            result = this->createObject(objName, BUParamsCommon::IMM_CLN_NONCLASSIC);
	    
            if (result)
            {
                // Sync from file to IMM
                BUParamsCommon::BUParam param;
                param.ffr     = Config::instance().getSbcToReloadFromFFR();
                param.sfr     = Config::instance().getSbcToReloadFromSFR();
                param.supTime = Config::instance().getSupervisionTime();

                TRACE((LOG_LEVEL_INFO, "Start to sync the object with IMM_CP_TYPE<%d>", 0, BUParamsCommon::CP_SINGLE));
                result = this->setBUParam(param, BUParamsCommon::CP_SINGLE);
            }
        }
    }
    catch (...)
    {
        TRACE((LOG_LEVEL_ERROR, "Failed to check the system type", 0));

        m_lastErrorCode = BUParamsCommon::SYSTYPE_CHECKING_FAILURE;
        m_lastErrorText = BUParamsCommon::errorCodeToString(BUParamsCommon::SYSTYPE_CHECKING_FAILURE);

        return false;
    }

    return result;
}

bool BUParamsOM::createObject(const string& instanceID, const string& className)
{
    newTRACE((LOG_LEVEL_INFO, "BUParamsOM::createObject(%s, %s)", 0, instanceID.c_str(), className.c_str()));

    // Check If OM initialized
    if (!m_initialized)
    {
        TRACE((LOG_LEVEL_ERROR, "Can't create IMM object due to uninitialized OM", 0));

        m_lastErrorCode = BUParamsCommon::UNINITIALIZED_OM;
        m_lastErrorText = BUParamsCommon::errorCodeToString(BUParamsCommon::UNINITIALIZED_OM);

        return false;
    }

    ACS_CC_ReturnType result;

    int numAttr = 4;

    // Define attribute list.
    vector<ACS_CC_ValuesDefinitionType> AttrList;
    ACS_CC_ValuesDefinitionType attribute[numAttr];
    void * value[1];

    // Fill the ID Attribute
    if (className == BUParamsCommon::IMM_CLN_CLUSTER) {
    	TRACE((LOG_LEVEL_TRACE, "setting ID Attribute for %s", 0, BUParamsCommon::IMM_ATTR_CLUSTER_ID.c_str()));
        attribute[0].attrName = (char*)BUParamsCommon::IMM_ATTR_CLUSTER_ID.c_str();
        attribute[0].attrType = ATTR_STRINGT;
        attribute[0].attrValuesNum = 1;
        value[0] = {(char*)instanceID.c_str()};
        attribute[0].attrValues = value;
    }
    else {
    	TRACE((LOG_LEVEL_TRACE, "setting ID Attribute for %s", 0, BUParamsCommon::IMM_ATTR_NONCLASSIC_ID.c_str()));
		attribute[0].attrName = (char*)BUParamsCommon::IMM_ATTR_NONCLASSIC_ID.c_str();
		attribute[0].attrType = ATTR_STRINGT;
		attribute[0].attrValuesNum = 1;
		value[0] = {(char*)instanceID.c_str()};
		attribute[0].attrValues = value;
    }

    // Fill the FFR Attribute
    u_int32 ffr = 0;
    attribute[1].attrName = (char*)BUParamsCommon::IMM_ATTR_FFR.c_str();
    attribute[1].attrType = ATTR_UINT32T;
    attribute[1].attrValuesNum = 1;
    void* valueFFR[1] = {&ffr};
    attribute[1].attrValues = valueFFR;

    // Fill the SFR Attribute
    u_int32 sfr = 0;
    attribute[2].attrName = (char*)BUParamsCommon::IMM_ATTR_SFR.c_str();
    attribute[2].attrType = ATTR_UINT32T;
    attribute[2].attrValuesNum = 1;
    void* valueSFR[1] = {&sfr};
    attribute[2].attrValues = valueSFR;

    // Fill the ST Attribute
    u_int32 st = 60;
    attribute[3].attrName = (char*)BUParamsCommon::IMM_ATTR_ST.c_str();
    attribute[3].attrType = ATTR_UINT32T;
    attribute[3].attrValuesNum = 1;
    void* valueST[1] = {&st};
    attribute[3].attrValues = valueST;

    // Add the attributes into the list
    for (int i = 0; i < numAttr; ++i)
    {
        AttrList.push_back(attribute[i]);
    }

    // Create Object for Non Classic class
    //TRACE(("OmHandler createObject with %s %s", 0, className.c_str(), BUParamsCommon::getParentDN().c_str()));
    result = m_omHandler.createObject(className.c_str(), BUParamsCommon::getParentDN().c_str(), AttrList);

    if (result == ACS_CC_SUCCESS)
    {
        TRACE((LOG_LEVEL_INFO, "Creation object %s completed", 0, instanceID.c_str()));
    }
    else
    {
        if (m_omHandler.getInternalLastError() == -14) // 14: ERR_ELEMENT_ALREADY_EXIST
        {
            TRACE((LOG_LEVEL_INFO, "Object %s exists", 0, instanceID.c_str()));
            result = ACS_CC_SUCCESS;
        }
        else
        {
            TRACE((LOG_LEVEL_ERROR, "Creation object %s failed with error: %d - %s", 0, instanceID.c_str(),
                   m_omHandler.getInternalLastError(), m_omHandler.getInternalLastErrorText()));

            m_lastErrorCode = m_omHandler.getInternalLastError();
            m_lastErrorText = m_omHandler.getInternalLastErrorText();
        }
    }

    return result == ACS_CC_SUCCESS;
}

bool BUParamsOM::getBUParam(BUParamsCommon::BUParam &param, BUParamsCommon::IMM_CP_TYPE cpType)
{
    newTRACE((LOG_LEVEL_INFO, "BUParamsOM::getBUParam(param, %d)", 0, cpType));
    
    // Check If OM initialized
    if (!m_initialized)
    {
        TRACE((LOG_LEVEL_ERROR, "Can't obtain the BUParam IMM object due to uninitialized OM", 0));

        m_lastErrorCode = BUParamsCommon::UNINITIALIZED_OM;
        m_lastErrorText = BUParamsCommon::errorCodeToString(BUParamsCommon::UNINITIALIZED_OM);

        return false;
    }

    ACS_CC_ReturnType result = ACS_CC_SUCCESS;    
    
    string objectName = BUParamsCommon::cpTypeToObjectName(cpType);
    
    if (objectName == "")
    {
        m_lastErrorCode = BUParamsCommon::OBJID_OBTAINING_FAILURE;
        m_lastErrorText = BUParamsCommon::errorCodeToString(BUParamsCommon::OBJID_OBTAINING_FAILURE);
        TRACE((LOG_LEVEL_ERROR, m_lastErrorText.c_str(), 0));
        
        return false;
    }
    
    string objectDN = objectName + "," + BUParamsCommon::getParentDN();

    ACS_APGCC_ImmAttribute ffrAttr;
	ACS_APGCC_ImmAttribute sfrAttr;
	ACS_APGCC_ImmAttribute supTimeAttr;
 
    ffrAttr.attrName = (char*) BUParamsCommon::IMM_ATTR_FFR.c_str();
    sfrAttr.attrName = (char*) BUParamsCommon::IMM_ATTR_SFR.c_str();
    supTimeAttr.attrName = (char*) BUParamsCommon::IMM_ATTR_ST.c_str();

    vector<ACS_APGCC_ImmAttribute *> buParamAttrs;
    buParamAttrs.push_back(&ffrAttr);
    buParamAttrs.push_back(&sfrAttr);
    buParamAttrs.push_back(&supTimeAttr);
    
    result = m_omHandler.getAttribute(objectDN.c_str(), buParamAttrs);
    
    if (result != ACS_CC_SUCCESS)
    {
        m_lastErrorCode = m_omHandler.getInternalLastError();
        m_lastErrorText = m_omHandler.getInternalLastErrorText();
        TRACE((LOG_LEVEL_ERROR, "Failed to get BU Param attribute: %d - %s", 0, m_lastErrorCode, m_lastErrorText.c_str()));
    }
    else
    {
        param.ffr = *(reinterpret_cast<u_int32 *> (ffrAttr.attrValues[0]));
        param.sfr = *(reinterpret_cast<u_int32 *> (sfrAttr.attrValues[0]));
        param.supTime = *(reinterpret_cast<u_int32 *> (supTimeAttr.attrValues[0]));
    }

    return result == ACS_CC_SUCCESS;
}

bool BUParamsOM::setBUParam(const BUParamsCommon::BUParam &buParam, BUParamsCommon::IMM_CP_TYPE cpType)
{
    newTRACE((LOG_LEVEL_INFO, "BUParamsOM::setBUParam(param, %d)", 0, cpType));

    // Check If OM initialized
    if (!m_initialized)
    {
        TRACE((LOG_LEVEL_ERROR, "Can't set the BUParam object due to uninitialized OM", 0));

        m_lastErrorCode = BUParamsCommon::UNINITIALIZED_OM;
        m_lastErrorText = BUParamsCommon::errorCodeToString(BUParamsCommon::UNINITIALIZED_OM);

        return false;
    }

    ACS_CC_ReturnType result = ACS_CC_SUCCESS;

    string objectName = BUParamsCommon::cpTypeToObjectName(cpType);
    BUParamsCommon::BUParam param = buParam;

    if (objectName == "")
    {
        m_lastErrorCode = BUParamsCommon::OBJID_OBTAINING_FAILURE;
        m_lastErrorText = BUParamsCommon::errorCodeToString(BUParamsCommon::OBJID_OBTAINING_FAILURE);
        TRACE((LOG_LEVEL_ERROR, m_lastErrorText.c_str(), 0));

        return false;
    }

    string objectDN = objectName + "," + BUParamsCommon::getParentDN();
    string transactionName("modifyCPReloadParam");
    ACS_CC_ImmParameter parToModify;

    do
    {
        // FFR
        parToModify.attrName = (char*) BUParamsCommon::IMM_ATTR_FFR.c_str();
        parToModify.attrType = ATTR_UINT32T;
        parToModify.attrValuesNum = 1;
        void* valueFFR[1] = {&param.ffr};
        parToModify.attrValues = valueFFR;

        result = m_omHandler.modifyAttribute(objectDN.c_str(), &parToModify, transactionName);

        if (result != ACS_CC_SUCCESS)
        {
            TRACE((LOG_LEVEL_ERROR, "Failed to modified attribute %s, %s - %u", 0, objectName.c_str(), parToModify.attrName, param.ffr));

            break;
        }

        // SFR
        parToModify.attrName = (char*) BUParamsCommon::IMM_ATTR_SFR.c_str();
        parToModify.attrType = ATTR_UINT32T;
        parToModify.attrValuesNum = 1;
        void* valueSFR[1] = {&param.sfr};
        parToModify.attrValues = valueSFR;

        result = m_omHandler.modifyAttribute(objectDN.c_str(), &parToModify, transactionName);

        if (result != ACS_CC_SUCCESS)
        {
            TRACE((LOG_LEVEL_ERROR, "Failed to modified attribute %s - %u", 0, parToModify.attrName, param.sfr));

            break;
        }

        // ST
        parToModify.attrName = (char*) BUParamsCommon::IMM_ATTR_ST.c_str();
        parToModify.attrType = ATTR_UINT32T;
        parToModify.attrValuesNum = 1;
        void* valueST[1] = {&param.supTime};
        parToModify.attrValues = valueST;

        result = m_omHandler.modifyAttribute(objectDN.c_str(), &parToModify, transactionName);

        if (result != ACS_CC_SUCCESS)
        {
            TRACE((LOG_LEVEL_ERROR, "Failed to modified attribute %s - %u", 0, parToModify.attrName, param.supTime));

            break;
        }
    }
    while (false);

    if (result != ACS_CC_SUCCESS)
    {
        m_omHandler.getExitCode(transactionName, m_lastErrorCode, m_lastErrorText);
        TRACE((LOG_LEVEL_ERROR, "Exit code: %d - %s", 0, m_lastErrorCode, m_lastErrorText.c_str()));

        TRACE((LOG_LEVEL_ERROR, "Internal Error: %d - %s", 0, m_omHandler.getInternalLastError(), m_omHandler.getInternalLastErrorText()));
        m_lastErrorCode = m_omHandler.getInternalLastError();
        m_lastErrorText = m_omHandler.getInternalLastErrorText();

        if (m_omHandler.resetRequest(transactionName) != ACS_CC_SUCCESS)
        {
            TRACE((LOG_LEVEL_ERROR, "Failed to reset the transaction: %s", 0, transactionName.c_str()));
        }
    }
    else
    {
        result = m_omHandler.applyRequest(transactionName);

        if (result != ACS_CC_SUCCESS)
        {
            TRACE((LOG_LEVEL_ERROR, "Failed to apply the transaction: %s", 0, transactionName.c_str()));
            TRACE((LOG_LEVEL_ERROR, "Internal Error: %d - %s", 0, m_omHandler.getInternalLastError(), m_omHandler.getInternalLastErrorText()));
            m_lastErrorCode = m_omHandler.getInternalLastError();
            m_lastErrorText = m_omHandler.getInternalLastErrorText();

            if (m_omHandler.resetRequest(transactionName) != ACS_CC_SUCCESS)
            {
                TRACE((LOG_LEVEL_ERROR, "Failed to reset the transaction: %s after apply failure", 0, transactionName.c_str()));
            }
        }
    }

    return result == ACS_CC_SUCCESS;
}

const string& BUParamsOM::getLastErrorText()
{
    newTRACE((LOG_LEVEL_TRACE, "BUParamsOM::getLastErrorText()", 0));
    
    return m_lastErrorText;
}

int BUParamsOM::getLastError()
{
    newTRACE((LOG_LEVEL_TRACE, "BUParamsOM::getLastError()", 0));
    
    return m_lastErrorCode;
}

#if 0
// This function must not throw any exception or logging anything to the AP event log
// The calling protocol will take appropriate action in case of error.
BUParamsOM::RC_BUP BUParamsOM::getBUParam(BUParamsCommon::BUParam &param, CPID cpId)
{
    newTRACE((LOG_LEVEL_INFO, "BUParamsOM::getBUParam(param, CPID)", 0));

    // Default values will be used in case of any failure
    param.reset();

    // Checking for CPCluster, SPX or One CP System
    bool isMultipleCP = Config::instance().isMultipleCP();
    ostringstream ss;
    if (isMultipleCP) {
    	// Check if CLUSTER
    	if (cpId <= BUParamsCommon::CPID_CLUSTERCP_MAX) {
    		ss << BUParamsCommon::IMM_ATTR_CLUSTER_ID;
    		ss << "=" << BUParamsCommon::CPNAME_CLUSTER;
    	}
    	// If not CLUSTER, it can only be SPXes
    	else {
			ss << BUParamsCommon::IMM_ATTR_NONCLASSIC_ID << "=";
			if (cpId == BUParamsCommon::CPID_SPX1) {
				ss << BUParamsCommon::CPNAME_SPX1;
			}
			else if (cpId == BUParamsCommon::CPID_SPX2) {
				ss << BUParamsCommon::CPNAME_SPX2;
			}
			else {
				// This is configuration error
				// Should do nothing; Calling function decides what to do
				return RC_CONFIG_ERROR;
			}
    	}
    }
    else if (cpId == 0xFFFF) {
    	ss << BUParamsCommon::IMM_ATTR_NONCLASSIC_ID << "=";
    	ss << BUParamsCommon::CPNAME_SPX1;
    }
    else {
    	// This is configuration error
    	// Should do nothing; Calling function decides what to do
    	return RC_CONFIG_ERROR;
    }

    ss << "," << BUParamsCommon::IMM_DN_PARENT;
    string objectDN = ss.str();

    ACS_APGCC_ImmAttribute ffrAttr;
	ACS_APGCC_ImmAttribute sfrAttr;
	ACS_APGCC_ImmAttribute supTimeAttr;

    ffrAttr.attrName = BUParamsCommon::IMM_ATTR_FFR;
    sfrAttr.attrName = BUParamsCommon::IMM_ATTR_SFR;
    supTimeAttr.attrName = BUParamsCommon::IMM_ATTR_ST;

    vector<ACS_APGCC_ImmAttribute *> buParamAttrs;
    buParamAttrs.push_back(&ffrAttr);
    buParamAttrs.push_back(&sfrAttr);
    buParamAttrs.push_back(&supTimeAttr);

    TRACE((LOG_LEVEL_INFO, "BUParamsOM::getBUParam(); Object DN is <%s>", 0, objectDN.c_str()));

   RC_BUP result;
   if (m_omHandler.getAttribute(objectDN.c_str(), buParamAttrs) != ACS_CC_SUCCESS)
    {
        m_lastErrorCode = m_omHandler.getInternalLastError();
        m_lastErrorText = m_omHandler.getInternalLastErrorText();
        TRACE((LOG_LEVEL_ERROR, "Failed to get BU Param attribute: %d - %s", 0, m_lastErrorCode, m_lastErrorText.c_str()));
        result = RC_IMM_ERROR;
    }
    else
    {
        param.ffr = *(reinterpret_cast<u_int32 *> (ffrAttr.attrValues[0]));
        param.sfr = *(reinterpret_cast<u_int32 *> (sfrAttr.attrValues[0]));
        param.supTime = *(reinterpret_cast<u_int32 *> (supTimeAttr.attrValues[0]));
        result = RC_SUCCESS;
    }

    return result;
}
#endif

