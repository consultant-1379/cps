#ifndef _DSDMSG_H_
#define _DSDMSG_H_
/*
NAME
    BUPMSG -
LIBRARY 3C++
PAGENAME BUPMSG
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE BUProtocolMsg.H

COPYRIGHT
    COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

    The Copyright to the computer program(s) herein is the property of
    Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
    copied only with the written permission from Ericsson Utvecklings AB
    or in accordance with the terms and conditions stipulated in the
    agreement/contract under which the program(s) have been supplied.

DESCRIPTION
    Implements all DSD BUSRV internal messages


ERROR HANDLING
    -

DOCUMENT NO
    190 89-CAA 109 xxx

AUTHOR
    2010-05-23 by XDT/DEK/xdtthng

Revision history
----------------


LINKAGE
    -

SEE ALSO
    -

*/

#include "SBCId.h"
#include <assert.h>
#include <ostream>
#include <string.h>
class ACS_DSD_Session;

using namespace std;

#pragma pack(push, 1)
//
// 
//========================================================================
class DSDTestMsg {
public:
    enum { MAX_LENGTH = 31 };
//foos
public:
    DSDTestMsg();
    DSDTestMsg(int nofCharacters, char* character);
    int nofCharacters() const { return m_nofCharacters; }
    char* character() { return m_character; }
    void set(int nofCharacters, char* character);
	void set(int n) {m_nofCharacters = n; }
    friend ostream& operator<<(ostream& os, const DSDTestMsg& exid);
//attr
private:
    int   m_nofCharacters;
    char  m_character[MAX_LENGTH + 1]; // may be 0-terminated
};
//
// inlines
//===========================================================================

#pragma pack(pop)

#pragma once
//
//
//===================================================================

class DSDMsg {
// types
public:
    enum { VERSION = 1 };
    enum { MSG_SIZE_IN_BYTES = 1024 };

    enum PRIMITIVE {
    
        // Primitives for maintenance and testing purposes
        FIRST_RESERVED_PRIMITIVE = 0,
        MAINT_MSG_TEXT_ECHO = FIRST_RESERVED_PRIMITIVE,
        MAINT_MSG_TEXT_ECHO_RSP,
        MAINT_MSG_RESERVED,
        MAINT_MSG_RESERVED_RSP,
        LAST_RESERVED_PRIMITIVE = MAINT_MSG_RESERVED_RSP,

        // Functional primitives        
        FIRST_PRIMITIVE = LAST_RESERVED_PRIMITIVE + 1,
        SET_CBC_RELOAD_FILE,
        SET_CBC_RELOAD_FILE_RSP,
        CBC_FILE_EXIST,
        CBC_FILE_EXIST_RSP,
        LAST_PRIMITIVE = CBC_FILE_EXIST_RSP
    };
    
    enum { NUMBER_OF_PRIMITIVES = LAST_PRIMITIVE - FIRST_PRIMITIVE + 1 };

    enum ERROR_CODE {
        FIRST_ERROR_CODE = 0,
        OK = FIRST_ERROR_CODE,
        NOCONTACT,
        FILE_NOT_FOUND,
		INTERNAL_ERROR,
		RC_FILE_OUT_OF_RANGE,
		RC_FUNCTION_NOT_SUPPORTED,
        LAST_ERROR_CODE = RC_FUNCTION_NOT_SUPPORTED
    };
    enum { NUMBER_OF_ERROR_CODES = LAST_ERROR_CODE - FIRST_ERROR_CODE + 1 };
    
    static const char* ERROR_CODE_STRING[NUMBER_OF_ERROR_CODES];
    
    //
    // DSDMsgHeader - common header for all messages
    //================================================================
    class DSDMsgHeader {
    protected:
        DSDMsgHeader(PRIMITIVE prim, unsigned int version) : m_primitive(prim), m_version(version) { }
    public:
        // access
        PRIMITIVE primitive() const { return m_primitive; }
        unsigned int version()  const { return m_version; }
        // modify
        void primitive(PRIMITIVE p) { m_primitive = p; }
        void version(unsigned int v) { m_version = v; }
        friend ostream& operator<<(ostream& , const DSDMsgHeader& );
    private:
        PRIMITIVE       m_primitive;
        unsigned int    m_version;
    };
    //
    // DSDRspHeader - common header for all response messages
    //================================================================
    class DSDRspHeader : public DSDMsgHeader {
    public:
        DSDRspHeader(PRIMITIVE prim, unsigned int ver, unsigned int ecode)
            : DSDMsgHeader(prim, ver), m_errorCode(ecode) { }
        // access
        unsigned int errorCode() const { return m_errorCode; }
        // modify
        void errorCode(unsigned int ecode) { m_errorCode = ecode; }
        friend ostream& operator<<(ostream& , const DSDRspHeader& );
    private:
        unsigned int m_errorCode;
    };
    
    
    //---------------------------------------------------------------
    //
    //================================================================
    class SetReloadFileMsg : public DSDMsgHeader {
    // types
    public:
        static const PRIMITIVE s_primitive;
    public:
        SetReloadFileMsg(unsigned int fn)
        : DSDMsgHeader(s_primitive, VERSION), m_fileNumber(fn) { }
        // access
        unsigned int fileNumber() const { return m_fileNumber; }
        void fileNumber(unsigned int fn) { m_fileNumber = fn; }
        // modify
        friend ostream& operator<<(ostream& , const SetReloadFileMsg& );
    private:
        unsigned int m_fileNumber;
    };   
    //
    //================================================================
    class SetReloadFileRspMsg : public DSDRspHeader {
    // types
    public:
        static const PRIMITIVE s_primitive;
    public:
        SetReloadFileRspMsg(unsigned int error)
        : DSDRspHeader(s_primitive, VERSION, error) { }
        // access
        // modify
        friend ostream& operator<<(ostream& , const SetReloadFileRspMsg& );
    private:
    };


    //---------------------------------------------------------------
    //
    //================================================================
    class CBCFileExistMsg : public DSDMsgHeader {
    // types
    public:
        static const PRIMITIVE s_primitive;
    public:
        CBCFileExistMsg(unsigned int fn)
        : DSDMsgHeader(s_primitive, VERSION), m_fileNumber(fn) { }
        // access
        unsigned int fileNumber() const { return m_fileNumber; }
        void fileNumber(unsigned int fn) { m_fileNumber = fn; }
        // modify
        friend ostream& operator<<(ostream& , const CBCFileExistMsg& );
    private:
        unsigned int m_fileNumber;
    };   
    //
    //
    //================================================================
    class CBCFileExistRspMsg : public DSDRspHeader {
    // types
    public:
        static const PRIMITIVE s_primitive;
    public:
        CBCFileExistRspMsg(unsigned int fn, unsigned int error, unsigned int res)
        : DSDRspHeader(s_primitive, VERSION, error), m_fileNumber(fn), m_result(res) { }
        // access
        unsigned int fileNumber() const { return m_fileNumber; }
		unsigned int result() const { return m_result; }
        void fileNumber(unsigned int fn) { m_fileNumber = fn; }
        // modify
        friend ostream& operator<<(ostream& , const CBCFileExistRspMsg& );
    private:
        unsigned int m_fileNumber;
        unsigned int m_result;
    };   

    

    // This is for testing the DSD Server implementation in BUSRV
    //
    //================================================================
    class EchoTestMsg : public DSDMsgHeader {
    // types
    public:
        static const PRIMITIVE s_primitive;
    public:
        EchoTestMsg(int len, char* m)
        : DSDMsgHeader(s_primitive, VERSION), m_testMsg(len, m) { }
        
        int nofCharacters() const { return m_testMsg.nofCharacters(); }
        char* character() { return m_testMsg.character(); }
		void set(int n, char* s) {m_testMsg.set(n, s);}
        friend ostream& operator<<(ostream& , const EchoTestMsg& );
    private:
		unsigned int	m_holder;
        DSDTestMsg		m_testMsg;
    }; 
      
    //
    //
    //================================================================
    class EchoTestRspMsg : public DSDRspHeader {
    // types
    public:
        static const PRIMITIVE s_primitive;
    public:
        EchoTestRspMsg(unsigned int error)
        : DSDRspHeader(s_primitive, VERSION, error) {}

        int nofCharacters() const { return m_testMsg.nofCharacters(); }
        char* character() { return m_testMsg.character(); }
		void set(int len) { m_testMsg.set(len);}
        friend ostream& operator<<(ostream& , const EchoTestRspMsg& );
    private:
        DSDTestMsg m_testMsg;
    };
  
public:
    DSDMsg() { reset();}
    void reset() {
        //ZeroMemory(&m_buffer, MSG_SIZE_IN_BYTES);
        memset(&m_buffer, 0, MSG_SIZE_IN_BYTES);
    }

    // access
    char* addr() { return reinterpret_cast<char*>(m_buffer); }
    const char* addr() const { return reinterpret_cast<const char*>(m_buffer); }
    unsigned int size() { return sizeof(m_buffer); }
    
    DSDMsgHeader* msgHeader() { return reinterpret_cast<DSDMsgHeader*>(addr()); }
    const DSDMsgHeader* msgHeader() const { return reinterpret_cast<const DSDMsgHeader*>(addr()); }

    
private:
    unsigned int m_buffer[MSG_SIZE_IN_BYTES / sizeof(unsigned int)];

};

#endif
