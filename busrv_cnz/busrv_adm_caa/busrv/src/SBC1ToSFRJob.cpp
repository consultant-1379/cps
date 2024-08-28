/*
NAME
   File_name:SBC1ToSFRJob.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

   The Copyright to the computer program(s) herein is the property of Ericsson
   Utvecklings AB, Sweden.
   The program(s) may be used and/or copied only with the written permission from
   Ericsson Utvecklings AB or in accordance with the terms and conditions
   stipulated in the agreement/contract under which the program(s) have been
   supplied.

DESCRIPTION
   Executes the BUP command SBC1_TO_SFR.

DOCUMENT NO
   190 89-CAA 109 0387

AUTHOR
   2002-02-05 by UAB/KY/SK Anders Gillgren (qabgill, tag :ag)

SEE ALSO
   

Revision history
----------------
2002-02-05 qabgill Created

*/

#include "stdafx.h"

#include "SBC1ToSFRJob.h"

#include "BUPFunx.h"
#include <new.h>
//
//
//===========================================================================
void SBC1ToSFRJob::execute() {
	m_exitcode = BUPMsg::INTERNAL_ERROR;
	msg().reset();
	BUPMsg::SBC1ToSFRRspMsg* rsp = new (msg().addr())
		BUPMsg::SBC1ToSFRRspMsg(msg().msgHeader()->transNum(), exitcode());
}
//
//
//===========================================================================
void SBC1ToSFRJob::copy(bool overwrite) {
	/*
	SBCList		sbcSFR(BUPMsg::SECOND_FILE_RANGE, true);
	u_int16		highestSBCId;

	try {
		// check if any file exists in SFR
		//=====================================================================
		if(sbcSFR.count() <= 0)
			throw BUPMsg::SWITCH_INSUFFICIENT_NOF_SBCS;

		// get the highest consecutive SBC in SFR
		//=====================================================================
		highestSBCId = sbcSFR.backupId(sbcSFR.count() - 1);

		// check if relfsw1 exists
		//=====================================================================
		string backupDir = Config::instance().sbcDir(FIRST_BACKUP_ID+1); 
		if(!FileFunx::dirExists(backupDir.c_str()))
			throw BUPMsg::SBC_NOT_FOUND;

		// reserve SBC1
		//=====================================================================
		SBCFunx::reserve(1, hSBC1); // may throw

		// copy relfsw1 to highest existing file in SFR
		//=====================================================================
		SBCFunx::copySBC(1, highestSBCId, overwrite);

		// unreserve SBC1
		//=====================================================================
		SBCFunx::unreserve(hSBC1);
	}
	//
	// catch all error & set exitcode
	//========================================================================
	catch(BUPMsg::ERROR_CODE exitcode) {
		throw exitcode;
	}
	catch(Win32Exception& x) {
		(void) x;
		throw BUPMsg::INTERNAL_ERROR;		
	}
	catch(CodeException& x) {
		switch(x.errcode()) {
			case CodeException::BAD_BACKUPINFO_FILE:
				throw BUPMsg::BAD_BACKUPINFO_FILE;
				break;
			case CodeException::REQ_SBC_NOT_FOUND:
				throw BUPMsg::SBC_NOT_FOUND;
				break;				
			case CodeException::DIR_RESERVE_FAILED:
				throw BUPMsg::SBC_RESERVE_FAILED;
				break;				
			case CodeException::REQ_FILE_MISSING:
				throw BUPMsg::SWITCH_FILE_MISSING_LOW;
				break;
			case CodeException::CONFIGURATION_ERROR:
				throw BUPMsg::INTERNAL_ERROR;// ska det vara en annan felkod???
				break;
			default:
				throw BUPMsg::INTERNAL_ERROR;
				break;
		}
	}
	catch(...) {
		throw BUPMsg::INTERNAL_ERROR;
	}
	*/
}
