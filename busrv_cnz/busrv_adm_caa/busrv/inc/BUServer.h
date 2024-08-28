/*
NAME
    File_name:BUServer.h

Ericsson Utvecklings AB

    COPYRIGHT Ericsson Utvecklings AB, Sweden 2000. All rights reserved.

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
    2010-08-25 by XDT/DEK xquydao
                  XDT/DEK xdtthng


SEE ALSO
    -

Revision history
----------------
2010-08-25 xquydao Created
2010-11-14 xdtthng Updated for initial load UC
2011-01-10 xquydao Updated after review

*/

#ifndef ECHO_TCP_SERVER_H
#define ECHO_TCP_SERVER_H

#include "TCPServer.h"
#include "BUSession.h"
#include "CriticalSection.h"

#include "boost/thread/mutex.hpp"
#include "boost/thread/condition_variable.hpp"

class JobThread;

class BUServer : public TCPServer 
{
public:
    BUServer(void);
    virtual ~BUServer(void);
    virtual void incoming_data_handler(TCPConnection_ptr connection_ptr, const char* data, std::size_t num);
    
    void set(JobThread *p) { m_jobThread = p; }
    
    virtual void stop(void);
    void jobDone(void);
    void handle_jobDone(void);
    
    virtual void run(void);

    // Wait until Server is started
    // This function must be called by thread different than the Thread of BUServer
    void waitUntilRunning(void);

private:
    JobThread*      m_jobThread;
    bool            m_cpfEstablised;
    bool            m_stopping;
    CriticalSection m_cs;
            
    VSPBUSession    m_buSessions;

    // For condition variable
    boost::mutex                m_mutex;
    boost::condition_variable   m_condition;

    void handle_service_dependency();
    void stop_handle_service_dependency(bool&);

};


#endif

