//****************************************************************************************
// 
// NAME
//		CPS_BUAP_DateTime.h - Include file for DateTime class
//
// COPYRIGHT
//		Ericsson AB 2004 - All Rights Reserved.
//
//		The Copyright to the computer program(s) herein is the	
//		property of Ericsson AB, Sweden. The program(s) may be
//		used and/or copied only with the written permission from
//		Ericsson AB or in accordance with the terms and conditions 
//		stipulated in the agreement/contract under which the 
//		program(s) have been supplied.
//
// DESCRIPTION 
//		Time and Date functionality for BUAP block	
//
// ERROR HANDLING
//		---
//
// DOCUMENT NO
//  190 89-CAA 109 1412
//
// AUTHOR 
//		2004-05-06 by EAB/UZ/DH QAMMKAL
//
// REVISION
//		R1A	
// 
// CHANGES
//
// RELEASE REVISION HISTORY
//
//		REV NO	DATE 	NAME		DESCRIPTION
//		R1A		040506  QAMMKAL		First version
//  101010 xchihoa     Ported to Linux for APG43L.
//
// LINKAGE
//		-
//
// SEE ALSO
//		-
//
//****************************************************************************************
#ifndef CPS_BUAP_DATETIME_H
#define CPS_BUAP_DATETIME_H

#include <time.h>
#include <sstream>

namespace CPS_BUAP
{

class DateTime
{
public:
	DateTime();
	DateTime(int day, int mon, int year);
	DateTime(int day, int mon, int year, int hr, int min);
	DateTime(int day, int mon, int year, int hr, int min, int sec);
	~DateTime();

	bool operator<(const DateTime &dt) const;
	bool operator>(const DateTime &dt) const;
	bool operator==(const DateTime &dt) const;
	bool operator!=(const DateTime &dt) const;
	bool operator<=(const DateTime &dt) const;
	bool operator>=(const DateTime &dt) const;
	bool isValid();

private:
	time_t m_time; //time in second from epoch
	bool m_valid;

	friend std::ostream& operator<<(std::ostream& out, const DateTime& dt);

};
} //End of the namespace
#endif
