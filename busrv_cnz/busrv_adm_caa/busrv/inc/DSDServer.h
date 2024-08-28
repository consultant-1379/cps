#ifndef _DSD_SERVER_SRM_H_
#define _DSD_SERVER_SRM_H_

#include "ACS_DSD_DSA2.h"
#include <windows.h>
#include <iostream>
#include <vector>

using namespace std;

class DSDServer
{
public:

	enum {
		ONE_SEC_DELAY_MS = 1000,
		ONE_HOUR_MS = 3600000,
		LOOP_DELAY_MS = 1000,
		MAX_NO_OF_HANDLES = 64
	};

	enum ERROR_CODE { 
		NO_DSD_ERROR, 
		PUBLISH_ERROR,
		SESSION_ERROR,
		ABORTING
	};


	DSDServer();
	~DSDServer();
	HANDLE run();

private:

	void publish();
	static unsigned __stdcall threadfunc(void* pv);

	ACS_DSD_Server* mDSDServer;
    vector< ACS_DSD_Session* > mDSDSessions;
	HANDLE mThread;
	HANDLE mAbortEvent;
	ERROR_CODE mError;
};

#endif
