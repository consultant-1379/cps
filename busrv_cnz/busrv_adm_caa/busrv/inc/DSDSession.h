#ifndef _DSD_BUSRV_SESSION_H
#define _DSD_BUSRV_SESSION_H

#include "ACS_DSD_DSA2.h"
#include "DSDMsg.h"
//#include "Config.h"

class DSDSession
{
public:
	DSDSession() : mSession(0), mMsg(0) {}
	DSDSession(ACS_DSD_Session* ds, DSDMsg* msg) : mSession(ds), mMsg(msg) {}

	void processMsg();

private:

	bool fileExist(SBCId&, DSDMsg::ERROR_CODE&); 
	void setReloadFile(SBCId&, DSDMsg::ERROR_CODE&); 

	ACS_DSD_Session*	mSession;
	DSDMsg*				mMsg;

};


#endif
