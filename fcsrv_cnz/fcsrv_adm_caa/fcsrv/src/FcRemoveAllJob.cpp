/*
  NAME
  File_name:FcRemoveAllJob.cpp

  COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

  The Copyright to the computer program(s) herein is the property of Ericsson
  Utvecklings AB, Sweden.
  The program(s) may be used and/or copied only with the written permission from
  Ericsson Utvecklings AB or in accordance with the terms and conditions
  stipulated in the agreement/contract under which the program(s) have been
  supplied.

  DESCRIPTION
  Executes the FCP command FC_REMOVE_ALL (configuration).

  DOCUMENT NO
  190 89-CAA 109 0670

  AUTHOR
  2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on FcRemoveAllJob.cpp of FCSRV in Windows

  SEE ALSO
  -

  Revision history
  ----------------
  2006-11-28	uabmnst Created from FCSRV's ReadConfJob.h
  2011-11-15 	xtuudoo Ported to APG43L
  2012-09-11 	xngudan Updated for Cluster session

*/


#include "FCPFunx.h"
#include "FileFunx.h"
#include "CodeException.h"
#include "EventReporter.h"
#include "LinuxException.h"
#include "Config.h"
#include "CPS_FCSRV_Trace.h"
#include "FCPMsg.h"
#include "FcRemoveAllJob.h"
#include "PlatformTypes.h"

//
//
//====================================================================
void FcRemoveAllJob::execute() {

	newTRACE((LOG_LEVEL_INFO, "FcRemoveAllJob::execute()", 0));

	//u_int32 fileSize = 0; // default setting
	u_int32 transNum = 0;
	CPID cpId = 0xFFFF; // default ONE CP system
	try {
        // init stuff
		//===================================================================
		if(verifyVersion()) { // if verify fails the errorcode will be set
			const FCPMsg::FcRemoveAllMsg* cmd = FCPFunx::safe_fcp_ptr_cast<FCPMsg::FcRemoveAllMsg>(msg());
			transNum = cmd->transNum(); // save for response
		   
			//check if ONE CP System
            if (cmd->cpSystem() != FCPMsg::ONE_CP_SYSTEM)
            	cpId = cmd->cpId();

			// create directory structure for this CP
			Config::instance().createDirStructure(cpId);

			// remove all file under ftp
			//----------------------------------
			size_t noFailedDel = FileFunx::cleanDir(Config::instance().ftpDir(cpId).c_str(), "*", false);
			if (noFailedDel != 0)
				throw FCPMsg::CANNOT_DELETE_FILE;
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
            m_exitcode = FCPMsg::INTERNAL_ERROR;
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
	new (msg().addr()) FCPMsg::FcRemoveAllRspMsg(transNum, exitcode());

	if (!exitcode())
      	   TRACE((LOG_LEVEL_INFO, "Exit with error code: %d", 0, m_exitcode));

        TRACE((LOG_LEVEL_INFO, "FcRemoveAllRsp: TransNum = %d exitcode = %d", 0, transNum, exitcode()));

	TRACE((LOG_LEVEL_INFO, "FcRemoveAllJob::execute() returns", 0));
}

//
//
//====================================================================
void FcRemoveAllJob_V4::execute() {

	newTRACE((LOG_LEVEL_INFO, "FcRemoveAllJob_V4::execute()", 0));

	//u_int32 fileSize = 0; // default setting
	u_int32 transNum = 0;

	try {
        // init stuff
		//===================================================================
		if(verifyVersion()) { // if verify fails the errorcode will be set
			const FCPMsg::FcRemoveAllMsg* cmd = FCPFunx::safe_fcp_ptr_cast<FCPMsg::FcRemoveAllMsg>(msg());
			transNum = cmd->transNum(); // save for response

			const Config::CLUSTER t;

			// create directory structure for this Cluster
			Config::instance().createDirStructure(t);

			// remove all file under ftp
			//----------------------------------
			size_t noFailedDel = FileFunx::cleanDir(Config::instance().ftpDir(t).c_str(), "*", false);
			if (noFailedDel != 0)
				throw FCPMsg::CANNOT_DELETE_FILE;
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
            m_exitcode = FCPMsg::INTERNAL_ERROR;		
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
	new (msg().addr()) FCPMsg::FcRemoveAllRspMsg(transNum, exitcode());

	if (!exitcode())
      TRACE((LOG_LEVEL_INFO, "Exit with error code: %d", 0, m_exitcode));
}
