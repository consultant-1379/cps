#include <algorithm>
#include <utility>

#include "DSDServer.h"
#include "CPS_BUSRV_Trace.h"
#include "CriticalSection.h"

using namespace std;

const int DSDServer::MAX_NO_OF_SERVER_HANDLES;
const int DSDServer::MAX_NO_OF_SESSION_HANDLES;
const int DSDServer::BUFFER_SIZE;


bool DSDServer::running()
{
	AutoCS a(m_cs);
	return mThreadRunning;
}

void DSDServer::running(bool val)
{
	AutoCS a(m_cs);
	mThreadRunning = val;
}

DSDServer::DSDServer(const char* name, const char* domain): mError(NO_DSD_ERROR),
		mPublishing(false), mThreadRunning(false), mAbortFD(),
		mDSDsrvName(name), mDSDsrvDomain(domain),
		mDSDServer(new ACS_DSD_Server(acs_dsd::SERVICE_MODE_INET_SOCKET)),
		m_cs()

{
    //newTRACE((LOG_LEVEL_INFO, "DSDServer::DSDServer()", 0));
}

DSDServer::~DSDServer()
{
	try {
		//newTRACE((LOG_LEVEL_INFO, "DSDServer::~DSDServer()", 0));

		// Stop the running thread first
		// -----------------------------

		// Must wait for DSD thread return
		if (running()) {
			//TRACE((LOG_LEVEL_INFO, "DSDServer::~DSDServer() while thread is still running", 0));
			mDSDThread.join();
		}
		unregister();
	}
	catch (...) {

	}
}

void DSDServer::cleanup()
{
	newTRACE((LOG_LEVEL_INFO, "DSDServer::cleanup()", 0));
	running(false);
	mVH.clear();
	mSessions.clear();
}

void DSDServer::unregister()
{
	//newTRACE((LOG_LEVEL_INFO, "DSDServer::unresigter()", 0));
    if (mDSDServer.get()) {
        mDSDServer->unregister();
        mDSDServer->close();
    }
}

void DSDServer::publish(void)
{
    newTRACE((LOG_LEVEL_INFO, "DSDServer::publish()", 0));
	
	// Try indefinitely until successfull or get aborted
	// Setting up infinite trial loop
    
    fd_set masterFDs;       // Master file descriptor list or event list   
    fd_set readFDs;         // Temp file descriptor list   
    struct timeval tv;      // Sleeping time between each trial

    // Reset the FD list
    FD_ZERO(&masterFDs);
    FD_ZERO(&readFDs);   
    FD_SET(mAbortFD.getFd(), &masterFDs);
    int maxFD = mAbortFD.getFd();   // The range of FDs; wait for single object   
    mPublishing = true;
    int retval = 0;    
	bool running = true;

    while (running) {
	
        readFDs = masterFDs;        
        tv.tv_sec = 10;      // Sleeps one second before each trial
        tv.tv_usec = 0;        

        if (retval == 0) {  // Time out, attempt publishing        
        	if (mDSDServer->open(acs_dsd::SERVICE_MODE_INET_SOCKET) < 0) {
        	   mError = PUBLISH_ERROR;
        	   TRACE((LOG_LEVEL_ERROR, "DSDServer::publish() fails to open DSD server", 0));
        	   TRACE((LOG_LEVEL_ERROR, "%s", 0, mDSDServer->last_error_text()));
        	}
        	//else if (mDSDServer->publish("BUSRVDSD","SBC") < 0) {
            else if (mDSDServer->publish(mDSDsrvName, mDSDsrvDomain) < 0) {
        	   mError = PUBLISH_ERROR;
        	   mDSDServer->close();
        	   TRACE((LOG_LEVEL_ERROR, "DSDServer::publish() fails to publish DSD server", 0));
        	   TRACE((LOG_LEVEL_ERROR, "%s", 0, mDSDServer->last_error_text()));
        	}
        	else {
        	   // Successfully publish DSD sever; stop this infinite loop
        	   mAbortFD.resetEvent();
        	   running = false;
        	   mError = NO_DSD_ERROR;
        	   TRACE((LOG_LEVEL_INFO, "DSDServer::publish() OK; Ready for use" , 0));
        	   mPublishing = false;
        	   break;
        	}
    	}
    	else if (FD_ISSET(mAbortFD.getFd(), &readFDs)) {
            mAbortFD.resetEvent();
            running = false;
            mError = ABORTING;
            TRACE((LOG_LEVEL_INFO, "DSDServer::publish() is aborted", 0));
            mPublishing = false;
            break;
    	}
        retval = select(maxFD + 1, &readFDs, NULL, NULL, &tv);
    }
}

void DSDServer::run()
{
    newTRACE((LOG_LEVEL_INFO, "DSDServer::run()", 0));

#if 0
    mDSDServer.reset(new (nothrow) ACS_DSD_Server(acs_dsd::SERVICE_MODE_INET_SOCKET));
	if (mDSDServer.get()) {
		TRACE((LOG_LEVEL_INFO, "DSDServer::publish() gets ACS_DSD_Server; attempt to publish", 0));
	}
	else {
		TRACE((LOG_LEVEL_ERROR, "DSDServer::publish() fails to get a new ACS_DSD_Server", 0));
		mError = PUBLISH_ERROR;
		mPublishing = false;
		return;				
	}
#endif

	running(true);
    publish();
    if (mError != NO_DSD_ERROR) {
        TRACE((LOG_LEVEL_WARN, "Error publishing DSD server", 0));
        TRACE((LOG_LEVEL_INFO, "DSDServer::run() returns", 0));
        cleanup();
        return;
    }

    // ------------------------------------------------------------------------   
    // Server infinite loop
    //-------------------------------------------------------------------------
    //fd_set masterFDs;       // Master file descriptor list or event list
    fd_set readFDs;         // Temp file descriptor list   
    struct timeval tv;      // Sleeping time between each trial
    //vector<acs_dsd::HANDLE> vH;

    // Prepare the FD list
    FD_ZERO(&mMasterFDs);
    FD_ZERO(&readFDs);   
    mVH.push_back(mAbortFD.getFd());
    FD_SET(mAbortFD.getFd(), &mMasterFDs);
    int maxFD = mAbortFD.getFd();
    TRACE((LOG_LEVEL_INFO, "Before loop, maxFD is <%d>", 0, maxFD));
	
    // Set FD list with DSD listening handles
	acs_dsd::HANDLE listenH[MAX_NO_OF_SERVER_HANDLES];
	acs_dsd::HANDLE sessionH[MAX_NO_OF_SESSION_HANDLES];
	int noListenH = MAX_NO_OF_SERVER_HANDLES;
	int noSessionH = MAX_NO_OF_SESSION_HANDLES;
	if (mDSDServer->get_handles(listenH, noListenH) >= 0) {
	   TRACE((LOG_LEVEL_INFO, "DSDServer::run(); get <%d> dsd listening handles", 0, noListenH));
	   for (int i = 0; i < noListenH; ++i) {
	       FD_SET(listenH[i], &mMasterFDs);
	       mVH.push_back(listenH[i]);
	       TRACE((LOG_LEVEL_INFO, "%d %d", 0, i, listenH[i]));
	       maxFD = max(maxFD, listenH[i]);
	       //TRACE(("%d %d %d", 0, i, listenH[i], maxFD));
	   }
	   //sort(vH.begin(), vH.end());
	   //maxFD = vH.back();
	   //maxFD = *max_element(vH.begin(), vH.end());
	   TRACE((LOG_LEVEL_INFO, "DSDServer::run(); maxFD <%d>", 0, maxFD));
	}
	else {
        TRACE((LOG_LEVEL_ERROR, "DSDServer::run() returns; Error when getting listening handles", 0));
        TRACE((LOG_LEVEL_ERROR, "%s", 0, mDSDServer->last_error_text() ));
        cleanup();
        return;
	}

    int retval = 0;    
    bool running = true;
    acs_dsd::HANDLE aHandle, bHandle;
    while (running) {

        readFDs = mMasterFDs;
        tv.tv_sec = 1;      // Sleeps one second before each trial; 10 for testing
        tv.tv_usec = 0;
        retval = select(maxFD + 1, &readFDs, NULL, NULL, &tv);
 
        // Timeout; at the moment, take no action       
        if (retval == 0) {
            continue;
        }
        

        // Not recoverable error; exit the loop        
        if (retval == -1 ) {
            // A signal was caught; at the moment, take no action
            if (errno == EINTR) {
                continue;
            }
            
            TRACE((LOG_LEVEL_WARN, "DSDServer::run(); select() returns negavtive result; exiting ..", 0));
            running = false;
            break;
        }

        // Abort event signaled
        TRACE((LOG_LEVEL_INFO, "Checking for abort event", 0));
    	if (FD_ISSET(mAbortFD.getFd(), &readFDs)) {
            mAbortFD.resetEvent();
            running = false;
            TRACE((LOG_LEVEL_INFO, "DSDServer::run() is aborted", 0));
            break;
    	}

    	// New connection signaled from remote end
    	// Accept new connection and setup new session
    	bool newConnection = false;
        for (int i = 0; i < noListenH; ++i) {
	       aHandle = listenH[i];
	       if (FD_ISSET(aHandle, &readFDs)) {
	    	   newConnection = true;
	           TRACE((LOG_LEVEL_INFO, "listenH[%d] == <%d> signaled", 0, i, aHandle));
	           SessionPtr aSession = SessionPtr(new ACS_DSD_Session);
	           if(mDSDServer->accept(*aSession) < 0) {
	               TRACE((LOG_LEVEL_ERROR, "Failed to accept signaled handle", 0));
	               continue;
	           }
	           noSessionH = MAX_NO_OF_SESSION_HANDLES;
	           if (aSession->get_handles(sessionH, noSessionH) < 0) {
	               TRACE((LOG_LEVEL_ERROR, "Failed to get session handles", 0));
	               continue;
	           }
	           TRACE((LOG_LEVEL_INFO, "No of Sessions per connection <%d>", 0, noSessionH));
	           for (int j = 0; j < noSessionH; ++j) {  // Store all session handles
	               bHandle = sessionH[j];
    	           FD_SET(bHandle, &mMasterFDs);
    	           maxFD = max(maxFD, bHandle);
    	           TRACE((LOG_LEVEL_INFO, "Session Handle <%d>", 0, bHandle));
    	           mSessions.insert(pair<acs_dsd::HANDLE, SessionPtr>(bHandle, aSession));
    	           mVH.push_back(bHandle);
	           }
	           TRACE((LOG_LEVEL_INFO, "maxFD is <%d>", 0, maxFD));
	       }
	   }

       if (newConnection)
    	   continue;

	   // New message from remote end signaled.
	   // Receive the process new message
	   SessionMapIter it = mSessions.begin();
	   SessionMapIter end = mSessions.end();
	   TRACE((LOG_LEVEL_INFO, "The size of mSessions is <%d>", 0, mSessions.size()));
	   while (it != end) {
		   aHandle = it->first;
	       if (FD_ISSET(aHandle, &readFDs)) {
	           char buf[BUFFER_SIZE];
	           ssize_t nob = it->second->recv(buf, BUFFER_SIZE);
	           if (nob <= 0) {
	        	   mSessions.erase(it++);
	        	   closeHandle(aHandle, maxFD);
	               continue;
	           }
	           else {
	               TRACE((LOG_LEVEL_INFO, "Received message len <%d> on handle <%d>", 0, nob, aHandle));
	               ssize_t res = this->incomingData(aHandle, it->second, buf, BUFFER_SIZE);
	               if (res <= 0) {
 		        	   mSessions.erase(it++);
		        	   closeHandle(aHandle, maxFD);
    	               continue;
	               }
	           }
	       }
	       ++it;
	   }
	   // ToDo: remember to remove this for final production
	   stringstream ss;
	   copy(mVH.begin(), mVH.end(), ostream_iterator<acs_dsd::HANDLE>(ss, " "));
	   TRACE((LOG_LEVEL_INFO, "Handles: <%s>", 0, ss.str().c_str()));
	   TRACE((LOG_LEVEL_INFO, "maxFD is <%d>", 0, maxFD));
	   TRACE((LOG_LEVEL_INFO, "Capacity of mVH is <%d>", 0, mVH.capacity()));
	   //TRACE(("value of FD_SETSIZE is <%d>", 0, FD_SETSIZE));
    }

    cleanup();
    TRACE((LOG_LEVEL_INFO, "DSDServer::run() returns; DSD server thread exiting", 0));
}

void DSDServer::closeHandle(acs_dsd::HANDLE aHandle, int& maxFD)
{
	newTRACE((LOG_LEVEL_INFO, "DSDServer::closeHandle()", 0));

    TRACE((LOG_LEVEL_INFO, "**** Remote connection closed; erase session", 0));
    FD_CLR(aHandle, &mMasterFDs);
    TRACE((LOG_LEVEL_INFO, "The size of mSessions is <%d>", 0, mSessions.size()));
    mVH.erase(remove(mVH.begin(), mVH.end(), aHandle),
    		mVH.end());

    if (aHandle == maxFD) {
    	maxFD = *max_element(mVH.begin(), mVH.end());
        TRACE((LOG_LEVEL_INFO, "Handle <%d> removed from master list", 0, aHandle));
        TRACE((LOG_LEVEL_INFO, "New maxFD is <%d>", 0, maxFD));
    }
    else {
    	TRACE((LOG_LEVEL_INFO, "maxFD <%d> remains unchanged", 0, maxFD));
    }
	stringstream ss;
	copy(mVH.begin(), mVH.end(), ostream_iterator<acs_dsd::HANDLE>(ss, " "));
	TRACE((LOG_LEVEL_INFO, "Handles: <%s>", 0, ss.str().c_str()));

}

void DSDServer::stop()
{
    newTRACE((LOG_LEVEL_INFO, "DSDServer::stop()", 0));

    if (running()) {
    	if (mAbortFD.setEvent()) {
    
    	    // Must wait for DSD thread return
            mDSDThread.join();
            unregister();
            //cleanup();
            running(false);
    	}
    }
    else
    {
        TRACE((LOG_LEVEL_INFO, "DSDServer was already stopped.", 0));
    }

    TRACE((LOG_LEVEL_INFO, "DSDServer::stop(); DSD Server thread returns", 0));
}

void DSDServer::start()
{
    newTRACE((LOG_LEVEL_INFO, "DSDServer::start()", 0));
    
    mDSDThread = boost::thread(boost::bind(&DSDServer::run, this));
}


