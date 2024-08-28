/*
 * AutoFd.h
 *
 *  Created on: Oct 29, 2010
 *      Author: xquydao
 */

#ifndef AUTOm_fdH_
#define AUTOm_fdH_

#include <cassert>
#include "FileFunx.h"

const int INVALID_FD_VALUE = -1;

// AutoFd for file descriptor
class AutoFd
{
public:
    AutoFd(int fd = INVALID_FD_VALUE) : m_fd(fd) { }

    ~AutoFd() { close(); }

    AutoFd& operator=(int fd);

    bool valid() const { return m_fd > INVALID_FD_VALUE; }

    void close();

    int handle() const { return m_fd; }

// Methods
private:
    // ass-op & copy ctor could be implemented, but the sematics aren't as clear
    // as they might appear at first glance. So for now they are disallowed.
    AutoFd(const AutoFd& );
    AutoFd& operator=(const AutoFd&);

// Attributes
private:
    int m_fd;
};

//
// Close the fd
// ===========================================
inline void AutoFd::close()
{
    if (valid())
    {
#ifdef _DEBUG
        assert(FileFunx::CloseFileX(m_fd));
#else
        FileFunx::CloseFileX(m_fd);
#endif
        m_fd = INVALID_FD_VALUE;
    }
}

//
//
// ===========================================
inline AutoFd& AutoFd::operator=(int fd)
{
    if (fd != m_fd)
    {
        close();
        m_fd = fd;
    }

    return *this;
}

#endif /* AUTOm_fdH_ */
