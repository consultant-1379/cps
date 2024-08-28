/*
 * GAMaintSession.h
 *
 *  Created on: May 22, 2012
 *      Author: xdtthng
 */

#ifndef GA_MAINT_SESSION_H_
#define GA_MAINT_SESSION_H_

#include "GAMaintMsg.h"
#include <boost/shared_ptr.hpp>

class ACS_DSD_Session;
typedef boost::shared_ptr<ACS_DSD_Session>	SessionPtr;



class GAMaintSession
{
public:
	GAMaintSession(SessionPtr session, GAMsg* msg, size_t size);
	~GAMaintSession();
	ssize_t process();

private:
	GAMsg*		mMsg;
	size_t		mSize;
	SessionPtr	mSession;
};

#endif /* GA_MAINT_SESSION_H_ */
