/*
Generic Application Message in BUSRV
*/

#ifndef _GENERIC_APPLICATION_MESSAGE_H_
#define _GENERIC_APPLICATION_MESSAGE_H_

#include <iostream>
#include <string.h>
#include "PlatformTypes.h"

using namespace std;

#pragma once
#pragma pack(push, 1)

class GAMsg 
{

public:
    enum { 
        VERSION = 1 
    };
    
    enum { 
        MSG_SIZE_IN_BYTES = 1024 
    };
    
    enum MESSAGE_CLASS {
        RESERVED_MESSAGE_CLASS =    0,
        MAINTENANCE_MESSAGE_CLASS = 1,
        SRM_MESSAGE_CLASS =         2,
        COMMAND_MESSAGE_CLASS =     3
    };
    
    //
    // GAMsgHeader - common header for all messages
    //================================================================
    class GAMsgClass {
    
    public:
        
        GAMsgClass(MESSAGE_CLASS mclass, u_int32 reservedClass = 0u) : m_messageClass(mclass), 
            m_messageClassReserve(reservedClass) {}
        GAMsgClass() {m_messageClass = RESERVED_MESSAGE_CLASS; m_messageClassReserve = 0;}
        
        MESSAGE_CLASS messageClass() const { return m_messageClass; }
        void messageClass(MESSAGE_CLASS mclass) { m_messageClass = mclass; }
        u_int32 messageClassReserve() const { return m_messageClassReserve;}
        
    private:
        MESSAGE_CLASS   m_messageClass;
        u_int32         m_messageClassReserve;
    };
    
   //
    //----------------------------------------------------------------
    // GAMsgHeader - common header for all GA messages
    //================================================================
    template<typename T>
    class GAMsgHeader : public GAMsgClass {
    
    public:
        GAMsgHeader(MESSAGE_CLASS mc, T prim, u_int32 version) :
            GAMsgClass(mc, 8888u), m_primitive(prim), m_version(version) { }
    
        T primitive() const { return m_primitive; }
        u_int32 version()  const { return m_version; }

        void primitive(T p) { m_primitive = p; }
        void version(u_int32 v) { m_version = v; }
        
    private:
        
        T           m_primitive;
        u_int32     m_version;
    };
    //
    //-----------------------------------------------------------------------
    // GARsgHeader - common header for all GA response messages
    //=======================================================================
    template <typename T>
    class GARspHeader : public GAMsgHeader<T> {
    
    public:
        GARspHeader(MESSAGE_CLASS mc, T prim, u_int32 version, u_int32 error) :
            GAMsgHeader<T>(mc, prim, version), m_errorCode(error) { }
    
        u_int32 errorCode() const { return m_errorCode; }
        void errorCode(u_int32 ecode) { m_errorCode = ecode; }
        
    private:        
        u_int32 m_errorCode;
    };
    
    
    GAMsg() { reset();}
    void reset() {
        memset(&m_buffer, 0, MSG_SIZE_IN_BYTES);
    }

    char* addr() { return reinterpret_cast<char*>(m_buffer); }
    const char* addr() const { return reinterpret_cast<const char*>(m_buffer); }
    u_int32 size() { return sizeof(m_buffer); }
    
    GAMsgClass* gaMsgHeader() { return reinterpret_cast<GAMsgClass*>(addr()); }
    const GAMsgClass* gaMsgHeader() const { return reinterpret_cast<const GAMsgClass*>(addr()); }
    MESSAGE_CLASS messageClass() const { return gaMsgHeader()->messageClass(); }
    u_int32 messageClassReserve() const { return gaMsgHeader()->messageClassReserve(); }
    
    
protected:
    u_int32 m_buffer[MSG_SIZE_IN_BYTES / sizeof(u_int32)];
};


#pragma pack(pop)
#endif
