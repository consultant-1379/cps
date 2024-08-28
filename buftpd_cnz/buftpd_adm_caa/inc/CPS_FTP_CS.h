/*
NAME
    File_name:CPS_FTP_CS.h

Ericsson Utvecklings AB

    COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

    The Copyright to the computer program(s) herein is the property of
    Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
    copied only with the written permission from Ericsson Utvecklings AB
    or in accordance with the terms and conditions stipulated in the
    agreement/contract under which the program(s) have been supplied.

DESCRIPTION
    This class is use for presenting the critical section

DOCUMENT NO
    190 89-CAA 109 1415

AUTHOR
    2011-14-07 by XDT/DEK/XQUYDAO

SEE ALSO
    -

Revision history
----------------
2011-14-07 xquydao Created (The content of this file is taken from BUSRV/CriticalSection.h)

*/

#ifndef CPS_FTP_CS_H_
#define CPS_FTP_CS_H_

#include "boost/thread/recursive_mutex.hpp"

class CPS_FTP_CS {

public:
	CPS_FTP_CS() : m_cs() { }
	~CPS_FTP_CS() {}
	void enter() { m_cs.lock(); }
	void leave() { m_cs.unlock(); }

private:
	boost::recursive_mutex  m_cs;
};


class AutoCS
{
public:
	AutoCS(CPS_FTP_CS& cs) : m_cs(cs) { m_cs.enter(); }
	~AutoCS() { m_cs.leave(); }

private:

	CPS_FTP_CS& m_cs;
};


#endif /* CPS_FTP_CS_H_ */
