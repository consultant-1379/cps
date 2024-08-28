#ifndef _CLEANDIRSJOB_H_
#define _CLEANDIRSJOB_H_
/*
NAME
	CLEANDIRSJOB - 
LIBRARY 3C++
PAGENAME CLEANDIRSJOB
HEADER  CPS  
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE CleanDirsJob.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2001. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the 
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION 
   Clean up the ftp/tmp filearea.

ERROR HANDLING
   -

DOCUMENT NO
   190 89-CAA 109 0387

AUTHOR 
   2001-09-07 by U/Y/SF Anders Gillgren (qabgill, tag: ag)

LINKAGE
	-

SEE ALSO 
   -

*/

#include "JobBase.h"
//#include "ACS_CS_API.h"

class CleanDirsJob : public JobBase {
//foos
public:
	CleanDirsJob();
	CleanDirsJob(CPID cpId);
	CleanDirsJob(CPID cpId, bool clusterSSI);
	void virtual execute();
private:
	CleanDirsJob(const CleanDirsJob& ); // not impl.
	CleanDirsJob& operator=(const CleanDirsJob& ); // -:-
	// are we interested in this file?
	//virtual void executeSpecial(u_int16) {}
	//virtual void executeNonOp(u_int16 sbc) {}
	//virtual void resetState() {}
	//virtual JQS_STATE getState(u_int& sbc) { sbc = 0xFF; return JQS_NORMAL; }
	//virtual void resetState(JQS_STATE, u_int16, Job*) {}
	//virtual Job* getHelpJob() { return NULL; }

//protected:
	//bool filter(ULARGE_INTEGER now, const WIN32_FIND_DATA& ) const; 
//attr
protected:
	// -
	// (all instance members are in the base class)
	static const unsigned int NOF_HUNDRED_NANOSECS_PER_MIN; // = 600000000;
	static const char* FILE_PATTERN; // = "/*"
	static const char* DIR_PATTERN; // = "*"
	static const char DOT; // = '.'
	CPID m_cpId;
	bool m_clusterSSI;
};

// THN: This is a house keeping job, not a protocol job
#endif
