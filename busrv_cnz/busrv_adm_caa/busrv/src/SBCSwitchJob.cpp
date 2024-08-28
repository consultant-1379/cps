/*
NAME
   File_name:SBCSwitchJob.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2002-2023. All rights reserved.

   The Copyright to the computer program(s) herein is the property of Ericsson
   Utvecklings AB, Sweden.
   The program(s) may be used and/or copied only with the written permission from
   Ericsson Utvecklings AB or in accordance with the terms and conditions
   stipulated in the agreement/contract under which the program(s) have been
   supplied.

DESCRIPTION
   Executes the BUP command SBC_SWITCH.

DOCUMENT NO
   190 89-CAA 109 0387

AUTHOR
   2002-02-05 by UAB/KY/SK Anders Gillgren (qabgill, tag :ag)

SEE ALSO


Revision history
----------------
2002-02-05 qabgill Created
2002-10-31 uablan  Added ACS_Trace
2006-08-21 uablan  Change way of switching files in SYTUC.
2006-08-09 uablan  Add FMS_PHYSICAL_FILE_ERROR in try/catch switch.
2010-05-23 xdtthng Modified for SSI
2010-12-14 xquydao Update for APG43L project
2011-09-14 xdtthng 43L, Total usage of FMS
2023-01-01 xkomala Added checks for valid cpid for TR IA20800
*/


#include "SBCSwitchJob.h"

#include "CPS_BUSRV_Trace.h"
#include "BUPFunx.h"
#include "CodeException.h"
#include "Config.h"
#include "DataFile.h"
#include "EventReporter.h"
#include "FileFunx.h"
#include "SBC.h"
#include "SBCList.h"
#include "TmpCurrDir.h"
#include "LinuxException.h"

#ifndef LOCAL_BUILD
#include "EnvFMS.h"
#include "fms_cpf_file.h"
#endif

#include <functional>
#include <algorithm>
#include <boost/scoped_array.hpp>
#include <boost/bind.hpp>
//
//
//====================================================================
void SBCSwitchJob::execute() {

	newTRACE((LOG_LEVEL_INFO, "SBCSwitchJob::execute()", 0));

	u_int32 transNum = 0;
	CPID cpId = 0xFFFF; // default ONE CP system
	const BUPMsg::SBCSwitchMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::SBCSwitchMsg>(msg());
	transNum = cmd->transNum(); 
	
	try {
		// init stuff
		//===================================================================
		if(verifyVersion()) {

			//check if ONE CP System
			if (cmd->cpSystem() != BUPMsg::ONE_CP_SYSTEM)
			{
				cpId = cmd->cpId();

				//TR_IA20800 START
				if(!(Config::instance().isValidSpxCpid(cpId)))
				{
					TRACE((LOG_LEVEL_ERROR, "SBCSwitchJob, CpId received is <%d> ,throwing internal error", 0, cpId));
					throw BUPMsg::INTERNAL_ERROR;
				} //TR_IA20800 END
			}

			//
			// check rules
			//=================================================================
			if(cmd->range() != SBCId::FIRST &&
				cmd->range() != SBCId::SECOND)
				throw BUPMsg::PARAM_SBC_RANGE_INVALID;

			if (cmd->operation() != BUPMsg::HIGH_LOW &&
				 cmd->operation() != BUPMsg::ROLL_DOWN)
				 throw BUPMsg::PARAM_SWITCH_OPERATION_INVALID;

			//
			// switch files
			//=================================================================
			switchSBCs(cmd->range(), cmd->operation(), cpId);
		}
	}
	catch(BUPMsg::ERROR_CODE errcode) {
		TRACE((LOG_LEVEL_ERROR, "SBCSwitchJob::execute() BUPMsg::ERROR_CODE=%d ", 0, errcode));
		m_exitcode = errcode;
	}
#ifndef LOCAL_BUILD
	catch(FMS_CPF_Exception& x) {
    	if (isTRACEABLE()) {
			stringstream str;
			str << "SBCSwitchJob::execute(), FMS Errorcode : " << x.errorCode() << ", Text: " << x.errorText() << ", Details: "
		    	<< x.detailInfo() << endl;
			TRACE((LOG_LEVEL_ERROR, "%s", 0, str.str().c_str()));
		}
		switch (x.errorCode()) {
			case FMS_CPF_Exception::ACCESSERROR:
			case FMS_CPF_Exception::INVALIDFILE:	// This is a work around
				m_exitcode = BUPMsg::SBC_RESERVE_FAILED;
				break;
			default:
				m_exitcode = BUPMsg::INTERNAL_ERROR;
				break;
		}
	}
#endif
	catch(CodeException& x) {
		switch(x.errcode()) {
		case CodeException::CP_APG43_CONFIG_ERROR:
			TRACE((LOG_LEVEL_ERROR, "SBCSwitchJob::execute() CS error ", 0));
			m_exitcode = BUPMsg::INTERNAL_ERROR;
			break;
		default:
			m_exitcode = BUPMsg::INTERNAL_ERROR;
			TRACE((LOG_LEVEL_ERROR, "SBCSwitchJob: CodeExcpetion error code: %d", 0, x.errcode()));
			break;
		}
	}
	catch(const Exception& x) {
		EventReporter::instance().write(x);
		m_exitcode = BUPMsg::INTERNAL_ERROR;
	}
	catch(...) {
		EventReporter::instance().write("Unhandled internal error.");
		m_exitcode = BUPMsg::INTERNAL_ERROR;
	}

	msg().reset();

	// create response in the msg buffer (re-use)
	new (msg().addr()) BUPMsg::SBCSwitchRspMsg(transNum, exitcode());

	TRACE((LOG_LEVEL_INFO, "SBCSWitchJobRspMsg: TransNum = %d exitcode = %d", 0, transNum, exitcode()));
}
//
// Switch consequtive files within an SBC range.
// throws only BUPMsg::ERROR_CODE
//===========================================================================
void SBCSwitchJob::switchSBCs(SBCId::SBC_RANGE range, BUPMsg::SWITCH_OPERATION operation, CPID cpId) {

	newTRACE((LOG_LEVEL_INFO, "SBCSwitchJob::switchSBCs(%d, %d)", 0,range, operation));

	string dir = Config::instance().relvolumsw(cpId);
	TmpCurrDir currDir(dir); // may throw Linux error

	// get temporary dir id (special, weird rules)
	//----------------------------
	SBCId tmpId(range == SBCId::FIRST ? 'X' : 'Y');

	// Check if RELFSWX/Y exists
	//--------------------------

#ifdef LOCAL_BUILD					
	dir = Config::instance().sbcDir(tmpId, cpId).c_str();
	if(FileFunx::dirExists(dir.c_str())) 
#else
	if (EnvFMS::dirExists(tmpId, cpId))
#endif	
		throw BUPMsg::SWITCH_TEMP_SBC_EXIST;

	// Get a list of SBC Id's for the consecutive files in FFR or SFR.
	//----------------------------------------------------------------
	SBCList sbcList(range, true, cpId);

	// Check count
	if(sbcList.count() <= 1)
		throw BUPMsg::SWITCH_INSUFFICIENT_NOF_SBCS;

	// Check low id
	if(!sbcList.sbcId(0).firstId(range))
		throw BUPMsg::SWITCH_LOW_ID_NOT_BASE;

	if(operation == BUPMsg::HIGH_LOW)
		switchHighLow(tmpId, sbcList, cpId);
	else
		switchRolldown(tmpId, sbcList, cpId);
}
//
// (SYNIC)
//===========================================================================
void SBCSwitchJob::switchHighLow(SBCId tmpId, const SBCList& sbcList, CPID cpId) {

	newTRACE((LOG_LEVEL_INFO, "SBCSwitchJob::switchHighLow(%s, SBCList)", 0, tmpId.dirName().c_str()));

	try {

		SBC sbcLow(sbcList.sbcId(0), cpId);
		SBC sbcHigh(sbcList.sbcId(sbcList.count() - 1), cpId);
		
		// Check buinfo
		if(sbcHigh.biQuery(cpId) != SBC::BQ_NO_ERROR)
			throw BUPMsg::SWITCH_HIGH_BACKUPINFO_BAD;

		// it is possible to switch in an SBC to a faulty RELFSW0/100
		SBC::BI_QUERY query = sbcLow.biQuery(cpId);
		if(query != SBC::BQ_FILE_NOT_FOUND) {
			// but if valid, check that the high SBC is greater than the low SBC
			if(query == SBC::BQ_NO_ERROR) {
				if(sbcHigh.cmpDateTime(sbcLow, cpId) != 1)
					throw BUPMsg::SWITCH_SBC_DATES_ILLEGAL_FOR_OP;
			}
			// else - file found, but invalid. shift it anyway. Strange!!!
		}

		// Reserve the low and the high SBC
		//----------------------------------

		if(!sbcLow.reserve(cpId)) {
			if(!sbcLow.dirExists(cpId))
				throw BUPMsg::SWITCH_LOW_SBC_NOT_FOUND;
			else
				throw BUPMsg::SBC_RESERVE_FAILED;
		}

		if(!sbcHigh.reserve(cpId)) {
			if(!sbcHigh.dirExists(cpId))
				throw BUPMsg::SWITCH_HIGH_SBC_NOT_FOUND;
			else
				throw BUPMsg::SBC_RESERVE_FAILED;
		}

		// switch the files
		//--------------------------
		sbcHigh.swap(sbcLow, tmpId, false, cpId);
	}
#ifndef LOCAL_BUILD					
	catch(FMS_CPF_Exception& x) {
    	if (isTRACEABLE()) {
			stringstream str;
			str << "SBCSwitchJob::switchHighLow(), FMS Errorcode : " << x.errorCode() << ", Text: " << x.errorText() << ", Details: "
		    	<< x.detailInfo() << endl;
			TRACE((LOG_LEVEL_ERROR, "%s", 0, str.str().c_str()));
		}
		throw;
	}
#endif
	catch(const CodeException& x) {
		switch(x.errcode()) {
		case CodeException::REQ_SBC_MISSING:
			EventReporter::instance().write(x);
			throw BUPMsg::INTERNAL_ERROR; //:lan  - this should not happen since both sbc are reserved before move!!
		case CodeException::SBC_RESERVE_FAILED:
			throw BUPMsg::SBC_RESERVE_FAILED;
		case CodeException::FMS_PHYSICAL_FILE_ERROR:
			throw BUPMsg::FMS_PHYSICAL_FILE_ERROR;
		default:
			throw BUPMsg::INTERNAL_ERROR;
		}
	}
	catch(const LinuxException& x) {
		assert(!"Unreachable code!?");
		EventReporter::instance().write(x);
		throw;
	}
	catch(BUPMsg::ERROR_CODE x) { // to avoid catch(...)
		throw;
	}
	catch(...) {
		EventReporter::instance().write("Unhandled exception");
		throw BUPMsg::INTERNAL_ERROR;
	}
}
//
// (SYTUC)
//===========================================================================
void SBCSwitchJob::switchRolldown(SBCId tmpId, const SBCList& sbcList, CPID cpId) {

	newTRACE((LOG_LEVEL_INFO, "SBCSwitchJob::switchRolldown(%s, SBCList)", 0,tmpId.dirName().c_str()));

	//SBC* sbc = new SBC[sbcList.count()];
	boost::scoped_array<SBC> sbc(new SBC[sbcList.count()]);
	try {

		//init the SBCs
		u_int32 i = 0;
		for(; i < sbcList.count(); ++i)
			sbc[i].id(sbcList.sbcId(i), cpId);

		//=====================================================================
		// check buinfo
		if(sbc[0].biQuery(cpId) != SBC::BQ_NO_ERROR)
			throw BUPMsg::SWITCH_LOW_BACKUPINFO_BAD;
		if(sbc[sbcList.count() - 1].biQuery(cpId) != SBC::BQ_NO_ERROR)
			throw BUPMsg::SWITCH_HIGH_BACKUPINFO_BAD;

		// Check that the highest SBC is younger than the low SBC
		if(sbc[sbcList.count() - 1].cmpDateTime(sbc[0], cpId) != 1)
			throw BUPMsg::SWITCH_SBC_DATES_ILLEGAL_FOR_OP;

		// reserve all SBC dirs
		for(i = 0; i < sbcList.count(); ++i) {
			if(!sbc[i].reserve(cpId))
				throw BUPMsg::SBC_RESERVE_FAILED;
		}
	
		// RELFSW0 -> RELFSWX/Y
		sbc[0].move(tmpId, false,cpId);
		// RELFSWn -> RELFSW0
		sbc[sbcList.count() - 1].move(sbcList.sbcId(0), false, cpId);
		// RELFSWn-1 -> RELFSWn
		for(i = sbcList.count() - 1; i > 1; --i)
			sbc[i - 1].move(sbcList.sbcId(i), false, cpId);
		// RELFSWX/Y -> RELFSW1
		sbc[0].move(sbcList.sbcId(1), false, cpId);
	}
#ifndef LOCAL_BUILD					
	catch(FMS_CPF_Exception& x) {
    	if (isTRACEABLE()) {
			stringstream str;
			str << "SBCSwitchJob::switchRolldown(), FMS Errorcode : " << x.errorCode() << ", Text: " << x.errorText() << ", Details: "
		    	<< x.detailInfo() << endl;
			TRACE((LOG_LEVEL_ERROR, "%s", 0, str.str().c_str()));
		}
		throw;
	}
#endif
	catch(const CodeException& x) {
		switch(x.errcode()) {
		case CodeException::REQ_SBC_MISSING:
				throw BUPMsg::SBC_NOT_FOUND;
			case CodeException::SBC_RESERVE_FAILED:
				throw BUPMsg::SBC_RESERVE_FAILED;
			case CodeException::FMS_PHYSICAL_FILE_ERROR:
				throw BUPMsg::FMS_PHYSICAL_FILE_ERROR;
			default:
				throw BUPMsg::INTERNAL_ERROR;
		}
	}
	catch(const LinuxException& x) {
		throw;
	}
	catch(BUPMsg::ERROR_CODE x) { // to avoid catch(...)
		throw;
	}
	catch(...) {
		EventReporter::instance().write("Unhandled exception");
		throw BUPMsg::INTERNAL_ERROR;
	}
}

///////////////////////////////////////////////////////////////////////
//
//
//====================================================================
void SBCSwitchJob_V4::execute() {

    newTRACE((LOG_LEVEL_INFO, "SBCSwitchJob_V4::execute()", 0));

    const BUPMsg::SBCSwitchMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::SBCSwitchMsg>(msg());
    u_int32 transNum = cmd->transNum(); // save for response
    CPID cpId = cmd->cpId();
	const Config::CLUSTER tc;

    try {

    	//TR_IA20800 START
    	if(!(Config::instance().isValidBladeCpid(cpId)))
    	{
    		TRACE((LOG_LEVEL_ERROR, "SBCSwitchJob_V4, CpId received is <%d> , throwing internal error", 0, cpId));
    		throw BUPMsg::INTERNAL_ERROR;
    	} //TR_IA20800 END


        // init stuff
        //===================================================================
        if(verifyVersion()) {
            const BUPMsg::SBCSwitchMsg* cmd = BUPFunx::safe_bup_ptr_cast<BUPMsg::SBCSwitchMsg>(msg());
            transNum = cmd->transNum(); // save for response
            cpId = cmd->cpId();

			// Clear ongoing dumping or loading marks if time supervision has elapsed
			Config::instance().noOngoingDump(tc);
			Config::instance().noOngoingLoad(tc);
			//
            // check rules
            //=================================================================
			if(cmd->range() != SBCId::FIRST &&
				cmd->range() != SBCId::SECOND)
				throw BUPMsg::PARAM_SBC_RANGE_INVALID;

            if (cmd->operation() != BUPMsg::HIGH_LOW &&
                 cmd->operation() != BUPMsg::ROLL_DOWN)
                 throw BUPMsg::PARAM_SWITCH_OPERATION_INVALID;
    
			DataFile::startSbcSwitch(tc);

            //
            // switch files
            //=================================================================
            switchSBCs(cmd->range(), cmd->operation(), cpId); 
        }
    }
    catch(BUPMsg::ERROR_CODE errcode) {
        m_exitcode = errcode;
    }
    catch(CodeException& x) {
        switch(x.errcode()) {
        case CodeException::CP_APG43_CONFIG_ERROR:
            TRACE((LOG_LEVEL_ERROR, "SBCSwitchJob::execute() CS error ", 0));
            m_exitcode = BUPMsg::INTERNAL_ERROR;
            break;
        default:
            m_exitcode = BUPMsg::INTERNAL_ERROR;
            TRACE((LOG_LEVEL_ERROR, "SBCSwitchJob: CodeExcpetion error code: %d", 0, x.errcode()));
            break;
        }
    }
    catch(const Exception& x) {
        EventReporter::instance().write(x);
        m_exitcode = BUPMsg::INTERNAL_ERROR;        
    }
    catch(...) {
        EventReporter::instance().write("Unhandled internal error.");
        m_exitcode = BUPMsg::INTERNAL_ERROR;
    }

    msg().reset();
	DataFile::endSbcSwitch(tc);
    new (msg().addr()) BUPMsg::SBCSwitchRspMsg(transNum, exitcode());

	TRACE((LOG_LEVEL_INFO, "BUPMsg::SBCSwitchRspMsg TransNum = %d exitcode = %d", 0, transNum, exitcode()));
}
//
// Switch consequtive files within an SBC range.
// throws only BUPMsg::ERROR_CODE
//===========================================================================
void SBCSwitchJob_V4::switchSBCs(SBCId::SBC_RANGE range, 
								 BUPMsg::SWITCH_OPERATION operation,
								 CPID cpId) {

	newTRACE((LOG_LEVEL_INFO, "SBCSwitchJob_V4::switchSBCs(%d, %d)", 0, range, operation));

    Config::CLUSTER t;
    string dir = Config::instance().relvolumsw(t);
    TmpCurrDir currDir(dir); // may throw win32 error
    
    // get temporary dir id (special, weird rules)
    //----------------------------
    SBCId tmpId(range == SBCId::FIRST ? 'X' : 'Y');
   
    // Check if RELFSWX/Y exists
    //--------------------------
    dir = Config::instance().sbcDir(tmpId, t).c_str();

    if(FileFunx::dirExists(dir.c_str())) {
        throw BUPMsg::SWITCH_TEMP_SBC_EXIST;
    }
    
    // Get a list of SBC Id's for the consecutive files in FFR or SFR. 
    //------------------------------------------------
    SBCList sbcList(range, true, t);  // may throw codeException dir not found

    // Check count
    if(sbcList.count() <= 1)
        throw BUPMsg::SWITCH_INSUFFICIENT_NOF_SBCS;
    
    // Check low id
    if(!sbcList.sbcId(0).firstId(range))
        throw BUPMsg::SWITCH_LOW_ID_NOT_BASE;
    

	if(operation == BUPMsg::HIGH_LOW) 
        switchHighLow(tmpId, sbcList, cpId);
	
	else 
        switchRolldown(tmpId, sbcList);
	
}

//
// (SYNIC)
//===========================================================================
void SBCSwitchJob_V4::switchHighLow(SBCId tmpId, const SBCList& sbcList, CPID cpId) {

	newTRACE((LOG_LEVEL_INFO, "SBCSwitchJob_V4::switchHighLow(%s, SBCList)", 0, tmpId.dirName().c_str()));

    try {

		const Config::CLUSTER tn(cpId, true);
		SBC_V4 sbcLow(sbcList.sbcId(0), tn);
        SBC_V4 sbcHigh(sbcList.sbcId(sbcList.count() - 1), tn);

		// NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
		// BUP Interactions
		// Dumping and synic/sytuc are mutually exlusive. Don't have to check dumping
		// If sbcLow or sbcHigh is currently used for loading, the switch is rejected
		DataFile::BUACTREC load;
		DataFile::onGoingLoad(load);
		DataFile::BUACTREC_ITERATOR it;
		DataFile::BUACTREC_ITERATOR last = load.end();
		u_int16 sbc[2];
		sbc[0] = sbcLow.id().id();
		sbc[1] = sbcHigh.id().id();
		if (!load.empty()) {
			it = find_first_of(load.begin(), last, &sbc[0], &sbc[2],
					boost::bind<bool>(equal_to<int>(),
							boost::bind(&DataFile::BUPAIR::first, _1),
							_2));
			if (it < last ) {
				TRACE((LOG_LEVEL_WARN, "SBCSwitchJob_V4::switchHighLow: Switch rejected due to ongoing load", 0));
				throw BUPMsg::SWITCH_REJECTED_DUE_TO_ONGOING_LOAD;
			}
		}

		//-------------------------------------------------------
        if(sbcHigh.biQuery() != SBC_V4::BQ_NO_ERROR)
            throw BUPMsg::SWITCH_HIGH_BACKUPINFO_BAD;

        // it is possible to switch in an SBC to a faulty RELFSW0/100
        SBC_V4::BI_QUERY query = sbcLow.biQuery();
        if(query != SBC_V4::BQ_FILE_NOT_FOUND) {
            // but if valid, check that the high SBC is greater than the low SBC
            if(query == SBC_V4::BQ_NO_ERROR) {
                if(sbcHigh.cmpDateTime(sbcLow) != 1)
                    throw BUPMsg::SWITCH_SBC_DATES_ILLEGAL_FOR_OP;
            }
            // else - file found, but invalid. shift it anyway. Strange!!!
        }

        // Reserve the low and the high SBC
        //------------------------------------------------
        if(!sbcLow.reserve()) {
            if(!sbcLow.dirExists())
                throw BUPMsg::SWITCH_LOW_SBC_NOT_FOUND;
            else
                throw BUPMsg::SBC_RESERVE_FAILED;
        }

        if(!sbcHigh.reserve()) {
            if(!sbcHigh.dirExists())
                throw BUPMsg::SWITCH_HIGH_SBC_NOT_FOUND;
            else
                throw BUPMsg::SBC_RESERVE_FAILED;
        }

        // switch the files
        //--------------------------
        sbcHigh.swap(sbcLow, tmpId, false);
    }
    catch(const CodeException& x) {
        switch(x.errcode()) {
        case CodeException::REQ_SBC_MISSING:
            EventReporter::instance().write(x);
            throw BUPMsg::INTERNAL_ERROR; //:lan  - this should not happen since both sbc are reserved before move!!
        case CodeException::SBC_RESERVE_FAILED:
            throw BUPMsg::SBC_RESERVE_FAILED;
        case CodeException::FMS_PHYSICAL_FILE_ERROR:
            throw BUPMsg::FMS_PHYSICAL_FILE_ERROR;
        default:
            throw BUPMsg::INTERNAL_ERROR;
        }
    }
    catch(const LinuxException& x) {
        assert(!"Unreachable code!?");
        EventReporter::instance().write(x);
        throw x;
    }
    catch(BUPMsg::ERROR_CODE x) { // to avoid catch(...)
        throw x;
    }
    catch(...) {
        EventReporter::instance().write("Unhandled exception");
        throw BUPMsg::INTERNAL_ERROR;
    }
}

//
// 
// (SYTUC)
//===========================================================================
void SBCSwitchJob_V4::switchRolldown(SBCId tmpId, const SBCList& sbcList) {

    newTRACE((LOG_LEVEL_INFO, "SBCSwitchJob::switchRolldown(%s, SBCList)", 0,tmpId.dirName().c_str()));

	const Config::CLUSTER tg;
	SBC_V4* sbc = new SBC_V4[sbcList.count()];
    try {        

		// NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
		// BUP Interactions
		// Dumping and synic/sytuc are mutually exlusive. Don't have to check dumping
		// If any of the SBCs participating in the swich is used for loading, the
		// switch request is rejected
		DataFile::BUACTREC load;
		DataFile::onGoingLoad(load);
		DataFile::BUACTREC_ITERATOR it;
		DataFile::BUACTREC_ITERATOR last = load.end();
		if (!load.empty()) {
			it = find_first_of(load.begin(), last, &sbcList.m_dat.sbcId[0], 
					&sbcList.m_dat.sbcId[sbcList.m_dat.count],
						boost::bind<bool>(equal_to<int>(),
								boost::bind(&DataFile::BUPAIR::first, _1),
								boost::bind(&SBCId::id, _2)));
			if (it < last ) {
				TRACE((LOG_LEVEL_WARN, "SBCSwitchJob_V4::switchHighLow: Switch rejected due to ongoing load", 0));
				throw BUPMsg::SWITCH_REJECTED_DUE_TO_ONGOING_LOAD;
			}
		}

		//init the SBCs
        u_int32 i = 0;
        for(i = 0; i < sbcList.count(); ++i)
            sbc[i].id(sbcList.sbcId(i));

        //=====================================================================
        // check buinfo
        if(sbc[0].biQuery() != SBC_V4::BQ_NO_ERROR)
            throw BUPMsg::SWITCH_LOW_BACKUPINFO_BAD;
        if(sbc[sbcList.count() - 1].biQuery() != SBC_V4::BQ_NO_ERROR)
            throw BUPMsg::SWITCH_HIGH_BACKUPINFO_BAD;

        // Check that the highest SBC is younger than the low SBC
        if(sbc[sbcList.count() - 1].cmpDateTime(sbc[0]) != 1)
            throw BUPMsg::SWITCH_SBC_DATES_ILLEGAL_FOR_OP;

        // reserve all SBC dirs
        for(i = 0; i < sbcList.count(); ++i) {
            if(!sbc[i].reserve())
                throw BUPMsg::SBC_RESERVE_FAILED;
        }
        // RELFSW0 -> RELFSWX/Y
        sbc[0].move(tmpId, false);
        // RELFSWn -> RELFSW0
        sbc[sbcList.count() - 1].move(sbcList.sbcId(0), false);
        // RELFSWn-1 -> RELFSWn
        for(i = sbcList.count() - 1; i > 1; --i)
            sbc[i - 1].move(sbcList.sbcId(i), false);
        // RELFSWX/Y -> RELFSW1
        sbc[0].move(sbcList.sbcId(1), false);
    }
    catch(const CodeException& x) {
        delete[] sbc;
        switch(x.errcode()) {
        case CodeException::REQ_SBC_MISSING:
                throw BUPMsg::SBC_NOT_FOUND;
            case CodeException::SBC_RESERVE_FAILED:
                throw BUPMsg::SBC_RESERVE_FAILED;
            case CodeException::FMS_PHYSICAL_FILE_ERROR:
                throw BUPMsg::FMS_PHYSICAL_FILE_ERROR;
            default:
                throw BUPMsg::INTERNAL_ERROR;
        }
    }
    catch(const LinuxException& x) {
        delete[] sbc;
        EventReporter::instance().write(x);
        throw x;
    }
    catch(BUPMsg::ERROR_CODE x) { // to avoid catch(...)
        delete[] sbc;
        throw x;
    }
    catch(...) {
        delete[] sbc;
        EventReporter::instance().write("Unhandled exception");
        throw BUPMsg::INTERNAL_ERROR;
    }
    delete[] sbc;

}

