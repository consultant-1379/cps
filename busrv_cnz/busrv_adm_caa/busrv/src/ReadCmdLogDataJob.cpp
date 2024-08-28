/*
NAME
	File_name:ReadCmdLogDataJob.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2002-2023. All rights reserved.

The Copyright to the computer program(s) herein is the property of Ericsson
Utvecklings AB, Sweden.
The program(s) may be used and/or copied only with the written permission from
Ericsson Utvecklings AB or in accordance with the terms and conditions
stipulated in the agreement/contract under which the program(s) have been
supplied.

DESCRIPTION
	Executes the BUP command READ_CMDLOG_DATA.

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
#include "DataFile.h"
#include "CodeException.h"
#include "EventReporter.h"
#include "LinuxException.h"
#include "CPS_BUSRV_Trace.h"
#include "Config.h"

#include "ReadCmdLogDataJob.h"

//#include <new.h>

#ifdef _LEGACY_BUP3_SUPPORTED
//
//
//====================================================================
void ReadCmdLogDataJob::execute() {

	newTRACE((LOG_LEVEL_INFO, "ReadCmdLogDataJob::execute()", 0));

	u_int32 currentCmdlog = 0; // in case of verifyVersion failes
	CPID cpId = 0xFFFF; // default ONE CP system
	const BUPMsg::ReadCmdLogDataMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::ReadCmdLogDataMsg>(msg());
	u_int32 transNum = cmd->transNum(); 
	u_int16 tvm = cmd->version().major();

	try {
		// init stuff
		//===================================================================
		if(verifyVersion()) { // if verify fails the errorcode will be set
			//const BUPMsg::ReadCmdLogDataMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::ReadCmdLogDataMsg>(msg());
			//transNum = cmd->transNum(); 

			//check if ONE CP System
			if (cmd->cpSystem() != BUPMsg::ONE_CP_SYSTEM)
			{
				cpId = cmd->cpId();

				//TR_IA20800 START
				if(!(Config::instance().isValidSpxCpid(cpId)))
				{
					TRACE((LOG_LEVEL_ERROR, "ReadCmdLogDataJob, CpId received is <%d> ,throwing internal error", 0, cpId));
					throw BUPMsg::INTERNAL_ERROR;
				} //TR_IA20800 END

			}

			// Read current cmdlog
			//--------------------
			currentCmdlog = DataFile::currCmdLog(cpId);  // no throw
		}
	}
	//
	// catch all error & set exitcode
	//
	catch(BUPMsg::ERROR_CODE exitcode) {
		m_exitcode = exitcode;
	}
	catch(CodeException& x) {
		switch(x.errcode()) {
		case CodeException::CP_APG43_CONFIG_ERROR:
			TRACE((LOG_LEVEL_ERROR, "ReadCmdLogDataJob::execute() CS error ", 0));
			m_exitcode = BUPMsg::INTERNAL_ERROR;
			break;
		default:
			m_exitcode = BUPMsg::INTERNAL_ERROR;
			TRACE((LOG_LEVEL_ERROR, "ListSBCsJob: CodeExcpetion error code: %d", 0, x.errcode()));
			break;
		}
	}
	catch(Exception& x) {
		EventReporter::instance().write(x);
		m_exitcode = BUPMsg::INTERNAL_ERROR;		
	}
	catch(...) {
		EventReporter::instance().write("Unhandled internal error.");
		m_exitcode = BUPMsg::INTERNAL_ERROR;
	}
	
	msg().reset();
	new (msg().addr()) BUPMsg::ReadCmdLogDataRspMsg(transNum, exitcode(), currentCmdlog);

	TRACE((LOG_LEVEL_INFO, "ReadCmdLogDataRspMsg: TransNum = %d exitcode = %d Version Major = %d", 0, transNum, exitcode(), tvm));
	if (!exitcode())
		TRACE((LOG_LEVEL_INFO, "ReadCmdLogDataRspMsg: currentCmdlog = %d", 0, currentCmdlog));
}
#endif

#if 0
////////////////////////////////////////////////////////////////////////////////
//
//
//====================================================================
void ReadCmdLogDataJob_V4::execute() {

	newTRACE((LOG_LEVEL_INFO, "ReadCmdLogDataJob::execute()", 0));

	u_int32 currentCmdlog = 0; // in case of verifyVersion failes
	const BUPMsg::ReadCmdLogDataMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::ReadCmdLogDataMsg>(msg());
	u_int32 transNum = cmd->transNum(); 
	CPID cpId = cmd->cpId(); 
	u_int16 tvm = cmd->version().major();

	try {
		// init stuff
		//===================================================================
		if(verifyVersion()) { // if verify fails the errorcode will be set

			// Read current cmdlog
			//--------------------
			//currentCmdlog = DataFile::currCmdLog(Config::CLUSTER(cpId, true));  // no throw

			// At the moment, return an error code
			throw BUPMsg::ERROR_CODE::NOT_SUP_IN_CLUSTER;
		}
	}
	//
	// catch all error & set exitcode
	//
	catch(BUPMsg::ERROR_CODE exitcode) {
		m_exitcode = exitcode;
	}
	catch(CodeException& x) {
		switch(x.errcode()) {
		case CodeException::CP_APG43_CONFIG_ERROR:
			TRACE((LOG_LEVEL_ERROR, "ReadCmdLogDataJob::execute() CS error ", 0));
			m_exitcode = BUPMsg::INTERNAL_ERROR;
			break;
		default:
			m_exitcode = BUPMsg::INTERNAL_ERROR;
			TRACE((LOG_LEVEL_ERROR, "ListSBCsJob: CodeExcpetion error code: %d", 0, x.errcode()));
			break;
		}
	}
	catch(Exception& x) {
		EventReporter::instance().write(x);
		m_exitcode = BUPMsg::INTERNAL_ERROR;		
	}
	catch(...) {
		EventReporter::instance().write("Unhandled internal error.");
		m_exitcode = BUPMsg::INTERNAL_ERROR;
	}
	
	msg().reset();
	BUPMsg::ReadCmdLogDataRspMsg* rsp = new (msg().addr())
		BUPMsg::ReadCmdLogDataRspMsg(transNum, exitcode(), currentCmdlog);

	TRACE((LOG_LEVEL_INFO, "ReadCmdLogDataRspMsg: TransNum = %d exitcode = %d Version Major = %d", 0, transNum, exitcode(), tvm));
	if (!exitcode())
		TRACE((LOG_LEVEL_INFO, "ReadCmdLogDataRspMsg: currentCmdlog = %d", 0, currentCmdlog));
}
//
//
//====================================================================
void ReadCmdLogDataJob_LEV4::execute() {

	newTRACE((LOG_LEVEL_INFO, "ReadCmdLogDataJob_LEV4::execute()", 0));

	u_int32 currentCmdlog = 0; // in case of verifyVersion failes
	CPID cpId = 0xFFFF; // default ONE CP system
	const BUPMsg::ReadCmdLogDataMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::ReadCmdLogDataMsg>(msg());
	u_int32 transNum = cmd->transNum(); 

	try {
		// init stuff
		//===================================================================
		if(verifyVersion()) { // if verify fails the errorcode will be set
			//const BUPMsg::ReadCmdLogDataMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::ReadCmdLogDataMsg>(msg());
			//transNum = cmd->transNum(); 

			//check if ONE CP System
			if (cmd->cpSystem() != BUPMsg::ONE_CP_SYSTEM)
				cpId = cmd->cpId();

			// Read current cmdlog
			//--------------------
			currentCmdlog = DataFile::currCmdLog((CPID)cpId);  // no throw
		}
	}
	//
	// catch all error & set exitcode
	//
	catch(BUPMsg::ERROR_CODE exitcode) {
		m_exitcode = exitcode;
	}
	catch(CodeException& x) {
		switch(x.errcode()) {
		case CodeException::CP_APG43_CONFIG_ERROR:
			TRACE((LOG_LEVEL_ERROR, "ReadCmdLogDataJob::execute() CS error ", 0));
			m_exitcode = BUPMsg::INTERNAL_ERROR;
			break;
		default:
			m_exitcode = BUPMsg::INTERNAL_ERROR;
			TRACE((LOG_LEVEL_ERROR, "ListSBCsJob: CodeExcpetion error code: %d", 0, x.errcode()));
			break;
		}
	}
	catch(Exception& x) {
		EventReporter::instance().write(x);
		m_exitcode = BUPMsg::INTERNAL_ERROR;		
	}
	catch(...) {
		EventReporter::instance().write("Unhandled internal error.");
		m_exitcode = BUPMsg::INTERNAL_ERROR;
	}
	
	msg().reset();
	BUPMsg::ReadCmdLogDataRspMsg* rsp = new (msg().addr())
		BUPMsg::ReadCmdLogDataRspMsg(transNum, exitcode(), currentCmdlog);

	if (!exitcode())
		TRACE((LOG_LEVEL_INFO, "ReadCmdLogDataRspMsg: currentCmdlog = %d", 0, currentCmdlog));
}
#endif
