/*

Generic Ericsson comments go here later

*/

#include "stdafx.h"
#include "buap.h"
#include "Config.h"
#include "DataFile.h"
#include "CPS_BUSRV_Trace.h"  
#include "dsdserver.h"
#include "DSDMsg.h"
#include "DSDSession.h"

#include <process.h>


DSDServer::DSDServer() : mDSDServer(0), mDSDSessions(), mThread(INVALID_HANDLE_VALUE),
						mAbortEvent(INVALID_HANDLE_VALUE), mError(NO_DSD_ERROR)
{
    //newTRACE((LOG_LEVEL_INFO, "DSDServer Constructor", 0));
	
	mAbortEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
/*	
	if (mAbortEvent == INVALID_HANDLE_VALUE) {
		TRACE((LOG_LEVEL_ERROR, "BUSRV DSDServer() cannot create abort event handle", 0));
	}
*/
}

DSDServer::~DSDServer()
{
    //newTRACE((LOG_LEVEL_INFO, "DSDServer::~DSDServer()", 0));

	if (mThread != INVALID_HANDLE_VALUE &&
		WAIT_TIMEOUT == WaitForSingleObject(mThread, 0) 
		&& mAbortEvent != INVALID_HANDLE_VALUE) {

		if (SetEvent(mAbortEvent)) {				// Send this event to the DSD thread
			WaitForSingleObject(mThread, INFINITE);	// Wait for DSD thread to close down
			CloseHandle(mThread);
			CloseHandle(mAbortEvent);
			mThread = INVALID_HANDLE_VALUE;
			mAbortEvent = INVALID_HANDLE_VALUE;
			//TRACE((LOG_LEVEL_INFO, "~DSDServer() closes DSD Server thread", 0));
		}
/*		
		else {
			TRACE((LOG_LEVEL_ERROR, "DSDServer::~DSDServer() cannot send abort event", 0));
		}
	}
	else {
		TRACE((LOG_LEVEL_INFO, "~DSDServer() does not close DSD Server thread. DSD thread already closed", 0));
	}
*/
	}	

	// Unregister CPS DSD Service
	if (mDSDServer) {
		mDSDServer->unregister();
		delete mDSDServer;
		mDSDServer = 0;
		//TRACE((LOG_LEVEL_INFO, "~DSDServer() unregisters DSD Service", 0));
	}
/*	
	else {
		TRACE((LOG_LEVEL_INFO, "~DSDServer() Service not registered yet", 0));
	}
*/	
}

void DSDServer::publish()
{
	newTRACE((LOG_LEVEL_INFO, "DSDServer::publish()", 0));

	bool go = true;
    unsigned int loopCount = 0;
	DWORD wRes;
	mError = PUBLISH_ERROR;

	try {
		// IO has been asked multiple times if ACS_DSD_Server(DSD_API::SOCKET) throws
		// an exception. There has been no answer. Thus it is assumed that it does
		if (!mDSDServer) {
			TRACE((LOG_LEVEL_INFO, "DSDServer::publish() attempts to get a new ACS_DSD_Server", 0));
			mDSDServer = new ACS_DSD_Server(DSD_API::SOCKET);
			if (mDSDServer) {
				TRACE((LOG_LEVEL_INFO, "DSDServer::publish() gets ACS_DSD_Server", 0));
			}
			else {
				TRACE((LOG_LEVEL_WARN, "DSDServer::publish() fails to get a new ACS_DSD_Server", 0));
			}
		}

		// Try indefinitely to publish BUSRV to DSD until one of the following event occurs
		// - sucessful publishment
		// - closed by Windows
		// - fatal error
		wRes = WAIT_TIMEOUT;	// Attempt to publish first before waiting
		while (go) {			
			if (wRes == WAIT_TIMEOUT) {				
				///*
				if (!mDSDServer->open()) {
					// If the DSD Server can't be opened, try again once every LOOP_DELAY_MS seconds.
					// Log an event that it was unable to be opened once an hour.
					if(loopCount % (ONE_HOUR_MS / LOOP_DELAY_MS) == 0) {
						// Log error, but not every single trial
						DateTime dt;
						dt.now();
						ostringstream os;
						os << "mDSDServer->open() fails at " << dt;
						TRACE((LOG_LEVEL_ERROR, "%s", 0, os.str().c_str()));
						EventReporter::instance().write(os.str().c_str());
					}
					loopCount++;
					//Sleep(LOOP_DELAY_MS);
				}
				else if (!mDSDServer->publish("BUSRVDSD","SBC")) {
					// If the DSD Server can't be published, try again once every LOOP_DELAY_MS seconds.
					// Log an event that it was unable to be published once an hour.
					if(loopCount % (ONE_HOUR_MS / LOOP_DELAY_MS) == 0) {
						// Log error, but not every single trial
						DateTime dt;
						dt.now();
						ostringstream os;
						os << "mDSDServer->publish(BUSRVDSD,SBC) fails at " << dt;
						TRACE((LOG_LEVEL_ERROR, "%s", 0, os.str().c_str()));
						EventReporter::instance().write(os.str().c_str());
					}
					mDSDServer->close();
					loopCount++;
					//Sleep(LOOP_DELAY_MS);
				}
				else {
					go = false;
					mError = NO_DSD_ERROR;
					TRACE((LOG_LEVEL_INFO, "DSDServer::publish() OK; Ready for use", 0));
					break;
				}
				//*/
			}
			else if (wRes ==  WAIT_OBJECT_0) {
				go = false;
				mError = ABORTING;
				break;
			}
			wRes = WaitForSingleObject(mAbortEvent, LOOP_DELAY_MS);		
		} // end while
	}
	catch (...) {
		TRACE((LOG_LEVEL_ERROR, "DSDServer::publish() fails, get thrown", 0));
		mError = ABORTING;
	}
}


HANDLE DSDServer::run() {

    newTRACE((LOG_LEVEL_INFO, "DSDServer::run()", 0));

	// This is still in TPC Server thread, thus be as brief as possible
    unsigned int threadId = 0;
    mThread = (HANDLE) _beginthreadex(NULL, 0, threadfunc, this, 0, &threadId);
	return mThread;
}

unsigned __stdcall DSDServer::threadfunc(void* pv) {

    newTRACE((LOG_LEVEL_INFO, "DSDServer::threadfunc(void* pv)", 0));
	DSDServer& dsd = *reinterpret_cast<DSDServer*>(pv);

	try {
		
		// Must call open before calling getHandles
		dsd.publish();
		if (dsd.mError != NO_DSD_ERROR)	// Window closes while publishing
			throw dsd.mError;

		HANDLE h[MAX_NO_OF_HANDLES];
		HANDLE* listenH = 0;
		int noH = 0;
		int nhp = 0;					// Next handle position
		h[nhp++] = dsd.mAbortEvent;		// This is the abort thread set by the dtor

		// get the listening handles.
		if (dsd.mDSDServer->getHandles(noH, listenH)) {
			for (int i=0; i<noH; i++) 
				h[nhp++]=listenH[i];
			int nwH = nhp;		// This is the position where BUSRV API messages starts

			DWORD wRes;
			bool go = true;
			while (go) {
				// create a waitfor array.
				nwH = nhp; // listening handles.
		    
				for (vector<ACS_DSD_Session*>::iterator it = dsd.mDSDSessions.begin();
						it != dsd.mDSDSessions.end();it++) {
					// add connection handles to the array.
					h[nwH++]=(*it)->getHandle();
				}
				char msg[DSDMsg::MSG_SIZE_IN_BYTES];
				// The following line is real code
				wRes=WaitForMultipleObjects(nwH, h, false, 1000);

				// Debugging code. Remember to remove
				//wRes=WaitForMultipleObjects(nwH, h, false, 10000);
				//TRACE(("Inside DSD While loop", 0));
		    
				if (wRes == WAIT_TIMEOUT) {
					// There may be nothing waiting
					// Already waiting for 1000 ms for this
					//TRACE(("Time out durig waitfor inside DSD while loop", 0));
				}
				else if (wRes == WAIT_OBJECT_0) {
					//-------------------------
					// Shutdown by window
					//-------------------------
					go = false;		// Exit the loop
					TRACE((LOG_LEVEL_INFO, "DSDServer::threadfunc exiting ... upon receiving abort event", 0));
				}
				else if (wRes<nhp) {
					//-------------------------
					// listen handle signalled
					//-------------------------
					ACS_DSD_Session* temp = new ACS_DSD_Session();

					if (dsd.mDSDServer->accept(*temp)) {
						dsd.mDSDSessions.push_back(temp);
					}
				}
				else if (wRes<nwH)          
				{
					//------------------------------
					// connection handle signalled.
					//------------------------------
					int ind = wRes - nhp;
					unsigned int nob = DSDMsg::MSG_SIZE_IN_BYTES;
					if (dsd.mDSDSessions[ind]->recvMsg(msg, nob)) {
						// According to the DSD API, a DSD Session can only be used once
						// It will be deleted after use
						//vector<ACS_DSD_Session*>::iterator it=dsd.mDSDSessions.begin();
						//it+=ind;
						//ACS_DSD_Session* dsdSession = *it;
						//dsd.mDSDSessions.erase(it);  
						if (nob>0) {
							// Process the message here

							DSDMsg *cmd = reinterpret_cast<DSDMsg*>(msg);
							DSDSession s(dsd.mDSDSessions[ind], cmd);
							//DSDSession s(dsdSession, cmd);
							s.processMsg();
						}
						else
						{
							//----------------------------
							// remote side disconnected.
							//----------------------------

							delete dsd.mDSDSessions[ind];
							vector<ACS_DSD_Session*>::iterator it=dsd.mDSDSessions.begin();
							it+=ind;
							dsd.mDSDSessions.erase(it);  

						}
						//dsdSession->close();
						//delete dsdSession;
					}
					else {
						// receive failed.
					}
				}
				else {
					TRACE((LOG_LEVEL_INFO, "Default catch after waitfor", 0));
				}
			} //end while

			// Cleaning up sessions
			for (vector<ACS_DSD_Session*>::iterator it = dsd.mDSDSessions.begin();
				it != dsd.mDSDSessions.end(); it++) {
				delete *it;;
			}
		}

	}
	catch (ERROR_CODE e) {
		TRACE((LOG_LEVEL_ERROR, "DSDServer::threadfunc() fails to publish %d", 0, e));
		// Publishing fails, there is nothing to clean up
	}
	catch (...) {
		TRACE((LOG_LEVEL_ERROR, "DSDServer::threadfunc() throws unknown exception", 0));
		// Cleaning up sessions
		for (vector<ACS_DSD_Session*>::iterator it = dsd.mDSDSessions.begin();
			it != dsd.mDSDSessions.end(); it++) {
			delete *it;;
		}
	}

	return 0;
}
