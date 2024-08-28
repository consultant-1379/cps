/*
NAME
    File_name:FCServer.h

Ericsson Utvecklings AB

    COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

    The Copyright to the computer program(s) herein is the property of
    Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
    copied only with the written permission from Ericsson Utvecklings AB
    or in accordance with the terms and conditions stipulated in the
    agreement/contract under which the program(s) have been supplied.

DESCRIPTION
    Class implements a debug context class used in the development of the
    reset of the code. Note that no methods are to be called directly,
    rather by use of macros so that the debug code may be conditionally
    compiled out.

DOCUMENT NO
    190 89-CAA 109 1409

AUTHOR
    2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on FCServer.h of FCSRV in Windows


SEE ALSO
    -

Revision history
----------------

*/

#ifndef ECHO_TCP_SERVER_H
#define ECHO_TCP_SERVER_H

#include "FCSession.h"
#include "TCPServer.h"

#include "boost/thread/mutex.hpp"
#include "boost/thread/condition_variable.hpp"

class JobThread;

class FCServer : public TCPServer
{
public:
    FCServer(void);
    virtual ~FCServer(void);
    virtual void incoming_data_handler(TCPConnection_ptr connection_ptr, const char* data, std::size_t num);
    
    void set(JobThread *p) { m_jobThread = p; }
    
    virtual void stop(void);
    void jobDone(void);
    void handle_jobDone(void);
    
    virtual void run(void);
    bool isRunning(bool = false);

    // Wait until Server is started
    // This function must be called by thread different than the Thread of FCServer
    void waitUntilRunning(void);

private:
    JobThread*      m_jobThread;
    VSPFCSession    m_fcSessions;

    // For condition variable
    boost::mutex    m_mutex;
    boost::condition_variable m_condition;

};


#endif

