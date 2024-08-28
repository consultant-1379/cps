/*
NAME
	File_name:ReadCmdLogConfJob.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2002-2023. All rights reserved.

The Copyright to the computer program(s) herein is the property of Ericsson
Utvecklings AB, Sweden.
The program(s) may be used and/or copied only with the written permission from
Ericsson Utvecklings AB or in accordance with the terms and conditions
stipulated in the agreement/contract under which the program(s) have been
supplied.

DESCRIPTION
	Executes the BUP command READ_CMDLOG_CONF.

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
	2002-05-02 by U/Y/SF Anders Gillgren (qabgill, tag :ag)

SEE ALSO
	

Revision history
----------------
2002-05-02 qabgill Created
2002-11-04 uablan  Added Acs Trace
2007-03-06 uablan  Updates for Multiple CP System.
2023-01-01 xkomala Added checks for valid cpid for TR IA20800
*/


#include "BUPFunx.h"
#include "CodeException.h"
#include "EventReporter.h"
#include "LinuxException.h"
#include "Config.h"
#include "CPS_BUSRV_Trace.h"

#include "ReadCmdLogConfJob.h"

//
//
//====================================================================
void ReadCmdLogConfJob::execute() {

	newTRACE((LOG_LEVEL_INFO, "ReadCmdLogConfJob::execute()", 0));

	int cmdlogHandling = 0; //default setting manual
	CPID cpId = 0xFFFF; // default ONE CP system

	const BUPMsg::ReadCmdLogConfMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::ReadCmdLogConfMsg>(msg());
	u_int32 transNum = cmd->transNum(); 
	u_int16 tvm = cmd->version().major();

	try {
		// init stuff
		//===================================================================
		if(verifyVersion()) { // if verify fails the errorcode will be set
			//const BUPMsg::ReadCmdLogConfMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::ReadCmdLogConfMsg>(msg());
			//transNum = cmd->transNum(); 

			//check if ONE CP System
			if (cmd->cpSystem() != BUPMsg::ONE_CP_SYSTEM)
			{
				cpId = cmd->cpId();

				//TR_IA20800 START
				if(!(Config::instance().isValidSpxCpid(cpId)))
				{
					TRACE((LOG_LEVEL_ERROR, "ReadCmdLogConfJob, CpId received is <%d> ,throwing internal error", 0, cpId));
					throw BUPMsg::INTERNAL_ERROR;
				} //TR_IA20800 END
			}

			// Read manCmdlogHandling flag
			//----------------------------
			cmdlogHandling = Config::instance().getCmdLogHandling(cpId);
		}
	}
	//
	// catch all error & set exitcode
	//
	catch(BUPMsg::ERROR_CODE exitcode) {
		m_exitcode = exitcode;
	}
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
	catch(...) {
		EventReporter::instance().write("Unhandled internal error.");
		m_exitcode = BUPMsg::INTERNAL_ERROR;
	}
	
	msg().reset();
	new (msg().addr()) BUPMsg::ReadCmdLogConfRspMsg(transNum, exitcode(), cmdlogHandling);

	TRACE((LOG_LEVEL_INFO, "ReadCmdLogConfRspMsg: TransNum = %d exitcode = %d Version Major = %d", 0, transNum, exitcode(), tvm));
	if (!exitcode())
		TRACE((LOG_LEVEL_INFO, "ReadCmdLogConfRspMsg: manCmdlogHandling = %d", 0, cmdlogHandling));
}

#if 0
///////////////////////////////////////////////////////////////////////////////
//
//
//====================================================================
void ReadCmdLogConfJob_V4::execute() {

	newTRACE(("ReadCmdLogConfJob_V4::execute()", 0));

	int cmdlogHandling = 0; //default setting manual
	const BUPMsg::ReadCmdLogConfMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::ReadCmdLogConfMsg>(msg());
	u_int32 transNum = cmd->transNum(); // save for response
	u_int16 tvm = cmd->version().major();

	try {
		// init stuff
		//===================================================================
		if(verifyVersion()) { // if verify fails the errorcode will be set

			// Read manCmdlogHandling flag
			//----------------------------
			//cmdlogHandling = Config::instance().getCmdLogHandling(Config::CLUSTER());

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
	catch(CodeException& x){
		switch(x.errcode()) {
		case CodeException::CP_APG43_CONFIG_ERROR:
			TRACE(("FallbackEndJob::execute() CS error ", 0));
			m_exitcode = BUPMsg::INTERNAL_ERROR;
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
	BUPMsg::ReadCmdLogConfRspMsg* rsp = new (msg().addr())
		BUPMsg::ReadCmdLogConfRspMsg(transNum, exitcode(), cmdlogHandling);

	TRACE(("ReadCmdLogConfRspMsg: TransNum = %d exitcode = %d Version Major = %d", 0, transNum, exitcode(), tvm));
	if (!exitcode())
		TRACE(("ReadCmdLogConfRspMsg: manCmdlogHandling = %d", 0, cmdlogHandling));
}

//
//====================================================================
void ReadCmdLogConfJob_LEV4::execute() {

	newTRACE(("ReadCmdLogConfJob::execute()", 0));

	int cmdlogHandling = 0; //default setting manual
	CPID cpId = 0xFFFF; // default ONE CP system

	const BUPMsg::ReadCmdLogConfMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::ReadCmdLogConfMsg>(msg());
	u_int32 transNum = cmd->transNum(); 

	try {
		// init stuff
		//===================================================================
		if(verifyVersion()) { // if verify fails the errorcode will be set
			//const BUPMsg::ReadCmdLogConfMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::ReadCmdLogConfMsg>(msg());
			//transNum = cmd->transNum(); 

			//check if ONE CP System
			if (cmd->cpSystem() != BUPMsg::ONE_CP_SYSTEM)
				cpId = cmd->cpId();

			// Read manCmdlogHandling flag
			//----------------------------
			cmdlogHandling = Config::instance().getCmdLogHandling(cpId);
		}
	}
	//
	// catch all error & set exitcode
	//
	catch(BUPMsg::ERROR_CODE exitcode) {
		m_exitcode = exitcode;
	}
	catch(CodeException& x){
		switch(x.errcode()) {
		case CodeException::CP_APG43_CONFIG_ERROR:
			TRACE(("FallbackEndJob::execute() CS error ", 0));
			m_exitcode = BUPMsg::INTERNAL_ERROR;
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
	BUPMsg::ReadCmdLogConfRspMsg* rsp = new (msg().addr())
		BUPMsg::ReadCmdLogConfRspMsg(transNum, exitcode(), cmdlogHandling);

	if (!exitcode())
		TRACE(("ReadCmdLogConfRspMsg: manCmdlogHandling = %d", 0, cmdlogHandling));
}
#endif
