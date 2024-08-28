#ifndef _GA_DSD_SERVER_H_
#define _GA_DSD_SERVER_H_

#include "DSDServer.h"

class GAServer: public DSDServer 
{
public:
    GAServer(const char* name, const char* domain);
    virtual ~GAServer();
    void virtual start();

    virtual ssize_t incomingData(acs_dsd::HANDLE handle, SessionPtr session, char* buffer, ssize_t noOfBytes);

private:

	GAServer(const GAServer&);             // Non copyable
	GAServer& operator=(const GAServer&);  // Non copyable

};

#endif
