#ifndef _GA_DSD_CLIENT_H_
#define _GA_DSD_CLIENT_H_

#include "ACS_DSD_Client.h"
#include "ACS_DSD_Session.h"
#include <string>

class GAClient
{
public:
	enum ResultCode {
		RC_OK,						// Command was executed
		RC_NOCONTACT,				// No contact with server
		RC_INTERNAL_ERROR,			// Error due to DSD, Linux ... 
		RC_FUNCTION_NOT_SUPPORTED	// Interface is used when APZ Profile == 0
    };
    
    GAClient(const char* name, const char* domain);
    ~GAClient();
    
    bool connect();
    ssize_t send(void* buf, size_t size);
    int getResultCode();
    
private:
    ResultCode      mResultCode;
    ACS_DSD_Client  mClient;
    ACS_DSD_Session mSession;
    std::string		mName;
    std::string		mDomain;
};

inline
int GAClient::getResultCode()
{
    return mResultCode; 
}

#endif
