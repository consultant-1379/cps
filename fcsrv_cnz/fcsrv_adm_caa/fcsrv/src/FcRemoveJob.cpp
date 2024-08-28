/*
  NAME
  File_name:FcRemoveJob.cpp

  COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

  The Copyright to the computer program(s) herein is the property of Ericsson
  Utvecklings AB, Sweden.
  The program(s) may be used and/or copied only with the written permission from
  Ericsson Utvecklings AB or in accordance with the terms and conditions
  stipulated in the agreement/contract under which the program(s) have been
  supplied.

  DESCRIPTION
  Executes the FCP command FC_REMOVE (configuration).

  DOCUMENT NO
  190 89-CAA 109 0670

  AUTHOR
  2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on FcRemoveJob.cpp of FCSRV in Windows
  SEE ALSO
  -

  Revision history
  ----------------
  2011-11-15 xtuudoo Created
  2012-09-11 xngudan Updated for Cluster session


*/

#include "FCPFunx.h"
#include "FileFunx.h"
#include "TmpCurrDir.h"
#include "CodeException.h"
#include "EventReporter.h"
#include "LinuxException.h"
#include "Config.h"
#include "CPS_FCSRV_Trace.h"
#include "FCPMsg.h"
#include "FcRemoveJob.h"
#include "PlatformTypes.h"
#include <string>

//
//
//====================================================================
void FcRemoveJob::execute() {
    
	newTRACE((LOG_LEVEL_INFO, "FcRemoveJob::execute()", 0));
    using namespace std;

	string fcFile;
	//u_int32 fileSize = 0; // default setting
	u_int32 transNum = 0;
	CPID cpId = 0xFFFF; // default ONE CP system
	try {
        // init stuff
		//===================================================================
		if(verifyVersion()) { // if verify fails the errorcode will be set
			const FCPMsg::FcRemoveMsg* cmd = FCPFunx::safe_fcp_ptr_cast<FCPMsg::FcRemoveMsg>(msg());
			transNum = cmd->transNum(); // save for response
			fcFile = cmd->fileName();
			
			//check if ONE CP System
			if (cmd->cpSystem() != FCPMsg::ONE_CP_SYSTEM)
				cpId = cmd->cpId();

			// create directory structure for this CP
			Config::instance().createDirStructure(cpId);

			// check if file exist and remove it
			//----------------------------------
			// reposition to the correct dir where to delete file
			// if this throws it is "internal error", not missing dir
			TmpCurrDir currDir(Config::instance().ftpDir(cpId).c_str());

			if (FileFunx::DeleteFileX(fcFile.c_str())){
				TRACE((LOG_LEVEL_INFO, "File %s exists", 0, fcFile.c_str()));
			}
			else
				TRACE((LOG_LEVEL_INFO, "File %s do not exists", 0, fcFile.c_str()));
		}
	}
	//
	// catch all error & set exitcode
	//
	catch(FCPMsg::ERROR_CODE &exitcode) {
		m_exitcode = exitcode;
	}
	catch(CodeException& x) {
		switch(x.errcode()) {
        case CodeException::CP_APG43_CONFIG_ERROR:
            TRACE((LOG_LEVEL_ERROR, "FcWriteGetPathJob::execute() CS error ", 0));
            m_exitcode = FCPMsg::INTERNAL_ERROR;
            break;
        default:
            EventReporter::instance().write(x);
            m_exitcode = FCPMsg::INTERNAL_ERROR;
            break;
		}
    }
	catch(LinuxException& x) {
		EventReporter::instance().write(x);
            m_exitcode = FCPMsg::CANNOT_DELETE_FILE;
            }
	catch(Exception& x) {
		EventReporter::instance().write(x);
		m_exitcode = FCPMsg::INTERNAL_ERROR;
	}
	catch(...) {
		EventReporter::instance().write("Unhandled internal error.");
		m_exitcode = FCPMsg::INTERNAL_ERROR;
	}

	msg().reset();
	new (msg().addr()) FCPMsg::FcRemoveRspMsg(transNum, exitcode());

        TRACE((LOG_LEVEL_INFO, "FcRemoveRsp: TransNum = %d exitcode = %d", 0, transNum, exitcode()));

	if (!exitcode())
		TRACE((LOG_LEVEL_INFO, "Exit with error code: %d", 0, m_exitcode));

	TRACE((LOG_LEVEL_INFO, "FcRemoveJob::execute() returns", 0));
}


//
//
//====================================================================
void FcRemoveJob_V4::execute() {

	newTRACE((LOG_LEVEL_INFO, "FcRemoveJob_V4::execute()", 0));
    using namespace std;

	string fcFile;
	//u_int32 fileSize = 0; // default setting
	u_int32 transNum = 0;

	try {
        // init stuff
		//===================================================================
		if(verifyVersion()) { // if verify fails the errorcode will be set
			const FCPMsg::FcRemoveMsg* cmd = FCPFunx::safe_fcp_ptr_cast<FCPMsg::FcRemoveMsg>(msg());
			transNum = cmd->transNum(); // save for response
			fcFile = cmd->fileName();

			const Config::CLUSTER t;

			// create directory structure for this Cluster
			Config::instance().createDirStructure(t);

			// check if file exist and remove it
			//----------------------------------
			// reposition to the correct dir where to delete file
			// if this throws it is "internal error", not missing dir
			TmpCurrDir currDir(Config::instance().ftpDir(t).c_str());

			if (FileFunx::DeleteFileX(fcFile.c_str())){
				TRACE((LOG_LEVEL_INFO, "File %s exists", 0, fcFile.c_str()));
			}
			else
				TRACE((LOG_LEVEL_INFO, "File %s do not exists", 0, fcFile.c_str()));
		}
	}
	//
	// catch all error & set exitcode
	//
	catch(FCPMsg::ERROR_CODE &exitcode) {
		m_exitcode = exitcode;
	}
	catch(CodeException& x) {
		switch(x.errcode()) {
        case CodeException::CP_APG43_CONFIG_ERROR:
            TRACE((LOG_LEVEL_ERROR, "FcWriteGetPathJob::execute() CS error ", 0));
            m_exitcode = FCPMsg::INTERNAL_ERROR;
            break;
        default:
            EventReporter::instance().write(x);
            m_exitcode = FCPMsg::INTERNAL_ERROR;
            break;
		}
    }
	catch(LinuxException& x) {
		EventReporter::instance().write(x);
            m_exitcode = FCPMsg::CANNOT_DELETE_FILE;		
            }
	catch(Exception& x) {
		EventReporter::instance().write(x);
		m_exitcode = FCPMsg::INTERNAL_ERROR;		
	}
	catch(...) {
		EventReporter::instance().write("Unhandled internal error.");
		m_exitcode = FCPMsg::INTERNAL_ERROR;
	}

	msg().reset();
	new (msg().addr()) FCPMsg::FcRemoveRspMsg(transNum, exitcode());

	if (!exitcode())
		TRACE((LOG_LEVEL_INFO, "Exit with error code: %d", 0, m_exitcode));
}

