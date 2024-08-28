/*
NAME
	File_name:UnknownJob.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

The Copyright to the computer program(s) herein is the property of Ericsson
Utvecklings AB, Sweden.
The program(s) may be used and/or copied only with the written permission from
Ericsson Utvecklings AB or in accordance with the terms and conditions
stipulated in the agreement/contract under which the program(s) have been
supplied.

DESCRIPTION
	Handles unknown primitives received in BUP.

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
	2002-03-13 by U/Y/SF Anders Gillgren (qabgill, tag :ag)


SEE ALSO
	-

Revision history
----------------
2002-03-13 qabgill Created
2002-11-04 uablan  Add ACS_Trace
*/

#include "UnknownJob.h"

#include "EventReporter.h"
#include "CPS_BUSRV_Trace.h"
#include "BUPFunx.h"
#include "BUService.h"

#include <vector>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <linux/sockios.h>

#include <vector>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sstream>
#include <iostream>

class BUSRV_Socket_Test
{
public:
	BUSRV_Socket_Test() {}
	void closeSockets(size_t n);
	void createSockets(size_t n);
	void openFiles(size_t n);
	void closeFiles(size_t n);
	void restartComponent();

	size_t numberOfSockets() {
		return mSockets.size();
	}

	void startTest(CPID cpId, size_t n);

private:
	vector<int> mSockets;
	vector<int>	mFiles;
};

static BUSRV_Socket_Test s_sockets;
extern BUService* g_pService;

void BUSRV_Socket_Test::restartComponent()
{
	newTRACE((LOG_LEVEL_INFO, "BUSRV_Socket_Test::restartComponent()", 0));
	if (g_pService) {
		std::ostringstream ss;
		ss << "Reporting component error to AMF in file <" << __FILE__ << "> at line <";
		ss << __LINE__ << ">";
    	TRACE((LOG_LEVEL_FATAL, "%s", 0, ss.str().c_str()));

    	//There is no resources to run this reporter
		//EventReporter::instance().write("Reporting component error to AMF");
		ACS_APGCC_ReturnType res = g_pService->componentReportError(ACS_APGCC_COMPONENT_RESTART);
		TRACE((LOG_LEVEL_FATAL, "Reporting component error to AMF retunrs <%d>", 0, res));

		// Give it a go
		EventReporter::instance().write(ss.str().c_str());
	}
}

void BUSRV_Socket_Test::closeFiles(size_t n)
{
	newTRACE((LOG_LEVEL_INFO, "BUSRV_Socket_Test::closeFiles(%d)", 0, n));
	size_t count = 0;
	int res;
	if (!n)
		n = mFiles.size();
	for (vector<int>::iterator it = mFiles.begin(); it != mFiles.end(); ++it) {
		res = close(*it);
		if (res < 0) {
			TRACE((LOG_LEVEL_ERROR, "BUSRV_Socket_Test::closeFiles failed with code <%d> for fd <%d>)", 0, res, *it));
		}
		//else {
		//	TRACE(("BUSRV_Socket_Test::closeFiles of for fd <%d>)", 0, *it));
		//}
		if (++count == n)
			break;
	}
	TRACE((LOG_LEVEL_INFO, "Total files closed are <%d>", 0, count));

}

void BUSRV_Socket_Test::openFiles(size_t n)
{
	newTRACE((LOG_LEVEL_INFO, "BUSRV_Socket_Test::openFiles(%d)", 0, n));
    size_t count;
    int fd;
    string name = "/data/cps/data/busrv/busrv.dat";
    for(count = 0; count < n; ++count) {
        fd =  open(name.c_str(), O_RDONLY);
        if (fd < 0) break;
        mFiles.push_back(fd);
    }

	TRACE((LOG_LEVEL_INFO, "Total openFiles created are <%d>", 0, count));
}

void BUSRV_Socket_Test::createSockets(size_t n)
{
	newTRACE((LOG_LEVEL_INFO, "BUSRV_Socket_Test::createSockets(%d)", 0, n));
	int fd = 0;
	int count = 0;
	for (size_t i = 0; i < n; ++i) {
		fd = socket(AF_INET, SOCK_DGRAM, 0);
		if (fd < 0)
			break;

		mSockets.push_back(fd);
		if (++count % 100 == 0) {
			TRACE((LOG_LEVEL_INFO, "wait ...", 0));
			sleep(1);
		}
	}
	TRACE((LOG_LEVEL_INFO, "Total sockets created are <%d>", 0, count));
}
void BUSRV_Socket_Test::closeSockets(size_t n)
{
	newTRACE((LOG_LEVEL_INFO, "BUSRV_Socket_Test::closeSockets()", 0));
	size_t count = 0;
	if (!n)
		n = mSockets.size();
	for (vector<int>::iterator it = mSockets.begin(); it != mSockets.end(); ++it) {
		close(*it);
		if (++count % 100 == 0) {
			TRACE((LOG_LEVEL_INFO, "wait ...", 0));
			sleep(1);
		}
		if (count == n)
			break;
	}
	TRACE((LOG_LEVEL_INFO, "Total sockets removed are <%d>", 0, count));
}
void BUSRV_Socket_Test::startTest(CPID cpId, size_t n)
{
	newTRACE((LOG_LEVEL_INFO, "BUSRV_Socket_Test::startTest(%d, %d)", 0, cpId, n));

	TRACE((LOG_LEVEL_INFO, "Executing Test Tool Hack", 0));
	switch (cpId) {
	case 59999:
		s_sockets.createSockets(n);
		break;
	case 59998:
		s_sockets.closeSockets(n);
		break;
	case 59995:
		s_sockets.openFiles(n);
		break;
	case 59994:
		s_sockets.closeFiles(n);
		break;
	case 49876:
		s_sockets.restartComponent();
		break;
	default:;
	}
}

//
// execute
//===========================================================================
void UnknownJob::execute() {

	newTRACE((LOG_LEVEL_INFO, "UnknownJob::execute()", 0));

    // rsp primitive = incoming primitive
	u_int32 unkPrimitive = msg().msgHeader()->primitive();	
	u_int32 unkPrimitiveRsp = unkPrimitive;
	u_int32 transNum = msg().msgHeader()->transNum(); 
	u_int16 tvm = msg().requestHeader()->version().major();
	m_exitcode = BUPMsg::UNKNOWN_PRIMITIVE;
	
	try {
		// plus one if even (all rsp primitives are odd numbers)
		if(!(unkPrimitiveRsp % 2))
			unkPrimitiveRsp++;
		
		// The only action for an unknown message
		//===================================================================
		if (verifyVersion()) {
			// Only for Coverity check; This is a dogma
		}
	}
	catch(...) {
		EventReporter::instance().write("Internal error when receiving unknown BUP primitive");
		m_exitcode = BUPMsg::INTERNAL_ERROR;
	}

	msg().reset();
	// create response in the msg buffer (re-use)
	new (msg().addr()) BUPMsg::RspHeader(static_cast<BUPMsg::PRIMITIVE>(unkPrimitiveRsp), transNum, exitcode());
	TRACE((LOG_LEVEL_INFO, "UnknownRspMsg: TransNum = %d exitcode = %d Version Major = %d, Msg Id=%d, Rsp Msg Id=%d", 0,
			transNum, exitcode(), tvm, unkPrimitive, unkPrimitiveRsp));
}
//
//
//===========================================================================
void UnknownTestJob::execute() {

	newTRACE((LOG_LEVEL_INFO, "UnknownTestJob::execute()", 0));

    // rsp primitive = incoming primitive
	u_int32 unkPrimitive = msg().msgHeader()->primitive();
	u_int32 unkPrimitiveRsp = unkPrimitive;
	u_int32 transNum = msg().msgHeader()->transNum();
	u_int16 tvm = msg().requestHeader()->version().major();
	m_exitcode = BUPMsg::UNKNOWN_PRIMITIVE;
	const BUPMsg::TestMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::TestMsg>(msg());
	CPID cpIdTest = cmd->cpId();
	size_t nSockets = cmd->sbcId().id();


	try {
		// plus one if even (all rsp primitives are odd numbers)
		if(!(unkPrimitiveRsp % 2))
			unkPrimitiveRsp++;

		// Start out of band test utility; which will be moved later
		s_sockets.startTest(cpIdTest, nSockets);

		// The only action for an unknown message
		//===================================================================
		if (verifyVersion()) {
			// Only for Coverity check; This is a dogma
		}
	}
	catch(...) {
		EventReporter::instance().write("Internal error when receiving unknown BUP primitive");
		m_exitcode = BUPMsg::INTERNAL_ERROR;
	}

	msg().reset();
	// create response in the msg buffer (re-use)
	new (msg().addr()) BUPMsg::RspHeader(static_cast<BUPMsg::PRIMITIVE>(unkPrimitiveRsp), transNum, exitcode());
	TRACE((LOG_LEVEL_INFO, "UnknownRspMsg: TransNum = %d exitcode = %d Version Major = %d, Msg Id=%d, Rsp Msg Id=%d", 0,
			transNum, exitcode(), tvm, unkPrimitive, unkPrimitiveRsp));
}
//
//
//===========================================================================

