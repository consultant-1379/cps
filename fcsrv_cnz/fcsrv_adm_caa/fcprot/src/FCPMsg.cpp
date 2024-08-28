/*
  NAME
  File_name:FCPMsg.cpp

  COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

  The Copyright to the computer program(s) herein is the property of Ericsson
  Utvecklings AB, Sweden.
  The program(s) may be used and/or copied only with the written permission from
  Ericsson Utvecklings AB or in accordance with the terms and conditions
  stipulated in the agreement/contract under which the program(s) have been
  supplied.

  DESCRIPTION
  Implements all the FCP Messages.

  DOCUMENT NO
  190 89-CAA 109 0670

  AUTHOR
  2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on FCPMsg.cpp of FCSRV in Windows

  SEE ALSO
  -

  Revision history
  ----------------


*/

#include "FCPMsg.h"

#include <iomanip>
//
// Static Inits
//====================================================================
const FCPMsg::PRIMITIVE FCPMsg::FcWriteGetPathMsg::s_primitive = FCPMsg::FC_WRITE_GET_PATH;
const FCPMsg::PRIMITIVE FCPMsg::FcWriteGetPathRspMsg::s_primitive = FCPMsg::FC_WRITE_GET_PATH_RSP;
const FCPMsg::PRIMITIVE FCPMsg::FcReadGetPathMsg::s_primitive = FCPMsg::FC_READ_GET_PATH;
const FCPMsg::PRIMITIVE FCPMsg::FcReadGetPathRspMsg::s_primitive = FCPMsg::FC_READ_GET_PATH_RSP;
const FCPMsg::PRIMITIVE FCPMsg::FcRemoveMsg::s_primitive = FCPMsg::FC_REMOVE;
const FCPMsg::PRIMITIVE FCPMsg::FcRemoveRspMsg::s_primitive = FCPMsg::FC_REMOVE_RSP;
const FCPMsg::PRIMITIVE FCPMsg::FcRemoveAllMsg::s_primitive = FCPMsg::FC_REMOVE_ALL;
const FCPMsg::PRIMITIVE FCPMsg::FcRemoveAllRspMsg::s_primitive = FCPMsg::FC_REMOVE_ALL_RSP;
const FCPMsg::PRIMITIVE FCPMsg::FcSizeMsg::s_primitive = FCPMsg::FC_SIZE;
const FCPMsg::PRIMITIVE FCPMsg::FcSizeRspMsg::s_primitive = FCPMsg::FC_SIZE_RSP;
//
//
//
const char* FCPMsg::ERROR_CODE_STRING[NUMBER_OF_ERROR_CODES] = {
	 "0 OK" ,
	 "1 UNSUP_FCPROT_VERSION" ,
	 "2 INTERNAL_ERROR" ,
	 "3 BAD_MSG" ,
	 "4 UNKNOWN_PRIMITIVE" ,
	"","","","","","","","","","",
	"","","","","","","","","","",
	"","","","","","","","","","",
	"","","","","","","","","","",
	"","","","","",
	 "50 FILE_DO_NOT_EXIST" ,
	 "51 CANNOT_DELETE_FILE" 
};
//
//
//============================================================================
ostream& operator<<(ostream& os, const FCPMsg::MsgHeader& msg) {
	os << dec << "<" << msg.primitive() << "><" << msg.transNum() << ">";
	return os;
}
//
//
//============================================================================
ostream& operator<<(ostream& os, const FCPMsg::ReqHeader& msg) {
	os << static_cast<const FCPMsg::MsgHeader&>(msg) << msg.version() << ">\n"; // TODO: check ??
	return os;
}
//
//
//============================================================================
ostream& operator<<(ostream& os, const FCPMsg::RspHeader& msg) {
	os << static_cast<const FCPMsg::MsgHeader&>(msg) << "<" << msg.errorCode() << ">\n";
	return os;
}
//
//
//============================================================================
ostream& operator<<(ostream& os, const FCPMsg::FcWriteGetPathMsg& msg) {
	os << static_cast<const FCPMsg::ReqHeader&>(msg)
       << msg.fileName() 
       << "\n"
       << msg.cpId();
	return os;
}
//
//
//============================================================================
ostream& operator<<(ostream& os, const FCPMsg::FcWriteGetPathRspMsg& msg) {
	os << static_cast<const FCPMsg::RspHeader&>(msg)
       << msg.pathName()
       << "\n"
       << msg.fileFlag();
	return os;
}
//
//
//============================================================================
ostream& operator<<(ostream& os, const FCPMsg::FcReadGetPathMsg& msg) {
	os << static_cast<const FCPMsg::ReqHeader&>(msg)
       << msg.fileName()
       << "\n"
       << msg.cpId();
	return os;
}
//
//
//============================================================================
ostream& operator<<(ostream& os, const FCPMsg::FcReadGetPathRspMsg& msg) {
	os << static_cast<const FCPMsg::RspHeader&>(msg)
       << msg.pathName()
       << "\n";
	return os;
}
//
//
//============================================================================
ostream& operator<<(ostream& os, const FCPMsg::FcRemoveMsg& msg) {
	os << static_cast<const FCPMsg::ReqHeader&>(msg)
       << msg.fileName()
       << "\n"
       << msg.cpId();
	return os;
}
//
//
//============================================================================
ostream& operator<<(ostream& os, const FCPMsg::FcRemoveRspMsg& msg) {
	os << static_cast<const FCPMsg::RspHeader&>(msg);
	return os;
}
//
//
//============================================================================
ostream& operator<<(ostream& os, const FCPMsg::FcRemoveAllMsg& msg) {
	os << static_cast<const FCPMsg::ReqHeader&>(msg) 
       << msg.cpId();
	return os;
}
//
//
//============================================================================
ostream& operator<<(ostream& os, const FCPMsg::FcRemoveAllRspMsg& msg) {
	os << static_cast<const FCPMsg::RspHeader&>(msg);
	return os;
}
//
//
//============================================================================
ostream& operator<<(ostream& os, const FCPMsg::FcSizeMsg& msg) {
	os << static_cast<const FCPMsg::ReqHeader&>(msg)
       << msg.fileName() 
       << "\n"
       << msg.cpId();
	return os;
}
//
//
//============================================================================
ostream& operator<<(ostream& os, const FCPMsg::FcSizeRspMsg& msg) {
	os << static_cast<const FCPMsg::RspHeader&>(msg)
       << msg.fileSizeHigh()
       << "\n"
       << msg.fileSizeLow();
	return os;
}

