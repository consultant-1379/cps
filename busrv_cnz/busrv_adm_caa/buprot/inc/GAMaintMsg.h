#ifndef _GENERIC_MAINTENANCE_MESSAGE_H_
#define _GENERIC_MAINTENANCE_MESSAGE_H_

#include "GAMsg.h"

#pragma once
#pragma pack(push, 1)


class GAMaintMsg: public GAMsg
{
public:

    enum MAINT_PRIMITIVE {
    
        // Primitives for maintenance and testing purposes
        FIRST_RESERVED_PRIMITIVE = 10,
        MAINT_MSG_TEXT_ECHO = FIRST_RESERVED_PRIMITIVE,
        MAINT_MSG_TEXT_ECHO_RSP
    };    
     //
    // 
    //========================================================================
    class GAMaintTestMsg {
    public:
        enum { MAX_LENGTH = 64 };
    //foos
    public:
        GAMaintTestMsg() {};
        GAMaintTestMsg(int n, const char* ch) {set(n, ch); }
        int nofCharacters() const { return m_nofCharacters; }
        const char* characters() const { return m_character; }
        void set(int nofCharacters, const char* character);
    	void set(int n) {m_nofCharacters = n; }
        friend ostream& operator<<(ostream& os, const GAMaintTestMsg& exid);
    //attr
    private:
        int   m_nofCharacters;
        char  m_character[MAX_LENGTH + 1]; // may be 0-terminated
    };
    //
    // This is for testing the DSD Server implementation in BUSRV
    //
    //================================================================
    class EchoTestMsg : public GAMsgHeader<MAINT_PRIMITIVE> {
    
    public:
        enum { MAX_LENGTH = 64 };
        static const MAINT_PRIMITIVE s_primitive = MAINT_MSG_TEXT_ECHO;;
    public:
        EchoTestMsg(int len, const char* m): 
            GAMsgHeader<MAINT_PRIMITIVE>(GAMsg::MAINTENANCE_MESSAGE_CLASS, s_primitive, VERSION), m_testMsg(len, m) {}
        
        int nofCharacters() const { return m_testMsg.nofCharacters(); }
        const char* characters() const { return m_testMsg.characters(); }
        friend ostream& operator<<(ostream& , const EchoTestMsg& );
    private:
        GAMaintTestMsg  m_testMsg;
    }; 
    //
    //
    //================================================================
    class EchoTestRspMsg : public GARspHeader<MAINT_PRIMITIVE> {
                                                    
    public:
        enum { MAX_LENGTH = 64 };
        static const MAINT_PRIMITIVE s_primitive = MAINT_MSG_TEXT_ECHO_RSP;;
    public:
        EchoTestRspMsg(int len, const char* m, u_int32 code): 
            GARspHeader<MAINT_PRIMITIVE>(GAMsg::MAINTENANCE_MESSAGE_CLASS, s_primitive, VERSION, code), m_testMsg(len, m) { }
        
        int nofCharacters() const { return m_testMsg.nofCharacters(); }
        const char* characters() const { return m_testMsg.characters(); }
        void set(int n, const char* s);
        friend ostream& operator<<(ostream& , const EchoTestMsg& );
    private:
        GAMaintTestMsg  m_testMsg;                
    }; 
    
    GAMsgHeader<MAINT_PRIMITIVE>* msgHeader()  {
        return reinterpret_cast<GAMsgHeader<MAINT_PRIMITIVE>* >(addr()); 
    }
    const GAMsgHeader<MAINT_PRIMITIVE>* msgHeader() const {
        return reinterpret_cast<const GAMsgHeader<MAINT_PRIMITIVE>* >(addr()); 
    }
    
    GARspHeader<MAINT_PRIMITIVE>* rspHeader() {
        return reinterpret_cast<GARspHeader<MAINT_PRIMITIVE>* >(addr()); 
    }
    const GARspHeader<MAINT_PRIMITIVE>* rspHeader() const {
        return reinterpret_cast<const GARspHeader<MAINT_PRIMITIVE>* >(addr()); 
    }
};

#pragma pack(pop)

#endif

