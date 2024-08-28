/*
NAME
   File_name:VerifySBCDataJob.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2002-2023. All rights reserved.

   The Copyright to the computer program(s) herein is the property of Ericsson
   Utvecklings AB, Sweden.
   The program(s) may be used and/or copied only with the written permission from
   Ericsson Utvecklings AB or in accordance with the terms and conditions
   stipulated in the agreement/contract under which the program(s) have been
   supplied.

DESCRIPTION
   Executes the BUP command VERIFY_SBC_DATA.

DOCUMENT NO
   190 89-CAA 109 0387

AUTHOR
   2002-02-05 by UAB/KY/SK Anders Gillgren (qabgill, tag :ag)

SEE ALSO
   

Revision history
----------------
2002-02-05 qabgill Created
2002-11-04 uablan  Add ACS_Trace
2023-01-01 xkomala Added checks for valid cpid for TR IA20800

*/


#include "Config.h"
#include "VerifySBCDataJob.h"

#include "APBackupInfo.h"
#include "BUPFunx.h"
#include "CodeException.h"
#include "CPS_BUSRV_Trace.h"
#include "EventReporter.h"
#include "TmpCurrDir.h"
#include "LinuxException.h"
//#include "ACS_CS_API.h"

#ifndef LOCAL_BUILD
#include "EnvFMS.h"
#include "fms_cpf_file.h"
#endif

//
//
//====================================================================
void VerifySBCDataJob::execute() {
    
    newTRACE((LOG_LEVEL_INFO, "VerifySBCDataJob::execute()", 0));

    // local variables used in response
    BUPMsg::SBC_DATA_STATUS result[5] = {
        BUPMsg::SDS_NO_ERROR,
        BUPMsg::SDS_NO_ERROR,
        BUPMsg::SDS_NO_ERROR,
        BUPMsg::SDS_NO_ERROR,
        BUPMsg::SDS_NO_ERROR
    };
    string sbcDir;
    CPID cpId = 0xFFFF; // default ONE CP system
    const BUPMsg::VerifySBCDataMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::VerifySBCDataMsg>(msg());
    u_int32 transNum = cmd->transNum();
    u_int16 tvm = cmd->version().major();

    try {
        // init stuff
        //===================================================================
        if(verifyVersion()) {
            //const BUPMsg::VerifySBCDataMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::VerifySBCDataMsg>(msg());
            //transNum = cmd->transNum();

            //check if ONE CP System
        	if (cmd->cpSystem() != BUPMsg::ONE_CP_SYSTEM)
        	{
        		cpId = cmd->cpId();

        		//TR_IA20800 START
        		if(!(Config::instance().isValidSpxCpid(cpId)))
        		{
        			TRACE((LOG_LEVEL_ERROR, "VerifySBCDataJob, CpId received is <%d> ,throwing internal error", 0, cpId));
        			throw BUPMsg::INTERNAL_ERROR;
        		} //TR_IA20800 END
        	}

            //
            //
            //==================================================================
            SBCId sbcId = cmd->sbcId();    
            if(!sbcId.valid())
                throw BUPMsg::PARAM_BACKUPID_OUT_OF_RANGE;

            TRACE((LOG_LEVEL_INFO, "sddCheck   ... = %d", 0, cmd->sddCheck()));
            TRACE((LOG_LEVEL_INFO, "ldd1Check  ... = %d", 0, cmd->ldd1Check()));
            TRACE((LOG_LEVEL_INFO, "ldd2Check  ... = %d", 0, cmd->ldd2Check()));
            TRACE((LOG_LEVEL_INFO, "psCheck    ... = %d", 0, cmd->psCheck()));
            TRACE((LOG_LEVEL_INFO, "rsCheck    ... = %d", 0, cmd->rsCheck()));


            // 1. execute command, but only if we have something to check
            //===================================================================
            if(cmd->sddCheck() || cmd->ldd1Check() || cmd->ldd2Check() ||
                cmd->psCheck() || cmd->rsCheck()) {
                
                // set dir
                sbcDir = Config::instance().sbcDir(sbcId, cpId);
                
#ifdef LOCAL_BUILD                    
                if(!FileFunx::dirExists(sbcDir.c_str())) {
#else
                if (!EnvFMS::dirExists(sbcId, cpId)) {
#endif
                    TRACE((LOG_LEVEL_ERROR, "SBC does not exist; throwing an exception", 0));
                    throw BUPMsg::SBC_NOT_FOUND;
                }

                TmpCurrDir currDir(sbcDir);

                // read buinfo
                APBackupInfo buinfo("./");
                if(buinfo.calcCSum() != buinfo.info()->checksum)
                    throw BUPMsg::BAD_BACKUPINFO;
                
                // Check files
                TRACE((LOG_LEVEL_INFO, "check files <%s>", 0, sbcDir.c_str()));

            bool adFileMarker = EnvFMS::fileExists(sbcId, Config::instance().MarkerFileName, cpId);
            bool admarker = (bool)buinfo.getAdMarker(); 
            TRACE((LOG_LEVEL_INFO, "BUINFO marker = <%d>, file ADMARKER = <%d>", 0, admarker, adFileMarker));

            if (admarker != adFileMarker) {
                    // If only one of the markers exists, information from BUINFO cannot be used
                    if (EnvFMS::fileExists(sbcId, SBCFile::name(SBCFile::SDD), cpId))
                        result[0] = BUPMsg::SDS_UNKNOWN_ERROR;
                    if (EnvFMS::fileExists(sbcId, SBCFile::name(SBCFile::LDD1), cpId))
                        result[1] = BUPMsg::SDS_UNKNOWN_ERROR;
                    if (EnvFMS::fileExists(sbcId, SBCFile::name(SBCFile::LDD2), cpId))
                        result[2] = BUPMsg::SDS_UNKNOWN_ERROR;
                    if (EnvFMS::fileExists(sbcId, SBCFile::name(SBCFile::PS), cpId))
                        result[3] = BUPMsg::SDS_UNKNOWN_ERROR;
                    if (EnvFMS::fileExists(sbcId, SBCFile::name(SBCFile::RS), cpId))
                        result[4] = BUPMsg::SDS_UNKNOWN_ERROR;
                    
                    throw BUPMsg::OK;

            }

                // add check for buinfo version?
                // check files
                if(cmd->sddCheck() && !isAborted())
                {    // If there is any inconsistency between BUINFO and physical files, then result
                     if ((buinfo.info()->sddSectorInfo.nofSegments() > 0) !=
                         EnvFMS::fileExists(sbcId, SBCFile::name(SBCFile::SDD), cpId)) {
                         TRACE((LOG_LEVEL_INFO, "BUINFO inconsistent with SDD physical file", 0));
                         result[0] = BUPMsg::SDS_UNKNOWN_ERROR;
                     }
                     else {
                        result[0] = verifyDataFile(SBCFile::name(SBCFile::SDD), buinfo.sectorSizeW8s(SBCFile::SDD),
                                                        buinfo.info()->sddSectorInfo, buinfo.sddHeader(), this);
                     }
                }
                if(cmd->ldd1Check() && !isAborted())
                {
                    if ((buinfo.info()->ldd1SectorInfo.nofSegments() > 0) !=
                        EnvFMS::fileExists(sbcId, SBCFile::name(SBCFile::LDD1), cpId)) {
                        TRACE((LOG_LEVEL_INFO, "BUINFO inconsistent with LDD1 physical file", 0));
                        result[1] = BUPMsg::SDS_UNKNOWN_ERROR;

                    }
                    else {

                        result[1] = verifyDataFile(SBCFile::name(SBCFile::LDD1), buinfo.sectorSizeW8s(SBCFile::LDD1),
                                                   buinfo.info()->ldd1SectorInfo, buinfo.ldd1Header(), this);
                    }
                }
                if(cmd->ldd2Check() && !isAborted())
                {
                    if ((buinfo.info()->ldd2SectorInfo.nofSegments() > 0) !=
                        EnvFMS::fileExists(sbcId, SBCFile::name(SBCFile::LDD2), cpId)) {
                        TRACE((LOG_LEVEL_INFO, "BUINFO inconsistent with LDD2 physical file", 0));
                        result[2] = BUPMsg::SDS_UNKNOWN_ERROR;
                    }
                    else {
                        result[2] = verifyDataFile(SBCFile::name(SBCFile::LDD2), buinfo.sectorSizeW8s(SBCFile::LDD2),
                                                buinfo.info()->ldd2SectorInfo, buinfo.ldd2Header(), this);
                    }

                }
                if(cmd->psCheck() && !isAborted())
                {
                     if ((buinfo.info()->psSectorInfo.nofSegments() > 0) !=
                        EnvFMS::fileExists(sbcId, SBCFile::name(SBCFile::PS), cpId)) {
                        TRACE((LOG_LEVEL_INFO, "BUINFO inconsistent with PS physical file", 0));
                        result[3] = BUPMsg::SDS_UNKNOWN_ERROR;
                     }
                     else {
                         result[3] = verifyDataFile(SBCFile::name(SBCFile::PS), buinfo.sectorSizeW8s(SBCFile::PS),
                                                    buinfo.info()->psSectorInfo, buinfo.psHeader(), this);
                     }
                }
                if(cmd->rsCheck() && !isAborted())
                {
                    if ((buinfo.info()->rsSectorInfo.nofSegments() > 0) !=
                        EnvFMS::fileExists(sbcId, SBCFile::name(SBCFile::RS), cpId)) {
                        TRACE((LOG_LEVEL_INFO, "BUINFO inconsistent with RS physical file", 0));
                        result[4] = BUPMsg::SDS_UNKNOWN_ERROR;;
                    }
                    else {
                        result[4] = verifyDataFile(SBCFile::name(SBCFile::RS), buinfo.sectorSizeW8s(SBCFile::RS),
                                                buinfo.info()->rsSectorInfo, buinfo.rsHeader(), this);
                    }

                }
        
            }    
        }
    }
    //
    // catch all error & set exitcode
    //
    catch(BUPMsg::ERROR_CODE exitcode) {
        m_exitcode = exitcode;
    }
#ifndef LOCAL_BUILD
    catch(FMS_CPF_Exception& x) {
        stringstream str;
        str << "VerifySBCDataJob, FMS Errorcode : " << x.errorCode() << ", Text: " << x.errorText() << ", Details: "
            << x.detailInfo() << endl;
        newTRACE((LOG_LEVEL_ERROR, "%s", 0, str.str().c_str()));
        switch(x.errorCode()) {
            case FMS_CPF_Exception::PHYSICALERROR:
                TRACE((LOG_LEVEL_ERROR, "VerifySBCDataJob, mapping FMS_CPF_Exception::PHYSICALERROR to BUPMsg::FMS_PHYSICAL_FILE_ERROR", 0));
                m_exitcode = BUPMsg::FMS_PHYSICAL_FILE_ERROR;
                break;
            default:
                m_exitcode = BUPMsg::INTERNAL_ERROR;
                break;
        }
        EventReporter::instance().write(str.str().c_str());    
    }
#endif    
    catch(const LinuxException& x) {
        TRACE((LOG_LEVEL_ERROR, "Unhandled LinuxException", 0));
        EventReporter::instance().write(x);
        m_exitcode = BUPMsg::INTERNAL_ERROR;        
    }
    catch(CodeException& x) {
        switch(x.errcode()) {
            case CodeException::BUINFO_MISSING: // fall thru
            case CodeException::REQ_SBC_MISSING:
            case CodeException::BUINFO_ACCESS_DENIED:
            case CodeException::BAD_BACKUPINFO:
                m_exitcode = BUPMsg::BAD_BACKUPINFO;
                break;
            case CodeException::CP_APG43_CONFIG_ERROR:
                TRACE((LOG_LEVEL_ERROR, "VerifySBCDataJob::execute() CS error ", 0));
                m_exitcode = BUPMsg::INTERNAL_ERROR;
                break;
            default:
                TRACE((LOG_LEVEL_ERROR, "Unhandled CodeException", 0));
                EventReporter::instance().write("Unhandled internal error.");
                m_exitcode = BUPMsg::INTERNAL_ERROR;
                break;
        }
    }
    catch(...) {
        EventReporter::instance().write("Unhandled internal error.");
        m_exitcode = BUPMsg::INTERNAL_ERROR;
    }
    if(isAborted())
        m_exitcode = BUPMsg::SHUTDOWN_PENDING;

    msg().reset();
    new (msg().addr()) BUPMsg::VerifySBCDataRspMsg(transNum,
        m_exitcode, result[0], result[1], result[2], result[3], result[4]);

    TRACE((LOG_LEVEL_INFO, "result[0] sdd  ... = %d", 0, result[0]));
    TRACE((LOG_LEVEL_INFO, "result[1] ldd1 ... = %d", 0, result[1]));
    TRACE((LOG_LEVEL_INFO, "result[2] ldd2 ... = %d", 0, result[2]));
    TRACE((LOG_LEVEL_INFO, "result[3] ps   ... = %d", 0, result[3]));
    TRACE((LOG_LEVEL_INFO, "result[4] rs   ... = %d", 0, result[4]));

    TRACE((LOG_LEVEL_INFO, "VerifySBCDataRspMsg: TransNum = %d exitcode = %d Version Major = %d", 0, transNum, exitcode(), tvm));

}


///////////////////////////////////////////////////////////////////////////////
//
// 
//====================================================================
void VerifySBCDataJob_V4::execute() {

    newTRACE((LOG_LEVEL_INFO, "VerifySBCDataJob_V4::execute()", 0));

    // local variables used in response
    BUPMsg::SBC_DATA_STATUS result[7] = {
        BUPMsg::SDS_NO_ERROR,
        BUPMsg::SDS_NO_ERROR,
        BUPMsg::SDS_NO_ERROR,
        BUPMsg::SDS_NO_ERROR,
        BUPMsg::SDS_NO_ERROR,
        BUPMsg::SDS_NO_ERROR,
        BUPMsg::SDS_NO_ERROR
    };
    string sbcDir;
    CPID cpId = 0xFFFF; // default ONE CP system
    const BUPMsg::VerifySBCDataMsg2* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::VerifySBCDataMsg2>(msg());
    u_int32 transNum = cmd->transNum();
    u_int16 tvm = cmd->version().major();

    try {
        // init stuff
        //===================================================================
        if(verifyVersion()) {

            //check    if ONE CP System
        	if (cmd->cpSystem()    != BUPMsg::ONE_CP_SYSTEM)
        	{
        		cpId = cmd->cpId();

        		//TR_IA20800 START
        		if(!(Config::instance().isValidBladeCpid(cpId)))
        		{
        			TRACE((LOG_LEVEL_ERROR, "VerifySBCDataJob, CpId received is <%d> ,throwing internal error", 0, cpId));
        			throw BUPMsg::INTERNAL_ERROR;
        		} //TR_IA20800 END
        	}

            //
            //
            //==================================================================
            if(!cmd->sbcId().valid())
                throw BUPMsg::PARAM_BACKUPID_OUT_OF_RANGE;

            // 1. execute command, but only    if we have something to    check
            //===================================================================
            if(cmd->sddCheck() || cmd->ldd1Check() || cmd->ldd2Check() ||
                cmd->psCheck() || cmd->rsCheck() ||
                cmd->ps2Check() || cmd->rs2Check()) {

                TRACE((LOG_LEVEL_INFO, "cmd->sddCheck()  ... = %d", 0, cmd->sddCheck()));
                TRACE((LOG_LEVEL_INFO, "cmd->ldd1Check() ... = %d", 0, cmd->ldd1Check()));
                TRACE((LOG_LEVEL_INFO, "cmd->ldd2Check() ... = %d", 0, cmd->ldd2Check()));
                TRACE((LOG_LEVEL_INFO, "cmd->psCheck()   ... = %d", 0, cmd->psCheck()));
                TRACE((LOG_LEVEL_INFO, "cmd->rsCheck()   ... = %d", 0, cmd->rsCheck()));
                TRACE((LOG_LEVEL_INFO, "cmd->ps2Check()  ... = %d", 0, cmd->ps2Check()));
                TRACE((LOG_LEVEL_INFO, "cmd->rs2Check()  ... = %d", 0, cmd->rs2Check()));

                // set dir
                //sbcDir = Config::instance().sbcDir(cmd->sbcId(), cpId);
                Config::CLUSTER tc;
                sbcDir = Config::instance().sbcDir(cmd->sbcId(), tc);
#ifdef LOCAL_BUILD
                if(!FileFunx::dirExists(Config::instance().sbcDir(cmd->sbcId(), tc).c_str()))
#else
                if (!EnvFMS::dirExists(cmd->sbcId(), tc))
#endif
                    throw BUPMsg::SBC_NOT_FOUND;
                TmpCurrDir currDir(sbcDir);

                // read    buinfo
                APBackupInfo buinfo("./");

                // Use BUP version for the checksum, this will ensure BUP version
                // and BUINFO format version matches
                if(buinfo.calcCSum() != buinfo.info()->checksum)
                    throw BUPMsg::BAD_BACKUPINFO;
                
                // check files
                TRACE((LOG_LEVEL_INFO, "check files >%s<", 0, sbcDir.c_str()))      //mnst
                
                //  Handling of failed auto dump
                bool adFileMarker = EnvFMS::fileExists(cmd->sbcId(), Config::instance().MarkerFileName, tc);
                bool admarker = (bool)buinfo.getAdMarker();

                TRACE((LOG_LEVEL_INFO, "BUINFO marker = <%d>, file ADMARKER = <%d>", 0, admarker, adFileMarker));

                // Check if none of the Auto Dump Marker exist or both of the markers exist
                if (adFileMarker != admarker) {
                    // If only one of the marker exists, information from BUINFO cannot be used

                    if (EnvFMS::fileExists(cmd->sbcId(), SBCFile::name(SBCFile::SDD), tc))
                        result[0] = BUPMsg::SDS_UNKNOWN_ERROR;
                    if (EnvFMS::fileExists(cmd->sbcId(), SBCFile::name(SBCFile::LDD1), tc))
                        result[1] = BUPMsg::SDS_UNKNOWN_ERROR;
                    if (EnvFMS::fileExists(cmd->sbcId(), SBCFile::name(SBCFile::LDD2), tc))
                        result[2] = BUPMsg::SDS_UNKNOWN_ERROR;
                    if (EnvFMS::fileExists(cmd->sbcId(), SBCFile::name(SBCFile::PS), tc))
                        result[3] = BUPMsg::SDS_UNKNOWN_ERROR;
                    if (EnvFMS::fileExists(cmd->sbcId(), SBCFile::name(SBCFile::RS), tc))
                        result[4] = BUPMsg::SDS_UNKNOWN_ERROR;
                    if (EnvFMS::fileExists(cmd->sbcId(), SBCFile::name(SBCFile::PS2), tc))
                        result[5] = BUPMsg::SDS_UNKNOWN_ERROR;
                    if (EnvFMS::fileExists(cmd->sbcId(), SBCFile::name(SBCFile::RS2), tc))
                        result[6] = BUPMsg::SDS_UNKNOWN_ERROR;

                    throw BUPMsg::OK;


                }

                SBCId sbcId = cmd->sbcId();
                BackupInfo_Base::Info_V4 *pinfo = buinfo.info_V4();
                if(cmd->sddCheck() && !isAborted())
                {
                    if ((pinfo->sddSectorInfo.nofSegments() > 0) !=
                         EnvFMS::fileExists(sbcId, SBCFile::name(SBCFile::SDD), tc)) {
                         TRACE((LOG_LEVEL_INFO, "BUINFO inconsistent with SDD physical file", 0));
                         result[0] = BUPMsg::SDS_UNKNOWN_ERROR;
                    }
                    else {
                        result[0] = verifyDataFile(SBCFile::name(SBCFile::SDD),    buinfo.sectorSizeW8s(SBCFile::SDD),
                                                       pinfo->sddSectorInfo, buinfo.sddHeader(), this);
                    }
                }    
                if(cmd->ldd1Check() && !isAborted())
                {
                    if ((pinfo->ldd1SectorInfo.nofSegments() > 0) !=
                         EnvFMS::fileExists(sbcId, SBCFile::name(SBCFile::LDD1), tc)) {
                         TRACE((LOG_LEVEL_INFO, "BUINFO inconsistent with LDD1 physical file", 0));
                         result[1] = BUPMsg::SDS_UNKNOWN_ERROR;
                    }
                    else {
                         result[1] =    verifyDataFile(SBCFile::name(SBCFile::LDD1), buinfo.sectorSizeW8s(SBCFile::LDD1),
                                                         pinfo->ldd1SectorInfo, buinfo.ldd1Header(), this);
                    }
                }
                if(cmd->ldd2Check() && !isAborted())
                {
                    if ((pinfo->ldd2SectorInfo.nofSegments() > 0) !=
                         EnvFMS::fileExists(sbcId, SBCFile::name(SBCFile::LDD2), tc)) {
                         TRACE((LOG_LEVEL_INFO, "BUINFO inconsistent with LDD2 physical file", 0));
                         result[2] = BUPMsg::SDS_UNKNOWN_ERROR;
                    }
                    else {
                        result[2] =    verifyDataFile(SBCFile::name(SBCFile::LDD2), buinfo.sectorSizeW8s(SBCFile::LDD2),
                        pinfo->ldd2SectorInfo, buinfo.ldd2Header(), this);
                    }
                }
                if(cmd->psCheck() && !isAborted())
                {
                    if ((pinfo->psSectorInfo.nofSegments() > 0) !=
                         EnvFMS::fileExists(sbcId, SBCFile::name(SBCFile::PS), tc)) {
                         TRACE((LOG_LEVEL_INFO, "BUINFO inconsistent with PS physical file", 0));
                         result[3] = BUPMsg::SDS_UNKNOWN_ERROR;
                    }
                    else {
                         result[3] =    verifyDataFile(SBCFile::name(SBCFile::PS), buinfo.sectorSizeW8s(SBCFile::PS),
                                                      pinfo->psSectorInfo, buinfo.psHeader(), this);
                    }
                }
                if(cmd->rsCheck() && !isAborted())
                {
                    if ((pinfo->rsSectorInfo.nofSegments() > 0) !=
                         EnvFMS::fileExists(sbcId, SBCFile::name(SBCFile::RS), tc)) {
                         TRACE((LOG_LEVEL_INFO, "BUINFO inconsistent with RS physical file", 0));
                         result[4] = BUPMsg::SDS_UNKNOWN_ERROR;
                    }
                    else {
                         result[4] =    verifyDataFile(SBCFile::name(SBCFile::RS), buinfo.sectorSizeW8s(SBCFile::RS),
                                                       pinfo->rsSectorInfo, buinfo.rsHeader(), this);
                    }
                }
                if(cmd->ps2Check() && !isAborted())
                {
                    if ((pinfo->ps2SectorInfo.nofSegments() > 0) !=
                         EnvFMS::fileExists(sbcId, SBCFile::name(SBCFile::PS2), tc)) {
                         TRACE((LOG_LEVEL_INFO, "BUINFO inconsistent with PS2 physical file", 0));
                         result[5] = BUPMsg::SDS_UNKNOWN_ERROR;
                    }
                    else {
                         result[5] =    verifyDataFile(SBCFile::name(SBCFile::PS2), buinfo.sectorSizeW8s(SBCFile::PS2),
                                                       pinfo->ps2SectorInfo, buinfo.ps2Header(), this);
                    }
                }
                if(cmd->rs2Check() && !isAborted())
                {
                    if ((pinfo->rs2SectorInfo.nofSegments() > 0) !=
                         EnvFMS::fileExists(sbcId, SBCFile::name(SBCFile::RS2), tc)) {
                         TRACE((LOG_LEVEL_INFO, "BUINFO inconsistent with RS2 physical file", 0));
                         result[6] = BUPMsg::SDS_UNKNOWN_ERROR;
                    }
                    else {
                         result[6] =    verifyDataFile(SBCFile::name(SBCFile::RS2), buinfo.sectorSizeW8s(SBCFile::RS2),
                                                        pinfo->rs2SectorInfo, buinfo.rs2Header(), this);
                    }

                }
            }
        }
    }
    //
    // catch all error & set exitcode
    //
    catch(BUPMsg::ERROR_CODE exitcode) {
        m_exitcode = exitcode;
    }
#ifndef LOCAL_BUILD
    catch(FMS_CPF_Exception& x) {
        stringstream str;
        str << "VerifySBCDataJob_V4, FMS Errorcode : " << x.errorCode() << ", Text: " << x.errorText() << ", Details: "
            << x.detailInfo() << endl;
        newTRACE((LOG_LEVEL_ERROR, "%s", 0, str.str().c_str()));
        switch(x.errorCode()) {
            case FMS_CPF_Exception::PHYSICALERROR:
                TRACE((LOG_LEVEL_ERROR, "VerifySBCDataJob_V4, mapping FMS_CPF_Exception::PHYSICALERROR to BUPMsg::FMS_PHYSICAL_FILE_ERROR", 0));
                m_exitcode = BUPMsg::FMS_PHYSICAL_FILE_ERROR;
                break;
            default:
                m_exitcode = BUPMsg::INTERNAL_ERROR;
                break;
        }
        EventReporter::instance().write(str.str().c_str());
    }
#endif
    catch(const LinuxException& x) {
        TRACE((LOG_LEVEL_ERROR, "Unhandled Win32Exception", 0));
        EventReporter::instance().write(x);
        m_exitcode = BUPMsg::INTERNAL_ERROR;        
    }
    catch(CodeException& x) {
        switch(x.errcode()) {
            case CodeException::BUINFO_MISSING: // fall thru
            case CodeException::REQ_SBC_MISSING:
            case CodeException::BUINFO_ACCESS_DENIED:
            case CodeException::BAD_BACKUPINFO:
            case CodeException::CHECKSUM_UNKNOWN_ERROR:
                m_exitcode = BUPMsg::BAD_BACKUPINFO;
                break;
            case CodeException::CP_APG43_CONFIG_ERROR:
                TRACE((LOG_LEVEL_ERROR, "VerifySBCDataJob::execute() CS error. ERR_CODE = %d ", 0, x.errcode()));
                m_exitcode = BUPMsg::INTERNAL_ERROR;
                break;
            default:
                TRACE((LOG_LEVEL_ERROR, "Unhandled CodeException", 0));
                EventReporter::instance().write("Unhandled internal error.");
                m_exitcode = BUPMsg::INTERNAL_ERROR;
                break;
        }
    }
    catch(...) {
        EventReporter::instance().write("Unhandled internal error.");
        m_exitcode = BUPMsg::INTERNAL_ERROR;
    }
    if(isAborted())
        m_exitcode = BUPMsg::SHUTDOWN_PENDING;

    
    msg().reset();
    new (msg().addr()) BUPMsg::VerifySBCDataRspMsg2(transNum,
        m_exitcode, result[0], result[1], result[2], result[3], result[4], result[5], result[6]);
    
    TRACE((LOG_LEVEL_INFO, "result[0]  ... = %d", 0, result[0]));
    TRACE((LOG_LEVEL_INFO, "result[1]  ... = %d", 0, result[1]));
    TRACE((LOG_LEVEL_INFO, "result[2]  ... = %d", 0, result[2]));
    TRACE((LOG_LEVEL_INFO, "result[3]  ... = %d", 0, result[3]));
    TRACE((LOG_LEVEL_INFO, "result[4]  ... = %d", 0, result[4]));
    TRACE((LOG_LEVEL_INFO, "result[5]  ... = %d", 0, result[5]));
    TRACE((LOG_LEVEL_INFO, "result[6]  ... = %d", 0, result[6]));

    TRACE((LOG_LEVEL_INFO, "VerifySBCDataRspMsg2: TransNum = %d exitcode = %d Version Major = %d", 0, transNum, exitcode(), tvm));
}
