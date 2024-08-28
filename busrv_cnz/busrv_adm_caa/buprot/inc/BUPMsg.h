#ifndef _BUPMSG_H_
#define _BUPMSG_H_
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
	Implements all the BUP Messages.

	Note: This file follows the AP coding standards and practice, but is
	also compiled into the APZ-VM binary.

  The main differences between CP and AP coding conventions are
  1) constants, enums etc. are all upper-case w. underscores, as in
     "MAX_PATH", not MaxPath
  2) Typenames and templates are allowed. (We have only light use of
     these facilities in the code.)
  3) The BUSRV program, specifically, has a preference rule to use the
     Win32 API rather than the standard C-library.

ERROR HANDLING
	-

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
	2002-02-01 by UAB/KY/SK Anders Gillgren (qabgill, tag: ag)

Revision history
----------------
2002-02-01 qabgill Created
2006-08-09 uablan  Add linkSpeed to LoadBeginRspMsg,FallbackBeginRspMsg and DumpBeginRspMsg.
2007-01-09 uablan  Add CP id to ReqHeader.
2010-05-23 xdtthng Modified for SSI


LINKAGE
	-

SEE ALSO
	-

*/
#pragma once

#include "PlatformTypes.h"

#include "APZVersion.h"
#include "BUPDefs.h"
#include "ExchangeId.h"
#include "FtpFileSet.h"
#include "Key.h"
#include "SBCId.h"
#include "SectorInfo.h"
#include "Version.h"
#include "ALogData.h"

#include <assert.h>
#include <ostream>
#include <string.h>

using namespace std;
//
//
//===================================================================
class BUPMsg {
// types
public:
	enum { VER_MAJOR = 3, VER_MINOR = 0 };
	enum { VER_MAJOR2 = 4, VER_MINOR2 = 0 };
	enum { MSG_SIZE_IN_BYTES = 1024 };// must be even 64-bit

	enum PRIMITIVE {
		FIRST_PRIMITIVE = 0,
		LOAD_BEGIN = FIRST_PRIMITIVE,
		LOAD_BEGIN_RSP,
		LOAD_END,
		LOAD_END_RSP,
		FALLBACK_BEGIN,
		FALLBACK_BEGIN_RSP,
		FALLBACK_END,
		FALLBACK_END_RSP,
		DUMP_BEGIN,
		DUMP_BEGIN_RSP,
		DUMP_END,
		DUMP_END_RSP,
		READ_CONF,
		READ_CONF_RSP,
		READ_CMDLOG_CONF,
		READ_CMDLOG_CONF_RSP,
		READ_CMDLOG_DATA,
		READ_CMDLOG_DATA_RSP,
		WRITE_CMDLOG_DATA,
		WRITE_CMDLOG_DATA_RSP,
		SBC_SWITCH,
		SBC_SWITCH_RSP,
		SBC1_TO_SFR,      // dummy - to be removed when VM code is removed
		SBC1_TO_SFR_RSP,  // dummy - to be removed when VM code is removed
		LIST_SBCS,
		LIST_SBCS_RSP,
		VERIFY_SBC_DATA,
		VERIFY_SBC_DATA_RSP,
		SBC_INFO,
		SBC_INFO_RSP,
		WRITE_ALOG_DATA,
		WRITE_ALOG_DATA_RSP,
		LAST_PRIMITIVE = WRITE_ALOG_DATA_RSP,
	};
	enum { NUMBER_OF_PRIMITIVES = LAST_PRIMITIVE - FIRST_PRIMITIVE + 1 };

	enum ERROR_CODE {
		FIRST_ERROR_CODE = 0,
		OK = FIRST_ERROR_CODE,
		UNSUP_BUPROT_VERSION = 1, // UNSUP == Unsupported
		INTERNAL_ERROR,
		BAD_MSG,
		UNKNOWN_PRIMITIVE,
		INVALID_KEY,
		CREATE_DATAAREA_FAILED,
		REQ_DATAAREA_MISSING,
		BAD_BACKUPINFO,
		BAD_DATAAREA,
		WRITE_DATAAREA_ERROR,
		SBC_ROOT_NOT_FOUND,
		SBC_NOT_FOUND,
		SBC_RESERVE_FAILED,
		PARAM_BACKUPID_OUT_OF_RANGE,
		PARAM_SBC_RANGE_INVALID,
		PARAM_EXTENT_INVALID,
		PARAM_SWITCH_OPERATION_INVALID,
		PARAM_ONLY_SBC0_FOR_PARTIAL_DUMP,
		SWITCH_INSUFFICIENT_NOF_SBCS,
		SWITCH_SBC_DATES_ILLEGAL_FOR_OP,
		SWITCH_LOW_SBC_NOT_FOUND,
		SWITCH_HIGH_SBC_NOT_FOUND,
		SWITCH_LOW_BACKUPINFO_BAD,
		SWITCH_HIGH_BACKUPINFO_BAD,
		SWITCH_LOW_ID_NOT_BASE, // low SBC must be 0 or 100
		SWITCH_TEMP_SBC_EXIST,
		SHUTDOWN_PENDING,
		FMS_PHYSICAL_FILE_ERROR,
		MAX_NO_DUMP_LOAD_ONGOING,
		NOT_SUP_IN_CLUSTER,
		DUMP_REJECTED_DUE_TO_ONGOING_DUMP_ON_SAME_SBCID,
		DUMP_REJECTED_DUE_TO_ONGOING_LOAD,
		LOAD_REJECTED_DUE_TO_ONGOING_DUMP,
		SWITCH_REJECTED_DUE_TO_ONGOING_LOAD,
                AUTODUMP_FAILED_BUINFO_CORRECTED,
                AUTODUMP_FAILED_BUINFO_NOT_CORRECTED,
		LAST_ERROR_CODE = AUTODUMP_FAILED_BUINFO_NOT_CORRECTED
	};
	enum { NUMBER_OF_ERROR_CODES = LAST_ERROR_CODE - FIRST_ERROR_CODE + 1 };
	static const char* ERROR_CODE_STRING[NUMBER_OF_ERROR_CODES];
	//
	//
	//---------------------------------------------------------------
	enum SWITCH_OPERATION {
		HIGH_LOW, ROLL_DOWN
	};
	//
	//
	//---------------------------------------------------------------
	enum SBC_DATA_STATUS {
		SDS_NO_ERROR,
		SDS_UNKNOWN_ERROR,
		SDS_BAD_DATA_SIZE,
		SDS_BAD_CHECKSUM,
		SDS_DATAAREA_MISSING
	};
	//
	//
	//---------------------------------------------------------------
	enum CP_SYSTEM_STATE {
		ONE_CP_SYSTEM,
		MULTI_CP_SYSTEM
	};
	enum RESULT {
		LOAD_OK = 0,
		DUMP_OK = 0,
		NOK = 1
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
	// ReqHeader - common header for all request messages
	//================================================================
	class ReqHeader : public MsgHeader {
	public:
		ReqHeader(PRIMITIVE prim, u_int32 transNum, u_int16 min = VER_MAJOR, u_int16 maj = VER_MINOR)
			: MsgHeader(prim, transNum),
			// This is used by the test program, by default is version 3.0
			m_version(min, maj) { }
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
	class LoadBeginMsg : public ReqHeader {
	// types
	public:
		static const PRIMITIVE s_primitive;
	public:
		LoadBeginMsg(u_int32 transNum, SBCId sbcId, u_int32 cpSys, u_int32 cpId,
		                  u_int16 maj = VER_MAJOR, u_int16 min = VER_MINOR)
		: ReqHeader(s_primitive, transNum, maj, min), m_sbcId(sbcId), m_cpSystem(cpSys), m_cpId(cpId) { }
		// access
		SBCId sbcId() const { return m_sbcId; }
		u_int32 cpSystem() const { return m_cpSystem; }
		u_int32 cpId() const { return m_cpId; }
		// modify
		void sbcId(SBCId sbcId) { m_sbcId = sbcId; }
		void cpSystem(u_int32 cpSys)  { m_cpSystem = cpSys; }
		void cpId(u_int32 cpId)  { m_cpId = cpId; }
		// funx
		ostream& writeParams(ostream& os) const { return os  << m_sbcId << '\n'; }
		friend ostream& operator<<(ostream& , const LoadBeginMsg& );
	private:
		SBCId m_sbcId;
		u_int32 m_cpSystem; //One CP System or Multiple CP System
		u_int32 m_cpId;
	};
	//
	//
	//================================================================
	class LoadBeginRspMsg : public RspHeader {
	// types
	public:
		static const PRIMITIVE s_primitive;
	public:
		LoadBeginRspMsg(u_int32 transNum, u_int32 ecode, const Key& key,
			const FtpFileSet& files, u_int32 linkSpeed)
		: RspHeader(s_primitive, transNum, ecode), m_key(false), m_linkSpeed(0u) {
			if(ecode == OK) {
				//assert(key != 0);
				m_key = key;
				m_files = files;
				m_linkSpeed = linkSpeed;
			}
		}
		// access
		const Key& key() const { return m_key; }
		const FtpFileSet& files() const { return m_files; }
		u_int32 linkSpeed() const { return m_linkSpeed; }
		friend ostream& operator<<(ostream& , const LoadBeginRspMsg& );
	private:
		Key m_key;
		FtpFileSet m_files;
		u_int32 m_linkSpeed;
	};
	
	//
	//
	//================================================================
	class LoadBeginRspMsg_V4 : public RspHeader {
	// types
	public:
		static const PRIMITIVE s_primitive;
	public:
		LoadBeginRspMsg_V4(u_int32 transNum, u_int32 ecode, const Key& key,
			const FtpFileSet_V4& files, u_int32 linkSpeed)
		: RspHeader(s_primitive, transNum, ecode), m_key(false) {
			if(ecode == OK) {
				//assert(key != 0);
				m_key = key;
				m_files = files;
				m_linkSpeed = linkSpeed;
			}
			else {
				m_linkSpeed = 0u;
			}
		}
		// access
		const Key& key() const { return m_key; }
		const FtpFileSet_V4& files() const { return m_files; }
		u_int32 linkSpeed() const { return m_linkSpeed; }
		friend ostream& operator<<(ostream& , const LoadBeginRspMsg_V4& );
	private:
		Key m_key;
		FtpFileSet_V4 m_files;
		u_int32 m_linkSpeed;
	};

	//
	//
	//================================================================
	class LoadEndMsg : public ReqHeader {
	// types
	public:
		static const PRIMITIVE s_primitive;
	public:
		LoadEndMsg(u_int32 transNum, const Key& key, u_int32 cpSys, u_int32 cpId, u_int32 loadResult,
		              u_int16 maj = VER_MAJOR, u_int16 min = VER_MINOR)
		: ReqHeader(s_primitive, transNum, maj, min), m_key(false), m_cpSystem(cpSys) , m_cpId(cpId), m_loadResult(loadResult) {
			m_key = key;
		}
		// access
		const Key& key() const { return m_key; }
		u_int32 cpSystem() const { return m_cpSystem; }
		u_int32 cpId() const { return m_cpId; }
		u_int32 loadResult() const { return m_loadResult; }
		// modify
		void key(const Key& key) { m_key = key; }
		void cpSystem(u_int32 cpSys)  { m_cpSystem = cpSys; }
		void cpId(u_int32 cpId)  { m_cpId = cpId; }
		void loadResult(u_int32 loadRes) { m_loadResult = loadRes;}
		friend ostream& operator<<(ostream& , const LoadEndMsg& );
	private:
		Key m_key;
		u_int32 m_cpSystem; //One CP System or Multiple CP System
		u_int32 m_cpId;
		u_int32 m_loadResult;
	};
	/* This is fake key
	//
	//
	//================================================================
	class LoadEndMsg_V4 : public ReqHeader {
	// types
	public:
		static const PRIMITIVE s_primitive;
	public:
		LoadEndMsg_V4(u_int32 transNum, const Key_V4& key, u_int32 cpSys, u_int32 cpId, u_int32 loadResult)
		: ReqHeader(s_primitive, transNum), m_key(false), m_cpSystem(cpSys) , m_cpId(cpId), m_loadResult(loadResult) {
			m_key = key;
		}
		// access
		const Key_V4& key() const { return m_key; }
		u_int32 cpSystem() const { return m_cpSystem; }
		u_int32 cpId() const { return m_cpId; }
		u_int32 loadResult() const { return m_loadResult; }
		// modify
		void key(const Key_V4& key) { m_key = key; }
		void cpSystem(u_int32 cpSys)  { m_cpSystem = cpSys; }
		void cpId(u_int32 cpId)  { m_cpId = cpId; }
		void loadResult(u_int32 loadRes) { m_loadResult = loadRes;}
		friend ostream& operator<<(ostream& , const LoadEndMsg_V4& );
	private:
		Key_V4 m_key;
		u_int32 m_cpSystem; //One CP System or Multiple CP System
		u_int32 m_cpId;
		u_int32 m_loadResult;
	};
	*/
	//
	//
	//================================================================
	class LoadEndRspMsg : public RspHeader {
	// types
	public:
		static const PRIMITIVE s_primitive;
	public:
		LoadEndRspMsg(u_int32 transNum, u_int32 ecode)
		: RspHeader(s_primitive, transNum, ecode) { }
		friend ostream& operator<<(ostream& , const LoadEndRspMsg& );
	private:
	};
	//
	//
	//================================================================
	class FallbackBeginMsg : public ReqHeader {
	// types
	public:
		static const PRIMITIVE s_primitive;
	public:
		FallbackBeginMsg(u_int32 transNum, u_int32 cpSys, u_int32 cpId, 
		                  u_int16 maj = VER_MAJOR, u_int16 min = VER_MINOR)
		: ReqHeader(s_primitive, transNum, maj, min), m_cpSystem(cpSys), m_cpId(cpId), m_sbcId() { }
		// access
		SBCId sbcId() const { return m_sbcId; }
		u_int32 cpSystem() const { return m_cpSystem; }
		u_int32 cpId() const { return m_cpId; }
		// modify
		void sbcId(SBCId id) { m_sbcId = id; }
		void cpSystem(u_int32 cpSys)  { m_cpSystem = cpSys; }
		void cpId(u_int32 cpId)  { m_cpId = cpId; }
		ostream& writeParams(ostream& os) const { return os  << m_sbcId << '\n'; }
		friend ostream& operator<<(ostream& , const FallbackBeginMsg& );
	private:
		u_int32 m_cpSystem; //One CP System or Multiple CP System
		u_int32 m_cpId;
		SBCId m_sbcId;
	};	
	//
	//
	//================================================================
	class FallbackBeginRspMsg : public RspHeader {
	// types
	public:
		static const PRIMITIVE s_primitive;
	public:
		FallbackBeginRspMsg(u_int32 transNum, u_int32 ecode, const Key& key,
			SBCId sbcId, bool lastPossibleSBCLoaded, const FtpFileSet& files, u_int32 linkSpeed)
			: RspHeader(s_primitive, transNum, ecode), m_lastPossibleSBCLoaded(false), m_linkSpeed(0) {
			if(ecode == OK) {
				//assert(key != 0);
				m_key = key;
				m_sbcId = sbcId;
				m_lastPossibleSBCLoaded = lastPossibleSBCLoaded;
				m_files = files;
				m_linkSpeed = linkSpeed;
			}
		}
		// access
		const Key& key() const { return m_key; }
		SBCId sbcId() const { return m_sbcId; }
		bool lastPossibleSBCLoaded() const { return m_lastPossibleSBCLoaded; }
		const FtpFileSet& files() const { return m_files; }
		u_int32 linkSpeed() const { return m_linkSpeed; }
		friend ostream& operator<<(ostream& , const FallbackBeginRspMsg& );
	private:
		Key m_key;
		SBCId m_sbcId;
		bool m_lastPossibleSBCLoaded;
		FtpFileSet m_files;
		u_int32 m_linkSpeed;
	};
	//
	//
	//
	//================================================================
	class FallbackBeginRspMsg_V4 : public RspHeader {
	// types
	public:
		static const PRIMITIVE s_primitive;
	public:
		FallbackBeginRspMsg_V4(u_int32 transNum, u_int32 ecode, const Key& key,
			SBCId sbcId, bool lastPossibleSBCLoaded, const FtpFileSet_V4& files, u_int32 linkSpeed)
			: RspHeader(s_primitive, transNum, ecode), m_key(false) {
			if(ecode == OK) {
				//assert(key != 0);
				m_key = key;
				m_sbcId = sbcId;
				m_lastPossibleSBCLoaded = lastPossibleSBCLoaded;
				m_files = files;
				m_linkSpeed = linkSpeed;
			}
		}
		// access
		const Key& key() const { return m_key; }
		SBCId sbcId() const { return m_sbcId; }
		bool lastPossibleSBCLoaded() const { return m_lastPossibleSBCLoaded; }
		const FtpFileSet_V4& files() const { return m_files; }
		u_int32 linkSpeed() const { return m_linkSpeed; }
		friend ostream& operator<<(ostream& , const FallbackBeginRspMsg_V4& );
	private:
		Key m_key;
		SBCId m_sbcId;
		bool m_lastPossibleSBCLoaded;
		FtpFileSet_V4 m_files;
		u_int32 m_linkSpeed;
	};
	//
	//
	//================================================================
	class FallbackEndMsg : public ReqHeader {
	// types
	public:
		static const PRIMITIVE s_primitive;
	public:
		FallbackEndMsg(u_int32 transNum, const Key& key, u_int32 cpSys, u_int32 cpId, u_int32 loadResult,
		                  u_int16 maj = VER_MAJOR, u_int16 min = VER_MINOR)
		: ReqHeader(s_primitive, transNum, maj, min), m_key(false), m_cpSystem(cpSys), m_cpId(cpId), m_loadResult(loadResult) {
			m_key = key;
		}
		// access
		const Key& key() const { return m_key; }
		u_int32 cpSystem() const { return m_cpSystem; }
		u_int32 cpId() const { return m_cpId; }
		u_int32 loadResult() const { return m_loadResult; }
		// modify
		void key(const Key& key) { m_key = key; }
		void cpSystem(u_int32 cpSys)  { m_cpSystem = cpSys; }
		void cpId(u_int32 cpId)  { m_cpId = cpId; }
		void loadResult(u_int32 loadRes) { m_loadResult = loadRes;}
		friend ostream& operator<<(ostream& , const FallbackEndMsg& );
	private:
		Key m_key;
		u_int32 m_cpSystem; //One CP System or Multiple CP System
		u_int32 m_cpId;
		u_int32 m_loadResult;
	};
	/* Faked Key
	//
	//
	//================================================================
	class FallbackEndMsg_V4 : public ReqHeader {
	// types
	public:
		static const PRIMITIVE s_primitive;
	public:
		FallbackEndMsg_V4(u_int32 transNum, const Key_V4& key, u_int32 cpSys, u_int32 cpId, u_int32 loadResult)
		: ReqHeader(s_primitive, transNum), m_key(false), m_cpSystem(cpSys), m_cpId(cpId), m_loadResult(loadResult) {
			m_key = key;
		}
		// access
		const Key_V4& key() const { return m_key; }
		u_int32 cpSystem() const { return m_cpSystem; }
		u_int32 cpId() const { return m_cpId; }
		u_int32 loadResult() const { return m_loadResult; }
		// modify
		void key(const Key_V4& key) { m_key = key; }
		void cpSystem(u_int32 cpSys)  { m_cpSystem = cpSys; }
		void cpId(u_int32 cpId)  { m_cpId = cpId; }
		void loadResult(u_int32 loadRes) { m_loadResult = loadRes;}
		friend ostream& operator<<(ostream& , const FallbackEndMsg_V4& );
	private:
		Key_V4 m_key;
		u_int32 m_cpSystem; //One CP System or Multiple CP System
		u_int32 m_cpId;
		u_int32 m_loadResult;
	};
	*/
	//
	//
	//================================================================
	class FallbackEndRspMsg : public RspHeader {
	// types
	public:
		static const PRIMITIVE s_primitive;
	public:
		FallbackEndRspMsg(u_int32 transNum, u_int32 ecode)
		: RspHeader(s_primitive, transNum, ecode) { }
		friend ostream& operator<<(ostream& , const FallbackEndRspMsg& );
	private:
	};
	//
	//
	//================================================================
	class DumpBeginMsg : public ReqHeader {
	// types
	public:
		static const PRIMITIVE s_primitive;
	public:
		DumpBeginMsg(u_int32 transNum, BUPDefs::EXTENT extent, SBCId sbcId, u_int32 cpSys, u_int32 cpId,
		              u_int16 maj = VER_MAJOR, u_int16 min = VER_MINOR)
		: ReqHeader(s_primitive, transNum, maj, min), m_extent(extent),
			m_sbcId(sbcId), m_cpSystem(cpSys), m_cpId(cpId) { }
		// access
		BUPDefs::EXTENT extent() const { return m_extent; }
		SBCId sbcId() const { return m_sbcId; }
		u_int32 cpSystem() const { return m_cpSystem; }
		u_int32 cpId() const { return m_cpId; }
		// modify
		void extent(BUPDefs::EXTENT extent) { m_extent = extent; }
		void sbcId(SBCId val) { m_sbcId = val; }
		void cpId(u_int32 cpId)  { m_cpId = cpId; }
		// funx
		ostream& writeParams(ostream& os) const { return os << m_sbcId << '\n' << m_extent << '\n'; }
		friend ostream& operator<<(ostream& , const DumpBeginMsg& );
	private:
		BUPDefs::EXTENT m_extent;
		SBCId m_sbcId;
		u_int32 m_cpSystem; //One CP System or Multiple CP System
		u_int32 m_cpId;
	};
	//
	//
	//================================================================
	class DumpBeginRspMsg : public RspHeader {
	// types
	public:
		static const PRIMITIVE s_primitive;
	public:
		DumpBeginRspMsg(u_int32 transNum, u_int32 ecode, const Key& key,
			const FtpFileSet& files, u_int32 linkSpeed)
			: RspHeader(s_primitive, transNum, ecode), m_key(false) {
			if(ecode == OK) {
				//assert(key != 0);
				m_key = key;
				m_files = files;
				m_linkSpeed = linkSpeed;
			}
			else {
				m_linkSpeed = 0u;
			}
		}
		// access
		const Key& key() const { return m_key; }
		const FtpFileSet& files() const { return m_files; }
		u_int32 linkSpeed() const { return m_linkSpeed; }
		friend ostream& operator<<(ostream& , const DumpBeginRspMsg& );
	private:
		Key m_key;
		FtpFileSet m_files;
		u_int32 m_linkSpeed;
	};
	//
	//
	//================================================================
	class DumpBeginRspMsg_V4 : public RspHeader {
	// types
	public:
		static const PRIMITIVE s_primitive;
	public:
		DumpBeginRspMsg_V4(u_int32 transNum, u_int32 ecode, const Key& key,
			const FtpFileSet_V4& files, u_int32 linkSpeed)
			: RspHeader(s_primitive, transNum, ecode), m_key(false) {
			if(ecode == OK) {
				//assert(key != 0);
				m_key = key;
				m_files = files;
				m_linkSpeed = linkSpeed;
			}
			else {
				m_linkSpeed = 0u;
			}
		}
		// access
		const Key& key() const { return m_key; }
		const FtpFileSet_V4& files() const { return m_files; }
		u_int32 linkSpeed() const { return m_linkSpeed; }
		friend ostream& operator<<(ostream& , const DumpBeginRspMsg_V4& );
	private:
		Key m_key;
		FtpFileSet_V4 m_files;
		u_int32 m_linkSpeed;
	};
	//
	//
	//================================================================
	class DumpEndMsg : public ReqHeader {
	// types
	public:
		static const PRIMITIVE s_primitive;
	public:
		DumpEndMsg(u_int32 transNum, const Key& key, u_int32 cpSys, u_int32 cpId, u_int32 dumpResult,
		              u_int16 maj = VER_MAJOR, u_int16 min = VER_MINOR)
		: ReqHeader(s_primitive, transNum, maj, min), m_key(false), m_cpSystem(cpSys), m_cpId(cpId), m_dumpResult(dumpResult) {
			m_key = key;
		}
		// access
		const Key& key() const { return m_key; }
		u_int32 cpSystem() const { return m_cpSystem; }
		u_int32 cpId() const { return m_cpId; }
		u_int32 dumpResult() const { return m_dumpResult; }
		// modify
		void key(const Key& key) { m_key = key; }
		void cpSystem(u_int32 cpSys)  { m_cpSystem = cpSys; }
		void cpId(u_int32 cpId)  { m_cpId = cpId; }
		void dumpResult(u_int32 dumpRes) { m_dumpResult = dumpRes; }
		friend ostream& operator<<(ostream& , const DumpEndMsg& );
	private:
		Key m_key;
		u_int32 m_cpSystem; //One CP System or Multiple CP System
		u_int32 m_cpId;
		u_int32 m_dumpResult;
	};
	/* Faked Key
	//
	//
	//================================================================
	class DumpEndMsg_V4 : public ReqHeader {
	// types
	public:
		static const PRIMITIVE s_primitive;
	public:
		DumpEndMsg_V4(u_int32 transNum, const Key_V4& key, u_int32 cpSys, u_int32 cpId, u_int32 dumpResult)
		: ReqHeader(s_primitive, transNum), m_key(false), m_cpSystem(cpSys), m_cpId(cpId), m_dumpResult(dumpResult) {
			m_key = key;
		}
		// access
		const Key_V4& key() const { return m_key; }
		u_int32 cpSystem() const { return m_cpSystem; }
		u_int32 cpId() const { return m_cpId; }
		u_int32 dumpResult() const { return m_dumpResult; }
		// modify
		void key(const Key_V4& key) { m_key = key; }
		void cpSystem(u_int32 cpSys)  { m_cpSystem = cpSys; }
		void cpId(u_int32 cpId)  { m_cpId = cpId; }
		void dumpResult(u_int32 dumpRes) { m_dumpResult = dumpRes; }
		friend ostream& operator<<(ostream& , const DumpEndMsg& );
	private:
		Key_V4 m_key;
		u_int32 m_cpSystem; //One CP System or Multiple CP System
		u_int32 m_cpId;
		u_int32 m_dumpResult;
	};
	*/
	//
	//
	//================================================================
	class DumpEndRspMsg : public RspHeader {
	// types
	public:
		static const PRIMITIVE s_primitive;
	public:
		DumpEndRspMsg(u_int32 transNum, u_int32 ecode)
		: RspHeader(s_primitive, transNum, ecode) { }
		friend ostream& operator<<(ostream& , const DumpEndRspMsg& );
	private:
	};
	//
	//
	//================================================================
	class ReadConfMsg : public ReqHeader {
	// types
	public:
		static const PRIMITIVE s_primitive;
	public:
		ReadConfMsg(u_int32 transNum, u_int32 cpSys, u_int32 cpId, u_int16 maj = VER_MAJOR, u_int16 min = VER_MINOR)
		: ReqHeader(s_primitive, transNum, maj, min), m_cpSystem(cpSys), m_cpId(cpId) { }
		// access
		u_int32 cpSystem() const { return m_cpSystem; }
		u_int32 cpId() const { return m_cpId; }
		// modify
		void cpSystem(u_int32 cpSys)  { m_cpSystem = cpSys; }
		void cpId(u_int32 cpId)  { m_cpId = cpId; }
		friend ostream& operator<<(ostream& , const ReadConfMsg& );
	private:
		u_int32 m_cpSystem; //One CP System or Multiple CP System
		u_int32 m_cpId;
	};
	//
	//
	//================================================================
	class ReadConfRspMsg : public RspHeader {
	// types
	public:
		static const PRIMITIVE s_primitive;
	public:
		ReadConfRspMsg(u_int32 transNum, u_int32 ecode, u_int32 supervisionTime)
		: RspHeader(s_primitive, transNum, ecode), m_supervisionTime(0) {
			if(ecode == OK) {
				m_supervisionTime = supervisionTime;
			}
		}
		// access
		u_int32 supervisionTime() const { return m_supervisionTime; }
		friend ostream& operator<<(ostream& , const ReadConfRspMsg& );
	private:
		u_int32 m_supervisionTime;
	};
	//
	//
	//================================================================
	class ReadCmdLogConfMsg : public ReqHeader {
	// types
	public:
		static const PRIMITIVE s_primitive;
	public:
		ReadCmdLogConfMsg(u_int32 transNum, u_int32 cpSys, u_int32 cpId, u_int16 maj = VER_MAJOR, u_int16 min = VER_MINOR)
		: ReqHeader(s_primitive, transNum, maj, min), m_cpSystem(cpSys), m_cpId(cpId) { }
		// access
		u_int32 cpSystem() const { return m_cpSystem; }
		u_int32 cpId() const { return m_cpId; }
		// modify
		void cpSystem(u_int32 cpSys)  { m_cpSystem = cpSys; }
		void cpId(u_int32 cpId)  { m_cpId = cpId; }
		friend ostream& operator<<(ostream& , const ReadCmdLogConfMsg& );
	private:
		u_int32 m_cpSystem; //One CP System or Multiple CP System
		u_int32 m_cpId;
	};
	//
	//
	//================================================================
	class ReadCmdLogConfRspMsg : public RspHeader {
	// types
	public:
		static const PRIMITIVE s_primitive;
	public:
		ReadCmdLogConfRspMsg(u_int32 transNum, u_int32 ecode, int cmdLogHandling)
		: RspHeader(s_primitive, transNum, ecode), m_cmdLogHandling(0) {
			if(ecode == OK) {
				m_cmdLogHandling = cmdLogHandling;
			}
		}
		// access
		int cmdLogHandling() const { return m_cmdLogHandling; }
		friend ostream& operator<<(ostream& , const ReadCmdLogConfRspMsg& );
	private:
		int m_cmdLogHandling; // automatic CmdLog handling
	};
	//
	//
	//================================================================
	class ReadCmdLogDataMsg : public ReqHeader {
	// types
	public:
		static const PRIMITIVE s_primitive;
	public:
		ReadCmdLogDataMsg(u_int32 transNum, u_int32 cpSys, u_int32 cpId, u_int16 maj = VER_MAJOR, u_int16 min = VER_MINOR)
		: ReqHeader(s_primitive, transNum, maj, min), m_cpSystem(cpSys), m_cpId(cpId) { }
		// access
		u_int32 cpSystem() const { return m_cpSystem; }
		u_int32 cpId() const { return m_cpId; }
		// modify
		void cpSystem(u_int32 cpSys)  { m_cpSystem = cpSys; }
		void cpId(u_int32 cpId)  { m_cpId = cpId; }
		friend ostream& operator<<(ostream& , const ReadCmdLogDataMsg& );
	private:
		u_int32 m_cpSystem; //One CP System or Multiple CP System
		u_int32 m_cpId;
	};
	//
	//
	//================================================================
	class ReadCmdLogDataRspMsg : public RspHeader {
	// types
	public:
		static const PRIMITIVE s_primitive;
	public:
		ReadCmdLogDataRspMsg(u_int32 transNum, u_int32 ecode, u_int32 currCmdLog)
		: RspHeader(s_primitive, transNum, ecode), m_currCmdLog(0u) {
			if(ecode == OK) {
				m_currCmdLog = currCmdLog;
			}
		}
		// access
		u_int32 currCmdLog() const { return m_currCmdLog; }
		friend ostream& operator<<(ostream& , const ReadCmdLogDataRspMsg& );
	private:
		u_int32 m_currCmdLog; // current cmd log number
	};
	//
	//
	//================================================================
	class WriteCmdLogDataMsg : public ReqHeader {
	// types
	public:
		static const PRIMITIVE s_primitive;
	public:
		WriteCmdLogDataMsg(u_int32 transNum, u_int32 currCmdLog, u_int32 cpSys, u_int32 cpId, 
		                      u_int16 maj = VER_MAJOR, u_int16 min = VER_MINOR)
		: ReqHeader(s_primitive, transNum, maj, min), m_currCmdLog(currCmdLog), m_cpSystem(cpSys), m_cpId(cpId) { }
		// access
		u_int32 currCmdLog() const { return m_currCmdLog; }
		u_int32 cpSystem() const { return m_cpSystem; }
		u_int32 cpId() const { return m_cpId; }
		// modify
		void currCmdLog(u_int32 val) { m_currCmdLog = val; }
		void cpSystem(u_int32 cpSys)  { m_cpSystem = cpSys; }
		void cpId(u_int32 cpId)  { m_cpId = cpId; }

		friend ostream& operator<<(ostream& , const WriteCmdLogDataMsg& );
	private:
		u_int32 m_currCmdLog; // active/current command log file number
		u_int32 m_cpSystem; //One CP System or Multiple CP System
		u_int32 m_cpId;
	};
	//
	//
	//================================================================
	class WriteCmdLogDataRspMsg : public RspHeader {
	// types
	public:
		static const PRIMITIVE s_primitive;
	public:
		WriteCmdLogDataRspMsg(u_int32 transNum, u_int32 ecode)
		: RspHeader(s_primitive, transNum, ecode) { }
		friend ostream& operator<<(ostream& , const WriteCmdLogDataRspMsg& );
	private:
	};
	//
	//
	//================================================================
	class SBCSwitchMsg : public ReqHeader {
	// types
	public:
		static const PRIMITIVE s_primitive;
	public:
		SBCSwitchMsg(u_int32 transNum, SWITCH_OPERATION operation,
			         SBCId::SBC_RANGE range, u_int32 cpSys, u_int32 cpId,
			         u_int16 maj = VER_MAJOR, u_int16 min = VER_MINOR)
		: ReqHeader(s_primitive, transNum, maj, min), m_operation(operation),
			m_range(range), m_cpSystem(cpSys), m_cpId(cpId) { }
		// access
		SWITCH_OPERATION operation() const { return m_operation; }
		SBCId::SBC_RANGE range() const { return m_range; }
		u_int32 cpSystem() const { return m_cpSystem; }
		u_int32 cpId() const { return m_cpId; }
		// modify
		void cpSystem(u_int32 cpSys)  { m_cpSystem = cpSys; }
		void cpId(u_int32 cpId)  { m_cpId = cpId; }

		friend ostream& operator<<(ostream& , const SBCSwitchMsg& );
	private:
		SWITCH_OPERATION m_operation;
		SBCId::SBC_RANGE m_range;
		u_int32 m_cpSystem; //One CP System or Multiple CP System
		u_int32 m_cpId;
	};

	//
	//
	//================================================================
	class SBCSwitchRspMsg : public RspHeader {
	// types
	public:
		static const PRIMITIVE s_primitive;
	public:
		SBCSwitchRspMsg(u_int32 transNum, u_int32 ecode)
		: RspHeader(s_primitive, transNum, ecode) { }
		friend ostream& operator<<(ostream& , const SBCSwitchRspMsg& );
	private:
	};
	//
	//
	//================================================================
	class ListSBCsMsg : public ReqHeader {
	// types
	public:
		static const PRIMITIVE s_primitive;
	public:
		ListSBCsMsg(u_int32 transNum, bool firstSequence, SBCId::SBC_RANGE range, u_int32 cpSys, u_int32 cpId,
		              u_int16 maj = VER_MAJOR, u_int16 min = VER_MINOR)
		: ReqHeader(s_primitive, transNum, maj, min), m_firstSequence(firstSequence),
			m_range(range), m_cpSystem(cpSys), m_cpId(cpId) { }
		// access
		bool firstSequence() const { return m_firstSequence; }
		SBCId::SBC_RANGE range() const { return m_range; }
		u_int32 cpSystem() const { return m_cpSystem; }
		u_int32 cpId() const { return m_cpId; }
		// modify
		void cpSystem(u_int32 cpSys)  { m_cpSystem = cpSys; }
		void cpId(u_int32 cpId)  { m_cpId = cpId; }

		friend ostream& operator<<(ostream& , const ListSBCsMsg& );
	private:
		bool m_firstSequence;
		SBCId::SBC_RANGE m_range; // FFR, SFR, COMPLETE
		u_int32 m_cpSystem; //One CP System or Multiple CP System
		u_int32 m_cpId;
	};
	//
	//
	//================================================================
	class ListSBCsRspMsg : public RspHeader {
	// types
	public:
		static const PRIMITIVE s_primitive;
	public:
		ListSBCsRspMsg(u_int32 transNum, u_int32 ecode, u_int32 nofSBCs, const SBCId* sbc)
		: RspHeader(s_primitive, transNum, ecode), m_nofSBCs(nofSBCs) {
			for(u_int32 i = 0; i < SBCId::MAX_NOF_SBCS; ++i)
				m_sbc[i] = 0u;
			if(ecode == OK) {
				for(u_int32 i = 0; i < nofSBCs; ++i)
					m_sbc[i] = sbc[i];
			}
			else
				m_nofSBCs = 0;
		}
		// access
		u_int32 nofSBCs() const { return m_nofSBCs; }
		const u_int16* sbc() const { return m_sbc; }
		friend ostream& operator<<(ostream& , const ListSBCsRspMsg& );
	private:
		u_int32 m_nofSBCs;
		u_int16 m_sbc[SBCId::MAX_NOF_SBCS];
	};
	//
	//
	//================================================================
	class VerifySBCDataMsg : public ReqHeader {
	// types
	public:
		static const PRIMITIVE s_primitive;
	public:
		VerifySBCDataMsg(u_int32 transNum, u_int32 sddCheck, u_int32 ldd1Check, u_int32 ldd2Check,
			u_int32 psCheck, u_int32 rsCheck, SBCId sbcId, u_int32 cpSys, u_int32 cpId,
			u_int16 maj = VER_MAJOR, u_int16 min = VER_MINOR)
		: ReqHeader(s_primitive, transNum, maj, min), m_sddCheck(sddCheck), m_ldd1Check(ldd1Check),
			m_ldd2Check(ldd2Check), m_psCheck(psCheck), m_rsCheck(rsCheck), m_sbcId(sbcId), m_cpSystem(cpSys), m_cpId(cpId) { }
		// access
		u_int32 sddCheck() const { return m_sddCheck; }
		u_int32 ldd1Check() const { return m_ldd1Check; }
		u_int32 ldd2Check() const { return m_ldd2Check; }
		u_int32 psCheck() const { return m_psCheck; }
		u_int32 rsCheck() const { return m_rsCheck; }
		SBCId sbcId() const { return m_sbcId; }
		u_int32 cpSystem() const { return m_cpSystem; }
		u_int32 cpId() const { return m_cpId; }
		// modify
		void cpSystem(u_int32 cpSys)  { m_cpSystem = cpSys; }
		void cpId(u_int32 cpId)  { m_cpId = cpId; }

		friend ostream& operator<<(ostream& , const VerifySBCDataMsg& );
	private:
		u_int32 m_sddCheck;
		u_int32 m_ldd1Check;
		u_int32 m_ldd2Check;
		u_int32 m_psCheck;
		u_int32 m_rsCheck;
		SBCId m_sbcId;
		u_int32 m_cpSystem; //One CP System or Multiple CP System
		u_int32 m_cpId;
	};

	//
	//
	//================================================================
	class VerifySBCDataRspMsg : public RspHeader {
	// types
	public:
		static const PRIMITIVE s_primitive;
	public:
		VerifySBCDataRspMsg(u_int32 transNum, u_int32 ecode,
			SBC_DATA_STATUS sddStatus, SBC_DATA_STATUS ldd1Status,
			SBC_DATA_STATUS ldd2Status, SBC_DATA_STATUS psStatus,
			SBC_DATA_STATUS rsStatus)
		: RspHeader(s_primitive, transNum, ecode) {
			if(ecode == OK) {
				m_sddStatus = sddStatus;
				m_ldd1Status = ldd1Status;
				m_ldd2Status = ldd2Status;
				m_psStatus = psStatus;
				m_rsStatus = rsStatus;
			}
			else {
                m_sddStatus = m_ldd1Status = m_ldd2Status = m_psStatus = m_rsStatus = SDS_NO_ERROR;
			}
		}
		// access
		SBC_DATA_STATUS sddStatus() const { return m_sddStatus; }
		SBC_DATA_STATUS ldd1Status() const { return m_ldd1Status; }
		SBC_DATA_STATUS ldd2Status() const { return m_ldd2Status; }
		SBC_DATA_STATUS psStatus() const { return m_psStatus; }
		SBC_DATA_STATUS rsStatus() const { return m_rsStatus; }
		friend ostream& operator<<(ostream& , const VerifySBCDataRspMsg& );
	private:
		SBC_DATA_STATUS m_sddStatus;
		SBC_DATA_STATUS m_ldd1Status;
		SBC_DATA_STATUS m_ldd2Status;
		SBC_DATA_STATUS m_psStatus;
		SBC_DATA_STATUS m_rsStatus;
	};
	//
	//
	//================================================================
	class VerifySBCDataMsg2 : public ReqHeader {
	// types
	public:
		static const PRIMITIVE s_primitive;
	public:
		VerifySBCDataMsg2(u_int32 transNum, u_int32 sddCheck, u_int32 ldd1Check, u_int32 ldd2Check,
			u_int32 psCheck, u_int32 rsCheck, u_int32 rs2Check, u_int32 ps2Check, SBCId sbcId, 
			u_int32 cpSys, u_int32 cpId)
		: ReqHeader(s_primitive, transNum), m_sddCheck(sddCheck), m_ldd1Check(ldd1Check),
			m_ldd2Check(ldd2Check), m_psCheck(psCheck), m_rsCheck(rsCheck), m_ps2Check(ps2Check), 
			m_rs2Check(rs2Check), m_sbcId(sbcId), m_cpSystem(cpSys), m_cpId(cpId) { }
		// access
		u_int32 sddCheck() const { return m_sddCheck; }
		u_int32 ldd1Check() const { return m_ldd1Check; }
		u_int32 ldd2Check() const { return m_ldd2Check; }
		u_int32 psCheck() const { return m_psCheck; }
		u_int32 rsCheck() const { return m_rsCheck; }
		u_int32 ps2Check() const { return m_ps2Check; }
		u_int32 rs2Check() const { return m_rs2Check; }
		SBCId sbcId() const { return m_sbcId; }
		u_int32 cpSystem() const { return m_cpSystem; }
		u_int32 cpId() const { return m_cpId; }
		// modify
		void cpSystem(u_int32 cpSys)  { m_cpSystem = cpSys; }
		void cpId(u_int32 cpId)  { m_cpId = cpId; }

		friend ostream& operator<<(ostream& , const VerifySBCDataMsg2& );
	private:
		u_int32 m_sddCheck;
		u_int32 m_ldd1Check;
		u_int32 m_ldd2Check;
		u_int32 m_psCheck;
		u_int32 m_rsCheck;
		u_int32 m_ps2Check;
		u_int32 m_rs2Check;
		SBCId m_sbcId;
		u_int32 m_cpSystem; //One CP System or Multiple CP System
		u_int32 m_cpId;
	};
	//
	//
	//================================================================
	class VerifySBCDataRspMsg2 : public RspHeader {
	// types
	public:
		static const PRIMITIVE s_primitive;
	public:
		VerifySBCDataRspMsg2(u_int32 transNum, u_int32 ecode,
			SBC_DATA_STATUS sddStatus, SBC_DATA_STATUS ldd1Status,
			SBC_DATA_STATUS ldd2Status, SBC_DATA_STATUS psStatus,
			SBC_DATA_STATUS rsStatus, SBC_DATA_STATUS ps2Status,
			SBC_DATA_STATUS rs2Status)
		: RspHeader(s_primitive, transNum, ecode) {
			if(ecode == OK) {
				m_sddStatus = sddStatus;
				m_ldd1Status = ldd1Status;
				m_ldd2Status = ldd2Status;
				m_psStatus = psStatus;
				m_rsStatus = rsStatus;
				m_ps2Status = ps2Status;
				m_rs2Status = rs2Status;
			}
		}
		// access
		SBC_DATA_STATUS sddStatus() const { return m_sddStatus; }
		SBC_DATA_STATUS ldd1Status() const { return m_ldd1Status; }
		SBC_DATA_STATUS ldd2Status() const { return m_ldd2Status; }
		SBC_DATA_STATUS psStatus() const { return m_psStatus; }
		SBC_DATA_STATUS rsStatus() const { return m_rsStatus; }
		SBC_DATA_STATUS ps2Status() const { return m_ps2Status; }
		SBC_DATA_STATUS rs2Status() const { return m_rs2Status; }
		friend ostream& operator<<(ostream& , const VerifySBCDataRspMsg2& );
	private:
		SBC_DATA_STATUS m_sddStatus;
		SBC_DATA_STATUS m_ldd1Status;
		SBC_DATA_STATUS m_ldd2Status;
		SBC_DATA_STATUS m_psStatus;
		SBC_DATA_STATUS m_rsStatus;
		SBC_DATA_STATUS m_ps2Status;
		SBC_DATA_STATUS m_rs2Status;
	};
	//
	//
	//================================================================
	class SBCInfoMsg : public ReqHeader {
	// types
	public:
		static const PRIMITIVE s_primitive;
	public:
		SBCInfoMsg(u_int32 transNum, SBCId sbcId, u_int32 cpSys, u_int32 cpId,
		              u_int16 maj = VER_MAJOR, u_int16 min = VER_MINOR)
		: ReqHeader(s_primitive, transNum, maj, min), m_sbcId(sbcId), m_cpSystem(cpSys), m_cpId(cpId) { }
		// access
		SBCId sbcId() const { return m_sbcId; }
		u_int32 cpSystem() const { return m_cpSystem; }
		u_int32 cpId() const { return m_cpId; }
		// modify
		void cpSystem(u_int32 cpSys)  { m_cpSystem = cpSys; }
		void cpId(u_int32 cpId)  { m_cpId = cpId; }

		friend ostream& operator<<(ostream& , const SBCInfoMsg& );
	private:
		SBCId m_sbcId;
		u_int32 m_cpSystem; //One CP System or Multiple CP System
		u_int32 m_cpId;
	};
	//
	//
	//================================================================
	class SBCInfoRspMsg : public RspHeader {
	// types
	public:
		static const PRIMITIVE s_primitive;
	public:
		SBCInfoRspMsg(u_int32 transNum, u_int32 ecode, Version buinfoVersion,
         APZVersion apzVersion, ExchangeId exchangeId, u_int32 generation,
         SectorInfo sddInfo, SectorInfo ldd1Info, SectorInfo ldd2Info, SectorInfo psInfo,
         SectorInfo rsInfo, u_int32 cmdLogFile1, u_int32 cmdLogFile2)
		: RspHeader(s_primitive, transNum, ecode) {
			if(ecode == OK) {
				m_buinfoVersion = buinfoVersion;
				m_apzVersion = apzVersion;
				m_exchangeId = exchangeId;
				m_generation = generation;
				m_sddInfo = sddInfo;
				m_ldd1Info = ldd1Info;
				m_ldd2Info = ldd2Info;
				m_psInfo = psInfo;
				m_rsInfo = rsInfo;
				m_cmdLogFile1 = cmdLogFile1;
				m_cmdLogFile2 = cmdLogFile2;
			}
			else {
				m_generation = m_cmdLogFile1 = m_cmdLogFile2 = 0u;
			}
		}

		// access
		Version buinfoVersion() const { return m_buinfoVersion; }
		const APZVersion& apzVersion() const { return m_apzVersion; }
		const ExchangeId& exchangeId() const { return m_exchangeId; }
		u_int32 generation() const { return m_generation; }
		SectorInfo sddInfo() const { return m_sddInfo; }
		SectorInfo ldd1Info() const { return m_ldd1Info; }
		SectorInfo ldd2Info() const { return m_ldd2Info; }
		SectorInfo psInfo() const { return m_psInfo; }
		SectorInfo rsInfo() const { return m_rsInfo; }
		u_int32 cmdLogFile1() const { return m_cmdLogFile1; }
		u_int32 cmdLogFile2() const { return m_cmdLogFile2; }
		friend ostream& operator<<(ostream& , const SBCInfoRspMsg& );
	private:
		Version m_buinfoVersion;
		APZVersion m_apzVersion;
		ExchangeId m_exchangeId;
		u_int32 m_generation;
		SectorInfo m_sddInfo;
		SectorInfo m_ldd1Info;
		SectorInfo m_ldd2Info;
		SectorInfo m_psInfo;
		SectorInfo m_rsInfo;
		u_int32 m_cmdLogFile1;
		u_int32 m_cmdLogFile2;
	};
	//
	//
	//================================================================
	class TestMsg : public ReqHeader {
	// types
	public:
		static const PRIMITIVE s_primitive;
	public:
		TestMsg(u_int32 transNum, SBCId sbcId, u_int32 cpSys, u_int32 cpId,
		              u_int16 maj = VER_MAJOR, u_int16 min = VER_MINOR)
		: ReqHeader(s_primitive, transNum, maj, min), m_sbcId(sbcId), m_cpSystem(cpSys), m_cpId(cpId) { }
		// access
		SBCId sbcId() const { return m_sbcId; }
		u_int32 cpSystem() const { return m_cpSystem; }
		u_int32 cpId() const { return m_cpId; }
		// modify
		void cpSystem(u_int32 cpSys)  { m_cpSystem = cpSys; }
		void cpId(u_int32 cpId)  { m_cpId = cpId; }

		//friend ostream& operator<<(ostream& , const TestMsg& );
	private:
		SBCId m_sbcId;
		u_int32 m_cpSystem; //One CP System or Multiple CP System
		u_int32 m_cpId;
	};

	//
	//
	//================================================================
	class SBCInfoRspMsg2 : public RspHeader {
	// types
	public:
		static const PRIMITIVE s_primitive;
	public:
		SBCInfoRspMsg2(u_int32 transNum, u_int32 ecode, Version buinfoVersion,
         APZVersion apzVersion, ExchangeId exchangeId, u_int32 generation,
         SectorInfo sddInfo, SectorInfo ldd1Info, SectorInfo ldd2Info, SectorInfo psInfo,
         SectorInfo rsInfo, SectorInfo ps2Info, SectorInfo rs2Info, u_int32 dsGeneration,
		 u_int32 ldd1OnMProfile, u_int32 ldd1APZProfile,
		 u_int32 ldd1APTProfile, u_int32 ldd1CpId,
		 u_int32 ldd2OnMProfile, u_int32 ldd2APZProfile,
		 u_int32 ldd2APTProfile, u_int32 ldd2CpId,
		 u_int32 cmdLogFile1, u_int32 cmdLogFile2)
		: RspHeader(s_primitive, transNum, ecode) {
			if(ecode == OK) {
				m_buinfoVersion = buinfoVersion;
				m_apzVersion = apzVersion;
				m_exchangeId = exchangeId;
				m_generation = generation;
				m_sddInfo = sddInfo;
				m_ldd1Info = ldd1Info;
				m_ldd2Info = ldd2Info;
				m_psInfo = psInfo;
				m_rsInfo = rsInfo;
				m_ps2Info = ps2Info;
				m_rs2Info = rs2Info;
				m_dsGeneration = dsGeneration;
				m_ldd1OnMProfile = ldd1OnMProfile;
				m_ldd1APZProfile = ldd1APZProfile;
				m_ldd1APTProfile = ldd1APTProfile;
				m_ldd1CpId = ldd1CpId;
				m_ldd2OnMProfile = ldd2OnMProfile;
				m_ldd2APZProfile = ldd2APZProfile;
				m_ldd2APTProfile = ldd2APTProfile;
				m_ldd2CpId = ldd2CpId;
				m_cmdLogFile1 = cmdLogFile1;
				m_cmdLogFile2 = cmdLogFile2;
			}
		}

		// access
		Version buinfoVersion() const { return m_buinfoVersion; }
		const APZVersion& apzVersion() const { return m_apzVersion; }
		const ExchangeId& exchangeId() const { return m_exchangeId; }
		u_int32 generation() const { return m_generation; }
		SectorInfo sddInfo() const { return m_sddInfo; }
		SectorInfo ldd1Info() const { return m_ldd1Info; }
		SectorInfo ldd2Info() const { return m_ldd2Info; }
		SectorInfo psInfo() const { return m_psInfo; }
		SectorInfo rsInfo() const { return m_rsInfo; }
		SectorInfo ps2Info() const { return m_ps2Info; }
		SectorInfo rs2Info() const { return m_rs2Info; }
		u_int32 dsGeneration() const { return m_dsGeneration; }
		u_int32 ldd1OnMProfile() const { return m_ldd1OnMProfile; }
		u_int32 ldd1APZProfile() const { return m_ldd1APZProfile; }
		u_int32 ldd1APTProfile() const { return m_ldd1APTProfile; }
		u_int32 ldd1CpId() const { return m_ldd1CpId; }
		u_int32 ldd2OnMProfile() const { return m_ldd2OnMProfile; }
		u_int32 ldd2APZProfile() const { return m_ldd2APZProfile; }
		u_int32 ldd2APTProfile() const { return m_ldd2APTProfile; }
		u_int32 ldd2CpId() const { return m_ldd2CpId; }
		u_int32 cmdLogFile1() const { return m_cmdLogFile1; }
		u_int32 cmdLogFile2() const { return m_cmdLogFile2; }
		friend ostream& operator<<(ostream& , const SBCInfoRspMsg2& );
	private:
		Version m_buinfoVersion;
		APZVersion m_apzVersion;
		ExchangeId m_exchangeId;
		u_int32 m_generation;
		SectorInfo m_sddInfo;
		SectorInfo m_ldd1Info;
		SectorInfo m_ldd2Info;
		SectorInfo m_psInfo;
		SectorInfo m_rsInfo;
		SectorInfo m_ps2Info;
		SectorInfo m_rs2Info;
		u_int32 m_dsGeneration;
		u_int32 m_ldd1OnMProfile;
		u_int32 m_ldd1APZProfile;
		u_int32 m_ldd1APTProfile;
		u_int32 m_ldd1CpId;
		u_int32 m_ldd2OnMProfile;
		u_int32 m_ldd2APZProfile;
		u_int32 m_ldd2APTProfile;
		u_int32 m_ldd2CpId;
		u_int32 m_cmdLogFile1;
		u_int32 m_cmdLogFile2;
	};
	//
	//
	//================================================================
	class WriteALogDataMsg : public ReqHeader {
	// types
	public:
		static const PRIMITIVE s_primitive;
	public:
		WriteALogDataMsg(u_int32 transNum, u_int32 cpSys, 
			u_int32 cpId, u_int8 len, u_int8 *buff)
		: ReqHeader(s_primitive, transNum), m_cpSystem(cpSys), m_cpId(cpId), 
			m_ALogData(len, buff) { }
		
		// access
		u_int32 cpSystem() const { return m_cpSystem; }
		u_int32 cpId() const { return m_cpId; }
		u_int8  nofCharacters() const { return m_ALogData.nofCharacters(); }
		const u_int8* character() const { return m_ALogData.character(); }
		const ALogData& getData() const { return m_ALogData; }
		
		// modify
		void cpSystem(u_int32 cpSys)  { m_cpSystem = cpSys; }
		void cpId(u_int32 cpId)  { m_cpId = cpId; }

		friend ostream& operator<<(ostream& , const WriteALogDataMsg& );
	private:
		u_int32		m_cpSystem; 
		u_int32		m_cpId;
		ALogData	m_ALogData;
	};
	//
	//
	//================================================================
	class WriteALogDataRspMsg : public RspHeader {
	// types
	public:
		static const PRIMITIVE s_primitive;
	public:
		WriteALogDataRspMsg(u_int32 transNum, u_int32 ecode)
		: RspHeader(s_primitive, transNum, ecode) { }
		friend ostream& operator<<(ostream& , const WriteALogDataRspMsg& );
	private:
	};
//
//
//
// funx
//================================================================
public:
	BUPMsg() { reset(); }
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
	friend ostream& operator<<(ostream& , const BUPMsg& );
// attrib
private:
	u_int64 m_buffer[MSG_SIZE_IN_BYTES / sizeof(u_int64)];
};
//
//
//===================================================================
template<class T>
T* bup_ptr_cast(BUPMsg& msg) {
	assert(msg.msgHeader()->primitive() == T::s_primitive);
	return msg.msgHeader()->primitive() == T::s_primitive ? reinterpret_cast<T*>(msg.addr()) : NULL;
}

template<class T>
const T* bup_ptr_cast(const BUPMsg& msg) {
	assert(msg.msgHeader()->primitive() == T::s_primitive);
	return msg.msgHeader()->primitive() == T::s_primitive ? reinterpret_cast<const T*>(msg.addr()) : NULL;
}
#endif
