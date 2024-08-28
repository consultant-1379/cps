/*
NAME
    File_name:CPS_BUAP_CS.h

Ericsson Utvecklings AB

    COPYRIGHT Ericsson Utvecklings AB, Sweden 2012. All rights reserved.

    The Copyright to the computer program(s) herein is the property of
    Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
    copied only with the written permission from Ericsson Utvecklings AB
    or in accordance with the terms and conditions stipulated in the
    agreement/contract under which the program(s) have been supplied.

DESCRIPTION
    This class is use for presenting the critical section

DOCUMENT NO
    190 89-CAA 109 1412

AUTHOR
    2012-19-04 by XDT/DEK/XNGUDAN

SEE ALSO
    -

Revision history
----------------
2012-19-04 xngudan Created (The content of this file is taken from BUSRV/CriticalSection.h)

*/

#ifndef CPS_BUAP_CS_H_
#define CPS_BUAP_CS_H_

#include "boost/thread/recursive_mutex.hpp"

class CPS_BUAP_CS {

public:
    CPS_BUAP_CS() : m_cs() { }
    ~CPS_BUAP_CS() {}
    void enter() { m_cs.lock(); }
    void leave() { m_cs.unlock(); }

private:
    boost::recursive_mutex  m_cs;
};


class AutoCS
{
public:
    AutoCS(CPS_BUAP_CS& cs) : m_cs(cs) { m_cs.enter(); }
    ~AutoCS() { m_cs.leave(); }

private:

    CPS_BUAP_CS& m_cs;
};


#endif /* CPS_BUAP_CS_H_ */
