//******************************************************************************
//
// NAME
//      %File_name:CPS_BUAP_BUPPRINT.cpp%
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
//      This class contains the methods required for the implementation
//      of the bupprint command.  The first method to call is initialise
//      which creates a client of the parmgr process.  The next method to
//      call is ParseCommandLine which checks and interprets the command
//      -line options.  Finally, if no exception occured, the method
//      ExecuteCommandLine should be called to carry out the command.

// DOCUMENT NO
//	190 89-CAA 109 0082

// AUTHOR 
// 	1997/08/06 by U/B/SD David Wade

// SEE ALSO 
// 	<Related classes, (abstract) baseclasses, friend classes, ...>
//
// Revision history
// ----------------
// 1997/08/06 Create by David Wadw
// 1999/07/02 LAN add new include file CPS_BUAP_getopt.H
// 
//******************************************************************************

//#include <windows.h>

#include <stdio.h>

#include "CPS_BUAP_Trace.h"
#include "CPS_BUAP_Events.h"
#include "CPS_BUAP_EventReporter.h"
#include "CPS_BUAP_Client.h"
#include "CPS_BUAP_MSG_IDs.h"
#include "CPS_BUAP_MSG_static_parameters.h"
#include "CPS_BUAP_MSG_dynamic_parameters.h"
#include "CPS_BUAP_MSG_command_log_references.h"
#include "CPS_BUAP_MSG_last_auto_reload.h"
#include "CPS_BUAP_MSG_delayed_measures.h"
#include "CPS_BUAP_BUPPRINT.h"
#include "CPS_BUAP_RPF.h"
#include "CPS_BUAP_getopt.h"

//******************************************************************************
//	CPS_BUAP_BUPPRINT()
//******************************************************************************
CPS_BUAP_BUPPRINT::CPS_BUAP_BUPPRINT()
{
    newTRACE(("CPS_BUAP_BUPPRINT::CPS_BUAP_BUPPRINT()", 0))

    m_pClient = (CPS_BUAP_Client *)0;

    m_Static = 0;
    m_Dynamic = 0;
    m_ClogRefs = 0;
    m_LAR = 0;
    m_DMeasures = 0;
    m_OldFormat = 0;
}

//******************************************************************************
//	~CPS_BUAP_BUPPRINT()
//******************************************************************************
CPS_BUAP_BUPPRINT::~CPS_BUAP_BUPPRINT()
{
newTRACE(("CPS_BUAP_BUPPRINT::~CPS_BUAP_BUPPRINT()", 0))

if (m_pClient)
{
    delete m_pClient;
}
}

//******************************************************************************
//	DoStaticPars()
//******************************************************************************
void CPS_BUAP_BUPPRINT::DoStaticPars()
{
    CPS_BUAP_MSG txMsg(CPS_BUAP_MSG_ID_read_static);
    CPS_BUAP_MSG_static_parameters rxMsg;

    try
    {
        m_pClient->TXRX(txMsg, rxMsg);
    } catch (CPS_BUAP_Exception&) //:ag1
    {
        throw eUnknown;
    }

    switch (rxMsg.UserData1())
    {
    case CPS_BUAP_MSG_ID_read_static_ok:
        break;

    case CPS_BUAP_MSG_ID_read_static_fail:
        throw eCPFSerror;

    default:
        throw eUnknown;
    }

    printf("\n\nSTATIC PARAMETERS");
    if (rxMsg.m_Validity())
    {
        printf(" are valid\n");
        printf("\n");
        printf("   CLH     NTAZ    NTCZ    LOAZ    INCL1   INCL2   SUP\n");
        printf(
                "   %-3d     %-3d     %-3d     %-3d     %-3d     %-3d     %-3d\n",
                rxMsg.m_ManAuto(), rxMsg.m_NTAZ(), rxMsg.m_NTCZ(),
                rxMsg.m_LOAZ(), rxMsg.m_INCL1(), rxMsg.m_INCL2(), rxMsg.m_SUP()
                        * 10);
        printf("\n");
        printf("   INCLA\n");
        printf("   %-s\n", FormatDate(false, rxMsg.m_INCLAcentury(),
                rxMsg.m_INCLAyear(), rxMsg.m_INCLAmonth(), rxMsg.m_INCLAday()));
    }
    else
    {
        printf(" are not valid\n");
    }
}

//******************************************************************************
//	DoDynamicPars()
//******************************************************************************
void CPS_BUAP_BUPPRINT::DoDynamicPars()
{
    CPS_BUAP_MSG txMsg(CPS_BUAP_MSG_ID_read_dynamic);
    CPS_BUAP_MSG_dynamic_parameters rxMsg;

    try
    {
        m_pClient->TXRX(txMsg, rxMsg);
    } catch (CPS_BUAP_Exception&) //:ag1
    {
        throw eUnknown;
    }

    switch (rxMsg.UserData1())
    {
    case CPS_BUAP_MSG_ID_read_dynamic_ok:
        break;

    case CPS_BUAP_MSG_ID_read_dynamic_fail:
        throw eCPFSerror;

    default:
        throw eUnknown;
    }

    printf("\n\nDYNAMIC PARAMETERS");
    if (rxMsg.m_Validity())
    {
        printf(" are valid\n");
        printf("\n");
        printf("   STATE   CTA     CFFR    CSFR    FOLD    SOLD    NCT\n");
        printf(
                "   %-3d     %-3d     %-3d     %-3d     %-3d     %-3d     %-3d\n",
                rxMsg.m_State(), rxMsg.m_CTA(), rxMsg.m_NumFFR(),
                //:ag1 0 || 100-127 valid, 99 used in-place of 0, a (not-so) 'smart' solution
                rxMsg.m_NumSFR() == 99 ? 0 : rxMsg.m_NumSFR(), rxMsg.m_NOld(),
                rxMsg.m_KOld(), rxMsg.m_NCT());

        printf("\n");
        printf("   LPF     OMISSION        EXP\n");
        printf("   %-3d     %-3d             %-s\n", rxMsg.m_LPF(),
                rxMsg.m_Omission(), FormatDate(true, rxMsg.m_EXPcentury(),
                        rxMsg.m_EXPyear(), rxMsg.m_EXPmonth(),
                        rxMsg.m_EXPday(), rxMsg.m_EXPhour(),
                        rxMsg.m_EXPminute()));
    }
    else
    {
        printf(" are not valid\n");
    }
}

//******************************************************************************
//	DoClogRefs()
//******************************************************************************
void CPS_BUAP_BUPPRINT::DoClogRefs()
{
    CPS_BUAP_MSG txMsg(CPS_BUAP_MSG_ID_read_clogrefs);
    CPS_BUAP_MSG_command_log_references rxMsg;

    try
    {
        m_pClient->TXRX(txMsg, rxMsg);
    } catch (CPS_BUAP_Exception&) //:ag1
    {
        throw eUnknown;
    }

    switch (rxMsg.UserData1())
    {
    case CPS_BUAP_MSG_ID_read_clogrefs_ok:
        break;

    case CPS_BUAP_MSG_ID_read_clogrefs_fail:
        throw eCPFSerror;

    default:
        throw eUnknown;
    }

    printf("\n\nCOMMAND LOG REFERENCES");
    if (rxMsg.m_Validity())
    {
        printf(" are valid\n");
        printf("\n");
        printf("   CURRENT         MAIN            PREPARED\n");
        printf("   %-12lu    %-12lu    %-12lu\n", rxMsg.m_Current(),
                rxMsg.m_Main(), rxMsg.m_Prepared());
    }
    else
    {
        printf(" are not valid\n");
    }
}

//******************************************************************************
//	DoLAR()
//******************************************************************************
void CPS_BUAP_BUPPRINT::DoLAR()
{
    CPS_BUAP_MSG txMsg(CPS_BUAP_MSG_ID_read_LAR);
    CPS_BUAP_MSG_last_auto_reload rxMsg;

    //TODO: check this: progaram phone.

    try
    {
        m_pClient->TXRX(txMsg, rxMsg);
    } catch (CPS_BUAP_Exception&) //:ag1
    {
        throw eUnknown;
    }

    switch (rxMsg.UserData1())
    {
    case CPS_BUAP_MSG_ID_read_LAR_ok:
        break;

    case CPS_BUAP_MSG_ID_read_LAR_fail:
        throw eCPFSerror;

    default:
        throw eUnknown;
    }

    printf("\n\nLAST AUTO RELOAD");
    if (rxMsg.m_Validity())
    {
        printf(" are valid\n");
        printf("\n");
        printf("   CLH     GFILE   LPGF    NCT     SUP     OMISSION  ALOG\n");
        printf(
                "   %-3d     %-3d     %-3d     %-3d     %-3d     %-3d       %-12lu\n",
                rxMsg.m_ManAuto(), rxMsg.m_LRG(), rxMsg.m_LRGF(),
                rxMsg.m_NCT(), rxMsg.m_SUP() * 10, rxMsg.m_Omission(),
                rxMsg.m_AssClogNum());
        printf("\n");

        printf("   OPSRS               ODSS                ODSL\n");
        // TR HP58825: FormatDate returns static string so that have to use 3 printf to print the output time respectively
        printf("   %-16s", FormatDate(true, rxMsg.m_OPSRScentury(), rxMsg.m_OPSRSyear(),
                rxMsg.m_OPSRSmonth(), rxMsg.m_OPSRSday(), rxMsg.m_OPSRShour(), rxMsg.m_OPSRSminute()));
        printf("    %-16s", FormatDate(true, rxMsg.m_ODSScentury(), rxMsg.m_ODSSyear(),
                rxMsg.m_ODSSmonth(), rxMsg.m_ODSSday(), rxMsg.m_ODSShour(), rxMsg.m_ODSSminute()));
        printf("    %-16s\n", FormatDate(true, rxMsg.m_ODSLcentury(), rxMsg.m_ODSLyear(),
                rxMsg.m_ODSLmonth(), rxMsg.m_ODSLday(), rxMsg.m_ODSLhour(), rxMsg.m_ODSLminute()));
    }
    else
    {
        printf(" are not valid\n");
    }
}

//******************************************************************************
//	FormatDate()
//******************************************************************************
char *CPS_BUAP_BUPPRINT::FormatDate(const unsigned withTime,
        const unsigned century, const unsigned year, const unsigned month,
        const unsigned day, const unsigned hour, const unsigned minute)
{
    static char l_date[64];
    int l_numChars;

    l_numChars = sprintf(l_date, "%02d%02d-%02d-%02d", century, year, month,
            day);

    if (withTime)
    {
        sprintf(&l_date[l_numChars], " %02d:%02d", hour, minute);
    }

    return l_date;
}

//******************************************************************************
//	DoDMeasures()
//******************************************************************************
void CPS_BUAP_BUPPRINT::DoDMeasures()
{
    CPS_BUAP_MSG txMsg(CPS_BUAP_MSG_ID_read_dmeasures);
    CPS_BUAP_MSG_delayed_measures rxMsg;

    try
    {
        m_pClient->TXRX(txMsg, rxMsg);
    } catch (CPS_BUAP_Exception&) //:ag1
    {
        throw eUnknown;
    }

    switch (rxMsg.UserData1())
    {
    case CPS_BUAP_MSG_ID_read_dmeasures_ok:
        break;

    case CPS_BUAP_MSG_ID_read_dmeasures_fail:
        throw eCPFSerror;

    default:
        throw eUnknown;
    }

    printf("\n\nDELAYED MEASURES");
    if (rxMsg.m_Validity())
    {
        printf(" are valid\n");
        printf("\n");
        printf("   DM1   DM2   DM3   DM4   DM5   DM6   DM7   DM8\n");
        //printf("");
        for (int i = 0; i < MAX_MEASURES; i++)
        {
            printf("   %-3d", rxMsg.m_Measures[i]());
        }
        printf("\n");
    }
    else
    {
        printf(" are not valid\n");
    }
}

//******************************************************************************
//	DoOldFormat()
//******************************************************************************
void CPS_BUAP_BUPPRINT::DoOldFormat()
{
    // This routine a result of PC-CPS. I think it is wrong to have this.  The
    // format is still not the same as the old command.  Raw values are displayed
    // even if they are not valid!!  Its a mess, but that is what was requested.
    // I disclaim all responsibility.

    DoStaticPars();

    CPS_BUAP_MSG txMsg(CPS_BUAP_MSG_ID_read_LAR);
    CPS_BUAP_MSG_last_auto_reload rxMsg;

    try
    {
        m_pClient->TXRX(txMsg, rxMsg);
    } catch (CPS_BUAP_Exception&) //:ag1
    {
        throw eUnknown;
    }

    switch (rxMsg.UserData1())
    {
    case CPS_BUAP_MSG_ID_read_LAR_ok:
        break;

    case CPS_BUAP_MSG_ID_read_LAR_fail:
        throw eCPFSerror;

    default:
        throw eUnknown;
    }

    printf("\n\nLAST RELOADED FILE");
    printf("\n");
    printf("   NAME          CREATED AT\n");
    printf("   RELFSW%-2d      %-16s\n", rxMsg.m_LRG(), FormatDate(true,
            rxMsg.m_OPSRScentury(), rxMsg.m_OPSRSyear(), rxMsg.m_OPSRSmonth(),
            rxMsg.m_OPSRSday(), rxMsg.m_OPSRShour(), rxMsg.m_OPSRSminute()));
    printf("\n");
}

//******************************************************************************
//	Initialise()
//******************************************************************************
void CPS_BUAP_BUPPRINT::Initialise()
{
newTRACE(("CPS_BUAP_BUPPRINT::Initialise()", 0))

try
{
    m_pClient = new CPS_BUAP_Client("BUPARMGR", 5, 1);
}
catch ( CPS_BUAP_Exception& ) //:ag1

{
    EVENT((CPS_BUAP_Events::no_server, 1))
    throw eNoServer;
}
}

//******************************************************************************
//	ParseCommandLine()
//******************************************************************************
void CPS_BUAP_BUPPRINT::ParseCommandLine(int argc, char **argv)
{
    newTRACE(("CPS_BUAP_BUPPRINT::ParseCommandLine()", 0))

    int l_opt;

    try
    {
        while ((l_opt = buap_getopt(argc, argv, "asdclm")) != EOF)
        {
            switch (l_opt)
            {
            case 'a':
                m_Static = 1;
                m_Dynamic = 1;
                m_ClogRefs = 1;
                m_LAR = 1;
                m_DMeasures = 1;
                break;

            case 's':
                m_Static = 1;
                break;

            case 'd':
                m_Dynamic = 1;
                break;

            case 'c':
                m_ClogRefs = 1;
                break;

            case 'l':
                m_LAR = 1;
                break;

            case 'm':
                m_DMeasures = 1;
                break;

            default:
                throw eSyntaxError;
            }
        }

        // if no option set then print static parameters
        if ((!m_Static) && (!m_Dynamic) && (!m_ClogRefs) && (!m_LAR)
                && (!m_DMeasures))
        {
            m_OldFormat = 1;
        }

    } catch (eError EE)
    {
        switch (EE)
        {
        case eSyntaxError:
            throw;
        default:
            throw;
        }
    }
}

//******************************************************************************
//	ExecuteCommand()
//******************************************************************************
void CPS_BUAP_BUPPRINT::ExecuteCommand()
{
    newTRACE(("CPS_BUAP_BUPPRINT::ExecuteCommand()", 0))

    CPS_BUAP_MSG l_rxMsg;

    if (m_Static)
        DoStaticPars();
    if (m_Dynamic)
        DoDynamicPars();
    if (m_ClogRefs)
        DoClogRefs();
    if (m_LAR)
        DoLAR();
    if (m_DMeasures)
        DoDMeasures();
    if (m_OldFormat)
        DoOldFormat();

    printf("\n");
}
