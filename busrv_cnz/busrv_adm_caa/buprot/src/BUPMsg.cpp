/*
NAME
	File_name:BUPMsg.cpp

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
	190 89-CAA 109 0387

AUTHOR
	2002-02-01 by UAB/KY/SK Anders Gillgren (qabgill, tag :ag)

SEE ALSO
	-

Revision history
----------------
2002-02-01 qabgill Created
2006-06-09 uablan  Added CP-AP link speed in return msg for DumpBeginRsp,LoadBeginRsp
                   and FallbackBeginRsp.
2010-05-23 xdtthng Modified for SSI

*/

#include "BUPMsg.h"
#include "BackupInfo.h"

#include <iomanip>
//
// Static Inits
//====================================================================
const BUPMsg::PRIMITIVE BUPMsg::LoadBeginMsg::s_primitive = BUPMsg::LOAD_BEGIN;
const BUPMsg::PRIMITIVE BUPMsg::LoadBeginRspMsg::s_primitive = BUPMsg::LOAD_BEGIN_RSP;
const BUPMsg::PRIMITIVE BUPMsg::LoadBeginRspMsg_V4::s_primitive = BUPMsg::LOAD_BEGIN_RSP;

const BUPMsg::PRIMITIVE BUPMsg::LoadEndMsg::s_primitive = BUPMsg::LOAD_END;
//const BUPMsg::PRIMITIVE BUPMsg::LoadEndMsg_V4::s_primitive = BUPMsg::LOAD_END;

const BUPMsg::PRIMITIVE BUPMsg::LoadEndRspMsg::s_primitive = BUPMsg::LOAD_END_RSP;
const BUPMsg::PRIMITIVE BUPMsg::DumpBeginMsg::s_primitive = BUPMsg::DUMP_BEGIN;
const BUPMsg::PRIMITIVE BUPMsg::DumpBeginRspMsg::s_primitive = BUPMsg::DUMP_BEGIN_RSP;
const BUPMsg::PRIMITIVE BUPMsg::DumpEndMsg::s_primitive = BUPMsg::DUMP_END;
const BUPMsg::PRIMITIVE BUPMsg::DumpEndRspMsg::s_primitive = BUPMsg::DUMP_END_RSP;
const BUPMsg::PRIMITIVE BUPMsg::DumpBeginRspMsg_V4::s_primitive = BUPMsg::DUMP_BEGIN_RSP;


const BUPMsg::PRIMITIVE BUPMsg::FallbackBeginMsg::s_primitive = BUPMsg::FALLBACK_BEGIN;
const BUPMsg::PRIMITIVE BUPMsg::FallbackBeginRspMsg::s_primitive = BUPMsg::FALLBACK_BEGIN_RSP;
const BUPMsg::PRIMITIVE BUPMsg::FallbackBeginRspMsg_V4::s_primitive = BUPMsg::FALLBACK_BEGIN_RSP;
const BUPMsg::PRIMITIVE BUPMsg::FallbackEndMsg::s_primitive = BUPMsg::FALLBACK_END;
const BUPMsg::PRIMITIVE BUPMsg::FallbackEndRspMsg::s_primitive = BUPMsg::FALLBACK_END_RSP;

const BUPMsg::PRIMITIVE BUPMsg::ReadConfMsg::s_primitive = BUPMsg::READ_CONF;
const BUPMsg::PRIMITIVE BUPMsg::ReadConfRspMsg::s_primitive = BUPMsg::READ_CONF_RSP;
const BUPMsg::PRIMITIVE BUPMsg::ReadCmdLogConfMsg::s_primitive = BUPMsg::READ_CMDLOG_CONF;
const BUPMsg::PRIMITIVE BUPMsg::ReadCmdLogConfRspMsg::s_primitive = BUPMsg::READ_CMDLOG_CONF_RSP;
const BUPMsg::PRIMITIVE BUPMsg::ReadCmdLogDataMsg::s_primitive = BUPMsg::READ_CMDLOG_DATA;
const BUPMsg::PRIMITIVE BUPMsg::ReadCmdLogDataRspMsg::s_primitive = BUPMsg::READ_CMDLOG_DATA_RSP;
const BUPMsg::PRIMITIVE BUPMsg::WriteCmdLogDataMsg::s_primitive = BUPMsg::WRITE_CMDLOG_DATA;
const BUPMsg::PRIMITIVE BUPMsg::WriteCmdLogDataRspMsg::s_primitive = BUPMsg::WRITE_CMDLOG_DATA_RSP;

#if 0
const BUPMsg::PRIMITIVE BUPMsg::FallbackBeginRspMsg_V4::s_primitive = BUPMsg::FALLBACK_BEGIN_RSP;

//const BUPMsg::PRIMITIVE BUPMsg::FallbackEndMsg::s_primitive = BUPMsg::FALLBACK_END;
//const BUPMsg::PRIMITIVE BUPMsg::FallbackEndMsg_V4::s_primitive = BUPMsg::FALLBACK_END;

//const BUPMsg::PRIMITIVE BUPMsg::FallbackEndRspMsg::s_primitive = BUPMsg::FALLBACK_END_RSP;

//const BUPMsg::PRIMITIVE BUPMsg::DumpEndMsg_V4::s_primitive = BUPMsg::DUMP_END;


const BUPMsg::PRIMITIVE BUPMsg::SBCSwitchMsg::s_primitive = BUPMsg::SBC_SWITCH;
#endif

const BUPMsg::PRIMITIVE BUPMsg::SBCSwitchMsg::s_primitive = BUPMsg::SBC_SWITCH;
const BUPMsg::PRIMITIVE BUPMsg::SBCSwitchRspMsg::s_primitive = BUPMsg::SBC_SWITCH_RSP;
const BUPMsg::PRIMITIVE BUPMsg::ListSBCsMsg::s_primitive = BUPMsg::LIST_SBCS;
const BUPMsg::PRIMITIVE BUPMsg::ListSBCsRspMsg::s_primitive = BUPMsg::LIST_SBCS_RSP;
const BUPMsg::PRIMITIVE BUPMsg::SBCInfoMsg::s_primitive = BUPMsg::SBC_INFO;
const BUPMsg::PRIMITIVE BUPMsg::SBCInfoRspMsg::s_primitive = BUPMsg::SBC_INFO_RSP;
const BUPMsg::PRIMITIVE BUPMsg::VerifySBCDataMsg::s_primitive = BUPMsg::VERIFY_SBC_DATA;
const BUPMsg::PRIMITIVE BUPMsg::VerifySBCDataRspMsg::s_primitive = BUPMsg::VERIFY_SBC_DATA_RSP;
const BUPMsg::PRIMITIVE BUPMsg::VerifySBCDataMsg2::s_primitive = BUPMsg::VERIFY_SBC_DATA;
const BUPMsg::PRIMITIVE BUPMsg::VerifySBCDataRspMsg2::s_primitive = BUPMsg::VERIFY_SBC_DATA_RSP;

const BUPMsg::PRIMITIVE BUPMsg::TestMsg::s_primitive = BUPMsg::SBC1_TO_SFR;
const BUPMsg::PRIMITIVE BUPMsg::SBCInfoRspMsg2::s_primitive = BUPMsg::SBC_INFO_RSP;

const BUPMsg::PRIMITIVE BUPMsg::WriteALogDataMsg::s_primitive = BUPMsg::WRITE_ALOG_DATA;
const BUPMsg::PRIMITIVE BUPMsg::WriteALogDataRspMsg::s_primitive = BUPMsg::WRITE_ALOG_DATA_RSP;


//
//
//
const char* BUPMsg::ERROR_CODE_STRING[NUMBER_OF_ERROR_CODES] = {
	 "0 OK" ,
	 "1 UNSUP_BUPROT_VERSION" ,
	 "2 INTERNAL_ERROR" ,
	 "3 BAD_MSG" ,
	 "4 UNKNOWN_PRIMITIVE" ,
	 "5 INVALID_KEY" ,
	 "6 CREATE_DATAAREA_FAILED" ,
	 "7 REQ_DATAAREA_MISSING" ,
	 "8 BAD_BACKUPINFO" ,
	 "9 BAD_DATAAREA" ,
	 "10 WRITE_DATAAREA_ERROR" ,
	 "11 SBC_ROOT_NOT_FOUND" ,
	 "12 SBC_NOT_FOUND" ,
	 "13 SBC_RESERVE_FAILED" ,
	 "14 PARAM_BACKUPID_OUT_OF_RANGE" ,
	 "15 PARAM_SBC_RANGE_INVALID" ,
	 "16 PARAM_EXTENT_INVALID" ,
	 "17 PARAM_SWITCH_OPERATION_INVALID" ,
	 "18 PARAM_ONLY_SBC0_FOR_PARTIAL_DUMP" ,
	 "19 SWITCH_INSUFFICIENT_NOF_SBCS" ,
	 "20 SWITCH_SBC_DATES_ILLEGAL_FOR_OP" ,
	 "21 SWITCH_LOW_SBC_NOT_FOUND" ,
	 "22 SWITCH_HIGH_SBC_NOT_FOUND" ,
	 "23 SWITCH_LOW_BACKUPINFO_BAD" ,
	 "24 SWITCH_HIGH_BACKUPINFO_BAD" ,
	 "25 SWITCH_LOW_ID_NOT_BASE" ,
	 "26 SWITCH_TEMP_SBC_EXIST" ,
	 "27 SHUTDOWN_PENDING" ,
	 "28 FMS_PHYSICAL_FILE_ERROR" ,
	 "29 MAX_NO_DUMP_LOAD_ONGOING" ,
	"30 THIS_HOLDS " ,
	"32",
	"33",
	"34"
};
//
//
//============================================================================
ostream& operator<<(ostream& os, const BUPMsg::MsgHeader& msg) {
	os << dec << "<" << msg.primitive() << "><" << msg.transNum() << ">";
	return os;
}
//
//
//============================================================================
ostream& operator<<(ostream& os, const BUPMsg::ReqHeader& msg) {
	os << static_cast<const BUPMsg::MsgHeader&>(msg) << msg.version() << ">\n";
	return os;
}
//
//
//============================================================================
ostream& operator<<(ostream& os, const BUPMsg::RspHeader& msg) {
	os << static_cast<const BUPMsg::MsgHeader&>(msg) << "<" << msg.errorCode() << ">\n";
	return os;
}
//
//
//============================================================================
ostream& operator<<(ostream& os, const BUPMsg::LoadBeginMsg& msg) {
	os << static_cast<const BUPMsg::ReqHeader&>(msg) << msg.sbcId() << "\n" << msg.cpId();
	return os;
}
//
//
//============================================================================
ostream& operator<<(ostream& os, const BUPMsg::LoadBeginRspMsg& msg) {
	os << static_cast<const BUPMsg::RspHeader&>(msg) << msg.key()
		<< msg.files() << "\n"
		<< msg.linkSpeed();
	return os;
}
//
//
//============================================================================
ostream& operator<<(ostream& os, const BUPMsg::LoadBeginRspMsg_V4& msg) {
	os << static_cast<const BUPMsg::RspHeader&>(msg) << msg.key() << "\n"
		<< msg.files() << "\n"
		<< msg.linkSpeed();
	return os;
}
//
//
//============================================================================
ostream& operator<<(ostream& os, const BUPMsg::LoadEndMsg& msg) {
	os << static_cast<const BUPMsg::ReqHeader&>(msg) << "<" << msg.key() << "\n" << msg.cpId();
	return os;
}
/* This is FAKE key
//
//
//============================================================================
ostream& operator<<(ostream& os, const BUPMsg::LoadEndMsg_V4& msg) {
	os << static_cast<const BUPMsg::ReqHeader&>(msg) << "<" << msg.key() << "\n" << msg.cpId();
	return os;
}
*/
//
//
//============================================================================
ostream& operator<<(ostream& os, const BUPMsg::LoadEndRspMsg& msg) {
	os << static_cast<const BUPMsg::RspHeader&>(msg);
	return os;
}
//
//
//============================================================================
ostream& operator<<(ostream& os, const BUPMsg::FallbackBeginMsg& msg) {
	os << static_cast<const BUPMsg::ReqHeader&>(msg) << "\n" << msg.cpId();
	return os;
}
//
//
//============================================================================
ostream& operator<<(ostream& os, const BUPMsg::FallbackBeginRspMsg& msg) {
	os << static_cast<const BUPMsg::RspHeader&>(msg) << msg.key()
		<< msg.sbcId() << "<" << msg.lastPossibleSBCLoaded() << ">\n"
		<< msg.files() << "\n"
		<< msg.linkSpeed();
	return os;
}
//
//
//============================================================================
ostream& operator<<(ostream& os, const BUPMsg::FallbackBeginRspMsg_V4& msg) {
	os << static_cast<const BUPMsg::RspHeader&>(msg) << msg.key()
		<< msg.sbcId() << "<" << msg.lastPossibleSBCLoaded() << ">\n"
		<< msg.files() << "\n"
		<< msg.linkSpeed();
	return os;
}
//
//
//============================================================================
ostream& operator<<(ostream& os, const BUPMsg::FallbackEndMsg& msg) {
	os << static_cast<const BUPMsg::ReqHeader&>(msg) << "<" << msg.key() << "\n" << msg.cpId();
	return os;
}
/* Faked Key
//
//
//============================================================================
ostream& operator<<(ostream& os, const BUPMsg::FallbackEndMsg_V4& msg) {
	os << static_cast<const BUPMsg::ReqHeader&>(msg) << "<" << msg.key() << "\n" << msg.cpId();
	return os;
}
*/
//
//
//============================================================================
ostream& operator<<(ostream& os, const BUPMsg::FallbackEndRspMsg& msg) {
	os << static_cast<const BUPMsg::RspHeader&>(msg);
	return os;
}


//
//
//============================================================================
ostream& operator<<(ostream& os, const BUPMsg::DumpBeginMsg& msg) {
	os << static_cast<const BUPMsg::ReqHeader&>(msg) << "<" << msg.extent() << ">"
		 << msg.sbcId() << "\n" << msg.cpId();
	return os;
}
//
//
//============================================================================
ostream& operator<<(ostream& os, const BUPMsg::DumpBeginRspMsg& msg) {
	os << static_cast<const BUPMsg::RspHeader&>(msg)
		<< msg.key() << "\n"
		<< msg.files()  << "\n"
		<< msg.linkSpeed();
	return os;
}
//
//
//============================================================================
ostream& operator<<(ostream& os, const BUPMsg::DumpBeginRspMsg_V4& msg) {
	os << static_cast<const BUPMsg::RspHeader&>(msg) << "\n"
		<< msg.key() << "\n"
		<< msg.files()  << "\n"
		<< msg.linkSpeed();
	return os;
}
//
//
//============================================================================
ostream& operator<<(ostream& os, const BUPMsg::DumpEndMsg& msg) {
	os << static_cast<const BUPMsg::ReqHeader&>(msg) << "<" << msg.key() << "\n" << msg.cpId();
	return os;
}
//
//
//============================================================================
ostream& operator<<(ostream& os, const BUPMsg::DumpEndRspMsg& msg) {
	os << static_cast<const BUPMsg::RspHeader&>(msg);
	return os;
}
//
//
//============================================================================
ostream& operator<<(ostream& os, const BUPMsg::ReadConfMsg& msg) {
	os << static_cast<const BUPMsg::ReqHeader&>(msg) << "\n" << msg.cpId();
	return os;
}
//
//
//============================================================================
ostream& operator<<(ostream& os, const BUPMsg::ReadConfRspMsg& msg) {
	os << static_cast<const BUPMsg::RspHeader&>(msg) << "<" << msg.supervisionTime() << ">" ;
	return os;
}
//
//
//============================================================================
ostream& operator<<(ostream& os, const BUPMsg::ReadCmdLogConfMsg& msg) {
	os << static_cast<const BUPMsg::ReqHeader&>(msg) << "\n" << msg.cpId();
	return os;
}
//
//
//============================================================================
ostream& operator<<(ostream& os, const BUPMsg::ReadCmdLogConfRspMsg& msg) {
	os << static_cast<const BUPMsg::RspHeader&>(msg) << "<" << msg.cmdLogHandling() << ">";
	return os;
}
//
//
//============================================================================
ostream& operator<<(ostream& os, const BUPMsg::ReadCmdLogDataMsg& msg) {
	os << static_cast<const BUPMsg::ReqHeader&>(msg) << "\n" << msg.cpId();
	return os;
}
//
//
//============================================================================
ostream& operator<<(ostream& os, const BUPMsg::ReadCmdLogDataRspMsg& msg) {
	os << static_cast<const BUPMsg::RspHeader&>(msg) << "<" << msg.currCmdLog() << ">";
	return os;
}
//
//
//============================================================================
ostream& operator<<(ostream& os, const BUPMsg::WriteCmdLogDataMsg& msg) {
	os << static_cast<const BUPMsg::ReqHeader&>(msg) << "<" << msg.currCmdLog() << ">" << "\n" << msg.cpId();
	return os;
}
//
//
//============================================================================
ostream& operator<<(ostream& os, const BUPMsg::WriteCmdLogDataRspMsg& msg) {
	os << static_cast<const BUPMsg::RspHeader&>(msg);
	return os;
}
//
//
//============================================================================
ostream& operator<<(ostream& os, const BUPMsg::SBCSwitchMsg& msg) {
    os << static_cast<const BUPMsg::ReqHeader&>(msg) << "<" << msg.operation()
        << "><" << msg.range() << ">" << "\n" << msg.cpId();
    return os;
}

//
//
//============================================================================
ostream& operator<<(ostream& os, const BUPMsg::SBCSwitchRspMsg& msg) {
	os << static_cast<const BUPMsg::RspHeader&>(msg);
	return os;
}
//
//
//============================================================================
ostream& operator<<(ostream& os, const BUPMsg::ListSBCsMsg& msg) {
	os << static_cast<const BUPMsg::ReqHeader&>(msg) << "<" << msg.firstSequence()
		<< "><" << msg.range() << ">" << "\n" << msg.cpId();
	return os;
}
//
//
//============================================================================
ostream& operator<<(ostream& os, const BUPMsg::ListSBCsRspMsg& msg) {
	os << static_cast<const BUPMsg::RspHeader&>(msg) << "<" << msg.nofSBCs()
		<< ">\n" << setfill('0');
	const u_int16* item = msg.sbc();
	for(int i = 0; i < SBCId::MAX_NOF_SBCS; ++i) {
		os << "<" << setw(3) << item[i] << ">";
		if(i % 10 == 9)
			os << "\n";
	}
	os << setfill(' ');
	return os;
}

//
//
//============================================================================
ostream& operator<<(ostream& os, const BUPMsg& msg) {
    const u_int8* ui = reinterpret_cast<const u_int8*>(msg.addr()); // need cast for print
    os << setfill('0') << hex;
    for(int i = 0; i < BUPMsg::MSG_SIZE_IN_BYTES; ++i) {
        os << setw(2) << static_cast<u_int16>(ui[i]);
        if(i % 40 == 39)
            os << "\n";
        else
            if(i % 4 == 3)
                os << " ";
    }
    os << setfill(' ') << dec;
    return os;
}

//
//
//============================================================================
ostream& operator<<(ostream& os, const BUPMsg::VerifySBCDataMsg& msg) {
	os << static_cast<const BUPMsg::ReqHeader&>(msg)
		<< msg.sddCheck() << "><"
		<< msg.ldd1Check() << "><"
		<< msg.ldd2Check() << "><"
		<< msg.psCheck() << "><"
		<< msg.rsCheck() << ">"
		<< msg.sbcId() << "\n" << msg.cpId();
	return os;
}
//
//
//============================================================================
ostream& operator<<(ostream& os, const BUPMsg::VerifySBCDataRspMsg& msg) {
	os << static_cast<const BUPMsg::RspHeader&>(msg) << "<"
		<< msg.sddStatus() << "><"
		<< msg.ldd1Status() << "><"
		<< msg.ldd2Status() << "><"
		<< msg.psStatus() << "><"
		<< msg.rsStatus() << ">";
	return os;
}
//
//
//============================================================================
ostream& operator<<(ostream& os, const BUPMsg::VerifySBCDataMsg2& msg) {
	os << static_cast<const BUPMsg::ReqHeader&>(msg)
		<< msg.sddCheck() << "><"
		<< msg.ldd1Check() << "><"
		<< msg.ldd2Check() << "><"
		<< msg.psCheck() << "><"
		<< msg.rsCheck() << ">"
		<< msg.ps2Check() << "><"
		<< msg.rs2Check() << ">"
		<< msg.sbcId() << "\n" << msg.cpId();
	return os;
}
//
//
//============================================================================
ostream& operator<<(ostream& os, const BUPMsg::VerifySBCDataRspMsg2& msg) {
	os << static_cast<const BUPMsg::RspHeader&>(msg) << "<"
		<< msg.sddStatus() << "><"
		<< msg.ldd1Status() << "><"
		<< msg.ldd2Status() << "><"
		<< msg.psStatus() << "><"
		<< msg.rsStatus() << ">"
		<< msg.ps2Status() << "><"
		<< msg.rs2Status() << ">";
	return os;
}
//
//
//
//============================================================================
ostream& operator<<(ostream& os, const BUPMsg::SBCInfoMsg& msg) {
	os << static_cast<const BUPMsg::ReqHeader&>(msg) << msg.sbcId() << "\n" << msg.cpId();
	return os;
}
//
//
//============================================================================
ostream& operator<<(ostream& os, const BUPMsg::SBCInfoRspMsg& msg) {
	os << static_cast<const BUPMsg::RspHeader&>(msg)
		<< msg.buinfoVersion() << "\n"
		<< msg.apzVersion() << "\n"
		<< msg.exchangeId() << "\n"
		<< "<" << msg.generation() << ">\n"
		<< msg.sddInfo() << "\n"
		<< msg.ldd1Info() << "\n"
		<< msg.ldd2Info() << "\n"
		<< msg.psInfo() << "\n"
		<< msg.rsInfo() << "\n<"
		<< msg.cmdLogFile1() << ">\n<"
		<< msg.cmdLogFile2() << ">";
	return os;
}

//
//
//============================================================================
ostream& operator<<(ostream& os, const BUPMsg::SBCInfoRspMsg2& msg) {
	os << static_cast<const BUPMsg::RspHeader&>(msg)
		<< msg.buinfoVersion() << "\n"
		<< msg.apzVersion() << "\n"
		<< msg.exchangeId() << "\n"
		<< "<" << msg.generation() << ">\n"
		<< msg.sddInfo() << "\n"
		<< msg.ldd1Info() << "\n"
		<< msg.ldd2Info() << "\n"
		<< msg.psInfo() << "\n"
		<< msg.rsInfo() << "\n"
		<< msg.ps2Info() << "\n"
		<< msg.rs2Info() << "\n"
		<< "<" << msg.dsGeneration() << ">\n"
		<< "<" << msg.ldd1OnMProfile() << ">\n"
		<< "<" << msg.ldd1APZProfile() << ">\n"
		<< "<" << msg.ldd1APTProfile() << ">\n"
		<< "<" << msg.ldd1CpId() << ">\n"
		<< "<" << msg.ldd2OnMProfile() << ">\n"
		<< "<" << msg.ldd2APZProfile() << ">\n"
		<< "<" << msg.ldd2APTProfile() << ">\n"
		<< "<" << msg.ldd2CpId() << ">\n<"
		<< msg.cmdLogFile1() << ">\n<"
		<< msg.cmdLogFile2() << ">";
	return os;
}
//
//
//============================================================================
ostream& operator<<(ostream& os, const BUPMsg::WriteALogDataMsg& msg) {
	os << static_cast<const BUPMsg::ReqHeader&>(msg) 
		<< "\n" << "<" << msg.cpSystem() << ">"
		<< "<" << msg.cpId() << ">"
		<< msg.m_ALogData;
	return os;
}
//
//
//============================================================================
ostream& operator<<(ostream& os, const BUPMsg::WriteALogDataRspMsg& msg) {
	os << static_cast<const BUPMsg::RspHeader&>(msg);
	return os;
}

