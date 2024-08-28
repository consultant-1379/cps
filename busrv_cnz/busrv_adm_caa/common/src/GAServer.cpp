#include "GAServer.h"
#include "GAMsg.h"
#include "GAMaintSession.h"
#include "GASrmSession.h"
#include "CPS_BUSRV_Trace.h"

#include "Config.h"

GAServer::GAServer(const char* name, const char* domain) : DSDServer(name, domain)
{
    //newTRACE((LOG_LEVEL_INFO, "GAServer::GAServer() ctor", 0));
}

GAServer::~GAServer()
{
    //newTRACE((LOG_LEVEL_INFO, "GAServer::~GAServer() dtor", 0));
}

ssize_t GAServer::incomingData(acs_dsd::HANDLE, SessionPtr session, char* buffer, ssize_t noOfBytes)
{
    newTRACE((LOG_LEVEL_INFO, "GAServer::incomingData()", 0));

    GAMsg *msg = reinterpret_cast<GAMsg*>(buffer);

    switch (msg->messageClass()) {
    case GAMsg::MAINTENANCE_MESSAGE_CLASS: {
    	GAMaintSession aSession(session, msg, noOfBytes);
    	return aSession.process();

    	break;
    }
    case GAMsg::SRM_MESSAGE_CLASS: {
    	GASrmSession aSession(session, msg, noOfBytes);
    	return aSession.process();
    	break;
    }
    default:
    	break;
    }

    //rsp += " Response from server";
    //ssize_t res = session->send(rsp.c_str(), BUFFER_SIZE);
    //TRACE(("Send result is <%d>",0, res));
    return 0;
}

void GAServer::start()
{
	if (Config::instance().isMultipleCP()) {
		DSDServer::start();
	}
}



