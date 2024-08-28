#include "GAClient.h"
#include "CPS_BUSRV_Trace.h"

//#include <iostream>
//using namespace std;

static const int MSG_SIZE_IN_BYTES = 1024;
static const int MAX_NO_OF_HANDLES = 64;
static const unsigned MAX_OF_TIMEOUT_MS = 3000;

GAClient::GAClient(const char* name, const char* domain): mResultCode(RC_OK),
		mClient(), mSession(), mName(name), mDomain(domain)
{
    //newTRACE(("GAClient::GAClient() ctor", 0));
}

GAClient::~GAClient()
{
    //newTRACE(("GAClient::~GAClient() dtor", 0));
}

bool GAClient::connect()
{
    
    std::vector<ACS_DSD_Node>  mReachableNodes;
    std::vector<ACS_DSD_Node>  mUnreachableNodes;
	if(mClient.query(mName, mDomain, acs_dsd::SYSTEM_TYPE_AP, mReachableNodes, mUnreachableNodes) < 0) {
        mResultCode = RC_NOCONTACT;
        return false;    		
	}
    
	if (mClient.connect(mSession, mName, mDomain) < 0) {
        mResultCode = RC_NOCONTACT;
        return false;    		
	}
    return true;
}

ssize_t GAClient::send(void* buf, size_t size)
{
    ssize_t res = mSession.sendf(buf, size, MSG_NOSIGNAL);
    if (res < 0) {
        mResultCode = RC_INTERNAL_ERROR;
        return res;
    }
    
    // Reuse the buffer for reponse message
    memset(buf, 0, size);
    
    // Prepare to receive the return message
    if ((res = mSession.recv(buf, size, MAX_OF_TIMEOUT_MS)) < 0) {
        mResultCode = RC_INTERNAL_ERROR;
        return res;
    }
    mResultCode = RC_OK;
    return res;
}

