#include "stdafx.h"
#include "CPS_BUSRV_Trace.h"
#include "DSDSession.h"
#include "filefunx.h"
#include "DataFile.h"
#include "FallbackMgr.h"


void DSDSession::processMsg()
{
	newTRACE((LOG_LEVEL_INFO, "DSDSession::processMsg()", 0));

	u_int16 apzProfile = Config::instance().getAPZProfile();
	switch(mMsg->msgHeader()->primitive()) {
	case DSDMsg::MAINT_MSG_TEXT_ECHO: {

		// Construct the response in the same space as the original message
        DSDMsg::EchoTestMsg* echoMsg = reinterpret_cast<DSDMsg::EchoTestMsg*>(mMsg->addr());
		DSDMsg::EchoTestRspMsg* echoRsp = new(mMsg->addr()) DSDMsg::EchoTestRspMsg(DSDMsg::OK);

		if (!mSession->sendMsg(mMsg->addr(), DSDMsg::MSG_SIZE_IN_BYTES)) {
			// Log the case here to be done
			string dsd = mSession->getLastErrorText();
			string err = "DSDSession::processMsg encounters: ";
			err += dsd;
			EventReporter::instance().write(err.c_str());
		}

        break;
	}
	case DSDMsg::SET_CBC_RELOAD_FILE: {
		const DSDMsg::SetReloadFileMsg* rMsg = reinterpret_cast<DSDMsg::SetReloadFileMsg*>(mMsg->addr());
		unsigned short id = rMsg->fileNumber();
		SBCId sbcId(id);
		
		DSDMsg::ERROR_CODE exitCode = DSDMsg::RC_FUNCTION_NOT_SUPPORTED;
		if (apzProfile >= 100) {
			if (fileExist(sbcId, exitCode))
				setReloadFile(sbcId, exitCode);
		}
		else
			EventReporter::instance().write("setCBCReloadFile rejected due to APZ Profile < 100");

		TRACE((LOG_LEVEL_INFO, "Message SET_CBC_RELOAD_FILE; sbc = %d; exitCode = %d", 0, id, exitCode));

		mMsg->reset();
		DSDMsg::SetReloadFileRspMsg* rsp = new(mMsg->addr()) DSDMsg::SetReloadFileRspMsg(exitCode);

		if (!mSession->sendMsg(mMsg->addr(), DSDMsg::MSG_SIZE_IN_BYTES)) {
			// Log the case here to be done
			string dsd = mSession->getLastErrorText();
			string err = "DSDSession::processMsg encounters: ";
			err += dsd;
			EventReporter::instance().write(err.c_str());
		}
		break;
	}
	case DSDMsg::CBC_FILE_EXIST: {
        const DSDMsg::CBCFileExistMsg* cbcExistMsg = reinterpret_cast<DSDMsg::CBCFileExistMsg*>(mMsg->addr());
		unsigned short id = cbcExistMsg->fileNumber();
		SBCId sbcId(id);

		DSDMsg::ERROR_CODE exitCode = DSDMsg::RC_FUNCTION_NOT_SUPPORTED;
		unsigned int res = 0;
		if (apzProfile >= 100) 
			res = fileExist(sbcId, exitCode);
		else
			EventReporter::instance().write("CBCFileExists rejected due to APZ Profile < 100");

		TRACE((LOG_LEVEL_INFO, "Message CBC_FILE_EXIST; sbc = %d; exitCode = %d", 0, id, exitCode));

		mMsg->reset();
		DSDMsg::CBCFileExistRspMsg* cbcExistRsp = new(mMsg->addr()) DSDMsg::CBCFileExistRspMsg(id, exitCode, res);

		if (!mSession->sendMsg(mMsg->addr(), DSDMsg::MSG_SIZE_IN_BYTES)) {
			// Log the case here to be done
			string dsd = mSession->getLastErrorText();
			string err = "DSDSession::processMsg encounters: ";
			err += dsd;
			EventReporter::instance().write(err.c_str());
		}
		break;
	}
	default:
		break;
	}
}

bool DSDSession::fileExist(SBCId& sbcId, DSDMsg::ERROR_CODE& error)
{
	const Config::CLUSTER tc;
	bool res = false;
	error = DSDMsg::FILE_NOT_FOUND;

	if (!sbcId.valid())
		return res;

	for (int i = 0; i < 3; i++)
		try {
			res = FileFunx::dirExists(Config::instance().sbcDir(sbcId, tc).c_str());
			error = res? DSDMsg::OK : DSDMsg::FILE_NOT_FOUND;;
			return res;
		}
		catch (...) {
			//error = DSDMsg::FILE_NOT_FOUND;
			//return false;

			// If query fails, perhaps try again
			EventReporter::instance().write("DSDSession::fileExist, FileFunx::dirExists fails");
			error = DSDMsg::INTERNAL_ERROR;
			Sleep(200);
		}
	return res;
}

void DSDSession::setReloadFile(SBCId& sbcId, DSDMsg::ERROR_CODE& error)
{
	error = DSDMsg::INTERNAL_ERROR;
	u_int16 id = sbcId.id();

	try {
		const Config::CLUSTER tc;
		FallbackMgr_V4 man(tc);
		man.setCBCReloadFile(sbcId.id(), error);
	}
	catch (...) {
		EventReporter::instance().write("DSDSession::setReloadFile, Unknown Error");
	}

}
