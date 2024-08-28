/*
 * GASrmSession.cpp
 *
 *  Created on: May 24, 2012
 *      Author: xdtthng
 *
 *  Update: 
 *  2012-12-24  xdthng  Change api method names
 */
 
#include "GASrmSession.h"
#include "GASrmMsg.h"
#include "ACS_DSD_Session.h"
#include "CPS_BUSRV_Trace.h"
#include "SBCId.h"
#include "EnvFMS.h"
#include "FallbackMgr.h"

GASrmSession::GASrmSession(SessionPtr session, GAMsg* msg, size_t size):
	mMsg(msg), mSize(size), mSession(session)
{

}

GASrmSession::~GASrmSession()
{

}

ssize_t GASrmSession::process()
{
	newTRACE((LOG_LEVEL_INFO, "GASrmSession::process()", 0));
	GASrmMsg *srmMsg = reinterpret_cast<GASrmMsg*>(mMsg->addr());

	//u_int16 apzProfile = Config::instance().getAPZProfile();
	switch(srmMsg->msgHeader()->primitive()) {
	case GASrmMsg::SET_SBC_RELOAD_FILE: {
		const GASrmMsg::SetReloadFileMsg* rMsg = reinterpret_cast<GASrmMsg::SetReloadFileMsg*>(mMsg->addr());
		u_int16 id = rMsg->fileNumber();
		mMsg->reset();
		u_int32 exitCode = setReloadFile(id);
		new(mMsg->addr()) GASrmMsg::SetReloadFileRspMsg(exitCode);
		return mSession->sendf(mMsg->addr(), mSize, MSG_NOSIGNAL);
		break;
	}
	case GASrmMsg::SBC_FILE_EXIST: {
        const GASrmMsg::SbcFileExistMsg* sbcExistMsg = reinterpret_cast<GASrmMsg::SbcFileExistMsg*>(mMsg->addr());
        u_int16 id = sbcExistMsg->fileNumber();
		mMsg->reset();
		u_int32 exitCode = fileExist(id);
		new(mMsg->addr()) GASrmMsg::SbcFileExistRspMsg(exitCode);
		return mSession->sendf(mMsg->addr(), mSize, MSG_NOSIGNAL);
		break;
	}
	default:
		break;
	}

	return 0;
}

GASrmMsg::ERROR_CODE GASrmSession::fileExist(u_int16 id)
{
	newTRACE((LOG_LEVEL_INFO, "GASrmSession::fileExist(%d)", 0, id));
	const Config::CLUSTER tc;
	GASrmMsg::ERROR_CODE res = GASrmMsg::RC_FILE_NOT_FOUND;
	SBCId sbcId(id);

	if (!sbcId.valid()) {
		TRACE((LOG_LEVEL_INFO, "GASrmSession::fileExist() return <%d>", 0, GASrmMsg::RC_FILE_OUT_OF_RANGE));
		return GASrmMsg::RC_FILE_OUT_OF_RANGE;
	}

	bool fExist;
	for (int i = 0; i < 3; i++)
		try {
			fExist = EnvFMS::dirExists(sbcId, tc);	// Test for NCD now
			res = fExist? GASrmMsg::RC_OK : GASrmMsg::RC_FILE_NOT_FOUND;
			TRACE((LOG_LEVEL_INFO, "GASrmSession::fileExist() return <%d>", 0, res));
			return res;
		}
		catch (...) {
			//error = DSDMsg::FILE_NOT_FOUND;
			//return false;

			// If query fails, perhaps try again
			//EventReporter::instance().write("DSDSession::fileExist, FileFunx::dirExists fails");
			res = GASrmMsg::RC_INTERNAL_ERROR;
			sleep(2);
		}
		TRACE((LOG_LEVEL_INFO, "GASrmSession::fileExist() return <%d>", 0, res));
		return res;
}

GASrmMsg::ERROR_CODE GASrmSession::setReloadFile(u_int16 id)
{
	newTRACE((LOG_LEVEL_INFO, "GASrmSession::setReloadFile(%d)", 0, id));
	GASrmMsg::ERROR_CODE res = GASrmMsg::RC_FILE_NOT_FOUND;
	SBCId sbcId(id);
	if (!sbcId.valid()) {
		TRACE((LOG_LEVEL_INFO, "GASrmSession::fileExist() return <%d>", 0, GASrmMsg::RC_FILE_OUT_OF_RANGE));
		return GASrmMsg::RC_FILE_OUT_OF_RANGE;
	}

	try {
		const Config::CLUSTER tc;
		FallbackMgr_V4 mgr(tc);
		mgr.setSBCReloadFile(id, res);
	}
	catch (...) {
		EventReporter::instance().write("DSDSession::setReloadFile, Unknown Error");
	}
	return res;
}


