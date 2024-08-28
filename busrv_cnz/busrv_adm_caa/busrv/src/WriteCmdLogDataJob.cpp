/*
NAME
	File_name:WriteCmdLogDataJob.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2002-2023. All rights reserved.

The Copyright to the computer program(s) herein is the property of Ericsson
Utvecklings AB, Sweden.
The program(s) may be used and/or copied only with the written permission from
Ericsson Utvecklings AB or in accordance with the terms and conditions
stipulated in the agreement/contract under which the program(s) have been
supplied.

DESCRIPTION
	Executes the BUP command WRITE_CMDLOG_DATA.

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
	2002-05-02 by U/Y/SF Anders Gillgren (qabgill, tag :ag)

SEE ALSO
	

Revision history
----------------
2002-05-02 qabgill Created
2002-11-04 uablan  Add ACS Trace
2007-03-06 uablan  Updates for Multiple CP System
2023-01-01 xkomala Added checks for valid cpid for TR IA20800
*/

#include "BUPFunx.h"
#include "Config.h"
#include "DataFile.h"
#include "CodeException.h"
#include "EventReporter.h"
#include "LinuxException.h"
#include "CPS_BUSRV_Trace.h"

#include "WriteCmdLogDataJob.h"


//
//
//====================================================================
void WriteCmdLogDataJob::execute() {

	newTRACE((LOG_LEVEL_INFO, "WriteCmdLogDataJob::execute()", 0));

	CPID cpId = 0xFFFF; 
	const BUPMsg::WriteCmdLogDataMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::WriteCmdLogDataMsg>(msg());
	u_int32 transNum = cmd->transNum(); // save for response
	u_int16 tvm = cmd->version().major();

	try {
		// if verify fails the errorcode will be set
		if(verifyVersion()) {
			//const BUPMsg::WriteCmdLogDataMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::WriteCmdLogDataMsg>(msg());
			//transNum = cmd->transNum(); // save for response

			//check if ONE CP System
			if (cmd->cpSystem() != BUPMsg::ONE_CP_SYSTEM)
			{
				cpId = cmd->cpId();

				//TR_IA20800 START
				if(!(Config::instance().isValidSpxCpid(cpId)))
				{
					TRACE((LOG_LEVEL_ERROR, "WriteCmdLogDataJob, CpId received is <%d> ,throwing internal error", 0, cpId));
					throw BUPMsg::INTERNAL_ERROR;
				} //TR_IA20800 END

			}

			// set current cmdlog in data file - may throw win32 on failure
			DataFile::currCmdLog(cmd->currCmdLog(), cpId);
			TRACE((LOG_LEVEL_INFO, "WriteCmdLogDataJob: currCmdLog = %d", 0, cmd->currCmdLog()));
		}
	}
	//
	// catch all error & set exitcode
	//
	catch(BUPMsg::ERROR_CODE exitcode) {
		m_exitcode = exitcode;
	}
	catch(LinuxException& x) {
		(void) x;
		m_exitcode = BUPMsg::WRITE_DATAAREA_ERROR;
	}
	catch(CodeException& x) {
		switch(x.errcode()) {
			case CodeException::CP_APG43_CONFIG_ERROR:
			TRACE((LOG_LEVEL_ERROR, "DumpEndJob::execute() CS error ", 0));
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
	
	msg().reset();
	new (msg().addr()) BUPMsg::WriteCmdLogDataRspMsg(transNum, exitcode());
	TRACE((LOG_LEVEL_INFO, "WriteCmdLogDataRspMsg: TransNum = %d exitcode = %d Version Major = %d", 0, transNum, exitcode(), tvm));
}

///////////////////////////////////////////////////////////////////////////////
//
//
//====================================================================
void WriteCmdLogDataJob_V4::execute() {

	newTRACE((LOG_LEVEL_INFO, "WriteCmdLogDataJob_V4::execute()", 0));

	const BUPMsg::WriteCmdLogDataMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::WriteCmdLogDataMsg>(msg());
	u_int32 transNum = cmd->transNum(); // save for response
	u_int16 tvm = cmd->version().major();
	//CPID cpId = cmd->cpId(); 

	try {
		// if verify fails the errorcode will be set
		if(verifyVersion()) {

			// set current cmdlog in data file - may throw win32 on failure
			//DataFile::currCmdLog(cmd->currCmdLog(), Config::CLUSTER(cpId, true));
			//TRACE(("WriteCmdLogDataJob: currCmdLog = %d", 0, cmd->currCmdLog()));

			// At the moment, return an error code
			throw BUPMsg::NOT_SUP_IN_CLUSTER;
		}
	}
	//
	// catch all error & set exitcode
	//
	catch(BUPMsg::ERROR_CODE exitcode) {
		m_exitcode = exitcode;
	}
	catch(LinuxException& x) {
		(void) x;
		m_exitcode = BUPMsg::WRITE_DATAAREA_ERROR;
	}
	catch(CodeException& x) {
		switch(x.errcode()) {
			case CodeException::CP_APG43_CONFIG_ERROR:
			TRACE((LOG_LEVEL_ERROR, "DumpEndJob::execute() CS error ", 0));
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
	
	msg().reset();
	new (msg().addr()) BUPMsg::WriteCmdLogDataRspMsg(transNum, exitcode());

	TRACE((LOG_LEVEL_INFO, "WriteCmdLogDataRspMsg: TransNum = %d exitcode = %d Version Major = %d", 0, transNum, exitcode(), tvm));
}

