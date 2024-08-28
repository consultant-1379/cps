/*
  NAME
  File_name:UnknownJob.cpp

  COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

  The Copyright to the computer program(s) herein is the property of Ericsson
  Utvecklings AB, Sweden.
  The program(s) may be used and/or copied only with the written permission from
  Ericsson Utvecklings AB or in accordance with the terms and conditions
  stipulated in the agreement/contract under which the program(s) have been
  supplied.

  DESCRIPTION
  Handles unknown primitives received in FCP.

  DOCUMENT NO
  190 89-CAA 109 0670

  AUTHOR
  2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on UnknownJob.cpp of FCSRV in Windows


  SEE ALSO
  -

  Revision history
  ----------------


*/


#include "UnknownJob.h"

#include "EventReporter.h"
 #include "CPS_FCSRV_Trace.h"
//
// execute
//===========================================================================
void UnknownJob::execute() {

	newTRACE((LOG_LEVEL_INFO, "UnknownJob::execute()", 0));

	u_int32 unkPrimitiveRsp = 0;
	u_int32 transNum = 0;
	m_exitcode = FCPMsg::UNKNOWN_PRIMITIVE;
	try {
		// rsp primitive = incoming primitive
		unkPrimitiveRsp = msg().msgHeader()->primitive();
		// plus one if even (all rsp primitives are odd numbers)
		if(!(unkPrimitiveRsp % 2))
			unkPrimitiveRsp++;

		transNum = msg().msgHeader()->transNum(); // save for response
		// init stuff
		//===================================================================
		//why do we need to verify version anyway? Obviously, an unknown primitive won't be
		//handled by the server. Leaving verifyVersion() will alter the response primitive.
		//verifyVersion();
	}
	catch(...) {
		EventReporter::instance().write("Internal error when receiving unknown FCP primitive");
		m_exitcode = FCPMsg::INTERNAL_ERROR;
	}

	new (msg().addr()) FCPMsg::RspHeader(static_cast<FCPMsg::PRIMITIVE>(unkPrimitiveRsp),
                          transNum, exitcode());

	TRACE((LOG_LEVEL_INFO, "UnknownJob::execute() returns", 0));
}
//
//
//===========================================================================

