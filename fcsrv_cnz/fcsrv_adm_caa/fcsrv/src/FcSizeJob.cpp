/*
  NAME
  File_name:FcSizeJob.cpp

  COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

  The Copyright to the computer program(s) herein is the property of Ericsson
  Utvecklings AB, Sweden.
  The program(s) may be used and/or copied only with the written permission from
  Ericsson Utvecklings AB or in accordance with the terms and conditions
  stipulated in the agreement/contract under which the program(s) have been
  supplied.

  DESCRIPTION
  Executes the FCP command FC_SIZE (configuration).

  DOCUMENT NO
  190 89-CAA 109 0670

  AUTHOR
  2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on FcSizeJob.cpp of FCSRV in Windows

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
#include "FcSizeJob.h"
#include "PlatformTypes.h"
//
//
//====================================================================
void FcSizeJob::execute() {

	newTRACE((LOG_LEVEL_INFO, "FcSizeJob::execute()", 0));
    using namespace std;

	string fcfile;
	u_int64 fileSize = 0; // default setting
	u_int32 transNum = 0;
	CPID cpId = 0xFFFF; // default ONE CP system
	try {
        // init stuff
		//===================================================================
		if(verifyVersion()) { // if verify fails the errorcode will be set
			const FCPMsg::FcSizeMsg* cmd = FCPFunx::safe_fcp_ptr_cast<FCPMsg::FcSizeMsg>(msg());
			transNum = cmd->transNum(); // save for response
			fcfile = cmd->fileName();
			//check if ONE CP System
			if (cmd->cpSystem() != FCPMsg::ONE_CP_SYSTEM)
				cpId = cmd->cpId();

			// create directory structure for this CP
			Config::instance().createDirStructure(cpId);

			// Check if file exist
			//----------------------------------
			string fcFileandpath = Config::instance().ftpDir(cpId).c_str();
			fcFileandpath.append(fcfile.c_str());
			if( FileFunx::dirExists(fcFileandpath.c_str()) == true ) {
				// open the file
				int file = FileFunx::CreateFileX(fcFileandpath.c_str());

				// get file size
				fileSize = FileFunx::getFileSize(fcFileandpath.c_str());
				// TODO:
				FileFunx::CloseFileX(file);
			}
			else
				throw FCPMsg::FILE_DO_NOT_EXIST;
		}
	}
	//
	// catch all error & set exitcode
	//
	catch(FCPMsg::ERROR_CODE &exitcode) {
		m_exitcode = exitcode;
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
	//low byte is defined as byte which contains least significant value, as oppose to high byte
	u_int32 fileSizeHigh = fileSize >> 8;//only take higher 8 bit, so it needs to be shifted
	u_int32 fileSizeLow = fileSize; //only take lower 8 bit

	msg().reset();
	new (msg().addr()) FCPMsg::FcSizeRspMsg(transNum, exitcode(), fileSizeHigh, fileSizeLow);

	if (!exitcode()){
		TRACE((LOG_LEVEL_INFO, "File size high: %d, low: %d", 0, fileSizeHigh, fileSizeLow));
	}
	
    	//TRACE((LOG_LEVEL_ERROR, "Error exit code: %d", 0, m_exitcode));
        TRACE((LOG_LEVEL_INFO, "FcSizeRsp: TransNum = %d exitcode = %d", 0, transNum, exitcode()));

	TRACE((LOG_LEVEL_INFO, "FcSizeJob::execute() returns", 0));
}


//
//
//====================================================================
void FcSizeJob_V4::execute() {

	newTRACE((LOG_LEVEL_INFO, "FcSizeJob_V4::execute()", 0));
    using namespace std;

	string fcfile;
	u_int64 fileSize = 0; // default setting
	u_int32 transNum = 0;

	try {
        // init stuff
		//===================================================================
		if(verifyVersion()) { // if verify fails the errorcode will be set
			const FCPMsg::FcSizeMsg* cmd = FCPFunx::safe_fcp_ptr_cast<FCPMsg::FcSizeMsg>(msg());
			transNum = cmd->transNum(); // save for response
			fcfile = cmd->fileName();
			const Config::CLUSTER t;

			// create directory structure for this Cluster
			Config::instance().createDirStructure(t);

			// Check if file exist
			//----------------------------------
			string fcFileandpath = Config::instance().ftpDir(t).c_str();
			fcFileandpath.append(fcfile.c_str());
			if( FileFunx::dirExists(fcFileandpath.c_str()) == true ) {
				// open the file
				int file = FileFunx::CreateFileX(fcFileandpath.c_str());

				// get file size
				fileSize = FileFunx::getFileSize(fcFileandpath.c_str());
				// TODO:
				FileFunx::CloseFileX(file);

			}
			else
				throw FCPMsg::FILE_DO_NOT_EXIST;
		}
	}
	//
	// catch all error & set exitcode
	//
	catch(FCPMsg::ERROR_CODE &exitcode) {
		m_exitcode = exitcode;
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
	//low byte is defined as byte which contains least significant value, as oppose to high byte
	u_int32 fileSizeHigh = fileSize >> 8;//only take higher 8 bit, so it needs to be shifted
	u_int32 fileSizeLow = fileSize; //only take lower 8 bit

	msg().reset();
	new (msg().addr()) FCPMsg::FcSizeRspMsg(transNum, exitcode(), fileSizeHigh, fileSizeLow);

	if (!exitcode()){
		TRACE((LOG_LEVEL_INFO, "File size high: %d, low: %d", 0, fileSizeHigh, fileSizeLow));
	}
	else
    	TRACE((LOG_LEVEL_ERROR, "Error exit code: %d", 0, m_exitcode));
}
