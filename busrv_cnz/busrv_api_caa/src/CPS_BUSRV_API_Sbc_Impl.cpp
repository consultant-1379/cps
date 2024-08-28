/*
 * CPS_BUSRV_API_Sbc_Impl.cpp
 *
 *  Created on: May 27, 2012
 *      Author: xdtthng
 */

#include "CPS_BUSRV_API_Sbc_Impl.h"
#include "GAMsg.h"
#include "GASrmMsg.h"


const int CPS_BUSRV_API_Sbc_Impl::MSG_SIZE_IN_BYTES;

//----------------------------------------------------------------------------
CPS_BUSRV_API_Sbc_Impl::CPS_BUSRV_API_Sbc_Impl(): mConnected(false),
		mClient(new GAClient("BUSRVDSD", "SBC"))

{
	mConnected = mClient? mClient->connect() : false;
}

//----------------------------------------------------------------------------
CPS_BUSRV_API_Sbc_Impl::~CPS_BUSRV_API_Sbc_Impl()
{
	//cout << "CPS_SBC_ApiImpl() dtor" << endl;
}


//----------------------------------------------------------------------------
CPS_BUSRV_API_Sbc_Impl::RCType CPS_BUSRV_API_Sbc_Impl::setSbcReloadFile(int filenumber)
{
	//cout << "CPS_BUSRV_API_Sbc_Impl::setSbcReloadFile: " << filenumber << endl;

	if (!mConnected) {
		return CPS_BUSRV_API_Sbc::RC_NOCONTACT;
	}

	char *buffer = new char[MSG_SIZE_IN_BYTES];
	GAMsg *msg = reinterpret_cast<GAMsg*>(buffer);
	msg->reset();
	new(msg->addr()) GASrmMsg::SetReloadFileMsg(filenumber);
	ssize_t res = mClient->send(buffer, MSG_SIZE_IN_BYTES);
	if (res < 0) {
		return static_cast<CPS_BUSRV_API_Sbc::Result>(mClient->getResultCode());
	}
	GASrmMsg::SetReloadFileRspMsg *rspMsg = reinterpret_cast<GASrmMsg::SetReloadFileRspMsg*>(msg->addr());
	return static_cast<CPS_BUSRV_API_Sbc::Result>(rspMsg->errorCode());
}

//----------------------------------------------------------------------------
CPS_BUSRV_API_Sbc_Impl::RCType CPS_BUSRV_API_Sbc_Impl::sbcFileExists(int filenumber)
{
	//cout << "CPS_SBC_ApiImpl::getFileExists() " << filenumber << endl;
	if (!mConnected) {
		return CPS_BUSRV_API_Sbc::RC_NOCONTACT;
	}

	char *buffer = new char[MSG_SIZE_IN_BYTES];
	GAMsg *msg = reinterpret_cast<GAMsg*>(buffer);
	msg->reset();
	new(msg->addr()) GASrmMsg::SbcFileExistMsg(filenumber);
	ssize_t res = mClient->send(buffer, MSG_SIZE_IN_BYTES);
	if (res < 0) {
		return static_cast<CPS_BUSRV_API_Sbc::Result>(mClient->getResultCode());
	}
	GASrmMsg::SbcFileExistRspMsg *rspMsg = reinterpret_cast<GASrmMsg::SbcFileExistRspMsg*>(msg->addr());
	return static_cast<CPS_BUSRV_API_Sbc::Result>(rspMsg->errorCode());
}
