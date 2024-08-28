//******************************************************************************
//
// NAME
//      %File_name:CPS_BUAP_BUPSET.cpp%
//
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 1997.
//  All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson Utvecklings AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.

// .DESCRIPTION 
//      This class contains the methods required to implement the
//      bupset command.  The first method to be used is Initialise
//      which will create a client of parmgr and initiate a
//      conversation.  Next, the ParseCommandLine method should
//      be called.  Finally the ExecuteCommand method should be
//      called.  If an error occurs at any time an exception will
//      be thrown.

// DOCUMENT NO
//	190 89-CAA 109 0082

// AUTHOR 
// 	1997/08/06 by U/B/SD David Wade

// SEE ALSO 
// 	<Related classes, (abstract) baseclasses, friend classes, ...>

// Revision history
// ----------------
// 1997/08/06 Created by David Wade
// 1999/07/02 LAN Added new include file CPS_BUAP_getopt.H
// 2012/07/10 XNGUDAN Adapt to IMM

//
//******************************************************************************
#if 0
#include "CPS_BUAP_Linux.h"
#include <string.h>
#endif

//#include <stdlib.h>
//#include <stdio.h>
//#include <iostream>

//#include "CPS_BUAP_DateTime.h"
#include "CPS_BUAP_EventReporter.h"
#include "CPS_BUAP_Events.h"
#include "CPS_BUAP_Trace.h"
//#include "CPS_BUAP_RPF.h"
//#include "CPS_BUAP_Client.h"
//#include "CPS_BUAP_MSG_IDs.h"
//#include "CPS_BUAP_MSG_static_parameters.h"
//#include "CPS_BUAP_MSG_write_reply.h"
#include "CPS_BUAP_BUPSET.h"
//#include "CPS_BUAP_getopt.h"
#include <sstream>
#include <vector>

//using namespace CPS_BUAP;
using namespace std;

//******************************************************************************
//	CPS_BUAP_BUPSET()
//******************************************************************************
CPS_BUAP_BUPSET::CPS_BUAP_BUPSET():
        m_clh(1), m_incl1(0), m_incl2(0), m_loaz(0), m_ntaz(0), m_ntcz(10), m_sup(60),
        m_updateFlag(0), m_initialized(false), m_lastErrorCode(0), m_lastErrorText("")
{
    newTRACE(("CPS_BUAP_BUPSET::CPS_BUAP_BUPSET()", 0))

    //m_Reset = 0;
    //m_Force = 0;
    //m_pClient = (CPS_BUAP_Client *) 0;
}

//******************************************************************************
//	~CPS_BUAP_BUPSET()
//******************************************************************************
CPS_BUAP_BUPSET::~CPS_BUAP_BUPSET()
{
    newTRACE(("CPS_BUAP_BUPSET::~CPS_BUAP_BUPSET()", 0))

/*
    if (m_pClient)
    {
        delete m_pClient;
    }
*/

    if (m_initialized)
    {
        m_omHandler.Finalize();
    }
    m_initialized = false;
}

//******************************************************************************
//	Initialise()
//******************************************************************************
#if 0
void CPS_BUAP_BUPSET::Initialise()
{
newTRACE(("CPS_BUAP_BUPSET::Initialise()", 0))

try
{
    m_pClient = new CPS_BUAP_Client("BUPARMGR", 5, 1);
}
catch ( ... )
{
    EVENT((CPS_BUAP_Events::no_server, 1))
    throw eNoServer;
}
}


//******************************************************************************
//	ParseCommandLine()
//******************************************************************************
void CPS_BUAP_BUPSET::ParseCommandLine(int argc, char **argv)
{
    newTRACE(("CPS_BUAP_BUPSET::ParseCommandLine()", 0))

    char l_separator;
    int l_opt;
    char *l_endptr;
    long l_optval;
    int l_century;
    int l_year;
    int l_month;
    int l_day;
    int l_count;

    try
    {
        while ((l_opt = buap_getopt(argc, argv, "rfm:t:c:o:d:1:2:s:")) != EOF)
        {

            switch (l_opt)
            {
            case 'r':
                m_StaticPars.m_ManAuto << c_ManAuto;
                m_StaticPars.m_NTAZ << c_NTAZ;
                m_StaticPars.m_NTCZ << c_NTCZ;
                m_StaticPars.m_LOAZ << c_LOAZ;
                m_StaticPars.m_INCLAcentury << c_INCLAcentury;
                m_StaticPars.m_INCLAyear << c_INCLAyear;
                m_StaticPars.m_INCLAmonth << c_INCLAmonth;
                m_StaticPars.m_INCLAday << c_INCLAday;
                m_StaticPars.m_INCL1 << c_INCL1;
                m_StaticPars.m_INCL2 << c_INCL2;
                m_StaticPars.m_SUP << c_SUP;

                m_Reset = 1;
                break;

            case 'f':
                m_Force = 1;
                break;

            case 'm':
                l_optval = strtol(optarg, &l_endptr, 10);
                if ((l_endptr[0]) || (l_optval < 0) || (l_optval > 1))
                {
                    throw eInvalidValue;
                }
                m_StaticPars.m_ManAuto << (l_optval & 0xFF);
                break;

            case 't':
                l_optval = strtol(optarg, &l_endptr, 10);
                if ((l_endptr[0]) || (l_optval < 0) || (l_optval > 255))
                {
                    throw eInvalidValue;
                }
                m_StaticPars.m_NTAZ << (l_optval & 0xFF);
                break;

            case 'c':
                l_optval = strtol(optarg, &l_endptr, 10);
                if ((l_endptr[0]) || (l_optval < 0) || (l_optval > 255))
                {
                    throw eInvalidValue;
                }
                m_StaticPars.m_NTCZ << (l_optval & 0xFF);
                break;

            case 'o':
                l_optval = strtol(optarg, &l_endptr, 10);
                if ((l_endptr[0]) || (l_optval < 0) || (l_optval > 2))
                {
                    throw eInvalidValue;
                }
                m_StaticPars.m_LOAZ << (l_optval & 0xFF);
                break;

            case 'd':
            {
                l_count = sscanf(optarg, "%2d%2d%1[/\\,.:;-]%d%1[/\\,.:;-]%d",
                        &l_century, &l_year, &l_separator, &l_month,
                        &l_separator, &l_day);

                if (l_count != 6)
                {
                    throw eInvalidValue;
                }
                DateTime date(l_day, l_month, l_year + (l_century * 100));

                { // Check for no future date
                    DateTime today;
                    if (date > today)
                    {
                        throw eInvalidValue;
                    }
                }

                if (date.isValid())
                {
                    m_StaticPars.m_INCLAcentury << (l_century & 0xFF);
                    m_StaticPars.m_INCLAyear << (l_year & 0xFF);
                    m_StaticPars.m_INCLAmonth << (l_month & 0xFF);
                    m_StaticPars.m_INCLAday << (l_day & 0xFF);
                }
                else
                {
                    throw eInvalidValue;
                }
                break;
            }

            case '1':
                l_optval = strtol(optarg, &l_endptr, 10);
                if ((l_endptr[0]) || (l_optval < 0) || (l_optval > 1))
                {
                    throw eInvalidValue;
                }
                m_StaticPars.m_INCL1 << (l_optval & 0xFF);
                break;

            case '2':
                l_optval = strtol(optarg, &l_endptr, 10);
                if ((l_endptr[0]) || ((l_optval != 0) && ((l_optval < 100)
                        || (l_optval > 127))))
                {
                    throw eInvalidValue;
                }

                m_StaticPars.m_INCL2 << (l_optval & 0xFF);
                break;

            case 's':
                l_optval = strtol(optarg, &l_endptr, 10);
                if ((l_endptr[0]) || (l_optval < 30) || (l_optval > 2400))
                {
                    throw eInvalidValue;
                }
                m_StaticPars.m_SUP << ((l_optval / 10) & 0xFF);
                break;

            default:
                throw eSyntaxError;
            }
        }
    } catch (eError EE)
    {
        switch (EE)
        {
        case eSyntaxError:
            //
            // getopt will have printed the error message
            //
            throw;

        case eInvalidValue:
            //:ag1 changed %c to %d
            printf("bupset: invalid value <%s> for option <%d>\n", optarg,
                    l_opt);
            throw;

        default:
            throw;
        }
    }
}

//******************************************************************************
//	ExecuteCommand()
//******************************************************************************
void CPS_BUAP_BUPSET::ExecuteCommand()
{
    newTRACE(("CPS_BUAP_BUPSET::ExecuteCommand()", 0))

    CPS_BUAP_MSG_write_reply l_rxMsg;
    char l_linein[256] = "y\n";

    try
    {
        if ((m_Reset) && (!m_Force))
        {
            printf(
                    "\nYou are about to reset all parameters to their default values!\n\n");
            printf("Do you wish to continue (y/n) [n] ? ");
            fflush( NULL); // uabmnst
            fgets(l_linein, 256, stdin);
        }

        if (strcmp(l_linein, "y\n") == 0)
        {
            m_StaticPars.UserData1() = CPS_BUAP_MSG_ID_write_static;

            m_pClient->TXRX(m_StaticPars, l_rxMsg);

            switch (l_rxMsg.UserData1())
            {
            case CPS_BUAP_MSG_ID_write_static_ok:
                break;

            case CPS_BUAP_MSG_ID_write_static_fail:
                if (l_rxMsg.m_ResultCode()
                        == CPS_BUAP_MSG_write_reply::function_busy)
                {
                    throw eLoadInProgress;
                }

                if (l_rxMsg.m_ResultCode()
                        == CPS_BUAP_MSG_write_reply::access_error)
                {
                    throw eCPFSerror;
                }

            default:
                throw eUnknown;
            }
        }
    } catch (CPS_BUAP_Exception&) //:ag1
    {
        throw eUnknown;
    }
}
#endif

bool CPS_BUAP_BUPSET::init()
{
    newTRACE(("CPS_BUAP_BUPSET::init()", 0));

    m_initialized = false;

    ACS_CC_ReturnType result = m_omHandler.Init();

    if (result == ACS_CC_SUCCESS)
    {
        m_initialized = true;
    }
    else
    {
        TRACE(("Failed to init the OM: %d - %s", 0, m_omHandler.getInternalLastError(), m_omHandler.getInternalLastErrorText()));

        m_lastErrorCode = m_omHandler.getInternalLastError();
        m_lastErrorText = m_omHandler.getInternalLastErrorText();
    }

    return m_initialized;
}

bool CPS_BUAP_BUPSET::update()
{
    newTRACE(("CPS_BUAP_BUPSET::update()", 0));

    bool updateResult = false;

    if (!m_initialized)
    {
        m_lastErrorCode = BUParamsCommon::NO_INITIALIZED;
        m_lastErrorText = BUParamsCommon::errorCodeToString(BUParamsCommon::NO_INITIALIZED);

        TRACE(("Error : %s", 0, m_lastErrorText.c_str()));

        return updateResult;
    }

    if (m_updateFlag == 0)
    {
        m_lastErrorCode = BUParamsCommon::NO_REQUESTUPDATED;
        m_lastErrorText = BUParamsCommon::errorCodeToString(BUParamsCommon::NO_REQUESTUPDATED);
        TRACE(("Error : %s", 0, m_lastErrorText.c_str()));

        return updateResult;
    }

    // Obtain the DN of the object containing the name of the value to be retrieved
    string objectDN = BUParamsCommon::IMM_SP_OBJ_DN;
    string transactionName("modifyStaticParams");

    ACS_CC_ImmParameter attrValueModify;
    ACS_CC_ReturnType result = ACS_CC_SUCCESS;

    // Modify attribute CLH
    if (m_updateFlag & BUParamsCommon::FLG_CLH)
    {
        attrValueModify.attrName = (char *) BUParamsCommon::IMM_ATTR_CLH.c_str();
        attrValueModify.attrType = ATTR_INT32T;
        attrValueModify.attrValuesNum = 1;
        void* newAttrValue[1] = {reinterpret_cast<void*>(&m_clh)};

        attrValueModify.attrValues = newAttrValue;

        result = m_omHandler.modifyAttribute(objectDN.c_str(), &attrValueModify, transactionName);

        if (result != ACS_CC_SUCCESS)
        {
            TRACE(("Failed to modify attribute %s - %u", 0, attrValueModify.attrName, m_clh));
        }
        else
        {
            TRACE(("Modify attribute %s - %u successfully", 0, attrValueModify.attrName, m_clh));

            updateResult = true;
        }
    }

    // Modify attribute INCL1
    if ((result == ACS_CC_SUCCESS) && (m_updateFlag & BUParamsCommon::FLG_INCL1))
    {
        attrValueModify.attrName = (char *) BUParamsCommon::IMM_ATTR_INCL1.c_str();
        attrValueModify.attrType = ATTR_INT32T;
        attrValueModify.attrValuesNum = 1;
        void* newAttrValue[1] = {reinterpret_cast<void*>(&m_incl1)};

        attrValueModify.attrValues = newAttrValue;

        result = m_omHandler.modifyAttribute(objectDN.c_str(), &attrValueModify, transactionName);

        if (result != ACS_CC_SUCCESS)
        {
            TRACE(("Failed to modify attribute %s - %u", 0, attrValueModify.attrName, m_incl1));

            updateResult = false;
        }
        else
        {
            TRACE(("Modify attribute %s - %u successfully", 0, attrValueModify.attrName, m_incl1));

            updateResult = true;
        }
    }

    // Modify attribute INCL2
    if ((result == ACS_CC_SUCCESS) && (m_updateFlag & BUParamsCommon::FLG_INCL2))
    {
        attrValueModify.attrName = (char *) BUParamsCommon::IMM_ATTR_INCL2.c_str();
        attrValueModify.attrType = ATTR_UINT32T;
        attrValueModify.attrValuesNum = 1;
        void* newAttrValue[1] = {reinterpret_cast<void*>(&m_incl2)};

        attrValueModify.attrValues = newAttrValue;

        result = m_omHandler.modifyAttribute(objectDN.c_str(), &attrValueModify, transactionName);

        if (result != ACS_CC_SUCCESS)
        {
            TRACE(("Failed to modify attribute %s - %u", 0, attrValueModify.attrName, m_incl2));

            updateResult = false;
        }
        else
        {
            TRACE(("Modify attribute %s - %u successfully", 0, attrValueModify.attrName, m_incl2));

            updateResult = true;
        }
    }

    // Modify attribute INCLA
    if ((result == ACS_CC_SUCCESS) && (m_updateFlag & BUParamsCommon::FLG_INCLA))
    {
        attrValueModify.attrName = (char *)BUParamsCommon::IMM_ATTR_INCLA.c_str();
        attrValueModify.attrType = ATTR_STRINGT;
        attrValueModify.attrValuesNum = 1;
        void* newAttrValue[1] = {reinterpret_cast<void*>((char*)m_incla.c_str())};

        attrValueModify.attrValues = newAttrValue;

        result = m_omHandler.modifyAttribute(objectDN.c_str(), &attrValueModify, transactionName);

        if (result != ACS_CC_SUCCESS)
        {
            TRACE(("Failed to modify attribute %s - %s", 0, attrValueModify.attrName, m_incla.c_str()));

            updateResult = false;
        }
        else
        {
            TRACE(("Modify attribute %s - %s successfully", 0, attrValueModify.attrName, m_incla.c_str()));

            updateResult = true;
        }
    }

    // Modify attribute LOAZ
    if ((result == ACS_CC_SUCCESS) && (m_updateFlag & BUParamsCommon::FLG_LOAZ))
    {
        attrValueModify.attrName = (char *)BUParamsCommon::IMM_ATTR_LOAZ.c_str();
        attrValueModify.attrType = ATTR_INT32T;
        attrValueModify.attrValuesNum = 1;
        void* newAttrValue[1] = {reinterpret_cast<void*>(&m_loaz)};

        attrValueModify.attrValues = newAttrValue;

        result = m_omHandler.modifyAttribute(objectDN.c_str(), &attrValueModify, transactionName);

        if (result != ACS_CC_SUCCESS)
        {
            TRACE(("Failed to modify attribute %s - %u", 0, attrValueModify.attrName, m_loaz));

            updateResult = false;
        }
        else
        {
            TRACE(("Modify attribute %s - %u successfully", 0, attrValueModify.attrName, m_loaz));

            updateResult = true;
        }
    }

    // Modify attribute NTAZ
    if ((result == ACS_CC_SUCCESS) && (m_updateFlag & BUParamsCommon::FLG_NTAZ))
    {
        attrValueModify.attrName = (char *)BUParamsCommon::IMM_ATTR_NTAZ.c_str();
        attrValueModify.attrType = ATTR_UINT32T;
        attrValueModify.attrValuesNum = 1;
        void* newAttrValue[1] = {reinterpret_cast<void*>(&m_ntaz)};

        attrValueModify.attrValues = newAttrValue;

        result = m_omHandler.modifyAttribute(objectDN.c_str(), &attrValueModify, transactionName);

        if (result != ACS_CC_SUCCESS)
        {
            TRACE(("Failed to modify attribute %s - %u", 0, attrValueModify.attrName, m_ntaz));

            updateResult = false;
        }
        else
        {
            TRACE(("Modify attribute %s - %u successfully", 0, attrValueModify.attrName, m_ntaz));

            updateResult = true;
        }
    }


    // Modify attribute NTCZ
    if ((result == ACS_CC_SUCCESS) && (m_updateFlag & BUParamsCommon::FLG_NTCZ))
    {
        attrValueModify.attrName = (char *)BUParamsCommon::IMM_ATTR_NTCZ.c_str();
        attrValueModify.attrType = ATTR_UINT32T;
        attrValueModify.attrValuesNum = 1;
        void* newAttrValue[1] = {reinterpret_cast<void*>(&m_ntcz)};

        attrValueModify.attrValues = newAttrValue;

        result = m_omHandler.modifyAttribute(objectDN.c_str(), &attrValueModify, transactionName);

        if (result != ACS_CC_SUCCESS)
        {
            TRACE(("Failed to modify attribute %s - %u", 0, attrValueModify.attrName, m_ntcz));

            updateResult = false;
        }
        else
        {
            TRACE(("Modify attribute %s - %u successfully", 0, attrValueModify.attrName, m_ntcz));

            updateResult = true;
        }
    }

    // Modify attribute SUP
    if ((result == ACS_CC_SUCCESS) && (m_updateFlag & BUParamsCommon::FLG_SUP))
    {
        attrValueModify.attrName = (char *)BUParamsCommon::IMM_ATTR_SUP.c_str();
        attrValueModify.attrType = ATTR_UINT32T;
        attrValueModify.attrValuesNum = 1;
        void* newAttrValue[1] = {reinterpret_cast<void*>(&m_sup)};

        attrValueModify.attrValues = newAttrValue;

        result = m_omHandler.modifyAttribute(objectDN.c_str(), &attrValueModify, transactionName);

        if (result != ACS_CC_SUCCESS)
        {
            TRACE(("Failed to modify attribute %s - %u", 0, attrValueModify.attrName, m_sup));

            updateResult = false;
        }
        else
        {
            TRACE(("Modify attribute %s - %u successfully", 0, attrValueModify.attrName, m_sup));

            updateResult = true;
        }
    }

    if (updateResult != true)
    {
        m_omHandler.getExitCode(transactionName, m_lastErrorCode, m_lastErrorText);
        TRACE(("Update failed with exit code: %d - %s", 0, m_lastErrorCode, m_lastErrorText.c_str()));

        if (m_lastErrorCode == 0)
        {
            TRACE(("Internal Error: %d - %s", 0, m_omHandler.getInternalLastError(), m_omHandler.getInternalLastErrorText()));
            m_lastErrorCode = m_omHandler.getInternalLastError();
            m_lastErrorText = m_omHandler.getInternalLastErrorText();
        }

        result = m_omHandler.resetRequest(transactionName);

        if (result != ACS_CC_SUCCESS)
        {
            TRACE(("Failed to reset the transaction: %s", 0, transactionName.c_str()));
            m_lastErrorCode = m_omHandler.getInternalLastError();
            m_lastErrorText = m_omHandler.getInternalLastErrorText();
        }
    }
    else
    {
        result = m_omHandler.applyRequest(transactionName);

        if (result != ACS_CC_SUCCESS)
        {
            TRACE(("Failed to apply the transaction: %s", 0, transactionName.c_str()));

            updateResult = false;
            result = m_omHandler.resetRequest(transactionName);

            if (result != ACS_CC_SUCCESS)
            {
                TRACE(("Failed to reset the transaction: %s after apply failure", 0, transactionName.c_str()));
            }

            m_lastErrorCode = m_omHandler.getInternalLastError();
            m_lastErrorText = m_omHandler.getInternalLastErrorText();
        }
    }

    m_updateFlag = 0;

    return updateResult;
}

void CPS_BUAP_BUPSET::setCLH(uint32_t clh)
{
    newTRACE(("CPS_BUAP_BUPSET::setCLH(%u)", 0, clh));

    m_clh = clh;
    m_updateFlag |= BUParamsCommon::FLG_CLH;
}

void CPS_BUAP_BUPSET::setINCL1(uint32_t incl1)
{
    newTRACE(("CPS_BUAP_BUPSET::setINCL1(%u)", 0, incl1));

    m_incl1 = incl1;
    m_updateFlag |= BUParamsCommon::FLG_INCL1;
}

void CPS_BUAP_BUPSET::setINCL2(uint32_t incl2)
{
    newTRACE(("CPS_BUAP_BUPSET::setINCL2(%u)", 0, incl2));

    m_incl2 = incl2;
    m_updateFlag |= BUParamsCommon::FLG_INCL2;
}

void CPS_BUAP_BUPSET::setINCLA(string &incla)
{
    newTRACE(("CPS_BUAP_BUPSET::setINCLA(%s)", 0, incla.c_str()));

    m_incla = incla;
    m_updateFlag |= BUParamsCommon::FLG_INCLA;
}

void CPS_BUAP_BUPSET::setLOAZ(uint32_t loaz)
{
    newTRACE(("CPS_BUAP_BUPSET::setLOAZ(%u)", 0, loaz));

    m_loaz = loaz;
    m_updateFlag |= BUParamsCommon::FLG_LOAZ;
}

void CPS_BUAP_BUPSET::setNTAZ(uint32_t ntaz)
{
    newTRACE(("CPS_BUAP_BUPSET::setNTAZ(%u)", 0, ntaz));

    m_ntaz = ntaz;
    m_updateFlag |= BUParamsCommon::FLG_NTAZ;
}

void CPS_BUAP_BUPSET::setNTCZ(uint32_t ntcz)
{
    newTRACE(("CPS_BUAP_BUPSET::setNTCZ(%u)", 0, ntcz));

    m_ntcz = ntcz;
    m_updateFlag |= BUParamsCommon::FLG_NTCZ;
}

void CPS_BUAP_BUPSET::setSUP(uint32_t sup)
{
    newTRACE(("CPS_BUAP_BUPSET::setSUP(%u)", 0, sup));

    m_sup = sup;
    m_updateFlag |= BUParamsCommon::FLG_SUP;
}

#if 0
bool CPS_BUAP_BUPSET::get(BUParamsCommon::StaticParam_ &param)
{
    newTRACE(("CPS_BUAP_BUPSET::get()", 0));

    ACS_CC_ReturnType result = ACS_CC_SUCCESS;
    string objectDN = BUParamsCommon::IMM_SP_OBJ_DN;

    ACS_APGCC_ImmAttribute clhAttr;
    ACS_APGCC_ImmAttribute incl1Attr;
    ACS_APGCC_ImmAttribute incl2Attr;
    ACS_APGCC_ImmAttribute inclaAttr;
    ACS_APGCC_ImmAttribute isValidAttr;
    ACS_APGCC_ImmAttribute lastReloadFileAttr;
    ACS_APGCC_ImmAttribute lastReloadTimeAttr;
    ACS_APGCC_ImmAttribute loazAttr;
    ACS_APGCC_ImmAttribute ntazAttr;
    ACS_APGCC_ImmAttribute ntczAttr;
    ACS_APGCC_ImmAttribute supAttr;

    clhAttr.attrName            = BUParamsCommon::IMM_ATTR_CLH;
    incl1Attr.attrName          = BUParamsCommon::IMM_ATTR_INCL1;
    incl2Attr.attrName          = BUParamsCommon::IMM_ATTR_INCL2;
    inclaAttr.attrName          = BUParamsCommon::IMM_ATTR_INCLA;
    isValidAttr.attrName        = BUParamsCommon::IMM_ATTR_IV;
    lastReloadFileAttr.attrName = BUParamsCommon::IMM_ATTR_LRF;
    lastReloadTimeAttr.attrName = BUParamsCommon::IMM_ATTR_LRT;
    loazAttr.attrName           = BUParamsCommon::IMM_ATTR_LOAZ;
    ntazAttr.attrName           = BUParamsCommon::IMM_ATTR_NTAZ;
    ntczAttr.attrName           = BUParamsCommon::IMM_ATTR_NTCZ;
    supAttr.attrName            = BUParamsCommon::IMM_ATTR_SUP;

    vector<ACS_APGCC_ImmAttribute *> staticParamAttrs;

    staticParamAttrs.push_back(&clhAttr);
    staticParamAttrs.push_back(&incl1Attr);
    staticParamAttrs.push_back(&incl2Attr);
    staticParamAttrs.push_back(&inclaAttr);
    staticParamAttrs.push_back(&isValidAttr);
    staticParamAttrs.push_back(&lastReloadFileAttr);
    staticParamAttrs.push_back(&lastReloadTimeAttr);
    staticParamAttrs.push_back(&loazAttr);
    staticParamAttrs.push_back(&ntazAttr);
    staticParamAttrs.push_back(&ntczAttr);
    staticParamAttrs.push_back(&supAttr);

    result = m_omHandler.getAttribute(objectDN.c_str(), staticParamAttrs);

    if (result != ACS_CC_SUCCESS)
    {
        m_lastErrorCode = m_omHandler.getInternalLastError();
        m_lastErrorText = m_omHandler.getInternalLastErrorText();

        TRACE(("Failed to get StaticParams attribute: %d - %s", 0, m_lastErrorCode, m_lastErrorText.c_str()));
    }
    else
    {
        m_clh = param.clh       = *reinterpret_cast<uint32_t *>(clhAttr.attrValues[0]);
        m_incl1 = param.incl1   = *reinterpret_cast<uint32_t *>(incl1Attr.attrValues[0]);
        m_incl2 = param.incl2   = *reinterpret_cast<uint32_t *>(incl2Attr.attrValues[0]);
        m_incla = param.incla   = string(reinterpret_cast<char *>(inclaAttr.attrValues[0]));
        param.isValid           = *reinterpret_cast<uint32_t *>(isValidAttr.attrValues[0]);
        param.lastReloadFile    = string(reinterpret_cast<char *>(lastReloadFileAttr.attrValues[0]));
        param.lastReloadTime    = string(reinterpret_cast<char *>(lastReloadTimeAttr.attrValues[0]));
        m_loaz = param.loaz     = *reinterpret_cast<uint32_t *>(loazAttr.attrValues[0]);
        m_ntaz = param.ntaz     = *reinterpret_cast<uint32_t *>(ntazAttr.attrValues[0]);
        m_ntcz= param.ntcz      = *reinterpret_cast<uint32_t *>(ntczAttr.attrValues[0]);
        m_sup = param.sup       = *reinterpret_cast<uint32_t *>(supAttr.attrValues[0]);
    }

    return (result == ACS_CC_SUCCESS ? true : false);
}
#endif


int CPS_BUAP_BUPSET::getLastError()
{
    newTRACE(("CPS_BUAP_BUPSET::getLastError()", 0));

    return m_lastErrorCode;
}
const string& CPS_BUAP_BUPSET::getLastErrorText()
{
    newTRACE(("CPS_BUAP_BUPSET::getLastErrorText()", 0));

    return m_lastErrorText;
}

bool CPS_BUAP_BUPSET::doReset()
{
    newTRACE(("CPS_BUAP_BUPSET::doReset()", 0));

    ACS_CC_ReturnType result = ACS_CC_SUCCESS;
    string temp("1990-01-01");
    // Set Static Params to default values
    this->setCLH(1);
    this->setINCL1(0);
    this->setINCL2(0);
    this->setINCLA(temp);
    this->setLOAZ(0);
    this->setNTAZ(0);
    this->setNTCZ(10);
    this->setSUP(60);

    if (!this->update())
    {
        result = ACS_CC_FAILURE;
    }

    return (result == ACS_CC_SUCCESS ? true : false);
}

