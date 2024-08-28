/*
 * LinuxException.cpp
 *
 *  Created on: Oct 14, 2010
 *      Author: xquydao
 */

#include "LinuxException.h"

#include <cassert>
#include <sstream>
#include <cerrno>

//
// ctor 1 - default msg, pick up error code
//----------------------------------------------------------------------
LinuxException::LinuxException(const char* file,
                               int line) : Exception("", errno, file, line)
{
    createMsg("");
}

//
// ctor 2 - default msg, error code as parameter
//----------------------------------------------------------------------
LinuxException::LinuxException(int errcode,
                               const char* file,
                               int line) : Exception("", errcode, file, line)
{
    createMsg("");
}

//
// ctor 3 - msg as parameter - pick up error code
//----------------------------------------------------------------------
LinuxException::LinuxException(const string& msg,
                               const char* file,
                               int line) : Exception("", errno, file, line)
{
    createMsg(msg);
}

//
// ctor 4 - msg & error code as parameters
//----------------------------------------------------------------------
LinuxException::LinuxException(const string& msg,
                               int errcode,
                               const char* file,
                               int line) : Exception("", errcode, file, line)
{
    createMsg(msg);
}

//
// create message
//----------------------------------------------------------------------
void LinuxException::createMsg(const string& msg)
{
    stringstream ss;

    ss << msg << (msg.empty() ? "" : " --- ") << "Linux Error: " << errcode() << endl;
    m_msg = ss.str();
}
