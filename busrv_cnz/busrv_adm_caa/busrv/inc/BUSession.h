#ifndef _BUSESSION_H_
#define _BUSESSION_H_
/*
NAME
	BUSESSION -
LIBRARY 3C++
PAGENAME BUSESSION 
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE BUSESSION.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2001. All rights reserved.

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
   190 89-CAA 109 0387

AUTHOR
   2001-03-20 by U/Y/SF Anders Gillgren (qabgill, tag: ag)

LINKAGE
	-

SEE ALSO
 	-

*/


#include "TCPConnection.h"
#include "BUPMsg.h"

class BUPJobFactory;
class JobThread;
class Job;

class BUSession : private boost::noncopyable
{
public:
    BUSession(TCPConnection_ptr, const char*, size_t, JobThread*);
    ~BUSession();
    long id() { return m_jobId; }
    TCPConnection_ptr getConnection() { return m_connection; }
    void send(Job*);
    

private:
    BUPMsg              m_protMsg;          // buffer for current BUPMsg in this session
    TCPConnection_ptr  m_connection;
    long                m_jobId;
};

typedef boost::shared_ptr<BUSession> SPBUSession;
//typedef BUSession* SPBUSession; 
typedef std::vector<SPBUSession> VSPBUSession;
typedef std::vector<SPBUSession>::iterator VSPBUSession_Iter;


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

class BUSession {
//foos
public:
	BUSession();
	~BUSession();
	const NetBuffer& netbuf() const { return m_netbuf; }
	void bytesTransferred(DWORD dw);
	WSABUF* wsabuf() { return m_netbuf.wsabuf(); }
	bool poll(); // return true if job was extracted/completed
private:
	bool jobCompleted(); // any pending jobs completed?

	BUSession(const BUSession& ); // not impl.
	BUSession& operator=(const BUSession& ); // -:-
//attr
private:
	// decl. order significant here!
	BUPMsg m_protMsg; // buffer for current BUPMsg in this session
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
inline bool BUSession::poll() {
	assert(!m_netbuf.isDone());
	return m_netbuf.isPending() ? jobCompleted() : false;
}

#endif

