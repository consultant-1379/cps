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
 *  StaticParamsOI.cpp
 *
 * DESCRIPTION
 *  OI handler for Classic class
 *
 * DOCUMENT NO
 *  190 89-CAA 109 1414
 *
 * AUTHOR
 *  XDT/DEK XNGUDAN
 *
 * === Revision history ===============================
 * 2012-02-20   XNGUDAN    Create
 * 2012-03-08   XNGUDAN    Update after model review
 * ====================================================
 */

#include "StaticParamsOI.h"
#include "CPS_BUAP_Trace.h"
#include "BUParamsCommon.h"
#include "CPS_BUAP_DateTime.h"
#include "CPS_BUAP_EventReporter.h"
#include "CPS_BUAP_Events.h"
#include "CPS_BUAP_MSG_IDs.h"
#include "CPS_BUAP_MSG_write_reply.h"
#include "CPS_BUAP_getopt.h"

#include "CPS_BUAP_Exception.h"
#include "CPS_BUAP_Config.h"

#include <sys/time.h>
#include <cerrno>
#include <cstring>
#include <boost/regex.hpp>
#include "boost/tokenizer.hpp"
#include "boost/lexical_cast.hpp"

using namespace std;
using namespace boost;
using namespace CPS_BUAP;

/*============================================================================
    ROUTINE: StaticParamsOI()
 ============================================================================ */
StaticParamsOI::StaticParamsOI():
        acs_apgcc_objectimplementerinterface_V3(BUParamsCommon::IMM_IMPL_NAME),
        m_running(false), m_pClient((CPS_BUAP_Client *) 0),
        m_pStaticPars(0)
{
	// change for Tracing order
    //newTRACE((LOG_LEVEL_INFO, "StaticParamsOI::StaticParamsOI()", 0));
}

/*============================================================================
    ROUTINE: ~StaticParamsOI()
 ============================================================================ */
StaticParamsOI::~StaticParamsOI()
{
	// change for Tracing Order
    //newTRACE((LOG_LEVEL_INFO, "StaticParamsOI::~StaticParamsOI()", 0));

    // Delete pointer to client.
    if (m_pClient)
    {
        delete m_pClient;
        m_pClient = 0;
    }

    // Delete pointer to Parameter Message
    if (m_pStaticPars)
    {
        delete m_pStaticPars;
        m_pStaticPars = 0;
    }

}

/*============================================================================
    ROUTINE: run()
 ============================================================================ */
void StaticParamsOI::run()
{
    newTRACE((LOG_LEVEL_INFO, "StaticParamsOI::run()", 0));

    ACS_CC_ReturnType result = ACS_CC_SUCCESS;

    // Use condition variable to notify other thread that Classic OI is started
    // to create objects.
    {
        boost::lock_guard<boost::mutex> lock(m_mutex);
        m_running = true;
    }
    m_condition.notify_one();

    // Register as OI for Classic class.
    result = m_oiHandler.addClassImpl(this, BUParamsCommon::IMM_CLASS_SP_NAME.c_str());

    if (result != ACS_CC_SUCCESS)
    {
        TRACE((LOG_LEVEL_ERROR, "Failed to register as OI for class %s", 0,
                BUParamsCommon::IMM_CLASS_SP_NAME.c_str()));
        EVENT((CPS_BUAP_Events::registerOI_fail, 0, ""));
        return;
    }

    // Master file descriptor list or event list
    fd_set masterFDs;
    // Temp file descriptor list
    fd_set readFDs;
    // The maximum FD in the master FD list
    int maxFD = 0;
    // Return value
    int retval;
    // Timeout value
    struct timeval tv;

    // Reset the FD list
    FD_ZERO(&masterFDs);
    FD_ZERO(&readFDs);

    // Add stop event into the master file descriptor.
    FD_SET(m_stopEvent.getFd(), &masterFDs);
    // Add callback event of the OI into the master file descriptor.
    FD_SET(this->getSelObj(), &masterFDs);

    maxFD = max(m_stopEvent.getFd(), this->getSelObj());

    // Run the IMM event loop and dispatch()
    bool running = true;
    //m_stopFD.resetEvent();
    while (running)
    {
        readFDs = masterFDs;

        // Select with 10 secs timeout
        tv.tv_sec = 10;
        tv.tv_usec = 0;

        retval = select(maxFD + 1, &readFDs, NULL, NULL, &tv);

        if (retval == -1)
        {
            if (errno == EINTR)
            {
                // A signal was caught
                continue;
            }

            TRACE((LOG_LEVEL_ERROR, "select error: %d - %s", 0, errno, strerror(errno)));
            break;
        }
        // Select loop timeout
        if (retval == 0)
        {
            continue;
        }

        if (FD_ISSET(m_stopEvent.getFd(), &readFDs))
        {
            TRACE((LOG_LEVEL_INFO, "StaticParamsOI received stop event", 0));
            m_stopEvent.resetEvent();
            running = false;
        }
        if (FD_ISSET(this->getSelObj(), &readFDs))
        {
            TRACE((LOG_LEVEL_INFO, "StaticParamsOI::dispatch()...", 0));
            result = this->dispatch(ACS_APGCC_DISPATCH_ONE);

            if (result != ACS_CC_SUCCESS)
            {
                TRACE((LOG_LEVEL_ERROR, "Failed to dispatch IMM event", 0));
                EVENT((CPS_BUAP_Events::dispatchIMMevent_fail, 0, ""));

                running = false;
            }
        }

    }

    running = false;

    m_running = false;

    // Clean up interaction with IMM
    m_oiHandler.removeClassImpl(this, BUParamsCommon::IMM_CLASS_SP_NAME.c_str());

    // Clean up all pointers
    if (m_pClient)
    {
        delete m_pClient;
        m_pClient = 0;
    }

    if (m_pStaticPars)
    {
        delete m_pStaticPars;
        m_pStaticPars = 0;
    }

    TRACE((LOG_LEVEL_INFO, "StaticParamsOI stopped gracefully", 0));

}

/*============================================================================
    ROUTINE: stop()
 ============================================================================ */
void StaticParamsOI::stop()
{
    newTRACE((LOG_LEVEL_INFO, "StaticParamsOI::stop()", 0));

    m_stopEvent.setEvent();
}


/*============================================================================
    ROUTINE: create()
 ============================================================================ */
ACS_CC_ReturnType StaticParamsOI::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId,
                                         const char *className, const char* parentName,
                                         ACS_APGCC_AttrValues **)
{
    newTRACE((LOG_LEVEL_INFO, "StaticParamsOI::create(%lu, %lu, %s, %s, attr)", 0, oiHandle, ccbId, ((className) ? className : "NULL"), ((parentName) ? parentName : "NULL")));


    ACS_CC_ReturnType result = ACS_CC_SUCCESS;

    return result;
}


/*============================================================================
    ROUTINE: deleted()
 ============================================================================ */
ACS_CC_ReturnType StaticParamsOI::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId,
                                          const char *objName)
{
    newTRACE((LOG_LEVEL_INFO, "StaticParamsOI::deleted(%lu, %lu, %s)", 0, oiHandle, ccbId, ((objName) ? objName : "NULL")));

    ACS_CC_ReturnType result = ACS_CC_FAILURE;

    // Do nothing and object must not be deleted

    return result;
}


/*============================================================================
    ROUTINE: modify()
 ============================================================================ */
ACS_CC_ReturnType StaticParamsOI::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId,
                                         const char *objName, ACS_APGCC_AttrModification **attrMods)
{
    newTRACE((LOG_LEVEL_INFO, "StaticParamsOI::modify(%lu, %lu, %s, attrMods)", 0, oiHandle, ccbId, ((objName) ? objName : "NULL")));

    ACS_CC_ReturnType result = ACS_CC_SUCCESS;

    // Create a Static Parameters messages in order to update the RELADMPAR file
    if (m_pStaticPars == NULL)
    {
        m_pStaticPars = new CPS_BUAP_MSG_static_parameters();
    }

    for (size_t idx = 0; attrMods[idx] != NULL ; ++idx)
     {
         ACS_APGCC_AttrValues modAttr = attrMods[idx]->modAttr;

         if (BUParamsCommon::IMM_ATTR_CLH.compare(modAttr.attrName) == 0)
         {
             uint32_t clh = *(reinterpret_cast<uint32_t *> (modAttr.attrValues[0]));
             // command log handling gets changed
             TRACE(("CLH: %u", 0, clh));
             result = this->validateCLH(clh);
         }
         else if (BUParamsCommon::IMM_ATTR_INCL1.compare(modAttr.attrName) == 0)
         {
             // First file range included gets changed
             uint32_t incl1 = *(reinterpret_cast<uint32_t *> (modAttr.attrValues[0]));
             TRACE(("INCL1: %u", 0, incl1));
             result = this->validateINCL1(incl1);
         }
         else if (BUParamsCommon::IMM_ATTR_INCL2.compare(modAttr.attrName) == 0)
         {
             // Second file range included gets changed
             uint32_t incl2 = *(reinterpret_cast<uint32_t *> (modAttr.attrValues[0]));
             TRACE(("INCL2: %u", 0, incl2));
             result = this->validateINCL2(incl2);
         }
         else if (BUParamsCommon::IMM_ATTR_INCLA.compare(modAttr.attrName) == 0)
         {
             // The oldest accepted birth date gets changed
             string incla = reinterpret_cast<char *> (modAttr.attrValues[0]);
             TRACE(("INCLA: %s", 0, incla.c_str()));
             result = this->validateINCLA(incla.c_str());
         }
         else if (BUParamsCommon::IMM_ATTR_LOAZ.compare(modAttr.attrName) == 0)
         {
             // Log subfile omission attempt gets changed
             uint32_t loaz = *(reinterpret_cast<uint32_t *> (modAttr.attrValues[0]));
             TRACE(("LOAZ: %u", 0, loaz));
             result = this->validateLOAZ(loaz);
         }
         else if (BUParamsCommon::IMM_ATTR_NTAZ.compare(modAttr.attrName) == 0)
         {
             // Number of truncation gets changed
             uint32_t ntaz = *(reinterpret_cast<uint32_t *> (modAttr.attrValues[0]));
             TRACE(("NTAZ: %u", 0, ntaz));
             result = this->validateNTAZ(ntaz);
         }
         else if (BUParamsCommon::IMM_ATTR_NTCZ.compare(modAttr.attrName) == 0)
         {
             // Number of additional commands to be truncated gets changed
             uint32_t ntcz = *(reinterpret_cast<uint32_t *> (modAttr.attrValues[0]));
             TRACE(("NTCZ: %u", 0, ntcz));
             result = this->validateNTCZ(ntcz);
         }
         else if (BUParamsCommon::IMM_ATTR_SUP.compare(modAttr.attrName) == 0)
         {
             // Supervision time gets changed
             uint32_t supTime = *(reinterpret_cast<uint32_t *> (modAttr.attrValues[0]));
             TRACE(("Sup Time: %d", 0, supTime));
             result = this->validateSupTime(supTime);
         }
         else
         {
             TRACE((LOG_LEVEL_WARN, "Unknown configurable attribute: %s", 0, modAttr.attrName));
             this->setExitCode(BUParamsCommon::INVALID_ATTR, BUParamsCommon::errorCodeToString(BUParamsCommon::INVALID_ATTR));
             result = ACS_CC_FAILURE;
         }

         if (result == ACS_CC_FAILURE)
         {
             break;
         }
     }

    TRACE((LOG_LEVEL_INFO, "StaticParamsOI::modify() leaving with result = %d", 0, result));

    return result;

}


/*============================================================================
    ROUTINE: complete()
 ============================================================================ */
ACS_CC_ReturnType StaticParamsOI::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
    newTRACE((LOG_LEVEL_INFO, "StaticParamsOI::complete(%lu, %lu)", 0, oiHandle, ccbId));

    ACS_CC_ReturnType result = ACS_CC_SUCCESS;

    // Create a reply message to receive results from PARMGR
    CPS_BUAP_MSG_write_reply l_rxMsg;

    try
    {
        // Create a client connecting to PARMGR.
        this->connectToParmgr();

        if (m_pStaticPars == NULL)
            throw eUnknown;

        m_pStaticPars->UserData1() = CPS_BUAP_MSG_ID_write_static;

        // Client sends request to PARMGR to write the static parameters.
        m_pClient->TXRX(*m_pStaticPars, l_rxMsg);

        // Getting the result from PARMGR.
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
                break;

            default:
                throw eUnknown;
        }

    }
    catch (eError& ex)
    {
        switch (ex)
        {
            case eNoServer:
                TRACE((LOG_LEVEL_ERROR, "Error - Failed to connect to PARMGR server.", 0));
                break;

            case eLoadInProgress:
                TRACE((LOG_LEVEL_ERROR, "Error - load in progress.", 0));
                break;

            case eCPFSerror:
                TRACE((LOG_LEVEL_ERROR, "Error - CPFS error.", 0));
                break;

            case eUnknown:
            default:
                TRACE((LOG_LEVEL_ERROR, "Error - unknown internal error.", 0));
                break;
        }

        this->setExitCode(BUParamsCommon::SYNC_FAILED, BUParamsCommon::errorCodeToString(BUParamsCommon::SYNC_FAILED));
        result = ACS_CC_FAILURE;
        //return ACS_CC_FAILURE;
    }
    catch (...)
    {
        TRACE((LOG_LEVEL_ERROR, "Error - failed to send signal TXRX.", 0));
        this->setExitCode(BUParamsCommon::SYNC_FAILED, BUParamsCommon::errorCodeToString(BUParamsCommon::SYNC_FAILED));
        result = ACS_CC_FAILURE;
        //return ACS_CC_FAILURE;
    }

    // Delete pointer to client.
    if (m_pClient)
    {
        delete m_pClient;
        m_pClient = 0;
    }

    TRACE((LOG_LEVEL_INFO, "StaticParamsOI::complete() leaves with result = %d", 0, result));
    return result;
}


/*============================================================================
    ROUTINE: abort()
 ============================================================================ */
void StaticParamsOI::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
    newTRACE((LOG_LEVEL_INFO, "StaticParamsOI::abort(%lu, %lu)", 0, oiHandle, ccbId));

    // Delete the m_pStaticPars
    if (m_pStaticPars)
    {
        delete m_pStaticPars;
        m_pStaticPars = 0;
    }

}


/*============================================================================
    ROUTINE: apply()
 ============================================================================ */
void StaticParamsOI::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
    newTRACE((LOG_LEVEL_INFO, "StaticParamsOI::apply(%lu, %lu)", 0, oiHandle, ccbId));

    // Delete the m_pStaticPars
    if (m_pStaticPars)
    {
        delete m_pStaticPars;
        m_pStaticPars = 0;
    }
}


/*============================================================================
    ROUTINE: updateRuntime()
 ============================================================================ */
ACS_CC_ReturnType StaticParamsOI::updateRuntime(const char* p_objName, const char** p_attrName)
{
    newTRACE((LOG_LEVEL_INFO, "StaticParamsOI::updateRuntime(%s, %s)", 0, ((p_objName) ? p_objName : "NULL"),
            ((p_attrName[0]) ? p_attrName[0] : "NULL")));

    ACS_CC_ReturnType result = ACS_CC_SUCCESS;

    // Do nothing

    return result;
}


/*============================================================================
    ROUTINE: adminOperationCallback()
 ============================================================================ */
void StaticParamsOI::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation,
                                         const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,
                                         ACS_APGCC_AdminOperationParamType**)
{
    newTRACE((LOG_LEVEL_INFO, "StaticParamsOIOI::adminOperationCallback(%lu, %lu, %s, %lu, paramList)", 0, oiHandle, invocation, ((p_objName) ? p_objName : "NULL"), operationId));

}

const acs_apgcc_oihandler_V3 &StaticParamsOI::getOIHandler() const
{
    return m_oiHandler;
}

/*============================================================================
    ROUTINE: validateCLH()
 ============================================================================ */
ACS_CC_ReturnType StaticParamsOI::validateCLH(uint32_t clh)
{
    newTRACE(("StaticParamsOI::validateCLH(%u)", 0, clh));

    if ((clh != 0) && (clh != 1))
    {
        TRACE((LOG_LEVEL_ERROR, "Invalid CLH : %u", 0, clh));
        this->setExitCode(BUParamsCommon::INVALID_CLH, BUParamsCommon::errorCodeToString(BUParamsCommon::INVALID_CLH));

        return ACS_CC_FAILURE;
    }

    // Write the value to the m_pStaticPars
    m_pStaticPars->m_ManAuto << (clh & 0xFF);

    return ACS_CC_SUCCESS;
}


/*============================================================================
    ROUTINE: validateINCL1()
 ============================================================================ */
ACS_CC_ReturnType StaticParamsOI::validateINCL1(uint32_t incl1)
{
    newTRACE(("StaticParamsOI::validateINCL1(%u)", 0, incl1));

    if ((incl1 != 0) && (incl1 != 1))
    {
        TRACE((LOG_LEVEL_ERROR, "Invalid INCL1 : %u", 0, incl1));
        this->setExitCode(BUParamsCommon::INVALID_FFR, BUParamsCommon::errorCodeToString(BUParamsCommon::INVALID_FFR));

        return ACS_CC_FAILURE;
    }

    // Write the value to the m_pStaticPars
    m_pStaticPars->m_INCL1 << (incl1 & 0xFF);

    return ACS_CC_SUCCESS;
}


/*============================================================================
    ROUTINE: validateINCL2()
 ============================================================================ */
ACS_CC_ReturnType StaticParamsOI::validateINCL2(uint32_t incl2)
{
    newTRACE(("StaticParamsOI::validateINCL2(%u)", 0, incl2));

    if (((incl2 != 0) && (incl2 < 100)) || (incl2 > 127))
    {
        TRACE((LOG_LEVEL_ERROR, "Invalid INCL2 : %u", 0, incl2));

        this->setExitCode(BUParamsCommon::INVALID_SFR, BUParamsCommon::errorCodeToString(BUParamsCommon::INVALID_SFR));

        return ACS_CC_FAILURE;
    }

    // Write the value to the m_pStaticPars
    m_pStaticPars->m_INCL2 << (incl2 & 0xFF);

    return ACS_CC_SUCCESS;
}


/*============================================================================
    ROUTINE: validateINCLA()
 ============================================================================ */
ACS_CC_ReturnType StaticParamsOI::validateINCLA(const char* date)
{
    newTRACE(("StaticParamsOI::validateINCLA(%s)", 0, date));

    //int century;
    uint32_t year;
    uint32_t month;
    uint32_t day;
    //int count;

    string datetime = "";
    boost::regex expression("^(19|20)([0-9][0-9])[- /.](0[1-9]|1[012])[- /.](0[1-9]|[12][0-9]|3[01])$");
    boost::cmatch matches;

    // date = "YYYY-MM-DD"
    if (regex_search(date, matches, expression, boost::match_posix))
    {
        datetime = string(matches[0].first, matches[0].second);
    }
    else
    {
        TRACE((LOG_LEVEL_ERROR, "Invalid INCLA : %s", 0, date));
        this->setExitCode(BUParamsCommon::INVALID_DATE, BUParamsCommon::errorCodeToString(BUParamsCommon::INVALID_DATE));

        return ACS_CC_FAILURE;
    }

    char_separator<char> sep("-/.");
    tokenizer<char_separator<char> > tok(datetime, sep);
    string parts[3] = "";
    int i = 0;

    for (tokenizer<char_separator<char> >::iterator it=tok.begin(); it != tok.end(); ++it)
    {
        parts[i++] = *it;
    }

    //check i!=3
    if (i != 3)
    {
        TRACE((LOG_LEVEL_ERROR, "Invalid INCLA : %s", 0, date));
        this->setExitCode(BUParamsCommon::INVALID_DATE, BUParamsCommon::errorCodeToString(BUParamsCommon::INVALID_DATE));

        return ACS_CC_FAILURE;
    }

    try
    {
        year  = lexical_cast<uint32_t>(parts[0]);
        month = lexical_cast<uint32_t>(parts[1]);
        day   = lexical_cast<uint32_t>(parts[2]);
    }
    catch (bad_lexical_cast &)
    {
        year  = 0;
        month = 0;
        day   = 0;

        TRACE((LOG_LEVEL_ERROR, "bad_lexical_cast", 0));

        return ACS_CC_FAILURE;
    }

    CPS_BUAP::DateTime dateTime(day, month, year);
    // Check for no future date
    CPS_BUAP::DateTime today;
    if (dateTime > today)
    {
        TRACE((LOG_LEVEL_ERROR, "Error:the oldest accepted birth date > today", 0));
        this->setExitCode(BUParamsCommon::INVALID_DATE, BUParamsCommon::errorCodeToString(BUParamsCommon::INVALID_DATE));

        return ACS_CC_FAILURE;
    }


    if (!dateTime.isValid())
    {
        TRACE((LOG_LEVEL_ERROR, "Invalid date : %s", 0, date));
        this->setExitCode(BUParamsCommon::INVALID_DATE, BUParamsCommon::errorCodeToString(BUParamsCommon::INVALID_DATE));

        return ACS_CC_FAILURE;
    }

    // Write the value to the m_pStaticPars
    uint32_t century   = year/100;

    m_pStaticPars->m_INCLAcentury << (century & 0xFF);
    m_pStaticPars->m_INCLAyear << ((year - century*100) & 0xFF);
    m_pStaticPars->m_INCLAmonth << (month & 0xFF);
    m_pStaticPars->m_INCLAday << (day & 0xFF);

    return ACS_CC_SUCCESS;
}


/*============================================================================
    ROUTINE: validateLOAZ()
 ============================================================================ */
ACS_CC_ReturnType StaticParamsOI::validateLOAZ(uint32_t loaz)
{
    newTRACE(("StaticParamsOI::validateLOAZ(%u)", 0, loaz));

    if ((loaz != 0) && (loaz != 1) && (loaz != 2))
    {
        TRACE((LOG_LEVEL_ERROR, "Invalid LOAZ : %u", 0, loaz));
        this->setExitCode(BUParamsCommon::INVALID_LOG, BUParamsCommon::errorCodeToString(BUParamsCommon::INVALID_LOG));

        return ACS_CC_FAILURE;
    }

    // Write the value to the m_pStaticPars
    m_pStaticPars->m_LOAZ << (loaz & 0xFF);

    return ACS_CC_SUCCESS;
}

/*============================================================================
    ROUTINE: validateNTAZ()
 ============================================================================ */
ACS_CC_ReturnType StaticParamsOI::validateNTAZ(uint32_t ntaz)
{
    newTRACE(("StaticParamsOI::validateNTAZ(%u)", 0, ntaz));

    if (ntaz > 255)
    {
        TRACE((LOG_LEVEL_ERROR, "Invalid NTAZ : %u", 0, ntaz));
        this->setExitCode(BUParamsCommon::INVALID_NTCA, BUParamsCommon::errorCodeToString(BUParamsCommon::INVALID_NTCA));

        return ACS_CC_FAILURE;
    }

    // Write the value to the m_pStaticPars
    m_pStaticPars->m_NTAZ << (ntaz & 0xFF);

    return ACS_CC_SUCCESS;
}


/*============================================================================
    ROUTINE: validateNTCZ()
 ============================================================================ */
ACS_CC_ReturnType StaticParamsOI::validateNTCZ(uint32_t ntcz)
{
    newTRACE(("StaticParamsOI::validateNTCZ(%u)", 0, ntcz));

    if (ntcz > 255)
    {
        TRACE((LOG_LEVEL_ERROR, "Invalid NTCZ : %u", 0, ntcz));
        this->setExitCode(BUParamsCommon::INVALID_NTCZ, BUParamsCommon::errorCodeToString(BUParamsCommon::INVALID_NTCZ));

        return ACS_CC_FAILURE;
    }

    // Write the value to the m_pStaticPars
    m_pStaticPars->m_NTCZ << (ntcz & 0xFF);

    return ACS_CC_SUCCESS;
}


/*============================================================================
    ROUTINE: validateSupTime()
 ============================================================================ */
ACS_CC_ReturnType StaticParamsOI::validateSupTime(uint32_t supTime)
{
    newTRACE(("StaticParamsOI::validateSupTime(%u)", 0, supTime));

    if ((supTime < 30) || (supTime > 2400))
    {
        TRACE((LOG_LEVEL_ERROR, "Invalid SUP : %u", 0, supTime));
        this->setExitCode(BUParamsCommon::INVALID_SUPTIME, BUParamsCommon::errorCodeToString(BUParamsCommon::INVALID_SUPTIME));

        return ACS_CC_FAILURE;
    }

    // Write the value to the m_pStaticPars
    m_pStaticPars->m_SUP << ((supTime/10) & 0xFF);

    return ACS_CC_SUCCESS;
}


//***************************************************************************
//    isRunning()
//***************************************************************************

bool StaticParamsOI::isRunning() const
{
    //newTRACE(("StaticParamsOI::isRunning() - %d", 0, (int)m_running));
    return m_running;
}

//***************************************************************************
//    waitUntilRunning()
//***************************************************************************
void StaticParamsOI::waitUntilRunning()
{
    newTRACE(("StaticParamsOI::waitUntilRunning()", 0));

    boost::unique_lock<boost::mutex> lock(m_mutex);

    while (m_running == false)
    {
        TRACE((LOG_LEVEL_INFO, "Wait until StaticParamsOI Thread signal", 0));
        m_condition.wait(lock);
        TRACE((LOG_LEVEL_INFO, "StaticParamsOI Thread signaled", 0));
    }
}

//***************************************************************************
//    connectToParmgr()
//***************************************************************************
void StaticParamsOI::connectToParmgr()
{
    newTRACE((LOG_LEVEL_INFO, "StaticParamsOI::connectToParmgr()", 0))

    try
    {
        // Create a new client connecting to BUPARMGR server
        m_pClient = new CPS_BUAP_Client("BUPARMGR", 10, 1);
    }
    catch ( ... )
    {
        EVENT((CPS_BUAP_Events::no_server, 1));
        this->setExitCode(BUParamsCommon::SYNC_FAILED, BUParamsCommon::errorCodeToString(BUParamsCommon::SYNC_FAILED));
        throw eNoServer;
    }
}






