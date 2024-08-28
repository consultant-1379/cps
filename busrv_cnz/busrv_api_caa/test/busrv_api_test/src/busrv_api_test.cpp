/*
NAME DEK/xdthng
*/

#include <iostream>
#include <string>
#include <sys/select.h>

#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <iterator>

#include "CPS_BUSRV_API_Sbc.h"
#include "CPS_BUSRV_Event_FD.h"

using namespace std;

static const int MAX_NO_OF_HANDLES=64;
static const int MSG_SIZE_IN_BYTES = 1024;

typedef CPS_BUSRV_API_Sbc::Result RCType;

typedef boost::shared_ptr<CPS_BUSRV_API_Sbc>	ApiPtr;
typedef vector<ApiPtr>							VecApiPtr;


static int run_as_console();

/*
enum Result {
	RC_SUCCESS,					// Request was executed
	RC_NOCONTACT,				// No contact with server
	RC_FILE_NOT_FOUND,			// Requested file not found
	RC_INTERNAL_ERROR,			// Error due to DSD, FMS, Linux, ...
	RC_FILE_OUT_OF_RANGE,		// File out of Cluster Reload Range
	RC_FUNCTION_NOT_SUPPORTED	// Interface is used when APZ Profile == 0
};
*/

const char* errorMessages[] = {
		"Success",
		"Server not contactable",
		"File does not exist",
		"Internal error",
		"File out of range",
		"Function not supported"
};

class TestServer
{
public:
	TestServer(int i = 0): mCase(i) {}
	~TestServer() {}

	void start()
	{
		mThread = boost::thread(boost::bind(&TestServer::run, this));
	}
	void stop()
	{
		mAbortFD.setEvent();
		mThread.join();
	}
	void join()
	{
		mThread.join();
	}
	void setCase(int n) { mCase = n; }

private:
	TestServer(const TestServer&);
	TestServer& operator=(const TestServer&);
	void run();
	void test();
	void test(int);

	int					mCase;
	CPS_BUSRV_Event_FD	mAbortFD;
	boost::thread		mThread;

};

void TestServer::run()
{
	cout << "TestServer::run() running" << endl;
    fd_set masterFDs;       // Master file descriptor list or event list
    fd_set readFDs;         // Temp file descriptor list
    FD_ZERO(&masterFDs);
    FD_ZERO(&readFDs);
    FD_SET(mAbortFD.getFd(), &masterFDs);
    int maxFD = mAbortFD.getFd();
    bool running = true;
    int retval = 0;
    struct timeval tv;
	struct timespec req;
	req.tv_sec = 5UL;
	req.tv_nsec = 0L;
	while (running) {
        readFDs = masterFDs;
        tv.tv_sec = 0;
        tv.tv_usec = 100000;
        retval = select(maxFD + 1, &readFDs, NULL, NULL, &tv);

        // Abort event signaled
    	if (FD_ISSET(mAbortFD.getFd(), &readFDs)) {
    		cout << "Test aborted" << endl;
            mAbortFD.resetEvent();
            running = false;
            break;
    	}

        // Timeout; at the moment, take no action
        if (retval == 0) {
        	test();
        	nanosleep(&req, NULL);
            continue;
        }


        // Not recoverable error; exit the loop
        if (retval == -1 ) {
            // A signal was caught; at the moment, take no action
            if (errno == EINTR) {
                continue;
            }

            running = false;
            break;
        }
    }
}

void TestServer::test(int)
{

	struct timespec req;
	int tt = 0;
	int fn = 0;
	int chance = 0;
	VecApiPtr api;
	api.reserve(4);
	RCType res;
	req.tv_sec = 2UL;
	for (int i = 0; i < 3; ++i) {
		tt = rand() % 250;
		req.tv_nsec = tt * 1000000L;
		fn = rand() % 130 + 1;
		api.push_back(ApiPtr(new CPS_BUSRV_API_Sbc));

		chance = rand() % 100;
		if (chance > 20){
			res = api[i]->sbcFileExists(fn);
			cout << "Checking file fn <" << fn << "> return code is <" << res
				 << "> <" << errorMessages[res] << ">" << endl;
		}
		else {
			res = api[i]->setSbcReloadFile(fn);
			cout << "Setting reload file fn <" << fn << "> return code is <" << res
				 << "> <" << errorMessages[res] << ">" << endl;
		}
		nanosleep(&req, NULL);
	}
	api.clear();

}

void TestServer::test()
{
	if (mCase) {
		test(mCase);
		return;
	}
	ApiPtr api = ApiPtr(new CPS_BUSRV_API_Sbc);

	RCType res;
	int fn = rand() % 130 + 1;
	int chance = rand() % 100;
	if (chance > 20){
		res = api->sbcFileExists(fn);
		cout << "Checking file fn <" << fn << "> return code is <" << res
			 << "> <" << errorMessages[res] << ">" << endl;
	}
	else {
		res = api->setSbcReloadFile(fn);
		cout << "Setting reload file fn <" << fn << "> return code is <" << res
			 << "> <" << errorMessages[res] << ">" << endl;
	}
}


int main(int, char**)
{

    try {
    	srand ( time(NULL) );
    	return run_as_console();
    }
	catch (...) {
		cout << "Catch exception" << endl;
		return 222;
	}

  	cout << "End of Test" << endl;
	return 0;
}

const int MAX_THREADS = 5;
int run_as_console()
{

    try {
    	TestServer aTest[MAX_THREADS];

    	int tt = 0;
    	for (int i = 0; i < MAX_THREADS; ++i) {
    		if (i > 2)
    			aTest[i].setCase(1);

    		tt = rand() % 1000;
    		aTest[i].start();
    		struct timespec req;
    		req.tv_sec = 0UL;
    		req.tv_nsec = tt * 1000000L;
    		nanosleep(&req, NULL);
    	}

        cout << "Hit Enter Key to stop" << endl;
        cin.get();
        for (int i = 0; i < MAX_THREADS; ++i) {
        	aTest[i].stop();
        }
    }
    catch (...) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

