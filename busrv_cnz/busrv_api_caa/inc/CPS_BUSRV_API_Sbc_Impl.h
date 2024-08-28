/*
 * CPS_BUSRV_API_Cbc_Impl.h
 *
 *  Created on: May 27, 2012
 *      Author: xdtthng
 */

#ifndef CPS_BUSRV_API_SBC_IMPL_H_
#define CPS_BUSRV_API_SBC_IMPL_H_

#include "CPS_BUSRV_API_Sbc.h"
#include "GAClient.h"
#include "boost/shared_ptr.hpp"

class CPS_BUSRV_API_Sbc_Impl
{
public:
	typedef CPS_BUSRV_API_Sbc::Result RCType;

	CPS_BUSRV_API_Sbc_Impl();
	virtual ~CPS_BUSRV_API_Sbc_Impl();

	RCType setSbcReloadFile(int  filenumber);
	RCType sbcFileExists(int filenumber);

private:

	bool						mConnected;
	boost::shared_ptr<GAClient>	mClient;
	static const int MSG_SIZE_IN_BYTES = 1024;
};


#endif /* CPS_BUSRV_API_SBC_IMPL_H_ */
