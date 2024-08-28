/*
NAME
	File_name:FallbackEndJob.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2002-2023. All rights reserved.

The Copyright to the computer program(s) herein is the property of Ericsson
Utvecklings AB, Sweden.
The program(s) may be used and/or copied only with the written permission from
Ericsson Utvecklings AB or in accordance with the terms and conditions
stipulated in the agreement/contract under which the program(s) have been
supplied.

DESCRIPTION
	Executes the BUP command FALLBACK_END.

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
	2002-05-02 by U/Y/SF Anders Gillgren (qabgill, tag :ag)

SEE ALSO
	

Revision history
----------------
2002-05-02 qabgill  Created
2002-10-31 uablan   Added ACS_Trace
2003-01-21 uablan   Added resetting of loadingInProgress flag
2006-03-01 uablan   Remove setting of LoadInProgress flag. (HG62821)
2007-03-05 uablan   Update for Multi CP System
2010-04-01 xdtthng  Update for SSI
2011-04-23 xdtthng  Update for 43L
2023-01-01 xkomala  Added checks for valid cpid for TR IA20800
*/


#include "Config.h"
#include "FallbackEndJob.h"

#include "CPS_BUSRV_Trace.h"

#include "BUPFunx.h"
#include "CodeException.h"
#include "EventReporter.h"
#include "FileFunx.h"
#include "TmpCurrDir.h"
#include "LinuxException.h"
#include "DataFile.h"
#include "LoadEndHelpJob.h"

#include "mcs_apcmh_loadbackup.h"

#include <fstream>

//
// virtual
//====================================================================
void FallbackEndJob::execute() {

	newTRACE((LOG_LEVEL_INFO, "FallbackEndJob::execute()", 0));

	// local variables used in response
	CPID cpId = 0xFFFF; // default ONE CP system
	const BUPMsg::FallbackEndMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::FallbackEndMsg>(msg());
	u_int32 transNum = cmd->transNum(); 
	u_int16 tvm = cmd->version().major();
	bool multiCPSystem = Config::instance().isMultipleCP();

	SBCId sbcId;

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
					TRACE((LOG_LEVEL_ERROR, "FallbackEndJob, CpId received is <%d> ,throwing internal error", 0, cpId));
					throw BUPMsg::INTERNAL_ERROR;
				} //TR_IA20800 END

			}
				
			TRACE((LOG_LEVEL_INFO, "FallbackEndJob, result = %d", 0, cmd->loadResult()));

			if (cmd->loadResult() == BUPMsg::LOAD_OK) {

				// set source dir as current
				string keyDir = Config::instance().tmpDir(cmd->key(), cpId);

				// check rules
				//===================================================================
				if(!cmd->key().valid())
					throw BUPMsg::INVALID_KEY;

				try {

					{
						// ENM impacts to get SBC file used to reload
						TmpCurrDir parDir(keyDir.c_str());
						// get params
						ifstream inf(PARAMS_FILE);
						if(!inf.good())
						{
							EventReporter::instance().write("Failed to open parameter file.");
						}
						inf >> sbcId;

						inf.close();

						TRACE((LOG_LEVEL_INFO, "FallbackEndJob, from SBC:(%d)", 0, sbcId.id()))
					}
					// clean dump dir (can't clean a dir we're in)
					//FileFunx::SetCurrentDirectoryX(Config::instance().dataBusrvDir(cpId).c_str());
					TmpCurrDir currDir(Config::instance().dataBusrvDir(cpId).c_str());

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
	catch(CodeException& x){
		switch(x.errcode()) {
		case CodeException::CP_APG43_CONFIG_ERROR:
			TRACE((LOG_LEVEL_ERROR, "FallbackEndJob::execute() CS error ", 0));
			m_exitcode = BUPMsg::INTERNAL_ERROR;
			break;
		}
	}
	catch(Exception& x) {
		EventReporter::instance().write(x);
		m_exitcode = BUPMsg::INTERNAL_ERROR;		
	}
	catch(BUPMsg::ERROR_CODE& exitcode) {
		m_exitcode = exitcode;
	}
	catch(...) {
		EventReporter::instance().write("Unhandled internal error.");
		m_exitcode = BUPMsg::INTERNAL_ERROR;
	}

	if ((!multiCPSystem && cpId == 0xffff) || (multiCPSystem && cpId != 0xffff)) {
		// reset flag that load is no longer on going
		DataFile::endLoading(cpId);
		// reset dump flag in case of that the load had interrupted a dump.
		DataFile::endDumping(cpId);
	}

	// Check enviroment and CP backup final result
	if( (cmd->cpSystem() == BUPMsg::ONE_CP_SYSTEM) &&
			(cmd->loadResult() == BUPMsg::LOAD_OK) &&
			(BUPMsg::OK == m_exitcode) )
	{
		try
		{
			// Communicate to APCMH that a CP dump reload has completed
			std::string dumpPath = Config::instance().sbcDir(sbcId, cpId);
			TRACE((LOG_LEVEL_INFO, "FallbackEndJob, call loadAPTCMMfile(%s)", 0, dumpPath.c_str()));
			MCS_APCMH_LoadBackup apcmh_client;
			int result = apcmh_client.loadAPTCMMfile(dumpPath.c_str());
			TRACE((LOG_LEVEL_INFO, "FallbackEndJob, loadAPTCMMfile() returns:<%d>", 0, result));
		}
		catch(...)
		{
			TRACE((LOG_LEVEL_ERROR, "FallbackEndJob, exception before call loadAPTCMMfile()", 0));
		}
	}

	msg().reset();

	// create response in the msg buffer (re-use)
	new (msg().addr()) BUPMsg::FallbackEndRspMsg(transNum, exitcode());
	TRACE((LOG_LEVEL_INFO, "FallbackEndRspMsg: TransNum = %d exitcode = %d Version Major = %d", 0, transNum, exitcode(), tvm));
	
}


///////////////////////////////////////////////////////////////////////////////

//
//
// virtual
//====================================================================
void FallbackEndJob_V4::execute() {

	newTRACE((LOG_LEVEL_INFO, "FallbackEndJob_V4::execute()", 0));

	/////////////////////////////////////////////////////////
	// newTRACE(("FAKE FallbackEndJob_V4::execute()", 0));
	/////////////////////////////////////////////////////////

	// local variables used in response
	Config::CLUSTER tc;
	//u_int16 APZProfile = Config::instance().getAPZProfile();
	//TRACE(("LoadBeginJob_V4::execute(); APZ Profile: %d", 0, APZProfile));

	const BUPMsg::FallbackEndMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::FallbackEndMsg>(msg());
	u_int32 transNum = cmd->transNum(); // save for response
	u_int16 tvm = cmd->version().major();
	CPID cpId = cmd->cpId();
	const Config::CLUSTER tn(cpId, true);
	SBCId loadingSbcId;

	try {

		//TR_IA20800 START
		if(!(Config::instance().isValidBladeCpid(cpId)))
		{
			TRACE((LOG_LEVEL_ERROR, "FallbackEndJob_V4, CpId received is <%d> , throwing internal error", 0, cpId));
			throw BUPMsg::INTERNAL_ERROR;
		}//TR_IA20800 END


		// init stuff
		//===================================================================
		if(verifyVersion()) {

			if (cmd->loadResult() == BUPMsg::LOAD_OK) {

				// set source dir as current
				string keyDir = Config::instance().tmpDir(cmd->key(), tn);

				// check rules
				//===================================================================
				if(!cmd->key().valid())
					throw BUPMsg::INVALID_KEY;

				// NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
				// Backup Protocol Interactions
				//

				// if this throws it is "internal error", not missing dir
				TmpCurrDir currDir(keyDir.c_str());
				// get params
				ifstream inf(PARAMS_FILE);
				if(!inf.good()) {
					EventReporter::instance().write("Failed to open parameter file.");
				}
				inf >> loadingSbcId;
				TRACE((LOG_LEVEL_INFO, "FallbackEndJob_V4::execute() TransNum: %u, loadingSbcId: %u ", 0, transNum, loadingSbcId.id()));
				inf.close();
				//
				// NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN

				try {
					// clean dump dir (can't clean a dir we're in)
					FileFunx::SetCurrentDirectoryX(Config::instance().dataBusrvDir(tn).c_str());

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
	catch(CodeException& x){
		switch(x.errcode()) {
		case CodeException::CP_APG43_CONFIG_ERROR:
			TRACE((LOG_LEVEL_ERROR, "FallbackEndJob_V4::execute() CS error ", 0));
			m_exitcode = BUPMsg::INTERNAL_ERROR;
			break;
		}
	}
	catch(Exception& x) {
		EventReporter::instance().write(x);
		m_exitcode = BUPMsg::INTERNAL_ERROR;		
	}
	catch(BUPMsg::ERROR_CODE exitcode) {
		m_exitcode = exitcode;
	}
	catch(...) {
		EventReporter::instance().write("Unhandled internal error.");
		m_exitcode = BUPMsg::INTERNAL_ERROR;
	}

	// This is the base product interaction
	// reset dump flag in case of that the load had interrupted a dump.
	DataFile::endDumping(tn);

	// reset flag that load is no longer on going
	DataFile::endLoading(tn);
	DataFile::endLoading(tc);
	DataFile::endLoadingSBCID(tn);

	// Clear in core loading activity
	DataFile::clearLoading(cpId, tc);

	msg().reset();
	new (msg().addr()) BUPMsg::FallbackEndRspMsg(transNum, exitcode());
	TRACE((LOG_LEVEL_INFO, "FallbackEndRspMsg: TransNum = %d exitcode = %d Version Major = %d", 0, transNum, exitcode(), tvm));

	DataFile::BitVector& dumpVector = DataFile::readDumpVector();
	if (dumpVector.dumpEndQueue()) {
		Job* helper = new LoadEndHelpJob;
		resetState(Job::JQS_RSPCONT, loadingSbcId, helper);
	}

	return;	
}
