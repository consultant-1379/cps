/*
NAME
   File_name:DumpBeginJob.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2002-2023. All rights reserved.

   The Copyright to the computer program(s) herein is the property of Ericsson
   Utvecklings AB, Sweden.
   The program(s) may be used and/or copied only with the written permission from
   Ericsson Utvecklings AB or in accordance with the terms and conditions
   stipulated in the agreement/contract under which the program(s) have been
   supplied.

DESCRIPTION
   Executes the BUP command DUMP_BEGIN.

DOCUMENT NO
   190 89-CAA 109 0387

AUTHOR
   2002-02-05 by UAB/KY/SK Anders Gillgren (qabgill, tag :ag)
   2010-04-15 xdtthng SSI added
   2011-09-14 xdtthng 43L, Total usage of FMS

SEE ALSO
   

Revision history
----------------
2002-02-05 qabgill Created
2002-10-20 uabmnst Add ACSTrace
2006-06-09 uablan  Added CP-AP link speed in return msg
2010-04-15 xdtthng SSI added
2011-09-14 xdtthng 43L, Total usage of FMS
2023-01-01 xkomala Added checks for valid cpid for TR IA20800

*/


#include "buap.h"
#include "Config.h"
#include "DumpBeginJob.h"
#include "BUPMsg.h"
#include "BUPFunx.h"
#include "CodeException.h"
#include "CPS_BUSRV_Trace.h"
#include "DataFile.h"
#include "EventReporter.h"
#include "FileFunx.h"
#include "FtpFileSet.h"
#include "FtpPath.h"
#include "Key.h"
#include "SBCId.h"
#include "SBCFile.h"
#include "TmpCurrDir.h"
#include "SystemInfo.h"
#include "LinuxException.h"
#include "APBackupInfo.h"

#ifndef LOCAL_BUILD					
#include "EnvFMS.h"
#include "fms_cpf_file.h"
#endif

#include <fstream>

using namespace buap;

//
//
//====================================================================
void DumpBeginJob::execute() {
	
	newTRACE((LOG_LEVEL_INFO, "DumpBeginJob::execute()", 0));

	// local variables used in response
	Key key(false);
	string ftpDumpDir;
	u_int32 speed = SystemInfo::DEFAULT_BANDWIDTH;
	CPID cpId = 0xFFFF; // default ONE CP system
	const BUPMsg::DumpBeginMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::DumpBeginMsg>(msg());
	u_int32 transNum = cmd->transNum(); // save for response
	u_int16 tvm = cmd->version().major();
	SBCId sbcId = cmd->sbcId();

	BUPDefs::EXTENT extent = INVALID_EXTENT;
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
					TRACE((LOG_LEVEL_ERROR, "DumpBeginJob, CpId received is <%d> ,throwing internal error", 0, cpId));
					throw BUPMsg::INTERNAL_ERROR;
				} //TR_IA20800 END

			}

			// Check if SPX. SPX must always dump.
			if ( cmd->cpSystem() != BUPMsg::ONE_CP_SYSTEM && cpId < 1000 ) {
				// Reset dump flag
				if (DataFile::dumping(cpId))
					DataFile::endDumping(cpId);

				// Check if job shall be started or if already to many dumps are ongoing
				if (Config::instance().noOngoingDump() >= Config::instance().maxNoOfSimDump())
					throw BUPMsg::MAX_NO_DUMP_LOAD_ONGOING;
			}

			// create directory structure for this CP
			Config::instance().createDirStructure(cpId);

			// Set flag in busrv.dat that dumping is ongoing
			DataFile::startDumping(cpId);
			
			//TRACE(("Extent: %d, TransNum: %u, SbcId: %u ", 0, cmd->extent(), transNum, cmd->sbcId()));
			// check rules
			//===================================================================
			if(!cmd->sbcId().valid())
				throw BUPMsg::PARAM_BACKUPID_OUT_OF_RANGE;
			extent = cmd->extent();
			TRACE((LOG_LEVEL_INFO, "DumpBeginJob; Extent = %d", 0, extent));
			
			if(extent != BUPDefs::SMALL && extent != BUPDefs::SMALL_AND_LARGE &&
				extent != BUPDefs::ALL)
				throw BUPMsg::PARAM_EXTENT_INVALID;
				
			if(extent != ALL && cmd->sbcId() != SBCId((u_int16)0))
				throw BUPMsg::PARAM_ONLY_SBC0_FOR_PARTIAL_DUMP;

                        string destDir = Config::instance().sbcDir(cmd->sbcId(), cpId);
			// SBC must exist
#ifdef LOCAL_BUILD					
			if (Config::instance().isMultipleCP()) {
				if(!FileFunx::dirExists(destDir.c_str()))
					throw BUPMsg::SBC_NOT_FOUND;
			}
			else {
				if(!FileFunx::dirExists(Config::instance().sbcDir(cmd->sbcId()).c_str()))
					throw BUPMsg::SBC_NOT_FOUND;
			}
#else
			if (!EnvFMS::dirExists(sbcId, cpId))
				throw BUPMsg::SBC_NOT_FOUND;
#endif

                        // Handling of failed auto dump
                        //string marker = destDir + Config::instance().MarkerFileName;
                        if (extent != ALL) {
                            APBackupInfo buinfo(destDir.c_str());
                            u_int32 admarker = buinfo.getAdMarker();
                            //bool adFileMarker = fileExists(marker.c_str());
                            bool adFileMarker = EnvFMS::fileExists(cmd->sbcId(), Config::instance().MarkerFileName, cpId);
                            TRACE((LOG_LEVEL_INFO, "BUINFO AdMarker = <%d>, File ADMARKER = <%d>", 0, admarker, adFileMarker));
                            if (adFileMarker || admarker) {
                                BUPMsg::ERROR_CODE err = admarker ?
                                         BUPMsg::AUTODUMP_FAILED_BUINFO_CORRECTED :
                                         BUPMsg::AUTODUMP_FAILED_BUINFO_NOT_CORRECTED;

                            ostringstream os;
                            os << "Auto dump extent <" << extent
                               << "> aborted with exit code <"
                               << err << "> during DUMP_BEGIN primitive";
                            TRACE((LOG_LEVEL_INFO, "%s", 0, os.str().c_str()));

                            EventReporter::instance().write(os.str().c_str());
                            throw err;
                       }
                       // Checking the consistency between BUINFO and physical files
                if ((buinfo.info()->sddSectorInfo.nofSegments() > 0) !=
                	EnvFMS::fileExists(sbcId, SBCFile::name(SBCFile::SDD), cpId)) {
                	TRACE((LOG_LEVEL_INFO, "BUINFO inconsistent with SDD physical file", 0));
                	throw BUPMsg::BAD_BACKUPINFO;
                }

                if ((buinfo.info()->ldd1SectorInfo.nofSegments() > 0) !=
                	EnvFMS::fileExists(sbcId, SBCFile::name(SBCFile::LDD1), cpId)) {
                	TRACE((LOG_LEVEL_INFO, "BUINFO inconsistent with LDD1 physical file", 0));
                	throw BUPMsg::BAD_BACKUPINFO;
                }

                if ((buinfo.info()->ldd2SectorInfo.nofSegments() > 0) !=
                	EnvFMS::fileExists(sbcId, SBCFile::name(SBCFile::LDD2), cpId)) {
                	TRACE((LOG_LEVEL_INFO, "BUINFO inconsistent with LDD2 physical file", 0));
                	throw BUPMsg::BAD_BACKUPINFO;
                }

                if ((buinfo.info()->rsSectorInfo.nofSegments() > 0) !=
                	EnvFMS::fileExists(sbcId, SBCFile::name(SBCFile::RS), cpId)) {
                	TRACE((LOG_LEVEL_INFO, "BUINFO inconsistent with RS physical file", 0));
                	throw BUPMsg::BAD_BACKUPINFO;
                }

                if ((buinfo.info()->psSectorInfo.nofSegments() > 0) !=
                	EnvFMS::fileExists(sbcId, SBCFile::name(SBCFile::PS), cpId)) {
                	TRACE((LOG_LEVEL_INFO, "BUINFO inconsistent with PS physical file", 0));
                	throw BUPMsg::BAD_BACKUPINFO;
                }


                        }
			// 1. execute command
			// 1.1 create new key
			//===================================================================
			key.newKey();
			
			// 1.2 create "virtual" path for response
			ftpDumpDir = Config::instance().ftpDumpDir(key, cpId);
			if(ftpDumpDir.length() > Config::FTP_PATH_MAX_DIR_LEN) {
				THROW_XCODE("Path to ftp virtual directory is too long", CodeException::CONFIGURATION_ERROR)
			}

			// 1.3 get (local) path to tmp dir based on key & create the directory
			string tmpDir = Config::instance().tmpDir(key, cpId);
			FileFunx::createDirX(tmpDir.c_str(), 0777);
			TmpCurrDir currDir(tmpDir.c_str());
			BUPFunx::writeParams(cmd);
		}
	}
	//
	// catch all error & set exitcode
	//
	catch(BUPMsg::ERROR_CODE& exitcode) {
		m_exitcode = exitcode;
	}
#ifndef LOCAL_BUILD					
	catch(FMS_CPF_Exception& x) {
		stringstream str;
		str << "DumpBeginJob, FMS Errorcode : " << x.errorCode() << ", Text: " << x.errorText() << ", Details: "
		    << x.detailInfo() << endl;
		newTRACE((LOG_LEVEL_INFO, "%s", 0, str.str().c_str()));
		m_exitcode = BUPMsg::INTERNAL_ERROR;
		EventReporter::instance().write(str.str().c_str());		
	}
#endif
	catch(CodeException& x) {
		switch(x.errcode()) {
			case CodeException::CP_APG43_CONFIG_ERROR:
				TRACE((LOG_LEVEL_ERROR, "DumpBeginJob::execute() CS error ", 0));
				m_exitcode = BUPMsg::INTERNAL_ERROR;
				break;
			default:
				EventReporter::instance().write(x);
				m_exitcode = BUPMsg::INTERNAL_ERROR;
				break;

		}
	}
	catch(const Exception& x) {
		EventReporter::instance().write(x);
		m_exitcode = BUPMsg::INTERNAL_ERROR;
	}
	catch(...) {
		EventReporter::instance().write("Unhandled internal error.");
		m_exitcode = BUPMsg::INTERNAL_ERROR;
	}

	msg().reset();
	FtpFileSet files;
	if(m_exitcode == BUPMsg::OK)
		BUPFunx::setFiles(files, extent, ftpDumpDir, true);

	//Get current link speed between AP-CP
	if (SystemInfo::getCurrentCPAPBandWidth(speed) != 0) {
		TRACE((LOG_LEVEL_WARN, "Failed to retrive CP-AP link speed. Use default speed = %d", 0, speed));
	}
	else {
		TRACE((LOG_LEVEL_INFO, "CP-AP link speed = %d", 0, speed));
	}

	new (msg().addr()) BUPMsg::DumpBeginRspMsg(transNum, exitcode(), key, files, speed);
	TRACE((LOG_LEVEL_INFO, "DumpdBeginRsp: TransNum = %d exitcode = %d Version Major = %d", 0, transNum, exitcode(), tvm));
}


///////////////////////////////////////////////////////////////////////////////
//
//
//====================================================================
void DumpBeginJob_V4::execute() {
    
    newTRACE((LOG_LEVEL_INFO, "DumpBeginJob_V4::execute()", 0));

    // local variables used in response
    Key key(false);
    string ftpDumpDir;
    u_int32 speed = SystemInfo::DEFAULT_BANDWIDTH;
    
    BUPDefs::EXTENT extent = INVALID_EXTENT;
    const BUPMsg::DumpBeginMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::DumpBeginMsg>(msg());
    u_int32 transNum = cmd->transNum(); 
	u_int16 tvm = cmd->version().major();

    try {
		//
        //===================================================================
        if(verifyVersion()) {
            CPID dumpingCpId = cmd->cpId();

            if(!(Config::instance().isValidBladeCpid(dumpingCpId)))
            {
            	TRACE((LOG_LEVEL_ERROR, "DumpBeginJob_V4, received CpId is <%d> ,throwing internal error", 0, dumpingCpId));
            	throw BUPMsg::INTERNAL_ERROR;
            }

			const Config::CLUSTER tCluster(dumpingCpId, true);
 			const Config::CLUSTER tc;

            // create directory structure for this Cluster
			Config::instance().createDataBusrvDirStructure(tc);
 			Config::instance().createFtpDirStructure(Config::instance().tmpRoot(tc).c_str());

			u_int16 dumpingSbcId = cmd->sbcId().id();

			int noDumps = Config::instance().noOngoingDump(tCluster);
			Config::instance().noOngoingLoad(tCluster);

			// Check rules
			// ---------------------------------------------------
            if(!cmd->sbcId().valid())
                throw BUPMsg::PARAM_BACKUPID_OUT_OF_RANGE;


            // NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
            // Backup Protocol Interaction
			// ----------------------------------------------------
			
			// All timeout dumps and loads, if exist, are reset at this point
			// This will clear hanging dump/load
			DataFile::BUACTREC load;
            DataFile::BUACTREC dump;
            DataFile::BUACTREC_ITERATOR it;
			DataFile::BitVector& loadVector = DataFile::readLoadVector();
			DataFile::BitVector& dumpVector = DataFile::readDumpVector();

			// Checking for on going dump and on going load is time consuming
			// It is avoided as much as possible
			if (loadVector != 0ULL && dumpVector != 0ULL)
				DataFile::onGoingDumpLoad(load, dump); 
			else if (loadVector != 0ULL)
				DataFile::onGoingLoad(load);
			else
				DataFile::onGoingDump(dump);
                       
			// For sbc != 0
			if (loadVector != 0ULL || dumpVector != 0ULL) {
				if (dumpingSbcId) {
					// Manual dumping to the sbc being loaded is rejected
					for (it = load.begin(); it != load.end(); ++it) {
						if (it->first == dumpingSbcId &&
							it->second != dumpingCpId)
							throw BUPMsg::DUMP_REJECTED_DUE_TO_ONGOING_LOAD;
					}
					// Only one blade can have write access to an sbc != 0
					// Multiple write request will be rejected for blade range
					// Multiple write request is accepted for cluster range
					for (it = dump.begin(); it != dump.end(); ++it) {
						if (it->first == dumpingSbcId) {
							if (it->second != dumpingCpId) {
								if (Config::instance().isSbcBladeRange(dumpingSbcId))
									throw BUPMsg::DUMP_REJECTED_DUE_TO_ONGOING_DUMP_ON_SAME_SBCID;
							}
							// If the same CP dumps again while previsous dump has 
							// not yet finished, clears all marks and accept new dumping
							// Multiple dumps on the same sbc are accepted. 
							// Only the last dump comes into effect	
							
							const Config::CLUSTER tn(it->second, true);
							DataFile::endDumping(tn);
							DataFile::endDumpingSBCID(tn);
							DataFile::clearDumping(dumpingCpId, tc);
							--noDumps;
						}
					}
				}
			}
			//
            // NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN


			// Check if job shall be started or if already to many dumps are ongoing
            // Also reset Dump mark if timeout
			if (noDumps >= Config::instance().maxNoOfSimDump())
				throw BUPMsg::MAX_NO_DUMP_LOAD_ONGOING;

            // create directory structure for this CP
			Config::instance().createDataBusrvDirStructure(tCluster);
 			Config::instance().createFtpDirStructure(Config::instance().tmpRoot(tCluster).c_str());
            
            TRACE((LOG_LEVEL_INFO, "Extent: %d, TransNum: %u, SbcId: %u ", 0, cmd->extent(), transNum, cmd->sbcId().id()));

			// Set flag in busrv.data that the dumping for a specific SBC is ongoing
            DataFile::startDumping(tCluster);
			DataFile::startDumpingSBCID(cmd->sbcId(), tCluster);

			DataFile::setDumping(dumpingCpId, tc);

			extent = cmd->extent();
            if(extent != BUPDefs::NEW_SMALL_AND_LARGE &&
                extent != BUPDefs::ALL)
                throw BUPMsg::PARAM_EXTENT_INVALID;
            if(extent != ALL && cmd->sbcId().id() != 0)
                throw BUPMsg::PARAM_ONLY_SBC0_FOR_PARTIAL_DUMP;

            //SBC must exist
            //if(!FileFunx::dirExists(Config::instance().sbcDir(cmd->sbcId(), cpId).c_str()))
            string destDir = Config::instance().sbcDir(cmd->sbcId(), tCluster);
#ifdef LOCAL_BUILD
            if(!FileFunx::dirExists(destDir.c_str()))
#else
            if (!EnvFMS::dirExists(cmd->sbcId(), tCluster))
#endif
                throw BUPMsg::SBC_NOT_FOUND;
                    
            // Handling of failed auto dump
            //string marker = destDir + Config::instance().MarkerFileName;
            if (extent != ALL) {
                APBackupInfo buinfo(destDir.c_str());
                u_int32 admarker = buinfo.getAdMarker();
                //bool adFileMarker = fileExists(marker.c_str());
                bool adFileMarker = EnvFMS::fileExists(cmd->sbcId(), Config::instance().MarkerFileName, tCluster);
                TRACE((LOG_LEVEL_INFO, "BUINFO AdMarker = <%d>, File ADMARKER = <%d>", 0, admarker, adFileMarker));
                if (adFileMarker || admarker) {
                    BUPMsg::ERROR_CODE err = admarker ? 
                        BUPMsg::AUTODUMP_FAILED_BUINFO_CORRECTED : 
                        BUPMsg::AUTODUMP_FAILED_BUINFO_NOT_CORRECTED;

                    ostringstream os;
                    os << "Auto dump extent <" << extent
                       << "> aborted with exit code <" 
                       << err << "> during DUMP_BEGIN primitive";
                    TRACE((LOG_LEVEL_INFO, "%s", 0, os.str().c_str()));

                    EventReporter::instance().write(os.str().c_str());
                    throw err;
                }

                // Checking the consistency between BUINFO and physical files
                SBCId sbcId = cmd->sbcId();
                if ((buinfo.info_V4()->sddSectorInfo.nofSegments() > 0) !=
                	EnvFMS::fileExists(sbcId, SBCFile::name(SBCFile::SDD), tCluster)) {
                	TRACE((LOG_LEVEL_INFO, "BUINFO inconsistent with SDD physical file", 0));
                	throw BUPMsg::BAD_BACKUPINFO;
                }

                if ((buinfo.info_V4()->ldd1SectorInfo.nofSegments() > 0) !=
                	EnvFMS::fileExists(sbcId, SBCFile::name(SBCFile::LDD1), tCluster)) {
                	TRACE((LOG_LEVEL_INFO, "BUINFO inconsistent with LDD1 physical file", 0));
                	throw BUPMsg::BAD_BACKUPINFO;
                }

                if ((buinfo.info_V4()->ldd2SectorInfo.nofSegments() > 0) !=
                	EnvFMS::fileExists(sbcId, SBCFile::name(SBCFile::LDD2), tCluster)) {
                	TRACE((LOG_LEVEL_INFO, "BUINFO inconsistent with LDD2 physical file", 0));
                	throw BUPMsg::BAD_BACKUPINFO;
                }

                if ((buinfo.info_V4()->rsSectorInfo.nofSegments() > 0) !=
                	EnvFMS::fileExists(sbcId, SBCFile::name(SBCFile::RS), tCluster)) {
                	TRACE((LOG_LEVEL_INFO, "BUINFO inconsistent with RS physical file", 0));
                	throw BUPMsg::BAD_BACKUPINFO;
                }

                if ((buinfo.info_V4()->psSectorInfo.nofSegments() > 0) !=
                	EnvFMS::fileExists(sbcId, SBCFile::name(SBCFile::PS), tCluster)) {
                	TRACE((LOG_LEVEL_INFO, "BUINFO inconsistent with PS physical file", 0));
                	throw BUPMsg::BAD_BACKUPINFO;
                }

                if ((buinfo.info_V4()->rs2SectorInfo.nofSegments() > 0) !=
                	EnvFMS::fileExists(sbcId, SBCFile::name(SBCFile::RS2), tCluster)) {
                	TRACE((LOG_LEVEL_INFO, "BUINFO inconsistent with RS2 physical file", 0));
                	throw BUPMsg::BAD_BACKUPINFO;
                }

                if ((buinfo.info_V4()->ps2SectorInfo.nofSegments() > 0) !=
                	EnvFMS::fileExists(sbcId, SBCFile::name(SBCFile::PS2), tCluster)) {
                	TRACE((LOG_LEVEL_INFO, "BUINFO inconsistent with PS2 physical file", 0));
                	throw BUPMsg::BAD_BACKUPINFO;
                }
            }

            // 1. execute command
            // 1.1 create new key
            //===================================================================
            key.newKey();
            
            // 1.2 create "virtual" path for response
            //ftpDumpDir = Config::instance().ftpDumpDir(key, cpId);
            ftpDumpDir = Config::instance().ftpDumpDir(key, tCluster);

			TRACE((LOG_LEVEL_INFO, "returning from Config::instance().ftpDumpDir %s", 0, ftpDumpDir.c_str()));
			TRACE((LOG_LEVEL_INFO, "Length of ftpDumdir is %d", 0, ftpDumpDir.length()));

			// Not sure if it works if we remove this
			if(ftpDumpDir.length() > Config::FTP_PATH_MAX_DIR_LEN_V4) {
                THROW_XCODE("Path to ftp virtual directory is too long", CodeException::CONFIGURATION_ERROR)
            }

            // 1.3 get (local) path to tmp dir based on key & create the directory
            string tmpDir = Config::instance().tmpDir(key, tCluster);
			TRACE((LOG_LEVEL_INFO, "tmpDir is: %s", 0, tmpDir.c_str()));

			
			FileFunx::createDirX(tmpDir.c_str(), 0777);
			TRACE((LOG_LEVEL_INFO, "returning from FileFunx::createDirX: ", 0));

            TmpCurrDir currDir(tmpDir.c_str());
            BUPFunx::writeParams(cmd);

			TRACE((LOG_LEVEL_INFO, "returning from BUPFunx::writeParams", 0));

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
		str << "DumpBeginJob_V4, FMS Errorcode : " << x.errorCode() << ", Text: " << x.errorText() << ", Details: "
		    << x.detailInfo() << endl;
		newTRACE((LOG_LEVEL_ERROR, "%s", 0, str.str().c_str()));
		m_exitcode = BUPMsg::INTERNAL_ERROR;
		EventReporter::instance().write(str.str().c_str());		
	}
#endif
    catch(CodeException& x) {
        switch(x.errcode()) {
            case CodeException::CP_APG43_CONFIG_ERROR:
                TRACE((LOG_LEVEL_ERROR, "DumpBeginJob_V4::execute() CS error ", 0));
                m_exitcode = BUPMsg::INTERNAL_ERROR;
                break;
            default:
                EventReporter::instance().write(x);
                m_exitcode = BUPMsg::INTERNAL_ERROR;
                break;

        }
    }
    catch(const Exception& x) {
        EventReporter::instance().write(x);
        m_exitcode = BUPMsg::INTERNAL_ERROR;
    }
    catch(...) {
        EventReporter::instance().write("Unhandled internal error.");
        m_exitcode = BUPMsg::INTERNAL_ERROR;
    }

    msg().reset();
    FtpFileSet_V4 files;
    if(m_exitcode == BUPMsg::OK)
        BUPFunx::setFiles(files, extent, ftpDumpDir, 7);

    //Get current link speed between AP-CP
    if (SystemInfo::getCurrentCPAPBandWidth(speed) != 0) {
        TRACE((LOG_LEVEL_WARN, "Failed to retrive CP-AP link speed. Use default speed = %d", 0, speed));
    }
    else {
        TRACE((LOG_LEVEL_INFO, "CP-AP link speed = %d", 0, speed));
    }

 
    new (msg().addr()) BUPMsg::DumpBeginRspMsg_V4(transNum, exitcode(), key, files, speed);

	int s = files.buinfo().dirLength();
	TRACE((LOG_LEVEL_INFO, "DumpdBeginRsp_V4: Dir length = %d TransNum = %d exitcode = %d Version Major = %d", 0, s, transNum, exitcode(), tvm));
}

