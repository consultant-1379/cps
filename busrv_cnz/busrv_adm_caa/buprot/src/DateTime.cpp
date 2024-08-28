/*
Linux version
*/


#include "DateTime.h"

//#include "CPS_BUSRV_Trace.h"

#include <iostream>
#include <iomanip>
#include <cstring>

//
// 
//===========================================================================
void DateTime::addSeconds(size_t val) 
{
    tm self_tm = systemtime();
    self_tm.tm_isdst = -1;
    time_t self = mktime(&self_tm);
    self += val;
    localtime_r(&self, &self_tm);
    systemtime(self_tm);        
    assert(valid());
}

//
// 
// 1 = greater, 0 = equal, -1 = less
//===========================================================================
int DateTime::cmp(const DateTime& rhs) const 
{
    assert(valid());
    tm self_tm = systemtime();
    tm rhs_tm = rhs.systemtime();
    self_tm.tm_isdst = rhs_tm.tm_isdst = -1;
    double res = ::difftime(mktime(&self_tm), mktime(&rhs_tm));
    return res > 0 ? 1 : res < 0? -1 : 0;
}
//
// 
//===========================================================================
Duration DateTime::operator-(const DateTime& rhs) const 
{
    assert(valid());
    tm self_tm = systemtime();
    tm rhs_tm = rhs.systemtime();
    self_tm.tm_isdst = rhs_tm.tm_isdst = -1;
    return Duration(mktime(&self_tm) - mktime(&rhs_tm));
}
//
// 
//===========================================================================
double DateTime::difftime(const DateTime& rhs) const 
{
    assert(valid());
    tm self_tm = systemtime();
    tm rhs_tm = rhs.systemtime();
    self_tm.tm_isdst = rhs_tm.tm_isdst = -1;
    return ::difftime(mktime(&self_tm), mktime(&rhs_tm));
}
//
// 
//===========================================================================
void DateTime::set(const time_t& info) 
{

    setNull();	
    tm result;                 // Convert to calendar time or broken down time
    localtime_r(&info, &result);
    systemtime(result);        // Convert to DateTime
}

//
// set to current system time
//===========================================================================
void DateTime::now() 
{
	
    time_t now = time(0);      // Current time
    tm result;                 // Convert to calendar time or broken down time
    localtime_r(&now, &result);
    systemtime(result);        // Convert to DateTime
}
//
// convert from tm to DateTime
//===========================================================================
void DateTime::systemtime(const tm& st) 
{
	m_year = st.tm_year % 100;
	m_month = static_cast<u_int8>(st.tm_mon + 1);  // tm_mon = 0 .. 11; Jan = 0
	m_day = static_cast<u_int8>(st.tm_mday);
	m_hour = static_cast<u_int8>(st.tm_hour);
	m_minute = static_cast<u_int8>(st.tm_min);
	m_second = static_cast<u_int8>(st.tm_sec);
	m_daylight = 0;
	
	assert(valid());
}
//
// convert from DateTime to tm
//===========================================================================
tm DateTime::systemtime() const 
{
	
	tm st;
	memset(&st, 0, sizeof(st));
	if(!null()) {
		st.tm_year = m_year + (m_year >= BREAK_YEAR ? 1900 : 2000) - 1900;
		st.tm_mon = m_month - 1;
		st.tm_mday = m_day;
		st.tm_hour = m_hour;
		st.tm_min = m_minute;
		st.tm_sec = m_second;
	} else {
		st.tm_year = 1980 -1900;
		st.tm_mon = 1 -1;
		st.tm_mday = 1;
		st.tm_hour = 0;
		st.tm_min = 0;
		st.tm_sec = 0;
	}
	return st;
}
//
//
//===========================================================================
ostream& operator<<(ostream& os, const DateTime& dt) 
{
    ios::fmtflags flg(os.flags());
	os << "<" << dec << setfill('0')
		<< setw(2) << static_cast<unsigned short>(dt.m_year) << "-"
		<< setw(2) << static_cast<unsigned short>(dt.m_month) << "-"
		<< setw(2) << static_cast<unsigned short>(dt.m_day) << " "
		<< setw(2) << static_cast<unsigned short>(dt.m_hour) << ":"
		<< setw(2) << static_cast<unsigned short>(dt.m_minute) << ":"
		<< setw(2) << static_cast<unsigned short>(dt.m_second) << ">";
    os.flags(flg);
	return os;
}
//
//
/////////////////////////////////////////////////////////////////////////////
//===========================================================================
Duration::Duration(int64 val)
{
    m_signed = val < 0 ? val = -val, true: false;    
    m_hour = val / DateTime::SECS_PER_HOUR;
    val %= DateTime::SECS_PER_HOUR;    
    m_minute = val / DateTime::SECS_PER_MINUTE;
    m_second = val % DateTime::SECS_PER_MINUTE;
}
//
//
//===========================================================================
ostream& operator<<(ostream& os, const Duration& dt) 
{
    ios::fmtflags flg(os.flags());
    os << "<" << (dt.m_signed ? "-" : "") << dt.m_hour << ":"
        << dec << setfill('0')        
        << setw(2) << static_cast<unsigned short>(dt.m_minute) << ":"
        << setw(2) << static_cast<unsigned short>(dt.m_second) << ">";
    os.flags(flg);
	return os;
}

#if 0
//
// This is the test drive. Must not delete
//
int main(int agrc, char **argv)
{
    cout << "\nStart of main\n" << endl;
    
    DateTime now;
    now.now();    
    cout << "The current time is " << now << endl;

    tm tinfo = now.systemtime();
    DateTime then(tinfo);
    then.addSeconds(600);
    
    cout << "The current time then is " << then << endl;        
    cout << "compare now and then is " << now.cmp(then) << endl;
    cout << "compare then and now is " << then.cmp(now) << endl;
    
    DateTime another = then;
    cout << "compare then and another is " << another.cmp(then) << endl;
            
    cout << "\nEnd of main\n" << endl;    
        
    return 0;
}
#endif
