#ifndef _VERIFYSBCDATAJOB_H_
#define _VERIFYSBCDATAJOB_H_
/*
NAME
	VERIFYSBCDATAJOB -
LIBRARY 3C++
PAGENAME VERIFYSBCDATAJOB
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE VerifySBCDataJob.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.
	
	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION
	Executes the BUP command VERIFY_SBC_DATA.

ERROR HANDLING
	-

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
	2002-02-05 by UAB/KY/SK Anders Gillgren (qabgill, tag: ag)

LINKAGE
	-

SEE ALSO
	

*/
#pragma once

#include "BUPJob.h"
#include "CPS_BUSRV_Trace.h"		//mnst
//#include "AutoHandle.h"
#include "BackupInfo.h"
#include "buap.h"
#include "BUPMsg.h"
#include "AutoFd.h"
#include "SegmentHeader.h"
#include "CriticalSection.h"

// fwd
class BUPMsg;

using namespace buap;

//
//
//====================================================================
template<typename T>
BUPMsg::SBC_DATA_STATUS verifyDataFile(const char* filename, u_int64 size,
		const SectorInfo& sectorInfo, const T* header, Job* aJob) 
		
{

	//newTRACE(("BUPMsg::SBC_DATA_STATUS verifyDataFile(const char*", 0));		//mnst

	BUPMsg::SBC_DATA_STATUS result = BUPMsg::SDS_NO_ERROR;

	// if size == 0 there should be no file, which we don't check
	if (!size)
	   return result;
	   	
    int fd;
    try {
        fd = FileFunx::CreateFileX(filename, O_RDONLY, S_IRUSR | S_IRGRP | S_IROTH);
    }
    catch(...) {
        result = BUPMsg::SDS_DATAAREA_MISSING;
        return result;
    }
    AutoFd aFd(fd);		
    if(FileFunx::getFileSize(filename) == size) {
        for(u_int32 i = 0; i < sectorInfo.nofSegments(); ++i) {
	        if(header[i].checksum() != FileFunx::segmentCSum(fd, header[i].blockSize(), aJob)) {
		        result = BUPMsg::SDS_BAD_CHECKSUM;
		        break;
	        }
        }
    }
    else
        result = BUPMsg::SDS_BAD_DATA_SIZE;
	
	return result;
}


class VerifySBCDataJob : public BUPJob {
public:
	VerifySBCDataJob(const BUPMsg& msg);
	virtual ~VerifySBCDataJob();
	virtual void execute();
	virtual void abort();
	virtual bool isAborted() const;


private:
	// THNG: Do not remove these comments
	//BUPMsg::SBC_DATA_STATUS checkFile(const char* filename, u_int64 size,
	//	const SectorInfo& sectorInfo, const DirectSegmentHeader* header);
	//BUPMsg::SBC_DATA_STATUS checkFile(const char* filename, u_int64 size,
	//	const SectorInfo& sectorInfo, const IndirectSegmentHeader* header);

	VerifySBCDataJob(const VerifySBCDataJob& rhs);
	VerifySBCDataJob& operator=(const VerifySBCDataJob& rhs);

private:
	bool               m_abort;
	mutable CriticalSection    m_cs;
};
//
// ctor
//====================================================================
inline
VerifySBCDataJob::VerifySBCDataJob(const BUPMsg& msg) : BUPJob(msg), m_abort(false) 
{	
}
//
// dtor
//====================================================================
inline
VerifySBCDataJob::~VerifySBCDataJob()
{	
}
//
//
//====================================================================
inline
void VerifySBCDataJob::abort()
{
    AutoCS a(m_cs);
    m_abort = true;
}
//
//
//====================================================================
inline
bool VerifySBCDataJob::isAborted() const 
{
    AutoCS a(m_cs);
    return m_abort;  
}

///////////////////////////////////////////////////////////////////////////////

class VerifySBCDataJob_V4 : public BUPJob {
//foos
public:
	VerifySBCDataJob_V4(const BUPMsg& msg);
	virtual ~VerifySBCDataJob_V4();
	virtual void execute();
	virtual void abort();
	virtual bool isAborted() const;
private:

	// THNG: Do not remove these commennts
	//BUPMsg::SBC_DATA_STATUS checkFile(const char* filename, u_int64 size,
	//	const SectorInfo& sectorInfo, const DirectSegmentHeader* header);
	//BUPMsg::SBC_DATA_STATUS checkFile(const char* filename, u_int64 size,
	//	const SectorInfo& sectorInfo, const IndirectSegmentHeader* header);
	VerifySBCDataJob_V4(const VerifySBCDataJob_V4& rhs);
	VerifySBCDataJob_V4& operator=(const VerifySBCDataJob_V4& rhs);
//attr
private:
	bool               m_abort;
	mutable CriticalSection    m_cs;
};
//
// ctor
//====================================================================
inline
VerifySBCDataJob_V4::VerifySBCDataJob_V4(const BUPMsg& msg) : BUPJob(msg), m_abort(false) 
{	
}
//
// dtor
//====================================================================
inline
VerifySBCDataJob_V4::~VerifySBCDataJob_V4()
{	
}
//
//
//====================================================================
inline
void VerifySBCDataJob_V4::abort()
{
    AutoCS a(m_cs);
    m_abort = true;
}
//
//
//====================================================================
inline
bool VerifySBCDataJob_V4::isAborted() const 
{
    AutoCS a(m_cs);
    return m_abort;  
}

#endif
