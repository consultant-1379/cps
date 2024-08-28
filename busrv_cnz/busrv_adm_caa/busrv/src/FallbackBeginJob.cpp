/*
NAME
    File_name:FallbackBeginJob.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2002-2023. All rights reserved.

The Copyright to the computer program(s) herein is the property of Ericsson
Utvecklings AB, Sweden.
The program(s) may be used and/or copied only with the written permission from
Ericsson Utvecklings AB or in accordance with the terms and conditions
stipulated in the agreement/contract under which the program(s) have been
supplied.

DESCRIPTION
    Executes the BUP command FALLBACK_BEGIN.

DOCUMENT NO
    190 89-CAA 109 0387

AUTHOR
    2002-05-02 by U/Y/SF Anders Gillgren (qabgill, tag :ag)

SEE ALSO


Revision history
----------------
2002-05-02 qabgill  Created
2002-10-31 uablan    Added ACS_Trace
2003-01-08 uablan   Added functionality for fallback selection of SBC
2003-01-21 uablan   Added setting of loadingInProgress flag in data file.
2006-03-01 uablan   Remove setting of LoadInProgress flag. (HG62821)
2006-06-09 uablan   Added CP-AP link speed in return msg
2007-03-05 uablan   Update for Multi CP System
2010-04-01 xdtthng  Update for SSI
2011-04-23 xdtthng  Update for 43L
2023-01-01 xkomala  Added checks for valid cpid for TR IA20800
 */


#include "Config.h"
#include "FallbackBeginJob.h"

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
#include "FallbackMgr.h"
#include "Key.h"
#include "TmpCurrDir.h"
#include "SystemInfo.h"
#include "LinuxException.h"

#ifndef LOCAL_BUILD                    
#include "EnvFMS.h"
#include "fms_cpf_file.h"
#endif

#include <fstream>

using namespace buap;
//using namespace SystemInfo;

//
// virtual
//====================================================================
void FallbackBeginJob::execute() {

	newTRACE((LOG_LEVEL_INFO, "FallbackBeginJob::execute()", 0));

	// local variables used in response
	Key key(false);
	string path;
	u_int32 speed = SystemInfo::DEFAULT_BANDWIDTH;

	m_exitcode = BUPMsg::OK;
	SBCId sbcId;
	bool lastPossSBCLoad = false;
	CPID cpId = 0xFFFF; // default ONE CP system
	bool useLDD1 = false;
	const BUPMsg::FallbackBeginMsg* constCmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::FallbackBeginMsg>(msg());
	BUPMsg::FallbackBeginMsg* cmd = const_cast<BUPMsg::FallbackBeginMsg*>(constCmd);

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
					TRACE((LOG_LEVEL_ERROR, "FallbackBeginJob, CpId received is <%d> ,throwing internal error", 0, cpId));
					throw BUPMsg::INTERNAL_ERROR;
				} //TR_IA20800 END

			}

			// Only One CP System for now
			// Check if SPX. SPX must always load.
			if ( cmd->cpSystem() != BUPMsg::ONE_CP_SYSTEM && cpId < 1000 ) {

				// Reset Load flag
				if (DataFile::loading(cpId))
					DataFile::endLoading(cpId);

				// Check if job shall be started or if already to many dumps are ongoing
				if (Config::instance().noOngoingLoad() == Config::instance().maxNoOfSimLoad())
					throw BUPMsg::MAX_NO_DUMP_LOAD_ONGOING;
			}

			// create busrv directory structure for this CP
			// this done because pior to initial load a reboot or start of the VM is done /// Check with ATa!!!!!!!
			Config::instance().createDirStructure(cpId);

			// Set flag in busrv.dat that loading is ongoing
			DataFile::startLoading(cpId);

			// check rules
			//===================================================================
			// select SBC here!
			//------------------
			FallbackMgr fallbMgr(cpId);
			sbcId.id(fallbMgr.currLoadId(cpId));
			TRACE((LOG_LEVEL_INFO, "select of sbc : %d", 0, sbcId.id()));
			// save SBC id into the cmd
			cmd->sbcId(sbcId);

			// save new lastLoadedSBC
			DataFile::lastLoadedSBC(sbcId.id(), cpId);
			// save new lastloadTime
			fallbMgr.updateLoadTime(cpId);
			// check if this is the last possible SBC to load
			lastPossSBCLoad = fallbMgr.lastLoadableSBC(cpId);

			// Logging fallback attempt to AP Event log
			DateTime fallbackTime = DataFile::lastLoadedTimeForFallback(cpId);
			ostringstream st;
			st << "Fallback attempt of RELFSW" << sbcId.id() << " at " << fallbackTime;
			if (cmd->cpSystem() != BUPMsg::ONE_CP_SYSTEM)
				st << " cpId <" << cpId << ">";

			TRACE((LOG_LEVEL_INFO, "FallbackMgr::updateLoadTime() logs <%s> to AP event log", 0, st.str().c_str()));
			EventReporter::instance().write(st.str().c_str());

			//-----------------
			// select SBC here!

			string destDir = Config::instance().sbcDir(sbcId, cpId);
			// make sure the RELFSWx exists
#ifdef LOCAL_BUILD                    
			if(!FileFunx::dirExists(destDir.c_str()))
#else
				if (!EnvFMS::dirExists(sbcId, cpId))
#endif 
					throw BUPMsg::SBC_NOT_FOUND;

			APBackupInfo buinfo(Config::instance().sbcDir(sbcId, cpId).c_str());

			BackupInfo_V4::BI_ERROR_CODE ec = buinfo.valid();
			if(ec != BackupInfo_V4::BEC_NO_ERROR)
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
			path = Config::instance().ftpLoadDir(sbcId, cpId);
			if(path.length() > Config::FTP_PATH_MAX_DIR_LEN)
				THROW_XCODE("Path to ftp virtual directory is too long", CodeException::CONFIGURATION_ERROR)

				// 1.3 get (local) path to tmp dir based on key & create the directory
				FileFunx::createDirX(Config::instance().tmpDir(key, cpId).c_str());
			TmpCurrDir currDir(Config::instance().tmpDir(key, cpId).c_str());
			BUPFunx::writeParams(cmd);
		}
	}
	//
	// catch all error & set exitcode
	//
	catch(BUPMsg::ERROR_CODE& exitcode) {
		TRACE((LOG_LEVEL_ERROR, "Exiting FallbackBegin w. error code: %d", 0, exitcode));
		m_exitcode = exitcode;
	}
#ifndef LOCAL_BUILD                    
	catch(FMS_CPF_Exception& x) {
		stringstream str;
		str << "FallbackBeginJob, FMS Errorcode : " << x.errorCode() << ", Text: " << x.errorText() << ", Details: "
				<< x.detailInfo() << endl;
		newTRACE((LOG_LEVEL_ERROR, "%s", 0, str.str().c_str()));
		m_exitcode = BUPMsg::INTERNAL_ERROR;

		// Remove this logging for final release
		EventReporter::instance().write(str.str().c_str());
	}
#endif
	catch(const CodeException& x) {
		switch(x.errcode()) {
		case CodeException::BAD_BACKUPINFO: // fall thru
		case CodeException::BUINFO_MISSING: // fall thru
		case CodeException::BUINFO_ACCESS_DENIED:
			m_exitcode = BUPMsg::BAD_BACKUPINFO;
			break;
		case CodeException::CP_APG43_CONFIG_ERROR:
			TRACE((LOG_LEVEL_ERROR, "FallbackBeginJob::execute() CS error ", 0));
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

	//Get current link speed between AP-CP
	if (SystemInfo::getCurrentCPAPBandWidth(speed) != 0) {
		TRACE((LOG_LEVEL_WARN, "Failed to retrive CP-AP link speed. Use default speed = %d", 0, speed));
	}
	else {
		TRACE((LOG_LEVEL_INFO, "CP-AP link speed = %d", 0, speed));
	}

	msg().reset();
	FtpFileSet files;
	BUPFunx::setFiles(files, ALL, path.c_str(), useLDD1);

	// create response in the msg buffer (re-use)
	new (msg().addr()) BUPMsg::FallbackBeginRspMsg(transNum, exitcode(), key, sbcId, lastPossSBCLoad, files, speed);

	TRACE((LOG_LEVEL_INFO, "FallbackBeginRspMsg: TransNum = %d exitcode = %d Version Major = %d", 0, transNum, exitcode(), tvm));
	if (!exitcode())
		TRACE((LOG_LEVEL_INFO, "Reload with SBC : %d", 0, sbcId.id()));
}

///////////////////////////////////////////////////////////////////////////////
//
// virtual
//====================================================================
void FallbackBeginJob_V4::execute() {

	newTRACE((LOG_LEVEL_INFO, "FallbackBeginJob_V4::execute()", 0));

	// local variables used in response
	Key key(false);
	string path;
	u_int32 speed = SystemInfo::DEFAULT_BANDWIDTH;
	//bool useLDD1 = false;    // do not need this any more
	u_int32 useSubFiles = 0;

	m_exitcode = BUPMsg::OK;
	SBCId sbcId;
	bool lastPossSBCLoad = false;
	CPID  cpId;
	const Config::CLUSTER tc;

	const BUPMsg::FallbackBeginMsg* constCmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::FallbackBeginMsg>(msg());
	BUPMsg::FallbackBeginMsg* cmd = const_cast<BUPMsg::FallbackBeginMsg*>(constCmd);
	u_int32 transNum = cmd->transNum(); // save for response
	u_int16 tvm = cmd->version().major();

	try {
		// init stuff
		//===================================================================
		if(verifyVersion()) {

			cpId = cmd->cpId();

 
			//TR_IA20800 START
			if(!(Config::instance().isValidBladeCpid(cpId)))
			{
				TRACE((LOG_LEVEL_ERROR, "FallbackBeginJob_V4, CpId received is <%d> , throwing internal error", 0, cpId));
				throw BUPMsg::INTERNAL_ERROR;
			} //TR_IA20800 END


			const Config::CLUSTER tn(cpId, true);

			// ------------
			// APG43L note
			// ------------
			// The check is not really needed. Version 4 of the BUP protocol instantiates
			// this object. Thus it is not for the case of SPX.
			// Remove this note after BT and DJT

			// Check if SPX. SPX must always load.
			//if ( cmd->cpSystem() != BUPMsg::ONE_CP_SYSTEM && cpId < 1000 ) {
			// Reset Load flag
			if (FileFunx::dirExists(Config::instance().datFile(tn).c_str()))
				DataFile::endLoading(tn);
			if (FileFunx::dirExists(Config::instance().datFile(tc).c_str()))
				DataFile::endLoading(tc);

			// Check if job shall be started or if already to many dumps are ongoing
			if (Config::instance().noOngoingLoad(tn) >= Config::instance().maxNoOfSimLoad())
				throw BUPMsg::MAX_NO_DUMP_LOAD_ONGOING;
			Config::instance().noOngoingDump(tn);
			//}

			// create busrv directory structure for this CP
			// this done because pior to initial load a reboot or start of the VM is done /// Check with ATa!!!!!!!
			Config::instance().createDataBusrvDirStructure(tn);
			Config::instance().createFtpDirStructure(Config::instance().tmpRoot(tn).c_str());

			Config::instance().createDataBusrvDirStructure(tc);
			Config::instance().createFtpDirStructure(Config::instance().tmpRoot(tc).c_str());

			// check rules
			//===================================================================
			// select SBC here!
			//------------------
			FallbackMgr_V4 fallbMgr(tn);
			sbcId.id(fallbMgr.currLoadId(tn));
			TRACE((LOG_LEVEL_INFO, "select of sbc : %d", 0, sbcId.id()));
			u_int16 loadingSbcId = sbcId.id();
			cmd->sbcId(sbcId);

			// NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
			// Backup Protocol Interaction
			//
			// Check if any dumping on the same sbc in progress
			DataFile::BUACTREC dump;
			DataFile::BUACTREC_ITERATOR it;

			DataFile::BitVector& dumpVector = DataFile::readDumpVector();
			if (dumpVector != 0ULL) {
				DataFile::onGoingDump(dump);

				// Loading from the sbc having manual dump in progress is rejected
				if (loadingSbcId)
					for (it = dump.begin(); it != dump.end(); ++it) {
						if (it->first == loadingSbcId &&
								it->second != cpId)
							throw BUPMsg::LOAD_REJECTED_DUE_TO_ONGOING_DUMP;
					}
			}
			//
			// NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN

			// Set flag in busrv.dat that loading is ongoing
			DataFile::startLoading(tc);
			DataFile::startLoading(tn);
			DataFile::startLoadingSBCID(sbcId, tn);

			// Mark in core loading activity
			DataFile::setLoading(cpId, tc);

			// save new lastLoadedSBC
			DataFile::lastLoadedSBC(sbcId.id(), tn);
			DataFile::lastLoadedSBC(sbcId.id(), tc);
			DataFile::lastLoadedSbcInfo(sbcId.id(), tc);

			// save new lastloadTime
			fallbMgr.updateLoadTime(tn);
			//fallbMgr.updateLoadTime(tc);
			// check if this is the last possible SBC to load
			lastPossSBCLoad = fallbMgr.lastLoadableSBC(sbcId.id());

			// Logging fallback attempt to AP Event log
			DateTime fallbackTime = DataFile::lastLoadedTimeForFallback(tn);
			ostringstream st;
			st << "Fallback attempt of RELFSW" << sbcId.id() << " at " << fallbackTime;
			st << " cpId <" << cpId << ">";
			TRACE((LOG_LEVEL_INFO, "FallbackMgr_V4::updateLoadTime() logs <%s> to AP event log", 0, st.str().c_str()));
			EventReporter::instance().write(st.str().c_str());

			//-----------------
			// select SBC here!

			// make sure the RELFSWx exists
			string destDir = Config::instance().sbcDir(sbcId,tc);

#ifdef LOCAL_BUILD    
			if(!FileFunx::dirExists(destDir.c_str()))
#else
				if (!EnvFMS::dirExists(sbcId, tc))
#endif
					throw BUPMsg::SBC_NOT_FOUND;

			APBackupInfo buinfo(destDir.c_str());

			BackupInfo_Base::BI_ERROR_CODE ec = buinfo.valid();
			if(ec != BackupInfo_V4::BEC_NO_ERROR)
				throw BUPMsg::BAD_BACKUPINFO;

			Version buVersion = buinfo.getVersion();

			// UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU
			// Tool Support
			// Allow loading of BUINFO Version 3 and Version 4 Under APZ Profile 100
			//
			// Under APZ Profile == 100, BUP expects BUINFO Version 4 under normal operating
			// conditions. This enhancement allows Old Tool using BUINFO Version 3 to be
			// loaded under APZ Profile == 100

			// Use LDD1 only if we don't have an LDD2
			// Either marker file or marker in BUINFO exsits, only check the disk
			// and ignore BUINFO information about LDD1 and LDD2

			//string marker = destDir + Config::instance().MarkerFileName;
			u_int32 admarker = buinfo.getAdMarker();
			bool adFileMarker = EnvFMS::fileExists(sbcId, Config::instance().MarkerFileName, tc);
			TRACE((LOG_LEVEL_INFO, "BUINFO AdMarker = <%d>, File ADMARKER = <%d>", 0, admarker, adFileMarker));

			if ((adFileMarker && admarker) || (!adFileMarker && !admarker)) {
				// Use the info from BUINFO
				if (buVersion >= Version(4, 0)) {
					// use LDD1 only if we don't have an LDD2
					BackupInfo_Base::Info_V4 *pinfo = buinfo.info_V4();
					if(pinfo->ldd2SectorInfo.outputTime().null())
						useSubFiles |= Config::useLDD1;
					// use PS only if we don't have an PS2, the same goes for RS/RS2
					if(pinfo->ps2SectorInfo.outputTime().null())
						useSubFiles |= Config::usePS;
					if( pinfo->rs2SectorInfo.outputTime().null())
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
			else { // If only one of the marker exists, information from BUINFO is not used
				// to select LDD. In this case the existence of the physical file LDD is
				// used for selecting LDD
				if (buVersion >= Version(4, 0)) {
					// The existence of the physical file LDD1 or LDD2 is used for selecting LDD sub file
					if(!EnvFMS::fileExists(sbcId, SBCFile::name(SBCFile::LDD2), tc))
					{
						useSubFiles |= Config::useLDD1;
					}
					// use PS only if we don't have an PS2, the same goes for RS/RS2
					if (!EnvFMS::fileExists(sbcId, SBCFile::name(SBCFile::PS2), tc))
					{
						useSubFiles |= Config::usePS;
					}
					if (!EnvFMS::fileExists(sbcId, SBCFile::name(SBCFile::RS2), tc))
					{
						useSubFiles |= Config::useRS;
					}
				}
				// BUINFO Version 3
				else {
					// Always use RS and PS for BUINFO Version 3
					useSubFiles |= Config::usePS | Config::useRS;

					if (!EnvFMS::fileExists(sbcId, SBCFile::name(SBCFile::LDD2), tc))
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
			path = Config::instance().ftpLoadDir(sbcId, tn);
			if(path.length() > Config::FTP_PATH_MAX_DIR_LEN)
				THROW_XCODE("Path to ftp virtual directory is too long", CodeException::CONFIGURATION_ERROR)

				// 1.3 get (local) path to tmp dir based on key & create the directory
				FileFunx::createDirX(Config::instance().tmpDir(key, tn).c_str());
			TmpCurrDir currDir(Config::instance().tmpDir(key, tn).c_str());
			BUPFunx::writeParams(cmd);
		}
	}
	//
	// catch all error & set exitcode
	//
	catch(BUPMsg::ERROR_CODE exitcode) {
		TRACE((LOG_LEVEL_ERROR, "Exiting FallbackBegin_V4 with error code: %d", 0, exitcode));
		m_exitcode = exitcode;
	}
#ifndef LOCAL_BUILD                    
	catch(FMS_CPF_Exception& x) {
		stringstream str;
		str << "FallbackBeginJob_V4, FMS Errorcode : " << x.errorCode() << ", Text: " << x.errorText() << ", Details: "
				<< x.detailInfo() << endl;
		newTRACE((LOG_LEVEL_ERROR, "%s", 0, str.str().c_str()));
		m_exitcode = BUPMsg::INTERNAL_ERROR;

		// Remove this logging for final release
		EventReporter::instance().write(str.str().c_str());
	}
#endif
	catch(const CodeException& x) {
		switch(x.errcode()) {
		case CodeException::BAD_BACKUPINFO: // fall thru
		case CodeException::BUINFO_MISSING: // fall thru
		case CodeException::BUINFO_ACCESS_DENIED:
			m_exitcode = BUPMsg::BAD_BACKUPINFO;
			break;
		case CodeException::CP_APG43_CONFIG_ERROR:
			TRACE((LOG_LEVEL_ERROR, "FallbackBeginJob::execute() CS error ", 0));
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

	//Get current link speed between AP-CP
	if (SystemInfo::getCurrentCPAPBandWidth(speed) != 0) {
		TRACE((LOG_LEVEL_WARN, "Failed to retrive CP-AP link speed. Use default speed = %d", 0, speed));
	}
	else {
		TRACE((LOG_LEVEL_INFO, "CP-AP link speed = %d", 0, speed));
	}

	msg().reset();
	FtpFileSet_V4 files;
	BUPFunx::setFiles(files, ALL, path.c_str(), useSubFiles);
	new (msg().addr()) BUPMsg::FallbackBeginRspMsg_V4(transNum,
			exitcode(), key, sbcId, lastPossSBCLoad, files, speed);

	TRACE((LOG_LEVEL_INFO, "FallbackBeginRspMsg_V4: TransNum = %d exitcode = %d Version Major = %d", 0, transNum, exitcode(), tvm));
	if (!exitcode())
		TRACE((LOG_LEVEL_INFO, "Reload with SBC: %d, last loaded SBC: %d", 0, sbcId.id(), lastPossSBCLoad));
}
