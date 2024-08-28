/*
  NAME
  File_name:FcWriteGetPathJob.cpp

  COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

  The Copyright to the computer program(s) herein is the property of Ericsson
  Utvecklings AB, Sweden.
  The program(s) may be used and/or copied only with the written permission from
  Ericsson Utvecklings AB or in accordance with the terms and conditions
  stipulated in the agreement/contract under which the program(s) have been
  supplied.

  DESCRIPTION
  Executes the FCP command FC_WRITE_GET_PATH (configuration).

  DOCUMENT NO
  190 89-CAA 109 0670

  AUTHOR
  2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on FcWriteGetPathJob.cpp of FCSRV in Windows

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
#include "FcWriteGetPathJob.h"
#include "PlatformTypes.h"
//
//
//====================================================================
void FcWriteGetPathJob::execute() {

	newTRACE((LOG_LEVEL_INFO, "FcWriteGetPathJob::execute()", 0));
    using namespace std;

	string fcFile;
	string fcWritePath;
	u_int32 fileFlag = 0; // default setting
	u_int32 transNum = 0;
	CPID cpId = 0xFFFF; // default ONE CP system
	try {
        // init stuff
		//===================================================================
		if(verifyVersion()) { // if verify fails the errorcode will be set
			const FCPMsg::FcWriteGetPathMsg* cmd = FCPFunx::safe_fcp_ptr_cast<FCPMsg::FcWriteGetPathMsg>(msg());
			transNum = cmd->transNum(); // save for response
			fcFile = cmd->fileName();
            TRACE((LOG_LEVEL_INFO, "FcWriteGetPathJob::execute(): File name = %s", 0, fcFile.c_str()));
                    
			//check if ONE CP System
			if (cmd->cpSystem() != FCPMsg::ONE_CP_SYSTEM)
				cpId = cmd->cpId();

			// create directory structure for this CP
			Config::instance().createDirStructure(cpId);

			// check if file exists
			string fcFileandpath = Config::instance().ftpDir(cpId).c_str();
			fcFileandpath.append(fcFile.c_str());
			if( FileFunx::dirExists(fcFileandpath.c_str()) )
				fileFlag = 0;
			else
				fileFlag = 1;

			fcWritePath = Config::instance().ftpWriteDir(cpId).c_str();
		}
	}
	//
	// catch all error & set exitcode
	//
	catch(FCPMsg::ERROR_CODE &exitcode) {
		m_exitcode = exitcode;
		TRACE((LOG_LEVEL_ERROR, "FcWriteGetPathJob::execute() FC error %d", 0, m_exitcode));
	}
	catch(LinuxException& x) {
		EventReporter::instance().write(x);
            m_exitcode = FCPMsg::INTERNAL_ERROR;		
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
      FCPFunx::setFiles(pathName, fcWritePath.c_str(),  fcFile.c_str());

	// Using placement new to construct an object on memory that's already allocated
	new (msg().addr()) FCPMsg::FcWriteGetPathRspMsg(transNum, exitcode(), pathName, fileFlag);
        TRACE((LOG_LEVEL_INFO, "FcWriteGetPathRsp: TransNum = %d exitcode = %d", 0, transNum, exitcode()));

	if (!exitcode()){
		TRACE((LOG_LEVEL_INFO, "File exists flag (0=exists,1=not exists): %d", 0, fileFlag));
        }

	TRACE((LOG_LEVEL_INFO, "FcWriteGetPathJob::execute() returns", 0));
}


//
//====================================================================
void FcWriteGetPathJob_V4::execute() {

	newTRACE((LOG_LEVEL_INFO, "FcWriteGetPathJob_V4::execute()", 0));
    using namespace std;

	string fcFile;
	string fcWritePath;
	u_int32 fileFlag = 0; // default setting
	u_int32 transNum = 0;

	try {
        // init stuff
		//===================================================================
		if(verifyVersion()) { // if verify fails the errorcode will be set
			const FCPMsg::FcWriteGetPathMsg* cmd = FCPFunx::safe_fcp_ptr_cast<FCPMsg::FcWriteGetPathMsg>(msg());
			transNum = cmd->transNum(); // save for response
			fcFile = cmd->fileName();
            TRACE((LOG_LEVEL_INFO, "FcWriteGetPathJob::execute(): File name = %s", 0, fcFile.c_str()));

			const Config::CLUSTER t;
			// create directory structure for this CLUSTER
			Config::instance().createDirStructure(t);

			// check if file exists
			string fcFileandpath = Config::instance().ftpDir(t).c_str();
			fcFileandpath.append(fcFile.c_str());
			if( FileFunx::dirExists(fcFileandpath.c_str()) )
				fileFlag = 0;
			else
				fileFlag = 1;

			fcWritePath = Config::instance().ftpWriteDir(t).c_str();
		}
	}
	//
	// catch all error & set exitcode
	//
	catch(FCPMsg::ERROR_CODE &exitcode) {
		m_exitcode = exitcode;
		TRACE((LOG_LEVEL_ERROR, "FcWriteGetPathJob::execute() FC error %d", 0, m_exitcode));
	}
	catch(LinuxException& x) {
		EventReporter::instance().write(x);
            m_exitcode = FCPMsg::INTERNAL_ERROR;
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
      FCPFunx::setFiles(pathName, fcWritePath.c_str(),  fcFile.c_str());

	new (msg().addr()) FCPMsg::FcWriteGetPathRspMsg(transNum, exitcode(), pathName, fileFlag);

	if (!exitcode()) {
		TRACE((LOG_LEVEL_INFO, "File exists flag (0=exists,1=not exists): %d", 0, fileFlag));
    }
}
