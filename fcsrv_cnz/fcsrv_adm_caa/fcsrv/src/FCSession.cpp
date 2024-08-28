/*
NAME
   File_name:FCSession.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

The Copyright to the computer program(s) herein is the property of Ericsson
Utvecklings AB, Sweden.
The program(s) may be used and/or copied only with the written permission from
Ericsson Utvecklings AB or in accordance with the terms and conditions
stipulated in the agreement/contract under which the program(s) have been
supplied.

.DESCRIPTION
   Main class for implementing the BUProt command handling.

DOCUMENT NO
   190 89-CAA 109 1410

AUTHOR
   2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on FCSession.cpp of FCSRV in Windows
   and BUSRV's BUSession.cpp


SEE ALSO
   -

Revision history
----------------
	2001-03-20 Created
	2002-10-20 uabmnst Add ACSTrace
	2004-06-18 uablan  cleanupJob is to be done only at LOAD_BEGIN, DUMP_BEGIN
	                   FALLBACK_BEGIN, SBC_SWITCH, LIST_SBCS, e.i at dump,load
					   sybfp, sytuc and synic.
*/

#include "boost/thread/mutex.hpp"
#include "boost/thread/condition_variable.hpp"

#include "Config.h"
#include "FCSession.h"
#include "TCPServer.h"

#include "FCPJobFactory.h"
#include "CriticalSection.h"
#include "JobThread.h"
//#include "CleanDirsJob.h"
#include "CPS_FCSRV_Trace.h"
#include "CodeException.h"
#include "LinuxException.h"
#include "FCPFunx.h"
//#include "DataFile.h"

#include <assert.h>
//#include <new.h>

#include <iostream>

//
// ctor
//-----------------------------------------------------------------------------
FCSession::FCSession(TCPConnection_ptr p, const char* data, size_t, JobThread* jobThread) :
						m_connection(p), m_jobId(0)
{

	// ToDo Need to check size_t n, the size of data received
	//

	newTRACE((LOG_LEVEL_INFO, "FCSession::FCSession()", 0));

	memset(m_protMsg.addr(), 0, 1024);
	memcpy(m_protMsg.addr(), data, 1024);

	Job* job = new FCPJobFactory(m_protMsg);
	m_jobId = jobThread->addJob(job);


    // Following is for testing the blocking server
    // Doing one job at a time
    //
#if 0

	job->execute();
	FCPJobFactory* jobfactory = reinterpret_cast<FCPJobFactory*>(job);
	memset(m_protMsg.addr(), 0, 1024);
	jobfactory->response(m_protMsg);
	m_connection->send(m_protMsg.addr(), 1024);

#endif
}
//
// dtor
//-----------------------------------------------------------------------------
FCSession::~FCSession() {
	newTRACE((LOG_LEVEL_INFO, "FCSession::~FCSession()", 0));
}

void FCSession::send(Job* job)
{
    newTRACE((LOG_LEVEL_INFO, "FCSession::send()", 0));
	memset(m_protMsg.addr(), 0, 1024);
	FCPJobFactory* obj = reinterpret_cast<FCPJobFactory*> (job);
    obj->response(m_protMsg);
    
    // Can send message directly using connection
	// m_connection->send(m_protMsg.addr(), 1024);
	// However, sending via the TCP server to allow the server performing
	// house keeping if required
	m_connection->getTCPServer().send(m_connection, m_protMsg.addr(), 1024);
	TRACE((LOG_LEVEL_INFO, "FCSession::send() returns", 0));
}

// rrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr
//
// From here downward is the Window version of the base product
// Will be removed
//
// rrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr

#if 0

//
// ctor
//-----------------------------------------------------------------------------
FCSession::FCSession() {
	newTRACE(("FCSession::FCSession()", 0));
	m_jobId = 0;
}
//
// dtor
//-----------------------------------------------------------------------------
FCSession::~FCSession() {
	newTRACE(("FCSession::~FCSession()", 0));
	if(m_jobId)
		JobThread::instance().abort(m_jobId); // will also delete the job
}
//
// something sent or received
// 1. check if the whole buffer was transferred
// 2. if TRUE continue
// 3. was it a recv (new message from CP), or a send (just a response)
// 4. take action accordingly
//
// note that any exceptions are throw out of this method. In this case
// the TCPServer will log the error, and shut down the connection
//-----------------------------------------------------------------------------
void FCSession::bytesTransferred(DWORD dw) {
	newTRACE(("FCSession::bytesTransferred(%u)", 0, dw));
	assert(!m_netbuf.isDone() && !m_netbuf.isPending());
	// is transfer complete
	if(m_netbuf.bytesTransferred(dw)) {
		if(m_netbuf.isReceiving()) {
			assert(!m_jobId); // make sure we don't have any running jobs

			// check which kind of job it is! 
            //-FCSRVR VILKA JOBB SKA MAN GGA IN cleanDirsJob
			switch( m_protMsg.msgHeader()->primitive() ) {
			
				case BUPMsg::DUMP_BEGIN:
					// remove old dirs... (move to BUPJobFactory?)
					if (Config::instance().isMultipleCP()) {
						const BUPMsg::DumpBeginMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::DumpBeginMsg>(m_protMsg);
						if (cmd->cpSystem() == BUPMsg::ONE_CP_SYSTEM)
							JobThread::instance().addJob(*(new CleanDirsJob(0xFFFF)));
						else {
							
							CPID cpId = cmd->cpId();
							Version tProtVer = m_protMsg.requestHeader()->version();
							if (cpId <= 63 && tProtVer >= Version(4, 0)) {
								// If this blade is dumping then do not clean directory
								//const Config::CLUSTER t(cpId, true);
								//if (!DataFile::dumping(t))
									JobThread::instance().addJob(*(new CleanDirsJob(cpId, true)));
							}
							else
								JobThread::instance().addJob(*(new CleanDirsJob(cpId)));
						}
					}
					else {
						JobThread::instance().addJob(*(new CleanDirsJob));
					}
					break;

				default:
					break;
			}
			
			Job* job = new BUPJobFactory(m_protMsg);
			m_jobId = JobThread::instance().addJob(*job);
			
			// change netbuf state to PENDING
			m_netbuf.pending();
		}
		else if(m_netbuf.isSending()) {
			m_netbuf.recv();
		}
		else {
			assert(!"Unreachable code");
		}
	}
}
//
// any pending jobs completed?
// this func is polled by the TCPServer (thru FCSession::poll())
//-----------------------------------------------------------------------------
bool FCSession::jobCompleted() {
	newTRACE(("FCSession::jobCompleted()", 0));
	assert(m_netbuf.isPending()); // state is changed in respond(...)

	// check if job is completed
	// cast must be safe since FCSession, which holds the m_jobId, only polls for
	// BUPJobFactory objects
	BUPJobFactory* job = reinterpret_cast<BUPJobFactory*>(JobThread::instance().jobCompleted(m_jobId));
	if(job) {
		m_jobId = 0; // just for assert
		job->response(m_protMsg); // get response data
		m_netbuf.send();
		delete job;
		return true;
	}
	return false;
}
#endif

