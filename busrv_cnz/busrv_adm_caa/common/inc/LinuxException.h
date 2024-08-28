/*
 * LinuxException.h
 *
 *  Created on: Oct 14, 2010
 *      Author: xquydao
 */

#ifndef LINUXEXCEPTION_H_
#define LINUXEXCEPTION_H_

#include "Exception.h"

#include <sstream>

using namespace std;
//
// user provided code & message
//----------------------------------------------------------------------------
#define THROW_LINUX(msg, errcode) { stringstream ss; ss << msg; \
    LinuxException x(ss.str(), errcode, __FILE__, __LINE__); \
    DUMP_THROW_X(x)
//
// auto-fetched code, generic message
//----------------------------------------------------------------------------
#define THROW_LINUX_AUTO() { LinuxException x(__FILE__, __LINE__); DUMP_THROW_X(x)
//
// user provided code, generic message
//----------------------------------------------------------------------------
#define THROW_LINUX_CODE(errcode) { LinuxException x((errcode), __FILE__, __LINE__); \
    DUMP_THROW_X(x)
//
// auto-fetched code, user provided message
//----------------------------------------------------------------------------
#define THROW_LINUX_MSG(msg) { stringstream ss; ss << msg; \
    LinuxException x(ss.str(), __FILE__, __LINE__); \
    DUMP_THROW_X(x)
//
// auto-fetched code, user provided message, and log
//----------------------------------------------------------------------------
#define THROW_LINUX_MSG_LOG(msg) { stringstream ss; ss << msg; \
    LinuxException x(ss.str(), __FILE__, __LINE__); \
    EventReporter::instance().write(x); \
    DUMP_THROW_X(x)
//
// Linux - errno exception
//----------------------------------------------------------------------------
class LinuxException : public Exception {
//foos
public:
    LinuxException(const char* file, int line);
    LinuxException(int errcode, const char* file, int line);
    LinuxException(const string& msg, const char* file, int line);
    LinuxException(const string& msg, int errcode, const char* file, int line);
    ~LinuxException() { }
    virtual const char* name() const { return "Linux API call error."; }
private:
    void createMsg(const string& );
//attr
private:
    // -
};

#endif /* LINUXEXCEPTION_H_ */
