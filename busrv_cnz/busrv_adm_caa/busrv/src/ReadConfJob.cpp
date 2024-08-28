/*
NAME
	File_name:ReadConfJob.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2002-2023. All rights reserved.

The Copyright to the computer program(s) herein is the property of Ericsson
Utvecklings AB, Sweden.
The program(s) may be used and/or copied only with the written permission from
Ericsson Utvecklings AB or in accordance with the terms and conditions
stipulated in the agreement/contract under which the program(s) have been
supplied.

DESCRIPTION
	Executes the BUP command READ_CONF (configuration).

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
	2002-05-02 by U/Y/SF Anders Gillgren (qabgill, tag :ag)

SEE ALSO
	

Revision history
----------------
2002-05-02 qabgill Created
2002-10-31 uablan  Add ACS_Trace
2007-03-06 uablan  Updates for Multiple CP System
2023-01-01 xkomala Added checks for valid cpid for TR IA20800
*/


#include "Config.h"
#include "BUPFunx.h"
#include "CodeException.h"
#include "EventReporter.h"
#include "LinuxException.h"
#include "CPS_BUSRV_Trace.h"

#include "ReadConfJob.h"


//
//
//====================================================================
void ReadConfJob::execute() {

	newTRACE((LOG_LEVEL_INFO, "ReadConfJob::execute()", 0));

	u_int32 supervisionTime = 60; // default setting
	CPID cpId = 0xFFFF; // default ONE CP system

	const BUPMsg::ReadConfMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::ReadConfMsg>(msg());
	u_int32 transNum = cmd->transNum(); 
	u_int16 tvm = cmd->version().major();
	
	try {
			// init stuff
		//===================================================================
		if(verifyVersion()) { // if verify fails the errorcode will be set
			//const BUPMsg::ReadConfMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::ReadConfMsg>(msg());
			//transNum = cmd->transNum(); // save for response

			//check if ONE CP System
			if (cmd->cpSystem() != BUPMsg::ONE_CP_SYSTEM)
			{
				cpId = cmd->cpId();

				//TR_IA20800 START
				if(!(Config::instance().isValidSpxCpid(cpId)))
				{
					TRACE((LOG_LEVEL_ERROR, "ReadConfJob, CpId received is <%d> ,throwing internal error", 0, cpId));
					throw BUPMsg::INTERNAL_ERROR;
				} //TR_IA20800 END

			}

			// Read conf - the supervision time
			//----------------------------------
			supervisionTime = Config::instance().getSupervisionTime(cpId);
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
	new (msg().addr()) BUPMsg::ReadConfRspMsg(transNum, exitcode(), supervisionTime);

	TRACE((LOG_LEVEL_INFO, "ReadConfRspMsg: TransNum = %d exitcode = %d Version Major = %d", 0, transNum, exitcode(), tvm));
	if (!exitcode())
		TRACE((LOG_LEVEL_INFO, "Read supervision time: %d", 0, supervisionTime));
}

///////////////////////////////////////////////////////////////////////////////
//
//
//====================================================================
void ReadConfJob_V4::execute() {

	newTRACE((LOG_LEVEL_INFO, "ReadConfJob_V4::execute()", 0));

	u_int32 supervisionTime = 60; // default setting
	CPID cpId = 0xFFFF; // default ONE CP system

	const BUPMsg::ReadConfMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::ReadConfMsg>(msg());
	u_int32 transNum = cmd->transNum(); 
	u_int16 tvm = cmd->version().major();

	try {
			// init stuff
		//===================================================================
		if(verifyVersion()) { // if verify fails the errorcode will be set

			//check if ONE CP System
			if (cmd->cpSystem() != BUPMsg::ONE_CP_SYSTEM)
			{
				cpId = cmd->cpId();

				//TR_IA20800 START
				if(!(Config::instance().isValidBladeCpid(cpId)))
				{
					TRACE((LOG_LEVEL_ERROR, "ReadConfJob_V4, CpId received is <%d> , throwing internal error", 0, cpId));
					throw BUPMsg::INTERNAL_ERROR;
				} //TR_IA20800 END

			}

			// Read conf - the supervision time
			//----------------------------------
			const Config::CLUSTER t;
			supervisionTime = Config::instance().getSupervisionTime(t);
			//
			// The above is real code; uncomment when implement V4 BUP
			
			// This is the stub to test t_busrv; remove when implementing BUP V4
			// Read conf - the supervision time
			//----------------------------------
			//supervisionTime = Config::instance().getSupervisionTime(static_cast<CPID>(0xFFFF));
			
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
	new (msg().addr()) BUPMsg::ReadConfRspMsg(transNum, exitcode(), supervisionTime);

	TRACE((LOG_LEVEL_INFO, "ReadConfRspMsg: TransNum = %d exitcode = %d Version Major = %d", 0, transNum, exitcode(), tvm));
	if (!exitcode())
		TRACE((LOG_LEVEL_INFO, "Read supervision time: %d", 0, supervisionTime));
}

#if 0
//
//====================================================================
void ReadConfJob_LEV4::execute() {

	newTRACE(("ReadConfJob_LEV4::execute()", 0));

	u_int32 supervisionTime = 60; // default setting
	CPID cpId = 0xFFFF; // default ONE CP system

	const BUPMsg::ReadConfMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::ReadConfMsg>(msg());
	u_int32 transNum = cmd->transNum(); 
	try {
			// init stuff
		//===================================================================
		if(verifyVersion()) { // if verify fails the errorcode will be set
			//const BUPMsg::ReadConfMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::ReadConfMsg>(msg());
			//transNum = cmd->transNum(); // save for response

			//check if ONE CP System
			if (cmd->cpSystem() != BUPMsg::ONE_CP_SYSTEM)
				cpId = cmd->cpId();

			// Read conf - the supervision time
			//----------------------------------
			supervisionTime = Config::instance().getSupervisionTime((CPID)cpId);
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
	BUPMsg::ReadConfRspMsg* rsp = new (msg().addr())
		BUPMsg::ReadConfRspMsg(transNum, exitcode(), supervisionTime);

	if (!exitcode())
		TRACE(("Read supervision time: %d", 0, supervisionTime));
}
#endif
