#ifndef _FCSESSION_H_
#define _FCSESSION_H_
/*
NAME
	FCSession -
LIBRARY 3C++
PAGENAME FCSession
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE FCSession.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION
   Main class for implementing the BUProt message handling.

ERROR HANDLING
   -

DOCUMENT NO
   190 89-CAA 109 1410

AUTHOR
   2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on FCSession.h of FCSRV in Windows
   and BUSRV's BUSession.h

LINKAGE
	-

SEE ALSO
 	-

*/


#include "TCPConnection.h"
#include "FCPMsg.h"

class FCPJobFactory;
class JobThread;
class Job;

class FCSession : private boost::noncopyable
{
public:
    FCSession(TCPConnection_ptr, const char*, size_t, JobThread*);
    ~FCSession();
    long id() { return m_jobId; }
    TCPConnection_ptr getConnection() { return m_connection; }
    void send(Job*);
    

private:
    FCPMsg              m_protMsg;          // buffer for current FCPMsg in this session
    TCPConnection_ptr  m_connection;
    long                m_jobId;
};

typedef boost::shared_ptr<FCSession> SPFCSession;
//typedef FCSession* SPFCSession;
typedef std::vector<SPFCSession> VSPFCSession;
typedef std::vector<SPFCSession>::iterator VSPFCSession_Iter;


#endif

// rrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr
//
// From here downward is the Window version of the base product
// Will be removed
//
// rrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr

#if 0
// fwd
class BUManager;

class FCSession {
//foos
public:
	FCSession();
	~FCSession();
	const NetBuffer& netbuf() const { return m_netbuf; }
	void bytesTransferred(DWORD dw);
	WSABUF* wsabuf() { return m_netbuf.wsabuf(); }
	bool poll(); // return true if job was extracted/completed
private:
	bool jobCompleted(); // any pending jobs completed?

	FCSession(const FCSession& ); // not impl.
	FCSession& operator=(const FCSession& ); // -:-
//attr
private:
	// decl. order significant here!
	FCPMsg m_protMsg; // buffer for current FCPMsg in this session
	NetBuffer m_netbuf; // helper for sending & receiving data over TCP
	long m_jobId; // holds id of any pending job 
};
//
// inlines
//----------------------------------------------------------------------------
// return true if job was extracted
// this could have been placed in the beginning of jobCompleted(), but this
// way we only enter that function when we have a pending job
//----------------------------------------------------------------------------
inline bool FCSession::poll() {
	assert(!m_netbuf.isDone());
	return m_netbuf.isPending() ? jobCompleted() : false;
}

#endif
