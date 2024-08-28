/*
 * GAMaintSession.cpp
 *
 *  Created on: May 22, 2012
 *      Author: xdtthng
 */

#include "GAMaintSession.h"
#include "GAMaintMsg.h"
#include "ACS_DSD_Session.h"
#include "CPS_BUSRV_Trace.h"


GAMaintSession::GAMaintSession(SessionPtr session, GAMsg* msg, size_t size):
	mMsg(msg), mSize(size), mSession(session)
{
}

GAMaintSession::~GAMaintSession()
{
}

ssize_t GAMaintSession::process()
{
	newTRACE((LOG_LEVEL_INFO, "GAMaintSession::process()", 0));

	GAMaintMsg *maintMsg = reinterpret_cast<GAMaintMsg*>(mMsg->addr());
	if (maintMsg->msgHeader()->primitive() != GAMaintMsg::MAINT_MSG_TEXT_ECHO) {
		TRACE((LOG_LEVEL_ERROR, "GAMaintSession::process(), unsupported primitive", 0));
		return -1;
	}
	GAMaintMsg::EchoTestMsg* echoMsg = reinterpret_cast<GAMaintMsg::EchoTestMsg*>(mMsg->addr());
	string str(echoMsg->characters(), echoMsg->nofCharacters());
	TRACE((LOG_LEVEL_INFO, "GAMaintSession::process(), recv msg <%s>", 0, str.c_str()));
	str += " returned from Server";
	mMsg->reset();
	new (mMsg->addr()) GAMaintMsg::EchoTestRspMsg(str.length(), str.c_str(), 0);
	return mSession->sendf(mMsg->addr(), mSize, MSG_NOSIGNAL);
}
