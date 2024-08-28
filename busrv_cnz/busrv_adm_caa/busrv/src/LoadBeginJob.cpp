/*
NAME
   File_name:LoadBeginJob.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2002-2023. All rights reserved.

   The Copyright to the computer program(s) herein is the property of Ericsson
   Utvecklings AB, Sweden.
   The program(s) may be used and/or copied only with the written permission from
   Ericsson Utvecklings AB or in accordance with the terms and conditions
   stipulated in the agreement/contract under which the program(s) have been
   supplied.

DESCRIPTION
   Executes the BUP command LOAD_BEGIN.

DOCUMENT NO
   190 89-CAA 109 0387

AUTHOR
   2002-02-05 by UAB/KY/SK Anders Gillgren (qabgill, tag :ag)
   2010-04-10 by DEK/xdtthng, Thanh Nguyen 
   
   2010-04-15 xdtthng SSI added
   2011-05-02 xdtthng Porting to Linux

SEE ALSO
   

Revision history
----------------
2002-02-05 qabgill Created
2006-06-09 uablan  Added CP-AP link speed in return msg
2007-03-05 uablan  Updates for Multi CP System
2011-09-05 xdtthng Total usage of FMS
2023-01-01 xkomala Added checks for valid cpid for TR IA20800
*/


#include "Config.h"
#include "LoadBeginJob.h"
#include "PlatformTypes.h"

#include "APBackupInfo.h"
#include "buap.h"
#include "BUPFunx.h"
#include "CodeException.h"
#include "CPS_BUSRV_Trace.h"
#include "DataFile.h"
#include "EventReporter.h"
#include "FileFunx.h"
#include "FtpFileSet.h"
#include "FtpPath.h"
#include "Key.h"
#include "TmpCurrDir.h"
#include "SystemInfo.h"
#include "LinuxException.h"

#ifndef LOCAL_BUILD                    
#include "EnvFMS.h"
#include "fms_cpf_file.h"
#endif

#include <fstream>
#include <sstream>

using namespace buap;
//using namespace SystemInfo;

//
// virtual
//====================================================================
void LoadBeginJob::execute() {

    newTRACE((LOG_LEVEL_INFO, "LoadBeginJob::execute()", 0));

    // local variables used in response
    Key key(false);
    string ftpLoadDir;
    u_int32 speed = SystemInfo::DEFAULT_BANDWIDTH;
    bool useLDD1 = false;
    CPID cpId = 0xFFFF; // default ONE CP system
    const BUPMsg::LoadBeginMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::LoadBeginMsg>(msg());
    u_int32 transNum = cmd->transNum(); 
    u_int16 tvm = cmd->version().major();

    try {

        // init stuff
        //===================================================================
        if(verifyVersion()) {

            //check if ONE CP System
        	if (cmd->cpSystem() != BUPMsg::ONE_CP_SYSTEM)
        	{
        		cpId = cmd->cpId();

        		//TR_IA20800 START
        		if(!(Config::instance().isValidSpxCpid(cpId)))
        		{
        			TRACE((LOG_LEVEL_ERROR, "LoadBeginJob, CpId received is <%d> ,throwing internal error", 0, cpId));
        			throw BUPMsg::INTERNAL_ERROR;
        		} //TR_IA20800 END

        	}

            TRACE((LOG_LEVEL_INFO, "LoadBeginJob, cpId = %d, CP System = %d", 0, cpId, cmd->cpSystem()));

            // Allways allow initial load from SPX and blades due to that
            // when starting APZ-VM a reload will be done and if relfsw0 is empty cyclic reload
            // will happen. And do not set flag for this load.

            // check rules
            //===================================================================
            // valid sbcId?
            if(!cmd->sbcId().valid())
                throw BUPMsg::PARAM_BACKUPID_OUT_OF_RANGE;

            // create busrv directory structure for this CP
            Config::instance().createDirStructure(cpId);
            
            TRACE((LOG_LEVEL_INFO, "LoadBeginJob, create dir structure ok", 0));

            SBCId sbcId = cmd->sbcId();    

            string destDir = Config::instance().sbcDir(sbcId, cpId);
            // make sure the RELFSWx exists
#ifdef LOCAL_BUILD                    
            if(!FileFunx::dirExists(destDir.c_str()))
#else
            if (!EnvFMS::dirExists(sbcId, cpId)) 
#endif            
                throw BUPMsg::SBC_NOT_FOUND;
                                
            TRACE((LOG_LEVEL_INFO, "LoadBeginJob, relfsw%d exists", 0, sbcId.id()));
            
            APBackupInfo buinfo(Config::instance().sbcDir(sbcId, cpId).c_str());
            BackupInfo_Base::BI_ERROR_CODE ec = buinfo.valid();
            if(ec != BackupInfo_Base::BEC_NO_ERROR)
                throw BUPMsg::BAD_BACKUPINFO;

            u_int32 admarker = buinfo.getAdMarker();
            bool adFileMarker = EnvFMS::fileExists(sbcId, Config::instance().MarkerFileName, cpId);
            TRACE((LOG_LEVEL_INFO, "BUINFO AdMarker = <%d>, File ADMARKER = <%d>", 0, admarker, adFileMarker));

            if ((adFileMarker && admarker) || (!adFileMarker && !admarker)) {
                // use LDD1 only if we don't have an LDD2
                useLDD1 = buinfo.info()->ldd2SectorInfo.outputTime().null();
            }
            else {
                useLDD1 = !EnvFMS::fileExists(sbcId, SBCFile::name(SBCFile::LDD2), cpId);
            }

            TRACE((LOG_LEVEL_INFO, "useLDD1 value is <%d>", 0, useLDD1));
                
            // 1. execute command
            // 1.1 create new key
            //===================================================================
            key.newKey();

            // 1.2 create "virtual" path for response
            ftpLoadDir = Config::instance().ftpLoadDir(sbcId, cpId);
            
            if(ftpLoadDir.length() > Config::FTP_PATH_MAX_DIR_LEN)            
                THROW_XCODE("Path to ftp virtual directory is too long", CodeException::CONFIGURATION_ERROR)

            // 1.3 get (local) path to tmp dir based on key & create the directory
            string tmpDir = Config::instance().tmpDir(key, cpId);
            FileFunx::createDirX(tmpDir.c_str());
            TmpCurrDir currDir(tmpDir.c_str());
            BUPFunx::writeParams(cmd);

        }
    }
    //
    // catch all error & set exitcode
    //
    catch(BUPMsg::ERROR_CODE exitcode) {
        newTRACE((LOG_LEVEL_ERROR, "Exiting LoadBegin w. error code: %d", 0, exitcode));
        m_exitcode = exitcode;
    }
#ifndef LOCAL_BUILD                    
    catch(FMS_CPF_Exception& x) {
        stringstream str;
        str << "LoadBeginJob, FMS Errorcode : " << x.errorCode() << ", Text: " << x.errorText() << ", Details: "
            << x.detailInfo() << endl;
        newTRACE((LOG_LEVEL_ERROR, "%s", 0, str.str().c_str()));
        switch(x.errorCode()) {
            case FMS_CPF_Exception::PHYSICALERROR:
                TRACE((LOG_LEVEL_ERROR, "LoadBeginJob, mapping FMS_CPF_Exception::PHYSICALERROR to BUPMsg::FMS_PHYSICAL_FILE_ERROR", 0));
                m_exitcode = BUPMsg::FMS_PHYSICAL_FILE_ERROR;
                break;
            default:
                m_exitcode = BUPMsg::INTERNAL_ERROR;
                break;
        }
        
        // Remove this logging for final release
        EventReporter::instance().write(str.str().c_str());        
    }
#endif
    catch(LinuxException& x) {

        newTRACE((LOG_LEVEL_ERROR, "To be removed; linuxException x is %d", 0, x.errcode()));

        EventReporter::instance().write(x);
        m_exitcode = BUPMsg::INTERNAL_ERROR;        
    }
    catch(CodeException& x) {

        newTRACE((LOG_LEVEL_ERROR, "To be removed; CodeException x is %d", 0, x.errcode()));

        switch(x.errcode()) {
            case CodeException::BUINFO_MISSING: // fall thru
            case CodeException::REQ_SBC_MISSING:
            case CodeException::BUINFO_ACCESS_DENIED:
            case CodeException::BAD_BACKUPINFO:            
                m_exitcode = BUPMsg::BAD_BACKUPINFO;
                break;
            case CodeException::CP_APG43_CONFIG_ERROR:
                TRACE((LOG_LEVEL_ERROR, "LoadBeginJob::execute() CS error ", 0));
                m_exitcode = BUPMsg::INTERNAL_ERROR;
                break;
            default:
                EventReporter::instance().write(x);
                m_exitcode = BUPMsg::INTERNAL_ERROR;
                break;
        }
    }
    catch(...) {

        newTRACE((LOG_LEVEL_ERROR, "To be removed; exception catch all", 0));

        EventReporter::instance().write("Unhandled internal error.");
        m_exitcode = BUPMsg::INTERNAL_ERROR;
    }

    // note that we may get here with an invalid info on exceptions, but the
    // data won't be set by the ctor if the ecode isn't OK
    msg().reset();
    FtpFileSet files;
    BUPFunx::setFiles(files, ALL, ftpLoadDir, useLDD1);
    

    //Get current link speed between AP-CP
    if (SystemInfo::getCurrentCPAPBandWidth(speed) != 0) {
        TRACE((LOG_LEVEL_WARN, "Failed to retrieve CP-AP link speed. Use default speed = %d", 0, speed));
    }
    else {
        TRACE((LOG_LEVEL_INFO, "CP-AP link speed = %d", 0, speed));
    }


    new (msg().addr()) BUPMsg::LoadBeginRspMsg(transNum, exitcode(), key, files, speed);
    TRACE((LOG_LEVEL_INFO, "LoadBeginRsp: exitcode = %d, Version Major = %d", 0, exitcode(), tvm));

}
//
//
// virtual
//====================================================================
void LoadBeginJob_V4::execute() {

    newTRACE((LOG_LEVEL_INFO, "LoadBeginJob_V4::execute()", 0));

    // local variables used in response
    Key key(false);
    string ftpLoadDir;
    u_int32 speed = SystemInfo::DEFAULT_BANDWIDTH;
    u_int32 useSubFiles = 0;
    
    //CPID cpId = 0xFFFF; // default ONE CP system
    const BUPMsg::LoadBeginMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::LoadBeginMsg>(msg());
    u_int32 transNum = cmd->transNum(); 
    u_int16 tvm = cmd->version().major();
    
    // remove loadingCPid for stub. Put this back on for blade
    CPID loadingCpId = cmd->cpId();
    SBCId sbcId = cmd->sbcId();

    try {

    	//TR_IA20800 START
    	if(!(Config::instance().isValidBladeCpid(loadingCpId)))
    	{
    		TRACE((LOG_LEVEL_ERROR, "LoadBeginJob_V4, CpId received is <%d> , throwing internal error", 0, loadingCpId));
    		throw BUPMsg::INTERNAL_ERROR;
    	} //TR_IA20800 END

        // init stuff
        //===================================================================
        if(verifyVersion()) {


            // Allways allow initial load from SPX and blades due to that
            // when starting APZ-VM a reload will be done and if relfsw0 is empty cyclic reload
            // will happen. And do not set flag for this load.
            //
            // SSI Project, THNG
            // The flag is set for protocol interaction. However, the flag is not read
            // and the rule "four simultaneous loads" not checked
            // If there is anything wrong this is the first to check

            const Config::CLUSTER tCluster(loadingCpId, true);
            const Config::CLUSTER tc;
            TRACE((LOG_LEVEL_INFO, "LoadBeginJob_V4, loadingCpId is <%d> sbcId<%d>", 0, loadingCpId, sbcId.id()));

            // check rules
            //===================================================================
            // valid sbcId?
            if(!cmd->sbcId().valid())
                throw BUPMsg::PARAM_BACKUPID_OUT_OF_RANGE;
        Config::instance().noOngoingDump(tCluster);

            // create busrv directory structure for this CP
        // /data/cps/data/cluster/bcx/busrv/
        Config::instance().createDataBusrvDirStructure(tCluster);
            Config::instance().createFtpDirStructure(Config::instance().tmpRoot(tCluster).c_str());
        // /data/cps/data/cluster/busrv/
        Config::instance().createDataBusrvDirStructure(tc);
        Config::instance().createFtpDirStructure(Config::instance().tmpRoot(tc).c_str());

            // make sure the RELFSWx exists
#ifdef LOCAL_BUILD
            if(!FileFunx::dirExists(Config::instance().sbcDir(cmd->sbcId(), tCluster).c_str()))
#else
            if (!EnvFMS::dirExists(sbcId, tCluster))
#endif
                throw BUPMsg::SBC_NOT_FOUND;
                     
            // NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
            // Backup Protocol Interaction
            // 
            // Check if any dumping on the same sbc in progress
            u_int16 loadingSbcId = cmd->sbcId().id();
            DataFile::BUACTREC dump;
            DataFile::BUACTREC_ITERATOR it;

            DataFile::BitVector& dumpVector = DataFile::readDumpVector();
            if (dumpVector != 0ULL) {
                DataFile::onGoingDump(dump);

                // Loading from the sbc having manual dump in progress is rejected
                if (loadingSbcId)
                    for (it = dump.begin(); it != dump.end(); ++it) {
                        if (it->first == loadingSbcId &&
                            it->second != loadingCpId)
                            throw BUPMsg::LOAD_REJECTED_DUE_TO_ONGOING_DUMP;
                    }
            }
            //
            // NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
            
            APBackupInfo buinfo(Config::instance().sbcDir(cmd->sbcId(), tCluster).c_str());
            BackupInfo_Base::BI_ERROR_CODE ec = buinfo.valid();
            if(ec != BackupInfo_Base::BEC_NO_ERROR)
                throw BUPMsg::BAD_BACKUPINFO;

            // Interaction code. Mark SBC loading in progress
            // This shall not be counted towards "max simult loadings"
            DataFile::startLoadingSBCID(loadingSbcId, tCluster);

            // Mark in core loading activity
            DataFile::setLoading(loadingCpId, tc);
            
            
            // UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU
            // Tool Support
            // Allow loading of BUINFO Version 3 and Version 4 Under APZ Profile 100
            //
            // Under APZ Profile == 100, BUP expects BUINFO Version 4 under normal operating
            // conditions. This enhancement allows Old Tool using BUINFO Version 3 to be 
            // loaded under APZ Profile == 100
    
            Version buVersion = buinfo.getVersion();
            string destDir = Config::instance().sbcDir(sbcId,tCluster);
            //string marker = destDir + Config::instance().MarkerFileName;
            u_int32 admarker = buinfo.getAdMarker();
            bool adFileMarker = EnvFMS::fileExists(sbcId, Config::instance().MarkerFileName, tCluster);
            TRACE((LOG_LEVEL_INFO, "BUINFO AdMarker = <%d>, File ADMARKER = <%d>", 0, admarker, adFileMarker));
                         
            // Check if none of the Auto Dump Marker exist or both of the markers exist
            if ((adFileMarker && admarker) || (!adFileMarker && !admarker)) {
                if (buVersion >= Version(4, 0)) {
                    // use LDD1 only if we don't have an LDD2
                    BackupInfo_Base::Info_V4 *pinfo = buinfo.info_V4();
                    if(pinfo->ldd2SectorInfo.outputTime().null())
                        useSubFiles |= Config::useLDD1;
                    // use PS only if we don't have an PS2, the same goes for RS/RS2
                    if(pinfo->ps2SectorInfo.outputTime().null())
                        useSubFiles |= Config::usePS;
                    if(pinfo->rs2SectorInfo.outputTime().null())
                        useSubFiles |= Config::useRS;
                 }
                 // BUINFO Version 3
                else {
                    // Always use RS and PS for BUINFO Version 3
                    BackupInfo_Base::Info *pinfo = buinfo.info();
                    useSubFiles |= Config::usePS | Config::useRS;
                    if(pinfo->ldd2SectorInfo.outputTime().null())
                        useSubFiles |= Config::useLDD1;
                }

            }
            else { // Only one of the marker exists

                if (buVersion >= Version(4, 0)) {
                    // The existence of the physical file LDD1 or LDD2 is used for selecting LDD sub file
                    if(!EnvFMS::fileExists(sbcId, SBCFile::name(SBCFile::LDD2), tCluster))
                    {
                        useSubFiles |= Config::useLDD1;
                    }
                    // use PS only if we don't have an PS2, the same goes for RS/RS2
                    if (!EnvFMS::fileExists(sbcId, SBCFile::name(SBCFile::PS2), tCluster))
                    {
                        useSubFiles |= Config::usePS;
                    }
                    if (!EnvFMS::fileExists(sbcId, SBCFile::name(SBCFile::RS2), tCluster))
                    {
                        useSubFiles |= Config::useRS;
                    }
                 }
                 // BUINFO Version 3
                else {
                    // Always use RS and PS for BUINFO Version 3
                    useSubFiles |= Config::usePS | Config::useRS;
                    
                    if (!EnvFMS::fileExists(sbcId, SBCFile::name(SBCFile::LDD2), tCluster))
                    {
                        useSubFiles |= Config::useLDD1;
                    }
                }
            }


            TRACE((LOG_LEVEL_INFO, "useSubFiles = %d for BUINFO Version %d", 0, useSubFiles, buVersion.major()));

            // 1. execute command
            // 1.1 create new key
            //===================================================================
            key.newKey();

            // 1.2 create "virtual" path for response
            //ftpLoadDir = Config::instance().ftpLoadDir(cmd->sbcId(), cpId);
            ftpLoadDir = Config::instance().ftpLoadDir(cmd->sbcId(), tCluster);
            if(ftpLoadDir.length() > Config::FTP_PATH_MAX_DIR_LEN)
                THROW_XCODE("Path to ftp virtual directory is too long", CodeException::CONFIGURATION_ERROR)

            // 1.3 get (local) path to tmp dir based on key & create the directory
            //string tmpDir = Config::instance().tmpDir(key, cpId);
            string tmpDir = Config::instance().tmpDir(key, tCluster);
            FileFunx::createDirX(tmpDir.c_str());
            TmpCurrDir currDir(tmpDir.c_str());
            BUPFunx::writeParams(cmd);
        }
    }
    //
    // catch all error & set exitcode
    //
    catch(BUPMsg::ERROR_CODE exitcode) {
        TRACE((LOG_LEVEL_ERROR, "Exiting LoadBegin with Exit Code: %d", 0, exitcode));
        m_exitcode = exitcode;
    }
#ifndef LOCAL_BUILD
    catch(FMS_CPF_Exception& x) {
        stringstream str;
        str << "LoadBeginJob_V4, FMS Errorcode : " << x.errorCode() << ", Text: " << x.errorText() << ", Details: "
            << x.detailInfo() << endl;
        newTRACE((LOG_LEVEL_ERROR, "%s", 0, str.str().c_str()));
        switch(x.errorCode()) {
            case FMS_CPF_Exception::PHYSICALERROR:
                TRACE((LOG_LEVEL_ERROR, "LoadBeginJob_V4, mapping FMS_CPF_Exception::PHYSICALERROR to BUPMsg::FMS_PHYSICAL_FILE_ERROR", 0));
                m_exitcode = BUPMsg::FMS_PHYSICAL_FILE_ERROR;
                break;
            default:
                m_exitcode = BUPMsg::INTERNAL_ERROR;
                break;
        }

        // Remove this logging for final release
        EventReporter::instance().write(str.str().c_str());
    }
#endif
    catch(LinuxException& x) {
        EventReporter::instance().write(x);
        m_exitcode = BUPMsg::INTERNAL_ERROR;        
    }
    catch(CodeException& x) {
        switch(x.errcode()) {
            case CodeException::BUINFO_MISSING: // fall thru
            case CodeException::BUINFO_ACCESS_DENIED:
                m_exitcode = BUPMsg::BAD_BACKUPINFO;
                break;
            case CodeException::CP_APG43_CONFIG_ERROR:
                TRACE((LOG_LEVEL_ERROR, "LoadBeginJob::execute() CS error ", 0));
                m_exitcode = BUPMsg::INTERNAL_ERROR;
                break;
            default:
                EventReporter::instance().write(x);
                m_exitcode = BUPMsg::INTERNAL_ERROR;
                break;
        }
    }
    catch(...) {
        EventReporter::instance().write("Unhandled internal error.");
        m_exitcode = BUPMsg::INTERNAL_ERROR;
    }

    // note that we may get here with an invalid info on exceptions, but the
    // data won't be set by the ctor if the ecode isn't OK
    
    msg().reset();
    FtpFileSet_V4 files;
     BUPFunx::setFiles(files, ALL, ftpLoadDir, useSubFiles);

    //Get current link speed between AP-CP
    if (SystemInfo::getCurrentCPAPBandWidth(speed) != 0) {
        TRACE((LOG_LEVEL_ERROR, "Failed to retrive CP-AP link speed", 0));
    }
    else {
        TRACE((LOG_LEVEL_INFO, "CP-AP link speed = %d", 0, speed));
    }

    new (msg().addr()) BUPMsg::LoadBeginRspMsg_V4(transNum, exitcode(), key, files, speed);
        
    TRACE((LOG_LEVEL_INFO, "LoadBeginRsp_V4: TransNum = %d exitcode = %d Version Major = %d", 0, transNum, exitcode(), tvm));
    if (!exitcode())
        TRACE((LOG_LEVEL_INFO, "Load with SBC: %d useSubFiles: %d", 0, sbcId.id(), useSubFiles));

}
