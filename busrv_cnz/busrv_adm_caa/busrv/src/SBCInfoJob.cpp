/*
NAME
   File_name:SBCInfoJob.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2002-2023. All rights reserved.

   The Copyright to the computer program(s) herein is the property of Ericsson
   Utvecklings AB, Sweden.
   The program(s) may be used and/or copied only with the written permission from
   Ericsson Utvecklings AB or in accordance with the terms and conditions
   stipulated in the agreement/contract under which the program(s) have been
   supplied.

DESCRIPTION
   Executes the BUP command SBC_INFO.

DOCUMENT NO
   190 89-CAA 109 0387

AUTHOR
   2002-02-05 by UAB/KY/SK Anders Gillgren (qabgill, tag :ag)

SEE ALSO
   

Revision history
----------------
2002-02-05 qabgill Created
2002-02-11 uablan  updated the class
2002-10-31 uablan  Added ACS_Trace
2007-03-05 uablan  Update code to handle multiple CP System
2011-09-14 xdtthng 43L2011-09-14 xdtthng 43L Total usage of FMS, Total usage of FMS
2012-12-24 xdtthng Updated for APG43L BC
2023-01-01 xkomala Added checks for valid cpid for TR IA20800
*/


#include "Config.h"
#include "SBCInfoJob.h"

#include "buap.h"
#include "BUPFunx.h"
#include "APBackupInfo.h"
#include "CodeException.h"
#include "CPS_BUSRV_Trace.h"
#include "EventReporter.h"
#include "FileFunx.h"
#include "LinuxException.h"
#include "DataFile.h"

#ifndef LOCAL_BUILD                    
#include "EnvFMS.h"
#include "fms_cpf_file.h"
#endif

#include <stdlib.h>


//
// execute
//====================================================================
void SBCInfoJob::execute() {
    
    newTRACE((LOG_LEVEL_INFO, "SBCInfoJob::execute()", 0));

    // reply data
    BackupInfo_Base::Info_Base info(false);
    CPID cpId = 0xFFFF; // default ONE CP system
    const BUPMsg::SBCInfoMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::SBCInfoMsg>(msg());
    u_int32 transNum = cmd->transNum(); // save for response
    u_int16 tvm = cmd->version().major();
    SBCId sbcId = cmd->sbcId();

    try {
        // init stuff
        //===================================================================
        if(verifyVersion()) { // if verify fails the errorcode will be set
            //const BUPMsg::SBCInfoMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::SBCInfoMsg>(msg());
            //transNum = cmd->transNum(); // save for response

            //check if ONE CP System
        	if (cmd->cpSystem() != BUPMsg::ONE_CP_SYSTEM)
        	{
        		cpId = cmd->cpId();
        		//TR_IA20800 START
        		if(!(Config::instance().isValidSpxCpid(cpId)))
        		{
        			TRACE((LOG_LEVEL_ERROR, "SBCInfoJob, CpId received is <%d> ,throwing internal error", 0, cpId));
        			throw BUPMsg::INTERNAL_ERROR;
        		} //TR_IA20800 END

        	}

                // check rules
            //===================================================================
            if(!cmd->sbcId().valid())
                throw BUPMsg::PARAM_BACKUPID_OUT_OF_RANGE;

            string backupDir = Config::instance().sbcDir(sbcId, cpId); 

#ifdef LOCAL_BUILD                    
            if(!FileFunx::dirExists(backupDir.c_str()))
                throw BUPMsg::SBC_NOT_FOUND;
#else
            if (!EnvFMS::dirExists(sbcId, cpId)) 
                throw BUPMsg::SBC_NOT_FOUND;                
#endif
            // map buinfo into the APBackupInfo object
            APBackupInfo buinfo(backupDir.c_str());

            bool adFileMarker = EnvFMS::fileExists(sbcId, Config::instance().MarkerFileName, cpId);
            bool admarker = (bool)buinfo.getAdMarker();

            TRACE((LOG_LEVEL_INFO, "BUINFO marker = <%d>, file ADMARKER = <%d>", 0, admarker, adFileMarker));

            // Check if none of the Auto Dump Marker exist or both of the markers exist
            if ((adFileMarker && admarker) || (!adFileMarker && !admarker)) {
                // BUP revision and and BUINFO format revision must match
                if (!(buinfo.info()->version == msg().requestHeader()->version()))
                    throw BUPMsg::BAD_BACKUPINFO;

                BackupInfo_Base::BI_ERROR_CODE ec = buinfo.valid();
                if(ec != BackupInfo_Base::BEC_NO_ERROR)
                    throw BUPMsg::BAD_BACKUPINFO;
                info = *buinfo.info(); // copy info part
            }
            else { // If only one of the markers exists, information from BUINFO cannot be used
                   THROW_XCODE("only one of the markers exists, information from BUINFO cannot be used",
                                                        CodeException::BAD_BACKUPINFO);
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
        str << "SBCInfoJob, FMS Errorcode : " << x.errorCode() << ", Text: " << x.errorText() << ", Details: "
            << x.detailInfo() << endl;
        newTRACE((LOG_LEVEL_ERROR, "%s", 0, str.str().c_str()));
        m_exitcode = BUPMsg::INTERNAL_ERROR;
        EventReporter::instance().write(str.str().c_str());    
    }
#endif    
    catch(const LinuxException& x) {
        (void) x;
        TRACE((LOG_LEVEL_ERROR, "Unhandled LinuxException, error code %d", 0, x.what().c_str()));
        EventReporter::instance().write("Unhandled internal error.");
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
                TRACE((LOG_LEVEL_ERROR, "SBCInfoJob::execute() CS error ", 0));
                m_exitcode = BUPMsg::INTERNAL_ERROR;
                break;
            default:
                TRACE((LOG_LEVEL_ERROR, "Unhandled CodeException, error code %d", 0,x.what().c_str()));
                EventReporter::instance().write("Unhandled internal error.");
                m_exitcode = BUPMsg::INTERNAL_ERROR;
                break;
        }
    }
    catch(...) {
        EventReporter::instance().write("Unhandled internal error.");
        m_exitcode = BUPMsg::INTERNAL_ERROR;
    }
    
    msg().reset();
        new (msg().addr()) BUPMsg::SBCInfoRspMsg(transNum,
            exitcode(), info.version, info.apzVersion,
            info.exchangeId, info.generation, info.sddSectorInfo,
            info.ldd1SectorInfo, info.ldd2SectorInfo, info.psSectorInfo, 
            info.rsSectorInfo, info.cmdLogFile1, info.cmdLogFile2);
        TRACE((LOG_LEVEL_INFO, "SBCInfoRspMsg TransNum = %d exitcode = %d Version Major = %d", 0, transNum, exitcode(), tvm));
    

    if(!exitcode()) {
        TRACE((LOG_LEVEL_INFO, "info.cmdLogFile1: %d, info.cmdLogFile2: %d", 0, info.cmdLogFile1,info.cmdLogFile2));
        TRACE((LOG_LEVEL_INFO, "APZVersion type : %d, version : %d, revision :%d, prodNumInfo : %d", 0, info.apzVersion.type(),
            info.apzVersion.version(), info.apzVersion.revision(), info.apzVersion.prodNumInfo()));

    }
}


//////////////////////////////////////////////////////////////////////
//
// execute
//====================================================================
void SBCInfoJob_V4::execute() {
    
    newTRACE((LOG_LEVEL_INFO, "SBCInfoJob_V4::execute()", 0));

    //u_int16 APZProfile = Config::instance().getAPZProfile();
    //TRACE(("LoadBeginJob_V4::execute(); APZ Profile: %d", 0, APZProfile));

    // reply data
    BackupInfo_V4::Info_V4 info_v4(false);
    BackupInfo_Base::Info info_v3(false);
    const BUPMsg::SBCInfoMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::SBCInfoMsg>(msg());
    u_int32 transNum = cmd->transNum(); 
    u_int16 tvm = cmd->version().major();
    Version buVersion;
    Version BUINFO_V4(4, 0);
    Version BUINFO_V3(3, 0);

    try {
        // init stuff
        //===================================================================
        if(verifyVersion()) { // if verify fails the errorcode will be set
            //const BUPMsg::SBCInfoMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::SBCInfoMsg>(msg());
            //transNum = cmd->transNum(); // save for response

            //check if ONE CP System
            //if (cmd->cpSystem() != BUPMsg::ONE_CP_SYSTEM)
            //    cpId = cmd->cpId();

                // check rules
            //===================================================================
            if(!cmd->sbcId().valid())
                throw BUPMsg::PARAM_BACKUPID_OUT_OF_RANGE;

            Config::CLUSTER t;
            string backupDir = Config::instance().sbcDir(cmd->sbcId(), t); 
            
#ifdef LOCAL_BUILD                    
            if(!FileFunx::dirExists(backupDir.c_str()))
#else
            if (!EnvFMS::dirExists(cmd->sbcId(), t)) 
#endif
                throw BUPMsg::SBC_NOT_FOUND;    
                            
            // map buinfo into the APBackupInfo object
            APBackupInfo buinfo(backupDir.c_str());

            // Handling of failed auto dump
            bool adFileMarker = EnvFMS::fileExists(cmd->sbcId(), Config::instance().MarkerFileName, t);
            bool admarker = (bool)buinfo.getAdMarker();

            TRACE((LOG_LEVEL_INFO, "BUINFO marker = <%d>, file ADMARKER = <%d>", 0, admarker, adFileMarker));

            // Check if none of the Auto Dump Marker exist or both of the markers exist
            if ((adFileMarker && admarker) || (!adFileMarker && !admarker)) {
                buVersion = buinfo.info()->version;

                // UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU
                // Tool Support
                //
                // Should be able to process BUINFO V3 or BUINFO V4 using BUP V4
                // The new check is below
                //
                if (buVersion != BUINFO_V4 && buVersion != BUINFO_V3)
                    throw BUPMsg::BAD_BACKUPINFO;

                BackupInfo_Base::BI_ERROR_CODE ec = buinfo.valid();
                if(ec != BackupInfo_V4::BEC_NO_ERROR)
                    throw BUPMsg::BAD_BACKUPINFO;

                if (buVersion >= BUINFO_V4) {
                    info_v4 = *buinfo.info_V4(); 
                }
                else {
                    info_v3 = *buinfo.info(); 
                }
            }
            else { // If only one of the markers exists, information from BUINFO cannot be used
                   THROW_XCODE("only one of the markers exists, information from BUINFO cannot be used", 
                       					CodeException::BAD_BACKUPINFO);
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
        str << "SBCInfoJob_V4, FMS Errorcode : " << x.errorCode() << ", Text: " << x.errorText() << ", Details: "
            << x.detailInfo() << endl;
        newTRACE((LOG_LEVEL_ERROR, "%s", 0, str.str().c_str()));
        m_exitcode = BUPMsg::INTERNAL_ERROR;
        
        // Remove this logging for final release
        EventReporter::instance().write(str.str().c_str());        
    }
#endif
    catch(const LinuxException& x) {
        (void) x;
        TRACE((LOG_LEVEL_ERROR, "Unhandled LinuxException, error code %d", 0, x.what().c_str()));
        EventReporter::instance().write("Unhandled internal error.");
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
                TRACE((LOG_LEVEL_ERROR, "SBCInfoJob::execute() CS error ", 0));
                m_exitcode = BUPMsg::INTERNAL_ERROR;
                break;
            default:
                TRACE((LOG_LEVEL_ERROR, "Unhandled CodeException, error code %d", 0,x.what().c_str()));
                EventReporter::instance().write("Unhandled internal error.");
                m_exitcode = BUPMsg::INTERNAL_ERROR;
                break;
        }
    }
    catch(...) {
        EventReporter::instance().write("Unhandled internal error.");
        m_exitcode = BUPMsg::INTERNAL_ERROR;
    }
    
    // UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU
    // Tool Support
    // Allow loading of BUINFO Version 3 and Version 4 Under APZ Profile 100
    //
    // Under APZ Profile == 100, BUP expects BUINFO Version 4 under normal operating
    // conditions. This enhancement allows old tools using BUINFO Version 3 to be 
    // loaded under APZ Profile == 100
    
    msg().reset();
    if (buVersion >= BUINFO_V4) {
        new (msg().addr()) BUPMsg::SBCInfoRspMsg2(transNum,
            exitcode(), info_v4.version, info_v4.apzVersion,
            info_v4.exchangeId, info_v4.generation, info_v4.sddSectorInfo,
            info_v4.ldd1SectorInfo, info_v4.ldd2SectorInfo, info_v4.psSectorInfo, 
            info_v4.rsSectorInfo, 
            info_v4.ps2SectorInfo, info_v4.rs2SectorInfo, 
            info_v4.dsGeneration, info_v4.ldd1OMProfile, info_v4.ldd1APZProfile,
            info_v4.ldd1APTProfile, info_v4.ldd1CpId, info_v4.ldd2OMProfile, 
            info_v4.ldd2APZProfile, info_v4.ldd2APTProfile, info_v4.ldd2CpId, 
            info_v4.cmdLogFile1, info_v4.cmdLogFile2);

        TRACE((LOG_LEVEL_INFO, "SBCInfoRspMsg2: TransNum = %d exitcode = %d Version Major = %d", 0, transNum, exitcode(), tvm));
        if(!exitcode()) {
            TRACE((LOG_LEVEL_INFO, "BUINFO Version: major: %d, minor: %d", 0, info_v4.version.major(),info_v4.version.minor()));
            TRACE((LOG_LEVEL_INFO, "APZVersion type : %d, version : %d, revision :%d, prodNumInfo : %d", 0, info_v4.apzVersion.type(),
                info_v4.apzVersion.version(), info_v4.apzVersion.revision(), info_v4.apzVersion.prodNumInfo()));

        }
    }
    else { // Return BUINFO Version 3 information
        SectorInfo zeroSector;
        memset(&zeroSector, 0, sizeof(SectorInfo));

        new (msg().addr()) BUPMsg::SBCInfoRspMsg2(transNum,
            exitcode(), info_v3.version, info_v3.apzVersion,
            info_v3.exchangeId, info_v3.generation, info_v3.sddSectorInfo,
            info_v3.ldd1SectorInfo, info_v3.ldd2SectorInfo, info_v3.psSectorInfo, 
            info_v3.rsSectorInfo, 

            /* info_v3.ps2SectorInfo*/ zeroSector, /* info_v3.rs2SectorInfo */ zeroSector, 
            /* info_v3.dsGeneration */ 0, /* info_v3.ldd1OMProfile */ 0, /* info_v3.ldd1APZProfile */ 0,
            /* info_v3.ldd1APTProfile */ 0, /* info_v3.ldd1CpId */ 0, /* info_v3.ldd2OMProfile */ 0, 
            /* info_v3.ldd2APZProfile */ 0, /* info_v3.ldd2APTProfile */ 0, /* info_v3.ldd2CpId */ 0, 

            info_v3.cmdLogFile1, info_v3.cmdLogFile2);

        TRACE((LOG_LEVEL_INFO, "SBCInfoRspMsg2: TransNum = %d exitcode = %d Version Major = %d", 0, transNum, exitcode(), tvm));
        if(!exitcode()) {
            TRACE((LOG_LEVEL_INFO, "BUINFO Version: major: %d, minor: %d", 0, info_v3.version.major(),info_v3.version.minor()));
            TRACE((LOG_LEVEL_INFO, "APZVersion type : %d, version : %d, revision :%d, prodNumInfo : %d", 0, info_v3.apzVersion.type(),
                info_v3.apzVersion.version(), info_v3.apzVersion.revision(), info_v3.apzVersion.prodNumInfo()));

        }
    }
}

