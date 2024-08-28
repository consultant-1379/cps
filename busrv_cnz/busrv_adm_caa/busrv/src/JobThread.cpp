/*
NAME
    File_name:JobThread.cpp

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
    2010-11-16 by XDT/DEK xdtthng
                  XDT/DEK xquydao


SEE ALSO
    -

Revision history
----------------
2010-11-16 xdtthng created
2011-01-10 xquydao Updated after review

*/

#include "boost/thread.hpp"
#include "boost/utility.hpp"
#include "boost/bind.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/thread/condition_variable.hpp"

#include "BUServer.h"
#include "CPS_BUSRV_Trace.h"
#include "CriticalSection.h"
#include "JobThread.h"
#include "Job.h"

//static boost::posix_time::milliseconds sleepTime = boost::posix_time::milliseconds(100);
const u_int16   JobThread::N100;
const u_int16   JobThread::N200;

JobThread::JobThread(BUServer& s) : m_abort(false), m_running(false), m_server(s),
		m_currJob(NULL), m_queuedJobs(this), m_Counter(0u)
{
    //newTRACE((LOG_LEVEL_INFO, "JobThread::JobThread() ctor", 0));
}

void JobThread::run()
{
    newTRACE((LOG_LEVEL_INFO, "JobThread::run()", 0));

    // Use condition variable to notify other thread
    // that the job thread is started
    {
        boost::lock_guard<boost::mutex> lock(m_mutex);
        m_running = true;
    }
    m_condition.notify_one();    
    
    // Reset variable to default value
    TRACE((LOG_LEVEL_INFO, "JobThread::run() is about to enter thread infinite run loop", 0));
    m_currJob = NULL;
    m_abort   = false;

    while (true) {
            
        {   // This scope is only for AutoCS usage
            AutoCS a(m_cs);
            if (++m_Counter % N100 == 0 && m_Counter % N200 != 0) {
                newTRACE((LOG_LEVEL_INFO, "JobThread::run() loop, CS counter <%ld> Job List size <%d>", 0, m_cs.getCounter(), m_pendingJobs.size()));
            }
            if (m_abort) {
                TRACE((LOG_LEVEL_INFO, "JobThread::run() being aborted ...", 0));
                break;
            }
            
            // Code from the base
    		currentJob(); // get new job (if any)
    	}
		//
		// we must leave the CS, so that we execute the Job while other threads
		// may be adding jobs, or polling for completion
		//----------------------------------------------------------------------
		if(m_currJob) {
			m_currJob->execute();
			nextJob(); // will enter/leave
		}
		//
		// if we don't have anything to do, let's sleep awhile
		//----------------------------------------------------------------------
		else {
			//boost::this_thread::sleep(sleepTime);
			//usleep(100000u); // 100,000 micro seconds
			struct timespec req;
			req.tv_sec = 0UL;
			req.tv_nsec = 100000000L;    // 100 ms = 100 000 000 nano seconds
			nanosleep(&req, NULL);			 
		}
        
    }
    
    m_running = false;
    m_abort = false;
    TRACE((LOG_LEVEL_INFO, "JobThread::run() exiting", 0));
}

void JobThread::stop()
{
    newTRACE((LOG_LEVEL_INFO, "JobThread::stop()", 0));
    AutoCS a(m_cs);
    m_abort = true;
	if(m_currJob) {
	   m_currJob->abort();
	}
}

bool JobThread::isAborted()
{
    newTRACE((LOG_LEVEL_INFO, "JobThread::isAborted()", 0));

    AutoCS a(m_cs);
	return m_abort;
}

//
// this functions takes ownership of the job 
//----------------------------------------------------------------------------
long JobThread::addJob(Job* job) {

	AutoCS acs(m_cs);
	newTRACE((LOG_LEVEL_INFO, "JobThread::addJob(Job Id <%d>), CS counter <%ld>, Job List size <%d>", 0, job->jobId(), m_cs.getCounter(), m_pendingJobs.size()));
	long id = job->jobId();
	m_pendingJobs.push_back(job);
	
	TRACE((LOG_LEVEL_INFO, "JobThread::addJob(), CS counter <%ld>, Job List size <%d>", 0, m_cs.getCounter(), m_pendingJobs.size()));
	
	return id;
}

//
// private, only use if you have the CritSection
// pick up next job from pending list
//----------------------------------------------------------------------------
void JobThread::currentJob() 
{        
	// Don't use enter...leave here!
	m_currJob = m_pendingJobs.empty() ? NULL : m_pendingJobs.front();
	if(m_currJob) {
		m_pendingJobs.pop_front();
        newTRACE((LOG_LEVEL_INFO, "JobThread::currentJob() picking up job id <%d>, Job List size <%d>, CS counter <%d>", 0, m_currJob->jobId(), m_pendingJobs.size(), m_cs.getCounter()));
	}
	else {
        if (m_Counter % N200 == 0) {
            newTRACE((LOG_LEVEL_INFO, "JobThread::currentJob() picks a NULL Job. Job List size <%d>, CS counter <%d>", 0, m_pendingJobs.size(), m_cs.getCounter()));
        }
	}
}

//
// private, remove AutoDelete job, or move to completed list
//----------------------------------------------------------------------------
void JobThread::jobDone() 
{
	//#if 0	
    AutoCS acs(m_cs);
	newTRACE((LOG_LEVEL_INFO, "JobThread::jobDone(), CS counter <%d>", 0, m_cs.getCounter()));
	if(m_currJob->autodelete())
		delete m_currJob;
	else {
		m_completedJobs.push_back(m_currJob);
		m_server.jobDone();
	}
	m_currJob = NULL;
	//#endif
	
    #if 0
    // This section is protected by one criticl section
	Job* currJob = NULL;
	{
	   AutoCS acs(m_cs);
	   newTRACE(("JobThread::jobDone() handle auto delete, CS counter <%d>", 0, m_cs.getCounter()));
	   currJob = m_currJob;
	   m_currJob = NULL;
	   
    	if(currJob->autodelete()) {
    		delete currJob;
    		return;
    	}
	}
	
	// This section is protected by a different critical section
    AutoCS acs(m_cscj);
	newTRACE(("JobThread::jobDone() place job in Complete Job Queue, CScj counter <%d>", 0, m_cscj.getCounter()));
	m_completedJobs.push_back(currJob);
	m_server.jobDone();
	#endif

}

Job* JobThread::getJob()
{
	AutoCS acs(m_cs);
	//AutoCS acs(m_cscj);
    //newTRACE(("JobThread::getJob(),  CScj counter <%d>", 0, m_cscj.getCounter()));
    newTRACE((LOG_LEVEL_INFO, "JobThread::getJob(),  CS counter <%d>", 0, m_cs.getCounter()));
    Job* res = m_completedJobs.empty() ? NULL : m_completedJobs.front();   
    if (res) 
        m_completedJobs.pop_front();     
    return res;
}

void JobThread::waitUntilRunning(void)
{
    newTRACE((LOG_LEVEL_INFO, "JobThread::waitUntilRunning()", 0));

    boost::unique_lock<boost::mutex> lock(m_mutex);

    while (m_running == false)
    {
        TRACE((LOG_LEVEL_INFO, "Wait until JobThread Thread signal", 0));
        m_condition.wait(lock);
        TRACE((LOG_LEVEL_INFO, "JobThread Thread signaled", 0));
    }
}
//
// Backup Protocol Interaction
//----------------------------------------------------------------------------
void JobThread::nextJob() {

	newTRACE((LOG_LEVEL_INFO, "JobThread::nextJob()", 0));

	u_int16 sbc;
	Job::JQS_STATE state = m_currJob->getState(sbc);
	
	switch (state) {

	case Job::JQS_NORMAL:		// This is the most executed cases
		jobDone();
	break;

	case Job::JQS_RSPCONT: {
		Job* help = m_currJob->getHelpJob();	// Keep LoadEndHelpJob
		jobDone();								// Done with LoadEndJob; returns
		help->executeSpecial(sbc);				// Check if need to execute DumpEndJob

		m_currJob = help;
		nextJob();
	}
	break;

	case Job::JQS_FLUSH:
		delete m_currJob;
		//TRACE(("Job Thread at JQS_FLUSH state", 0));

		m_currJob = m_queuedJobs.execute(sbc);	// Flush the job queue		
	break;

	case Job::JQS_QUEUED:					// Queuing the current job
		m_cs.enter();						// to delay its execution
		m_queuedJobs.add(sbc, m_currJob);	// and to vacate the job thread
		m_currJob = NULL;					// for "higher prioritised" jobs
		m_cs.leave();
	break;
	default:
		;
		// Can't really get here
	}
}

Job* JobQueue::execute(u_int16 sbc)
{
	newTRACE((LOG_LEVEL_INFO, "JobQueue::execute(%d)", 0, sbc));
	TRACE((LOG_LEVEL_INFO, "The size of the Job Queue is: %d", 0, m_jobQueue.size()));


    // There is no job queued for any sbc    
    JobMapIter  jmIt;
    jmIt = m_jobQueue.find(sbc);
    if (jmIt == m_jobQueue.end()) {
        return NULL;
    }

    // There is no job queued for the specific sbc. This is not reachable...        
    JobList& jobList = jmIt->second;
    if (jobList.empty())
        return NULL;

    TRACE((LOG_LEVEL_INFO, "******** JobQueue::execute(); size of jobList is: %d ", 0, jobList.size()));

    // Remember the most significant job and remove it from the queue
    Job *lastJob = jobList.back();
    jobList.pop_back();
    
	TRACE((LOG_LEVEL_INFO, "After popping, the size of the Job Queue is: %d", 0, jobList.size()));

    // Peform non op for previously queued jobs
    JobListIter it;
	for (it = jobList.begin(); it != jobList.end(); it++) {
        (*it)->executeNonOp(sbc);
		m_JobThread->m_cs.enter();
		m_JobThread->m_completedJobs.push_back(*it);
		m_JobThread->m_server.jobDone();
		m_JobThread->m_cs.leave();
	}

	// At the moment, there is at most .... well, ... one job in the queue
    jobList.erase(jobList.begin(), jobList.end());      
    
    TRACE((LOG_LEVEL_INFO, "******** JobQueue::execute(); starting lastJob->executeSpecial(sbc)", 0));

    // Execute the last Job
	lastJob->executeSpecial(sbc);
	m_JobThread->m_cs.enter();
	m_JobThread->m_completedJobs.push_back(lastJob);	// Finish the queued job 
	m_JobThread->m_server.jobDone();
	m_JobThread->m_cs.leave();

	return NULL;
}

