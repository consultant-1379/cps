/*
NAME
	File_name:WriteALogDataJob.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

The Copyright to the computer program(s) herein is the property of Ericsson
Utvecklings AB, Sweden.
The program(s) may be used and/or copied only with the written permission from
Ericsson Utvecklings AB or in accordance with the terms and conditions
stipulated in the agreement/contract under which the program(s) have been
supplied.

DESCRIPTION
	Executes the BUP command WRITE_ALOG_DATA.

DOCUMENT NO
	190 89-CAA 109 xxxx

AUTHOR
	2010-05-01 XDT/DEK/Thanh Nguyen (Tag: THNG)

SEE ALSO
	

Revision history
----------------
2002-05-02 qabgill Created
2002-11-04 uablan  Add ACS Trace
2007-03-06 uablan  Updates for Multiple CP System
*/

#include "WriteALogDataJob.h"

#include "BUPFunx.h"
#include "CodeException.h"
#include "EventReporter.h"
#include "LinuxException.h"
#include "CPS_BUSRV_Trace.h"
#include "Config.h"

#include "acs_alog_drainer.h"
#include "BUPMsg.h"



///////////////////////////////////////////////////////////////////////////////
//
//
//====================================================================
void WriteALogDataJob_V4::execute() {

	newTRACE((LOG_LEVEL_INFO, "WriteALogDataJob_V4::execute()", 0));

	const BUPMsg::WriteALogDataMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::WriteALogDataMsg>(msg());
	u_int32 transNum = cmd->transNum(); // save for response
	u_int16 tvm = cmd->version().major();

	try {
		// if verify fails the errorcode will be set
		if(verifyVersion()) {

			acs_alog_Drainer logger;
			logger.logData(MMLCmdLog,
				reinterpret_cast<const char*>(cmd->character()), "CP-PLEX", "busrv",
				cmd->nofCharacters() );
			
			TRACE((LOG_LEVEL_INFO, "ALog Data is <%d><%s>",0, cmd->nofCharacters(), cmd->getData().toStr().c_str()));
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
			TRACE((LOG_LEVEL_ERROR, "WriteALogDataJob::execute() CS error ", 0));
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
	new (msg().addr()) BUPMsg::WriteALogDataRspMsg(transNum, exitcode());

	TRACE((LOG_LEVEL_INFO, "WriteALogDataRspMsg: TransNum = %d exitcode = %d Version Major = %d", 0, transNum, exitcode(), tvm));
}
