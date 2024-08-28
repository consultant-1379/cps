//******************************************************************************
//
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 2012.
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
//  CPS_BUAP_RPF.cpp
//
//  DESCRIPTION 
//  This class implements the RELADMPAR file.  It creates it
//  filling default field values if it doesn't exist. It
//  implements read and write methods for each of the records
//  within the file.  Note that access is on a record basis
//  which means that if a single field is to be updated then
//  then whole record must be read first, the field updated,
//  and then the whole record written.
//
//  Note that on instantiation the contents of the RELADMPAR file
//  are read and held in memory.  Subsequent reads will use the
//  copy in memory rather then in the file. Methods updating
//  records are also implemented updating the memory copy.  Only
//  when the FinishUpdate method is invoked are the values actually
//  written back to the file.
//
//  The file is organised in 6 records. Each record is 32 bytes
//  in length.  However, read and writes to the file are made
//  to the whole file, i.e. 6*32bytes is the size of all file
//  operations.
//
//  DOCUMENT NO
//  190 89-CAA 109 1412
//
//  AUTHOR 
//  1999/06/30 by UAB/B/SF Lillemor Petterson
//                      (- change open/read/.. to _open/_read..)
//
//******************************************************************************
// === Revision history ===
// 990630 LAN   PA1     Created.
// 991103 BIR   PA2     Removal of method StartUpdate.
//                      m_fd (file descriptor) changed to m_fh (file handle).
// 061003 LAN           Add [] after delete in destructor.
// 101010 xchihoa       Ported to Linux for APG43L.
// 120314 xngudan       implement Object Manager.
//******************************************************************************
#include "CPS_BUAP_Linux.h"
#include "CPS_BUAP_RPF.h"
#include "CPS_BUAP_Trace.h"
#include "CPS_BUAP_Events.h"
#include "CPS_BUAP_Exception.h"
#include "ClassicOI.h"
#include "BUParamsOM.h"
#include "BUParamsCommon.h"
#include "CPS_BUAP_ParseIni.h"

#include <stdio.h>
#include <sys/stat.h> // _S_IREAD, _S_IWRITE
#include <fcntl.h>   // _O_CREAT, _O_RDWR
#include <string.h>
#include <errno.h>


using namespace std;
// Global pointer used to notify updating the runtime attribute of Classic class
extern ClassicOI* pClassicOI;

//******************************************************************************
// CPS_BUAP_RPF()
//******************************************************************************
CPS_BUAP_RPF::CPS_BUAP_RPF(const char *filename)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_RPF::CPS_BUAP_RPF(file=%s)", 0, filename));

    m_Filename = new char[strlen(filename)+1];
    strcpy(m_Filename, filename);

    //
    // Setup default values
    //
    memset((void *) &m_Reladmpar, 0, sizeof(sReladmpar));
    // StaticParams
    m_Reladmpar.StaticPar.ManAuto     = c_ManAuto;
    m_Reladmpar.StaticPar.NTAZ        = c_NTAZ;
    m_Reladmpar.StaticPar.NTCZ        = c_NTCZ;
    m_Reladmpar.StaticPar.LOAZ        = c_LOAZ;
    m_Reladmpar.StaticPar.INCLAcentury = c_INCLAcentury;
    m_Reladmpar.StaticPar.INCLAyear   = c_INCLAyear;
    m_Reladmpar.StaticPar.INCLAmonth   = c_INCLAmonth;
    m_Reladmpar.StaticPar.INCLAday    = c_INCLAday;
    m_Reladmpar.StaticPar.INCL1       = c_INCL1;
    m_Reladmpar.StaticPar.INCL2       = c_INCL2;
    m_Reladmpar.StaticPar.SUP             = c_SUP;

    // DynamicParams
    m_Reladmpar.DynamicPar.State          = 1;
    m_Reladmpar.DynamicPar.EXP.century = 19;
    m_Reladmpar.DynamicPar.EXP.year   = 90;
    m_Reladmpar.DynamicPar.EXP.month   = 01;
    m_Reladmpar.DynamicPar.EXP.day    = 01;
    m_Reladmpar.DynamicPar.EXP.hour   = 00;
    m_Reladmpar.DynamicPar.EXP.minute  = 00;

    m_Reladmpar.Validities.Value         |= General;

    m_Reladmpar.DynamicPar.CTA = 0;
    m_Reladmpar.DynamicPar.NumFFR = 0;
    m_Reladmpar.DynamicPar.NumSFR = 99;
    m_Reladmpar.DynamicPar.NOld = 0;
    m_Reladmpar.DynamicPar.KOld = 0;
    m_Reladmpar.DynamicPar.NCT = 0;
    m_Reladmpar.DynamicPar.LPF = 0;
    m_Reladmpar.DynamicPar.Omission = 0;

    // CommandLogReferences
    m_Reladmpar.CLogRefs.Current = 0;
    m_Reladmpar.CLogRefs.Main = 0;
    m_Reladmpar.CLogRefs.Prepared = 0;

    // LastAutoReload
    m_Reladmpar.LastAutoReload.ManAuto = 0;
    m_Reladmpar.LastAutoReload.LRG = 0;
    m_Reladmpar.LastAutoReload.LRGF = 0;
    m_Reladmpar.LastAutoReload.NCT = 0;
    m_Reladmpar.LastAutoReload.SUP = 0;
    m_Reladmpar.LastAutoReload.Omission = 0;
    m_Reladmpar.LastAutoReload.AssClogNum = 0;
    m_Reladmpar.LastAutoReload.OPSRS.century = 00;
    m_Reladmpar.LastAutoReload.OPSRS.year = 00;
    m_Reladmpar.LastAutoReload.OPSRS.month = 00;
    m_Reladmpar.LastAutoReload.OPSRS.day = 00;
    m_Reladmpar.LastAutoReload.OPSRS.hour = 00;
    m_Reladmpar.LastAutoReload.OPSRS.minute = 00;
    m_Reladmpar.LastAutoReload.ODSS.century = 00;
    m_Reladmpar.LastAutoReload.ODSS.year = 00;
    m_Reladmpar.LastAutoReload.ODSS.month = 00;
    m_Reladmpar.LastAutoReload.ODSS.day = 00;
    m_Reladmpar.LastAutoReload.ODSS.hour = 00;
    m_Reladmpar.LastAutoReload.ODSS.minute = 00;
    m_Reladmpar.LastAutoReload.ODSL.century = 00;
    m_Reladmpar.LastAutoReload.ODSL.year = 00;
    m_Reladmpar.LastAutoReload.ODSL.month = 00;
    m_Reladmpar.LastAutoReload.ODSL.day = 00;
    m_Reladmpar.LastAutoReload.ODSL.hour = 00;
    m_Reladmpar.LastAutoReload.ODSL.minute = 00;

    // DelayedMeasures
    m_Reladmpar.DelayedMeasures.Measures[0] = 0;
    m_Reladmpar.DelayedMeasures.Measures[1] = 0;
    m_Reladmpar.DelayedMeasures.Measures[2] = 0;
    m_Reladmpar.DelayedMeasures.Measures[3] = 0;
    m_Reladmpar.DelayedMeasures.Measures[4] = 0;
    m_Reladmpar.DelayedMeasures.Measures[5] = 0;
    m_Reladmpar.DelayedMeasures.Measures[6] = 0;
    m_Reladmpar.DelayedMeasures.Measures[7] = 0;

    // Added _O_BINARY, probably doesn't matter here
    m_fh = _open(m_Filename, O_RDWR);

    if (m_fh != -1)
    {
#if 0
        bytes_read =  _read(m_fh, (char *)&m_Reladmpar, sizeof(sReladmpar));
        if (bytes_read == -1)
        {
           TRACE(("Error reading file: %s", 0, m_Filename));
            EVENT((CPS_BUAP_Events::reladmpar_read_fail, 0, strerror(errno), m_Filename));
            THROW("Error reading file reladmpar");
        }
#endif

        if (!this->readReladmpar(m_Filename, m_Reladmpar))
        {
            TRACE((LOG_LEVEL_ERROR, "Error reading file: %s", 0, m_Filename));
            EVENT((CPS_BUAP_Events::reladmpar_read_fail, 0, strerror(errno), m_Filename));
            THROW("Error reading file reladmpar.ini");
        }

    }
    else
    {
       TRACE((LOG_LEVEL_WARN, "File: %s not exist! Try to create new file", 0, m_Filename));
        // Added _O_BINARY, needed when we create the file
       mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

        m_fh =  _open(m_Filename, O_RDWR | O_CREAT, mode); // | _O_CREAT | _O_RDWR, _S_IREAD | _S_IWRITE
        if (m_fh == -1)
        {
            EVENT((CPS_BUAP_Events::reladmpar_create_fail, 0, strerror(errno), m_Filename));
            THROW("Failed to create file reladmpar.ini");
        }
        FinishUpdate();
    }
}

//******************************************************************************
// ~CPS_BUAP_RPF()
//******************************************************************************
CPS_BUAP_RPF::~CPS_BUAP_RPF()
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_RPF::~CPS_BUAP_RPF()", 0));

    close(m_fh);

    if (m_Filename)
    {
        delete [] m_Filename;
    }
}

//******************************************************************************
// ReadValidities()
//******************************************************************************
void CPS_BUAP_RPF::ReadValidities(sValidities &validities)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_RPF::fValidities()", 0));

    validities = m_Reladmpar.Validities;

    TRACE((LOG_LEVEL_INFO, "   GEN     STAT    DYN     CLOG    LAR     DM", 0));
    TRACE((LOG_LEVEL_INFO, "   %-3d     %-3d     %-3d     %-3d     %-3d     %-3d",
        0,
        ((m_Reladmpar.Validities.Value & General)>0),
        ((m_Reladmpar.Validities.Value & StaticPars)>0),
        ((m_Reladmpar.Validities.Value & DynamicPars)>0),
        ((m_Reladmpar.Validities.Value & CommandLogReferences)>0),
        ((m_Reladmpar.Validities.Value & LastAutoReload)>0),
        ((m_Reladmpar.Validities.Value & DelayedMeasures)>0)));
}

//******************************************************************************
// ReadStaticParameters()
//******************************************************************************
void CPS_BUAP_RPF::ReadStaticParameters (sStaticParameters &staticPar)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_RPF::ReadStaticParameters()", 0));

    staticPar  = m_Reladmpar.StaticPar;

#if 0
    TRACE(("   CLH     NTAZ    NTCZ    LOAZ    INCL1   INCL2   SUP", 0));
    TRACE(("   %-3d     %-3d     %-3d     %-3d     %-3d     %-3d     %-3d",
        0,
        m_Reladmpar.StaticPar.ManAuto,
        m_Reladmpar.StaticPar.NTAZ,
        m_Reladmpar.StaticPar.NTCZ,
        m_Reladmpar.StaticPar.LOAZ,
        m_Reladmpar.StaticPar.INCL1,
        m_Reladmpar.StaticPar.INCL2,
        m_Reladmpar.StaticPar.SUP*10));
    TRACE(("", 0));
    TRACE(("   INCLA", 0));
    TRACE(("   %02d%02d-%02d-%02d", 0,
        m_Reladmpar.StaticPar.INCLAcentury,
        m_Reladmpar.StaticPar.INCLAyear,
        m_Reladmpar.StaticPar.INCLAmonth,
        m_Reladmpar.StaticPar.INCLAday));
#endif

    TRACE((LOG_LEVEL_INFO, "****************************************", 0));
    TRACE((LOG_LEVEL_INFO, "commandLogMode = %d", 0, m_Reladmpar.StaticPar.ManAuto));
    TRACE((LOG_LEVEL_INFO, "numberOfTruncationAttempts = %d", 0, m_Reladmpar.StaticPar.NTAZ));
    TRACE((LOG_LEVEL_INFO, "numberOfTruncationCommands = %d", 0, m_Reladmpar.StaticPar.NTCZ));
    TRACE((LOG_LEVEL_INFO, "logOmissionMode = %d", 0, m_Reladmpar.StaticPar.LOAZ));
    TRACE((LOG_LEVEL_INFO, "includeFirstFileRange = %d", 0, m_Reladmpar.StaticPar.INCL1));
    TRACE((LOG_LEVEL_INFO, "includeSecondFileRange = %d", 0, m_Reladmpar.StaticPar.INCL2));
    TRACE((LOG_LEVEL_INFO, "supervisionTime = %d", 0, m_Reladmpar.StaticPar.SUP*10));
    TRACE((LOG_LEVEL_INFO, "oldestAcceptedDate = %02d%02d-%02d-%02d", 0, m_Reladmpar.StaticPar.INCLAcentury,
                                                         m_Reladmpar.StaticPar.INCLAyear,
                                                         m_Reladmpar.StaticPar.INCLAmonth,
                                                         m_Reladmpar.StaticPar.INCLAday));
    TRACE((LOG_LEVEL_INFO, "****************************************", 0));
}

//******************************************************************************
// WriteStaticParameters()
//******************************************************************************
void CPS_BUAP_RPF::WriteStaticParameters(sStaticParameters &staticPar)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_RPF::WriteStaticParameters()", 0));

    m_Reladmpar.StaticPar = staticPar;
    m_Reladmpar.Validities.Value |= StaticPars;

#if 0
    TRACE(("   CLH     NTAZ    NTCZ    LOAZ    INCL1   INCL2   SUP", 0));
    TRACE(("   %-3d     %-3d     %-3d     %-3d     %-3d     %-3d     %-3d", 0,
        m_Reladmpar.StaticPar.ManAuto,
        m_Reladmpar.StaticPar.NTAZ,
        m_Reladmpar.StaticPar.NTCZ,
        m_Reladmpar.StaticPar.LOAZ,
        m_Reladmpar.StaticPar.INCL1,
        m_Reladmpar.StaticPar.INCL2,
        m_Reladmpar.StaticPar.SUP*10));
    TRACE(("", 0));
    TRACE(("   INCLA", 0));
    TRACE(("   %02d%02d-%02d-%02d", 0,
        m_Reladmpar.StaticPar.INCLAcentury,
        m_Reladmpar.StaticPar.INCLAyear,
        m_Reladmpar.StaticPar.INCLAmonth,
        m_Reladmpar.StaticPar.INCLAday));
#endif

    TRACE((LOG_LEVEL_INFO, "****************************************", 0));
    TRACE((LOG_LEVEL_INFO, "commandLogMode = %d", 0, m_Reladmpar.StaticPar.ManAuto));
    TRACE((LOG_LEVEL_INFO, "numberOfTruncationAttempts = %d", 0, m_Reladmpar.StaticPar.NTAZ));
    TRACE((LOG_LEVEL_INFO, "numberOfTruncationCommands = %d", 0, m_Reladmpar.StaticPar.NTCZ));
    TRACE((LOG_LEVEL_INFO, "logOmissionMode = %d", 0, m_Reladmpar.StaticPar.LOAZ));
    TRACE((LOG_LEVEL_INFO, "includeFirstFileRange = %d", 0, m_Reladmpar.StaticPar.INCL1));
    TRACE((LOG_LEVEL_INFO, "includeSecondFileRange = %d", 0, m_Reladmpar.StaticPar.INCL2));
    TRACE((LOG_LEVEL_INFO, "supervisionTime = %d", 0, m_Reladmpar.StaticPar.SUP*10));
    TRACE((LOG_LEVEL_INFO, "oldestAcceptedDate = %02d%02d-%02d-%02d", 0, m_Reladmpar.StaticPar.INCLAcentury,
                                                         m_Reladmpar.StaticPar.INCLAyear,
                                                         m_Reladmpar.StaticPar.INCLAmonth,
                                                         m_Reladmpar.StaticPar.INCLAday));
    TRACE((LOG_LEVEL_INFO, "****************************************", 0));

    // Mark the Static flag to update in IMM
    pClassicOI->setAttrFlag(StaticPars);
}

//******************************************************************************
// ReadDynamicParameters()
//******************************************************************************
void CPS_BUAP_RPF::ReadDynamicParameters (sDynamicParameters &dynamicPar)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_RPF::ReadDynamicParameters()", 0));

    dynamicPar = m_Reladmpar.DynamicPar;

#if 0
    TRACE(("   STATE   CTA     CFFR    CSFR    FOLD    SOLD    NCT", 0));
    TRACE(("   %-3d     %-3d     %-3d     %-3d     %-3d     %-3d     %-3d", 0,
        m_Reladmpar.DynamicPar.State,
        m_Reladmpar.DynamicPar.CTA,
        m_Reladmpar.DynamicPar.NumFFR,
        m_Reladmpar.DynamicPar.NumSFR,
        m_Reladmpar.DynamicPar.NOld,
        m_Reladmpar.DynamicPar.KOld,
        m_Reladmpar.DynamicPar.NCT));

    TRACE(("", 0));
    TRACE(("   LPF     OMISSION        EXP", 0));
    TRACE(("   %-3d     %-3d             %02d%02d-%02d-%02d @ %02d:%02d", 0,
        m_Reladmpar.DynamicPar.LPF,
        m_Reladmpar.DynamicPar.Omission,
        m_Reladmpar.DynamicPar.EXP.century,
        m_Reladmpar.DynamicPar.EXP.year,
        m_Reladmpar.DynamicPar.EXP.month,
        m_Reladmpar.DynamicPar.EXP.day,
        m_Reladmpar.DynamicPar.EXP.hour,
        m_Reladmpar.DynamicPar.EXP.minute));
#endif

    TRACE((LOG_LEVEL_INFO, "****************************************", 0));
    TRACE((LOG_LEVEL_INFO, "loadingState = %d",0,m_Reladmpar.DynamicPar.State));
    TRACE((LOG_LEVEL_INFO, "currentTruncationAttempt = %d", 0, m_Reladmpar.DynamicPar.CTA));
    TRACE((LOG_LEVEL_INFO, "currentFileNumberInFFR = %d", 0, m_Reladmpar.DynamicPar.NumFFR));
    TRACE((LOG_LEVEL_INFO, "currentFileNumberInSFR = %d", 0, m_Reladmpar.DynamicPar.NumSFR));
    TRACE((LOG_LEVEL_INFO, "hightestFileNumberInFFR = %d", 0, m_Reladmpar.DynamicPar.NOld));
    TRACE((LOG_LEVEL_INFO, "highestFileNumberInSFR = %d", 0, m_Reladmpar.DynamicPar.KOld));
    TRACE((LOG_LEVEL_INFO, "numberOfTruncatedCommands = %d", 0, m_Reladmpar.DynamicPar.NCT));
    TRACE((LOG_LEVEL_INFO, "lastPermissibleGeneration = %d", 0, m_Reladmpar.DynamicPar.LPF));
    TRACE((LOG_LEVEL_INFO, "logOmissionSetting = %d", 0, m_Reladmpar.DynamicPar.Omission));
    TRACE((LOG_LEVEL_INFO, "expiringTimeSupervision = %02d%02d-%02d-%02d @ %02d:%02d", 0,
                                            m_Reladmpar.DynamicPar.EXP.century,
                                            m_Reladmpar.DynamicPar.EXP.year,
                                            m_Reladmpar.DynamicPar.EXP.month,
                                            m_Reladmpar.DynamicPar.EXP.day,
                                            m_Reladmpar.DynamicPar.EXP.hour,
                                            m_Reladmpar.DynamicPar.EXP.minute));
    TRACE((LOG_LEVEL_INFO, "****************************************", 0));
}

//******************************************************************************
// WriteDynamicParameters()
//******************************************************************************
void CPS_BUAP_RPF::WriteDynamicParameters(sDynamicParameters &dynamicPar)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_RPF::WriteDynamicParameters()", 0));

    m_Reladmpar.DynamicPar = dynamicPar;
    m_Reladmpar.Validities.Value |= DynamicPars;

#if 0
    TRACE(("   STATE   CTA     CFFR    CSFR    FOLD    SOLD    NCT", 0));
    TRACE(("   %-3d     %-3d     %-3d     %-3d     %-3d     %-3d     %-3d", 0,
        m_Reladmpar.DynamicPar.State,
        m_Reladmpar.DynamicPar.CTA,
        m_Reladmpar.DynamicPar.NumFFR,
        m_Reladmpar.DynamicPar.NumSFR,
        m_Reladmpar.DynamicPar.NOld,
        m_Reladmpar.DynamicPar.KOld,
        m_Reladmpar.DynamicPar.NCT));

    TRACE(("", 0));
    TRACE(("   LPF     OMISSION        EXP", 0));
    TRACE(("   %-3d     %-3d             %02d%02d-%02d-%02d @ %02d:%02d", 0,
        m_Reladmpar.DynamicPar.LPF,
        m_Reladmpar.DynamicPar.Omission,
        m_Reladmpar.DynamicPar.EXP.century,
        m_Reladmpar.DynamicPar.EXP.year,
        m_Reladmpar.DynamicPar.EXP.month,
        m_Reladmpar.DynamicPar.EXP.day,
        m_Reladmpar.DynamicPar.EXP.hour,
        m_Reladmpar.DynamicPar.EXP.minute));
#endif

    TRACE((LOG_LEVEL_INFO, "****************************************", 0));
    TRACE((LOG_LEVEL_INFO, "loadingState = %d",0,m_Reladmpar.DynamicPar.State));
    TRACE((LOG_LEVEL_INFO, "currentTruncationAttempt = %d", 0, m_Reladmpar.DynamicPar.CTA));
    TRACE((LOG_LEVEL_INFO, "currentFileNumberInFFR = %d", 0, m_Reladmpar.DynamicPar.NumFFR));
    TRACE((LOG_LEVEL_INFO, "currentFileNumberInSFR = %d", 0, m_Reladmpar.DynamicPar.NumSFR));
    TRACE((LOG_LEVEL_INFO, "hightestFileNumberInFFR = %d", 0, m_Reladmpar.DynamicPar.NOld));
    TRACE((LOG_LEVEL_INFO, "highestFileNumberInSFR = %d", 0, m_Reladmpar.DynamicPar.KOld));
    TRACE((LOG_LEVEL_INFO, "numberOfTruncatedCommands = %d", 0, m_Reladmpar.DynamicPar.NCT));
    TRACE((LOG_LEVEL_INFO, "lastPermissibleGeneration = %d", 0, m_Reladmpar.DynamicPar.LPF));
    TRACE((LOG_LEVEL_INFO, "logOmissionSetting = %d", 0, m_Reladmpar.DynamicPar.Omission));
    TRACE((LOG_LEVEL_INFO, "expiringTimeSupervision = %02d%02d-%02d-%02d @ %02d:%02d", 0,
                                            m_Reladmpar.DynamicPar.EXP.century,
                                            m_Reladmpar.DynamicPar.EXP.year,
                                            m_Reladmpar.DynamicPar.EXP.month,
                                            m_Reladmpar.DynamicPar.EXP.day,
                                            m_Reladmpar.DynamicPar.EXP.hour,
                                            m_Reladmpar.DynamicPar.EXP.minute));
    TRACE((LOG_LEVEL_INFO, "****************************************", 0));

    BUParamsOM buparamsOM;
    // Write Dynamic Parameters into IMM & raise event
    if (buparamsOM.isInitialized())
    {
        char buf[20];
        // Store the time supervision.
        sprintf(buf, "%02d%02d-%02d-%02d %02d:%02d", dynamicPar.EXP.century,
                                                     dynamicPar.EXP.year,
                                                     dynamicPar.EXP.month,
                                                     dynamicPar.EXP.day,
                                                     dynamicPar.EXP.hour,
                                                     dynamicPar.EXP.minute);

        BUParamsCommon::DynamicParam_ dynamicPars;
        dynamicPars.state = (int)dynamicPar.State;
        dynamicPars.cffr = (int)dynamicPar.NumFFR;
        dynamicPars.csfr = (dynamicPar.NumSFR == 99 ? 0 : dynamicPar.NumSFR);
        dynamicPars.cta = (int)dynamicPar.CTA;
        dynamicPars.nct = (int)dynamicPar.NCT;
        dynamicPars.fold = (int)dynamicPar.NOld;
        dynamicPars.sold = (int)dynamicPar.KOld;
        dynamicPars.lpf = (int)dynamicPar.LPF;
        dynamicPars.omission = (int)dynamicPar.Omission;
        dynamicPars.exp = std::string(buf);

        std::string transName("modifyDynamic");

        // Modify the attributes of Dynamic Parameters
        buparamsOM.updateDynamicParams(dynamicPars, transName);

        // Apply all requests associated at the transaction name
        buparamsOM.applyReq(transName);

    }
    // Mark the Dynamic flag to update in IMM
    pClassicOI->setAttrFlag(DynamicPars);
}

//******************************************************************************
// ReadLastAutoReload()
//******************************************************************************
void CPS_BUAP_RPF::ReadLastAutoReload (sLastAutoReload &lastAutoReload)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_RPF::ReadLastAutoReload()", 0));

    lastAutoReload = m_Reladmpar.LastAutoReload;

#if 0
    TRACE(("   CLH     GFILE   LPGF    NCT     SUP     OMISSION  ALOG", 0));
    TRACE(("   %-3d     %-3d     %-3d     %-3d     %-3d    %-3d       %-12u", 0,
        m_Reladmpar.LastAutoReload.ManAuto,
        m_Reladmpar.LastAutoReload.LRG,
        m_Reladmpar.LastAutoReload.LRGF,
        m_Reladmpar.LastAutoReload.NCT,
        m_Reladmpar.LastAutoReload.SUP*10,
        m_Reladmpar.LastAutoReload.Omission,
        m_Reladmpar.LastAutoReload.AssClogNum));
    TRACE(("", 0));
    TRACE(("   OPSRS               ODSS                ODSL", 0));
    TRACE(("   %02d%02d-%02d-%02d @ %02d:%02d"
        "  %02d%02d-%02d-%02d @ %02d:%02d"
        "  %02d%02d-%02d-%02d @ %02d:%02d", 0,
        m_Reladmpar.LastAutoReload.OPSRS.century,
        m_Reladmpar.LastAutoReload.OPSRS.year,
        m_Reladmpar.LastAutoReload.OPSRS.month,
        m_Reladmpar.LastAutoReload.OPSRS.day,
        m_Reladmpar.LastAutoReload.OPSRS.hour,
        m_Reladmpar.LastAutoReload.OPSRS.minute,
        m_Reladmpar.LastAutoReload.ODSS.century,
        m_Reladmpar.LastAutoReload.ODSS.year,
        m_Reladmpar.LastAutoReload.ODSS.month,
        m_Reladmpar.LastAutoReload.ODSS.day,
        m_Reladmpar.LastAutoReload.ODSS.hour,
        m_Reladmpar.LastAutoReload.ODSS.minute,
        m_Reladmpar.LastAutoReload.ODSL.century,
        m_Reladmpar.LastAutoReload.ODSL.year,
        m_Reladmpar.LastAutoReload.ODSL.month,
        m_Reladmpar.LastAutoReload.ODSL.day,
        m_Reladmpar.LastAutoReload.ODSL.hour,
        m_Reladmpar.LastAutoReload.ODSL.minute));
#endif

    TRACE((LOG_LEVEL_INFO, "****************************************", 0));
    TRACE((LOG_LEVEL_INFO, "commandLogMode = %d", 0, m_Reladmpar.LastAutoReload.ManAuto));
    TRACE((LOG_LEVEL_INFO, "generationFileNumber = %d", 0, m_Reladmpar.LastAutoReload.LRG));
    TRACE((LOG_LEVEL_INFO, "lastPossibleGenerationFile = %d", 0, m_Reladmpar.LastAutoReload.LRGF));
    TRACE((LOG_LEVEL_INFO, "currentTruncatedCommands = %d", 0, m_Reladmpar.LastAutoReload.NCT));
    TRACE((LOG_LEVEL_INFO, "supervisionTime = %d", 0, m_Reladmpar.LastAutoReload.SUP*10));
    TRACE((LOG_LEVEL_INFO, "logOmissionSetting = %d", 0, m_Reladmpar.LastAutoReload.Omission));
    TRACE((LOG_LEVEL_INFO, "logSubfileNoOfLastAutoReload = %u", 0, m_Reladmpar.LastAutoReload.AssClogNum));
    TRACE((LOG_LEVEL_INFO, "programStoreReferenceStoreOutputTime = %02d%02d-%02d-%02d @ %02d:%02d", 0,
                                m_Reladmpar.LastAutoReload.OPSRS.century,
                                m_Reladmpar.LastAutoReload.OPSRS.year,
                                m_Reladmpar.LastAutoReload.OPSRS.month,
                                m_Reladmpar.LastAutoReload.OPSRS.day,
                                m_Reladmpar.LastAutoReload.OPSRS.hour,
                                m_Reladmpar.LastAutoReload.OPSRS.minute));
    TRACE((LOG_LEVEL_INFO, "dataStoreSmallDumpOutputTime = %02d%02d-%02d-%02d @ %02d:%02d", 0,
                                m_Reladmpar.LastAutoReload.ODSS.century,
                                m_Reladmpar.LastAutoReload.ODSS.year,
                                m_Reladmpar.LastAutoReload.ODSS.month,
                                m_Reladmpar.LastAutoReload.ODSS.day,
                                m_Reladmpar.LastAutoReload.ODSS.hour,
                                m_Reladmpar.LastAutoReload.ODSS.minute));
    TRACE((LOG_LEVEL_INFO, "dataStoreLargeDumpOutputTime = %02d%02d-%02d-%02d @ %02d:%02d", 0,
                                m_Reladmpar.LastAutoReload.ODSL.century,
                                m_Reladmpar.LastAutoReload.ODSL.year,
                                m_Reladmpar.LastAutoReload.ODSL.month,
                                m_Reladmpar.LastAutoReload.ODSL.day,
                                m_Reladmpar.LastAutoReload.ODSL.hour,
                                m_Reladmpar.LastAutoReload.ODSL.minute));
    TRACE((LOG_LEVEL_INFO, "****************************************", 0));

}

//******************************************************************************
// WriteLastAutoReload()
//******************************************************************************
void CPS_BUAP_RPF::WriteLastAutoReload(sLastAutoReload &lastAutoReload)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_RPF::WriteLastAutoReload()", 0));

    m_Reladmpar.LastAutoReload = lastAutoReload;
    m_Reladmpar.Validities.Value |= LastAutoReload;

#if 0
    TRACE(("   CLH     GFILE   LPGF    NCT     SUP     OMISSION  ALOG", 0));
    TRACE(("   %-3d     %-3d     %-3d     %-3d     %-3d    %-3d       %-12u", 0,
        m_Reladmpar.LastAutoReload.ManAuto,
        m_Reladmpar.LastAutoReload.LRG,
        m_Reladmpar.LastAutoReload.LRGF,
        m_Reladmpar.LastAutoReload.NCT,
        m_Reladmpar.LastAutoReload.SUP*10,
        m_Reladmpar.LastAutoReload.Omission,
        m_Reladmpar.LastAutoReload.AssClogNum));
    TRACE(("", 0));
    TRACE(("   OPSRS               ODSS                ODSL", 0));
    TRACE(("   %02d%02d-%02d-%02d @ %02d:%02d"
        "  %02d%02d-%02d-%02d @ %02d:%02d"
        "  %02d%02d-%02d-%02d @ %02d:%02d", 0,
        m_Reladmpar.LastAutoReload.OPSRS.century,
        m_Reladmpar.LastAutoReload.OPSRS.year,
        m_Reladmpar.LastAutoReload.OPSRS.month,
        m_Reladmpar.LastAutoReload.OPSRS.day,
        m_Reladmpar.LastAutoReload.OPSRS.hour,
        m_Reladmpar.LastAutoReload.OPSRS.minute,
        m_Reladmpar.LastAutoReload.ODSS.century,
        m_Reladmpar.LastAutoReload.ODSS.year,
        m_Reladmpar.LastAutoReload.ODSS.month,
        m_Reladmpar.LastAutoReload.ODSS.day,
        m_Reladmpar.LastAutoReload.ODSS.hour,
        m_Reladmpar.LastAutoReload.ODSS.minute,
        m_Reladmpar.LastAutoReload.ODSL.century,
        m_Reladmpar.LastAutoReload.ODSL.year,
        m_Reladmpar.LastAutoReload.ODSL.month,
        m_Reladmpar.LastAutoReload.ODSL.day,
        m_Reladmpar.LastAutoReload.ODSL.hour,
        m_Reladmpar.LastAutoReload.ODSL.minute));
#endif

    TRACE((LOG_LEVEL_INFO, "****************************************", 0));
    TRACE((LOG_LEVEL_INFO, "commandLogMode = %d", 0, m_Reladmpar.LastAutoReload.ManAuto));
    TRACE((LOG_LEVEL_INFO, "generationFileNumber = %d", 0, m_Reladmpar.LastAutoReload.LRG));
    TRACE((LOG_LEVEL_INFO, "lastPossibleGenerationFile = %d", 0, m_Reladmpar.LastAutoReload.LRGF));
    TRACE((LOG_LEVEL_INFO, "currentTruncatedCommands = %d", 0, m_Reladmpar.LastAutoReload.NCT));
    TRACE((LOG_LEVEL_INFO, "supervisionTime = %d", 0, m_Reladmpar.LastAutoReload.SUP*10));
    TRACE((LOG_LEVEL_INFO, "logOmissionSetting = %d", 0, m_Reladmpar.LastAutoReload.Omission));
    TRACE((LOG_LEVEL_INFO, "logSubfileNoOfLastAutoReload = %u", 0, m_Reladmpar.LastAutoReload.AssClogNum));
    TRACE((LOG_LEVEL_INFO, "programStoreReferenceStoreOutputTime = %02d%02d-%02d-%02d @ %02d:%02d", 0,
                                m_Reladmpar.LastAutoReload.OPSRS.century,
                                m_Reladmpar.LastAutoReload.OPSRS.year,
                                m_Reladmpar.LastAutoReload.OPSRS.month,
                                m_Reladmpar.LastAutoReload.OPSRS.day,
                                m_Reladmpar.LastAutoReload.OPSRS.hour,
                                m_Reladmpar.LastAutoReload.OPSRS.minute));
    TRACE((LOG_LEVEL_INFO, "dataStoreSmallDumpOutputTime = %02d%02d-%02d-%02d @ %02d:%02d", 0,
                                m_Reladmpar.LastAutoReload.ODSS.century,
                                m_Reladmpar.LastAutoReload.ODSS.year,
                                m_Reladmpar.LastAutoReload.ODSS.month,
                                m_Reladmpar.LastAutoReload.ODSS.day,
                                m_Reladmpar.LastAutoReload.ODSS.hour,
                                m_Reladmpar.LastAutoReload.ODSS.minute));
    TRACE((LOG_LEVEL_INFO, "dataStoreLargeDumpOutputTime = %02d%02d-%02d-%02d @ %02d:%02d", 0,
                                m_Reladmpar.LastAutoReload.ODSL.century,
                                m_Reladmpar.LastAutoReload.ODSL.year,
                                m_Reladmpar.LastAutoReload.ODSL.month,
                                m_Reladmpar.LastAutoReload.ODSL.day,
                                m_Reladmpar.LastAutoReload.ODSL.hour,
                                m_Reladmpar.LastAutoReload.ODSL.minute));
    TRACE((LOG_LEVEL_INFO, "****************************************", 0));

    BUParamsOM buparamsOM;
    // Write Last Auto Reload Parameters into IMM & raise event.
    if (buparamsOM.isInitialized())
    {
        BUParamsCommon::LastAutoReload_ lastReloadPar;
        lastReloadPar.clh = (int)lastAutoReload.ManAuto;
        lastReloadPar.nct = (int)lastAutoReload.NCT;
        lastReloadPar.gfile = (int)lastAutoReload.LRG;
        lastReloadPar.lpgf = (int)lastAutoReload.LRGF;
        lastReloadPar.omission = (int)lastAutoReload.Omission;
        lastReloadPar.alog = (int)lastAutoReload.AssClogNum;
        lastReloadPar.sup = (int)lastAutoReload.SUP*10;

        char buf[20];
         // Store the PS RS output time.
         sprintf(buf, "%02d%02d-%02d-%02d %02d:%02d", lastAutoReload.OPSRS.century,
                                                      lastAutoReload.OPSRS.year,
                                                      lastAutoReload.OPSRS.month,
                                                      lastAutoReload.OPSRS.day,
                                                      lastAutoReload.OPSRS.hour,
                                                      lastAutoReload.OPSRS.minute);

        lastReloadPar.opsrs = std::string(buf);

        memset(buf, ' ', sizeof(buf));

         // Store the Small Data Store output time.
         sprintf(buf, "%02d%02d-%02d-%02d %02d:%02d", lastAutoReload.ODSS.century,
                                                      lastAutoReload.ODSS.year,
                                                      lastAutoReload.ODSS.month,
                                                      lastAutoReload.ODSS.day,
                                                      lastAutoReload.ODSS.hour,
                                                      lastAutoReload.ODSS.minute);
         lastReloadPar.odss = std::string(buf);

         memset(buf, ' ', sizeof(buf));

         // Store the Large Data Store output time.
         sprintf(buf, "%02d%02d-%02d-%02d %02d:%02d", lastAutoReload.ODSL.century,
                                                      lastAutoReload.ODSL.year,
                                                      lastAutoReload.ODSL.month,
                                                      lastAutoReload.ODSL.day,
                                                      lastAutoReload.ODSL.hour,
                                                      lastAutoReload.ODSL.minute);
         lastReloadPar.odsl = std::string(buf);

        std::string transName("modifyAutoReload");

        // Modify the attributes of Last Auto Reload Parameters
        buparamsOM.updateLastReloadParams(lastReloadPar, transName);

        // Apply all requests associated at the transaction name
        buparamsOM.applyReq(transName);
    }
    // Mark the Last Auto Reload flag to update in IMM
    pClassicOI->setAttrFlag(LastAutoReload);
}

//******************************************************************************
// ReadCommandLogReferences()
//******************************************************************************
void CPS_BUAP_RPF::ReadCommandLogReferences (sCommandLogReferences &clogRefs)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_RPF::ReadCommandLogReferences()", 0));

    clogRefs = m_Reladmpar.CLogRefs;

#if 0
    TRACE(("   CURRENT         MAIN            PREPARED", 0));
    TRACE(("   %-12u    %-12u    %-12u", 0,
        m_Reladmpar.CLogRefs.Current,
        m_Reladmpar.CLogRefs.Main,
        m_Reladmpar.CLogRefs.Prepared));
#endif
    TRACE((LOG_LEVEL_INFO, "****************************************", 0));
    TRACE((LOG_LEVEL_INFO, "currentSubfileNumber = %u", 0, m_Reladmpar.CLogRefs.Current));
    TRACE((LOG_LEVEL_INFO, "mainStoreSubfileNumber = %u", 0, m_Reladmpar.CLogRefs.Main));
    TRACE((LOG_LEVEL_INFO, "preparedSubfileNumber = %u", 0, m_Reladmpar.CLogRefs.Prepared));
    TRACE((LOG_LEVEL_INFO, "****************************************", 0));
}

//******************************************************************************
// WriteCommandLogReferences()
//******************************************************************************
void CPS_BUAP_RPF::WriteCommandLogReferences(sCommandLogReferences &clogRefs)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_RPF::WriteCommandLogReferences()", 0));

    m_Reladmpar.CLogRefs = clogRefs;
    m_Reladmpar.Validities.Value |= CommandLogReferences;

#if 0
    TRACE(("   CURRENT         MAIN            PREPARED", 0));
    TRACE(("   %-12u    %-12u    %-12u", 0,
        m_Reladmpar.CLogRefs.Current,
        m_Reladmpar.CLogRefs.Main,
        m_Reladmpar.CLogRefs.Prepared));
#endif
    TRACE((LOG_LEVEL_INFO, "****************************************", 0));
    TRACE((LOG_LEVEL_INFO, "currentSubfileNumber = %u", 0, m_Reladmpar.CLogRefs.Current));
    TRACE((LOG_LEVEL_INFO, "mainStoreSubfileNumber = %u", 0, m_Reladmpar.CLogRefs.Main));
    TRACE((LOG_LEVEL_INFO, "preparedSubfileNumber = %u", 0, m_Reladmpar.CLogRefs.Prepared));
    TRACE((LOG_LEVEL_INFO, "****************************************", 0));

    BUParamsOM buparamsOM;
    // Write command log references into IMM & raise event.
    if (buparamsOM.isInitialized())
    {
        BUParamsCommon::CommmandLogReference_ cmdLogPar;
        cmdLogPar.current = (int)clogRefs.Current;
        cmdLogPar.main = (int)clogRefs.Main;
        cmdLogPar.prepared = (int)clogRefs.Prepared;

        std::string transName("modifyCmdLogRef");

        // Modify attributes of Command Log Reference parameters
        buparamsOM.updateCmdLogParams(cmdLogPar, transName);

        // Apply all requests associated at the transaction name
        buparamsOM.applyReq(transName);
    }
    // Mark the Command Log Reference flag to update in IMM
    pClassicOI->setAttrFlag(CommandLogReferences);
}

//******************************************************************************
// ReadDelayedMeasures()
//******************************************************************************
void CPS_BUAP_RPF::ReadDelayedMeasures (sDelayedMeasures &delayedMeasures)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_RPF::ReadDelayedMeasures()", 0));

    delayedMeasures = m_Reladmpar.DelayedMeasures;

#if 0
    TRACE(("   DM1   DM2   DM3   DM4   DM5   DM6   DM7   DM8", 0));
    TRACE(("   %-3d   %-3d   %-3d   %-3d   %-3d   %-3d   %-3d   %-3d", 0,
        m_Reladmpar.DelayedMeasures.Measures[0],
        m_Reladmpar.DelayedMeasures.Measures[1],
        m_Reladmpar.DelayedMeasures.Measures[2],
        m_Reladmpar.DelayedMeasures.Measures[3],
        m_Reladmpar.DelayedMeasures.Measures[4],
        m_Reladmpar.DelayedMeasures.Measures[5],
        m_Reladmpar.DelayedMeasures.Measures[6],
        m_Reladmpar.DelayedMeasures.Measures[7]));
#endif
    TRACE((LOG_LEVEL_INFO, "****************************************", 0));
    TRACE((LOG_LEVEL_INFO, "preparedCommandLog = %d", 0, m_Reladmpar.DelayedMeasures.Measures[0]));
    TRACE((LOG_LEVEL_INFO, "mainStoreCommandLog = %d", 0, m_Reladmpar.DelayedMeasures.Measures[1]));
    TRACE((LOG_LEVEL_INFO, "currentCommandLog = %d", 0, m_Reladmpar.DelayedMeasures.Measures[2]));
    TRACE((LOG_LEVEL_INFO, "mainStoreStepping = %d", 0, m_Reladmpar.DelayedMeasures.Measures[3]));
    TRACE((LOG_LEVEL_INFO, "dumpStepping = %d", 0, m_Reladmpar.DelayedMeasures.Measures[4]));
    TRACE((LOG_LEVEL_INFO, "preparedStepping = %d", 0, m_Reladmpar.DelayedMeasures.Measures[5]));
    TRACE((LOG_LEVEL_INFO, "smallBackupArea = %d", 0, m_Reladmpar.DelayedMeasures.Measures[6]));
    TRACE((LOG_LEVEL_INFO, "largeBackupArea = %d", 0, m_Reladmpar.DelayedMeasures.Measures[7]));
    TRACE((LOG_LEVEL_INFO, "****************************************", 0));
}

//******************************************************************************
// WriteDelayedMeasures()
//******************************************************************************
void CPS_BUAP_RPF::WriteDelayedMeasures(sDelayedMeasures &delayedMeasures)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_RPF::WriteDelayedMeasures()", 0));

    m_Reladmpar.DelayedMeasures = delayedMeasures;
    m_Reladmpar.Validities.Value |= DelayedMeasures;

#if 0
    TRACE(("   DM1   DM2   DM3   DM4   DM5   DM6   DM7   DM8", 0));
    TRACE(("   %-3d   %-3d   %-3d   %-3d   %-3d   %-3d   %-3d   %-3d", 0,
        m_Reladmpar.DelayedMeasures.Measures[0],
        m_Reladmpar.DelayedMeasures.Measures[1],
        m_Reladmpar.DelayedMeasures.Measures[2],
        m_Reladmpar.DelayedMeasures.Measures[3],
        m_Reladmpar.DelayedMeasures.Measures[4],
        m_Reladmpar.DelayedMeasures.Measures[5],
        m_Reladmpar.DelayedMeasures.Measures[6],
        m_Reladmpar.DelayedMeasures.Measures[7]));
#endif
    TRACE((LOG_LEVEL_INFO, "****************************************", 0));
    TRACE((LOG_LEVEL_INFO, "preparedCommandLog = %d", 0, m_Reladmpar.DelayedMeasures.Measures[0]));
    TRACE((LOG_LEVEL_INFO, "mainStoreCommandLog = %d", 0, m_Reladmpar.DelayedMeasures.Measures[1]));
    TRACE((LOG_LEVEL_INFO, "currentCommandLog = %d", 0, m_Reladmpar.DelayedMeasures.Measures[2]));
    TRACE((LOG_LEVEL_INFO, "mainStoreStepping = %d", 0, m_Reladmpar.DelayedMeasures.Measures[3]));
    TRACE((LOG_LEVEL_INFO, "dumpStepping = %d", 0, m_Reladmpar.DelayedMeasures.Measures[4]));
    TRACE((LOG_LEVEL_INFO, "preparedStepping = %d", 0, m_Reladmpar.DelayedMeasures.Measures[5]));
    TRACE((LOG_LEVEL_INFO, "smallBackupArea = %d", 0, m_Reladmpar.DelayedMeasures.Measures[6]));
    TRACE((LOG_LEVEL_INFO, "largeBackupArea = %d", 0, m_Reladmpar.DelayedMeasures.Measures[7]));
    TRACE((LOG_LEVEL_INFO, "****************************************", 0));

    BUParamsOM buparamsOM;
    // Write Delayed Measures parameters into IMM & raise event.
    if (buparamsOM.isInitialized())
    {
        BUParamsCommon::DelayedMeasure_ dmPar;
        dmPar.dm[0] = delayedMeasures.Measures[0];
        dmPar.dm[1] = delayedMeasures.Measures[1];
        dmPar.dm[2] = delayedMeasures.Measures[2];
        dmPar.dm[3] = delayedMeasures.Measures[3];
        dmPar.dm[4] = delayedMeasures.Measures[4];
        dmPar.dm[5] = delayedMeasures.Measures[5];
        dmPar.dm[6] = delayedMeasures.Measures[6];
        dmPar.dm[7] = delayedMeasures.Measures[7];

        std::string transName("modifyDMeasures");

        // Modify attributes of Delayed Measures parameters
        buparamsOM.updateDMeasuresParams(dmPar, transName);

        // Apply all requests associated at the transaction name
        buparamsOM.applyReq(transName);

    }
    // Mark the Delayed Measures flag to update in IMM
    pClassicOI->setAttrFlag(DelayedMeasures);
}


//******************************************************************************
// FinishUpdate()
//******************************************************************************
void CPS_BUAP_RPF::FinishUpdate()
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_RPF::FinishUpdate()", 0));

#if 0
    long new_pos =  _lseek(m_fh, 0, SEEK_SET);
    if (new_pos == -1L)
    {
        EVENT((CPS_BUAP_Events::reladmpar_seek_fail, 0, strerror(errno), m_Filename));
        THROW(strerror(errno));
    }

    int bytes_written =  write(m_fh, (char *)&m_Reladmpar, sizeof(sReladmpar));
    if (bytes_written == -1)
    {
        EVENT((CPS_BUAP_Events::reladmpar_write_fail, 0, strerror(errno), m_Filename));
        THROW(strerror(errno));
    }
#endif

    if (!this->writeReladmpar(m_Filename, m_Reladmpar))
    {
        EVENT((CPS_BUAP_Events::reladmpar_write_fail, 0, strerror(errno), m_Filename));
        THROW(strerror(errno));
    }

    // Notify to Classic OI to update the "isValid" attributes
    pClassicOI->notifyToUpdateIsValidAttrs();
}

//******************************************************************************
// getReloadFile()
//******************************************************************************
const sReladmpar& CPS_BUAP_RPF::getReloadFile() const
{
    return m_Reladmpar;
}

//******************************************************************************
// readReladmpar()
//******************************************************************************
bool CPS_BUAP_RPF::readReladmpar(const char* fileName, sReladmpar& reladmpar)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_RPF::readReladmpar(%s)", 0, fileName));
    using boost::property_tree::ptree;

    ptree pt;
    try
    {
        read_ini(fileName, pt);
    }
    catch(...)
    {
        TRACE((LOG_LEVEL_WARN, "CPS_BUAP_RPF::readReladmpar() returns false ", 0));
        return false;
    }
    //####################### Read the validity flags ###################################
    //###################################################################################
    reladmpar.Validities.Value |= General;

    if (ParseINIFunx::getPrivateProfileInt(BUParamsCommon::VALIDITY_FLAG.c_str(),
        BUParamsCommon::IMM_ATTR_STATIC_VALID.c_str(), 0, pt))
    {
        reladmpar.Validities.Value |= StaticPars;
    }

    if (ParseINIFunx::getPrivateProfileInt(BUParamsCommon::VALIDITY_FLAG.c_str(),
        BUParamsCommon::IMM_ATTR_DYNAMIC_VALID.c_str(), 0, pt))
    {
        reladmpar.Validities.Value |= DynamicPars;
    }

    if (ParseINIFunx::getPrivateProfileInt(BUParamsCommon::VALIDITY_FLAG.c_str(),
        BUParamsCommon::IMM_ATTR_LASTRELOAD_VALID.c_str(), 0, pt))
    {
        reladmpar.Validities.Value |= LastAutoReload;
    }

    if (ParseINIFunx::getPrivateProfileInt(BUParamsCommon::VALIDITY_FLAG.c_str(),
        BUParamsCommon::IMM_ATTR_CMDLOGREF_VALID.c_str(), 0, pt))
    {
        reladmpar.Validities.Value |= CommandLogReferences;
    }

    if (ParseINIFunx::getPrivateProfileInt(BUParamsCommon::VALIDITY_FLAG.c_str(),
        BUParamsCommon::IMM_ATTR_DMEASURES_VALID.c_str(), 0, pt))
    {
        reladmpar.Validities.Value |= DelayedMeasures;
    }

    TRACE(("Validity flags = %d", 0, reladmpar.Validities.Value));
    //####################### Read StaticParams ###################################
    //#############################################################################
    // TODO:
    reladmpar.StaticPar.ManAuto = ParseINIFunx::getPrivateProfileInt(
                                BUParamsCommon::IMM_CLASS_SP_NAME_.c_str(),
                                BUParamsCommon::IMM_ATTR_CLH.c_str(), 1, pt);

    reladmpar.StaticPar.INCL1 = ParseINIFunx::getPrivateProfileInt(
            BUParamsCommon::IMM_CLASS_SP_NAME_.c_str(),
            BUParamsCommon::IMM_ATTR_INCL1.c_str(), 0, pt);

    reladmpar.StaticPar.INCL2 = ParseINIFunx::getPrivateProfileInt(
            BUParamsCommon::IMM_CLASS_SP_NAME_.c_str(),
            BUParamsCommon::IMM_ATTR_INCL2.c_str(), 0, pt);

    string tempDay = ParseINIFunx::getPrivateProfileString(BUParamsCommon::IMM_CLASS_SP_NAME_.c_str(),
            BUParamsCommon::IMM_ATTR_INCLA.c_str(), "1990-01-01", pt);
    //reladmpar.StaticPar.INCLAcentury = 19;
    //reladmpar.StaticPar.INCLAyear = 90;
    //reladmpar.StaticPar.INCLAmonth = 1;
    //reladmpar.StaticPar.INCLAday = 1;
    ParseINIFunx::splitString(tempDay, reladmpar.StaticPar.INCLAcentury,
            reladmpar.StaticPar.INCLAyear,
            reladmpar.StaticPar.INCLAmonth,
            reladmpar.StaticPar.INCLAday);

    reladmpar.StaticPar.LOAZ = ParseINIFunx::getPrivateProfileInt(
            BUParamsCommon::IMM_CLASS_SP_NAME_.c_str(),
            BUParamsCommon::IMM_ATTR_LOAZ.c_str(), 0, pt);

    reladmpar.StaticPar.NTAZ = ParseINIFunx::getPrivateProfileInt(
            BUParamsCommon::IMM_CLASS_SP_NAME_.c_str(),
            BUParamsCommon::IMM_ATTR_NTAZ.c_str(), 0, pt);

    reladmpar.StaticPar.NTCZ = ParseINIFunx::getPrivateProfileInt(
            BUParamsCommon::IMM_CLASS_SP_NAME_.c_str(),
            BUParamsCommon::IMM_ATTR_NTCZ.c_str(), 10, pt);

    reladmpar.StaticPar.SUP = (ParseINIFunx::getPrivateProfileInt(
            BUParamsCommon::IMM_CLASS_SP_NAME_.c_str(),
            BUParamsCommon::IMM_ATTR_SUP.c_str(), 60, pt))/10;

    //####################### Read DynamicParams ###################################
    //#############################################################################
    reladmpar.DynamicPar.State = ParseINIFunx::getPrivateProfileInt(
            BUParamsCommon::IMM_CLASS_DP_NAME_.c_str(),
            BUParamsCommon::IMM_ATTR_STATE.c_str(), 1, pt);

    reladmpar.DynamicPar.CTA = ParseINIFunx::getPrivateProfileInt(
            BUParamsCommon::IMM_CLASS_DP_NAME_.c_str(),
            BUParamsCommon::IMM_ATTR_CTA.c_str(), 0, pt);

    reladmpar.DynamicPar.NumFFR = ParseINIFunx::getPrivateProfileInt(
            BUParamsCommon::IMM_CLASS_DP_NAME_.c_str(),
            BUParamsCommon::IMM_ATTR_CFFR.c_str(), 0, pt);

    // TODO:
    int tempSFR = ParseINIFunx::getPrivateProfileInt(
            BUParamsCommon::IMM_CLASS_DP_NAME_.c_str(),
            BUParamsCommon::IMM_ATTR_CSFR.c_str(), 0, pt);
    reladmpar.DynamicPar.NumSFR = (tempSFR == 0 ? 99 : tempSFR);

    reladmpar.DynamicPar.NOld = ParseINIFunx::getPrivateProfileInt(
            BUParamsCommon::IMM_CLASS_DP_NAME_.c_str(),
            BUParamsCommon::IMM_ATTR_FOLD.c_str(), 0, pt);

    reladmpar.DynamicPar.KOld = ParseINIFunx::getPrivateProfileInt(
            BUParamsCommon::IMM_CLASS_DP_NAME_.c_str(),
            BUParamsCommon::IMM_ATTR_SOLD.c_str(), 0, pt);

    reladmpar.DynamicPar.NCT = ParseINIFunx::getPrivateProfileInt(
            BUParamsCommon::IMM_CLASS_DP_NAME_.c_str(),
            BUParamsCommon::IMM_ATTR_NCT.c_str(), 0, pt);

    reladmpar.DynamicPar.LPF = ParseINIFunx::getPrivateProfileInt(
            BUParamsCommon::IMM_CLASS_DP_NAME_.c_str(),
            BUParamsCommon::IMM_ATTR_LPF.c_str(), 0, pt);

    reladmpar.DynamicPar.Omission = ParseINIFunx::getPrivateProfileInt(
            BUParamsCommon::IMM_CLASS_DP_NAME_.c_str(),
            BUParamsCommon::IMM_ATTR_OMISSION.c_str(), 0, pt);

    tempDay = ParseINIFunx::getPrivateProfileString(BUParamsCommon::IMM_CLASS_DP_NAME_.c_str(),
                BUParamsCommon::IMM_ATTR_EXP.c_str(), "1990-01-01 00:00", pt);

    ParseINIFunx::splitString(tempDay, reladmpar.DynamicPar.EXP.century,
            reladmpar.DynamicPar.EXP.year,
            reladmpar.DynamicPar.EXP.month,
            reladmpar.DynamicPar.EXP.day,
            reladmpar.DynamicPar.EXP.hour,
            reladmpar.DynamicPar.EXP.minute, true);

    //####################### Read CommandLogReferences ###########################
    //#############################################################################
    reladmpar.CLogRefs.Current = ParseINIFunx::getPrivateProfileInt(
            BUParamsCommon::IMM_CLASS_CMDLOG_NAME_.c_str(),
            BUParamsCommon::IMM_ATTR_CURRENT.c_str(), 0, pt);

    reladmpar.CLogRefs.Main = ParseINIFunx::getPrivateProfileInt(
            BUParamsCommon::IMM_CLASS_CMDLOG_NAME_.c_str(),
            BUParamsCommon::IMM_ATTR_MAIN.c_str(), 0, pt);

    reladmpar.CLogRefs.Prepared = ParseINIFunx::getPrivateProfileInt(
            BUParamsCommon::IMM_CLASS_CMDLOG_NAME_.c_str(),
            BUParamsCommon::IMM_ATTR_PREPARED.c_str(), 0, pt);

    //####################### Read LastAutoReload #################################
    //#############################################################################
    reladmpar.LastAutoReload.ManAuto = ParseINIFunx::getPrivateProfileInt(
            BUParamsCommon::IMM_CLASS_LAR_NAME_.c_str(),
            BUParamsCommon::IMM_ATTR_LAR_CLH.c_str(), 0, pt);

    reladmpar.LastAutoReload.LRG = ParseINIFunx::getPrivateProfileInt(
            BUParamsCommon::IMM_CLASS_LAR_NAME_.c_str(),
            BUParamsCommon::IMM_ATTR_GFILE.c_str(), 0, pt);

    reladmpar.LastAutoReload.LRGF = ParseINIFunx::getPrivateProfileInt(
            BUParamsCommon::IMM_CLASS_LAR_NAME_.c_str(),
            BUParamsCommon::IMM_ATTR_LPGF.c_str(), 0, pt);

    reladmpar.LastAutoReload.NCT = ParseINIFunx::getPrivateProfileInt(
            BUParamsCommon::IMM_CLASS_LAR_NAME_.c_str(),
            BUParamsCommon::IMM_ATTR_LAR_NCT.c_str(), 0, pt);

    reladmpar.LastAutoReload.SUP = (ParseINIFunx::getPrivateProfileInt(
            BUParamsCommon::IMM_CLASS_LAR_NAME_.c_str(),
            BUParamsCommon::IMM_ATTR_LAR_SUP.c_str(), 0, pt))/10;

    reladmpar.LastAutoReload.Omission = ParseINIFunx::getPrivateProfileInt(
            BUParamsCommon::IMM_CLASS_LAR_NAME_.c_str(),
            BUParamsCommon::IMM_ATTR_LAR_OMISSION.c_str(), 0, pt);

    reladmpar.LastAutoReload.AssClogNum = ParseINIFunx::getPrivateProfileInt(
            BUParamsCommon::IMM_CLASS_LAR_NAME_.c_str(),
            BUParamsCommon::IMM_ATTR_ALOG.c_str(), 0, pt);

    tempDay = ParseINIFunx::getPrivateProfileString(BUParamsCommon::IMM_CLASS_LAR_NAME_.c_str(),
                BUParamsCommon::IMM_ATTR_OPSRS.c_str(), "0000-00-00 00:00", pt);

    ParseINIFunx::splitString(tempDay, reladmpar.LastAutoReload.OPSRS.century,
            reladmpar.LastAutoReload.OPSRS.year,
            reladmpar.LastAutoReload.OPSRS.month,
            reladmpar.LastAutoReload.OPSRS.day,
            reladmpar.LastAutoReload.OPSRS.hour,
            reladmpar.LastAutoReload.OPSRS.minute, false);

    tempDay = ParseINIFunx::getPrivateProfileString(BUParamsCommon::IMM_CLASS_LAR_NAME_.c_str(),
                BUParamsCommon::IMM_ATTR_ODSS.c_str(), "0000-00-00 00:00", pt);

    ParseINIFunx::splitString(tempDay, reladmpar.LastAutoReload.ODSS.century,
            reladmpar.LastAutoReload.ODSS.year,
            reladmpar.LastAutoReload.ODSS.month,
            reladmpar.LastAutoReload.ODSS.day,
            reladmpar.LastAutoReload.ODSS.hour,
            reladmpar.LastAutoReload.ODSS.minute, false);

    tempDay = ParseINIFunx::getPrivateProfileString(BUParamsCommon::IMM_CLASS_LAR_NAME_.c_str(),
                BUParamsCommon::IMM_ATTR_ODSL.c_str(), "0000-00-00 00:00", pt);

    ParseINIFunx::splitString(tempDay, reladmpar.LastAutoReload.ODSL.century,
            reladmpar.LastAutoReload.ODSL.year,
            reladmpar.LastAutoReload.ODSL.month,
            reladmpar.LastAutoReload.ODSL.day,
            reladmpar.LastAutoReload.ODSL.hour,
            reladmpar.LastAutoReload.ODSL.minute, false);

    //####################### Read DelayedMeasures #################################
    //#############################################################################
    reladmpar.DelayedMeasures.Measures[0] = ParseINIFunx::getPrivateProfileInt(
            BUParamsCommon::IMM_CLASS_DM_NAME_.c_str(),
            BUParamsCommon::IMM_ATTR_DM1.c_str(), 0, pt);

    reladmpar.DelayedMeasures.Measures[1] = ParseINIFunx::getPrivateProfileInt(
            BUParamsCommon::IMM_CLASS_DM_NAME_.c_str(),
            BUParamsCommon::IMM_ATTR_DM2.c_str(), 0, pt);

    reladmpar.DelayedMeasures.Measures[2] = ParseINIFunx::getPrivateProfileInt(
            BUParamsCommon::IMM_CLASS_DM_NAME_.c_str(),
            BUParamsCommon::IMM_ATTR_DM3.c_str(), 0, pt);

    reladmpar.DelayedMeasures.Measures[3] = ParseINIFunx::getPrivateProfileInt(
            BUParamsCommon::IMM_CLASS_DM_NAME_.c_str(),
            BUParamsCommon::IMM_ATTR_DM4.c_str(), 0, pt);

    reladmpar.DelayedMeasures.Measures[4] = ParseINIFunx::getPrivateProfileInt(
            BUParamsCommon::IMM_CLASS_DM_NAME_.c_str(),
            BUParamsCommon::IMM_ATTR_DM5.c_str(), 0, pt);

    reladmpar.DelayedMeasures.Measures[5] = ParseINIFunx::getPrivateProfileInt(
            BUParamsCommon::IMM_CLASS_DM_NAME_.c_str(),
            BUParamsCommon::IMM_ATTR_DM6.c_str(), 0, pt);

    reladmpar.DelayedMeasures.Measures[6] = ParseINIFunx::getPrivateProfileInt(
            BUParamsCommon::IMM_CLASS_DM_NAME_.c_str(),
            BUParamsCommon::IMM_ATTR_DM7.c_str(), 0, pt);

    reladmpar.DelayedMeasures.Measures[7] = ParseINIFunx::getPrivateProfileInt(
            BUParamsCommon::IMM_CLASS_DM_NAME_.c_str(),
            BUParamsCommon::IMM_ATTR_DM8.c_str(), 0, pt);

    TRACE((LOG_LEVEL_INFO, "CPS_BUAP_RPF::readReladmpar() returns true ", 0));
    return true;
}


//******************************************************************************
// writeReladmpar()
//******************************************************************************
bool CPS_BUAP_RPF::writeReladmpar(const char* fileName, sReladmpar& reladmpar)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_RPF::writeReladmpar(%s)", 0, fileName));
    using boost::property_tree::ptree;

    ptree pt;
    try
    {
        read_ini(fileName, pt);

        //####################### Write the validity flags ##################################
        //###################################################################################
        ParseINIFunx::writePrivateProfile<int>(BUParamsCommon::VALIDITY_FLAG.c_str(),
                    BUParamsCommon::IMM_ATTR_STATIC_VALID.c_str(),
                    (reladmpar.Validities.Value & StaticPars) ? 1 : 0, pt);

        ParseINIFunx::writePrivateProfile<int>(BUParamsCommon::VALIDITY_FLAG.c_str(),
                    BUParamsCommon::IMM_ATTR_DYNAMIC_VALID.c_str(),
                    (reladmpar.Validities.Value & DynamicPars) ? 1 : 0, pt);

        ParseINIFunx::writePrivateProfile<int>(BUParamsCommon::VALIDITY_FLAG.c_str(),
                    BUParamsCommon::IMM_ATTR_LASTRELOAD_VALID.c_str(),
                    (reladmpar.Validities.Value & LastAutoReload) ? 1 : 0, pt);

        ParseINIFunx::writePrivateProfile<int>(BUParamsCommon::VALIDITY_FLAG.c_str(),
                    BUParamsCommon::IMM_ATTR_CMDLOGREF_VALID.c_str(),
                    (reladmpar.Validities.Value & CommandLogReferences) ? 1 : 0, pt);

        ParseINIFunx::writePrivateProfile<int>(BUParamsCommon::VALIDITY_FLAG.c_str(),
                    BUParamsCommon::IMM_ATTR_DMEASURES_VALID.c_str(),
                    (reladmpar.Validities.Value & DelayedMeasures) ? 1 : 0, pt);

        //####################### Write StaticParams ##################################
        //#############################################################################
        ParseINIFunx::writePrivateProfile<int>(BUParamsCommon::IMM_CLASS_SP_NAME_.c_str(),
                                    BUParamsCommon::IMM_ATTR_CLH.c_str(),
                                    reladmpar.StaticPar.ManAuto, pt);

        ParseINIFunx::writePrivateProfile<int>(
                BUParamsCommon::IMM_CLASS_SP_NAME_.c_str(),
                BUParamsCommon::IMM_ATTR_INCL1.c_str(), reladmpar.StaticPar.INCL1, pt);

        ParseINIFunx::writePrivateProfile<int>(
                BUParamsCommon::IMM_CLASS_SP_NAME_.c_str(),
                BUParamsCommon::IMM_ATTR_INCL2.c_str(), reladmpar.StaticPar.INCL2, pt);

        char buf[20];
        // Store the time supervision.
        sprintf(buf, "%02d%02d-%02d-%02d", reladmpar.StaticPar.INCLAcentury,
                                           reladmpar.StaticPar.INCLAyear,
                                           reladmpar.StaticPar.INCLAmonth,
                                           reladmpar.StaticPar.INCLAday);

        ParseINIFunx::writePrivateProfile<char *>(BUParamsCommon::IMM_CLASS_SP_NAME_.c_str(),
                                            BUParamsCommon::IMM_ATTR_INCLA.c_str(), buf, pt);

        ParseINIFunx::writePrivateProfile<int>(
                BUParamsCommon::IMM_CLASS_SP_NAME_.c_str(),
                BUParamsCommon::IMM_ATTR_LOAZ.c_str(), reladmpar.StaticPar.LOAZ, pt);

        ParseINIFunx::writePrivateProfile<int>(
                BUParamsCommon::IMM_CLASS_SP_NAME_.c_str(),
                BUParamsCommon::IMM_ATTR_NTAZ.c_str(), reladmpar.StaticPar.NTAZ, pt);

        ParseINIFunx::writePrivateProfile<int>(
                BUParamsCommon::IMM_CLASS_SP_NAME_.c_str(),
                BUParamsCommon::IMM_ATTR_NTCZ.c_str(), reladmpar.StaticPar.NTCZ, pt);

        ParseINIFunx::writePrivateProfile<int>(
                BUParamsCommon::IMM_CLASS_SP_NAME_.c_str(),
                BUParamsCommon::IMM_ATTR_SUP.c_str(), reladmpar.StaticPar.SUP*10, pt);

        //####################### Write DynamicParams #################################
        //#############################################################################
        ParseINIFunx::writePrivateProfile<int>(
                BUParamsCommon::IMM_CLASS_DP_NAME_.c_str(),
                BUParamsCommon::IMM_ATTR_STATE.c_str(), reladmpar.DynamicPar.State, pt);

        ParseINIFunx::writePrivateProfile<int>(
                BUParamsCommon::IMM_CLASS_DP_NAME_.c_str(),
                BUParamsCommon::IMM_ATTR_CTA.c_str(), reladmpar.DynamicPar.CTA, pt);

        ParseINIFunx::writePrivateProfile<int>(
                BUParamsCommon::IMM_CLASS_DP_NAME_.c_str(),
                BUParamsCommon::IMM_ATTR_CFFR.c_str(), reladmpar.DynamicPar.NumFFR, pt);

        // TODO:
        ParseINIFunx::writePrivateProfile<int>(
                BUParamsCommon::IMM_CLASS_DP_NAME_.c_str(),
                BUParamsCommon::IMM_ATTR_CSFR.c_str(), reladmpar.DynamicPar.NumSFR == 99 ? 0 : reladmpar.DynamicPar.NumSFR, pt);

        ParseINIFunx::writePrivateProfile<int>(
                BUParamsCommon::IMM_CLASS_DP_NAME_.c_str(),
                BUParamsCommon::IMM_ATTR_FOLD.c_str(), reladmpar.DynamicPar.NOld, pt);

        ParseINIFunx::writePrivateProfile<int>(
                BUParamsCommon::IMM_CLASS_DP_NAME_.c_str(),
                BUParamsCommon::IMM_ATTR_SOLD.c_str(), reladmpar.DynamicPar.KOld, pt);

        ParseINIFunx::writePrivateProfile<int>(
                BUParamsCommon::IMM_CLASS_DP_NAME_.c_str(),
                BUParamsCommon::IMM_ATTR_NCT.c_str(), reladmpar.DynamicPar.NCT, pt);

        ParseINIFunx::writePrivateProfile<int>(
                BUParamsCommon::IMM_CLASS_DP_NAME_.c_str(),
                BUParamsCommon::IMM_ATTR_LPF.c_str(), reladmpar.DynamicPar.LPF, pt);

        ParseINIFunx::writePrivateProfile<int>(
                BUParamsCommon::IMM_CLASS_DP_NAME_.c_str(),
                BUParamsCommon::IMM_ATTR_OMISSION.c_str(), reladmpar.DynamicPar.Omission, pt);

        memset(buf, ' ', sizeof(buf));
        // Store the time supervision.
        sprintf(buf, "%02d%02d-%02d-%02d %02d:%02d", reladmpar.DynamicPar.EXP.century,
                                                     reladmpar.DynamicPar.EXP.year,
                                                     reladmpar.DynamicPar.EXP.month,
                                                     reladmpar.DynamicPar.EXP.day,
                                                     reladmpar.DynamicPar.EXP.hour,
                                                     reladmpar.DynamicPar.EXP.minute);

        ParseINIFunx::writePrivateProfile<char *>(BUParamsCommon::IMM_CLASS_DP_NAME_.c_str(),
                    BUParamsCommon::IMM_ATTR_EXP.c_str(), buf, pt);

        //####################### Write CommandLogReferences ##########################
        //#############################################################################
        ParseINIFunx::writePrivateProfile<int>(
                BUParamsCommon::IMM_CLASS_CMDLOG_NAME_.c_str(),
                BUParamsCommon::IMM_ATTR_CURRENT.c_str(), reladmpar.CLogRefs.Current, pt);

        ParseINIFunx::writePrivateProfile<int>(
                BUParamsCommon::IMM_CLASS_CMDLOG_NAME_.c_str(),
                BUParamsCommon::IMM_ATTR_MAIN.c_str(), reladmpar.CLogRefs.Main, pt);

        ParseINIFunx::writePrivateProfile<int>(
                BUParamsCommon::IMM_CLASS_CMDLOG_NAME_.c_str(),
                BUParamsCommon::IMM_ATTR_PREPARED.c_str(), reladmpar.CLogRefs.Prepared, pt);

        //####################### Write LastAutoReload ################################
        //#############################################################################
        ParseINIFunx::writePrivateProfile<int>(
                BUParamsCommon::IMM_CLASS_LAR_NAME_.c_str(),
                BUParamsCommon::IMM_ATTR_LAR_CLH.c_str(), reladmpar.LastAutoReload.ManAuto, pt);

        ParseINIFunx::writePrivateProfile<int>(
                BUParamsCommon::IMM_CLASS_LAR_NAME_.c_str(),
                BUParamsCommon::IMM_ATTR_GFILE.c_str(), reladmpar.LastAutoReload.LRG, pt);

        ParseINIFunx::writePrivateProfile<int>(
                BUParamsCommon::IMM_CLASS_LAR_NAME_.c_str(),
                BUParamsCommon::IMM_ATTR_LPGF.c_str(), reladmpar.LastAutoReload.LRGF, pt);

        ParseINIFunx::writePrivateProfile<int>(
                BUParamsCommon::IMM_CLASS_LAR_NAME_.c_str(),
                BUParamsCommon::IMM_ATTR_LAR_NCT.c_str(), reladmpar.LastAutoReload.NCT, pt);

        ParseINIFunx::writePrivateProfile<int>(
                BUParamsCommon::IMM_CLASS_LAR_NAME_.c_str(),
                BUParamsCommon::IMM_ATTR_LAR_SUP.c_str(), reladmpar.LastAutoReload.SUP*10, pt);

        ParseINIFunx::writePrivateProfile<int>(
                BUParamsCommon::IMM_CLASS_LAR_NAME_.c_str(),
                BUParamsCommon::IMM_ATTR_LAR_OMISSION.c_str(), reladmpar.LastAutoReload.Omission, pt);

        ParseINIFunx::writePrivateProfile<int>(
                BUParamsCommon::IMM_CLASS_LAR_NAME_.c_str(),
                BUParamsCommon::IMM_ATTR_ALOG.c_str(), reladmpar.LastAutoReload.AssClogNum, pt);

        memset(buf, ' ', sizeof(buf));
        // Store the time supervision.
        sprintf(buf, "%02d%02d-%02d-%02d %02d:%02d", reladmpar.LastAutoReload.OPSRS.century,
                reladmpar.LastAutoReload.OPSRS.year,
                reladmpar.LastAutoReload.OPSRS.month,
                reladmpar.LastAutoReload.OPSRS.day,
                reladmpar.LastAutoReload.OPSRS.hour,
                reladmpar.LastAutoReload.OPSRS.minute);

        ParseINIFunx::writePrivateProfile<char *>(BUParamsCommon::IMM_CLASS_LAR_NAME_.c_str(),
                    BUParamsCommon::IMM_ATTR_OPSRS.c_str(), buf, pt);

        memset(buf, ' ', sizeof(buf));
        // Store the time supervision.
        sprintf(buf, "%02d%02d-%02d-%02d %02d:%02d", reladmpar.LastAutoReload.ODSS.century,
                reladmpar.LastAutoReload.ODSS.year,
                reladmpar.LastAutoReload.ODSS.month,
                reladmpar.LastAutoReload.ODSS.day,
                reladmpar.LastAutoReload.ODSS.hour,
                reladmpar.LastAutoReload.ODSS.minute);

        ParseINIFunx::writePrivateProfile<char *>(BUParamsCommon::IMM_CLASS_LAR_NAME_.c_str(),
                    BUParamsCommon::IMM_ATTR_ODSS.c_str(), buf, pt);

        memset(buf, ' ', sizeof(buf));
        // Store the time supervision.
        sprintf(buf, "%02d%02d-%02d-%02d %02d:%02d", reladmpar.LastAutoReload.ODSL.century,
                reladmpar.LastAutoReload.ODSL.year,
                reladmpar.LastAutoReload.ODSL.month,
                reladmpar.LastAutoReload.ODSL.day,
                reladmpar.LastAutoReload.ODSL.hour,
                reladmpar.LastAutoReload.ODSL.minute);

        ParseINIFunx::writePrivateProfile<char *>(BUParamsCommon::IMM_CLASS_LAR_NAME_.c_str(),
                    BUParamsCommon::IMM_ATTR_ODSL.c_str(), buf, pt);

        //####################### Write DelayedMeasures ###############################
        //#############################################################################
        ParseINIFunx::writePrivateProfile<int>(
                BUParamsCommon::IMM_CLASS_DM_NAME_.c_str(),
                BUParamsCommon::IMM_ATTR_DM1.c_str(), reladmpar.DelayedMeasures.Measures[0], pt);

        ParseINIFunx::writePrivateProfile<int>(
                BUParamsCommon::IMM_CLASS_DM_NAME_.c_str(),
                BUParamsCommon::IMM_ATTR_DM2.c_str(), reladmpar.DelayedMeasures.Measures[1], pt);

        ParseINIFunx::writePrivateProfile<int>(
                BUParamsCommon::IMM_CLASS_DM_NAME_.c_str(),
                BUParamsCommon::IMM_ATTR_DM3.c_str(), reladmpar.DelayedMeasures.Measures[2], pt);

        ParseINIFunx::writePrivateProfile<int>(
                BUParamsCommon::IMM_CLASS_DM_NAME_.c_str(),
                BUParamsCommon::IMM_ATTR_DM4.c_str(), reladmpar.DelayedMeasures.Measures[3], pt);

        ParseINIFunx::writePrivateProfile<int>(
                BUParamsCommon::IMM_CLASS_DM_NAME_.c_str(),
                BUParamsCommon::IMM_ATTR_DM5.c_str(), reladmpar.DelayedMeasures.Measures[4], pt);

        ParseINIFunx::writePrivateProfile<int>(
                BUParamsCommon::IMM_CLASS_DM_NAME_.c_str(),
                BUParamsCommon::IMM_ATTR_DM6.c_str(), reladmpar.DelayedMeasures.Measures[5], pt);

        ParseINIFunx::writePrivateProfile<int>(
                BUParamsCommon::IMM_CLASS_DM_NAME_.c_str(),
                BUParamsCommon::IMM_ATTR_DM7.c_str(), reladmpar.DelayedMeasures.Measures[6], pt);

        ParseINIFunx::writePrivateProfile<int>(
                BUParamsCommon::IMM_CLASS_DM_NAME_.c_str(),
                BUParamsCommon::IMM_ATTR_DM8.c_str(), reladmpar.DelayedMeasures.Measures[7], pt);

        // Write to ini file
        write_ini(fileName, pt);
    }
    catch(...)
    {
        TRACE((LOG_LEVEL_WARN, "CPS_BUAP_RPF::writeReladmpar() returns false ", 0));
        return false;
    }

    TRACE((LOG_LEVEL_INFO, "CPS_BUAP_RPF::writeReladmpar() returns true ", 0));
    return true;
}


