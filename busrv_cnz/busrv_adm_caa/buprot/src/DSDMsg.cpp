/*
NAME
    File_name:DSDMsg.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

    The Copyright to the computer program(s) herein is the property of Ericsson
    Utvecklings AB, Sweden.
    The program(s) may be used and/or copied only with the written permission from
    Ericsson Utvecklings AB or in accordance with the terms and conditions
    stipulated in the agreement/contract under which the program(s) have been
    supplied.

DESCRIPTION
    Implements all the BUP Messages.

DOCUMENT NO
    190 89-CAA 109 

AUTHOR
    2002-02-01 by XDT/DEK/xdtthng for SSI

SEE ALSO
    -

Revision history
----------------
2002-02-01 qabgill Created
2006-06-09 uablan  Added CP-AP link speed in return msg for DumpBeginRsp,LoadBeginRsp
                   and FallbackBeginRsp.

*/
#include "stdafx.h"
#include "ACS_DSD_DSA2.h"
#include "DSDMsg.h"
#include "CPS_BUSRV_Trace.h"

#include <iostream>
#include <iomanip>


//
// Static Inits
//====================================================================

const DSDMsg::PRIMITIVE DSDMsg::EchoTestMsg::s_primitive = DSDMsg::MAINT_MSG_TEXT_ECHO;
const DSDMsg::PRIMITIVE DSDMsg::EchoTestRspMsg::s_primitive = DSDMsg::MAINT_MSG_TEXT_ECHO_RSP;

const DSDMsg::PRIMITIVE DSDMsg::SetReloadFileMsg::s_primitive = DSDMsg::SET_CBC_RELOAD_FILE;
const DSDMsg::PRIMITIVE DSDMsg::SetReloadFileRspMsg::s_primitive = DSDMsg::SET_CBC_RELOAD_FILE_RSP;

const DSDMsg::PRIMITIVE DSDMsg::CBCFileExistMsg::s_primitive = DSDMsg::CBC_FILE_EXIST;
const DSDMsg::PRIMITIVE DSDMsg::CBCFileExistRspMsg::s_primitive = DSDMsg::CBC_FILE_EXIST_RSP;

//
//============================================================================
ostream& operator<<(ostream& os, const DSDMsg::DSDMsgHeader& msg) 
{
    os << dec << "<" << msg.primitive() << "><" << msg.version() << ">";
    return os;
}

ostream& operator<<(ostream& os, const DSDMsg::SetReloadFileMsg& msg)
{
    os << static_cast<const DSDMsg::DSDMsgHeader&>(msg) << "<" <<
        msg.m_fileNumber << ">";
    return os;
}

ostream& operator<<(ostream& os, const DSDMsg::DSDRspHeader& msg)
{
    os << static_cast<const DSDMsg::DSDMsgHeader&>(msg) << "<" <<
        msg.m_errorCode << ">";
    return os;

}

ostream& operator<<(ostream& os, const DSDMsg::SetReloadFileRspMsg& msg)
{
    os << static_cast<const DSDMsg::DSDRspHeader&>(msg);
    
    return os;
}

ostream& operator<<(ostream& os, const DSDMsg::CBCFileExistMsg& msg)
{
    os << static_cast<const DSDMsg::DSDMsgHeader&>(msg) << "<" <<
        msg.m_fileNumber << ">";
    return os;
}

ostream& operator<<(ostream& os, const DSDMsg::CBCFileExistRspMsg& msg)
{
    os << static_cast<const DSDMsg::DSDRspHeader&>(msg) << "<" <<
        msg.m_fileNumber << ">" << "<" << msg.m_result << ">" ;
    return os;
}

//
// ctor 1
//===========================================================================
DSDTestMsg::DSDTestMsg() {

    //newTRACE(("DSDTestMsg::DSDTestMsg()", 0));

    //set(0, 0);
}
//
// ctor 2
//===========================================================================
DSDTestMsg::DSDTestMsg(int nofCharacters, char* character) {
    //newTRACE(("DSDTestMsg::DSDTestMsg(%D, %s)", 0, nofCharacters, character));
    set(nofCharacters, character);
}
//
//
//===========================================================================
void DSDTestMsg::set(int nofCharacters, char* character) {
    //newTRACE(("DSDTestMsg::set(%d, %s)", 0, nofCharacters, character));

    assert(nofCharacters <= MAX_LENGTH);
    m_nofCharacters = nofCharacters <= MAX_LENGTH ? nofCharacters : 0;
    //ZeroMemory(m_character, MAX_LENGTH);
    memset(&m_character, 0, MAX_LENGTH);
    //CopyMemory(m_character, character, m_nofCharacters);
	if (character)
		memcpy(m_character, character, m_nofCharacters + 1);
}
//
// friend
//===========================================================================
ostream& operator<<(ostream& os, const DSDTestMsg& exid) {
    os << dec << "<" << exid.nofCharacters() << "><";
    //for(int i = 0; i < DSDTestMsg::MAX_LENGTH; ++i)
    //    os << (exid.m_character[i] == 0 ? ' ' : static_cast<char>(exid.m_character[i]));
    os << exid.m_character << ">";
    return os;
}

ostream& operator<<(ostream& os, const DSDMsg::EchoTestMsg& msg)
{
    os << static_cast<const DSDMsg::DSDMsgHeader&>(msg) << "<" <<
        msg.m_testMsg << ">";
    return os;
}

ostream& operator<<(ostream& os, const DSDMsg::EchoTestRspMsg& msg)
{
    os << static_cast<const DSDMsg::DSDRspHeader&>(msg) << "<" <<
        msg.m_testMsg << ">";
    
    //os << static_cast<const DSDMsg::DSDRspHeader&>(msg);
    
    return os;
}


////////////////////////////////////////////////
// This is the test program
////////////////////////////////////////////////

/*
using namespace std;

void print(DSDMsg* msg)
{
    switch(msg->msgHeader()->primitive()) {
    case DSDMsg::MAINT_MSG_TEXT_ECHO:
        const DSDMsg::EchoTestMsg* echoMsg = reinterpret_cast<DSDMsg::EchoTestMsg*>(msg->addr());
        cout << *echoMsg << endl;

        break;
    case DSDMsg::MAINT_MSG_TEXT_ECHO_RSP:
        const DSDMsg::EchoTestRspMsg* echoRsp = reinterpret_cast<DSDMsg::EchoTestRspMsg*>(msg->addr());
        cout << *echoRsp << endl;

        break;
    case DSDMsg::SET_CBC_RELOAD_FILE:
        const DSDMsg::SetReloadFileMsg* set = reinterpret_cast<DSDMsg::SetReloadFileMsg*>(msg->addr());
        cout << *set << endl;

        break;
    case DSDMsg::SET_CBC_RELOAD_FILE_RSP:
        const DSDMsg::SetReloadFileRspMsg* setRsp = reinterpret_cast<DSDMsg::SetReloadFileRspMsg*>(msg->addr());
        cout << *setRsp << endl;

    case DSDMsg::CBC_FILE_EXIST:
        const DSDMsg::CBCFileExistMsg* cbcExist = reinterpret_cast<DSDMsg::CBCFileExistMsg*>(msg->addr());
        cout << *cbcExist << endl;

        break;
    case DSDMsg::CBC_FILE_EXIST_RSP:
        const DSDMsg::CBCFileExistRspMsg* cbcExistRsp = reinterpret_cast<DSDMsg::CBCFileExistRspMsg*>(msg->addr());
        cout << *cbcExistRsp << endl;

        break;
        break;
    default:
        break;
    
    }
}

int main(int argc, char *argv)
{

    // Simulate the message from the wire
    unsigned int buff[DSDMsg::MSG_SIZE_IN_BYTES/sizeof(unsigned int)];    
    buff[0] = DSDMsg::SET_CBC_RELOAD_FILE;
    buff[1] = DSDMsg::VERSION;
    buff[2] = 5;
 
    // 1. This is the message from the wire   
    DSDMsg *msg = reinterpret_cast<DSDMsg*>(buff);
    
    // 2. Check what the message is
    print(msg);
    
    // 3. Construct the response message
    msg->reset();  
    DSDMsg::SetReloadFileRspMsg* rsp = new(msg->addr()) DSDMsg::SetReloadFileRspMsg(DSDMsg::OK);
    print(msg);
        
    cout << "\nEcho Message testing" << endl;
    msg->reset();
    string str("Bravo");
    unsigned char len = str.length();
    const char* s = str.c_str();
    DSDMsg::EchoTestMsg* echoMsg = new(msg->addr()) DSDMsg::EchoTestMsg(len, const_cast<char *>(s));
    print(msg);  
 
    int len1 = echoMsg->nofCharacters(); 
    char chb[1024];
    strcpy(chb, echoMsg->character());
     
    msg->reset();
    DSDMsg::EchoTestRspMsg* echoRsp = new(msg->addr()) DSDMsg::EchoTestRspMsg(DSDMsg::OK, len1, chb);
    print(msg);
              
    cout << "CBCFileExistMsg Testing" << endl;
    
    msg->reset();
    DSDMsg::CBCFileExistMsg* cbcExist = new(msg->addr()) DSDMsg::CBCFileExistMsg(3);
    print(msg);
    
    msg->reset();
    DSDMsg::CBCFileExistRspMsg* cbcExistRsp = new(msg->addr()) DSDMsg::CBCFileExistRspMsg(3, 0, 1);
    print(msg);
    return 0;
}
*/

