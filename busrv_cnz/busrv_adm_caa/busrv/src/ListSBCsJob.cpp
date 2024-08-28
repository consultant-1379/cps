/*
NAME
	File_name:ListSBCsJob.cpp

	COPYRIGHT Ericsson Utvecklings AB, Sweden 2002-2023. All rights reserved.
 
	The Copyright to the computer program(s) herein is the property of Ericsson
	Utvecklings AB, Sweden.
	The program(s) may be used and/or copied only with the written permission from
	Ericsson Utvecklings AB or in accordance with the terms and conditions
	stipulated in the agreement/contract under which the program(s) have been
	supplied.

	DESCRIPTION
	Executes the BUP command LIST_SBCS.

	DOCUMENT NO
	190 89-CAA 109 0387

	AUTHOR
	2002-02-05 by UAB/KY/SK Anders Gillgren (qabgill, tag :ag)

	SEE ALSO

	Revision history
	----------------
	2002-02-05 qabgill Created
	2002-10-31 uablan  Added ACS_Trace
	2010-04-15 xdtthng SSI added
	2012-12-24 xdtthng Updated for APG43L BC
        2023-01-01 xkomala  Added checks for valid cpid for TR IA20800
		  
*/

#include "Config.h"
#include "ListSBCsJob.h"

#include "CPS_BUSRV_Trace.h"
#include "BUPFunx.h"
#include "CodeException.h"
#include "EventReporter.h"
#include "SBCList.h"
#include "LinuxException.h"

#ifndef LOCAL_BUILD					
#include "fms_cpf_file.h"
#endif

//
// execute
//====================================================================
void ListSBCsJob::execute() {

	newTRACE((LOG_LEVEL_INFO, "ListSBCsJob::execute()", 0));
	
	SBCList sbcList;
	CPID cpId = 0xFFFF; // default ONE CP system
	const BUPMsg::ListSBCsMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::ListSBCsMsg>(msg());
	u_int32 transNum = cmd->transNum(); 
	u_int16 tvm = cmd->version().major();
	
	try {
		// init stuff
		//===================================================================
		if(verifyVersion()) { // if verify fails the errorcode will be set
			//const BUPMsg::ListSBCsMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::ListSBCsMsg>(msg());
			//transNum = cmd->transNum(); // save for response

			//check if ONE CP System
			if (cmd->cpSystem() != BUPMsg::ONE_CP_SYSTEM)
			{
				cpId = cmd->cpId();

				//TR_IA20800 START
				if(!(Config::instance().isValidSpxCpid(cpId)))
				{
					TRACE((LOG_LEVEL_ERROR, "ListSBCsJob, CpId received is <%d> ,throwing internal error", 0, cpId));
					throw BUPMsg::INTERNAL_ERROR;
				} //TR_IA20800 END

			}

			// check rules
			//===================================================================
			if(cmd->range() != SBCId::FIRST &&
				cmd->range() != SBCId::SECOND &&
				cmd->range() != SBCId::COMPLETE)
				throw BUPMsg::PARAM_SBC_RANGE_INVALID;
			
			// Get identity for all existing SBC
			sbcList.update(cmd->range(), cmd->firstSequence(), cpId);
		}
	}
	//
	// catch all error & set exitcode
	//
	catch(BUPMsg::ERROR_CODE& exitcode) {
		m_exitcode = exitcode;
	}
#ifndef LOCAL_BUILD					
	catch(FMS_CPF_Exception& x) {
		stringstream str;
		str << "ListSBCsJob, FMS Errorcode : " << x.errorCode() << ", Text: " << x.errorText() << ", Details: "
		    << x.detailInfo() << endl;
		newTRACE((LOG_LEVEL_ERROR, "%s", 0, str.str().c_str()));
		m_exitcode = BUPMsg::INTERNAL_ERROR;
		
		// Remove this logging for final release
		EventReporter::instance().write(str.str().c_str());		
	}
#endif
	catch(CodeException& x) {
		switch(x.errcode()) {
		case CodeException::CONFIGURATION_ERROR:
			m_exitcode = BUPMsg::SBC_ROOT_NOT_FOUND;
			break;
		case CodeException::CP_APG43_CONFIG_ERROR:
			TRACE((LOG_LEVEL_ERROR, "ListSBCsJob::execute() CS error ", 0));
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

	// create response in the msg buffer (re-use)
	new (msg().addr()) BUPMsg::ListSBCsRspMsg(transNum, exitcode(), sbcList.count(), sbcList.sbcIds());

	TRACE((LOG_LEVEL_INFO, "ListSBCsRspMsg: TransNum = %d exitcode = %d Version Major = %d", 0, transNum, exitcode(), tvm));
	if (!exitcode())
		TRACE((LOG_LEVEL_INFO, "No of SBC in list: %d", 0, sbcList.count()));
}
//
//
//====================================================================
void ListSBCsJob_V4::execute() {

    newTRACE((LOG_LEVEL_INFO, "ListSBCsJob_V4::execute()", 0));
    
    SBCList sbcList;    
    const BUPMsg::ListSBCsMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::ListSBCsMsg>(msg());
    u_int32 transNum = cmd->transNum(); // save for response
	u_int16 tvm = cmd->version().major();
	CPID cpId = cmd->cpId();

    try {

    	//TR_IA20800 START
    	if(!(Config::instance().isValidBladeCpid(cpId)))
    	{
    		TRACE((LOG_LEVEL_ERROR, "ListSBCJob_V4, CpId received is <%d> , throwing internal error", 0, cpId));
    		throw BUPMsg::INTERNAL_ERROR;
    	} //TR_IA20800 END

        // init stuff
        //===================================================================
        if(verifyVersion()) { // if verify fails the errorcode will be set

        	TRACE((LOG_LEVEL_INFO, "ListSBCsJob_V4, cpId = %d, CP System = %d", 0, cpId, cmd->cpSystem()));

            // Check rules
            // THNG: SBCList enscapsulate the range rules for SSI and non SSI
            //===============================================================
            if(cmd->range() != SBCId::FIRST &&
                cmd->range() != SBCId::SECOND &&
                cmd->range() != SBCId::COMPLETE)
                throw BUPMsg::PARAM_SBC_RANGE_INVALID;
            
            // Get identity for all existing SBC
			const Config::CLUSTER t;
            sbcList.update(cmd->range(), cmd->firstSequence(), t);
        }
    }
    //
    // catch all error & set exitcode
    //
    catch(BUPMsg::ERROR_CODE exitcode) {
        m_exitcode = exitcode;
    }
#ifndef LOCAL_BUILD					
	catch(FMS_CPF_Exception& x) {
		stringstream str;
		str << "ListSBCsJob_V4, FMS Errorcode : " << x.errorCode() << ", Text: " << x.errorText() << ", Details: "
		    << x.detailInfo() << endl;
		newTRACE((LOG_LEVEL_ERROR, "%s", 0, str.str().c_str()));
		m_exitcode = BUPMsg::INTERNAL_ERROR;
		
		// Remove this logging for final release
		EventReporter::instance().write(str.str().c_str());		
	}
#endif
    catch(CodeException& x) {
        switch(x.errcode()) {
        case CodeException::CONFIGURATION_ERROR:
            m_exitcode = BUPMsg::SBC_ROOT_NOT_FOUND;
            break;
        case CodeException::CP_APG43_CONFIG_ERROR:
            TRACE((LOG_LEVEL_ERROR, "ListSBCsJob_V4::execute() CS error ", 0));
            m_exitcode = BUPMsg::INTERNAL_ERROR;
            break;
        default:
            m_exitcode = BUPMsg::INTERNAL_ERROR;
            TRACE((LOG_LEVEL_ERROR, "ListSBCsJob_V4: CodeExcpetion error code: %d", 0, x.errcode()));
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
    new (msg().addr()) BUPMsg::ListSBCsRspMsg(transNum, exitcode(), sbcList.count(), sbcList.sbcIds());

	TRACE((LOG_LEVEL_INFO, "ListSBCsRspMsg: TransNum = %d exitcode = %d Version Major = %d", 0, transNum, exitcode(), tvm));
	if (!exitcode())
		TRACE((LOG_LEVEL_INFO, "No of SBC in list: %d", 0, sbcList.count()));
}
