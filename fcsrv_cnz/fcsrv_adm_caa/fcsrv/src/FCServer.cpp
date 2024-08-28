/*
NAME
    File_name:FCServer.cpp

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
    2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on FCServer.cpp of FCSRV in Windows


SEE ALSO
    -

Revision history
----------------


*/

#include <algorithm>
#include <vector>
#include <iterator>
#include <functional>
#include <iostream>
#include <iomanip>
#include <sstream>

#include "boost/bind.hpp"
#include "boost/thread/thread.hpp"


#include "FCServer.h"
#include "CriticalSection.h"
#include "JobThread.h"
#include "CPS_FCSRV_Trace.h"
#include "FCSession.h"
#include "FCPJobFactory.h"
#include "EventReporter.h"
#include "LinuxException.h"

#include "FCPMsg.h"
#include "FCPFunx.h"
#include "Version.h"


FCServer::FCServer(void) : TCPServer(), m_jobThread(NULL)
{
    //newTRACE((LOG_LEVEL_INFO, "FCServer::FCServer()", 0));
}

FCServer::~FCServer(void)
{
    //newTRACE((LOG_LEVEL_INFO, "FCServer::~FCServer(void)", 0));
    
    // Do nothing
}

void FCServer::run(void)
{
    newTRACE((LOG_LEVEL_INFO, "FCServer::run(void)", 0));

    // Use condition variable to notify other thread
    // that server is started
    {
        boost::lock_guard<boost::mutex> lock(m_mutex);
        m_running = true;
    }
    m_condition.notify_one();

    if (isTRACEABLE()) {
        std::ostringstream ss;
        ss << "FCServer::run() thread id "<< boost::this_thread::get_id();
        TRACE((LOG_LEVEL_INFO, "%s", 0, ss.str().c_str()));
    }

    TCPServer::run();

    // Stop the job thread
    m_jobThread->stop();

    m_running = false;
    TRACE((LOG_LEVEL_INFO, "FCServer::run(void) returns", 0));
}

bool FCServer::isRunning(bool trace)
{
	if (trace) {
    	newTRACE((LOG_LEVEL_INFO, "FCServer::isRunning(void)", 0));
    }

    return m_running;
}

void FCServer::incoming_data_handler(TCPConnection_ptr p, const char* data, std::size_t num)
{
    newTRACE((LOG_LEVEL_INFO, "FCServer::incoming_data_handler : %u", 0, num));
    
    // Create a FCSession
    m_fcSessions.push_back(SPFCSession(new FCSession(p, data, num, m_jobThread)));
}

void FCServer::stop(void)
{
    newTRACE((LOG_LEVEL_INFO, "FCServer::stop()", 0));
    m_jobThread->stop();
    TCPServer::stop();
}

void FCServer::jobDone(void)
{
    newTRACE((LOG_LEVEL_INFO, "FCServer::jobDone()", 0));
    io_service_.post(boost::bind(&FCServer::handle_jobDone, this));    
}

void FCServer::handle_jobDone(void)
{
    newTRACE((LOG_LEVEL_INFO, "FCServer::handle_jobDone()", 0));
    Job* job = m_jobThread->getJob();
    
    if (!job) {
        TRACE((LOG_LEVEL_WARN, "Attempting to handle a NULL job", 0));
        EventReporter::instance().write("Attempting to handle a NULL job");
        return;
    }
    
    VSPFCSession_Iter it;

    // Given the job id, find the connection to send data back    
    it = find_if(m_fcSessions.begin(), m_fcSessions.end(), 
            boost::bind(std::equal_to<long>(), job->jobId(),
                boost::bind(&FCSession::id, _1)));
        
    if (it != m_fcSessions.end()) {

        if (isTRACEABLE()) {    
            std::ostringstream ss;
            ss << "found the job id : " << job->jobId()
                << std::hex << " connection ptr is : " << (*it)->getConnection();
            TRACE((LOG_LEVEL_INFO, "%s", 0, ss.str().c_str()));
        }
 
        try {       

            (*it)->send(job);
            m_fcSessions.erase(it);        
            delete job;
        }
        catch (boost::system::system_error &e)
        {
            boost::system::error_code ec = e.code();
            TRACE((LOG_LEVEL_ERROR, "Attempting sending. Error code: %d - %s", 0, ec.value(), ec.message().c_str()));
            TRACE((LOG_LEVEL_ERROR, "Attempting sending stuff up", 0));
            EventReporter::instance().write("Attempting sending results in error");
        }
        
    }

}

void FCServer::waitUntilRunning(void)
{
    newTRACE((LOG_LEVEL_INFO, "FCServer::waitUntilRunning()", 0));

    boost::unique_lock<boost::mutex> lock(m_mutex);

    while (m_running == false)
    {
        TRACE((LOG_LEVEL_INFO, "Wait until FCServer Thread signal", 0));
        m_condition.wait(lock);
        TRACE((LOG_LEVEL_INFO, "FCServer Thread signaled", 0));
    }
}

