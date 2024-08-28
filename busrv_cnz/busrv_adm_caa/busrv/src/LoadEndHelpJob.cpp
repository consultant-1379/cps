/*
NAME
   File_name:LoadEndHelpJob.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2001. All rights reserved.

The Copyright to the computer program(s) herein is the property of Ericsson
Utvecklings AB, Sweden.
The program(s) may be used and/or copied only with the written permission from
Ericsson Utvecklings AB or in accordance with the terms and conditions
stipulated in the agreement/contract under which the program(s) have been
supplied.

.DESCRIPTION
   

DOCUMENT NO
   190 89-CAA 109 

AUTHOR 
   

SEE ALSO 
   -

Revision history
----------------
	2010-04-15 xdtthng SSI added
	2012-12-24 xdtthng Updated for APG43L BC
*/

#include "LoadEndHelpJob.h"

#include <assert.h>

#include "buap.h"
#include "Config.h"
#include "DataFile.h"
#include "CPS_BUSRV_Trace.h"
#include "EventReporter.h"
//#include "Win32Exception.h"


//
// ctor
//----------------------------------------------------------------------------
LoadEndHelpJob::LoadEndHelpJob() : JobBase(true) {
    newTRACE((LOG_LEVEL_INFO, "LoadEndHelpJob::LoadEndHelpJob()", 0));
}
//
// pure virtual in base, called when the Job is "run"
//----------------------------------------------------------------------------
void LoadEndHelpJob::executeSpecial(u_int16 sbc) {

	newTRACE((LOG_LEVEL_INFO, "LoadEndHelpJob::executeSpecial()", 0));

	// NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
	// Backup Protocol Interaction
    // 
    // Determine conditions to flush the DumpEnd queue
	//u_int16 loadingSbcId = sbc;

    DataFile::BUACTREC dump;
    DataFile::BUACTREC_ITERATOR it;
	DataFile::dumpEndQueued(dump);
    
    // If there is no dump in progress then just return
	if (dump.empty()) {
		TRACE((LOG_LEVEL_INFO, "JQS State is %d", 0, m_state.m_next));
		TRACE((LOG_LEVEL_INFO, "LoadEndHelpJob returns when no pending dump exists", 0));
        return;
	}
        
    // If thee is no Dumping on the same sbc then just return
    for (it = dump.begin(); it != dump.end(); ++it) {
        if (it->first == sbc) 
            break;
    }
	if (it == dump.end()) {
		TRACE((LOG_LEVEL_INFO, "JQS State is %d", 0, m_state.m_next));
		TRACE((LOG_LEVEL_INFO, "LoadEndHelpJob returns when pending dump exists, but not on same sbc", 0));
        return;
	}
    
	// At this point, it is certain that DumpEnd has arrived and been queued
	// If no more loading then flush the DumpEnd queue    
    DataFile::BUACTREC load;
	DataFile::BitVector& loadVector = DataFile::readLoadVector();
	if (loadVector != 0ULL)
		DataFile::onGoingLoad(load);

    if (load.empty()) {     
		TRACE((LOG_LEVEL_INFO, "JQS State is %d", 0, m_state.m_next));
		TRACE((LOG_LEVEL_INFO, "LoadEndHelpJob flush the queue when no more loading exists", 0));
        resetState(Job::JQS_FLUSH, sbc);
        return;
    }
    
    // If thee is no more Loading on the same sbc then flust the DumpEnd queue
    for (it = load.begin(); it != load.end(); ++it) {
        if (it->first == sbc)
            break;
    }
    if (it == load.end()) {
		TRACE((LOG_LEVEL_INFO, "JQS State is %d", 0, m_state.m_next));
		TRACE((LOG_LEVEL_INFO, "LoadEndHelpJob flush the queue when no more loading exists on same sbc", 0));
        resetState(Job::JQS_FLUSH, sbc);
        return;
    }
	TRACE((LOG_LEVEL_INFO, "JQS State is %d", 0, m_state.m_next));
	TRACE((LOG_LEVEL_INFO, "The last statement of LoadEndHelpJob", 0));
	//
    //
    // NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
}
