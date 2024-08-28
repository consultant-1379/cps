/*
NAME
   File_name:BUPJobFactory.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

   The Copyright to the computer program(s) herein is the property of Ericsson
   Utvecklings AB, Sweden.
   The program(s) may be used and/or copied only with the written permission from
   Ericsson Utvecklings AB or in accordance with the terms and conditions
   stipulated in the agreement/contract under which the program(s) have been
   supplied.

DESCRIPTION
   Creates Job that corresponds to an incoming BUPMsg (BUP command).

DOCUMENT NO
   190 89-CAA 109 0387

AUTHOR
   2002-02-05 by UAB/KY/SK Anders Gillgren (qabgill, tag :ag)

SEE ALSO
   -

Revision history
----------------
2002-02-05 qabgill Created
2002-10-20 uabmnst Add ACSTrace

*/

#include "Config.h"
#include "BUPJobFactory.h"

#include "buap.h"
#include "CodeException.h"
#include "CPS_BUSRV_Trace.h"
#include "EventReporter.h"

// include all jobs

#include "FallbackBeginJob.h"
#include "FallbackEndJob.h"
#include "LoadBeginJob.h"
#include "LoadEndJob.h"
#include "UnknownJob.h"
#include "DumpBeginJob.h"
#include "DumpEndJob.h"
#include "ListSBCsJob.h"
#include "SBCSwitchJob.h"
#include "VerifySBCDataJob.h"
#include "SBCInfoJob.h"
#include "ReadConfJob.h"
#include "ReadCmdLogConfJob.h"
#include "ReadCmdLogDataJob.h"
#include "WriteCmdLogDataJob.h"
#include "WriteALogDataJob.h"

#include "BUPFunx.h"
#include "Version.h"

//
// ctor
//====================================================================
BUPJobFactory::~BUPJobFactory() {
	
	newTRACE((LOG_LEVEL_INFO, "BUPJobFactory::~BUPJobFactory() dtor", 0));
	delete m_job; 
}

//
// ctor
//====================================================================
BUPJobFactory::BUPJobFactory(const BUPMsg& msg) {
	
	newTRACE((LOG_LEVEL_INFO, "BUPJobFactory::BUPJobFactory(%d)", 0, msg.msgHeader()->primitive()));

	Version tProtVer = msg.requestHeader()->version();	
	TRACE((LOG_LEVEL_INFO, "BUPJobFactory, Prot Version Major <%d>", 0, tProtVer.major()));

	switch(msg.msgHeader()->primitive()) {
	
	case BUPMsg::LOAD_BEGIN: 
		TRACE((LOG_LEVEL_INFO, "BUPJobFactory::BUPJobFactory(); LOAD_BEGIN received", 0));
		
		if (tProtVer >= Version(4, 0))
			m_job = new LoadBeginJob_V4(msg);
		else 
			m_job = new LoadBeginJob(msg);
		break;
			
	case BUPMsg::LOAD_END: {
		TRACE((LOG_LEVEL_INFO, "BUPJobFactory::BUPJobFactory(); LOAD_END received", 0));

		if (tProtVer >= Version(4, 0)) 
			m_job = new LoadEndJob_V4(msg);
		else
			m_job = new LoadEndJob(msg);                    	
		break;
	}
	
	case BUPMsg::FALLBACK_BEGIN: {
		TRACE((LOG_LEVEL_INFO, "FALLBACK_BEGIN received", 0));

		if (tProtVer >= Version(4, 0))
            m_job = new FallbackBeginJob_V4(msg);
			//m_job = new UnknownJob(msg);
        else
            m_job = new FallbackBeginJob(msg);
		break;
	}
	
	case BUPMsg::FALLBACK_END: {
		TRACE((LOG_LEVEL_INFO, "FALLBACK_END received", 0));

		if (tProtVer >= Version(4, 0))
            m_job = new FallbackEndJob_V4(msg);
        else
            m_job = new FallbackEndJob(msg);
		break;
	}

	case BUPMsg::DUMP_BEGIN: {
		TRACE((LOG_LEVEL_INFO, "DUMP_BEGIN received", 0));

		if (tProtVer >= Version(4, 0))
			m_job = new DumpBeginJob_V4(msg);
		else
			m_job = new DumpBeginJob(msg);
		break;				
	}
	case BUPMsg::DUMP_END: {
		TRACE((LOG_LEVEL_INFO, "DUMP_END received", 0));
		
		if (tProtVer >= Version(4, 0))
			m_job = new DumpEndJob_V4(msg);
		else
			m_job = new DumpEndJob(msg);
		break;						
	}
	
	case BUPMsg::READ_CONF: {
		TRACE((LOG_LEVEL_INFO, "READ_CONF received", 0));
		
		if (tProtVer >= Version(4, 0))
        	m_job = new ReadConfJob_V4(msg);
        else
        	m_job = new ReadConfJob(msg);
		break;
	}
	
	case BUPMsg::READ_CMDLOG_CONF: {
		TRACE((LOG_LEVEL_INFO, "READ_CMDLOG_CONF received", 0));
		
		//if (tProtVer >= Version(4, 0))
        //    m_job = new ReadCmdLogConfJob_V4(msg);
        //else
            m_job = new ReadCmdLogConfJob(msg);
		break;
	}
	
	case BUPMsg::READ_CMDLOG_DATA: {
		TRACE((LOG_LEVEL_INFO, "READ_CMDLOG_DATA received", 0));
		
		//if (tProtVer >= Version(4, 0))
		//    m_job = new ReadCmdLogDataJob_V4(msg);
		//else
            m_job = new ReadCmdLogDataJob(msg);
		break;
	}
	
	case BUPMsg::WRITE_CMDLOG_DATA: {
		TRACE((LOG_LEVEL_INFO, "WRITE_CMDLOG_DATA received", 0));
		
		if (tProtVer >= Version(4, 0))
		    m_job = new WriteCmdLogDataJob_V4(msg);
		else
            m_job = new WriteCmdLogDataJob(msg);
		break;
	}

	case BUPMsg::SBC_SWITCH: {
		TRACE((LOG_LEVEL_INFO, "SBC_SWITCH received", 0));

		if (tProtVer >= Version(4, 0))
		    m_job = new SBCSwitchJob_V4(msg);
			//m_job = new UnknownJob(msg);
		else
            m_job = new SBCSwitchJob(msg);
		break;
	}

	case BUPMsg::LIST_SBCS: {
		TRACE((LOG_LEVEL_INFO, "LIST_SBCS received", 0));
		
		if (tProtVer >= Version(4, 0))
			m_job = new ListSBCsJob_V4(msg);
		else
			m_job = new ListSBCsJob(msg);			
		break;				
		
	}

	case BUPMsg::VERIFY_SBC_DATA: {
        TRACE((LOG_LEVEL_INFO, "VERIFY_SBC_DATA received", 0));
        
		if (tProtVer >= Version(4, 0))
		    m_job = new VerifySBCDataJob_V4(msg);
			//m_job = new UnknownJob(msg);
		else
            m_job = new VerifySBCDataJob(msg);
		break;
	}
	
 
	case BUPMsg::SBC_INFO: {
		TRACE((LOG_LEVEL_INFO, "SBC_INFO received", 0));

		if (tProtVer >= Version(4, 0))
		    m_job = new SBCInfoJob_V4(msg);
		else
            m_job = new SBCInfoJob(msg);
		break;
	}
	
	case BUPMsg::WRITE_ALOG_DATA: {
		TRACE((LOG_LEVEL_INFO, "WRITE_ALOG_DATA received", 0));

		if (tProtVer >= Version(4, 0))
			m_job = new WriteALogDataJob_V4(msg);
		else
			m_job = new UnknownJob(msg);
		break;
	}

	case BUPMsg::SBC1_TO_SFR: {
		TRACE((LOG_LEVEL_INFO, "SBC1_TO_SFR == TestMsg Test Hack received", 0));
		m_job = new UnknownTestJob(msg);
		break;
	}
	default:
		TRACE((LOG_LEVEL_WARN, "Unknown BUP primitive: %d ", 0, msg.msgHeader()->primitive()));
		m_job = new UnknownJob(msg);
		break;
	}
}
