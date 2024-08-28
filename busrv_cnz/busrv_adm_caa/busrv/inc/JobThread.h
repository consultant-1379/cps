/*
NAME
    File_name:JobThread.h

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

#ifndef JOB_THREAD_H_
#define JOB_THREAD_H_

#include <list>
#include "PlatformTypes.h"

namespace boost {
    class mutex;
    class condition_variable;
}

class CriticalSection;
class BUServer;
class Job;
class JobThread;

typedef std::list<Job*> JobList;
typedef std::list<Job*>::iterator	JobListIter;


typedef map<u_int16, JobList>           JobMap;
typedef map<u_int16, JobList>::iterator JobMapIter;

// NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
// Backup Protocol Interaction
//
class JobQueue
{
public:
    JobQueue(JobThread* jt) : m_JobThread(jt) {};
    ~JobQueue() {}   
    
    // Add job to the Job Queue
    void add(u_int16 sbc, Job* job) {
        m_jobQueue[sbc].push_back(job);
    }
    
    Job* execute(u_int16 sbc);
    
private:
    JobThread*	m_JobThread;
    JobMap    	m_jobQueue;
};
//
// NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN


class JobThread : private boost::noncopyable
{
public:
	friend class JobQueue;
    JobThread(BUServer&);
    void run();
    void stop();
    bool isAborted();
    
    long addJob(Job* job);
    Job* getJob();
    void waitUntilRunning(void);
    
private:
    bool            m_abort;
    bool            m_running;
    BUServer&       m_server;
 
    Job* 			m_currJob;	// must be NULL or a valid Job ptr

    JobQueue 		m_queuedJobs;
    u_int32     	m_Counter;  // This increment every loop which is 100ms

    JobList m_pendingJobs;      // list of jobs waiting to be executed
    JobList m_completedJobs;    // list of finished jobs

    // For condition variable
    boost::mutex    			m_mutex;
    boost::condition_variable 	m_condition;
    CriticalSection 			m_cs;
    //CriticalSection m_cscj;   // For completed Jobs
    
    void currentJob();          // m_currJob = first job in pending list if any
    void nextJob();
    void jobDone();
    
    // For debugging purposes
    // Counter constant in JobThread run loop
    static const    u_int16    N100 = 300u;    // Count every 30 seconds (300u)
    static const    u_int16    N200 = 1200u;   // Count every 120 seconds (2 minutes) (1200u)
    
};

#endif

