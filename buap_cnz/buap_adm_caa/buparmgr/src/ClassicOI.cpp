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
 *  ClassicOI.cpp
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
 * === Revision history ================================
 * 2012-02-20   XNGUDAN    Create
 * 2012-03-08   XNGUDAN    Update after model review
 * =====================================================
 *
 */

#include "ClassicOI.h"
#include "BUParamsOM.h"
#include "CPS_BUAP_Trace.h"
#include "BUParamsCommon.h"
#include "acs_apgcc_paramhandling.h"
#include "CPS_BUAP_EventReporter.h"
#include "CPS_BUAP_Events.h"
#include "CPS_BUAP_Exception.h"
#include "CPS_BUAP_Config.h"

#include <time.h>
#include <cerrno>
#include <cstring>


using namespace std;
using namespace boost;


namespace actionResult
{
    int SUCCESS = 1;  // SA_AIS_OK
    int FAILED = 21;  // SA_AIS_ERR_FAILED_OPERATION
}

CPS_BUAP_CS ClassicOI::s_CSession;


/*============================================================================
    ROUTINE: ClassicOI()
 ============================================================================ */
ClassicOI::ClassicOI():
        acs_apgcc_objectimplementerinterface_V3(BUParamsCommon::IMM_CLASSIC_OBJ_DN,
                BUParamsCommon::IMM_IMPL_PARENT_NAME, ACS_APGCC_ONE),
        m_running(false), m_isConfig(false),m_Flag(1), m_pRPF(0),
        m_isStaticValid(0), m_isDynamicValid(0), m_isLAReloadValid(0),
        m_isCmdLogRefValid(0), m_isDelayedMValid(0)
{
	// change for Tracing order
    //newTRACE((LOG_LEVEL_INFO, "ClassicOI::ClassicOI()", 0));
}

/*============================================================================
    ROUTINE: ~ClassicOI()
 ============================================================================ */
ClassicOI::~ClassicOI()
{
	// change for Tracing order
    //newTRACE((LOG_LEVEL_INFO, "ClassicOI::~ClassicOI()", 0));

    // Delete pointer to RELADMPAR file object
    if (m_pRPF)
    {
        delete m_pRPF;
        m_pRPF = 0;
    }

}


/*============================================================================
    ROUTINE: run()
 ============================================================================ */
void ClassicOI::run()
{
    newTRACE((LOG_LEVEL_INFO, "ClassicOI::run()", 0));

    // Reset events
    m_stopEvent.resetEvent();
    m_updateEvent.resetEvent();

    ACS_CC_ReturnType result = ACS_CC_SUCCESS;
    bool isClassic = false;
    bool isObjectInitialized = false;

    // Use condition variable to notify other thread that ClassicOI is started
    // to create objects.
    {
        boost::lock_guard<boost::mutex> lock(m_mutex);

        try
        {
            // Check if system is Classic.
            if ((isClassic = CPS_BUAP_Config::instance().isClassicCP()) == true)
            {
                BUParamsOM buparamsOM;
                // Initialize objects
                isObjectInitialized = buparamsOM.createInitialObjects();

                if (buparamsOM.isInitialized())
                {
                    // Clean up interaction with IMM
                    buparamsOM.finalize();
                }
                else
                {
                    // Create parameter objects failed, exit.
                    TRACE((LOG_LEVEL_ERROR, "Failed to initialize the classic parameters.", 0));
                    EVENT((CPS_BUAP_Events::initializeObj_failed, 0, ""));
                }

                // Synchronize data in local file RELADMPAR to IMM
                 try
                 {
                     if (isObjectInitialized)
                     {
                         this->synchronizeData();
                     }
                 }
                 catch (...)
                 {
                     TRACE((LOG_LEVEL_ERROR, "Cannot access to RELADMPAR file", 0));
                     isObjectInitialized = false;
                 }

            }
        }
        catch (...)
        {
            isClassic = false;
        }

        m_running = true;
    }
    m_condition.notify_one();

    // If System is not Classic or cannot create classic objects,
    // so no need to run OI, going to exit.
    if ((isClassic == false)||(isObjectInitialized == false))
    {
        EVENT((CPS_BUAP_Events::OInotrunning, 0, ""));
        return;
    }

    // Register as OI for Classic class.
    result = m_oiHandler.addClassImpl(this, BUParamsCommon::IMM_CLASSIC_CLASS_NAME.c_str());
    //result = m_oiHandler.addObjectImpl(this);

    if (result != ACS_CC_SUCCESS)
    {
        TRACE((LOG_LEVEL_ERROR, "Failed to register as OI for class %s", 0,
                BUParamsCommon::IMM_CLASSIC_CLASS_NAME.c_str()));
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
    // Timeout value for select loop
    struct timeval tv;

    // Reset the FD list
    FD_ZERO(&masterFDs);
    FD_ZERO(&readFDs);

    // Add stop event into the master file descriptor.
    FD_SET(m_stopEvent.getFd(), &masterFDs);
    FD_SET(m_updateEvent.getFd(), &masterFDs);

    // Add callback event of the OI into the master file descriptor.
    FD_SET(this->getSelObj(), &masterFDs);

    maxFD = max(m_stopEvent.getFd(), this->getSelObj());

    maxFD = max(m_updateEvent.getFd(), maxFD);

    // Synchronize the runtime attributes with the local file
    this->updateRuntimeAttr(BUParamsCommon::IMM_CLASSIC_OBJ_DN,
            BUParamsCommon::IMM_ATTR_STATIC_VALID, m_isStaticValid);

    this->updateRuntimeAttr(BUParamsCommon::IMM_CLASSIC_OBJ_DN,
            BUParamsCommon::IMM_ATTR_DYNAMIC_VALID, m_isDynamicValid);

    this->updateRuntimeAttr(BUParamsCommon::IMM_CLASSIC_OBJ_DN,
            BUParamsCommon::IMM_ATTR_LASTRELOAD_VALID, m_isLAReloadValid);

    this->updateRuntimeAttr(BUParamsCommon::IMM_CLASSIC_OBJ_DN,
            BUParamsCommon::IMM_ATTR_CMDLOGREF_VALID, m_isCmdLogRefValid);

    this->updateRuntimeAttr(BUParamsCommon::IMM_CLASSIC_OBJ_DN,
            BUParamsCommon::IMM_ATTR_DMEASURES_VALID, m_isDelayedMValid);

    // Run the IMM event loop and dispatch()
    bool running = true;
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
        // Select timeout
        if (retval == 0)
        {
            continue;
        }
        // Receive stop event
        if (FD_ISSET(m_stopEvent.getFd(), &readFDs))
        {
            TRACE((LOG_LEVEL_INFO, "ClassicOI received stop event", 0));
            m_stopEvent.resetEvent();
            running = false;
        }
        // Receive update runtime attributes event
        if (FD_ISSET(m_updateEvent.getFd(), &readFDs))
        {
            m_updateEvent.resetEvent();
            if (m_isConfig == true)
            {
                // OI is still in modify/complete progress, do nothing
                //::nanosleep(&req, &rem);
                //this->notifyToUpdateIsValidAttrs();
            }
            else
            {
                TRACE((LOG_LEVEL_INFO, "Update isValid attributes", 0));

                 int isValid = 1;
                 // Check m_Flag and update the corresponding attributes
                 if (m_Flag & StaticPars)
                 {
                     this->updateRuntimeAttr(BUParamsCommon::IMM_CLASSIC_OBJ_DN,
                             BUParamsCommon::IMM_ATTR_STATIC_VALID, isValid);
                 }
                 if (m_Flag & DynamicPars)
                 {
                     this->updateRuntimeAttr(BUParamsCommon::IMM_CLASSIC_OBJ_DN,
                             BUParamsCommon::IMM_ATTR_DYNAMIC_VALID, isValid);
                 }
                 if (m_Flag & LastAutoReload)
                 {
                     this->updateRuntimeAttr(BUParamsCommon::IMM_CLASSIC_OBJ_DN,
                             BUParamsCommon::IMM_ATTR_LASTRELOAD_VALID, isValid);
                 }
                 if (m_Flag & CommandLogReferences)
                 {
                     this->updateRuntimeAttr(BUParamsCommon::IMM_CLASSIC_OBJ_DN,
                             BUParamsCommon::IMM_ATTR_CMDLOGREF_VALID, isValid);
                 }
                 if (m_Flag & DelayedMeasures)
                 {
                     this->updateRuntimeAttr(BUParamsCommon::IMM_CLASSIC_OBJ_DN,
                             BUParamsCommon::IMM_ATTR_DMEASURES_VALID, isValid);
                 }

                 // Reset the flags
                 m_isConfig = false;
                 AutoCS a(s_CSession);
                 {
                     m_Flag = 1;
                 }
            }

        }
        // Receive Callback from the IMM
        if (FD_ISSET(this->getSelObj(), &readFDs))
        {
            TRACE((LOG_LEVEL_INFO, "ClassicOI::dispatch()...", 0));
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
    m_oiHandler.removeClassImpl(this, BUParamsCommon::IMM_CLASSIC_CLASS_NAME.c_str());

    if (m_pRPF)
    {
        delete m_pRPF;
        m_pRPF = 0;
    }

    TRACE((LOG_LEVEL_INFO, "ClassicOI stopped gracefully", 0));

}

/*============================================================================
    ROUTINE: stop()
 ============================================================================ */
void ClassicOI::stop()
{
    newTRACE((LOG_LEVEL_INFO, "ClassicOI::stop()", 0));

    m_stopEvent.setEvent();
}


/*============================================================================
    ROUTINE: create()
 ============================================================================ */
ACS_CC_ReturnType ClassicOI::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId,
                                         const char *className, const char* parentName,
                                         ACS_APGCC_AttrValues **)
{
    newTRACE((LOG_LEVEL_INFO, "ClassicOI::create(%lu, %lu, %s, %s, attr)", 0, oiHandle, ccbId, ((className) ? className : "NULL"), ((parentName) ? parentName : "NULL")));

    ACS_CC_ReturnType result = ACS_CC_SUCCESS;

    return result;
}


/*============================================================================
    ROUTINE: deleted()
 ============================================================================ */
ACS_CC_ReturnType ClassicOI::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId,
                                          const char *objName)
{
    newTRACE((LOG_LEVEL_INFO, "ClassicOI::deleted(%lu, %lu, %s)", 0, oiHandle, ccbId, ((objName) ? objName : "NULL")));

    ACS_CC_ReturnType result = ACS_CC_FAILURE;

    // Do nothing and object must not be deleted

    return result;
}


/*============================================================================
    ROUTINE: modify()
 ============================================================================ */
ACS_CC_ReturnType ClassicOI::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId,
                                         const char *objName, ACS_APGCC_AttrModification **)
{
    newTRACE((LOG_LEVEL_INFO, "ClassicOI::modify(%lu, %lu, %s, attrMods)", 0, oiHandle, ccbId, ((objName) ? objName : "NULL")));

    ACS_CC_ReturnType result = ACS_CC_SUCCESS;

    // Indicate OI in configure progress
    m_isConfig = true;

    return result;

}


/*============================================================================
    ROUTINE: complete()
 ============================================================================ */
ACS_CC_ReturnType ClassicOI::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
    newTRACE((LOG_LEVEL_INFO, "ClassicOI::complete(%lu, %lu)", 0, oiHandle, ccbId));

    ACS_CC_ReturnType result = ACS_CC_SUCCESS;

    // Indicate OI in configure progress
    m_isConfig = true;

    return result;
}


/*============================================================================
    ROUTINE: abort()
 ============================================================================ */
void ClassicOI::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
    newTRACE((LOG_LEVEL_INFO, "ClassicOI::abort(%lu, %lu)", 0, oiHandle, ccbId));

    // Indicate OI is not in configure progress
    m_isConfig = false;
    this->notifyToUpdateIsValidAttrs();
}


/*============================================================================
    ROUTINE: apply()
 ============================================================================ */
void ClassicOI::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
    newTRACE((LOG_LEVEL_INFO, "ClassicOI::apply(%lu, %lu)", 0, oiHandle, ccbId));

    // Indicate OI is not in configure progress
    m_isConfig = false;
    this->notifyToUpdateIsValidAttrs();
}


/*============================================================================
    ROUTINE: updateRuntime()
 ============================================================================ */
ACS_CC_ReturnType ClassicOI::updateRuntime(const char* p_objName, const char** p_attrName)
{
    newTRACE((LOG_LEVEL_INFO, "ClassicOI::updateRuntime(%s, %s)", 0, ((p_objName) ? p_objName : "NULL"),
            ((p_attrName[0]) ? p_attrName[0] : "NULL")));

    ACS_CC_ReturnType result = ACS_CC_SUCCESS;

    // Do nothing

    return result;
}


/*============================================================================
    ROUTINE: adminOperationCallback()
 ============================================================================ */
void ClassicOI::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation,
                                         const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,
                                         ACS_APGCC_AdminOperationParamType**)
{
    newTRACE((LOG_LEVEL_INFO, "ClassicOI::adminOperationCallback(%lu, %lu, %s, %lu, paramList)", 0, oiHandle, invocation, ((p_objName) ? p_objName : "NULL"), operationId));

    //m_isConfig = true;
    
    int errorCode = 0;
 
    // Reset the Static Parameters
    if (this->resetStatic(errorCode) != ACS_CC_SUCCESS)
    {
        TRACE((LOG_LEVEL_ERROR, "resetStatic() failed", 0));

        std::vector<ACS_APGCC_AdminOperationParamType> outParameters;
        ACS_APGCC_AdminOperationParamType msgError1;
        //char errorMsg[] = "actionFailed";
        if (errorCode == (int) BUParamsCommon::ACTION_SYNCFAILED)
        {
            msgError1.attrName = (char*) BUParamsCommon::errorCodeToString(BUParamsCommon::ACTION_SYNCFAILED).c_str();
            msgError1.attrType = ATTR_STRINGT;
            msgError1.attrValues = reinterpret_cast<void*>((char*) BUParamsCommon::SYNCFAILED_MSG.c_str());
        }
        else
        {
            msgError1.attrName = (char*) BUParamsCommon::errorCodeToString(BUParamsCommon::ACTION_INITFAILED).c_str();
            msgError1.attrType = ATTR_STRINGT;
            msgError1.attrValues = reinterpret_cast<void*>((char*) BUParamsCommon::INITFAILED_MSG.c_str());
        }

        outParameters.push_back(msgError1);
 
        // Report error to COM-CLI     
        this->adminOperationResult(oiHandle, invocation, actionResult::FAILED, outParameters);

//        this->setExitCode(BUParamsCommon::INVALID_ACTION,
//                BUParamsCommon::errorCodeToString(BUParamsCommon::INVALID_ACTION));

        return;
    }

    this->adminOperationResult(oiHandle, invocation, actionResult::SUCCESS);

    //m_isConfig = false;

    TRACE((LOG_LEVEL_INFO, "ClassicOI::adminOperationCallback() leaving...", 0));
}

const acs_apgcc_oihandler_V3 &ClassicOI::getOIHandler() const
{
    return m_oiHandler;
}

//***************************************************************************
//    isRunning()
//***************************************************************************

bool ClassicOI::isRunning() const
{
    //newTRACE(("ClassicOI::isRunning() - %d", 0, (int)m_running));
    return m_running;
}

//***************************************************************************
//    waitUntilRunning()
//***************************************************************************
void ClassicOI::waitUntilRunning()
{
    newTRACE(("ClassicOI::waitUntilRunning()", 0));

    boost::unique_lock<boost::mutex> lock(m_mutex);

    while (m_running == false)
    {
        TRACE((LOG_LEVEL_INFO, "Wait until ClassicOI Thread signal", 0));
        m_condition.wait(lock);
        TRACE((LOG_LEVEL_INFO, "ClassicOI Thread signaled", 0));
    }
}

//***************************************************************************
//    resetStatic()
//***************************************************************************
ACS_CC_ReturnType ClassicOI::resetStatic(int &errorCode)
{
    newTRACE((LOG_LEVEL_INFO, "ClassicOI::resetStatic()", 0));
    ACS_CC_ReturnType result = ACS_CC_SUCCESS;
    
    BUParamsOM buparamsOM;
    // Initialize interaction with IMM
    if (!buparamsOM.isInitialized())
    {
        errorCode = (int) BUParamsCommon::ACTION_INITFAILED;
        result = ACS_CC_FAILURE;
        return result;
    }

    BUParamsCommon::StaticParam_ staticPars;

    staticPars.clh = 1;
    staticPars.incl1 = 0;
    staticPars.incl2 = 0;
    staticPars.incla = "1990-01-01";
    staticPars.loaz = 0;
    staticPars.ntaz = 0;
    staticPars.ntcz = 10;
    staticPars.sup = 60;
    std::string transName("resetStaticParams");
    // Reset the attributes of Static Parameters to default values
    buparamsOM.updateStaticParams(staticPars, transName);

    // Apply all requests associated at the transaction name
    if (!buparamsOM.applyReq(transName))
    {
        errorCode = (int) BUParamsCommon::ACTION_SYNCFAILED;
        result = ACS_CC_FAILURE;
    }

    // Finalize interaction with IMM automatically called in destructor of BUParamsOM

    return result;
}


//***************************************************************************
//    synchronizeData()
//***************************************************************************
void ClassicOI::synchronizeData()
{
    newTRACE((LOG_LEVEL_INFO, "ClassicOI::synchronizeData()", 0));

    // Get the physical path of the "RELADMPAR" file
    const char* reloadFileName = CPS_BUAP_Config::instance().cpsReladmpar().c_str();

    if (m_pRPF == NULL)
    {
        try
        {
            m_pRPF = new CPS_BUAP_RPF(reloadFileName);
        }
        catch (CPS_BUAP_Exception &)
        {
            EVENT((CPS_BUAP_Events::reladmpar_fail, 0, reloadFileName))
            throw;
        }
    }

    BUParamsOM buparamsOM;
    if (!buparamsOM.isInitialized())
    {
        return;
    }
    // Read data from RELADMPAR file and start synchronizing with IMM database
    std::string transName("Synchronize CP Backup Reload File");
    int isValid = 0;

    // *********************************************************
    // Modify the attributes of Dynamic Parameters
    // *********************************************************
    isValid = m_pRPF->getReloadFile().Validities.Value & DynamicPars;
    m_isDynamicValid = isValid ? 1 : 0;

    BUParamsCommon::DynamicParam_ dynamicPars;
    dynamicPars.state = (int)m_pRPF->getReloadFile().DynamicPar.State;

    char buf[20];
    // Store the expiring time.
    sprintf(buf, "%02d%02d-%02d-%02d %02d:%02d",
            m_pRPF->getReloadFile().DynamicPar.EXP.century,
            m_pRPF->getReloadFile().DynamicPar.EXP.year,
            m_pRPF->getReloadFile().DynamicPar.EXP.month,
            m_pRPF->getReloadFile().DynamicPar.EXP.day,
            m_pRPF->getReloadFile().DynamicPar.EXP.hour,
            m_pRPF->getReloadFile().DynamicPar.EXP.minute);

    dynamicPars.exp = std::string(buf);

    // If Dynamic Parameters is invalid, clear all the remaining attributes
    // to default value
//    if (isValid)
//    {
        dynamicPars.cffr = (int)m_pRPF->getReloadFile().DynamicPar.NumFFR;
        dynamicPars.csfr = (m_pRPF->getReloadFile().DynamicPar.NumSFR == 99 ?
                                0 : m_pRPF->getReloadFile().DynamicPar.NumSFR);
        dynamicPars.cta = (int)m_pRPF->getReloadFile().DynamicPar.CTA;
        dynamicPars.nct = (int)m_pRPF->getReloadFile().DynamicPar.NCT;
        dynamicPars.fold = (int)m_pRPF->getReloadFile().DynamicPar.NOld;
        dynamicPars.sold = (int)m_pRPF->getReloadFile().DynamicPar.KOld;
        dynamicPars.lpf = (int)m_pRPF->getReloadFile().DynamicPar.LPF;
        dynamicPars.omission = (int)m_pRPF->getReloadFile().DynamicPar.Omission;
//    }
/*
    else
    {
        dynamicPars.cffr = 0;
        dynamicPars.csfr = 0;
        dynamicPars.cta = 0;
        dynamicPars.nct = 0;
        dynamicPars.fold = 0;
        dynamicPars.sold = 0;
        dynamicPars.lpf = 0;
        dynamicPars.omission = 0;
    }
*/

    // Update the dynamic parameters in IMM
    buparamsOM.updateDynamicParams(dynamicPars, transName);

    // *********************************************************
    // Modify the attributes of Last Auto Reload Parameters
    // *********************************************************
    isValid = m_pRPF->getReloadFile().Validities.Value & LastAutoReload;
    m_isLAReloadValid = isValid ? 1 : 0;

    BUParamsCommon::LastAutoReload_ lastReloadPar;
    // If Last Auto Reload Parameters is invalid, clear all the remaining attributes
    // to default value
//    if (isValid)
//    {

        lastReloadPar.clh = (int)m_pRPF->getReloadFile().LastAutoReload.ManAuto;
        lastReloadPar.nct = (int)m_pRPF->getReloadFile().LastAutoReload.NCT;
        lastReloadPar.gfile = (int)m_pRPF->getReloadFile().LastAutoReload.LRG;
        lastReloadPar.lpgf = (int)m_pRPF->getReloadFile().LastAutoReload.LRGF;
        lastReloadPar.omission = (int)m_pRPF->getReloadFile().LastAutoReload.Omission;
        lastReloadPar.alog = (int)m_pRPF->getReloadFile().LastAutoReload.AssClogNum;
        lastReloadPar.sup = (int)m_pRPF->getReloadFile().LastAutoReload.SUP*10;

        memset(buf, ' ', sizeof(buf));
         // Store the PS RS output time.
         sprintf(buf, "%02d%02d-%02d-%02d %02d:%02d", m_pRPF->getReloadFile().LastAutoReload.OPSRS.century,
                                                      m_pRPF->getReloadFile().LastAutoReload.OPSRS.year,
                                                      m_pRPF->getReloadFile().LastAutoReload.OPSRS.month,
                                                      m_pRPF->getReloadFile().LastAutoReload.OPSRS.day,
                                                      m_pRPF->getReloadFile().LastAutoReload.OPSRS.hour,
                                                      m_pRPF->getReloadFile().LastAutoReload.OPSRS.minute);

        lastReloadPar.opsrs = std::string(buf);

        memset(buf, ' ', sizeof(buf));

         // Store the Small Data Store output time.
         sprintf(buf, "%02d%02d-%02d-%02d %02d:%02d", m_pRPF->getReloadFile().LastAutoReload.ODSS.century,
                                                      m_pRPF->getReloadFile().LastAutoReload.ODSS.year,
                                                      m_pRPF->getReloadFile().LastAutoReload.ODSS.month,
                                                      m_pRPF->getReloadFile().LastAutoReload.ODSS.day,
                                                      m_pRPF->getReloadFile().LastAutoReload.ODSS.hour,
                                                      m_pRPF->getReloadFile().LastAutoReload.ODSS.minute);
         lastReloadPar.odss = std::string(buf);

         memset(buf, ' ', sizeof(buf));

         // Store the Large Data Store output time.
         sprintf(buf, "%02d%02d-%02d-%02d %02d:%02d", m_pRPF->getReloadFile().LastAutoReload.ODSL.century,
                                                      m_pRPF->getReloadFile().LastAutoReload.ODSL.year,
                                                      m_pRPF->getReloadFile().LastAutoReload.ODSL.month,
                                                      m_pRPF->getReloadFile().LastAutoReload.ODSL.day,
                                                      m_pRPF->getReloadFile().LastAutoReload.ODSL.hour,
                                                      m_pRPF->getReloadFile().LastAutoReload.ODSL.minute);
         lastReloadPar.odsl = std::string(buf);
//    }
/*
    else
    {
        lastReloadPar.clh = 0;
        lastReloadPar.nct = 0;
        lastReloadPar.gfile = 0;
        lastReloadPar.lpgf = 0;
        lastReloadPar.omission = 0;
        lastReloadPar.alog = 0;
        lastReloadPar.sup = 0;
        lastReloadPar.opsrs = "0000-00-00 00:00";
        lastReloadPar.odss = "0000-00-00 00:00";
        lastReloadPar.odsl = "0000-00-00 00:00";

    }
*/

    buparamsOM.updateLastReloadParams(lastReloadPar, transName);

    // *********************************************************
    // Modify the attributes of Command Log Reference Parameters
    // *********************************************************
    isValid = m_pRPF->getReloadFile().Validities.Value & CommandLogReferences;
    m_isCmdLogRefValid = isValid ? 1 : 0;

    BUParamsCommon::CommmandLogReference_ cmdLogPar;

//    if (isValid)
//    {
        cmdLogPar.current = (int)m_pRPF->getReloadFile().CLogRefs.Current;
        cmdLogPar.main = (int)m_pRPF->getReloadFile().CLogRefs.Main;
        cmdLogPar.prepared = (int)m_pRPF->getReloadFile().CLogRefs.Prepared;
/*    }
    else
    {
        cmdLogPar.current = 0;
        cmdLogPar.main = 0;
        cmdLogPar.prepared = 0;
    }
*/

    buparamsOM.updateCmdLogParams(cmdLogPar, transName);

    // *********************************************************
    // Modify the attributes of Delayed Measures Parameters
    // *********************************************************
    isValid = m_pRPF->getReloadFile().Validities.Value & DelayedMeasures;
    m_isDelayedMValid = isValid ? 1 : 0;

    BUParamsCommon::DelayedMeasure_ dmPar;

//    if (isValid)
//    {
        dmPar.dm[0] = m_pRPF->getReloadFile().DelayedMeasures.Measures[0];
        dmPar.dm[1] = m_pRPF->getReloadFile().DelayedMeasures.Measures[1];
        dmPar.dm[2] = m_pRPF->getReloadFile().DelayedMeasures.Measures[2];
        dmPar.dm[3] = m_pRPF->getReloadFile().DelayedMeasures.Measures[3];
        dmPar.dm[4] = m_pRPF->getReloadFile().DelayedMeasures.Measures[4];
        dmPar.dm[5] = m_pRPF->getReloadFile().DelayedMeasures.Measures[5];
        dmPar.dm[6] = m_pRPF->getReloadFile().DelayedMeasures.Measures[6];
        dmPar.dm[7] = m_pRPF->getReloadFile().DelayedMeasures.Measures[7];
/*    }
    else
    {
        dmPar.dm[0] = 0;
        dmPar.dm[1] = 0;
        dmPar.dm[2] = 0;
        dmPar.dm[3] = 0;
        dmPar.dm[4] = 0;
        dmPar.dm[5] = 0;
        dmPar.dm[6] = 0;
        dmPar.dm[7] = 0;
    }
*/
    buparamsOM.updateDMeasuresParams(dmPar, transName);

    // *********************************************************
    // Modify the attributes of Static Parameters
    // *********************************************************
    isValid = m_pRPF->getReloadFile().Validities.Value & StaticPars;
    m_isStaticValid = isValid ? 1 : 0;

    BUParamsCommon::StaticParam_ staticPars;

    staticPars.clh = m_pRPF->getReloadFile().StaticPar.ManAuto;
    staticPars.incl1 = m_pRPF->getReloadFile().StaticPar.INCL1;
    staticPars.incl2 = m_pRPF->getReloadFile().StaticPar.INCL2;
    staticPars.loaz = m_pRPF->getReloadFile().StaticPar.LOAZ;
    staticPars.ntaz = m_pRPF->getReloadFile().StaticPar.NTAZ;
    staticPars.ntcz = m_pRPF->getReloadFile().StaticPar.NTCZ;
    staticPars.sup = m_pRPF->getReloadFile().StaticPar.SUP*10;

    // Store the oldest accepted date
    memset(buf, ' ', sizeof(buf));
    sprintf(buf, "%02d%02d-%02d-%02d",
            m_pRPF->getReloadFile().StaticPar.INCLAcentury,
            m_pRPF->getReloadFile().StaticPar.INCLAyear,
            m_pRPF->getReloadFile().StaticPar.INCLAmonth,

            m_pRPF->getReloadFile().StaticPar.INCLAday);

    staticPars.incla = std::string(buf);

    buparamsOM.updateStaticParams(staticPars, transName);

    // *********************************************************
    // Apply all requests associated at the transaction name
    // *********************************************************
    buparamsOM.applyReq(transName);

    // After synchronizing, clear pointer to RELADMPAR file object
    if (m_pRPF)
    {
        delete m_pRPF;
        m_pRPF = 0;
    }
}


//***************************************************************************
//    updateRuntimeAttr()
//***************************************************************************
ACS_CC_ReturnType ClassicOI::updateRuntimeAttr(const std::string& objectName,
                                      const std::string& attrName, int value)
{
    newTRACE((LOG_LEVEL_INFO, "ClassicOI::updateRuntimeAttr(%s, %d)", 0, attrName.c_str(), value));

    ACS_CC_ImmParameter attrVal;

    // Update the "isValid" attribute
    attrVal.attrName = (char *)attrName.c_str();
    attrVal.attrType = ATTR_INT32T;
    attrVal.attrValuesNum = 1;
    void* newValue[1] = {reinterpret_cast<void*>(&value)};

    attrVal.attrValues = newValue;

    if (this->modifyRuntimeObj(objectName.c_str(), &attrVal))
    {
        TRACE((LOG_LEVEL_ERROR, "Failed to modify runtime attribute %s. Error: %d - %s", 0,
                attrName.c_str(),
                this->getInternalLastError(), this->getInternalLastErrorText()));

        return ACS_CC_FAILURE;
    }

    return ACS_CC_SUCCESS;
}

//***************************************************************************
//    notifyToUpdateIsValidAttrs()
//***************************************************************************
void ClassicOI::notifyToUpdateIsValidAttrs()
{
    newTRACE((LOG_LEVEL_INFO, "Notifying to Classic OI to update isValid ", 0));
    m_updateEvent.setEvent();
}

//***************************************************************************
//    setAttrFlag()
//***************************************************************************
void ClassicOI::setAttrFlag(unsigned char flag)
{
    AutoCS a(s_CSession);
    m_Flag |= flag;
}





