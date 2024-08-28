#ifndef _FCPMsg_H_
#define _FCPMsg_H_
#include <iostream>
	using namespace std;
/*
  NAME
  FCPMsg -
  LIBRARY 3C++
  PAGENAME FCPMsg
  HEADER  CPS
  LEFT_FOOTER Ericsson Utvecklings AB
  INCLUDE FCProtocolMsg.h

  COPYRIGHT
  COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.
	
  The Copyright to the computer program(s) herein is the property of
  Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
  copied only with the written permission from Ericsson Utvecklings AB
  or in accordance with the terms and conditions stipulated in the
  agreement/contract under which the program(s) have been supplied.

  DESCRIPTION
  Implements all the FCP Messages.

  Note: This file follows the AP coding standards and practice, but is
  also compiled into the APZ-VM binary.

  The main differences between CP and AP coding conventions are
  1) constants, enums etc. are all upper-case w. underscores, as in
  "MAX_PATH", not MaxPath
  2) Typenames and templates are allowed. (We have only light use of
  these facilities in the code.)
  3) The FCSRV program, specifically, has a preference rule to use the
  Win32 API rather than the standard C-library.

  ERROR HANDLING
  -

  DOCUMENT NO
  190 89-CAA 109 0670

  AUTHOR
  2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on FCPMsg.h of FCSRV's Windows version

  LINKAGE
  -

  SEE ALSO
  -
  
  evision history
  ----------------

*/

#include "PlatformTypes.h"

#include "Version.h"
#include "FtpFileSet.h"
#include "FileName.h"

#include <assert.h>
#include <ostream>
#include <string.h>

using namespace std;
//
//
//===================================================================
class FCPMsg {
// types
public:
	enum { VERSION_MAJOR = 1, VERSION_MINOR = 0 };
	enum { MSG_SIZE_IN_BYTES = 1024 };// must be even 64-bit
	
	enum PRIMITIVE {
		FIRST_PRIMITIVE = 100,
		FC_WRITE_GET_PATH = FIRST_PRIMITIVE,
		FC_WRITE_GET_PATH_RSP,
		FC_READ_GET_PATH,
		FC_READ_GET_PATH_RSP,
		FC_REMOVE,
		FC_REMOVE_RSP,
		FC_REMOVE_ALL,
		FC_REMOVE_ALL_RSP,
		FC_SIZE,
		FC_SIZE_RSP,
		LAST_PRIMITIVE = FC_SIZE_RSP
	};
	enum { NUMBER_OF_PRIMITIVES = LAST_PRIMITIVE - FIRST_PRIMITIVE + 1 };

	enum ERROR_CODE {
		FIRST_ERROR_CODE = 0,
		OK = FIRST_ERROR_CODE,
		UNSUP_FCPROT_VERSION = 1, // UNSUP == Unsupported
		INTERNAL_ERROR,
		BAD_MSG,
		UNKNOWN_PRIMITIVE,
		FILE_DO_NOT_EXIST = 50,
		CANNOT_DELETE_FILE = 51,
		LAST_ERROR_CODE = CANNOT_DELETE_FILE
	};
	enum { NUMBER_OF_ERROR_CODES = LAST_ERROR_CODE - FIRST_ERROR_CODE + 1 };
	static const char* ERROR_CODE_STRING[NUMBER_OF_ERROR_CODES];

	//
	//
	//---------------------------------------------------------------
	enum CP_SYSTEM_STATE {
		ONE_CP_SYSTEM,
		MULTI_CP_SYSTEM
	};
	//
	// MsgHeader - common header for all messages
	//================================================================


	class MsgHeader {
	protected:
		MsgHeader(PRIMITIVE prim, u_int32 transNum) : m_primitive(prim), m_transNum(transNum) { }
	public:
		// access
		PRIMITIVE primitive() const { return m_primitive; }
		u_int32 transNum()  const { return m_transNum; }
		// modify
		void primitive(PRIMITIVE p) { m_primitive = p; }
		void transNum(u_int32 transNum) { m_transNum = transNum; }
		friend ostream& operator<<(ostream& , const MsgHeader& );
	private:
		PRIMITIVE m_primitive;
		u_int32 m_transNum;
	};
	//
	//
	// ReqHeader - common header for all request messages
	//================================================================
	class ReqHeader : public MsgHeader {
	public:
		ReqHeader(PRIMITIVE prim, u_int32 transNum)
			: MsgHeader(prim, transNum),
              m_version(FCPMsg::VERSION_MAJOR, FCPMsg::VERSION_MINOR) { }
		// access
		Version version() const { return m_version; }
		friend ostream& operator<<(ostream& , const ReqHeader& );

	private:
		Version m_version;
	};
	//
	// RspHeader - common header for all response messages
	//================================================================
	class RspHeader : public MsgHeader {
	public:
		RspHeader(PRIMITIVE prim, u_int32 transNum, u_int32 ecode)
			: MsgHeader(prim, transNum), m_errorCode(ecode) { }
		// access
		u_int32 errorCode() const { return m_errorCode; }
		// modify
		void errorCode(u_int32 ecode) { m_errorCode = ecode; }
		friend ostream& operator<<(ostream& , const RspHeader& );
	private:
		u_int32 m_errorCode;
	};
	//
	//
	//================================================================
	class FcWriteGetPathMsg : public ReqHeader {
        // types
	public:
		static const PRIMITIVE s_primitive;
	public:
		FcWriteGetPathMsg(u_int32 transNum, const FileName& fileName, u_int32 cpSys, u_int32 cpId)
            : ReqHeader(s_primitive, transNum), m_fileName(fileName), m_cpSystem(cpSys), m_cpId(cpId) { }
		// access
		const char* fileName() const { return m_fileName.filename(); }
		u_int32 cpSystem() const { return m_cpSystem; }
		u_int32 cpId() const { return m_cpId; }
		// modify
		void fileName(const FileName& fileName) { m_fileName = fileName; }
		void cpSystem(u_int32 cpSys)  { m_cpSystem = cpSys; }
		void cpId(u_int32 cpId)  { m_cpId = cpId; }
		// funx
		friend ostream& operator<<(ostream& , const FcWriteGetPathMsg& );
	private:
		FileName m_fileName;
		u_int32 m_cpSystem; //One CP System or Multiple CP System
		u_int32 m_cpId;
	};
	//
	//
	//================================================================
	class FcWriteGetPathRspMsg : public RspHeader {
        // types
	public:
		static const PRIMITIVE s_primitive;
	public:
		FcWriteGetPathRspMsg(u_int32 transNum, u_int32 ecode,
                             const FtpFileSet& pathName, u_int32 fileFlag)
            : RspHeader(s_primitive, transNum, ecode){
			if(ecode == OK) {
				m_pathName = pathName;
				m_fileFlag = fileFlag;
			}
		}
		// access
		const FtpFileSet& pathName() const { return m_pathName; }
		u_int32 fileFlag() const { return m_fileFlag; }
		// funx
		friend ostream& operator<<(ostream& , const FcWriteGetPathRspMsg& );
	private:
		FtpFileSet m_pathName;
		u_int32 m_fileFlag;
	};
	//
	//
	//================================================================
	class FcReadGetPathMsg : public ReqHeader {
        // types
	public:
		static const PRIMITIVE s_primitive;
	public:
		FcReadGetPathMsg(u_int32 transNum, const FileName& fileName, u_int32 cpSys, u_int32 cpId)
            : ReqHeader(s_primitive, transNum), m_fileName(fileName), m_cpSystem(cpSys), m_cpId(cpId) { }
		// access
		const char* fileName() const { return m_fileName.filename(); }
		u_int32 cpSystem() const { return m_cpSystem; }
		u_int32 cpId() const { return m_cpId; }
		// modify
		void fileName(const FileName& fileName) { m_fileName = fileName; }
		void cpSystem(u_int32 cpSys)  { m_cpSystem = cpSys; }
		void cpId(u_int32 cpId)  { m_cpId = cpId; }
		// funx
		friend ostream& operator<<(ostream& , const FcReadGetPathMsg& );
	private:
		FileName m_fileName;
		u_int32 m_cpSystem; //One CP System or Multiple CP System
		u_int32 m_cpId;
	};
	//
	//
	//================================================================
	class FcReadGetPathRspMsg : public RspHeader {
        // types
	public:
		static const PRIMITIVE s_primitive;
	public:
		FcReadGetPathRspMsg(u_int32 transNum, u_int32 ecode,
                            const FtpFileSet& pathName)
            : RspHeader(s_primitive, transNum, ecode){
			if(ecode == OK) {
				m_pathName = pathName;
			}
		}
		// access
		const FtpFileSet& pathName() const { return m_pathName; }
		// funx
		friend ostream& operator<<(ostream& , const FcReadGetPathRspMsg& );
	private:
		FtpFileSet m_pathName;
		u_int32 m_fileFlag;
	};
	//
	//
	//================================================================
	class FcRemoveMsg : public ReqHeader {
        // types
	public:
		static const PRIMITIVE s_primitive;
	public:
		FcRemoveMsg(u_int32 transNum, FileName& fileName, u_int32 cpSys, u_int32 cpId)
            : ReqHeader(s_primitive, transNum), m_fileName(fileName), m_cpSystem(cpSys), m_cpId(cpId) { }
		// access
		const char* fileName() const { return m_fileName.filename(); }
		u_int32 cpSystem() const { return m_cpSystem; }
		u_int32 cpId() const { return m_cpId; }
		// modify
		void fileName(const FileName& fileName) { m_fileName = fileName; }
		void cpSystem(u_int32 cpSys)  { m_cpSystem = cpSys; }
		void cpId(u_int32 cpId)  { m_cpId = cpId; }
		// funx
		friend ostream& operator<<(ostream& , const FcRemoveMsg& );
	private:
		FileName m_fileName;
		u_int32 m_cpSystem; //One CP System or Multiple CP System
		u_int32 m_cpId;
	};
	//
	//
	//================================================================
	class FcRemoveRspMsg : public RspHeader {
        // types
	public:
		static const PRIMITIVE s_primitive;
	public:
		FcRemoveRspMsg(u_int32 transNum, u_int32 ecode)
            : RspHeader(s_primitive, transNum, ecode){
			if(ecode == OK) {
				// do something
			}
		}
		// access
		friend ostream& operator<<(ostream& , const FcRemoveRspMsg& );
	private:
	};
	//
	//
	//================================================================
	class FcRemoveAllMsg : public ReqHeader {
        // types
	public:
		static const PRIMITIVE s_primitive;
	public:
		FcRemoveAllMsg(u_int32 transNum, u_int32 cpSys, u_int32 cpId)
            : ReqHeader(s_primitive, transNum), m_cpSystem(cpSys), m_cpId(cpId) { }
		// access
		u_int32 cpSystem() const { return m_cpSystem; }
		u_int32 cpId() const { return m_cpId; }
		// modify
		void cpSystem(u_int32 cpSys)  { m_cpSystem = cpSys; }
		void cpId(u_int32 cpId)  { m_cpId = cpId; }
		// funx
		friend ostream& operator<<(ostream& , const FcRemoveAllMsg& );
	private:
    u_int32 m_cpSystem; //One CP System or Multiple CP System
		u_int32 m_cpId;
	};
	//
	//
	//================================================================
	class FcRemoveAllRspMsg : public RspHeader {
        // types
	public:
		static const PRIMITIVE s_primitive;
	public:
		FcRemoveAllRspMsg(u_int32 transNum, u_int32 ecode)
            : RspHeader(s_primitive, transNum, ecode){
			if(ecode == OK) {
				// do something
			}
		}
		// access
		friend ostream& operator<<(ostream& , const FcRemoveAllRspMsg& );
	private:
	};
	//
	//================================================================
	class FcSizeMsg : public ReqHeader {
        // types
	public:
		static const PRIMITIVE s_primitive;
	public:
		FcSizeMsg(u_int32 transNum, const FileName& fileName, u_int32 cpSys, u_int32 cpId)
            : ReqHeader(s_primitive, transNum), m_fileName(fileName), m_cpSystem(cpSys), m_cpId(cpId) { }
		// access
		const char* fileName() const { return m_fileName.filename(); }
		u_int32 cpSystem() const { return m_cpSystem; }
		u_int32 cpId() const { return m_cpId; }
		// modify
		void fileName(const FileName& fileName) { m_fileName = fileName; }
		void cpSystem(u_int32 cpSys)  { m_cpSystem = cpSys; }
		void cpId(u_int32 cpId)  { m_cpId = cpId; }
		// funx
		friend ostream& operator<<(ostream& , const FcSizeMsg& );
	private:
		FileName m_fileName;
		u_int32 m_cpSystem; //One CP System or Multiple CP System
		u_int32 m_cpId;
	};
	//
	//
	//================================================================
	class FcSizeRspMsg : public RspHeader {
        // types
	public:
		static const PRIMITIVE s_primitive;
	public:
		FcSizeRspMsg(u_int32 transNum, u_int32 ecode,
                     u_int32 fileSizeHigh, u_int32 fileSizeLow)
            : RspHeader(s_primitive, transNum, ecode){
			if(ecode == OK) {
				m_fileSizeHigh = fileSizeHigh;
				m_fileSizeLow = fileSizeLow;
			}
		}
		// access
		u_int32 fileSizeHigh() const { return m_fileSizeHigh; }
		u_int32 fileSizeLow() const { return m_fileSizeLow; }
		// funx
		friend ostream& operator<<(ostream& , const FcSizeRspMsg& );
	private:
		u_int32 m_fileSizeHigh;
		u_int32 m_fileSizeLow;
	};
//
// funx
//================================================================
public:
	FCPMsg() { reset(); }
	// access
	char* addr() { return reinterpret_cast<char*>(m_buffer); }
	const char* addr() const { return reinterpret_cast<const char*>(m_buffer); }
	u_int32 size() { return sizeof(m_buffer); }
	// msgs
	MsgHeader* msgHeader() { return reinterpret_cast<MsgHeader*>(addr()); }
	const MsgHeader* msgHeader() const { return reinterpret_cast<const MsgHeader*>(addr()); }
	ReqHeader* requestHeader() { return reinterpret_cast<ReqHeader*>(addr()); }
	const ReqHeader* requestHeader() const { return reinterpret_cast<const ReqHeader*>(addr()); }
	RspHeader* responseHeader() { return reinterpret_cast<RspHeader*>(addr()); }
	const RspHeader* responseHeader() const { return reinterpret_cast<const RspHeader*>(addr()); }
	// modify
	void reset() {
		memset(&m_buffer, 0, MSG_SIZE_IN_BYTES);
	}
	friend ostream& operator<<(ostream& , const FCPMsg& );
// attrib
private:
	u_int64 m_buffer[MSG_SIZE_IN_BYTES / sizeof(u_int64)];
};
//
//
//===================================================================
template<class T>
T* fcp_ptr_cast(FCPMsg& msg) {
	assert(msg.msgHeader()->primitive() == T::s_primitive);
	return msg.msgHeader()->primitive() == T::s_primitive ? reinterpret_cast<T*>(msg.addr()) : NULL;
}

template<class T>
const T* fcp_ptr_cast(const FCPMsg& msg) {
	assert(msg.msgHeader()->primitive() == T::s_primitive);
	return msg.msgHeader()->primitive() == T::s_primitive ? reinterpret_cast<const T*>(msg.addr()) : NULL;
}
#endif
