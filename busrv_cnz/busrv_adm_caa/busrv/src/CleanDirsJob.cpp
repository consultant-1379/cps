/*
NAME
   File_name:CleanDirsJob.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2001. All rights reserved.

The Copyright to the computer program(s) herein is the property of Ericsson
Utvecklings AB, Sweden.
The program(s) may be used and/or copied only with the written permission from
Ericsson Utvecklings AB or in accordance with the terms and conditions
stipulated in the agreement/contract under which the program(s) have been
supplied.

.DESCRIPTION
   Clean up the ftp/tmp filearea. Will delete all - "*" - files in the <key> tmp
	dirs in the ftp area, along with the <key> directories themselves.

DOCUMENT NO
   190 89-CAA 109 0387

AUTHOR 
   2001-09-07 by U/Y/SF Anders Gillgren (qabgill, tag :ag)

SEE ALSO 
   -

Revision history
----------------
2001-09-07 Created
2002-10-20 uabmnst Add ACSTrace
2004-06-24 uablan  Remove filter().
2004-08-24 uablan  Add filter without checking age of file/dir.
*/


#include "Config.h"
#include "CleanDirsJob.h"

#include <assert.h>

#include "buap.h"
#include "CPS_BUSRV_Trace.h"
#include "EventReporter.h"
#include "FileFunx.h"
#include "TmpCurrDir.h"
#include "LinuxException.h"

#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <cerrno>


//
// static inits
//
const unsigned int CleanDirsJob::NOF_HUNDRED_NANOSECS_PER_MIN = 600000000;
const char* CleanDirsJob::FILE_PATTERN = "/*";
const char* CleanDirsJob::DIR_PATTERN = "*";
const char CleanDirsJob::DOT = '.';
//
// ctor
//----------------------------------------------------------------------------
CleanDirsJob::CleanDirsJob() : JobBase(true) {
	newTRACE((LOG_LEVEL_INFO, "CleanDirsJob::CleanDirsJob()", 0));
	m_cpId = 0xFFFF;
	m_clusterSSI = false;
}
//
// ctor
//----------------------------------------------------------------------------
CleanDirsJob::CleanDirsJob(CPID cpId) : JobBase(true) {
	newTRACE((LOG_LEVEL_INFO, "CleanDirsJob::CleanDirsJob(%d)", 0, cpId));
	m_cpId = cpId;
	m_clusterSSI = false;
}
//
// ctor
//----------------------------------------------------------------------------
CleanDirsJob::CleanDirsJob(CPID cpId, bool clusterSSI) : JobBase(true) {
	newTRACE((LOG_LEVEL_INFO, "CleanDirsJob::CleanDirsJob(%d)", 0, cpId));
	m_cpId = cpId;
	m_clusterSSI = clusterSSI;
}
//
// pure virtual in base, called when the Job is "run"
//----------------------------------------------------------------------------
void CleanDirsJob::execute() {

	newTRACE((LOG_LEVEL_INFO, "CleanDirsJob::execute()", 0));
	

    // One CP System for now
    string keyDir;
    
	// get path to ftp area
	if (m_clusterSSI) {
		Config::CLUSTER tCluster(m_cpId, true);
		keyDir = Config::instance().tmpRoot(tCluster);
	}
	else if (Config::instance().isMultipleCP()) 
		keyDir = Config::instance().tmpRoot(m_cpId);
	else
		keyDir = Config::instance().tmpRoot();
	
    DIR *dirp;
    struct dirent *entp;
    
    try {	
    	if (!(dirp = opendir(keyDir.c_str()))) {
    	   THROW_LINUX_MSG("Open dir operation failed");
        }
        
        size_t failCnt = 0;
        string name = keyDir;         
        while ((entp = readdir(dirp)) != NULL) {
            // Only delete directories not starting with '.'
            if (entp->d_type != DT_DIR || entp->d_name[0] == DOT)
                continue;
            name = keyDir + entp->d_name + "/";            
                    
            failCnt += FileFunx::cleanDir(name.c_str(), "*");
            
        }
        if (closedir(dirp) != 0) {
            THROW_LINUX_MSG("Close dir operation failed");
        }
        TRACE((LOG_LEVEL_INFO, "CleanDirJob fail count: %d", 0, failCnt));
    }
    catch (Exception& x) {
		EventReporter::instance().write(x); // log error
		TRACE((LOG_LEVEL_ERROR, "CleanDirsJob failed, error code: %d", 0, x.errcode()));
	}
	catch (...) {
		EventReporter::instance().write("Unhandled error in CleanDir"); // log error
		TRACE((LOG_LEVEL_ERROR, "CleanDirsJob failed.", 0));
    }
}

