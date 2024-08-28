/*
NAME
	File_name:SBCDir.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

The Copyright to the computer program(s) herein is the property of Ericsson
Utvecklings AB, Sweden.
The program(s) may be used and/or copied only with the written permission from
Ericsson Utvecklings AB or in accordance with the terms and conditions
stipulated in the agreement/contract under which the program(s) have been
supplied.

DESCRIPTION
	Low-level directory operations using either FMS or Win32 functions.

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
	2002-05-30 by U/Y/SF Anders Gillgren (qabgill, tag :ag)

SEE ALSO
	-

Revision history
----------------
2002-05-30 qabgill Created
2002-10-31 uablan  Add ACS_Trace
2007-02-07 aublan  Add changes for Multiple CP System
2010-12-14 xquydao Update for APG43L project

*/

#include "SBCDir.h"

#include "buap.h"
#include "CodeException.h"
#include "Config.h"
#include "CPS_BUSRV_Trace.h"
#include "EventReporter.h"
#include "FileFunx.h"
#include "LinuxException.h"
#include "BUPFunx.h"
#include "EnvFMS.h"
#include "fms_cpf_file.h"

#if HAVE_ACS_CS
#include "ACS_CS_API.h" // def CPID
#endif

#include <cassert>

const string LOCK_FILE = ".lock";
//
// ctor 1
//===========================================================================
SBCDir::SBCDir(SBCId sbcId, bool useFMS, bool reserveDir /* = false*/, CPID cpId /* = 0xFFFF*/)
: m_id(sbcId), m_hDir(), m_useFMS(useFMS)
{
	newTRACE((LOG_LEVEL_INFO, "SBCDir::SBCDir(%d, %d, %d, %d /* = false */)", 0, sbcId.id(), useFMS, cpId, reserveDir));

#ifdef LOCAL_BUILD
	assert(!useFMS && "Cannot use FMS if LOCAL_BUILD is defined");
#else
	m_fmsDir = NULL;
#endif

	//if(reserveDir)
	//{
	//	reserve(cpId);
	//}
}
//
// ctor 2, set tmp object
//===========================================================================
SBCDir::SBCDir(bool useFMS) : m_id(), m_hDir(), m_useFMS(useFMS)
{
	newTRACE((LOG_LEVEL_INFO, "SBCDir::SBCDir(%d)", 0, useFMS));

#ifdef LOCAL_BUILD
	assert(!useFMS && "Cannot use FMS if LOCAL_BUILD is defined");
#else
	m_fmsDir = NULL;
#endif
}
//
//
//===========================================================================
SBCDir::~SBCDir()
{
	newTRACE((LOG_LEVEL_INFO, "SBCDir::~SBCDir()", 0));

#ifndef LOCAL_BUILD
	if (m_fmsDir) {
	
		try {
			if (m_fmsDir->isReserved()) 
				m_fmsDir->unreserve();
		}
		catch (...) {
			// do nothing
		}
		 
	    delete m_fmsDir;
	    m_fmsDir = NULL;
	    TRACE((LOG_LEVEL_INFO, "SBCDir::~SBCDir() on sbc = %d", 0, m_id.id()));
	}
#endif
}
//
// dir (L:\fms\...\RELFSWn)
//===========================================================================
string SBCDir::dir(CPID cpId) const
{
	newTRACE((LOG_LEVEL_INFO, "SBCDir::dir()", 0));

	return Config::instance().sbcDir(m_id, cpId);
}
//
//
//===========================================================================
bool SBCDir::dirExists(CPID cpId) const
{
	newTRACE((LOG_LEVEL_INFO, "SBCDir::dirExists()", 0));
	
#ifdef LOCAL_BUILD					
	return FileFunx::dirExists(dir(cpId).c_str());
#else
	return EnvFMS::dirExists(m_id, cpId); 
#endif			
}
//
// change ID, maintain reservation state
//===========================================================================
void SBCDir::id(SBCId sbcId, CPID cpId)
{
	newTRACE((LOG_LEVEL_INFO, "SBCDir::id(%d, %d)", 0, sbcId.id(), cpId));
	
	bool rereserve = reserved();

	if (rereserve) 
	    unreserve();

	m_id = sbcId.id();

	if(rereserve)
	    reserve(cpId);
}
//
// returns false if access denied. throws on errors.
//===========================================================================
bool SBCDir::linuxReserve(CPID cpId)
{
	newTRACE((LOG_LEVEL_INFO, "SBCDir::linuxReserve()", 0));

	bool result = true;

	if (!reserved())
	{
	    try
	    {
	        m_lockFile = Config::instance().sbcDir(m_id, cpId) + LOCK_FILE;
	        // Create then open lock file
	        m_hDir = FileFunx::CreateFileX(m_lockFile.c_str());
	    }
	    catch (LinuxException& e)
	    {
	        TRACE((LOG_LEVEL_ERROR, "linuxReserve failed. Error code: %d", 0, e.errcode()));

	        if (e.errcode() == EACCES)
	        {
	            result = false;
	        }
	        else
	        {
	            throw e;
	        }
	    }
	}

	return result;
}

//
// Linux funx
//===========================================================================
void SBCDir::linuxUnreserve()
{
    newTRACE((LOG_LEVEL_INFO, "SBCDir::linuxUnreserve()", 0));
    // Close lock file
    m_hDir.close();

    try
    {
        // Delete lock file
        FileFunx::DeleteFileX(m_lockFile.c_str());
    }
    catch (LinuxException& e)
    {
        TRACE((LOG_LEVEL_ERROR, "linuxUnreserve failed. Error code: %d", 0, e.errcode()));
        // Do nothing
    }
}

//
// Linux funx
//===========================================================================
bool SBCDir::linuxReserved() const
{
    newTRACE((LOG_LEVEL_INFO, "SBCDir::linuxReserved()", 0));

    bool result = m_hDir.valid();

    try
    {
        // Check lock file exist
        result = result && FileFunx::fileExists(m_lockFile.c_str());
    }
    catch (...)
    {
        result = false;
    }

    return result;
}


//
//
//===========================================================================
void SBCDir::linuxMove(SBCId sbcId, bool overwrite, CPID cpId)
{
	newTRACE((LOG_LEVEL_INFO, "SBCDir::linuxMove(%d, %d)", 0, sbcId.id(), overwrite));

	string linuxdir = dir(cpId);
	bool rereserve = reserved();

	if(!rereserve && !reserve(cpId))
	{
		// check this
		if (!FileFunx::dirExists(linuxdir.c_str()))
		{
			THROW_XCODE("Required directory (" << linuxdir.c_str() << ") missing for move operation",
				CodeException::REQ_SBC_MISSING);
		}

		THROW_XCODE("Failed to reserve directory(" << linuxdir.c_str() << ") for move operation",
			CodeException::SBC_RESERVE_FAILED);
	}
	
	string newDir = Config::instance().sbcDir(sbcId, cpId);

	if (overwrite)
	{
	    try
	    {
	        // Clean the newDir if exist
	        FileFunx::cleanDir(newDir.c_str(), "*", true); // Ignore result
	    }
	    catch (...)
	    {
	        // Do nothing, ignore result
	    }
	}

	// Unreserve before move
	unreserve();

	if (FileFunx::MoveFileX(linuxdir.c_str(), newDir.c_str()))
	{
		m_id = sbcId;
	}
	else
	{
	    THROW_LINUX_AUTO();
	}

	if (rereserve)
	{
		reserve(cpId);
	}
}

//
// returns false if file is busy. throws on errors.
//===========================================================================
#ifndef LOCAL_BUILD
bool SBCDir::fmsReserve(CPID cpId) {

	newTRACE((LOG_LEVEL_INFO, "SBCDir::fmsReserve(%d)", 0, cpId));
	TRACE((LOG_LEVEL_INFO, "fmsReserve() on %s for CP %d begin", 0, m_id.dirName().c_str(), cpId));

	try {
		bool isMultipleCP = Config::instance().isMultipleCP();
		if (isMultipleCP && cpId != 0xFFFF){
			char cpName[25];
			int res = BUPFunx::convertCPIdtoCPName(cpId, cpName);
			if (res != 0)
				THROW_XCODE_LOG("fmsReserve: Convertion of CPId to CP Name failed.", CodeException::CP_APG43_CONFIG_ERROR);

			m_fmsDir = new FMS_CPF_File(fmsDir().c_str(), cpName);
		}
		else if (!isMultipleCP && cpId == 0xFFFF){
			m_fmsDir = new FMS_CPF_File(fmsDir().c_str());
		}
		else if (isMultipleCP && cpId == 0xFFFF) {
			// CP = Multi CP system, APG = One CP system
			THROW_XCODE_LOG("fmsReserve: CP = One CP system, APG = Multi CP system.", CodeException::CP_APG43_CONFIG_ERROR);
		}
		else { //if (!isMultipleCP && cpId != 0xFFFF) {
			// CP = One CP system, APG = Multi CP system
			THROW_XCODE_LOG("fmsReserve: CP = Multi CP system, APG = One CP system.", CodeException::CP_APG43_CONFIG_ERROR);
		}
		
		m_fmsDir->reserve(FMS_CPF_Types::XR_XW_);

		// This is a work around for rename		
		//TRACE(("fmsReserve(), Work around for rename()", 0));
		//if (m_fmsDir->isReserved())
		//	m_fmsDir->unreserve();
	}
	catch(FMS_CPF_Exception& x) {
    	//if (isTRACEABLE()) {
		stringstream str;
		str << "SBCDir::fmsReserve(), FMS_CPF_File(), FMS Errorcode : " << x.errorCode() << ", Text: " << x.errorText() << ", Details: "
	    	<< x.detailInfo() << endl;
		TRACE((LOG_LEVEL_ERROR, "%s", 0, str.str().c_str()));
		EventReporter::instance().write(str.str().c_str());		
		//}
		throw;
	}
	catch(CodeException& x) {
		newTRACE((LOG_LEVEL_ERROR, "SBCDir::fmsReserve(), Text: %s. Errorcode: %d", 0, x.what().c_str(), x.errcode()));
		throw;
	}
	catch(...) {
		EventReporter::instance().write("SBCDir::fmsReserve(), Unknown exception during fms reserve");
		throw CodeException::INTERNAL_ERROR;
	}

	TRACE((LOG_LEVEL_INFO, "fmsReserve OK\n", 0));
	return true;
}
#endif
//
//
//===========================================================================
#ifndef LOCAL_BUILD
void SBCDir::fmsUnreserve() 
{
	newTRACE((LOG_LEVEL_INFO, "SBCDir::fmsUnreserve()", 0));
	if (m_fmsDir->isReserved()) 
		m_fmsDir->unreserve(); 
	delete m_fmsDir; 
	m_fmsDir = 0; 
}
#endif
//
//
//===========================================================================
#ifndef LOCAL_BUILD
void SBCDir::fmsMove(SBCId sbcId, bool overwrite, CPID cpId) {

	newTRACE((LOG_LEVEL_INFO, "SBCDir::fmsMove(%d, %d, %d)", 0, sbcId.id(), overwrite, cpId));
	
	assert(!overwrite); // not supported for FMS (yet?!)
	
	TRACE((LOG_LEVEL_INFO, "fmsMove from %s to %s begin", 0, m_id.dirName().c_str(), sbcId.dirName().c_str()));


	bool release = reserved();
	if(!release) {
		TRACE((LOG_LEVEL_INFO, "Reservation of <%s> during fmsmov()", 0, m_id.dirName().c_str()));
		release = reserve(cpId);
		if(!release) { // FMS must be reserved for rename/move op
			string windir = dir(cpId);
			TRACE((LOG_LEVEL_INFO, "fmsMove: dir  %s", 0, windir.c_str()));
			//if(!FileFunx::dirExists(windir.c_str())) {
			if (!EnvFMS::dirExists(m_id, cpId)) { 			
				THROW_XCODE("Required directory (" << windir.c_str() << ") missing for move operation",
					CodeException::REQ_SBC_MISSING);
			}
			THROW_XCODE("Failed to reserve directory(" << windir.c_str() << ") for move operation",
				CodeException::SBC_RESERVE_FAILED);
		}
	}


	try {
		TRACE((LOG_LEVEL_INFO, "fmsMove: rename %s %s to %s", 0, m_fmsDir->getPhysicalPath().c_str(), m_id.dirName().c_str(), sbcId.dirName().c_str()));
		//TRACE(("fmsMove: rename from %s to %s", 0, m_id.dirName().c_str(), sbcId.dirName().c_str()));
		
		m_fmsDir->rename(sbcId.dirName().c_str());
	}
	catch(FMS_CPF_Exception& x) {
		stringstream str;
		str << "fmsMove; rename, FMS Errorcode : " << x.errorCode() << ", Text: " << x.errorText() << ", Details: "
		    << x.detailInfo() << endl;
		TRACE((LOG_LEVEL_ERROR, "%s", 0, str.str().c_str()));
		EventReporter::instance().write(str.str().c_str());		
		throw;		
	}
	
	m_id = sbcId;

	if(release)
		unreserve();

	TRACE((LOG_LEVEL_INFO, "SBCDir::fmsMove() OK", 0));
}
#endif

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
// ctor 1
//===========================================================================
SBCDir_V4::SBCDir_V4(SBCId sbcId, const Config::CLUSTER& c, bool useFMS, bool reserveDir)
: m_id(sbcId), m_useFMS(useFMS), m_cluster(c) {

	newTRACE((LOG_LEVEL_INFO, "SBCDir_V4::SBCDir_V4(%d, CLUSTER, %d, %d)", 0, sbcId.id(), useFMS, reserveDir));

#ifdef LOCAL_BUILD
    assert(!useFMS && "Cannot use FMS if LOCAL_BUILD is defined");
#else
    m_fmsDir = NULL;
#endif

    //if(reserveDir) {
    //    reserve(1);
    //}
}
//
// ctor 2, set tmp object
//===========================================================================
SBCDir_V4::SBCDir_V4(bool useFMS) : m_useFMS(useFMS) {

	newTRACE((LOG_LEVEL_INFO, "SBCDir_V4::SBCDir_V4(%d)", 0, useFMS));

#ifdef LOCAL_BUILD
	assert(!useFMS && "Cannot use FMS if LOCAL_BUILD is defined");
#else
	m_fmsDir = NULL;
#endif
}
//
//
//===========================================================================
SBCDir_V4::~SBCDir_V4() {

    newTRACE((LOG_LEVEL_INFO, "SBCDir_V4r::~SBCDir_V4()", 0));

#ifndef LOCAL_BUILD
	if (m_fmsDir) {
	
		try {
			if (m_fmsDir->isReserved()) 
				m_fmsDir->unreserve();
		}
		catch (...) {
			// do nothing
		}
		 
	    delete m_fmsDir;
	    m_fmsDir = NULL;
	    TRACE((LOG_LEVEL_INFO, "SBCDir::~SBCDir() on sbc = %d", 0, m_id.id()));
	}
#endif
}
//
//==========================================================================
ostream &SBCDir_V4::print(ostream &os) const
{
	os << m_id 
	   << "<" << dir() << ">"
	   << "<" << (dirExists() ? "true" : "false") << ">";
	return os;
}
//
//
ostream &operator<<(ostream &os, const SBCDir_V4& sdir)
{
	return sdir.print(os);
}
//
// win dir (L:\fms\...\RELFSWn)
//===========================================================================
string SBCDir_V4::dir() const {

    newTRACE((LOG_LEVEL_INFO, "SBCDir::dir()", 0));

    return Config::instance().sbcDir(m_id, m_cluster);
}
//
//
//===========================================================================
bool SBCDir_V4::dirExists() const {

    newTRACE((LOG_LEVEL_INFO, "BCDir_V4::dirExists()", 0));
    
    return EnvFMS::dirExists(m_id, m_cluster); 
}
//
// change ID, maintain reservation state
//===========================================================================
void SBCDir_V4::id(SBCId sbcId) {

    newTRACE((LOG_LEVEL_INFO, "SBCDir_V4::id(%d)", 0, sbcId.id()));

    bool rereserve = reserved();
    if(rereserve)
        unreserve();
    m_id = sbcId.id();
    if(rereserve)
        reserve(2);
}
//
// returns false if file is busy. throws on errors.
//===========================================================================
#ifndef LOCAL_BUILD
bool SBCDir_V4::fmsReserve() {

    newTRACE((LOG_LEVEL_INFO, "SBCDir_V4::fmsReserve()", 0));

    TRACE((LOG_LEVEL_INFO, "fmsReserve on %s for CLUSTER begin", 0, m_id.dirName().c_str()));
	TRACE((LOG_LEVEL_INFO, "fmsDir().c_str(): %s", 0, fmsDir().c_str()));

	try {
		if (Config::instance().isMultipleCP()){
			m_fmsDir = new FMS_CPF_File(fmsDir().c_str(), Config::CLUSTER_STR);
		}
		else { 
			THROW_XCODE_LOG("fmsReserve: CP = Multi CP system, APG = One CP system.", CodeException::CP_APG43_CONFIG_ERROR);
		}
		
		m_fmsDir->reserve(FMS_CPF_Types::XR_XW_);

		// This is a work around for rename		
		//TRACE(("fmsReserve(), Work around for rename()", 0));
		//if (m_fmsDir->isReserved())
		//	m_fmsDir->unreserve();
	}
	catch(FMS_CPF_Exception& x) {
    	//if (isTRACEABLE()) {
		stringstream str;
		str << "SBCDir::fmsReserve(), FMS_CPF_File(), FMS Errorcode : " << x.errorCode() << ", Text: " << x.errorText() << ", Details: "
	    	<< x.detailInfo() << endl;
		TRACE((LOG_LEVEL_ERROR, "%s", 0, str.str().c_str()));
		EventReporter::instance().write(str.str().c_str());		
		//}
		throw;
	}
	catch(CodeException& x) {
		newTRACE((LOG_LEVEL_ERROR, "SBCDir::fmsReserve(), Text: %s. Errorcode: %d", 0, x.what().c_str(), x.errcode()));
		throw;
	}
	catch(...) {
		EventReporter::instance().write("SBCDir::fmsReserve(), Unknown exception during fms reserve");
		throw CodeException::INTERNAL_ERROR;
	}

    TRACE((LOG_LEVEL_INFO, "fmsReserve OK\n", 0));
    return true;
}
#endif
//
//
//===========================================================================
#ifndef LOCAL_BUILD
void SBCDir_V4::fmsUnreserve() 
{
	newTRACE((LOG_LEVEL_INFO, "SBCDir-V4::fmsUnreserve()", 0));
	if (m_fmsDir->isReserved()) 
		m_fmsDir->unreserve(); 
	delete m_fmsDir; 
	m_fmsDir = 0; 
}
#endif

//
//
//===========================================================================
#ifndef LOCAL_BUILD
void SBCDir_V4::fmsMove(SBCId sbcId, bool overwrite) {

    newTRACE((LOG_LEVEL_INFO, "SBCDir_V4::fmsMove(%d, %d)", 0, sbcId.id(), overwrite));
    
    assert(!overwrite); // not supported for FMS (yet?!)
    
    TRACE((LOG_LEVEL_INFO, "fmsMove from %s to %s begin", 0, m_id.dirName().c_str(), sbcId.dirName().c_str()));


    bool release = reserved();
    if(!release) {
        release = reserve(3);
        if(!release) { // FMS must be reserved for rename/move op
            string windir = dir();
            TRACE((LOG_LEVEL_INFO, "fmsMove: dir  %s", 0, windir.c_str()));
            if(!EnvFMS::dirExists(m_id, m_cluster)) {
                THROW_XCODE("Required directory (" << windir.c_str() << ") missing for move operation",
                    CodeException::REQ_SBC_MISSING);
            }
            THROW_XCODE("Failed to reserve directory(" << windir.c_str() << ") for move operation",
                CodeException::SBC_RESERVE_FAILED);
        }
    }
    try {
        TRACE((LOG_LEVEL_INFO, "fmsMove: rename %s %s to %s", 0, m_fmsDir->getPhysicalPath().c_str(), m_id.dirName().c_str(), sbcId.dirName().c_str()));
        m_fmsDir->rename(sbcId.dirName().c_str());
    }
	catch(FMS_CPF_Exception& x) {
		stringstream str;
		str << "fmsMove; rename, FMS Errorcode : " << x.errorCode() << ", Text: " << x.errorText() << ", Details: "
		    << x.detailInfo() << endl;
		TRACE((LOG_LEVEL_ERROR, "%s", 0, str.str().c_str()));
		EventReporter::instance().write(str.str().c_str());		
		throw;		
	}  
    
    m_id = sbcId;

    if(release)
        unreserve();

    TRACE((LOG_LEVEL_INFO, "fmsMove OK", 0));
}
#endif



