/*
  NAME
  File_name:FcReadGetPathJob.cpp

  COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

  The Copyright to the computer program(s) herein is the property of Ericsson
  Utvecklings AB, Sweden.
  The program(s) may be used and/or copied only with the written permission from
  Ericsson Utvecklings AB or in accordance with the terms and conditions
  stipulated in the agreement/contract under which the program(s) have been
  supplied.

  DESCRIPTION
  Executes the FCP command FC_READ_GET_PATH (configuration).

  DOCUMENT NO
  190 89-CAA 109 0670

  AUTHOR
  2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on FcReadGetPathJob.cpp of FCSRV in Windows

  SEE ALSO
  -

  Revision history
  ----------------
  2011-11-15 xtuudoo Created
  2012-09-11 xngudan Updated for Cluster session

*/

#include "FCPFunx.h"
#include "FileFunx.h"
#include "CodeException.h"
#include "EventReporter.h"
#include "LinuxException.h"
#include "Config.h"
#include "CPS_FCSRV_Trace.h"
#include "FCPMsg.h"
#include "FcReadGetPathJob.h"
#include "PlatformTypes.h"
#include <string>
//
//
//====================================================================
void FcReadGetPathJob::execute() {

	newTRACE((LOG_LEVEL_INFO, "FcReadGetPathJob::execute()", 0));
    using namespace std;

	string fcFile;
	string fcReadPath;
	u_int32 transNum = 0;
	CPID cpId = 0xFFFF; // default ONE CP system
	try {
        // init stuff
		//===================================================================
		if(verifyVersion()) { // if verify fails the errorcode will be set
			const FCPMsg::FcReadGetPathMsg* cmd = FCPFunx::safe_fcp_ptr_cast<FCPMsg::FcReadGetPathMsg>(msg());
			transNum = cmd->transNum(); // save for response
			fcFile = cmd->fileName();
            if (fcFile.size() == 0) throw FCPMsg::FILE_DO_NOT_EXIST;
            //check if ONE CP System
            if (cmd->cpSystem() != FCPMsg::ONE_CP_SYSTEM)
            	cpId = cmd->cpId();

            // create directory structure for this CP
            Config::instance().createDirStructure(cpId);
                
            // check if file exists
            string fcFileandpath = Config::instance().ftpDir(cpId).c_str();
            fcFileandpath.append(fcFile.c_str());
            if( FileFunx::dirExists(fcFileandpath.c_str()) == false )
            	throw FCPMsg::FILE_DO_NOT_EXIST;

            fcReadPath = Config::instance().ftpReadDir(cpId).c_str();
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
            TRACE((LOG_LEVEL_ERROR, "FcReadGetPathJob::execute() CS error ", 0));
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
	FtpFileSet pathName;
	if(m_exitcode == FCPMsg::OK)
		FCPFunx::setFiles(pathName, fcReadPath,  fcFile.c_str());

	new (msg().addr()) FCPMsg::FcReadGetPathRspMsg(transNum, exitcode(), pathName);

	TRACE((LOG_LEVEL_INFO, "FcReadGetPathRsp: TransNum = %d exitcode = %d", 0, transNum, exitcode()));
	if (!exitcode())
		TRACE((LOG_LEVEL_INFO, "Exit with error code: %d", 0, m_exitcode));

	TRACE((LOG_LEVEL_INFO, "FcReadGetPathJob::execute() returns", 0));
}


//
//
//====================================================================
void FcReadGetPathJob_V4::execute() {

	newTRACE((LOG_LEVEL_INFO, "FcReadGetPathJob_V4::execute()", 0));
    using namespace std;

	string fcFile;
	string fcReadPath;
	u_int32 transNum = 0;
	try {
        // init stuff
		//===================================================================
		if(verifyVersion()) { // if verify fails the errorcode will be set
			const FCPMsg::FcReadGetPathMsg* cmd = FCPFunx::safe_fcp_ptr_cast<FCPMsg::FcReadGetPathMsg>(msg());
			transNum = cmd->transNum(); // save for response
			fcFile = cmd->fileName();
            if (fcFile.size() == 0) throw FCPMsg::FILE_DO_NOT_EXIST;

			const Config::CLUSTER t;
			// create directory structure for this Cluster
			Config::instance().createDirStructure(t);

			// check if file exists
			string fcFileandpath = Config::instance().ftpDir(t).c_str();
			fcFileandpath.append(fcFile.c_str());
			if( FileFunx::dirExists(fcFileandpath.c_str()) == false )
				throw FCPMsg::FILE_DO_NOT_EXIST;

			fcReadPath = Config::instance().ftpReadDir(t).c_str();
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
            TRACE((LOG_LEVEL_ERROR, "FcReadGetPathJob::execute() CS error ", 0));
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
	FtpFileSet pathName;
	if(m_exitcode == FCPMsg::OK)
		FCPFunx::setFiles(pathName, fcReadPath,  fcFile.c_str());

	new (msg().addr()) FCPMsg::FcReadGetPathRspMsg(transNum, exitcode(), pathName);

	if (!exitcode())
		TRACE((LOG_LEVEL_INFO, "Exit with error code: %d", 0, m_exitcode));
}
