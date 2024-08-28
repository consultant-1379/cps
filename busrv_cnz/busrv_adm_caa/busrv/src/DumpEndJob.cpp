/*
NAME
   File_name:DumpEndJob.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2002-2023. All rights reserved.

   The Copyright to the computer program(s) herein is the property of Ericsson
   Utvecklings AB, Sweden.
   The program(s) may be used and/or copied only with the written permission from
   Ericsson Utvecklings AB or in accordance with the terms and conditions
   stipulated in the agreement/contract under which the program(s) have been
   supplied.

DESCRIPTION
   Executes the BUP command DUMP_END.

DOCUMENT NO
   190 89-CAA 109 0387

AUTHOR
   2002-02-05 by UAB/KY/SK Anders Gillgren (qabgill, tag :ag)
   2010-04-15 xdtthng SSI added

SEE ALSO
   

Revision history
----------------
2002-02-05 qabgill Created
2002-10-20 uabmnst Add ACSTrace
2006-03-01 uablan  Remove throw when open of internal file params fail.(HG69602)
2006-06-26 uablan  Remove the verify part. (HG69602)
2007-02-08 uablan  Changes for multiple CP system.
2010-05-23 xdtthng Modified for SSI
2011-09-14 xdtthng 43L, Total usage of FMS
2023-01-01 xkomala Added checks for valid cpid for TR IA20800
*/


#include "APBackupInfo.h"
#include "buap.h"
#include "BUPFunx.h"
#include "CodeException.h"
#include "Config.h"
#include "CPS_BUSRV_Trace.h"
#include "DataFile.h"
#include "DumpEndJob.h"
#include "EventReporter.h"
#include "FileFunx.h"
#include "SBCFile.h"
#include "TmpCurrDir.h"
#include "LinuxException.h"

#include "mcs_apcmh_loadbackup.h"

#ifndef LOCAL_BUILD                    
#include "EnvFMS.h"
#include "fms_cpf_file.h"
#endif

// Do not have these yet
//#include "ACS_CS_API.h"


#include <fstream>
#include <memory>

#include <cerrno>
#include <unistd.h>


using namespace std;
using namespace buap;

class UMask
{
public:
    UMask() {
        m_mode = umask(0);
    }
    ~UMask() {
        umask(m_mode);
    }
private:
    mode_t  m_mode;
};

//
// virtual
//====================================================================
void DumpEndJob::execute()
{

    newTRACE((LOG_LEVEL_INFO, "DumpEndJob::execute()", 0));

    // local variables used in response
    CPID cpId = 0xFFFF; // default ONE CP system
    const BUPMsg::DumpEndMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::DumpEndMsg>(msg());
    u_int32 transNum = cmd->transNum(); // save for response
    u_int16 tvm = cmd->version().major();

    SBCId sbcId;

    try
    {
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
        			TRACE((LOG_LEVEL_ERROR, "DumpEndJob, CpId received is <%d> ,throwing internal error", 0, cpId));
        			throw BUPMsg::INTERNAL_ERROR;
        		}//TR_IA20800 END
        	}
                
            TRACE((LOG_LEVEL_INFO, "DumpEndJob, result=%d, CP System=%d, ", 0, cmd->dumpResult(), cmd->cpSystem()));

            if (cmd->dumpResult() == BUPMsg::DUMP_OK)
            {
                // set source dir as current
                string keyDir = Config::instance().tmpDir(cmd->key(), cpId);

                // Need to use FileFunx::dirExists()
                if(!cmd->key().valid() || !FileFunx::dirExists(keyDir.c_str())) {
                    throw BUPMsg::INVALID_KEY;
                }
                // if this throws it is "internal error", not missing dir
                TmpCurrDir currDir(keyDir.c_str());
                
                // get params
                ifstream inf(PARAMS_FILE);
                if(!inf.good()) {
                    EventReporter::instance().write("Failed to open parameter file.");
                }

                u_int32 tmp32 = 0U;
                inf >> sbcId >> tmp32;
                BUPDefs::EXTENT extent = static_cast<BUPDefs::EXTENT>(tmp32);
                TRACE((LOG_LEVEL_INFO, "DumpEndJob::execute() Extent: %d, TransNum: %u, SbcId: %u ", 0, extent, transNum, sbcId.id()));

                inf.close();

                // check for destination dir
                //===================================================================
                string destDir = Config::instance().sbcDir(sbcId, cpId);

#ifdef LOCAL_BUILD                    
                if(!FileFunx::dirExists(destDir.c_str())) 
#else
                if (!EnvFMS::dirExists(sbcId, cpId))
#endif
                    throw BUPMsg::SBC_NOT_FOUND;

                // Verify input files
                //=====================================================================
                // read & check buinfo
                auto_ptr<APBackupInfo> buinfo(new APBackupInfo("./"));
                BackupInfo_Base::BI_ERROR_CODE ec = buinfo->valid();
                if(ec != BackupInfo_Base::BEC_NO_ERROR)
                    throw BUPMsg::BAD_BACKUPINFO;

                auto_ptr<APBackupInfo> biOld;
                bool oldHasSDD = false;
                if(extent != ALL) {
                    // Merge buinfo; check old file
                    biOld.reset(new APBackupInfo(destDir.c_str()));
                    if(biOld->valid() != BackupInfo_V4::BEC_NO_ERROR)
                        throw BUPMsg::BAD_BACKUPINFO;
                        
                    oldHasSDD = biOld->info()->sddSectorInfo.nofSegments() > 0;
/*
                    if(extent == SMALL)
                        biOld->replaceSmall(*buinfo);
                    
                    else
                        biOld->replaceSmallAndLarge(*buinfo);
                    biOld->createFile("./"); // replace new bi from CP w. "merged" file
*/
                }
                else {
                    // going destructive on RELFSW here
                    TRACE((LOG_LEVEL_INFO, "********* Prepare to call EnvFMS::cleanDir()", 0));
#ifdef LOCAL_BUILD                    
                    FileFunx::cleanDir(destDir.c_str(), "*", false);
#else
                    EnvFMS::cleanDir(sbcId, cpId);
#endif
                }

                // move data files        
                //=====================================================================
                // sdd

                int status = JustStarted;
                int progress = JustStarted;
    
                try {
                
                    if(extent != ALL) {
                        progress |= (status = CreatingMarker);
                        createMarker(sbcId, destDir.c_str(), cpId);
                        TRACE((LOG_LEVEL_INFO, "Marker created OK; returning to DumpEndJob", 0));
                        progress |= (status = MarkerCreated);
                    }
 
                    string destFile = destDir + SBCFile::name(SBCFile::SDD);
                    if(buinfo->info()->sddSectorInfo.nofSegments()) {
                        moveFile(sbcId, SBCFile::name(SBCFile::SDD), destFile, (extent == ALL || !oldHasSDD), cpId);
                    }
                    else {
#ifdef LOCAL_BUILD                    
                         FileFunx::DeleteFileX(destFile.c_str()); // returns false if file not found
#else
                         if (EnvFMS::fileExists(sbcId, SBCFile::name(SBCFile::SDD), cpId))
                             EnvFMS::deleteFile(sbcId, SBCFile::name(SBCFile::SDD), cpId);
#endif
                    }

                    progress |= (status = SDDmoved);   

                    // Only simulate crashing at auto dump
                    // ********* Crash simulation ********************
                    // Do not remove; Comment out this before release
                    //
                    if(extent != ALL)
                        Config::instance().crashing(LDD1renamed, cpId);
                    //
                    // ***********************************************

#ifdef LOCAL_BUILD                    
                    // ldd
                    if(extent == SMALL_AND_LARGE) {
                        moveFile(sbcId, destDir + SBCFile::name(SBCFile::LDD1), destDir + SBCFile::name(SBCFile::LDD2), false, cpId);
                    }
                    if(extent == SMALL_AND_LARGE || extent == ALL) {
                        moveFile(sbcId, SBCFile::name(SBCFile::LDD1), destDir + SBCFile::name(SBCFile::LDD1), extent == ALL, cpId);
                    }
#else
                    // ldd
                    if(extent == SMALL_AND_LARGE && EnvFMS::fileExists(sbcId, SBCFile::name(SBCFile::LDD1), cpId)) {
                        if (EnvFMS::fileExists(sbcId, SBCFile::name(SBCFile::LDD2), cpId))
                            EnvFMS::deleteFile(sbcId, SBCFile::name(SBCFile::LDD2), cpId);
                        EnvFMS::rename(sbcId, SBCFile::name(SBCFile::LDD1), SBCFile::name(SBCFile::LDD2), cpId);
                        progress |= (status = LDD1renamed);
                    }
                    if(extent == SMALL_AND_LARGE || extent == ALL) {

                        // Only simulate crashing at auto dump
                        // ********* Crash simulation ********************
                        // Do not remove; Comment out this before release
                        //
                        if(extent != ALL)
                            Config::instance().crashing(LDD1moved, cpId);
                        //
                        // ***********************************************
                        //moveFile(sbcId, SBCFile::name(SBCFile::LDD1), destDir + SBCFile::name(SBCFile::LDD1), 
                        //                  extent == ALL || !biOld->info()->ldd1SectorInfo.nofSegments(), cpId);
                        moveFile(sbcId, SBCFile::name(SBCFile::LDD1), destDir + SBCFile::name(SBCFile::LDD1), true,  cpId);

                        progress |= (status = LDD1moved);
                    }
#endif                
                    // ps & rs
                    if (extent == ALL) {
                        moveFile(sbcId, SBCFile::name(SBCFile::PS), destDir + SBCFile::name(SBCFile::PS), true, cpId);
                        moveFile(sbcId, SBCFile::name(SBCFile::RS), destDir + SBCFile::name(SBCFile::RS), true, cpId);
                    }
                    else {
                        // Merge BUINFO for auto dump
                        if(extent == SMALL)
                            biOld->replaceSmall(*buinfo);                        
                        else
                            biOld->replaceSmallAndLarge(*buinfo);

                        biOld->createFile("./");
                    }
                        
                    // Move merged BUINFO to destination
                    moveFile(sbcId, SBCFile::name(SBCFile::BACKUP_INFO), destDir + SBCFile::name(SBCFile::BACKUP_INFO), extent == ALL, cpId);
                    progress |= (status = BUINFOmoved);

                   if (extent != ALL) {
                        TRACE((LOG_LEVEL_INFO, "Deleting Marker attmept", 0));
                        EnvFMS::deleteFile(sbcId, Config::instance().MarkerFileName, cpId);
                        TRACE((LOG_LEVEL_INFO, "Marker deleted OK", 0));
                    } 
                }
                catch (...) {

                    TRACE((LOG_LEVEL_INFO, "Dump failed when moving files, status = %d", 0, status));
                    
                    // There is nothing which can be done for the following
                    // - can not create the marker
                    // - can not moved BUINFO
                    // - can not delete the marker
                    if (!status || status == CreatingMarker || status == LDD1moved || status == BUINFOmoved) {
                        ostringstream os;
                        os << "Dump ended abnormally during DUMP_END primitive\nat progress value = <" << progress << "> RELFSW0 remains unchanged";
                        TRACE((LOG_LEVEL_INFO, "%s", 0, os.str().c_str()));
                        EventReporter::instance().write(os.str().c_str());
                        //throw BUPMsg::FMS_PHYSICAL_FILE_ERROR;
                        if (extent != ALL) {
                            throw BUPMsg::AUTODUMP_FAILED_BUINFO_NOT_CORRECTED;
                        }
                        else
                            throw BUPMsg::FMS_PHYSICAL_FILE_ERROR;
                    }
                        
                    // BUINFO recovering process
                    TRACE((LOG_LEVEL_INFO, "Dumping progress is %d", 0, progress));                    
                    if (extent != ALL) {
                    	bool mvBuInfo = true;
                    	try {
							if(extent == SMALL)
								biOld->replaceSmall(*buinfo, progress);
							else
								biOld->replaceSmallAndLarge(*buinfo, progress);
							biOld->createFile("./");
							TRACE((LOG_LEVEL_INFO, "Moving recovered BUINFO to destination", 0));
							moveFile(sbcId, SBCFile::name(SBCFile::BACKUP_INFO), destDir + SBCFile::name(SBCFile::BACKUP_INFO), extent == ALL, cpId);
                    	}
                    	catch (...) {
                    		mvBuInfo = false;
                    	}

                        ostringstream os;
                        os << "Dump ended abnormally during DUMP_END primitive\nat progress value = <" << progress 
                           << ">  RELFSW0 is updated with Auto Dump Marker";
                        TRACE((LOG_LEVEL_INFO, "%s", 0, os.str().c_str()));
                        EventReporter::instance().write(os.str().c_str());

                        // It is still an error auto dump even though relfsw0
                        // is ensured as loadable as possible
                        //throw BUPMsg::FMS_PHYSICAL_FILE_ERROR;
                         throw mvBuInfo? BUPMsg::AUTODUMP_FAILED_BUINFO_CORRECTED :
                        				BUPMsg::AUTODUMP_FAILED_BUINFO_NOT_CORRECTED;
                    }
                    else 
                        throw BUPMsg::FMS_PHYSICAL_FILE_ERROR;   
                }

                try {
                    // clean temp dump dir (can't clean a dir we're in)
                    FileFunx::SetCurrentDirectoryX(Config::instance().dataBusrvDir(cpId).c_str());
                    size_t failCnt = FileFunx::cleanDir(keyDir.c_str(), "*");
                    if (failCnt != 0) {
                        TRACE((LOG_LEVEL_ERROR, "DumpEndJob, Failed to delete %d no of files", 0, failCnt));
                        EventReporter::instance().write("Faild to delete temp files");
                    }
                }
                catch(LinuxException& x) {
                    EventReporter::instance().write(x); // log on AP, but OK to CP
                }
                catch(...) {
                    EventReporter::instance().write("Failed to clean dir");
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
        str << "DumpEndJob, FMS Errorcode : " << x.errorCode() << ", Text: " << x.errorText() << ", Details: "
            << x.detailInfo() << endl;
        newTRACE((LOG_LEVEL_ERROR, "%s", 0, str.str().c_str()));
        m_exitcode = BUPMsg::INTERNAL_ERROR;
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
            case CodeException::BAD_BACKUPINFO:
                m_exitcode = BUPMsg::BAD_BACKUPINFO;
                break;
            case CodeException::REQ_SBC_MISSING:
                m_exitcode = BUPMsg::SBC_NOT_FOUND;
                break;
            case CodeException::BAD_DATA_FILE:
                m_exitcode = BUPMsg::BAD_DATAAREA;
                break;
            case CodeException::FMS_CREATE_FILE_FAILED:
                m_exitcode = BUPMsg::CREATE_DATAAREA_FAILED;
                break;
            case CodeException::FMS_PHYSICAL_FILE_ERROR:
                TRACE((LOG_LEVEL_ERROR, "DumpEndJob::execute() Physical file error ", 0));
                m_exitcode = BUPMsg::FMS_PHYSICAL_FILE_ERROR;
                break;
            case CodeException::CP_APG43_CONFIG_ERROR:
                TRACE((LOG_LEVEL_ERROR, "DumpEndJob::execute() CS error ", 0));
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

    // Check enviroment and CP backup final result
    if( (cmd->cpSystem() == BUPMsg::ONE_CP_SYSTEM) &&
    		(cmd->dumpResult() == BUPMsg::DUMP_OK) &&
			(BUPMsg::OK == m_exitcode) )
    {
    	try
    	{
			// Communicate to APCMH to add its parts to the CP Backup
			std::string dumpPath = Config::instance().sbcDir(sbcId, cpId);
			TRACE((LOG_LEVEL_INFO, "DumpEndJob, call backupAPTCMMfile(%s)", 0, dumpPath.c_str()));
			MCS_APCMH_LoadBackup apcmh_client;
			int result = apcmh_client.backupAPTCMMfile(dumpPath.c_str());
			TRACE((LOG_LEVEL_INFO, "DumpEndJob, backupAPTCMMfile() returns:<%d>", 0, result));
    	}
    	catch(...)
    	{
    		TRACE((LOG_LEVEL_ERROR, "DumpEndJob, exception before call backupAPTCMMfile()", 0));
    	}

    }

    // reset flag that dump is no longer on going
    DataFile::endDumping(cpId);

    //
    // create response in the msg buffer (re-use)
    //======================================================================
    msg().reset();
    new (msg().addr()) BUPMsg::DumpEndRspMsg(transNum, exitcode());
    TRACE((LOG_LEVEL_INFO, "DumpEndRspMsg: TransNum = %d exitcode = %d Version Major = %d", 0, transNum, exitcode(), tvm));

}


//
//
//==========================================================================
void DumpEndJob::createMarker(const SBCId& sbcId, const char* fn, CPID cpId)
{
    newTRACE((LOG_LEVEL_INFO, "DumpEndJob::createMarker(%s, %d)", 0, fn, cpId));

    string name = fn;
    name += Config::instance().MarkerFileName;
    if (FileFunx::fileExists(name.c_str())) {
        ostringstream os;
        os << "Auto dump aborted during DUMP_END primitive due to marker File exist <" << name << ">";
        TRACE((LOG_LEVEL_ERROR, "%s", 0, os.str().c_str()));
        EventReporter::instance().write(os.str().c_str());
        //throw BUPMsg::FMS_PHYSICAL_FILE_ERROR;
        throw BUPMsg::AUTODUMP_FAILED_BUINFO_NOT_CORRECTED;
    }

    TRACE((LOG_LEVEL_INFO, "Marker file does not exist; attempt to create marker file", 0));
    try {
        EnvFMS::createFile(sbcId, Config::instance().MarkerFileName, cpId);
        TRACE((LOG_LEVEL_INFO, "Marker file is created successfully", 0));
    }
    catch (...) {
        ostringstream os;
        os << "Cannot create marker file <" << name.c_str() << ">; auto dump aborted";
        TRACE((LOG_LEVEL_ERROR, "%s", 0, os.str().c_str()));
        EventReporter::instance().write(os.str().c_str());
        //throw BUPMsg::FMS_PHYSICAL_FILE_ERROR;
        throw BUPMsg::AUTODUMP_FAILED_BUINFO_NOT_CORRECTED;
    }
}


//
//
//===========================================================================
void DumpEndJob::moveFile(SBCId sbcId, const string& srcfile,
                                  const string& destfile, bool fmsCall, CPID cpId) {

    newTRACE((LOG_LEVEL_INFO, "DumpEndJob::moveFile(%d, %s, %s, fmsCall=%d, %d)", 0, sbcId.id(), srcfile.c_str(), destfile.c_str(), fmsCall, cpId));

#ifndef LOCAL_BUILD
    try {
        //TODO:
        FileFunx::chown(srcfile.c_str());

        //TRACE(("******** DumpEndJob::moveFile() running FMS ********************", 0));
        if(fmsCall){
            TRACE((LOG_LEVEL_INFO, "******* DumpEndJob::moveFile() calling EnvFMS::createFile()", 0));
            EnvFMS::createFile(sbcId, srcfile.c_str(), cpId);
            //TRACE(("******* DumpEndJob::moveFile(); EnvFMS::createFile() returns", 0));
        }
    }
    catch(FMS_CPF_Exception& x){
        if (isTRACEABLE()) {
            stringstream str;
            str << "DumpEndJob::moveFile(), FMS Errorcode : " << x.errorCode() << ", Text: " << x.errorText() << ", Details: "
                << x.detailInfo() << endl;
            TRACE((LOG_LEVEL_ERROR, "%s", 0, str.str().c_str()));
        }
        switch(x.errorCode()) {
            case FMS_CPF_Exception::FILEEXISTS:
                // This should not be an exception
                TRACE((LOG_LEVEL_WARN, "*** It is ok to open an existing file; should not throw; resume execution ****", 0));
                break;
            default:
                throw;
                break;
        }
    }
    catch(CodeException& x) {
        newTRACE((LOG_LEVEL_ERROR, "DumpEndJob::moveFile(), Configuration error. Errorcode: %d", 0, x.errcode()));
        throw;
    }
    catch(...){
        //throw CS fel
        throw;
    }
#endif

    DumpEndMoveFile move;
    move(srcfile, destfile);


}

///////////////////////////////////////////////////////////////////////////////
//
// virtual
//====================================================================
void DumpEndJob_V4::execute() {

    newTRACE((LOG_LEVEL_INFO, "DumpEndJob_V4::execute()", 0));

    // local variables used in response
    const BUPMsg::DumpEndMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::DumpEndMsg>(msg());
    u_int32 transNum = cmd->transNum(); 
    u_int16 tvm = cmd->version().major();
    CPID origCpId = 0xffff;
    //u_int16 APZProfile = Config::instance().getAPZProfile();
    //TRACE(("DumpEndJob_V4::execute(); APZ Profile: %d", 0, APZProfile));

    try {
        // init stuff
        //===================================================================
        if(verifyVersion()) {

            origCpId = cmd->cpId();

            //TR_IA20800 START
            if(!(Config::instance().isValidBladeCpid(origCpId)))
            {
            	TRACE((LOG_LEVEL_ERROR, "DumpEndJob_V4, CpId received is <%d> ,throwing internal error", 0, origCpId));
            	throw BUPMsg::INTERNAL_ERROR;
            } //TR_IA20800 END


            const Config::CLUSTER tCluster(origCpId, true);

            TRACE((LOG_LEVEL_INFO, "DumpEndJob, result=%d, CP System=%d, ", 0, cmd->dumpResult(), cmd->cpSystem()));
            if (cmd->dumpResult() == BUPMsg::DUMP_OK) {

                // set source dir as current
                //string keyDir = Config::instance().tmpDir(cmd->key(), cpId);
                string keyDir = Config::instance().tmpDir(cmd->key(), tCluster);

                if(!cmd->key().valid() || !FileFunx::dirExists(keyDir.c_str())) {
                    throw BUPMsg::INVALID_KEY;
                }
                // if this throws it is "internal error", not missing dir
                TmpCurrDir currDir(keyDir.c_str());
                
                // get params
                ifstream inf(PARAMS_FILE);
                if(!inf.good()) {
                    EventReporter::instance().write("Failed to open parameter file.");
                }
                SBCId sbcId;
                u_int32 tmp32 = 0;
                inf >> sbcId >> tmp32;
                BUPDefs::EXTENT extent = static_cast<BUPDefs::EXTENT>(tmp32);
                TRACE((LOG_LEVEL_INFO, "DumpEndJob_V4::execute() Extent: %d, TransNum: %u, SbcId: %u cpId %u", 0, extent, transNum, sbcId.id(), origCpId));

                inf.close();

                // NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
                // Backup Protocol Interaction
                // 
                // If sbc 0 is used to load a CP, file output to sbc 0
                // is queued for later execution
                
                DataFile::BUACTREC load;
                DataFile::BUACTREC_ITERATOR it;
                m_sbcId = sbcId.id();        // Used by executeSpecial()
                DataFile::BitVector& loadVector = DataFile::readLoadVector();
                if (loadVector != 0ULL) {
                    DataFile::onGoingLoad(load);
                
                    if (sbcId.id() == 0)
                        for (it = load.begin(); it != load.end(); ++it) {
                            if (it->first == m_sbcId) {
                                const Config::CLUSTER tn(origCpId, true);
                                DataFile::dumpEndQueued(sbcId, tn);
                                resetState(Job::JQS_QUEUED, sbcId);
                                DataFile::BitVector& dv = DataFile::readDumpVector();
                                dv.dumpEndQueue(true);
                                return;
                            }
                        }
                }
                //
                // NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
                //

                // check for destination dir
                //===================================================================
                string destDir = Config::instance().sbcDir(sbcId, tCluster);

#ifdef LOCAL_BUILD
                if(!FileFunx::dirExists(destDir.c_str()))
#else
                if (!EnvFMS::dirExists(sbcId, tCluster))
#endif
                    // is this an internal error? checked in DumpBegin, but now its gone
                    throw BUPMsg::SBC_NOT_FOUND; 

                // Verify input files
                //=====================================================================
                // read & check buinfo
                auto_ptr<APBackupInfo> buinfo(new APBackupInfo("./"));
                BackupInfo_Base::BI_ERROR_CODE ec = buinfo->valid();
                if(ec != BackupInfo_Base::BEC_NO_ERROR)
                    throw BUPMsg::BAD_BACKUPINFO;

                auto_ptr<APBackupInfo> biOld;
                if(extent != ALL) {
                    // merge buinfo
                    biOld.reset(new APBackupInfo(destDir.c_str()));
                    // check old file
                    if(biOld->valid() != BackupInfo_Base::BEC_NO_ERROR)
                        throw BUPMsg::BAD_BACKUPINFO;

                    //biOld->replaceSmallAndLarge(*buinfo);
                    //biOld->createFile("./"); // replace new bi from CP w. "merged" file
                }
                else {
                    // going destructive on RELFSW here
                    TRACE((LOG_LEVEL_INFO, "********* Prepare to call EnvFMS::cleanDir()", 0));
#ifdef LOCAL_BUILD
            FileFunx::cleanDir(destDir.c_str(), "*", false);
#else
                EnvFMS::cleanDir(sbcId, tCluster);
#endif
               }


                // move data files      
                //=====================================================================
                int status = JustStarted;
                int progress = JustStarted;

                try {
                    // Create marker
                    if(extent != ALL) {
                        progress |= (status = CreatingMarker);
                        createMarker(sbcId, destDir.c_str(), tCluster);
                        TRACE((LOG_LEVEL_INFO, "Marker created OK; returning to DumpEndJob_V4", 0));
                        progress |= (status = MarkerCreated);
                    }
                    
                    if(extent == NEW_SMALL_AND_LARGE) {

                        // Rename LDD1
                        if (EnvFMS::fileExists(sbcId, SBCFile::name(SBCFile::LDD2), tCluster))
                            EnvFMS::deleteFile(sbcId, SBCFile::name(SBCFile::LDD2), tCluster);
                        //TODO:
                        Config::instance().crashing(LDD1renamed, tCluster);

                        EnvFMS::rename(sbcId, SBCFile::name(SBCFile::LDD1), SBCFile::name(SBCFile::LDD2), tCluster);
                        progress |= (status = LDD1renamed);                         

                        if (EnvFMS::fileExists(sbcId, SBCFile::name(SBCFile::PS2), tCluster))
                            EnvFMS::deleteFile(sbcId, SBCFile::name(SBCFile::PS2), tCluster);
                        EnvFMS::rename(sbcId, SBCFile::name(SBCFile::PS), SBCFile::name(SBCFile::PS2), tCluster);
                        progress |= (status = PSrenamed);

                        if (EnvFMS::fileExists(sbcId, SBCFile::name(SBCFile::RS2), tCluster))
                            EnvFMS::deleteFile(sbcId, SBCFile::name(SBCFile::RS2), tCluster);
                        EnvFMS::rename(sbcId, SBCFile::name(SBCFile::RS), SBCFile::name(SBCFile::RS2), tCluster);
                        progress |= (status = RSrenamed);

                    } 
                    
                    // For SSI, extent == NEW_SMALL_AND_LARGE and extent == ALL have the same hadnling
                    if(extent == NEW_SMALL_AND_LARGE || extent == ALL) {

                        //TODO: Move LDD1 failed
                        Config::instance().crashing(LDD1moved, tCluster);                        

                        moveFile(sbcId, SBCFile::name(SBCFile::LDD1), destDir + SBCFile::name(SBCFile::LDD1), true, tCluster);
                        progress |= (status = LDD1moved);

                        //TODO: Move PS failed
                        Config::instance().crashing(PSmoved, tCluster);

                        moveFile(sbcId, SBCFile::name(SBCFile::PS), destDir + SBCFile::name(SBCFile::PS), true, tCluster);
                        progress |= (status = PSmoved);

                        //TODO: Move RS failed
                        Config::instance().crashing(RSmoved, tCluster);

                        moveFile(sbcId, SBCFile::name(SBCFile::RS), destDir + SBCFile::name(SBCFile::RS), true, tCluster);
                        progress |= (status = RSmoved);
                    }
                    
                    if (extent == NEW_SMALL_AND_LARGE) {
                        biOld->replaceSmallAndLarge(*buinfo);
                        biOld->createFile("./"); // replace new bi from CP w. "merged" file
                    }
                    // Move merged BUINFO to destination
                    moveFile(sbcId, SBCFile::name(SBCFile::BACKUP_INFO), destDir + SBCFile::name(SBCFile::BACKUP_INFO), extent == ALL, tCluster);
                    progress |= (status = BUINFOmoved);

                    if(extent != ALL) {
                        TRACE((LOG_LEVEL_INFO, "Deleting Marker attempt", 0));
                        EnvFMS::deleteFile(sbcId, Config::instance().MarkerFileName, tCluster);
                        TRACE((LOG_LEVEL_INFO, "Marker deleted OK", 0));
                    }

                }
                catch (...) {
                    TRACE((LOG_LEVEL_ERROR, "Dump failed when moving files, status = %d", 0, status));
                    
                    if (!status || status == CreatingMarker || status == RSmoved || status == BUINFOmoved)
                    {
                        ostringstream os;
                        os << "Dump ended abnormally during DUMP_END  primitive\nat progress value = <" << progress;
                        TRACE((LOG_LEVEL_WARN, "%s", 0, os.str().c_str()));
                        EventReporter::instance().write(os.str().c_str());
                        if (extent != ALL) {
                            throw BUPMsg::AUTODUMP_FAILED_BUINFO_NOT_CORRECTED; 
                        }
                        else
                           throw BUPMsg::FMS_PHYSICAL_FILE_ERROR;

                    }

                    // BUINFO recovering process
                    TRACE((LOG_LEVEL_INFO, "Dumping progress is %d", 0, progress));                    
                    if (extent != ALL) {
                    	bool mvBuInfo = true;
                    	try {
							biOld->replaceSmallAndLarge(*buinfo, progress);
							biOld->createFile("./");
							TRACE((LOG_LEVEL_INFO, "Moving recovered BUINFO to destination", 0));
							moveFile(sbcId, SBCFile::name(SBCFile::BACKUP_INFO), destDir + SBCFile::name(SBCFile::BACKUP_INFO), extent == ALL, tCluster);
                    	}
                    	catch (...) {
                    		mvBuInfo = false;
                        }
                        ostringstream os;
                        os << "Dump ended abnormally during DUMP_END primitive\nat progress value = <" << progress 
                           << ">  RELFSW0 is updated with Auto Dump Marker";
                        TRACE((LOG_LEVEL_WARN, "%s", 0, os.str().c_str()));
                        EventReporter::instance().write(os.str().c_str());

                        // It is still an error auto dump even though relfsw0
                        // is ensured as loadable as possible
                        //throw BUPMsg::FMS_PHYSICAL_FILE_ERROR;
                        throw mvBuInfo? BUPMsg::AUTODUMP_FAILED_BUINFO_CORRECTED :
                        				BUPMsg::AUTODUMP_FAILED_BUINFO_NOT_CORRECTED;
                    }
                    else
                        throw BUPMsg::FMS_PHYSICAL_FILE_ERROR;
                }
                
                try {
                    // clean dump dir (can't clean a dir we're in)
                    FileFunx::SetCurrentDirectoryX(Config::instance().dataBusrvDir(tCluster).c_str());

                    int failCnt = FileFunx::cleanDir(keyDir.c_str(), "*");
                    if (failCnt != 0) {
                        TRACE((LOG_LEVEL_ERROR, "Failed to delete %d no of files", 0, failCnt));
                        EventReporter::instance().write("Faild to delete files");
                    }
                }
                catch(LinuxException& x) {
                    EventReporter::instance().write(x); // log on AP, but OK to CP
                }
                catch(...) {
                    EventReporter::instance().write("Failed to clean dir");
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
        str << "DumpEndJob_V4, FMS Errorcode : " << x.errorCode() << ", Text: " << x.errorText() << ", Details: "
            << x.detailInfo() << endl;
        newTRACE((LOG_LEVEL_ERROR, "%s", 0, str.str().c_str()));
        m_exitcode = BUPMsg::INTERNAL_ERROR;
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
            case CodeException::BAD_BACKUPINFO:
                m_exitcode = BUPMsg::BAD_BACKUPINFO;
                break;
            case CodeException::REQ_SBC_MISSING:
                m_exitcode = BUPMsg::SBC_NOT_FOUND;
                break;
            case CodeException::BAD_DATA_FILE:
                m_exitcode = BUPMsg::BAD_DATAAREA;
                break;
            case CodeException::FMS_CREATE_FILE_FAILED:
                m_exitcode = BUPMsg::CREATE_DATAAREA_FAILED;
                break;
            case CodeException::FMS_PHYSICAL_FILE_ERROR:
                TRACE((LOG_LEVEL_ERROR, "DumpEndJob_V4::execute() Physical file error ", 0));
                m_exitcode = BUPMsg::FMS_PHYSICAL_FILE_ERROR;
                break;
#if 0
            case CodeException::FAILED_BUINFO_CORRECTED:
                m_exitcode = BUPMsg::AUTODUMP_REJECTED_BUINFO_CORRECTED;
                break;
            case CodeException::FAILED_BUINFO_NOT_CORRECTED:
                m_exitcode = BUPMsg::AUTODUMP_REJECTED_BUINFO_NOT_CORRECTED;
                break;
#endif
            case CodeException::CP_APG43_CONFIG_ERROR:
                TRACE((LOG_LEVEL_ERROR, "DumpEndJob_V4::execute() CS error ", 0));
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

    // reset flag that dump is no longer on going
    //const BUPMsg::DumpEndMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::DumpEndMsg>(msg());
    const Config::CLUSTER tn(origCpId, true);
    DataFile::endDumping(tn);
    DataFile::endDumpingSBCID(tn);

    const Config::CLUSTER tc;
    DataFile::clearDumping(origCpId, tc);


    //
    // create response in the msg buffer (re-use)
    //======================================================================
    msg().reset();
    new (msg().addr()) BUPMsg::DumpEndRspMsg(transNum, exitcode());

    //TRACE(("DumpEndRspMsg: TransNum = %d exitcode = %d Version Major = %d APZ Profile = %d", 0, transNum, exitcode(), tvm, APZProfile));
    TRACE((LOG_LEVEL_INFO, "DumpEndRspMsg: TransNum = %d exitcode = %d Version Major = %d", 0, transNum, exitcode(), tvm));
}
//
//
//==========================================================================
void DumpEndJob_V4::createMarker(const SBCId& sbcId, const char* fn, const Config::CLUSTER& t)
{
    newTRACE((LOG_LEVEL_INFO, "DumpEndJob_V4::createMarker(%s)", 0, fn));

    string name = fn;
    name += Config::instance().MarkerFileName;
    if (FileFunx::fileExists(name.c_str())) {
        ostringstream os;
        os << "Auto dump aborted during DUMP_END primitive due to marker File exist <" << name << ">";
        TRACE((LOG_LEVEL_ERROR, "%s", 0, os.str().c_str()));
        EventReporter::instance().write(os.str().c_str());
        //throw BUPMsg::FMS_PHYSICAL_FILE_ERROR;
        throw BUPMsg::AUTODUMP_FAILED_BUINFO_NOT_CORRECTED;
    }

    TRACE((LOG_LEVEL_INFO, "Marker file does not exist; attempt to create marker file", 0));
    try {
        EnvFMS::createFile(sbcId, Config::instance().MarkerFileName, t);
        TRACE((LOG_LEVEL_INFO, "Marker file is created successfully", 0));
    }
    catch (...) {
        ostringstream os;
        os << "Cannot create marker file <" << name.c_str() << ">; auto dump aborted";
        TRACE((LOG_LEVEL_ERROR, "%s", 0, os.str().c_str()));
        EventReporter::instance().write(os.str().c_str());
        //throw BUPMsg::FMS_PHYSICAL_FILE_ERROR;
        throw BUPMsg::AUTODUMP_FAILED_BUINFO_NOT_CORRECTED;
    }    
}

//
//
//===========================================================================
void DumpEndJob_V4::moveFile(SBCId sbcId, const string& srcfile,
                const string& destfile, bool fmsCall, const Config::CLUSTER& t) {

    newTRACE((LOG_LEVEL_INFO, "DumpEndJob_V4::moveFile(%d, %s, %s, CLUSTER)", 0, sbcId.id(), srcfile.c_str(), destfile.c_str()));

    try {
        //TODO:
        FileFunx::chown(srcfile.c_str());

        if(fmsCall){
            EnvFMS::createFile(sbcId, srcfile.c_str(), t);
        }
    }
    catch(FMS_CPF_Exception& x){
        if (isTRACEABLE()) {
            stringstream str;
            str << "DumpEndJob_V4::moveFile(), FMS Errorcode : " << x.errorCode() << ", Text: " << x.errorText() << ", Details: "
                << x.detailInfo() << endl;
            TRACE((LOG_LEVEL_ERROR, "%s", 0, str.str().c_str()));
        }
        switch(x.errorCode()) {
            case FMS_CPF_Exception::FILEEXISTS:
                // This should not be an exception
                TRACE((LOG_LEVEL_WARN, "*** It is ok to open an existing file; should not throw; resume execution ****", 0));
                break;
            default:
                throw;
                break;
        }
    }
    catch(CodeException& x) {
        newTRACE((LOG_LEVEL_ERROR, "DumpEndJob::moveFile(), Configuration error. Errorcode: %d", 0, x.errcode()));
        throw;
    }
    catch(...){
        //throw CS fel
        throw;
    }


    DumpEndMoveFile move;
    move(srcfile, destfile);
}

//
//
// NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
// Backup Protocol Interaction
//
// virtual
//====================================================================
void DumpEndJob_V4::executeSpecial(u_int16) {

    newTRACE((LOG_LEVEL_INFO, "DumpEndJob_V4::executeSpecial()", 0));
    

    const BUPMsg::DumpEndMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::DumpEndMsg>(msg());
    const Config::CLUSTER tn(cmd->cpId(), true);
    execute();

    DataFile::BitVector& dv = DataFile::readDumpVector();
    dv.dumpEndQueue(false);
    DataFile::dumpEndQueued(128, tn);    // Clear DumpEnd mark for the sbc on this CP
    DataFile::dumpEndQueued(m_sbcId);    // Clear DumpEnd mark for the sbc on all CPs


}
//
//
// NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
// Backup Protocol Interaction
//
// virtual
//====================================================================
void DumpEndJob_V4::executeNonOp(u_int16 sbc) {

    newTRACE((LOG_LEVEL_INFO, "DumpEndJob_V4::executeNonOp(%d)", 0, sbc));


    // local variables used in response
    const BUPMsg::DumpEndMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::DumpEndMsg>(msg());
    u_int32 transNum = cmd->transNum(); 
    u_int16 tvm = cmd->version().major();

    try {
        // init stuff
        //===================================================================
        if(verifyVersion()) {

            CPID origCpId = cmd->cpId();
            Config::CLUSTER tCluster(origCpId, true);

            if (cmd->dumpResult() == BUPMsg::DUMP_OK) {

                // set source dir as current
                //string keyDir = Config::instance().tmpDir(cmd->key(), cpId);
                string keyDir = Config::instance().tmpDir(cmd->key(), tCluster);

                if(!cmd->key().valid() || !FileFunx::dirExists(keyDir.c_str())) {
                    throw BUPMsg::INVALID_KEY;
                }
                // if this throws it is "internal error", not missing dir
                TmpCurrDir currDir(keyDir.c_str());
                
                // get params
                ifstream inf(PARAMS_FILE);
                if(!inf.good()) {
                    EventReporter::instance().write("Failed to open parameter file.");
                }
                SBCId sbcId;
                u_int32 tmp32 = 0;
                inf >> sbcId >> tmp32;
                BUPDefs::EXTENT extent = static_cast<BUPDefs::EXTENT>(tmp32);
                TRACE((LOG_LEVEL_INFO, "DumpEndJob_V4::executeNonOp() Extent: %d, TransNum: %u, SbcId: %u ", 0, extent, transNum, sbcId.id()));
                m_sbcId = sbcId.id();
                inf.close();

                //
                // check for destination dir
                //===================================================================
                //string destDir = Config::instance().sbcDir(sbcId, cpId);
                string destDir = Config::instance().sbcDir(sbcId, tCluster);

#ifdef LOCAL_BUILD
                if(!FileFunx::dirExists(destDir.c_str())) {
#else
                    if (!EnvFMS::dirExists(sbcId, tCluster)) {
#endif

                    // is this an internal error? checked in DumpBegin, but now its gone
                    throw BUPMsg::SBC_NOT_FOUND; 
                }

                // Verify input files
                //=====================================================================
                // read & check buinfo
                auto_ptr<APBackupInfo> buinfo(new APBackupInfo("./"));
                BackupInfo_Base::BI_ERROR_CODE ec = buinfo->valid();
                if(ec != BackupInfo_Base::BEC_NO_ERROR)
                    throw BUPMsg::BAD_BACKUPINFO;            
                 
                try {
                    // clean dump dir (can't clean a dir we're in)
                    //SetCurrentDirectory(Config::instance().dataBusrvDir(cpId).c_str());
                    FileFunx::SetCurrentDirectoryX(Config::instance().dataBusrvDir(tCluster).c_str());

                    int failCnt = FileFunx::cleanDir(keyDir.c_str(), "*");
                    if (failCnt != 0) {
                        TRACE((LOG_LEVEL_ERROR, "Failed to delete %d no of files", 0, failCnt));
                        EventReporter::instance().write("Faild to delete files");
                    }
                }
                catch(LinuxException& x) {
                    EventReporter::instance().write(x); // log on AP, but OK to CP
                }
                catch(...) {
                    EventReporter::instance().write("Failed to clean dir");
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
        str << "DumpEndJob_V4, FMS Errorcode : " << x.errorCode() << ", Text: " << x.errorText() << ", Details: "
            << x.detailInfo() << endl;
        newTRACE((LOG_LEVEL_ERROR, "%s", 0, str.str().c_str()));
        m_exitcode = BUPMsg::INTERNAL_ERROR;
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
            case CodeException::BAD_BACKUPINFO:
                m_exitcode = BUPMsg::BAD_BACKUPINFO;
                break;
            case CodeException::REQ_SBC_MISSING:
                m_exitcode = BUPMsg::SBC_NOT_FOUND;
                break;
            case CodeException::BAD_DATA_FILE:
                m_exitcode = BUPMsg::BAD_DATAAREA;
                break;
            case CodeException::FMS_CREATE_FILE_FAILED:
                m_exitcode = BUPMsg::CREATE_DATAAREA_FAILED;
                break;
            case CodeException::FMS_PHYSICAL_FILE_ERROR:
                TRACE((LOG_LEVEL_ERROR, "DumpEndJob_V4::execute() Physical file error ", 0));
                m_exitcode = BUPMsg::FMS_PHYSICAL_FILE_ERROR;
                break;
            case CodeException::CP_APG43_CONFIG_ERROR:
                TRACE((LOG_LEVEL_ERROR, "DumpEndJob_V4::execute() CS error ", 0));
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

    // reset flag that dump is no longer on going
    //const BUPMsg::DumpEndMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::DumpEndMsg>(msg());
    const Config::CLUSTER tn(cmd->cpId(), true);
    DataFile::endDumping(tn);
    DataFile::endDumpingSBCID(tn);
    DataFile::BitVector& dv = DataFile::readDumpVector();
    dv.dumpEndQueue(false);

    //
    // create response in the msg buffer (re-use)
    //======================================================================
    msg().reset();
    new (msg().addr()) BUPMsg::DumpEndRspMsg(transNum, exitcode());

    TRACE((LOG_LEVEL_INFO, "DumpEndRspMsg: TransNum = %d exitcode = %d Version Major = %d", 0, transNum, exitcode(), tvm));

}

void DumpEndMoveFile::operator()(const string& src, const string& dest)
{
    newTRACE((LOG_LEVEL_INFO, "DumpEndMoveFile::operator()", 0));

    // Since the ftp still can be saving the transferred file, moving it can
    // result in a sharing violation. If so we must wait for a while and then try
    // again to move it.
    
    UMask aMask;

    DateTime srcTime;
    int srcExist = FileFunx::getWriteFileTime(src.c_str(), srcTime);
    if (srcExist) {
       if (isTRACEABLE()) {
          ostringstream st;
          st << srcTime;
          TRACE((LOG_LEVEL_INFO, "Source file is %s", 0, src.c_str()));
          TRACE((LOG_LEVEL_INFO, "Source time stamp is %s", 0, st.str().c_str()));
        }
    }

    int trial = 0;
    int result = EACCES;
    while (trial < 16 && result) {
        if((result = rename(src.c_str(), dest.c_str()))) {
            
            int lastError = errno;
            if (srcExist) {
                DateTime destTime;
                if (FileFunx::getWriteFileTime(dest.c_str(), destTime)) {
                    if (isTRACEABLE()) {
                       ostringstream st;
                       st << destTime;
                       TRACE((LOG_LEVEL_INFO, "Trial, Destination file is %s", 0, dest.c_str()));
                       TRACE((LOG_LEVEL_INFO, "Trial, Destinaton file time stamp is %s", 0, st.str().c_str()));
                    }
                    if (destTime == srcTime) {
                        return;
                    }
                }
            }

            //if (lastError == ERROR_SHARING_VIOLATION || lastError == ERROR_ACCESS_DENIED)
            // ENOENT     No such path or directory
            // EACCES     Permission denied
            if (lastError == ENOENT || lastError == EACCES) {
                ostringstream st;
                st << "Failed to move dump file " << src
                   << " inside trial loop; Linux error code is " << lastError;
                EventReporter::instance().write(st.str().c_str());
                TRACE((LOG_LEVEL_ERROR, "%s", 0, st.str().c_str()));
                if (trial++ < 5 )
                    sleep(10);    // 10 sec
                else sleep(6);    // 6 sec
            }
            else {
                TRACE((LOG_LEVEL_ERROR, "Failed to move dump file %s; Linux error code is %d", 0, src.c_str(), lastError));
                THROW_LINUX_MSG("Failed to move dump file " << src << " to " << dest << " Linux error code is " << lastError);
            }
        }
    }
    
    if (result) {
        TRACE((LOG_LEVEL_ERROR, "Failed to move dump file after trial %s to %s", 0, src.c_str(), dest.c_str()));
        THROW_LINUX_MSG("Failed to move dump file after trial " << src << " to " << dest);
    }
    else {
        chmod(dest.c_str(), 0666);
        DateTime destTime;
        if (FileFunx::getWriteFileTime(dest.c_str(), destTime)) {
            if (isTRACEABLE()) {
                ostringstream st;
                st << "Destination file <" << dest.c_str() << "> at time stamp " << destTime;
                TRACE((LOG_LEVEL_INFO, "%s", 0, st.str().c_str()));
            }
        }        
        //FileFunx::chown(dest.c_str());        
    }

}
