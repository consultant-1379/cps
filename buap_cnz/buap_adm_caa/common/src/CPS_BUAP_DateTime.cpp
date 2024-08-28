//****************************************************************************************
// 
// NAME
//		CPS_BUAP_DateTime.cpp - Implementation file for DateTime class
//
// COPYRIGHT
//		 Ericsson AB 2004 - All Rights Reserved.
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
#include "CPS_BUAP_DateTime.h"
#include <iostream>
#include <iomanip>

namespace CPS_BUAP {

using namespace std;

/**************** Default constructor *****************/
DateTime::DateTime() {

        // Return current local time in seconds
        m_time = time(NULL);
        m_valid = true;

}

/**************** Constructor for date *****************/
DateTime::DateTime(int day, int mon, int year) {

	m_valid = false;

	struct tm tmTime;
	tmTime.tm_sec = 0;
	tmTime.tm_min = 0;
	tmTime.tm_hour = 0;
	tmTime.tm_mday = day;
	tmTime.tm_mon = mon - 1;
	tmTime.tm_year = year - 1900;
	tmTime.tm_wday = 0;
	tmTime.tm_yday = 0;
	tmTime.tm_isdst = 0;

	m_time = mktime(&tmTime);
	if(m_time != -1)
		m_valid = true;
}

/**************** Constructor for date with hours and minutes *****************/
DateTime::DateTime(int day, int mon, int year, int hr, int min) {


	m_valid = false;

	struct tm tmTime;
	tmTime.tm_sec = 0;
	tmTime.tm_min = min;
	tmTime.tm_hour = hr;
	tmTime.tm_mday = day;
	tmTime.tm_mon = mon - 1;
	tmTime.tm_year = year - 1900;

	m_time = mktime(&tmTime);
	if(m_time != -1)
		m_valid = true;
}

/**************** Constructor for date with hours, minutes and seconds *****************/
DateTime::DateTime(int day, int mon, int year, int hr, int min, int sec) {


	m_valid = false;

	struct tm tmTime;
	tmTime.tm_sec = sec;
	tmTime.tm_min = min;
	tmTime.tm_hour = hr;
	tmTime.tm_mday = day;
	tmTime.tm_mon = mon - 1;
	tmTime.tm_year = year - 1900;

	m_time = mktime(&tmTime);
	if(m_time != -1)
		m_valid = true;

}

/**************** Default destructor *****************/
DateTime::~DateTime() {
}

/**************** 'Less than' operator *****************/
bool DateTime::operator <(const DateTime &dt) const {
	return m_time < dt.m_time;
}

/**************** 'Greater than' operator *****************/
bool DateTime::operator >(const DateTime &dt) const {
	return m_time > dt.m_time;
}

/**************** 'Equality' operator *****************/
bool DateTime::operator ==(const DateTime &dt) const {
	return m_time == dt.m_time;
}

/**************** 'Unequality' operator *****************/
bool DateTime::operator !=(const DateTime &dt) const {
	return m_time!= dt.m_time;
}

/**************** 'Less than or equality' operator *****************/
bool DateTime::operator <=(const DateTime &dt) const {
	return m_time <= dt.m_time;
}

/**************** 'Greater than or equality' operator *****************/
bool DateTime::operator >=(const DateTime &dt) const {
	return m_time >= dt.m_time;
}

/**************** Whether the date is valid or not *****************/
bool DateTime::isValid() {
	return m_valid;
}

/**************** Output stream operator *****************/
ostream & operator<<(std::ostream &out, const DateTime &dt) {

	struct tm * tmTime;
	tmTime = localtime(&dt.m_time );

	//Do the formatting
	std::stringstream ss;
	ss << setfill('0') << setw(2)<< tmTime->tm_mon +1 <<"/"
		<< setfill('0') << setw(2)<< tmTime->tm_mday<<"/"
		<< setw(4) << tmTime->tm_year + 1900 <<" "
		<<setfill('0') << setw(2)<< tmTime->tm_hour<<":"
		<<setfill('0') << setw(2)<< tmTime->tm_min<<":"
		<<setfill('0') << setw(2)<< tmTime->tm_sec;

	out <<ss.str();
	return out;

}

} //End of the namespace
