//******************************************************************************
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 2011.
//  All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson Utvecklings AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.
//
//  NAME
//  CPS_BUAP_PARMGR.cpp
//
//  DESCRIPTION 
//  This class, which inherits from CPS_BUAP_Server,
//  implements the controlling logic for the buparmgr process.
//
//  DOCUMENT NO
//	190 89-CAA 109 1413
//
//  AUTHOR 
// 	1999/07/27 by UAB/B/SF Birgit Berggren
//
//*****************************************************************************
// === Revision history ===
// 990727 BIR PA1 Created.
// 991103 BIR PA2 Calls for method RPF->StartUpdate removed.
// 050117 LAN     Remove lock hanlding on file reladmpar
// 111101 XNGUDAN Updated (migrated from Win2k3 to Linux)
//*****************************************************************************


#include "CPS_BUAP_PARMGR.h"
#include "CPS_BUAP_MSG_IDs.h"
#include "CPS_BUAP_Trace.h"
#include "CPS_BUAP_Events.h"
#include "CPS_BUAP_PARMGR_Global.h"
#include "CPS_BUAP_Server.h"
#include "CPS_BUAP_Linux.h"
#include "CPS_BUAP_DateTime.h"
#include "CPS_BUAP_Types.h"
#include "CPS_BUAP_Service.h"

#include "fms_cpf_file.h"

#include <string>


using namespace CPS_BUAP;

//=============================================================================
// Extern variable used to report error.
//=============================================================================
extern CPS_BUAP_Service* pService;
extern bool reportErrorFlag;
extern boost::recursive_mutex mutex;

//******************************************************************************
//	CPS_BUAP_PARMGR()
//******************************************************************************
CPS_BUAP_PARMGR::CPS_BUAP_PARMGR(const char *filename) :
        m_pService(0), m_pRPF(0), m_fileName(filename),
        m_running(false)
{
	// change for Tracing Order
    //newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_PARMGR::CPS_BUAP_PARMGR(filename=%s)", 0, filename));

    // Assign the derive name
    m_deriveName = string("PARMGR");

    //***************************************************************************
    // Initiate the array of thread control objects.
    //***************************************************************************
    //TRACE(("initiating hNTObjectArray(BUPARMGR)", 0))
    hNTObjectArray[ABORT_EVENT] =  m_stopEvent.getFd(); // There's always a
    noOfNTHandles = 1;                                  // handle to the AbortEvent
}

//******************************************************************************
//	~CPS_BUAP_PARMGR()
//******************************************************************************
CPS_BUAP_PARMGR::~CPS_BUAP_PARMGR()
{
	// Change for Tracing Order
    //newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_PARMGR::~CPS_BUAP_PARMGR()", 0));

    if (m_pService)
    {
        delete m_pService;
        m_pService = 0;
    }

    if (m_pRPF)
    {
        delete m_pRPF;
        m_pRPF = 0;
    }
}

//******************************************************************************
//	RXTX()
//******************************************************************************
void CPS_BUAP_PARMGR::RXTX(HANDLE handle, unsigned short rxUserData1,
        unsigned short rxUserData2, CPS_BUAP_Buffer &rxBuffer,
        unsigned short &txUserData1, unsigned short &txUserData2,
        CPS_BUAP_Buffer &txBuffer)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_PARMGR::RXTX(handle=%x,, rxUserData1=%d, rxUserData2=%d)",0, handle, rxUserData1, rxUserData2))

    CPS_BUAP_MSG *txMsg;

    //***************************************************************************
    // Create a RELADMPAR file instance if it has not been created.
    //***************************************************************************
    if(NULL == m_pRPF)
    {
        try
        {
            m_pRPF = new CPS_BUAP_RPF(m_fileName.c_str());
        }
        catch (CPS_BUAP_Exception&)
        {
            EVENT((CPS_BUAP_Events::reladmpar_fail, 0, m_fileName.c_str()))
            throw;
        }
    }

    switch (rxUserData1)
    {
    case CPS_BUAP_MSG_ID_read_static:
    {
        Process_read_static(&txMsg);
    }
        break;

    case CPS_BUAP_MSG_ID_write_static:
    {
        CPS_BUAP_MSG_static_parameters rxMsg;
        rxMsg.UserData1() = rxUserData1;
        rxMsg.UserData2() = rxUserData2;
        rxMsg.Decode(rxBuffer);

        Process_write_static(handle, rxMsg, &txMsg);

    }
        break;

    case CPS_BUAP_MSG_ID_read_dynamic:
    {
        Process_read_dynamic(&txMsg);
    }
        break;

    case CPS_BUAP_MSG_ID_write_dynamic:
    {
        CPS_BUAP_MSG_dynamic_parameters rxMsg;
        rxMsg.UserData1() = rxUserData1;
        rxMsg.UserData2() = rxUserData2;
        rxMsg.Decode(rxBuffer);

        Process_write_dynamic(handle, rxMsg, &txMsg);

    }
        break;

    case CPS_BUAP_MSG_ID_read_LAR:
    {
        Process_read_LAR(&txMsg);
    }
        break;

    case CPS_BUAP_MSG_ID_write_LAR:
    {
        CPS_BUAP_MSG_last_auto_reload rxMsg;
        rxMsg.UserData1() = rxUserData1;
        rxMsg.UserData2() = rxUserData2;
        rxMsg.Decode(rxBuffer);

        Process_write_LAR(handle, rxMsg, &txMsg);

    }
        break;

    case CPS_BUAP_MSG_ID_read_clogrefs:
    {
        Process_read_clogrefs(&txMsg);
    }
        break;

    case CPS_BUAP_MSG_ID_read_dmeasures:
    {
        Process_read_dmeasures(&txMsg);
    }
        break;

    case CPS_BUAP_MSG_ID_read_reladmpar:
    {
        Process_read_reladmpar(&txMsg);
    }
        break;

    case CPS_BUAP_MSG_ID_write_clog:
    {
        CPS_BUAP_MSG_write_clog rxMsg;
        rxMsg.UserData1() = rxUserData1;
        rxMsg.UserData2() = rxUserData2;
        rxMsg.Decode(rxBuffer);

        Process_write_clog(handle, rxMsg, &txMsg);
    }
        break;

    case CPS_BUAP_MSG_ID_write_dmr:
    {
        CPS_BUAP_MSG_write_dmr rxMsg;
        rxMsg.UserData1() = rxUserData1;
        rxMsg.UserData2() = rxUserData2;
        rxMsg.Decode(rxBuffer);

        Process_write_dmr(handle, rxMsg, &txMsg);
    }
        break;

    default:
    {
        EVENT((CPS_BUAP_Events::invalid_message, 0, rxUserData1))
        THROW("Unknown message type received");
        break;
    }
    }

    txMsg->Encode(txBuffer);
    txUserData1 = txMsg->UserData1();
    txUserData2 = txMsg->UserData2();

}

//******************************************************************************
//	Process_read_static()
//******************************************************************************
void CPS_BUAP_PARMGR::Process_read_static(CPS_BUAP_MSG **txMsg)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_PARMGR::Process_read_static()", 0))

    static CPS_BUAP_MSG_static_parameters l_txMsg;
    sValidities l_validities;
    sStaticParameters l_staticParameters;

    try
    {
        m_pRPF->ReadValidities(l_validities);
        m_pRPF->ReadStaticParameters(l_staticParameters);

        l_txMsg.m_Validity = (l_validities.Value & StaticPars) ? Valid
                : Invalid;
        l_txMsg.m_ManAuto << l_staticParameters.ManAuto;
        l_txMsg.m_NTAZ << l_staticParameters.NTAZ;
        l_txMsg.m_NTCZ << l_staticParameters.NTCZ;
        l_txMsg.m_LOAZ << l_staticParameters.LOAZ;
        l_txMsg.m_INCLAcentury << l_staticParameters.INCLAcentury;
        l_txMsg.m_INCLAyear << l_staticParameters.INCLAyear;
        l_txMsg.m_INCLAmonth << l_staticParameters.INCLAmonth;
        l_txMsg.m_INCLAday << l_staticParameters.INCLAday;
        l_txMsg.m_INCL1 << l_staticParameters.INCL1;
        l_txMsg.m_INCL2 << l_staticParameters.INCL2;
        l_txMsg.m_SUP << l_staticParameters.SUP;

        l_txMsg.UserData1() = CPS_BUAP_MSG_ID_read_static_ok;
        l_txMsg.UserData2() = 1;
    } catch (CPS_BUAP_Exception&)
    {
        l_txMsg.UserData1() = CPS_BUAP_MSG_ID_read_static_fail;
        l_txMsg.UserData2() = 1;

        *txMsg = &l_txMsg;
    }

    *txMsg = &l_txMsg;
}

//******************************************************************************
//	Process_read_dynamic()
//******************************************************************************
void CPS_BUAP_PARMGR::Process_read_dynamic(CPS_BUAP_MSG **txMsg)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_PARMGR::Process_read_dynamic()", 0))

    static CPS_BUAP_MSG_dynamic_parameters l_txMsg;
    sValidities l_validities;
    sDynamicParameters l_dynamicParameters;

    try
    {
        m_pRPF->ReadValidities(l_validities);
        m_pRPF->ReadDynamicParameters(l_dynamicParameters);

        l_txMsg.m_Validity << (((l_validities.Value & DynamicPars) ? Valid
                : Invalid) & 0xFF);
        l_txMsg.m_State << l_dynamicParameters.State;
        l_txMsg.m_CTA << l_dynamicParameters.CTA;
        l_txMsg.m_NumFFR << l_dynamicParameters.NumFFR;
        l_txMsg.m_NumSFR << l_dynamicParameters.NumSFR;
        l_txMsg.m_NOld << l_dynamicParameters.NOld;
        l_txMsg.m_KOld << l_dynamicParameters.KOld;
        l_txMsg.m_NCT << l_dynamicParameters.NCT;
        l_txMsg.m_EXPcentury << l_dynamicParameters.EXP.century;
        l_txMsg.m_EXPyear << l_dynamicParameters.EXP.year;
        l_txMsg.m_EXPmonth << l_dynamicParameters.EXP.month;
        l_txMsg.m_EXPday << l_dynamicParameters.EXP.day;
        l_txMsg.m_EXPhour << l_dynamicParameters.EXP.hour;
        l_txMsg.m_EXPminute << l_dynamicParameters.EXP.minute;
        l_txMsg.m_LPF << l_dynamicParameters.LPF;
        l_txMsg.m_Omission << l_dynamicParameters.Omission;

        l_txMsg.UserData1() = CPS_BUAP_MSG_ID_read_dynamic_ok;
        l_txMsg.UserData2() = 1;
    } catch (...)
    {
        l_txMsg.UserData1() = CPS_BUAP_MSG_ID_read_dynamic_fail;
        l_txMsg.UserData2() = 1;

        *txMsg = &l_txMsg;
    }

    *txMsg = &l_txMsg;
}

//******************************************************************************
//	Process_read_LAR()
//******************************************************************************
void CPS_BUAP_PARMGR::Process_read_LAR(CPS_BUAP_MSG **txMsg)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_PARMGR::Process_read_LAR()", 0))

    static CPS_BUAP_MSG_last_auto_reload l_txMsg;
    sValidities l_validities;
    sLastAutoReload l_lastAutoReload;

    try
    {
        m_pRPF->ReadValidities(l_validities);
        m_pRPF->ReadLastAutoReload(l_lastAutoReload);

        l_txMsg.m_Validity << (((l_validities.Value & LastAutoReload) ? Valid
                : Invalid) & 0xFF);
        l_txMsg.m_ManAuto << l_lastAutoReload.ManAuto;
        l_txMsg.m_LRG << l_lastAutoReload.LRG;
        l_txMsg.m_OPSRScentury << l_lastAutoReload.OPSRS.century;
        l_txMsg.m_OPSRSyear << l_lastAutoReload.OPSRS.year;
        l_txMsg.m_OPSRSmonth << l_lastAutoReload.OPSRS.month;
        l_txMsg.m_OPSRSday << l_lastAutoReload.OPSRS.day;
        l_txMsg.m_OPSRShour << l_lastAutoReload.OPSRS.hour;
        l_txMsg.m_OPSRSminute << l_lastAutoReload.OPSRS.minute;
        l_txMsg.m_ODSScentury << l_lastAutoReload.ODSS.century;
        l_txMsg.m_ODSSyear << l_lastAutoReload.ODSS.year;
        l_txMsg.m_ODSSmonth << l_lastAutoReload.ODSS.month;
        l_txMsg.m_ODSSday << l_lastAutoReload.ODSS.day;
        l_txMsg.m_ODSShour << l_lastAutoReload.ODSS.hour;
        l_txMsg.m_ODSSminute << l_lastAutoReload.ODSS.minute;
        l_txMsg.m_ODSLcentury << l_lastAutoReload.ODSL.century;
        l_txMsg.m_ODSLyear << l_lastAutoReload.ODSL.year;
        l_txMsg.m_ODSLmonth << l_lastAutoReload.ODSL.month;
        l_txMsg.m_ODSLday << l_lastAutoReload.ODSL.day;
        l_txMsg.m_ODSLhour << l_lastAutoReload.ODSL.hour;
        l_txMsg.m_ODSLminute << l_lastAutoReload.ODSL.minute;
        l_txMsg.m_LRGF << l_lastAutoReload.LRGF;
        l_txMsg.m_NCT << l_lastAutoReload.NCT;
        l_txMsg.m_Omission << l_lastAutoReload.Omission;
        l_txMsg.m_SUP << l_lastAutoReload.SUP;
        l_txMsg.m_AssClogNum << l_lastAutoReload.AssClogNum;

        l_txMsg.UserData1() = CPS_BUAP_MSG_ID_read_LAR_ok;
        l_txMsg.UserData2() = 1;
    } catch (...)
    {
        l_txMsg.UserData1() = CPS_BUAP_MSG_ID_read_LAR_fail;
        l_txMsg.UserData2() = 1;

        *txMsg = &l_txMsg;
    }

    *txMsg = &l_txMsg;
}

//******************************************************************************
//	Process_read_clogrefs()
//******************************************************************************
void CPS_BUAP_PARMGR::Process_read_clogrefs(CPS_BUAP_MSG **txMsg)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_PARMGR::Process_read_clogrefs()", 0))

    static CPS_BUAP_MSG_command_log_references l_txMsg;
    sValidities l_validities;
    sCommandLogReferences l_commandLogReferences;

    try
    {
        m_pRPF->ReadValidities(l_validities);
        m_pRPF->ReadCommandLogReferences(l_commandLogReferences);

        l_txMsg.m_Validity
                = (l_validities.Value & CommandLogReferences) ? Valid : Invalid;
        l_txMsg.m_Current << l_commandLogReferences.Current;
        l_txMsg.m_Prepared << l_commandLogReferences.Prepared;
        l_txMsg.m_Main << l_commandLogReferences.Main;

        l_txMsg.UserData1() = CPS_BUAP_MSG_ID_read_clogrefs_ok;
        l_txMsg.UserData2() = 1;
    } catch (...)
    {
        l_txMsg.UserData1() = CPS_BUAP_MSG_ID_read_clogrefs_fail;
        l_txMsg.UserData2() = 1;

        *txMsg = &l_txMsg;
    }

    *txMsg = &l_txMsg;
}

//******************************************************************************
//	Process_read_dmeasures()
//******************************************************************************
void CPS_BUAP_PARMGR::Process_read_dmeasures(CPS_BUAP_MSG **txMsg)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_PARMGR::Process_read_dmeasures()", 0))

    static CPS_BUAP_MSG_delayed_measures l_txMsg;
    sValidities l_validities;
    sDelayedMeasures l_delayedMeasures;

    try
    {
        m_pRPF->ReadValidities(l_validities);
        m_pRPF->ReadDelayedMeasures(l_delayedMeasures);

        l_txMsg.m_Validity = (l_validities.Value & DelayedMeasures) ? Valid
                : Invalid;

        for (int i = 0; i < MAX_MEASURES; i++)
        {
            l_txMsg.m_Measures[i] << l_delayedMeasures.Measures[i];
        }

        l_txMsg.UserData1() = CPS_BUAP_MSG_ID_read_dmeasures_ok;
        l_txMsg.UserData2() = 1;
    } catch (...)
    {
        l_txMsg.UserData1() = CPS_BUAP_MSG_ID_read_dmeasures_fail;
        l_txMsg.UserData2() = 1;

        *txMsg = &l_txMsg;
    }

    *txMsg = &l_txMsg;
}

//******************************************************************************
//	Process_write_static()
//******************************************************************************
void CPS_BUAP_PARMGR::Process_write_static(HANDLE handle,
        CPS_BUAP_MSG_static_parameters rxMsg, CPS_BUAP_MSG **txMsg)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_PARMGR::Process_write_static(%d)", 0, handle))

    static CPS_BUAP_MSG_write_reply l_txMsg;
    sStaticParameters l_staticParameters;

    try
    {
        m_pRPF->ReadStaticParameters(l_staticParameters);

        rxMsg.m_ManAuto >> l_staticParameters.ManAuto;
        rxMsg.m_NTAZ >> l_staticParameters.NTAZ;
        rxMsg.m_NTCZ >> l_staticParameters.NTCZ;
        rxMsg.m_LOAZ >> l_staticParameters.LOAZ;
        rxMsg.m_INCLAcentury >> l_staticParameters.INCLAcentury;
        rxMsg.m_INCLAyear >> l_staticParameters.INCLAyear;
        rxMsg.m_INCLAmonth >> l_staticParameters.INCLAmonth;
        rxMsg.m_INCLAday >> l_staticParameters.INCLAday;
        rxMsg.m_INCL1 >> l_staticParameters.INCL1;
        rxMsg.m_INCL2 >> l_staticParameters.INCL2;
        rxMsg.m_SUP >> l_staticParameters.SUP;

        m_pRPF->WriteStaticParameters(l_staticParameters);

        SetupDynamicParameters();

        m_pRPF->FinishUpdate();

        l_txMsg.UserData1() = CPS_BUAP_MSG_ID_write_static_ok;
        l_txMsg.UserData2() = 1;
        l_txMsg.m_ResultCode << CPS_BUAP_MSG_write_reply::success;
    } catch (eWrongKey&)
    {
        l_txMsg.UserData1() = CPS_BUAP_MSG_ID_write_static_fail;
        l_txMsg.UserData2() = 1;
        l_txMsg.m_ResultCode << CPS_BUAP_MSG_write_reply::function_busy;
    } catch (...)
    {
        l_txMsg.UserData1() = CPS_BUAP_MSG_ID_write_static_fail;
        l_txMsg.UserData2() = 1;
        l_txMsg.m_ResultCode << CPS_BUAP_MSG_write_reply::access_error;

        *txMsg = &l_txMsg;
    }

    *txMsg = &l_txMsg;
}

//******************************************************************************
//	Process_write_dynamic()
//******************************************************************************
void CPS_BUAP_PARMGR::Process_write_dynamic(HANDLE handle,
        CPS_BUAP_MSG_dynamic_parameters rxMsg, CPS_BUAP_MSG **txMsg)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_PARMGR::Process_write_dynamic(%d)", 0, handle))

    static CPS_BUAP_MSG_write_reply l_txMsg;
    sDynamicParameters l_dynamicParameters;

    try
    {

        m_pRPF->ReadDynamicParameters(l_dynamicParameters);

        rxMsg.m_State >> l_dynamicParameters.State;
        rxMsg.m_CTA >> l_dynamicParameters.CTA;
        rxMsg.m_NumFFR >> l_dynamicParameters.NumFFR;
        rxMsg.m_NumSFR >> l_dynamicParameters.NumSFR;
        rxMsg.m_NOld >> l_dynamicParameters.NOld;
        rxMsg.m_KOld >> l_dynamicParameters.KOld;
        rxMsg.m_NCT >> l_dynamicParameters.NCT;
        rxMsg.m_EXPcentury >> l_dynamicParameters.EXP.century;
        rxMsg.m_EXPyear >> l_dynamicParameters.EXP.year;
        rxMsg.m_EXPmonth >> l_dynamicParameters.EXP.month;
        rxMsg.m_EXPday >> l_dynamicParameters.EXP.day;
        rxMsg.m_EXPhour >> l_dynamicParameters.EXP.hour;
        rxMsg.m_EXPminute >> l_dynamicParameters.EXP.minute;
        rxMsg.m_LPF >> l_dynamicParameters.LPF;
        rxMsg.m_Omission >> l_dynamicParameters.Omission;

        m_pRPF->WriteDynamicParameters(l_dynamicParameters);

        m_pRPF->FinishUpdate();

        l_txMsg.UserData1() = CPS_BUAP_MSG_ID_write_dynamic_ok;
        l_txMsg.UserData2() = 1;
        l_txMsg.m_ResultCode << CPS_BUAP_MSG_write_reply::success;
    } catch (eWrongKey&)
    {
        l_txMsg.UserData1() = CPS_BUAP_MSG_ID_write_dynamic_fail;
        l_txMsg.UserData2() = 1;
        l_txMsg.m_ResultCode << CPS_BUAP_MSG_write_reply::function_busy;
    } catch (...)
    {
        l_txMsg.UserData1() = CPS_BUAP_MSG_ID_write_dynamic_fail;
        l_txMsg.UserData2() = 1;
        l_txMsg.m_ResultCode << CPS_BUAP_MSG_write_reply::access_error;

        *txMsg = &l_txMsg;
    }

    *txMsg = &l_txMsg;
}

//******************************************************************************
//	Process_write_LAR()
//******************************************************************************
void CPS_BUAP_PARMGR::Process_write_LAR(HANDLE handle,
        CPS_BUAP_MSG_last_auto_reload rxMsg, CPS_BUAP_MSG **txMsg)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_PARMGR::Process_write_LAR(%d)", 0, handle))

    static CPS_BUAP_MSG_write_reply l_txMsg;
    sLastAutoReload l_lastAutoReload;

    try
    {

        m_pRPF->ReadLastAutoReload(l_lastAutoReload);

        rxMsg.m_ManAuto >> l_lastAutoReload.ManAuto;
        rxMsg.m_LRG >> l_lastAutoReload.LRG;
        rxMsg.m_OPSRScentury >> l_lastAutoReload.OPSRS.century;
        rxMsg.m_OPSRSyear >> l_lastAutoReload.OPSRS.year;
        rxMsg.m_OPSRSmonth >> l_lastAutoReload.OPSRS.month;
        rxMsg.m_OPSRSday >> l_lastAutoReload.OPSRS.day;
        rxMsg.m_OPSRShour >> l_lastAutoReload.OPSRS.hour;
        rxMsg.m_OPSRSminute >> l_lastAutoReload.OPSRS.minute;
        rxMsg.m_ODSScentury >> l_lastAutoReload.ODSS.century;
        rxMsg.m_ODSSyear >> l_lastAutoReload.ODSS.year;
        rxMsg.m_ODSSmonth >> l_lastAutoReload.ODSS.month;
        rxMsg.m_ODSSday >> l_lastAutoReload.ODSS.day;
        rxMsg.m_ODSShour >> l_lastAutoReload.ODSS.hour;
        rxMsg.m_ODSSminute >> l_lastAutoReload.ODSS.minute;
        rxMsg.m_ODSLcentury >> l_lastAutoReload.ODSL.century;
        rxMsg.m_ODSLyear >> l_lastAutoReload.ODSL.year;
        rxMsg.m_ODSLmonth >> l_lastAutoReload.ODSL.month;
        rxMsg.m_ODSLday >> l_lastAutoReload.ODSL.day;
        rxMsg.m_ODSLhour >> l_lastAutoReload.ODSL.hour;
        rxMsg.m_ODSLminute >> l_lastAutoReload.ODSL.minute;
        rxMsg.m_LRGF >> l_lastAutoReload.LRGF;
        rxMsg.m_NCT >> l_lastAutoReload.NCT;
        rxMsg.m_Omission >> l_lastAutoReload.Omission;
        rxMsg.m_SUP >> l_lastAutoReload.SUP;
        rxMsg.m_AssClogNum >> l_lastAutoReload.AssClogNum;

        m_pRPF->WriteLastAutoReload(l_lastAutoReload);

        m_pRPF->FinishUpdate();

        l_txMsg.UserData1() = CPS_BUAP_MSG_ID_write_LAR_ok;
        l_txMsg.UserData2() = 1;
        l_txMsg.m_ResultCode << CPS_BUAP_MSG_write_reply::success;
    } catch (eWrongKey&)
    {
        l_txMsg.UserData1() = CPS_BUAP_MSG_ID_write_LAR_fail;
        l_txMsg.UserData2() = 1;
        l_txMsg.m_ResultCode << CPS_BUAP_MSG_write_reply::function_busy;
    } catch (...)
    {
        l_txMsg.UserData1() = CPS_BUAP_MSG_ID_write_LAR_fail;
        l_txMsg.UserData2() = 1;
        l_txMsg.m_ResultCode << CPS_BUAP_MSG_write_reply::access_error;

        *txMsg = &l_txMsg;
    }

    *txMsg = &l_txMsg;
}

//******************************************************************************
//	Process_write_clog()
//******************************************************************************
void CPS_BUAP_PARMGR::Process_write_clog(HANDLE handle,
        CPS_BUAP_MSG_write_clog rxMsg, CPS_BUAP_MSG **txMsg)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_PARMGR::Process_write_clog(%d)", 0, handle))

    static CPS_BUAP_MSG_write_reply l_txMsg;
    sCommandLogReferences l_clogRefs;

    try
    {

        m_pRPF->ReadCommandLogReferences(l_clogRefs);

        if (rxMsg.m_ClogRefType() < 5)
        {
            rxMsg.m_ClogRef >> l_clogRefs.Current;
        }

        if ((rxMsg.m_ClogRefType() == 2) || (rxMsg.m_ClogRefType() == 4))
        {
            rxMsg.m_ClogRef >> l_clogRefs.Prepared;
        }

        if (rxMsg.m_ClogRefType() >= 3)
        {
            rxMsg.m_ClogRef >> l_clogRefs.Main;
        }

        m_pRPF->WriteCommandLogReferences(l_clogRefs);

        m_pRPF->FinishUpdate();

        l_txMsg.UserData1() = CPS_BUAP_MSG_ID_write_clog_ok;
        l_txMsg.UserData2() = 1;
        l_txMsg.m_ResultCode << 0;
    } catch (eWrongKey&)
    {
        l_txMsg.UserData1() = CPS_BUAP_MSG_ID_write_clog_fail;
        l_txMsg.UserData2() = 1;
        l_txMsg.m_ResultCode << CPS_BUAP_MSG_write_reply::function_busy;
    } catch (...)
    {
        l_txMsg.UserData1() = CPS_BUAP_MSG_ID_write_clog_fail;
        l_txMsg.UserData2() = 1;
        l_txMsg.m_ResultCode << 0;

        *txMsg = &l_txMsg;
    }

    *txMsg = &l_txMsg;
}

//******************************************************************************
//	Process_write_dmr()
//******************************************************************************
void CPS_BUAP_PARMGR::Process_write_dmr(HANDLE handle,
        CPS_BUAP_MSG_write_dmr rxMsg, CPS_BUAP_MSG **txMsg)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_PARMGR::Process_write_dmr(%d)", 0, handle))
    unsigned char l_valueToSet;
    unsigned char l_numMeasures;
    unsigned char l_measure;
    sDelayedMeasures l_delayedMeasures;

    static CPS_BUAP_MSG_write_reply l_txMsg;

    try
    {
        m_pRPF->ReadDelayedMeasures(l_delayedMeasures);

        switch (rxMsg.m_WriteOrder())
        {
        case CPS_BUAP_MSG_write_dmr::eWriteOrder:
            l_valueToSet = 1;
            break;

        case CPS_BUAP_MSG_write_dmr::eResetOrder:
            l_valueToSet = 0;
            break;

        default:
            THROW("Invalid write dmr order")
        }

        l_numMeasures = rxMsg.m_NumMeasures();

        if ((l_numMeasures < 1) || (l_numMeasures > 8))
        {
            THROW("Invalid number of delayed measures");
        }

        for (int i = 0; i < l_numMeasures; i++)
        {
            l_measure = rxMsg.m_Measures[i]();

            if ((l_measure >= 1) && (l_measure <= 8))
            {
                l_delayedMeasures.Measures[l_measure - 1] = l_valueToSet;
            }
            else
            {
                THROW("Invalid delayed measure");
            }
        }

        m_pRPF->WriteDelayedMeasures(l_delayedMeasures);

        m_pRPF->FinishUpdate();

        l_txMsg.UserData1() = CPS_BUAP_MSG_ID_write_dmr_ok;
        l_txMsg.UserData2() = 1;
        l_txMsg.m_ResultCode << 0;
    } catch (eWrongKey&)
    {
        l_txMsg.UserData1() = CPS_BUAP_MSG_ID_write_dmr_fail;
        l_txMsg.UserData2() = 1;
        l_txMsg.m_ResultCode << CPS_BUAP_MSG_write_reply::function_busy;
    } catch (...)
    {
        //dcERROR(("Writing of delayed measures failed"))
        l_txMsg.UserData1() = CPS_BUAP_MSG_ID_write_dmr_fail;
        l_txMsg.UserData2() = 1;
        l_txMsg.m_ResultCode << 0;

        *txMsg = &l_txMsg;
    }

    *txMsg = &l_txMsg;
}

//******************************************************************************
//	Process_read_reladmpar()
//******************************************************************************
void CPS_BUAP_PARMGR::Process_read_reladmpar(CPS_BUAP_MSG **txMsg)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_PARMGR::Process_read_reladmpar()", 0))
    unsigned char l_numMeasures = 0;

    sValidities l_validities;
    sStaticParameters l_staticParameters;
    sLastAutoReload l_lastAutoReload;
    sCommandLogReferences l_clogrefs;
    sDelayedMeasures l_delayedMeasures;

    static CPS_BUAP_MSG_reladmpar l_txMsg;

    static unsigned short EncryptedValue;

    try
    {
        m_pRPF->ReadValidities(l_validities);
        m_pRPF->ReadStaticParameters(l_staticParameters);
        m_pRPF->ReadLastAutoReload(l_lastAutoReload);
        m_pRPF->ReadCommandLogReferences(l_clogrefs);
        m_pRPF->ReadDelayedMeasures(l_delayedMeasures);

        unsigned short GEN = (l_validities.Value & General);
        unsigned short LR = (l_validities.Value & LastAutoReload) ? 1 : 0;
        unsigned short CL = (l_validities.Value & CommandLogReferences) ? 2 : 0;
        unsigned short DMR = (l_validities.Value & DelayedMeasures) ? 4 : 0;

        if (!GEN)
            EncryptedValue = 0;
        else
            EncryptedValue = LR + CL + DMR;

        l_txMsg.m_Validities << EncryptedValue;
        l_txMsg.m_ManAuto << l_staticParameters.ManAuto;
        l_txMsg.m_LRG << l_lastAutoReload.LRG;
        l_txMsg.m_OPSRSyear << l_lastAutoReload.OPSRS.year;
        l_txMsg.m_OPSRSmonth << l_lastAutoReload.OPSRS.month;
        l_txMsg.m_OPSRSday << l_lastAutoReload.OPSRS.day;
        l_txMsg.m_OPSRShour << l_lastAutoReload.OPSRS.hour;
        l_txMsg.m_OPSRSminute << l_lastAutoReload.OPSRS.minute;
        l_txMsg.m_ODSSyear << l_lastAutoReload.ODSS.year;
        l_txMsg.m_ODSSmonth << l_lastAutoReload.ODSS.month;
        l_txMsg.m_ODSSday << l_lastAutoReload.ODSS.day;
        l_txMsg.m_ODSShour << l_lastAutoReload.ODSS.hour;
        l_txMsg.m_ODSSminute << l_lastAutoReload.ODSS.minute;
        l_txMsg.m_ODSLyear << l_lastAutoReload.ODSL.year;
        l_txMsg.m_ODSLmonth << l_lastAutoReload.ODSL.month;
        l_txMsg.m_ODSLday << l_lastAutoReload.ODSL.day;
        l_txMsg.m_ODSLhour << l_lastAutoReload.ODSL.hour;
        l_txMsg.m_ODSLminute << l_lastAutoReload.ODSL.minute;
        l_txMsg.m_LRGF << l_lastAutoReload.LRGF;
        l_txMsg.m_NCT << l_lastAutoReload.NCT;
        l_txMsg.m_Omission << l_lastAutoReload.Omission;
        l_txMsg.m_SUP << l_lastAutoReload.SUP;
        l_txMsg.m_AssClogNum << l_lastAutoReload.AssClogNum;
        l_txMsg.m_Current << l_clogrefs.Current;
        l_txMsg.m_Prepared << l_clogrefs.Prepared;
        l_txMsg.m_Main << l_clogrefs.Main;

        for (int i = 0; i < MAX_MEASURES; i++)
        {
            if (l_delayedMeasures.Measures[i])
            {
                l_txMsg.m_Measures[i] << (unsigned char) i + 1;
                l_numMeasures++;
            }
        }
        l_txMsg.m_NumMeasures << l_numMeasures;

        l_txMsg.UserData1() = CPS_BUAP_MSG_ID_read_reladmpar_ok;
        l_txMsg.UserData2() = 1;
        l_txMsg.m_ResultCode << CPS_BUAP_MSG_reladmpar::eSuccess;
    } catch (...)
    {
        //dcERROR(("Reading of RELADMPAR failed"))
        l_txMsg.UserData1() = CPS_BUAP_MSG_ID_read_reladmpar_fail;
        l_txMsg.UserData2() = 1;
        l_txMsg.m_ResultCode << CPS_BUAP_MSG_reladmpar::eNotAccessable;

        *txMsg = &l_txMsg;
    }

    *txMsg = &l_txMsg;
}

//******************************************************************************
//	Run()
//******************************************************************************
void CPS_BUAP_PARMGR::Run()
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_PARMGR::Run()", 0));

    // Use condition variable to notify other thread that server is started
    {
        boost::lock_guard<boost::mutex> lock(m_mutex);
        m_running = true;
    }
    m_condition.notify_one();

    try
    {
        // Start ACS_JTP Service
        if (!this->initiateJTPService())
        {
            m_running = false;
            TRACE((LOG_LEVEL_INFO, "CPS_BUAP_PARMGR server stopped", 0));
            return;
        }

        TRACE((LOG_LEVEL_INFO, "CPS_BUAP_PARMGR server started", 0));

        // Enter the select loop to wait for incoming event
        while (!Poll())
            ;
    }
    catch (...)
    {
        TRACE((LOG_LEVEL_ERROR, "BUAP_PARMGR runs with Exception!!!", 0));

        {
            // Lock the session to protect
            boost::recursive_mutex::scoped_lock scoped_lock(mutex);
            if (pService && (!reportErrorFlag))
            {
                TRACE((LOG_LEVEL_FATAL, "PARMGR -Report error to AMF.", 0));
                reportErrorFlag = true;
                pService->componentReportError(ACS_APGCC_COMPONENT_RESTART);
            }
        }
    }

    m_running = false;
    TRACE((LOG_LEVEL_INFO, "CPS_BUAP_PARMGR server stopped gracefully", 0));
}

//******************************************************************************
//	OnDisconnect()
//******************************************************************************
void CPS_BUAP_PARMGR::OnDisconnect(HANDLE handle)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_PARMGR::OnDisconnect(handle=%x)", 0, handle))

}

//******************************************************************************
//	SetupDynamicParameters()
//******************************************************************************
void CPS_BUAP_PARMGR::SetupDynamicParameters()
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_PARMGR::SetupDynamicParameters()", 0))

    sStaticParameters l_staticParameters;
    sDynamicParameters l_dynamicParameters;

    try
    {
        m_pRPF->ReadStaticParameters(l_staticParameters);
        m_pRPF->ReadDynamicParameters(l_dynamicParameters);

        DateTime INCLA(l_staticParameters.INCLAday,
                l_staticParameters.INCLAmonth, l_staticParameters.INCLAyear
                        + (l_staticParameters.INCLAcentury * 100));

        l_dynamicParameters.State = 1;
        l_dynamicParameters.CTA = 0;
        l_dynamicParameters.NumFFR = 0;
        l_dynamicParameters.NumSFR = 99;
        l_dynamicParameters.NOld = HighestFile(0, 99, INCLA);
        l_dynamicParameters.KOld = HighestFile(100,
                l_staticParameters.INCL2 ? l_staticParameters.INCL2 : 100,
                INCLA);
        l_dynamicParameters.NCT = 0;
        l_dynamicParameters.LPF = 0;
        l_dynamicParameters.Omission = 0;

        m_pRPF->WriteDynamicParameters(l_dynamicParameters);
    }
    catch (FMS_CPF_Exception& EE)
    {
        TRACE((LOG_LEVEL_ERROR, "FMS_CPF_Exception", 0))
        TRACE((LOG_LEVEL_ERROR, "   errorType: %d", 0, EE.errorCode()))
        TRACE((LOG_LEVEL_ERROR, "   errorText: %s", 0, EE.errorText()))
        TRACE((LOG_LEVEL_ERROR, "   errorInfo: %s", 0, EE.detailInfo().c_str()))
    }
    catch (...)
    {
        TRACE((LOG_LEVEL_ERROR, "Unknown Exception", 0))
    }
}

//******************************************************************************
//	HighestFile()
//******************************************************************************
int CPS_BUAP_PARMGR::HighestFile(int low, int high, DateTime &INCLA)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_PARMGR::HighestFile(%d, %d)", 0, low, high))

    int i;
    int fd;
    int century;
    int highest;
    char filename[16];
    outputTimesSector l_outputTimes;

    highest = 0;

    for (i = low; i <= high; i++)
    {
        sprintf(filename, "RELFSW%d-R0", i);
        FMS_CPF_File file(filename);

        if (!file.exists())
        {
            break;
        }
        else
        {
            file.reserve(FMS_CPF_Types::XR_);
            fd = _open(file.getPhysicalPath().c_str(), O_RDONLY);
            if (fd == -1)
            {
                THROW(("Its gone wrong"))
            }

            _lseek(fd, sizeof(controlInfoSector), SEEK_CUR);
            _read(fd, &l_outputTimes, sizeof(outputTimesSector));

            _close(fd);
            file.unreserve();

            if (l_outputTimes.dump.year[0] < 90)
            {
                century = 20;
            }
            else
            {
                century = 19;
            }

            TRACE(("%s @ %04d-%02d-%02d",
                            0,
                            filename,
                            int(l_outputTimes.dump.year[0]) + century*100,
                            int(l_outputTimes.dump.month[0]),
                            int(l_outputTimes.dump.day[0])))

            DateTime date(int(l_outputTimes.dump.day[0]), int(
                    l_outputTimes.dump.month[0]), int(
                    l_outputTimes.dump.year[0]) + century * 100);

            if (date < INCLA)
            {
                break;
            }

            highest = i;
        }
    }

    return highest;
}

//******************************************************************************
//	Process_thread_control()
//******************************************************************************
DWORD CPS_BUAP_PARMGR::Process_thread_control(DWORD objectHandle)

{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_PARMGR::Process_thread_control", 0))

    DWORD result = NO_ERROR;

    if (objectHandle == (DWORD) hNTObjectArray[ABORT_EVENT])
    {
        TRACE((LOG_LEVEL_INFO, "CPS_BUAP_PARMGR::ABORT_EVENT", 0));

        //**********************************************************************
        // So far there is only one thread control event.
        //**********************************************************************
        result = SERVICE_ABORTED;

        // Reset the stop event.
        m_stopEvent.resetEvent();

        // Clear the JTP Handlers list.
        if (!m_ConversationList.empty())
        {
            // delete all conversations from the list
            TRACE(("%s - Delete all conversation from the list", 0, m_deriveName.c_str()));
            for (CPS_BUAP_Conversation_LIST_ITERATOR iConversation =
                    m_ConversationList.begin(); iConversation
                    != m_ConversationList.end(); ++iConversation)
            {
                //delete *iConversation;
		(*iConversation).reset();
            }
        }

        m_ConversationList.clear();

        // Server closed, clear pointer to JTP service
        if (m_pService)
        {
            delete m_pService;
            m_pService = 0;
        }

        // Delete pointer to RELADMPAR file object
        if (m_pRPF)
        {
            delete m_pRPF;
            m_pRPF = 0;
        }
    }
    else
    {
        result = INVALID_HANDLE;
    }

    return result;
}

//******************************************************************************
//  initiateJTPService()
//******************************************************************************
bool CPS_BUAP_PARMGR::initiateJTPService()
{
    CPS_BUAP_Conversation_Ptr l_pConversation;
    int noOfServerHandles = 15;
    HANDLE *serverHandles = new HANDLE[15];
    bool jtpResult = false;

#if 0    //xchihoa: Fix the problem with service reboot but data disk not ready (mounted)
    //***************************************************************************
    // Create a RELADMPAR file instance.
    //***************************************************************************
    try
    {
        m_pRPF = new CPS_BUAP_RPF(m_fileName.c_str());
    }
    catch (CPS_BUAP_Exception&)
    {
        EVENT((CPS_BUAP_Events::reladmpar_fail, 0, m_fileName.c_str()))

        delete[] serverHandles;
        serverHandles = 0;
        throw;
    }
#endif

    //***************************************************************************
    // Start the BUPARMGR server.
    //***************************************************************************
    newTRACE((LOG_LEVEL_INFO, "new ACS_JTP_Service(BUPARMGR)", 0))

    char acServiceName[] = "BUPARMGR";
    uint64_t u;
    ssize_t num;

    bool isTrace = false;

    do
    {
        // Create a JTP to BUPARMGR server.
        if (m_pService)
        {
            delete m_pService;
            m_pService = 0;
        }
        m_pService = new ACS_JTP_Service(acServiceName);

        TRACE(("CPS_BUAP_PARMGR calling jidrepreq", 0));
        // try to connect to JTP Name Service (DSD at the end) until it not fails.
        jtpResult = m_pService->jidrepreq();

        if (jtpResult == false)
        {
            if (!isTrace)
            {
                TRACE((LOG_LEVEL_ERROR, "ACS_JTP_Service(BUPARMGR)->jidrepreq: failed to register to DSD", 0));
                EVENT((CPS_BUAP_Events::jidrepreq_fail, 0, "BUPARMGR"));
                isTrace = true;
            }

            // Check if the stop event is set.
            num = ::read(m_stopEvent.getFd(), &u, sizeof(uint64_t));
            if (num == -1)
            {
                if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
                {
                    // Not receive stop event, go to sleep.
                    ::sleep((unsigned int) 1);
                    continue;
                    //THROW("Register to DSD failed.");
                }

                TRACE((LOG_LEVEL_ERROR, "CPS_BUAP_PARMGR - Failed to read stop event", 0));
                if (serverHandles)
                {
                    delete [] serverHandles;
                    serverHandles = 0;
                }
                //return false;
                THROW("Reading stop event failed");
            }
            else
            {
                TRACE((LOG_LEVEL_INFO, "CPS_BUAP_PAMGR - Receives stop event %d ", 0, m_stopEvent.getFd()));

                m_stopEvent.resetEvent();

                if (serverHandles)
                {
                    delete [] serverHandles;
                    serverHandles = 0;
                }
                return false;
            }
        }
        else
        {
            TRACE((LOG_LEVEL_INFO, "ACS_JTP_Service(BUPARMGR) register successfully", 0));
        }
    }
    while (jtpResult == false);

    //****************************************************************************
    // Get JTP handles for service and link them into conversation list.
    //****************************************************************************
    m_pService->getHandles(noOfServerHandles, serverHandles);

    for (int i = 0; i < noOfServerHandles; i++)
    {
        l_pConversation.reset(new CPS_BUAP_Conversation(serverHandles[i], m_pService));

        AddConversation(l_pConversation);
    }

    if (serverHandles)
    {
        delete[] serverHandles;
        serverHandles = 0;
    }

    TRACE((LOG_LEVEL_INFO, "initiateJTPService returned true", 0));
    return true;
}

//***************************************************************************
//      stop()
//***************************************************************************

void CPS_BUAP_PARMGR::stop()
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_PARMGR::stop()", 0));

    //Set event stop
    m_stopEvent.setEvent();

}

//***************************************************************************
//    isRunning()
//***************************************************************************

bool CPS_BUAP_PARMGR::isRunning() const
{
    //newTRACE(("CPS_BUAP_PARMGR::isRunning() - %d", 0, (int)m_running));
    return m_running;
}

//***************************************************************************
//    waitUntilRunning()
//***************************************************************************
void CPS_BUAP_PARMGR::waitUntilRunning()
{
    newTRACE(("CPS_BUAP_PARMGR::waitUntilRunning()", 0));

    boost::unique_lock<boost::mutex> lock(m_mutex);

    while (m_running == false)
    {
        TRACE((LOG_LEVEL_INFO, "Wait until CPS_BUAP_PARMGR server Thread signal", 0));
        m_condition.wait(lock);
        TRACE((LOG_LEVEL_INFO, "CPS_BUAP_PARMGR server Thread signaled", 0));
    }
}


