/*
NAME
   File_name:LoadEndJob.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2002-2023. All rights reserved.

   The Copyright to the computer program(s) herein is the property of Ericsson
   Utvecklings AB, Sweden.
   The program(s) may be used and/or copied only with the written permission from
   Ericsson Utvecklings AB or in accordance with the terms and conditions
   stipulated in the agreement/contract under which the program(s) have been
   supplied.

DESCRIPTION
   Executes the BUP command LOAD_END, which really doesn't do much in
	this implementation. It just OK's the op, and cleans up the key-dir.

DOCUMENT NO
   190 89-CAA 109 0387

AUTHOR
   2002-02-05 by UAB/KY/SK Anders Gillgren (qabgill, tag :ag)

SEE ALSO
   

Revision history
----------------
    2002-02-05 qabgill Created
    2012-12-24 xdtthng Updated for APG43L BC
    2023-01-01 xkomala Added checks for valid cpid for TR IA20800
*/


#include "Config.h"
#include "LoadEndJob.h"

#include "CPS_BUSRV_Trace.h"

#include "BUPFunx.h"
#include "CodeException.h"
#include "DataFile.h"
#include "EventReporter.h"
#include "FileFunx.h"
#include "TmpCurrDir.h"
#include "LinuxException.h"
#include "LoadEndHelpJob.h"

#include "mcs_apcmh_loadbackup.h"

#include <fstream>


//
// virtual
//====================================================================
void LoadEndJob::execute()
{

	newTRACE((LOG_LEVEL_INFO, "LoadEndJob::execute()", 0));

	// local variables used in response
	CPID cpId = 0xFFFF; // default ONE CP system
	const BUPMsg::LoadEndMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::LoadEndMsg>(msg());
	u_int32 transNum = cmd->transNum(); // save for response
	u_int16 tvm = cmd->version().major();
	SBCId sbcId;

	try
	{
		// init stuff
		//===================================================================
		if(verifyVersion())
		{
			//check if ONE CP System
			if (cmd->cpSystem() != BUPMsg::ONE_CP_SYSTEM)
			{
				cpId = cmd->cpId();

				//TR_IA20800 START
				if(!(Config::instance().isValidSpxCpid(cpId)))
				{
					TRACE((LOG_LEVEL_ERROR, "LoadEndJob, CpId received is <%d> ,throwing internal error", 0, cpId));
					throw BUPMsg::INTERNAL_ERROR;
				}//TR_IA20800 END
			}
				
			TRACE((LOG_LEVEL_INFO, "LoadEndJob, result = %d", 0, cmd->loadResult()));

			if (cmd->loadResult() == BUPMsg::LOAD_OK)
			{
				// check rules
				if(!cmd->key().valid())
					throw BUPMsg::INVALID_KEY;

				// set source dir as current
				string keyDir = Config::instance().tmpDir(cmd->key(), cpId);
                
				try
				{
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
					}

					// clean dump dir (can't clean a dir we're in)
					TmpCurrDir currDir(Config::instance().dataBusrvDir(cpId).c_str());

					int failCnt = FileFunx::cleanDir(keyDir.c_str(), "*");
					if (failCnt != 0) {
						TRACE((LOG_LEVEL_ERROR, "LoadEndJob, Failed to delete %d no of files", 0, failCnt));
						EventReporter::instance().write("Faild to delete files");
					}
				}
				catch(LinuxException& x) {
					EventReporter::instance().write(x); // log on AP, but OK to CP
				}
				catch(...) {
					EventReporter::instance().write("LoadEndJob, Failed to clean dir");
				}
			}
		}
	}
	//
	// catch all error & set exitcode
	//
	catch(CodeException& x)
	{
		switch(x.errcode()) {
		case CodeException::CP_APG43_CONFIG_ERROR:
			TRACE((LOG_LEVEL_ERROR, "LoadEndJob::execute() CS error ", 0));
			m_exitcode = BUPMsg::INTERNAL_ERROR;
			break;
		}
	}
	catch(BUPMsg::ERROR_CODE exitcode) {
		m_exitcode = exitcode;
	}
	catch(Exception& x) {
		EventReporter::instance().write(x);
		m_exitcode = BUPMsg::INTERNAL_ERROR;		
	}
	catch(...) {
		EventReporter::instance().write("Unhandled internal error.");
		m_exitcode = BUPMsg::INTERNAL_ERROR;
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
			TRACE((LOG_LEVEL_INFO, "LoadEndJob, call loadAPTCMMfile(%s)", 0, dumpPath.c_str()));
			MCS_APCMH_LoadBackup apcmh_client;
			int result = apcmh_client.loadAPTCMMfile(dumpPath.c_str());
			TRACE((LOG_LEVEL_INFO, "LoadEndJob, loadAPTCMMfile() returns:<%d>", 0, result));
		}
		catch(...)
		{
			TRACE((LOG_LEVEL_ERROR, "LoadEndJob, exception before call loadAPTCMMfile()", 0));
		}
	}

	// reset dump flag in case of that the load had interrupted a dump.
	// this potentially throw an exception and terminate with coredump if fail to write
	DataFile::endDumping(cpId);

	msg().reset();
	    
	new (msg().addr()) BUPMsg::LoadEndRspMsg(transNum, exitcode());
    TRACE((LOG_LEVEL_INFO, "LoadEndRspMsg: TransNum = %d exitcode = %d Version Major = %d", 0, transNum, exitcode(), tvm));
	
}

//
// virtual
//====================================================================
void LoadEndJob_V4::execute() {

	newTRACE((LOG_LEVEL_INFO, "LoadEndJob_V4::execute()", 0));

	////////////////////////////////////////////////////////////
	//newTRACE(("Fake LoadEndJob_V4::execute()", 0));
	///////////////////////////////////////////////////////////

	//u_int16 APZProfile = Config::instance().getAPZProfile();
	//TRACE(("LoadEndJob_V4::execute(); APZ Profile: %d", 0, APZProfile));

	//CPID cpId = 0xFFFF; 
	const BUPMsg::LoadEndMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::LoadEndMsg>(msg());
	u_int32 transNum = cmd->transNum(); // save for response
    CPID origCpId = cmd->cpId();
	u_int16 tvm = cmd->version().major();
	const Config::CLUSTER tCluster(origCpId, true);
	SBCId loadingSbcId;
	const Config::CLUSTER tc;

	try {

		//TR_IA20800 START
		if(!(Config::instance().isValidBladeCpid(origCpId)))
		{
			TRACE((LOG_LEVEL_ERROR, "LoadEndJob_V4, CpId received is <%d> , throwing internal error", 0, origCpId));
			throw BUPMsg::INTERNAL_ERROR;
		} //TR_IA20800 END

		// init stuff
		//===================================================================
		if(verifyVersion()) {

			if (cmd->loadResult() == BUPMsg::LOAD_OK) {

				// set source dir as current
				string keyDir = Config::instance().tmpDir(cmd->key(), tCluster);

				// check rules
				// Fake
				// Do not remove. This will be used when the path lentth is increased
				//===================================================================
                //if(!cmd->key().valid() || !FileFunx::dirExists(keyDir.c_str())) {
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
				TRACE((LOG_LEVEL_INFO, "LoadEndJob_V4::execute() TransNum: %u, loadingSbcId: %u origCpId: %u", 0, transNum, loadingSbcId.id(), origCpId));
				inf.close();
				//
				// NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
					            
				try {
					// clean dump dir (can't clean a dir we're in)
					//SetCurrentDirectory(Config::instance().dataBusrvDir(tCluster).c_str());
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
	catch(CodeException& x){
		switch(x.errcode()) {
		case CodeException::CP_APG43_CONFIG_ERROR:
			TRACE((LOG_LEVEL_INFO, "LoadEndJob_V4::execute() CS error ", 0));
			m_exitcode = BUPMsg::INTERNAL_ERROR;
			break;
		}
	}
	catch(BUPMsg::ERROR_CODE exitcode) {
		m_exitcode = exitcode;
	}
	catch(Exception& x) {
		EventReporter::instance().write(x);
		m_exitcode = BUPMsg::INTERNAL_ERROR;		
	}
	catch(...) {
		EventReporter::instance().write("Unhandled internal error.");
		m_exitcode = BUPMsg::INTERNAL_ERROR;
	}

	// This is the base product interaction
	// reset dump flag in case of that the load had interrupted a dump.
	DataFile::endDumping(tCluster);

	DataFile::endLoadingSBCID(tCluster);
	DataFile::clearDumping(origCpId, tc);
	
	// This has not been counted towards "max simult loadings"
	DataFile::endLoadingSBCID(tCluster);

	// Clear in core loading activity
	DataFile::clearLoading(origCpId, tc);

	msg().reset();
	new (msg().addr()) BUPMsg::LoadEndRspMsg(transNum, exitcode());

	TRACE((LOG_LEVEL_INFO, "LoadEndRspMsg: TransNum = %d exitcode = %d Version Major = %d", 0, transNum, exitcode(), tvm));
	
	DataFile::BitVector& dumpVector = DataFile::readDumpVector();
	if (dumpVector.dumpEndQueue()) {
		Job* helper = new LoadEndHelpJob;
		resetState(Job::JQS_RSPCONT, loadingSbcId, helper);
	}
	return;	
}
