/*
 * GASrmSession.h
 *
 *  Created on: May 24, 2012
 *      Author: xdtthng
 *
 *  Update: 
 *  2012-12-24  xdthng  Change api method names
*/

#ifndef GA_SRM_SESSION_H_
#define GA_SRM_SESSION_H_

#include "GASrmMsg.h"
#include <boost/shared_ptr.hpp>

class ACS_DSD_Session;
typedef boost::shared_ptr<ACS_DSD_Session>	SessionPtr;



class GASrmSession
{
public:
	GASrmSession(SessionPtr session, GAMsg* msg, size_t size);
	~GASrmSession();
	ssize_t process();

private:
	GAMsg*		mMsg;
	size_t		mSize;
	SessionPtr	mSession;

	GASrmMsg::ERROR_CODE fileExist(u_int16);
	GASrmMsg::ERROR_CODE setReloadFile(u_int16);
};

#endif /* GA_SRM_SESSION_H_ */
