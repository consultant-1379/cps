/*
NAME
   File_name: BUParamsOI.cpp

COPYRIGHT Ericsson AB, Sweden 2012. All rights reserved.

   The Copyright to the computer program(s) herein is the property of Ericsson
   Utvecklings AB, Sweden.
   The program(s) may be used and/or copied only with the written permission from
   Ericsson Utvecklings AB or in accordance with the terms and conditions
   stipulated in the agreement/contract under which the program(s) have been
   supplied.

DESCRIPTION
   Object Implementer (OI) for Non Classic & Cluster CPReload Params.

DOCUMENT NO
   190 89-CAA 109 1409

AUTHOR
   2012-04-20 by Quyen Dao

SEE ALSO


Revision history
----------------
2012-04-20 xquydao Created

*/

#include "BUParamsOI.h"
#include "BUParamsOM.h"
#include "CPS_BUSRV_Trace.h"
#include "Config.h"
#include "SBCList.h"
#include "CPS_Parameters.h"
//#include "CPS_Utils.h"
#include "CodeException.h"
#include "DataFile.h"

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>


using namespace std;

const Config::CLUSTER g_cluster;

BUParamsOI::BUParamsOI() : acs_apgcc_objectimplementerinterface_V3(BUParamsCommon::IMM_IMPL_NAME),
							m_running(false)
{
    //newTRACE((LOG_LEVEL_INFO, "BUParamsOI::BUParamsOI()", 0));
    m_updateInfo.reset();
}

BUParamsOI::~BUParamsOI()
{
    //newTRACE((LOG_LEVEL_INFO, "BUParamsOI::~BUParamsOI()", 0));
}

void BUParamsOI::run()
{
    newTRACE((LOG_LEVEL_INFO, "BUParamsOI::run()", 0));

    // Use condition variable to notify other thread
    // that OI is started
    {
        boost::lock_guard<boost::mutex> lock(m_mutex);
        m_running = true;
    }
    m_condition.notify_one();

    ACS_CC_ReturnType result = ACS_CC_SUCCESS;
    vector<string> classNames;
    classNames.push_back(BUParamsCommon::IMM_CLN_NONCLASSIC);
    classNames.push_back(BUParamsCommon::IMM_CLN_CLUSTER);

    // Register as OI
    result = m_oiHandler.addMultipleClassImpl(this, classNames);

    if (result != ACS_CC_SUCCESS)
    {
        TRACE((LOG_LEVEL_ERROR, "Failed to register as OI for classes %s - %s", 0, classNames[0].c_str(), classNames[1].c_str()));

        EventReporter::instance().write("Unable to register as OI");
        return;
    }

    // Master file descriptor list or event list
    fd_set masterFDs;
    // Temp file descriptor list
    fd_set readFDs;
    // The maximum FD in the master FD list
    int maxFD;
    // Return value
    int retval;
    // Timeout value
    struct timeval tv;

    // Reset the FD list
    FD_ZERO(&masterFDs);
    FD_ZERO(&readFDs);

    // Add OI fd & stop event into master FD list
    FD_SET(this->getSelObj(), &masterFDs);
    FD_SET(m_stopFD.getFd(), &masterFDs);
    TRACE((LOG_LEVEL_INFO, "OI FD: %d - Stop Event FD: %d", 0, this->getSelObj(), m_stopFD.getFd()));
    maxFD = MAX(this->getSelObj(), m_stopFD.getFd());
    
    // Reset to default value before run
    bool running = true;
    m_stopFD.resetEvent();
    m_updateInfo.reset();

    // Run the IMM event loop and disptach()
    while (running)
    {
        readFDs = masterFDs;
        
        // Select with 60 secs timeout
        tv.tv_sec = 60;
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
            EventReporter::instance().write("Select failed");
            break;
        }

        if (retval == 0)
        {
            continue;
        }
        
        if (FD_ISSET(m_stopFD.getFd(), &readFDs))
        {
            TRACE((LOG_LEVEL_INFO, "Received stop event", 0));
            m_stopFD.resetEvent();
            running = false;
        }

        if (FD_ISSET(this->getSelObj(), &readFDs))
        {            
            result = this->dispatch(ACS_APGCC_DISPATCH_ONE);
            
            if (result != ACS_CC_SUCCESS)
            {
                TRACE((LOG_LEVEL_ERROR, "Failed to dispatch IMM event", 0));
                EventReporter::instance().write("Failed to dispatch IMM event");
                running = false;
            }
        }
    }

    running = false;
    m_running = false;

    // Clean up here
    m_oiHandler.removeMultipleClassImpl(this, classNames);

    if (result != ACS_CC_SUCCESS)
    {
        TRACE((LOG_LEVEL_ERROR, "Failed to unregister as OI for classes %s - %s", 0, classNames[0].c_str(), classNames[1].c_str()));

        EventReporter::instance().write("Unable to unregister as OI");
        return;
    }
    else
    {
        TRACE((LOG_LEVEL_INFO, "OI stopped gracefully", 0));
    }
}

void BUParamsOI::stop()
{
    newTRACE((LOG_LEVEL_INFO, "BUParamsOI::stop()", 0));
    
    m_stopFD.setEvent();
}
    
ACS_CC_ReturnType BUParamsOI::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **)
{
    newTRACE((LOG_LEVEL_INFO, "BUParamsOI::create(%lu, %lu, %s, %s, attr)", 0, oiHandle, ccbId, ((className) ? className : "NULL"), ((parentName) ? parentName : "NULL")));


    ACS_CC_ReturnType result = ACS_CC_SUCCESS;

    // Do nothing

    return result;
}

/*============================================================================
    ROUTINE: deleted
 ============================================================================ */
ACS_CC_ReturnType BUParamsOI::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
{
    newTRACE((LOG_LEVEL_INFO, "BUParamsOI::deleted(%lu, %lu, %s)", 0, oiHandle, ccbId, ((objName) ? objName : "NULL")));

    ACS_CC_ReturnType result = ACS_CC_FAILURE;

    // Do nothing and object must not be deleted

    return result;
}

/*============================================================================
    ROUTINE: modify
 ============================================================================ */
ACS_CC_ReturnType BUParamsOI::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
{
    newTRACE((LOG_LEVEL_INFO, "BUParamsOI::modify(%lu, %lu, %s, attrMods)", 0, oiHandle, ccbId, ((objName) ? objName : "NULL")));

    ACS_CC_ReturnType result = ACS_CC_SUCCESS;
 try
 {

    BUParamsCommon::IMM_CP_TYPE cpType = BUParamsCommon::CP_NONE;

    // Check which object (cpType) is being modified
    cpType = BUParamsCommon::objectDNToCpType(objName);

    if (cpType == BUParamsCommon::CP_NONE)
    {
        TRACE((LOG_LEVEL_WARN, "Invalid configuration object: %d", 0, objName));
        this->setExitCode(BUParamsCommon::INVALID_OBJ_NAME, BUParamsCommon::errorCodeToString(BUParamsCommon::INVALID_OBJ_NAME));

        return ACS_CC_FAILURE;
    }

    // Validate the modifying attribute
    m_updateInfo.cpType = cpType;
    for (size_t idx = 0; attrMods[idx] != NULL ; ++idx)
    {
        ACS_APGCC_AttrValues modAttr = attrMods[idx]->modAttr;

        if (BUParamsCommon::IMM_ATTR_FFR.compare(modAttr.attrName) == 0)
        {
            u_int32 ffr = *(reinterpret_cast<u_int32 *> (modAttr.attrValues[0]));
            // First file name gets changed
            TRACE((LOG_LEVEL_INFO, "FFR: %u", 0, ffr));
            m_updateInfo.buParam.ffr = ffr;
            m_updateInfo.updateFlag |= BUParamsCommon::FLG_FFR;
            result = this->validateFFR(ffr, cpType);
        }
        else if (BUParamsCommon::IMM_ATTR_SFR.compare(modAttr.attrName) == 0)
        {
            // Second file range gets changed
            u_int32 sfr = *(reinterpret_cast<u_int32 *> (modAttr.attrValues[0]));
            TRACE((LOG_LEVEL_INFO, "SFR: %u", 0, sfr));
            m_updateInfo.buParam.sfr = sfr;
            m_updateInfo.updateFlag |= BUParamsCommon::FLG_SFR;
            result = this->validateSFR(sfr, cpType);
        }
        else if (BUParamsCommon::IMM_ATTR_ST.compare(modAttr.attrName) == 0)
        {
            // Time Supervision gets changed
            u_int32 supTime = *(reinterpret_cast<u_int32 *> (modAttr.attrValues[0]));
            TRACE((LOG_LEVEL_INFO, "Sup Time: %d", 0, supTime));
            m_updateInfo.buParam.supTime = supTime;
            m_updateInfo.updateFlag |= BUParamsCommon::FLG_ST;
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
  }
  catch(...)
  {
      TRACE((LOG_LEVEL_ERROR, "Unexpected error", 0));
      result = ACS_CC_FAILURE; 
  }

    return result;
}

/*============================================================================
    ROUTINE: complete
 ============================================================================ */
ACS_CC_ReturnType BUParamsOI::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
    newTRACE((LOG_LEVEL_INFO, "BUParamsOI::complete(%lu, %lu)", 0, oiHandle, ccbId));

    ACS_CC_ReturnType result = ACS_CC_SUCCESS;

    try
    {
        BUParamsOM om;
        BUParamsCommon::BUParam buparam;

        // Retrieve the buparam from IMM
        if (!om.getBUParam(buparam, m_updateInfo.cpType))
        {
            this->setExitCode(BUParamsCommon::BUPARAM_OBTAINING_FAILURE, BUParamsCommon::errorCodeToString(BUParamsCommon::BUPARAM_OBTAINING_FAILURE));
            return ACS_CC_FAILURE;

            //break;
        }

        // Overwrite the COM modified one
        if (m_updateInfo.updateFlag &  BUParamsCommon::FLG_FFR)
        {
            buparam.ffr = m_updateInfo.buParam.ffr;
        }

        if (m_updateInfo.updateFlag &  BUParamsCommon::FLG_SFR)
        {
            buparam.sfr = m_updateInfo.buParam.sfr;
        }

        if (m_updateInfo.updateFlag &  BUParamsCommon::FLG_ST)
        {
            buparam.supTime = m_updateInfo.buParam.supTime;
        }

        switch (m_updateInfo.cpType)
        {
        case BUParamsCommon::CP_SINGLE:
        case BUParamsCommon::CP_SPX1:
        case BUParamsCommon::CP_SPX2:
        {

            u_int16 cpId = BUParamsCommon::cpTypeToCPID(m_updateInfo.cpType);

            // Create directory structure
            Config::instance().createDirStructure(cpId);

            // Save to file
            Config::instance().save(buparam.supTime, buparam.ffr, buparam.sfr, cpId);

            // Reset the reload sequence
            DataFile::startReloadSeq(true, cpId);

            break;
        }

        case BUParamsCommon::CP_CLUSTER: {
            // Create directory structure
            Config::instance().createDirStructure(g_cluster);

            // Save to file
            Config::instance().save(buparam.supTime, buparam.ffr, buparam.sfr, g_cluster);

            // Reset the reload sequence at cluster level
    		DataFile::startReloadSeq(true, g_cluster);
    		DataFile::lastLoadedSBC(SBCId(static_cast<u_int16 >(0)), g_cluster);	// Reset last loaded sbc
    		DataFile::srmReloadSBC(SBCId(static_cast<u_int16 >(0xFF)), g_cluster);

    		// Reset the reload sequence at blade level
    		ACS_CS_API_IdList cpList;
    		ACS_CS_API_CP * CP = ACS_CS_API::createCPInstance();

    		ACS_CS_API_NS::CS_API_Result res = CP->getCPList(cpList);

    		if (res != ACS_CS_API_NS::Result_Success) {
    			ACS_CS_API::deleteCPInstance(CP);
                this->setExitCode(BUParamsCommon::UNKNOWN, BUParamsCommon::errorCodeToString(BUParamsCommon::UNKNOWN));
                return ACS_CC_FAILURE;
    		}

    		DateTime now;
    		now.setNull();
    		for (u_int32 i = 0; i < cpList.size(); i++) {
    			CPID cpId = cpList[i];
    			if (cpId > 63) continue;
    			const Config::CLUSTER t(cpId, true);

    			DataFile::startReloadSeq(true, t);
    			DataFile::lastLoadedSBC(SBCId(static_cast<u_int16 >(0)), t);	// Reset last loaded sbc
    			DataFile::lastLoadedTimeForFallback(now, t);
    			DataFile::srmReloadSBC(SBCId(static_cast<u_int16 >(0xFF)), t);
    		}
    		ACS_CS_API::deleteCPInstance(CP);
            break;
        }

        default:
            TRACE((LOG_LEVEL_WARN, "Unsupported modified parameter", 0));
            this->setExitCode(BUParamsCommon::INVALID_OBJ_NAME, BUParamsCommon::errorCodeToString(BUParamsCommon::INVALID_OBJ_NAME));
            result = ACS_CC_FAILURE;

            break;
        };
    }
    catch (...)
    {
        TRACE((LOG_LEVEL_ERROR, "Unexpected error", 0));
        this->setExitCode(BUParamsCommon::UNKNOWN, BUParamsCommon::errorCodeToString(BUParamsCommon::UNKNOWN));
        result = ACS_CC_FAILURE;
    }

    return result;
}

/*============================================================================
    ROUTINE: abort
 ============================================================================ */
void BUParamsOI::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
    newTRACE((LOG_LEVEL_INFO, "BUParamsOI::abort(%lu, %lu)", 0, oiHandle, ccbId));

    // Reset update info
    m_updateInfo.reset();
}

/*============================================================================
ROUTINE: apply
============================================================================ */
void BUParamsOI::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
    newTRACE((LOG_LEVEL_INFO, "BUParamsOI::apply(%lu, %lu)", 0, oiHandle, ccbId));

    // Reset update info
    m_updateInfo.reset();
}

/*============================================================================
ROUTINE: updateRuntime
============================================================================ */
ACS_CC_ReturnType BUParamsOI::updateRuntime(const char* p_objName, const char** )
{
    newTRACE((LOG_LEVEL_INFO, "BUParamsOI::updateRuntime(%s)", 0, ((p_objName) ? p_objName : "NULL")));

    ACS_CC_ReturnType result = ACS_CC_SUCCESS;

    // Do nothing

    return result;
}

/*============================================================================
ROUTINE: adminOperationCallback
============================================================================ */
void BUParamsOI::adminOperationCallback(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_InvocationType invocation,
        const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,
        ACS_APGCC_AdminOperationParamType**)
{
    newTRACE((LOG_LEVEL_INFO, "BUParamsOI::adminOperationCallback(%lu, %lu, %s, %lu, paramList)", 0, oiHandle, invocation,
            ((p_objName) ? p_objName : "NULL"), operationId));

    // Do nothing
}

ACS_CC_ReturnType BUParamsOI::validateFFR(u_int32 ffr, BUParamsCommon::IMM_CP_TYPE cpType)
{
    newTRACE((LOG_LEVEL_INFO, "BUParamsOI::validateFFR(%u, %d)", 0, ffr, cpType));

    if ((cpType == BUParamsCommon::CP_CLUSTER && ffr > SBCId::FFR_LAST_CLUSTER_ID) || (cpType != BUParamsCommon::CP_CLUSTER && ffr > SBCId::FFR_LAST_ID))
    {
        TRACE((LOG_LEVEL_WARN, "Invalid FFR: %u", 0, ffr));
        this->setExitCode(BUParamsCommon::INVALID_FFR, BUParamsCommon::errorCodeToString(BUParamsCommon::INVALID_FFR));

        return ACS_CC_FAILURE;
    }

    SBCList sbcList;

    switch (cpType)
    {
    case BUParamsCommon::CP_SINGLE:
    case BUParamsCommon::CP_SPX1:
    case BUParamsCommon::CP_SPX2:
    {
        u_int16 cpId = BUParamsCommon::cpTypeToCPID(cpType);
        sbcList.update(SBCId::FIRST, true, (CPID) cpId);
        break;
    }

    case BUParamsCommon::CP_CLUSTER:
        sbcList.update(SBCId::FIRST, true, g_cluster);
        break;

    default:
        break;
    }

    if (sbcList.count() <= ffr)
    {
        TRACE((LOG_LEVEL_WARN, "Input FFR is not in the FFR range (%d-%d) defined by FMS", 0, SBCId::FFR_FIRST_ID, (SBCId::FFR_FIRST_ID + sbcList.count() - 1)));
        this->setExitCode(BUParamsCommon::FFR_NOT_IN_RANGE, BUParamsCommon::errorCodeToString(BUParamsCommon::FFR_NOT_IN_RANGE));

        return ACS_CC_FAILURE;
    }

    return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType BUParamsOI::validateSFR(u_int32 sfr, BUParamsCommon::IMM_CP_TYPE cpType)
{
    newTRACE((LOG_LEVEL_INFO, "BUParamsOI::validateSFR(%u, %d)", 0, sfr, cpType));

    if (sfr == 0)
    {
        // Nothing to check
        return ACS_CC_SUCCESS;
    }

    if (sfr < SBCId::SFR_FIRST_ID || sfr > SBCId::SFR_LAST_ID)
    {
        TRACE((LOG_LEVEL_WARN, "Invalid SFR: %u", 0, sfr));
        this->setExitCode(BUParamsCommon::INVALID_SFR, BUParamsCommon::errorCodeToString(BUParamsCommon::INVALID_SFR));

        return ACS_CC_FAILURE;
    }

    SBCList sbcList;

    switch (cpType)
    {
    case BUParamsCommon::CP_SINGLE:
    case BUParamsCommon::CP_SPX1:
    case BUParamsCommon::CP_SPX2:
    {
        u_int16 cpId = BUParamsCommon::cpTypeToCPID(cpType);
        sbcList.update(SBCId::SECOND, true, (CPID) cpId);
        break;
    }

    case BUParamsCommon::CP_CLUSTER:
        sbcList.update(SBCId::SECOND, true, g_cluster);
        break;

    default:
        break;
    }

    if (sbcList.count() <=  (u_int32) (sfr - SBCId::SFR_FIRST_ID))
    {
        TRACE((LOG_LEVEL_WARN, "Input SFR is not in the SFR range (%d-%d) defined by FMS", 0, SBCId::SFR_FIRST_ID, (SBCId::SFR_FIRST_ID + sbcList.count() - 1)));
        this->setExitCode(BUParamsCommon::SFR_NOT_IN_RANGE, BUParamsCommon::errorCodeToString(BUParamsCommon::SFR_NOT_IN_RANGE));

        return ACS_CC_FAILURE;
    }

    return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType BUParamsOI::validateSupTime(u_int32 supTime)
{
    newTRACE((LOG_LEVEL_INFO, "BUParamsOI::validateSupTime(%u)", 0, supTime));

    try
    {
        Config::instance().validate(supTime);
    }
    catch (...)
    {
        TRACE((LOG_LEVEL_WARN, "Input SupTime %u is invalid", 0, supTime));
        this->setExitCode(BUParamsCommon::INVALID_SUPTIME, BUParamsCommon::errorCodeToString(BUParamsCommon::INVALID_SUPTIME));

        return ACS_CC_FAILURE;
    }

    return ACS_CC_SUCCESS;
}

void BUParamsOI::waitUntilRunning(void)
{
    newTRACE((LOG_LEVEL_INFO, "BUParamsOI::waitUntilRunning()", 0));

    boost::unique_lock<boost::mutex> lock(m_mutex);

    if (m_running == false) {
		while (m_running == false)
		{
			TRACE((LOG_LEVEL_INFO, "Wait until BUParamsOI Thread signal", 0));
			m_condition.wait(lock);
			TRACE((LOG_LEVEL_INFO, "BUParamsOI Thread signaled", 0));
		}
    }
    else {
    	TRACE((LOG_LEVEL_INFO, "BUParamsOI Thread already ran", 0));
    }
}

