/*
NAME
    File_name:BUServer.cpp

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

#include <algorithm>
#include <vector>
#include <iterator>
#include <functional>
#include <iostream>
#include <iomanip>
#include <sstream>

#include "boost/bind.hpp"
#include <boost/thread/thread.hpp>


#include "BUServer.h"
#include "CriticalSection.h"
#include "JobThread.h"
#include "CPS_BUSRV_Trace.h"
#include "BUSession.h"
#include "BUPJobFactory.h"

#include "BUPMsg.h"
#include "BUPFunx.h"
#include "SectorInfo.h"
#include "Version.h"
#include "LoadBeginJob.h"
#include "SBCId.h"
#include "Config.h"
#include "fms_cpf_file.h"

BUServer::BUServer(void) : TCPServer(), m_jobThread(NULL), m_cpfEstablised(false), m_stopping(false),
								m_cs()
{
    //newTRACE((LOG_LEVEL_INFO, "BUServer::BUServer()", 0));
}

BUServer::~BUServer(void)
{
    //newTRACE((LOG_LEVEL_INFO, "BUServer::~BUServer(void)", 0));
    
    // Do nothing
}

void BUServer::stop_handle_service_dependency(bool& cpfUp)
{
	newTRACE((LOG_LEVEL_INFO, "BUServer::stop_handle_service_dependency()", 0));
    {
	   AutoCS a(m_cs);
	   m_stopping = true;
	   cpfUp = m_cpfEstablised;
	}
}

void BUServer::handle_service_dependency()
{
	newTRACE((LOG_LEVEL_INFO, "BUServer::handle_service_dependency()", 0));

	bool multiCP = Config::instance().isMultipleCP();
	static const char* name = "RELFSW200";
	struct timespec req;
	req.tv_sec = 1UL;
	req.tv_nsec = 200000000L;    // 200 ms = 200 000 000 nano seconds

	m_cpfEstablised = false;
    while (true) {
        try {           
            TRACE((LOG_LEVEL_INFO, "Checks if CPF Server is ready", 0));
            {
        	   AutoCS a(m_cs);
        	   if (m_stopping) {
        		   m_cpfEstablised = false;
        		   m_stopping = false;
        		   TRACE((LOG_LEVEL_INFO, "BUServer::handle_service_dependency() being stopped", 0));
        		   break;
        	   }
        	}

            if (multiCP) {
            	// Todo: when BC comes, loop through the CP list to pickup any valid cpId
            	// If there is no cp defined, then gave up.
            	// FMS needs to support this function properly
            	FMS_CPF_File file(name, "CP1");
            	if (file.exists()) {};		// Coverity dogma
            	TRACE((LOG_LEVEL_INFO, "CPF Server is ready; no error found; Multi CP System", 0));
            }
            else {
            	FMS_CPF_File file(name);
            	if (file.exists()) {};		// Coverity dogma
            	TRACE((LOG_LEVEL_INFO, "CPF Server is ready; no error found; One CP system", 0));
            }
        }
        catch (FMS_CPF_Exception& x) {
            if (x.errorCode() == 117) {
                TRACE((LOG_LEVEL_WARN, "CPF Server is not ready, try again", 0));
                nanosleep(&req, NULL);
                continue;
            }
            TRACE((LOG_LEVEL_INFO, "CPF Server ready with error <%d>, error text <%s>", 0, x.errorCode(), x.errorText()));
        }
        catch (...) {
            TRACE((LOG_LEVEL_ERROR, "Unknown, non FMS exception", 0));
        }
        m_cpfEstablised = true;
        TRACE((LOG_LEVEL_INFO, "BUServer::handle_service_dependency() returns", 0));
        break;
    }

}

void BUServer::run(void)
{
    newTRACE((LOG_LEVEL_INFO, "BUServer::run(void)", 0));

    // Use condition variable to notify other thread
    // that server is started
    {
        boost::lock_guard<boost::mutex> lock(m_mutex);
        m_running = true;
        m_fatal_error = 0;
    }
    m_condition.notify_one();

    // CPF server must run first
    handle_service_dependency();

   if (isTRACEABLE()) {
        std::ostringstream ss;
        ss << "BUServer::run() thread id "<< boost::this_thread::get_id();
        TRACE((LOG_LEVEL_INFO, "%s", 0, ss.str().c_str()));
    }

	if (m_cpfEstablised) {
		TCPServer::run();
	}

    // Since the TCPServer stopped, now stop the jobThread
    m_jobThread->stop();

    m_running = false;
    m_cpfEstablised = false;
    m_stopping = false;
    TRACE((LOG_LEVEL_INFO, "BUServer::run(void) returns", 0));
}

void BUServer::incoming_data_handler(TCPConnection_ptr p, const char* data, std::size_t num)
{
    newTRACE((LOG_LEVEL_INFO, "BUServer::incoming_data_handler : %u", 0, num));
    
    // Create a BUSession
    m_buSessions.push_back(SPBUSession(new BUSession(p, data, num, m_jobThread)));
    
}

void BUServer::stop(void)
{
    newTRACE((LOG_LEVEL_INFO, "BUServer::stop()", 0));
    bool cpfOn = false;
    stop_handle_service_dependency(cpfOn);
    m_jobThread->stop();
    if (cpfOn) {
    	TCPServer::stop();
    }
}

void BUServer::jobDone(void)
{
    newTRACE((LOG_LEVEL_INFO, "BUServer::jobDone()", 0));
    io_service_.post(boost::bind(&BUServer::handle_jobDone, this));    
}

void BUServer::handle_jobDone(void)
{
    newTRACE((LOG_LEVEL_INFO, "BUServer::handle_jobDone()", 0));
    Job* job = m_jobThread->getJob();
    
    if (!job) {
        TRACE((LOG_LEVEL_WARN, "Attempting to handle a NULL job", 0));
        EventReporter::instance().write("Attempting to handle a NULL job");
        return;
    }
    
    VSPBUSession_Iter it;

    // Given the job id, find the connection to send data back    
    it = find_if(m_buSessions.begin(), m_buSessions.end(), 
            boost::bind(std::equal_to<long>(), job->jobId(),
                boost::bind(&BUSession::id, _1)));
        
    if (it != m_buSessions.end()) {

        if (isTRACEABLE()) {    
            std::ostringstream ss;
            ss << "found the job id : " << job->jobId()
                << std::hex << " connection ptr is : " << (*it)->getConnection();
            TRACE((LOG_LEVEL_INFO, "%s", 0, ss.str().c_str()));
        }
 
        try {       

            (*it)->send(job);
            m_buSessions.erase(it);        
            delete job;
        }
        catch (/*boost::system::system_error &e*/ ... )
        {
            //boost::system::error_code ec = e.code(); 
            //TRACE(("Attempting sending. Error code: %d - %s", 0, ec.value(), ec.message().c_str()));
            TRACE((LOG_LEVEL_ERROR, "Attempting sending results in error", 0));
            EventReporter::instance().write("Attempting sending results in error");
        }
        
    }

}

void BUServer::waitUntilRunning(void)
{
    newTRACE((LOG_LEVEL_INFO, "BUServer::waitUntilRunning()", 0));

    boost::unique_lock<boost::mutex> lock(m_mutex);

    while (m_running == false)
    {
        TRACE((LOG_LEVEL_INFO, "Wait until BUServer Thread signal", 0));
        m_condition.wait(lock);
        TRACE((LOG_LEVEL_INFO, "BUServer Thread signaled", 0));
    }
}

