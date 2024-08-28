/*
NAME
    File_name:CPS_BUSRV_Event_FD.h

Ericsson Utvecklings AB

    COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

    The Copyright to the computer program(s) herein is the property of
    Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
    copied only with the written permission from Ericsson Utvecklings AB
    or in accordance with the terms and conditions stipulated in the
    agreement/contract under which the program(s) have been supplied.

DESCRIPTION
    This class is use for presenting the Event

DOCUMENT NO
    190 89-CAA 109 0387

AUTHOR
    2011-14-07 by XDT/DEK/XQUYDAO

SEE ALSO
    -

Revision history
----------------
2011-08-11 xquydao Created (The content of this file is taken from BUFTPD)

*/
#ifndef CPS_BUSRV_EVENT_FD_H_
#define CPS_BUSRV_EVENT_FD_H_


#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/eventfd.h>
#include <stdint.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <boost/noncopyable.hpp>
#include "CriticalSection.h"

class CPS_BUSRV_Event_FD : private boost::noncopyable {

public:

    enum STATE {
        INVALID,
        IDLE,
        WRITE,
        READ
    };

    CPS_BUSRV_Event_FD() : mState(INVALID), mError(0), mFd(-1) {
        int ret = eventfd(0, 0);
        if ( ret == -1) {
            mError = errno;

        }
        else{
            AutoCS a(mCs);
            mFd = ret;
            fcntl(mFd,F_SETFL,O_NONBLOCK);
            mState = IDLE;
        }
    }

    ~CPS_BUSRV_Event_FD() {
        close(mFd);
        mState = INVALID;
    }

    int getFd () const {
        return mFd;
    }

    bool setEvent() {

        uint64_t u = 1ULL;

        mCs.enter();
        if (mState != IDLE) {
            mCs.leave();
            return false;
        }
        mCs.leave();

        bool res = write(mFd, &u, sizeof(uint64_t)) == sizeof(uint64_t);
        mError = errno;

        AutoCS a(mCs);
        mState = res? WRITE : IDLE;
        return res;
    }

    bool resetEvent() {

        uint64_t u = 1ULL;
        mCs.enter();
        if (mState != WRITE) {
            mCs.leave();
            return false;
        }
        mCs.leave();

        int ret;
        do {
            ret = read(mFd, &u, sizeof(uint64_t));
            mError = errno;
            u = 0ULL;
        } while (!(ret == -1 && mError == EAGAIN));

        AutoCS a(mCs);
        mState = IDLE;
        return true;
    }

private:

    STATE           mState;
    int             mError;
    int             mFd;
    CriticalSection mCs;
};

#endif
