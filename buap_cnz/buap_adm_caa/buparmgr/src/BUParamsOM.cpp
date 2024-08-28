/*
 * COPYRIGHT Ericsson Utvecklings AB, Sweden 2012.
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
 *  BUParamsOM.cpp
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

#include "BUParamsOM.h"
#include "CPS_BUAP_Trace.h"
#include "CPS_BUAP_EventReporter.h"
#include "CPS_BUAP_Events.h"

//#include "ACS_CS_API.h"             // provides getCPId()

#include <sstream>

using namespace std;

//**********************************************************************
//  BUParamsOM()
//**********************************************************************
BUParamsOM::BUParamsOM(): m_initialized(false)
{
    newTRACE((LOG_LEVEL_INFO, "BUParamsOM::BUParamsOM()", 0));
    // Initialize interaction with IMM
    this->init();
}

//**********************************************************************
//  ~BUParamsOM()
//**********************************************************************
BUParamsOM::~BUParamsOM()
{
    newTRACE((LOG_LEVEL_INFO, "BUParamsOM::~BUParamsOM()", 0));

    // Finalize the interaction with IMM
    if (m_initialized)
    {
        m_omHandler.Finalize();
    }

    m_initialized = false;
}

//**********************************************************************
//  createInitialObjects()
//**********************************************************************
bool BUParamsOM::createInitialObjects()
{
    newTRACE((LOG_LEVEL_INFO, "BUParamsOM::createInitialObjects()", 0));

    if (!m_initialized)
    {
        return false;
    }

    // Create Object for Classic class
    if ((!this->createClassicObject()) ||
        (!this->createStaticObject()) ||
        (!this->createDynamicObject()) ||
        (!this->createCmdLogRefObject()) ||
        (!this->createDelayMeasuresObject()) ||
        (!this->createLastAutoReloadObject()))
    {
        TRACE((LOG_LEVEL_WARN, "Create objects for Classic class failed", 0));
        return false;
    }

    return true;
}

//**********************************************************************
//  finalize()
//**********************************************************************
void BUParamsOM::finalize()
{
    newTRACE((LOG_LEVEL_INFO, "BUParamsOM::finalize()", 0));

    if (m_initialized)
    {
        m_omHandler.Finalize();
    }

    m_initialized = false;
}

//**********************************************************************
//  isInitialized()
//**********************************************************************
bool BUParamsOM::isInitialized() const
{
    return m_initialized;
}

//**********************************************************************
//  createClassicObject()
//**********************************************************************
bool BUParamsOM::createClassicObject()
{
    newTRACE(("BUParamsOM::createClassicObject()", 0));

    int numAttr = 6;
    //int isValid = 0;
    ACS_CC_ReturnType returnCode = ACS_CC_FAILURE;

    // Define attribute list.
    vector<ACS_CC_ValuesDefinitionType> AttrList;
    ACS_CC_ValuesDefinitionType *attribute;
    attribute = new ACS_CC_ValuesDefinitionType[numAttr];

    //char* rdnValue_temp = const_cast<char *>("classicCpId=1");
    char* rdnValue_temp = const_cast<char *>(BUParamsCommon::IMM_CLASSIC_OBJ_DN.c_str());

    //TODO : use getCPId() to get the Id of the Classic class.
#if 0
    ACS_CS_API_CP *CPNameVer = ACS_CS_API::createCPInstance();
    ACS_CS_API_Name CPName("CP1");
    CPID cpId;

    ACS_CS_API_NS::CS_API_Result result = CPNameVer->getCPId(CPName, cpId);

    ACS_CS_API::deleteCPInstance(CPNameVer);

    if (result == ACS_CS_API_NS::Result_Success)
    {
        stringstream ss;
        ss << "classicId=" << cpId;

        TRACE(("getCPId returns id = %d", 0, cpId));

        rdnValue_temp = const_cast<char *>(ss.str().c_str());
    }
    else
    {
        TRACE(("getCPId() failed.", 0));

        return false;
    }
#endif


    if (attribute != NULL)
    {
        /*Fill the rdn Attribute */
        attribute[0].attrName = (char*)BUParamsCommon::IMM_ATTR_CLASSIC_RDN.c_str();
        attribute[0].attrType = ATTR_STRINGT;
        attribute[0].attrValuesNum = 1;
        void* valueRDN[1]={reinterpret_cast<void*>(rdnValue_temp)};
        attribute[0].attrValues = valueRDN;

        rdnValue_temp = const_cast<char *>(BUParamsCommon::IMM_DP_OBJ_DN.c_str());

        /*Fill the dynamicParams Attribute */
        attribute[1].attrName = (char*)BUParamsCommon::IMM_ATTR_DYNAMIC.c_str();
        attribute[1].attrType = ATTR_NAMET;
        attribute[1].attrValuesNum = 1;
        void* valueDynamic[1]={reinterpret_cast<void*>(rdnValue_temp)};
        attribute[1].attrValues = valueDynamic;

        rdnValue_temp = const_cast<char *>(BUParamsCommon::IMM_CMDLOG_OBJ_DN.c_str());

        /*Fill the commandLogReferences Attribute */
        attribute[2].attrName = (char*)BUParamsCommon::IMM_ATTR_CMDLOGREF.c_str();
        attribute[2].attrType = ATTR_NAMET;
        attribute[2].attrValuesNum = 1;
        void* valueCmdLog[1]={reinterpret_cast<void*>(rdnValue_temp)};
        attribute[2].attrValues = valueCmdLog;

        rdnValue_temp = const_cast<char *>(BUParamsCommon::IMM_DM_OBJ_DN.c_str());

        /*Fill the delayedMeasures Attribute */
        attribute[3].attrName = (char*)BUParamsCommon::IMM_ATTR_DMEASURES.c_str();
        attribute[3].attrType = ATTR_NAMET;
        attribute[3].attrValuesNum = 1;
        void* valueDMeasures[1]={reinterpret_cast<void*>(rdnValue_temp)};
        attribute[3].attrValues = valueDMeasures;

        rdnValue_temp = const_cast<char *>(BUParamsCommon::IMM_LAR_OBJ_DN.c_str());

        /*Fill the lastAutoReload Attribute */
        attribute[4].attrName = (char*)BUParamsCommon::IMM_ATTR_LASTRELOAD.c_str();
        attribute[4].attrType = ATTR_NAMET;
        attribute[4].attrValuesNum = 1;
        void* valueLastReload[1]={reinterpret_cast<void*>(rdnValue_temp)};
        attribute[4].attrValues = valueLastReload;

        rdnValue_temp = const_cast<char *>(BUParamsCommon::IMM_SP_OBJ_DN.c_str());

        /*Fill the staticParams Attribute */
        attribute[5].attrName = (char*)BUParamsCommon::IMM_ATTR_STATIC.c_str();
        attribute[5].attrType = ATTR_NAMET;
        attribute[5].attrValuesNum = 1;
        void* valueStatic[1]={reinterpret_cast<void*>(rdnValue_temp)};
        attribute[5].attrValues = valueStatic;

#if 0
        /*Fill the isStaticParamsValid Attribute */
        attribute[6].attrName = (char*)BUParamsCommon::IMM_ATTR_STATIC_VALID.c_str();
        attribute[6].attrType = ATTR_INT32T;
        attribute[6].attrValuesNum = 1;
        void* IsStaticValid[1]={reinterpret_cast<void*>(&isValid)};
        attribute[6].attrValues = IsStaticValid;

        /*Fill the isDynamicParamsValid Attribute */
        attribute[7].attrName = (char*)BUParamsCommon::IMM_ATTR_DYNAMIC_VALID.c_str();
        attribute[7].attrType = ATTR_INT32T;
        attribute[7].attrValuesNum = 1;
        void* IsDynamicValid[1]={reinterpret_cast<void*>(&isValid)};
        attribute[7].attrValues = IsDynamicValid;

        /*Fill the isDelayedMeasuresValid Attribute */
        attribute[8].attrName = (char*)BUParamsCommon::IMM_ATTR_DMEASURES_VALID.c_str();
        attribute[8].attrType = ATTR_INT32T;
        attribute[8].attrValuesNum = 1;
        void* IsDMValid[1]={reinterpret_cast<void*>(&isValid)};
        attribute[8].attrValues = IsDMValid;

        /*Fill the isCommandLogReferencesValid Attribute */
        attribute[9].attrName = (char*)BUParamsCommon::IMM_ATTR_CMDLOGREF_VALID.c_str();
        attribute[9].attrType = ATTR_INT32T;
        attribute[9].attrValuesNum = 1;
        void* IsCLRValid[1]={reinterpret_cast<void*>(&isValid)};
        attribute[9].attrValues = IsCLRValid;

        /*Fill the isLastAutoReloadValid Attribute */
        attribute[10].attrName = (char*)BUParamsCommon::IMM_ATTR_LASTRELOAD_VALID.c_str();
        attribute[10].attrType = ATTR_INT32T;
        attribute[10].attrValuesNum = 1;
        void* IsLARValid[1]={reinterpret_cast<void*>(&isValid)};
        attribute[10].attrValues = IsLARValid;
#endif

        // Add the attributes into the list
        for(int i = 0; i < numAttr; i++)
        {
            AttrList.push_back(attribute[i]);
        }

        // Create Object for Classic class
        returnCode = m_omHandler.createObject(BUParamsCommon::IMM_CLASSIC_CLASS_NAME.c_str(),
                                                NULL, AttrList);

        if ((returnCode == ACS_CC_SUCCESS) || (m_omHandler.getInternalLastError() == -14))
        {
            //TRACE(("Creation object %s completed\n", 0, BUParamsCommon::IMM_CLASSIC_CLASS_NAME.c_str()));
            returnCode = ACS_CC_SUCCESS;
        }
        else
        {
            TRACE((LOG_LEVEL_ERROR, "error - Creation object %s failed with error: %d - %s\n", 0,
                    BUParamsCommon::IMM_CLASSIC_CLASS_NAME.c_str(),
                    m_omHandler.getInternalLastError(), m_omHandler.getInternalLastErrorText()));
        }
    }
    // Remove the attribute pointer when finishing
    if (attribute)
    {
        delete [] attribute;
        attribute = 0;
    }

    return (returnCode == ACS_CC_SUCCESS);

}


//**********************************************************************
//  createStaticObject()
//**********************************************************************
bool BUParamsOM::createStaticObject()
{
    newTRACE(("BUParamsOM::createStaticObject()", 0));

    int numAttr = 9;
    int clh = 1;
    int incl1 = 0;
    int incl2 = 0;
    int ntcz = 10;
    int sup = 60;
    char* incla = const_cast<char *>("1990-01-01");

    ACS_CC_ReturnType returnCode = ACS_CC_FAILURE;

    // Define attribute list.
    vector<ACS_CC_ValuesDefinitionType> AttrList;
    ACS_CC_ValuesDefinitionType *attribute;
    attribute = new ACS_CC_ValuesDefinitionType[numAttr];

    //const string IMM_ATTR_DP_RDN_temp = "id";
    char* rdnValue_temp = const_cast<char *>("id=staticParams_0");

    if (attribute != NULL)
    {
        /*Fill the id Attribute */
        attribute[0].attrName = (char*)BUParamsCommon::IMM_ATTR_ID.c_str();
        attribute[0].attrType = ATTR_STRINGT;
        attribute[0].attrValuesNum = 1;
        void* valueId[1]={reinterpret_cast<void*>(rdnValue_temp)};
        attribute[0].attrValues = valueId;

        /*Fill the Command Log handling Mode Attribute */
        attribute[1].attrName = (char*)BUParamsCommon::IMM_ATTR_CLH.c_str();
        attribute[1].attrType = ATTR_INT32T;
        attribute[1].attrValuesNum = 1;
        void* valueCLH[1]={reinterpret_cast<void*>(&clh)};
        attribute[1].attrValues = valueCLH;


        /*Fill the incl1 Attribute */
        attribute[2].attrName = (char*)BUParamsCommon::IMM_ATTR_INCL1.c_str();
        attribute[2].attrType = ATTR_INT32T;
        attribute[2].attrValuesNum = 1;
        void* valueINCL1[1]={reinterpret_cast<void*>(&incl1)};
        attribute[2].attrValues = valueINCL1;

        /*Fill the incl2 Attribute */
        attribute[3].attrName = (char*)BUParamsCommon::IMM_ATTR_INCL2.c_str();
        attribute[3].attrType = ATTR_UINT32T;
        attribute[3].attrValuesNum = 1;
        void* valueINCL2[1]={reinterpret_cast<void*>(&incl2)};
        attribute[3].attrValues = valueINCL2;

        /*Fill the incla Attribute */
        attribute[4].attrName = (char*)BUParamsCommon::IMM_ATTR_INCLA.c_str();
        attribute[4].attrType = ATTR_STRINGT;
        attribute[4].attrValuesNum = 1;
        void* valueINCLA[1]={reinterpret_cast<void*>(incla)};
        attribute[4].attrValues = valueINCLA;

        /*Fill the LOAZ Attribute */
        attribute[5].attrName = (char*)BUParamsCommon::IMM_ATTR_LOAZ.c_str();
        attribute[5].attrType = ATTR_INT32T;
        attribute[5].attrValuesNum = 1;
        void* valueLOAZ[1]={reinterpret_cast<void*>(&incl1)};
        attribute[5].attrValues = valueLOAZ;


        /*Fill the NTAZ Attribute */
        attribute[6].attrName = (char*)BUParamsCommon::IMM_ATTR_NTAZ.c_str();
        attribute[6].attrType = ATTR_UINT32T;
        attribute[6].attrValuesNum = 1;
        void* valueNTAZ[1]={reinterpret_cast<void*>(&incl1)};
        attribute[6].attrValues = valueNTAZ;


        /*Fill the NTCZ Attribute */
        attribute[7].attrName = (char*)BUParamsCommon::IMM_ATTR_NTCZ.c_str();
        attribute[7].attrType = ATTR_UINT32T;
        attribute[7].attrValuesNum = 1;
        void* valueNTCZ[1]={reinterpret_cast<void*>(&ntcz)};
        attribute[7].attrValues = valueNTCZ;


        /*Fill the SUP Attribute */
        attribute[8].attrName = (char*)BUParamsCommon::IMM_ATTR_SUP.c_str();
        attribute[8].attrType = ATTR_UINT32T;
        attribute[8].attrValuesNum = 1;
        void* valueSUP[1]={reinterpret_cast<void*>(&sup)};
        attribute[8].attrValues = valueSUP;

        // Add the attributes into the list
        for(int i = 0; i < numAttr; i++)
        {
            AttrList.push_back(attribute[i]);
        }

        // Create Object for Static class
        returnCode = m_omHandler.createObject(BUParamsCommon::IMM_CLASS_SP_NAME.c_str(),
                                    BUParamsCommon::IMM_CLASSIC_OBJ_DN.c_str(), AttrList);

        if((returnCode == ACS_CC_SUCCESS) || (m_omHandler.getInternalLastError() == -14))
        {
            //TRACE(("Creation object %s completed\n", 0, BUParamsCommon::IMM_CLASS_SP_NAME.c_str()));
            returnCode = ACS_CC_SUCCESS;
        }
        else
        {
            TRACE((LOG_LEVEL_ERROR, "error - Creation object %s failed with error: %d - %s\n", 0,
                    BUParamsCommon::IMM_CLASS_SP_NAME.c_str(),
                    m_omHandler.getInternalLastError(), m_omHandler.getInternalLastErrorText()));
        }
    }

    if (attribute)
    {
        delete [] attribute;
        attribute = 0;
    }

    return (returnCode == ACS_CC_SUCCESS);
}

//**********************************************************************
//  createDynamicObject()
//**********************************************************************
bool BUParamsOM::createDynamicObject()
{
    newTRACE(("BUParamsOM::createDynamicObject()", 0));

    ACS_CC_ReturnType returnCode = ACS_CC_FAILURE;

    // Define the default values
    int numAttr = 11;
    uint32_t stateValue = 0;
    uint32_t ctaValue = 0;
    uint32_t cffrValue = 0;
    uint32_t csfrValue = 0;
    uint32_t soldValue = 0;
    uint32_t foldValue = 0;
    uint32_t nctValue = 0;
    uint32_t lpfValue = 0;
    uint32_t omissionValue = 0;
    char* expValue = const_cast<char *>("0000-00-00 00:00");

    /*The list of attributes*/
    vector<ACS_CC_ValuesDefinitionType> AttrList;
    ACS_CC_ValuesDefinitionType *attribute;

    attribute = new ACS_CC_ValuesDefinitionType[numAttr];

    //const string IMM_ATTR_DP_RDN_temp = "id";
    char* rdnValue_temp = const_cast<char *>("id=dynamicParams_0");

    if (attribute != NULL)
    {
        /*Fill the id Attribute */
        attribute[0].attrName = (char*)BUParamsCommon::IMM_ATTR_ID.c_str();
        attribute[0].attrType = ATTR_STRINGT;
        attribute[0].attrValuesNum = 1;
        void* valueId[1]={reinterpret_cast<void*>(rdnValue_temp)};
        attribute[0].attrValues = valueId;

        /*Fill the attribute attrState*/
        attribute[1].attrName = (char*)BUParamsCommon::IMM_ATTR_STATE.c_str();
        attribute[1].attrType = ATTR_INT32T;
        attribute[1].attrValuesNum = 1;
        void* valueState[1] = { reinterpret_cast<void*>(&stateValue) };
        attribute[1].attrValues = valueState;

        /*Fill the attribute attrCTA*/
        attribute[2].attrName = (char*)BUParamsCommon::IMM_ATTR_CTA.c_str();
        attribute[2].attrType = ATTR_UINT32T;
        attribute[2].attrValuesNum = 1;
        void* valueCTA[1] = { reinterpret_cast<void*>(&ctaValue) };
        attribute[2].attrValues = valueCTA;

        /*Fill the attribute attrCFFR*/
        attribute[3].attrName = (char*)BUParamsCommon::IMM_ATTR_CFFR.c_str();
        attribute[3].attrType = ATTR_UINT32T;
        attribute[3].attrValuesNum = 1;
        void* valueCFFR[1] = { reinterpret_cast<void*>(&cffrValue) };
        attribute[3].attrValues = valueCFFR;

        /*Fill the attribute attrCSFR*/
        attribute[4].attrName = (char*)BUParamsCommon::IMM_ATTR_CSFR.c_str();
        attribute[4].attrType = ATTR_UINT32T;
        attribute[4].attrValuesNum = 1;
        void* valueCSFR[1] = { reinterpret_cast<void*>(&csfrValue) };
        attribute[4].attrValues = valueCSFR;

        /*Fill the attribute attribute[6]*/
        attribute[5].attrName = (char*)BUParamsCommon::IMM_ATTR_FOLD.c_str();
        attribute[5].attrType = ATTR_UINT32T;
        attribute[5].attrValuesNum = 1;
        void* valueFOLD[1] = { reinterpret_cast<void*>(&foldValue) };
        attribute[5].attrValues = valueFOLD;

        /*Fill the attribute attribute[7]*/
        attribute[6].attrName = (char*)BUParamsCommon::IMM_ATTR_SOLD.c_str();
        attribute[6].attrType = ATTR_UINT32T;
        attribute[6].attrValuesNum = 1;
        void* valueSOLD[1] = { reinterpret_cast<void*>(&soldValue) };
        attribute[6].attrValues = valueSOLD;

        /*Fill the attribute attribute[8]*/
        attribute[7].attrName = (char*)BUParamsCommon::IMM_ATTR_NCT.c_str();
        attribute[7].attrType = ATTR_UINT32T;
        attribute[7].attrValuesNum = 1;
        void* valueNCT[1] = { reinterpret_cast<void*>(&nctValue) };
        attribute[7].attrValues = valueNCT;

        /*Fill the attribute attribute[9]*/
        attribute[8].attrName = (char*)BUParamsCommon::IMM_ATTR_LPF.c_str();
        attribute[8].attrType = ATTR_UINT32T;
        attribute[8].attrValuesNum = 1;
        void* valueLPF[1] = { reinterpret_cast<void*>(&lpfValue) };
        attribute[8].attrValues = valueLPF;

        /*Fill the attribute attribute[10]*/
        attribute[9].attrName = (char*)BUParamsCommon::IMM_ATTR_OMISSION.c_str();
        attribute[9].attrType = ATTR_INT32T;
        attribute[9].attrValuesNum = 1;
        void* valueOmission[1] = { reinterpret_cast<void*>(&omissionValue) };
        attribute[9].attrValues = valueOmission;

        /*Fill the attribute attribute[11]*/
        attribute[10].attrName = (char*)BUParamsCommon::IMM_ATTR_EXP.c_str();
        attribute[10].attrType = ATTR_STRINGT;
        attribute[10].attrValuesNum = 1;
        void* valueEXP[1] = { reinterpret_cast<void*>(expValue) };
        attribute[10].attrValues = valueEXP;

        // Add the attributes into the list
        for(int i = 0; i < numAttr; i++)
        {
            AttrList.push_back(attribute[i]);
        }

        // Create Object for Dynamic class
        returnCode = m_omHandler.createObject(BUParamsCommon::IMM_CLASS_DP_NAME.c_str(),
                                    BUParamsCommon::IMM_CLASSIC_OBJ_DN.c_str(), AttrList);

        if((returnCode == ACS_CC_SUCCESS) || (m_omHandler.getInternalLastError() == -14))
        {
            //TRACE(("Creation object %s completed\n", 0, BUParamsCommon::IMM_CLASS_DP_NAME.c_str()));
            returnCode = ACS_CC_SUCCESS;
        }
        else
        {
            TRACE((LOG_LEVEL_ERROR, "error - Creation object %s failed with error: %d - %s\n", 0,
                    BUParamsCommon::IMM_CLASS_DP_NAME.c_str(),
                    m_omHandler.getInternalLastError(), m_omHandler.getInternalLastErrorText()));
        }
    }

    if (attribute)
    {
        delete [] attribute;
        attribute = 0;
    }

    return (returnCode == ACS_CC_SUCCESS);
}

//**********************************************************************
//  createCmdLogRefObject()
//**********************************************************************
bool BUParamsOM::createCmdLogRefObject()
{
    newTRACE(("BUParamsOM::createCmdLogRefObject()", 0));

    ACS_CC_ReturnType returnCode = ACS_CC_FAILURE;

    // Define the default values
    int numAttr = 4;
    uint32_t currentValue = 0;
    uint32_t mainValue = 0;
    uint32_t preparedValue = 0;
    //const string IMM_ATTR_DP_RDN_temp = "id";
    char* rdnValue = const_cast<char *>("id=commandLogReferences_0");
    //int isValidValue = 0;

    vector<ACS_CC_ValuesDefinitionType> AttrList;
    ACS_CC_ValuesDefinitionType *attribute;

    attribute = new ACS_CC_ValuesDefinitionType[numAttr];


    if (attribute != NULL)
    {
        /*Fill the id Attribute */
        attribute[0].attrName = (char*)BUParamsCommon::IMM_ATTR_ID.c_str();
        attribute[0].attrType = ATTR_STRINGT;
        attribute[0].attrValuesNum = 1;
        void* valueRDN[1]={reinterpret_cast<void*>(rdnValue)};
        attribute[0].attrValues = valueRDN;


        /*Fill the attribute attribute[1]*/
        attribute[1].attrName = (char*)BUParamsCommon::IMM_ATTR_CURRENT.c_str();
        attribute[1].attrType = ATTR_UINT32T;
        attribute[1].attrValuesNum = 1;
        void* valueCurrent[1] = { reinterpret_cast<void*>(&currentValue) };
        attribute[1].attrValues = valueCurrent;

        /*Fill the attribute attribute[2]*/
        attribute[2].attrName = (char*)BUParamsCommon::IMM_ATTR_MAIN.c_str();
        attribute[2].attrType = ATTR_UINT32T;
        attribute[2].attrValuesNum = 1;
        void* valueMain[1] = { reinterpret_cast<void*>(&mainValue) };
        attribute[2].attrValues = valueMain;

        /*Fill the attribute attribute[3]*/
        attribute[3].attrName = (char*)BUParamsCommon::IMM_ATTR_PREPARED.c_str();
        attribute[3].attrType = ATTR_UINT32T;
        attribute[3].attrValuesNum = 1;
        void* valuePreapred[1] = { reinterpret_cast<void*>(&preparedValue) };
        attribute[3].attrValues = valuePreapred;

        // Add the attributes into the list
        for(int i = 0; i < numAttr; i++)
        {
            AttrList.push_back(attribute[i]);
        }

        // Create Object for CmdLogReference class
        returnCode = m_omHandler.createObject(BUParamsCommon::IMM_CLASS_CMDLOG_NAME.c_str(),
                                    BUParamsCommon::IMM_CLASSIC_OBJ_DN.c_str(), AttrList);

        if((returnCode == ACS_CC_SUCCESS) || (m_omHandler.getInternalLastError() == -14))
        {
            //TRACE(("Creation object %s completed\n", 0, BUParamsCommon::IMM_CLASS_CMDLOG_NAME.c_str()));
            returnCode = ACS_CC_SUCCESS;
        }
        else
        {
            TRACE((LOG_LEVEL_ERROR, "error - Creation object %s failed with error: %d - %s\n", 0,
                    BUParamsCommon::IMM_CLASS_CMDLOG_NAME.c_str(),
                    m_omHandler.getInternalLastError(), m_omHandler.getInternalLastErrorText()));
        }
    }

    if (attribute)
    {
        delete [] attribute;
        attribute = 0;
    }

    return (returnCode == ACS_CC_SUCCESS);
}

//**********************************************************************
//  createDelayMeasuresObject()
//**********************************************************************
bool BUParamsOM::createDelayMeasuresObject()
{
    newTRACE(("BUParamsOM::createDelayMeasuresObject()", 0));

    ACS_CC_ReturnType returnCode = ACS_CC_FAILURE;

    // Define the default values
    int numAttr = 9;
    int dmValue = 0;
    //const string IMM_ATTR_RDN_temp = "id";
    char* rdnValue = const_cast<char *>("id=delayedMeasures_0");

    vector<ACS_CC_ValuesDefinitionType> AttrList;
    ACS_CC_ValuesDefinitionType *attribute;

    attribute = new ACS_CC_ValuesDefinitionType[numAttr];

    if (attribute != NULL)
    {
        /*Fill the rdn Attribute */
        attribute[0].attrName = (char*)BUParamsCommon::IMM_ATTR_ID.c_str();
        attribute[0].attrType = ATTR_STRINGT;
        attribute[0].attrValuesNum = 1;
        void* valueRDN[1]={reinterpret_cast<void*>(rdnValue)};
        attribute[0].attrValues = valueRDN;


        /* Fill the attribute attrDM */

        attribute[1].attrName = (char*)BUParamsCommon::IMM_ATTR_DM1.c_str();
        attribute[1].attrType = ATTR_INT32T;
        attribute[1].attrValuesNum = 1;
        void* valueDM1[1] = { reinterpret_cast<void*>(&dmValue) };
        attribute[1].attrValues = valueDM1;

        attribute[2].attrName = (char*)BUParamsCommon::IMM_ATTR_DM2.c_str();
        attribute[2].attrType = ATTR_INT32T;
        attribute[2].attrValuesNum = 1;
        attribute[2].attrValues = valueDM1;

        attribute[3].attrName = (char*)BUParamsCommon::IMM_ATTR_DM3.c_str();
        attribute[3].attrType = ATTR_INT32T;
        attribute[3].attrValuesNum = 1;
        attribute[3].attrValues = valueDM1;

        attribute[4].attrName = (char*)BUParamsCommon::IMM_ATTR_DM4.c_str();
        attribute[4].attrType = ATTR_INT32T;
        attribute[4].attrValuesNum = 1;
        attribute[4].attrValues = valueDM1;

        attribute[5].attrName = (char*)BUParamsCommon::IMM_ATTR_DM5.c_str();
        attribute[5].attrType = ATTR_INT32T;
        attribute[5].attrValuesNum = 1;
        attribute[5].attrValues = valueDM1;

        attribute[6].attrName = (char*)BUParamsCommon::IMM_ATTR_DM6.c_str();
        attribute[6].attrType = ATTR_INT32T;
        attribute[6].attrValuesNum = 1;
        attribute[6].attrValues = valueDM1;

        attribute[7].attrName = (char*)BUParamsCommon::IMM_ATTR_DM7.c_str();
        attribute[7].attrType = ATTR_INT32T;
        attribute[7].attrValuesNum = 1;
        attribute[7].attrValues = valueDM1;

        attribute[8].attrName = (char*)BUParamsCommon::IMM_ATTR_DM8.c_str();
        attribute[8].attrType = ATTR_INT32T;
        attribute[8].attrValuesNum = 1;
        attribute[8].attrValues = valueDM1;

        // Add the attributes into the list
        for(int i = 0; i < numAttr; i++)
        {
            AttrList.push_back(attribute[i]);
        }

        // Create Object for DelayedMeasures class
        returnCode = m_omHandler.createObject(BUParamsCommon::IMM_CLASS_DM_NAME.c_str(),
                                    BUParamsCommon::IMM_CLASSIC_OBJ_DN.c_str(), AttrList);

        if((returnCode == ACS_CC_SUCCESS) || (m_omHandler.getInternalLastError() == -14))
        {
            //TRACE(("Creation object %s completed\n", 0, BUParamsCommon::IMM_CLASS_DM_NAME.c_str()));
            returnCode = ACS_CC_SUCCESS;
        }
        else
        {
            TRACE((LOG_LEVEL_ERROR, "error - Creation object %s failed with error: %d - %s\n", 0,
                    BUParamsCommon::IMM_CLASS_DM_NAME.c_str(),
                    m_omHandler.getInternalLastError(), m_omHandler.getInternalLastErrorText()));
        }
    }

    if (attribute)
    {
        delete [] attribute;
        attribute = 0;
    }

    return (returnCode == ACS_CC_SUCCESS);
}

//**********************************************************************
//  createLastAutoReloadObject()
//**********************************************************************
bool BUParamsOM::createLastAutoReloadObject()
{
    newTRACE(("BUParamsOM::createLastAutoReloadObject()", 0));

    ACS_CC_ReturnType returnCode = ACS_CC_FAILURE;

    // Define the default values
    int numAttr = 11;
    uint32_t alogValue = 0;
    uint32_t clhValue = 0;
    uint32_t gfileValue = 0;
    uint32_t lpgfValue = 0;
    uint32_t nctValue = 0;
    uint32_t omissionValue = 0;
    uint32_t supValue = 0;

    char* odslValue  = const_cast<char *>("0000-00-00 00:00");
    char* odssValue  = const_cast<char *>("0000-00-00 00:00");
    char* opsrsValue = const_cast<char *>("0000-00-00 00:00");
    //const string IMM_ATTR_RDN_temp = "id";
    char* rdnValue = const_cast<char *>("id=lastReload_0");

    vector<ACS_CC_ValuesDefinitionType> AttrList;
    ACS_CC_ValuesDefinitionType *attribute;

    attribute = new ACS_CC_ValuesDefinitionType[numAttr];

    if (attribute != NULL)
    {
        /*Fill the rdn Attribute */
        attribute[0].attrName = (char*)BUParamsCommon::IMM_ATTR_ID.c_str();
        attribute[0].attrType = ATTR_STRINGT;
        attribute[0].attrValuesNum = 1;
        void* valueRDN[1]={reinterpret_cast<void*>(rdnValue)};
        attribute[0].attrValues = valueRDN;


        /*Fill the attribute attribute[1]*/
        attribute[1].attrName = (char*)BUParamsCommon::IMM_ATTR_LAR_CLH.c_str();
        attribute[1].attrType = ATTR_INT32T;
        attribute[1].attrValuesNum = 1;
        void* valueClh[1] = { reinterpret_cast<void*>(&clhValue) };
        attribute[1].attrValues = valueClh;

        /*Fill the attribute attribute[3]*/
        attribute[2].attrName = (char*)BUParamsCommon::IMM_ATTR_GFILE.c_str();
        attribute[2].attrType = ATTR_UINT32T;
        attribute[2].attrValuesNum = 1;
        void* valueGfile[1] = { reinterpret_cast<void*>(&gfileValue) };
        attribute[2].attrValues = valueGfile;

        /*Fill the attribute attribute[4]*/
        attribute[3].attrName = (char*)BUParamsCommon::IMM_ATTR_LPGF.c_str();
        attribute[3].attrType = ATTR_INT32T;
        attribute[3].attrValuesNum = 1;
        void* valueLpgf[1] = { reinterpret_cast<void*>(&lpgfValue) };
        attribute[3].attrValues = valueLpgf;

        /*Fill the attribute attribute[5]*/
        attribute[4].attrName = (char*)BUParamsCommon::IMM_ATTR_LAR_NCT.c_str();
        attribute[4].attrType = ATTR_UINT32T;
        attribute[4].attrValuesNum = 1;
        void* valueNct[1] = { reinterpret_cast<void*>(&nctValue) };
        attribute[4].attrValues = valueNct;

        /*Fill the attribute attribute[10]*/
        attribute[5].attrName = (char*)BUParamsCommon::IMM_ATTR_LAR_SUP.c_str();
        attribute[5].attrType = ATTR_UINT32T;
        attribute[5].attrValuesNum = 1;
        void* valueSup[1] = { reinterpret_cast<void*>(&supValue) };
        attribute[5].attrValues = valueSup;

        /*Fill the attribute attribute[8]*/
        attribute[6].attrName = (char*)BUParamsCommon::IMM_ATTR_LAR_OMISSION.c_str();
        attribute[6].attrType = ATTR_INT32T;
        attribute[6].attrValuesNum = 1;
        void* valueOmission[1] = { reinterpret_cast<void*>(&omissionValue) };
        attribute[6].attrValues = valueOmission;

        /*Fill the attribute attribute[1]*/
        attribute[7].attrName = (char*)BUParamsCommon::IMM_ATTR_ALOG.c_str();
        attribute[7].attrType = ATTR_UINT32T;
        attribute[7].attrValuesNum = 1;
        void* valueAlog[1] = { reinterpret_cast<void*>(&alogValue) };
        attribute[7].attrValues = valueAlog;

        /*Fill the attribute attOpsrs*/
        attribute[8].attrName = (char*)BUParamsCommon::IMM_ATTR_OPSRS.c_str();
        attribute[8].attrType = ATTR_STRINGT;
        attribute[8].attrValuesNum = 1;
        void* valueOpsrs[1] = { reinterpret_cast<void*>(opsrsValue) };
        attribute[8].attrValues = valueOpsrs;

        /*Fill the attribute attribute[7]*/
        attribute[9].attrName = (char*)BUParamsCommon::IMM_ATTR_ODSS.c_str();
        attribute[9].attrType = ATTR_STRINGT;
        attribute[9].attrValuesNum = 1;
        void* valueOdss[1] = { reinterpret_cast<void*>(odssValue) };
        attribute[9].attrValues = valueOdss;

        /*Fill the attribute attribute[6]*/
        attribute[10].attrName = (char*)BUParamsCommon::IMM_ATTR_ODSL.c_str();
        attribute[10].attrType = ATTR_STRINGT;
        attribute[10].attrValuesNum = 1;
        void* valueOdsl[1] = { reinterpret_cast<void*>(odslValue) };
        attribute[10].attrValues = valueOdsl;

        // Add the attributes into the list
        for(int i = 0; i < numAttr; i++)
        {
            AttrList.push_back(attribute[i]);
        }

        // Create Object for LastAutoReload class
        returnCode = m_omHandler.createObject(BUParamsCommon::IMM_CLASS_LAR_NAME.c_str(),
                                    BUParamsCommon::IMM_CLASSIC_OBJ_DN.c_str(), AttrList);

        if((returnCode == ACS_CC_SUCCESS) || (m_omHandler.getInternalLastError() == -14))
        {
            //TRACE(("Creation object %s completed\n", 0, BUParamsCommon::IMM_CLASS_LAR_NAME.c_str()));
            returnCode = ACS_CC_SUCCESS;
        }
        else
        {
            TRACE((LOG_LEVEL_ERROR, "error - Creation object %s failed with error: %d - %s\n", 0,
                    BUParamsCommon::IMM_CLASS_LAR_NAME.c_str(),
                    m_omHandler.getInternalLastError(), m_omHandler.getInternalLastErrorText()));
        }
    }

    if (attribute)
    {
        delete [] attribute;
        attribute = 0;
    }

    return (returnCode == ACS_CC_SUCCESS);
}

//**********************************************************************
//  modifyIntAttrs()
//**********************************************************************
bool BUParamsOM::modifyIntAttrs(const std::string& objectName, const std::string& attrName,
                                     int value, bool type, const std::string& transName)
{
    //newTRACE(("BUParamsOM::modifyIntAttrs(%s, %d)", 0, attrName.c_str(), value));

    ACS_CC_ReturnType returnCode;

    // Modify
    ACS_CC_ImmParameter attributeRDN1;

    /*Fill the rdn Attribute */
    attributeRDN1.attrName = (char*)attrName.c_str();
    // Check the value type
    if (type)
    {
        attributeRDN1.attrType = ATTR_UINT32T;
    }
    else
    {
        attributeRDN1.attrType = ATTR_INT32T;
    }
    attributeRDN1.attrValuesNum = 1;
    void* valueRDN1[1]={reinterpret_cast<void*>(&value)};
    attributeRDN1.attrValues = valueRDN1;

    returnCode = m_omHandler.modifyAttribute(objectName.c_str(), &attributeRDN1, transName);

    if(returnCode == ACS_CC_SUCCESS)
    {
        //TRACE(("Modify Attribute %s completed\n", 0, attrName.c_str()));
    }
    else
    {
        newTRACE((LOG_LEVEL_ERROR, "error - Modify attribute %s failed with error: %d - %s\n", 0,
                attrName.c_str(),
                m_omHandler.getInternalLastError(), m_omHandler.getInternalLastErrorText()));
        EVENT((CPS_BUAP_Events::modifyAttr_failed, 0, ""));
    }

    return (returnCode == ACS_CC_SUCCESS);
}

//**********************************************************************
//  modifyStringAttrs()
//**********************************************************************
bool BUParamsOM::modifyStringAttrs(const std::string& objectName, const std::string& attrName,
                                         const std::string& value, const std::string& transName)
{
    //newTRACE(("BUParamsOM::modifyStringAttrs(%s, %s)", 0, attrName.c_str(), value.c_str()));

    ACS_CC_ReturnType returnCode;

    // Modify
    ACS_CC_ImmParameter attributeRDN1;

    /*Fill the rdn Attribute */
    attributeRDN1.attrName = (char*)attrName.c_str();
    attributeRDN1.attrType = ATTR_STRINGT;
    attributeRDN1.attrValuesNum = 1;
    void* valueRDN1[1]={reinterpret_cast<void*>((char*)value.c_str())};
    attributeRDN1.attrValues = valueRDN1;

    returnCode = m_omHandler.modifyAttribute(objectName.c_str(), &attributeRDN1, transName);

    if(returnCode == ACS_CC_SUCCESS)
    {
        //TRACE(("Modify Attribute %s completed with transaction %s \n", 0,
        //        attrName.c_str(), transName.c_str()));
    }
    else
    {
        newTRACE((LOG_LEVEL_ERROR, "error - Modify attribute %s failed with error: %d - %s\n", 0,
                attrName.c_str(),
                m_omHandler.getInternalLastError(), m_omHandler.getInternalLastErrorText()));
        EVENT((CPS_BUAP_Events::modifyAttr_failed, 0, ""));
    }

    return (returnCode == ACS_CC_SUCCESS);
}

//**********************************************************************
//  init()
//**********************************************************************
void BUParamsOM::init()
{
    newTRACE((LOG_LEVEL_INFO, "BUParamsOM::init()", 0));

    ACS_CC_ReturnType result;
    m_initialized = false;

    // Initialize the interaction with IMM.
    result = m_omHandler.Init(NO_REGISTERED_OI);

    if (result == ACS_CC_SUCCESS)
    {
        m_initialized = true;
    }
    else
    {
        TRACE((LOG_LEVEL_ERROR, "Failed to init the OM: %d - %s", 0, m_omHandler.getInternalLastError(),
                m_omHandler.getInternalLastErrorText()));

    }

    //return (result == ACS_CC_SUCCESS);
}

//**********************************************************************
//  applyReq()
//**********************************************************************
bool BUParamsOM::applyReq(const std::string& transName)
{
    newTRACE((LOG_LEVEL_INFO, "BUParamsOM::applyReq(%s)", 0, transName.c_str()));

    ACS_CC_ReturnType returnCode = m_omHandler.applyRequest(transName);

    if(returnCode == ACS_CC_SUCCESS)
    {
        //TRACE(("Apply completed\n", 0));
    }
    else
    {
        TRACE((LOG_LEVEL_ERROR, "Apply failed with error: %d - %s\n", 0,
                m_omHandler.getInternalLastError(), m_omHandler.getInternalLastErrorText()));
        // Reset all requests with the transaction name
        m_omHandler.resetRequest(transName);
    }

    return (returnCode == ACS_CC_SUCCESS);
}


//**********************************************************************
//  updateStaticParams()
//**********************************************************************
void BUParamsOM::updateStaticParams(const BUParamsCommon::StaticParam_& staticPar,
        const std::string& transName)
{
    newTRACE(("BUParamsOM::updateStaticParams(%s)", 0, transName.c_str()));

    this->modifyIntAttrs(BUParamsCommon::IMM_SP_OBJ_DN,
                        BUParamsCommon::IMM_ATTR_CLH,
                        staticPar.clh, false, transName);

    this->modifyIntAttrs(BUParamsCommon::IMM_SP_OBJ_DN,
                        BUParamsCommon::IMM_ATTR_INCL1,
                        staticPar.incl1, false, transName);

    this->modifyIntAttrs(BUParamsCommon::IMM_SP_OBJ_DN,
                        BUParamsCommon::IMM_ATTR_INCL2,
                        staticPar.incl2, true, transName);

    this->modifyIntAttrs(BUParamsCommon::IMM_SP_OBJ_DN,
                        BUParamsCommon::IMM_ATTR_LOAZ,
                        staticPar.loaz, false, transName);

    this->modifyIntAttrs(BUParamsCommon::IMM_SP_OBJ_DN,
                        BUParamsCommon::IMM_ATTR_NTAZ,
                        staticPar.ntaz, true, transName);

    this->modifyIntAttrs(BUParamsCommon::IMM_SP_OBJ_DN,
                        BUParamsCommon::IMM_ATTR_NTCZ,
                        staticPar.ntcz, true, transName);

    this->modifyIntAttrs(BUParamsCommon::IMM_SP_OBJ_DN,
                        BUParamsCommon::IMM_ATTR_SUP,
                        staticPar.sup, true, transName);

    this->modifyStringAttrs(BUParamsCommon::IMM_SP_OBJ_DN,
                            BUParamsCommon::IMM_ATTR_INCLA, staticPar.incla, transName);

}


void BUParamsOM::updateDynamicParams(const BUParamsCommon::DynamicParam_& dynamicPar,
        const std::string& transName)
{
    newTRACE(("BUParamsOM::updateDynamicParams(%s)", 0, transName.c_str()));

    this->modifyIntAttrs(BUParamsCommon::IMM_DP_OBJ_DN,
                        BUParamsCommon::IMM_ATTR_STATE, dynamicPar.state, false, transName);

    this->modifyIntAttrs(BUParamsCommon::IMM_DP_OBJ_DN,
                            BUParamsCommon::IMM_ATTR_CTA, dynamicPar.cta, true, transName);

    this->modifyIntAttrs(BUParamsCommon::IMM_DP_OBJ_DN,
                            BUParamsCommon::IMM_ATTR_CFFR, dynamicPar.cffr, true, transName);

    this->modifyIntAttrs(BUParamsCommon::IMM_DP_OBJ_DN,
                            BUParamsCommon::IMM_ATTR_CSFR, dynamicPar.csfr, true, transName);

    this->modifyIntAttrs(BUParamsCommon::IMM_DP_OBJ_DN,
                            BUParamsCommon::IMM_ATTR_FOLD, dynamicPar.fold, true, transName);

    this->modifyIntAttrs(BUParamsCommon::IMM_DP_OBJ_DN,
                            BUParamsCommon::IMM_ATTR_SOLD, dynamicPar.sold, true, transName);

    this->modifyIntAttrs(BUParamsCommon::IMM_DP_OBJ_DN,
                            BUParamsCommon::IMM_ATTR_NCT, dynamicPar.nct, true, transName);

    this->modifyIntAttrs(BUParamsCommon::IMM_DP_OBJ_DN,
                            BUParamsCommon::IMM_ATTR_LPF, dynamicPar.lpf, true, transName);

    this->modifyIntAttrs(BUParamsCommon::IMM_DP_OBJ_DN,
                            BUParamsCommon::IMM_ATTR_OMISSION, dynamicPar.omission, false, transName);

    this->modifyStringAttrs(BUParamsCommon::IMM_DP_OBJ_DN,
                            BUParamsCommon::IMM_ATTR_EXP, dynamicPar.exp, transName);
}


void BUParamsOM::updateCmdLogParams(const BUParamsCommon::CommmandLogReference_& cmdLogPar,
        const std::string& transName)
{
    newTRACE(("BUParamsOM::updateCmdLogParams(%s)", 0, transName.c_str()));

    this->modifyIntAttrs(BUParamsCommon::IMM_CMDLOG_OBJ_DN,
                        BUParamsCommon::IMM_ATTR_CURRENT, cmdLogPar.current, true, transName);

    this->modifyIntAttrs(BUParamsCommon::IMM_CMDLOG_OBJ_DN,
                        BUParamsCommon::IMM_ATTR_MAIN, cmdLogPar.main, true, transName);

    this->modifyIntAttrs(BUParamsCommon::IMM_CMDLOG_OBJ_DN,
                        BUParamsCommon::IMM_ATTR_PREPARED, cmdLogPar.prepared, true, transName);
}


void BUParamsOM::updateDMeasuresParams(const BUParamsCommon::DelayedMeasure_& dmPar,
        const std::string& transName)
{
    newTRACE(("BUParamsOM::updateDMeasuresParams(%s)", 0, transName.c_str()));

    this->modifyIntAttrs(BUParamsCommon::IMM_DM_OBJ_DN,
                        BUParamsCommon::IMM_ATTR_DM1, dmPar.dm[0], false, transName);

    this->modifyIntAttrs(BUParamsCommon::IMM_DM_OBJ_DN,
                        BUParamsCommon::IMM_ATTR_DM2, dmPar.dm[1], false, transName);

    this->modifyIntAttrs(BUParamsCommon::IMM_DM_OBJ_DN,
                        BUParamsCommon::IMM_ATTR_DM3, dmPar.dm[2], false, transName);

    this->modifyIntAttrs(BUParamsCommon::IMM_DM_OBJ_DN,
                        BUParamsCommon::IMM_ATTR_DM4, dmPar.dm[3], false, transName);

    this->modifyIntAttrs(BUParamsCommon::IMM_DM_OBJ_DN,
                        BUParamsCommon::IMM_ATTR_DM5, dmPar.dm[4], false, transName);

    this->modifyIntAttrs(BUParamsCommon::IMM_DM_OBJ_DN,
                        BUParamsCommon::IMM_ATTR_DM6, dmPar.dm[5], false, transName);

    this->modifyIntAttrs(BUParamsCommon::IMM_DM_OBJ_DN,
                        BUParamsCommon::IMM_ATTR_DM7, dmPar.dm[6], false, transName);

    this->modifyIntAttrs(BUParamsCommon::IMM_DM_OBJ_DN,
                        BUParamsCommon::IMM_ATTR_DM8, dmPar.dm[7], false, transName);
}


void BUParamsOM::updateLastReloadParams(const BUParamsCommon::LastAutoReload_& lastReloadPar,
        const std::string& transName)
{
    newTRACE(("BUParamsOM::updateLastReloadParams(%s)", 0, transName.c_str()));

    this->modifyIntAttrs(BUParamsCommon::IMM_LAR_OBJ_DN,
                        BUParamsCommon::IMM_ATTR_LAR_CLH, lastReloadPar.clh, false, transName);

    this->modifyIntAttrs(BUParamsCommon::IMM_LAR_OBJ_DN,
                            BUParamsCommon::IMM_ATTR_LAR_NCT, lastReloadPar.nct, true, transName);

    this->modifyIntAttrs(BUParamsCommon::IMM_LAR_OBJ_DN,
                            BUParamsCommon::IMM_ATTR_GFILE, lastReloadPar.gfile, true, transName);

    this->modifyIntAttrs(BUParamsCommon::IMM_LAR_OBJ_DN,
                            BUParamsCommon::IMM_ATTR_LPGF, lastReloadPar.lpgf, false, transName);

    this->modifyIntAttrs(BUParamsCommon::IMM_LAR_OBJ_DN,
                            BUParamsCommon::IMM_ATTR_LAR_OMISSION, lastReloadPar.omission, false, transName);

    this->modifyIntAttrs(BUParamsCommon::IMM_LAR_OBJ_DN,
                            BUParamsCommon::IMM_ATTR_ALOG, lastReloadPar.alog, true, transName);

    this->modifyIntAttrs(BUParamsCommon::IMM_LAR_OBJ_DN,
                            BUParamsCommon::IMM_ATTR_LAR_SUP, lastReloadPar.sup, true, transName);

    this->modifyStringAttrs(BUParamsCommon::IMM_LAR_OBJ_DN,
                            BUParamsCommon::IMM_ATTR_OPSRS, lastReloadPar.opsrs, transName);

    this->modifyStringAttrs(BUParamsCommon::IMM_LAR_OBJ_DN,
                            BUParamsCommon::IMM_ATTR_ODSS, lastReloadPar.odss, transName);

    this->modifyStringAttrs(BUParamsCommon::IMM_LAR_OBJ_DN,
                            BUParamsCommon::IMM_ATTR_ODSL, lastReloadPar.odsl, transName);
}
