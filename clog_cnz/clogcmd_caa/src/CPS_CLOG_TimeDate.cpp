
#include "CPS_CLOG_TimeDate.h"
#include "CPS_CLOG_CommandArg.h"
#include <stdlib.h>
#include <sstream>
#include <string.h>
#include <iostream>
//#include <assert.h>
#include <sys/time.h>
#include "CPS_CLOG_CommandImp.h"
using namespace std;

/**
 ** \addtogroup cmdlls CMDLLS command
 **/
/*@{*/

int TimeDate::progress_count = 0;
int TimeDate::bar_pos = 0;
long long TimeDate::last_progress_ms = 0;

//-----------------------------------------------------------------------------
const int TimeDate::days_before[12] =
{ BEFORE_JAN, BEFORE_FEB, BEFORE_MAR, BEFORE_APR, BEFORE_MAY, BEFORE_JUN,
  BEFORE_JUL, BEFORE_AUG, BEFORE_SEP, BEFORE_OCT, BEFORE_NOV, BEFORE_DEC
};

const int TimeDate::days_month[12] =
{ DAYS_IN_JAN, DAYS_IN_FEB, DAYS_IN_MAR, DAYS_IN_APR,
  DAYS_IN_MAY, DAYS_IN_JUN, DAYS_IN_JUL, DAYS_IN_AUG,
  DAYS_IN_SEP, DAYS_IN_OCT, DAYS_IN_NOV, DAYS_IN_DEC
};
//-----------------------------------------------------------------------------
bool
TimeDate::parse(const char * arg, TimestampFormat fmt)
{
   mode = TM_INVALID;
   if (!strncmp(arg, "none", 5))
   {
        mode = TM_NO_STAMP;
        return true;
   }

   else if (!strncmp(arg, "past", 5))
   {
        mode = TM_PAST;
        return true;
   }

   else if (!strncmp(arg, "future", 7))
   {
        mode = TM_FUTURE;
        return true;
   }

   else if (fmt == TF_20YYMMDD_hhmm)   // [20]YYMMDD-hhmm (command line argument)
   {
	istringstream istr(arg);
	string argTime = "";
        if (! (istr >> argTime)) // Try to convert operand to functional board name
        return false;
        if(argTime.size() == 13)//[yy]yymmdd-hhmm
        {
	    if( argTime[8] != '-')
            return false;
        
        	for(unsigned int i=0; i<argTime.size(); i++)
        	{
            		if (i != 8)
            		{
                	  if((argTime[i] < '0' || argTime[i] > '9'))
                	  {
                	    return false;
			  }
            		}
        	}
	}
        else if(argTime.size() == 11)//[yy]yymmdd-hhmm
        {
            if( argTime[6] != '-')
            return false;
            for(unsigned int i=0; i<argTime.size(); i++)
            {
            if (i != 6)
            {
                if((argTime[i] < '0' || argTime[i] > '9'))
                {
                    return false;
                }
            }
            }

	}
        else 
            return false;
        int YYMMDD;
        int hhmm;
        if (sscanf(arg, "%u-%u", &YYMMDD, &hhmm) != 2)  // not YYMMDD-hhmm
        {
             if (log_control & LOG_DEBUG)
                cerr << "    time does not have format [20]YYMMDD-hhmm\r\n";
             return false;
        }

         
        if (log_control & LOG_DEBUG)
           cerr << "    time '" << arg << "'has format YYMMDD-hhmm\r\n";
         
        const int DD = YYMMDD       % 100 - 1;
        const int MM = (YYMMDD/100) % 100 - 1;
              int YY = (YYMMDD/10000);
        const int hh = hhmm / 100;
        const int mm = hhmm % 100;

        // argument range check
        //
        if (YY > 2099)                        return false;
        if (YY >= 2000)                       YY -= 2000;
        if (YY > 99)                          return false;
        if (YY < 0)                           return false;
        if (MM < 0)                           return false;
        if (MM > 11)                          return false;
        if (DD < 0)                           return false;
        if (DD > days_in_month(YY, MM) - 1)   return false;
        if (hh > 24)                          return false;
        if (mm > 59)                          return false;

        // input is ok here. Compute days and seconds.
        //
        seconds = compute_seconds(hh, mm, 0);
        days = compute_days(YY, MM, DD);
	
        mode = TM_NORMAL;
        return true;
   }

   else if (fmt == TF_YYMMDD)   // [20]YYMMDD (DATE= parameter)
   {
        if (strlen(arg) == 8)   arg += 2;   // 20YYMMDD -> YYMMDD
        if (strlen(arg) == 6)        // YYMMDD
        {
             const int YY = get_2(arg);
             const int MM = get_2(arg + 2) - 1;
             const int DD = get_2(arg + 4) - 1;
             days = compute_days(YY, MM, DD);
             mode = TM_NORMAL;
             return true;
        }

        return false;
   }

   else if (fmt == TF_hhmm)   // hhmm[ss] (TIME= parameter)
   {
        if (strlen(arg) == 6)        // hhmm[ss]
        {
             const int hh = get_2(arg);
             const int mm = get_2(arg + 2);
             const int ss = get_2(arg + 4);
             seconds = compute_seconds(hh, mm, ss);
             mode = TM_NORMAL;
             return true;
        }

        if (strlen(arg) == 4)        // hhmm
        {
             const int hh = get_2(arg);
             const int mm = get_2(arg + 2);
             seconds = compute_seconds(hh, mm, 0);
             mode = TM_NORMAL;
             return true;
        }

        return false;
   }

   else if (fmt == TF_YY_MM_DD_hhmmss)   // [20]YY-MM-DD (logfile Date/time: line)
   {
             int YY;
             int MM;
             int DD;
             int hhmmss;
             if (sscanf(arg, " %u-%u-%u %u", &YY, &MM, &DD, &hhmmss) == 4)
             {
                  const int hh = hhmmss/10000;
                  const int mm = hhmmss/100 % 100;
                  const int ss = hhmmss     % 100;
                  days = compute_days(YY, MM - 1, DD - 1);
                  seconds = ss + mm*SECONDS_PER_MINUTE + hh*SECONDS_PER_HOUR;
                  mode = TM_NORMAL;
                  return true;
             }
             else
             {
                  cerr << "Bad TF_YY_MM_DD_hhmmss: " << arg << endl;
             }

        return false;
   }
   assert(0 && "Bad time format");
   return true;
}
//-----------------------------------------------------------------------------
int
TimeDate::get_2(const char * arg)
{
   char c3[] = { arg[0], arg[1], 0 };
   return atoi(c3);
}
//-----------------------------------------------------------------------------
int
TimeDate::read_system_time()
{
   tzset();

   const time_t now = time(0);
   const struct tm * tmp = localtime(&now);

   mode = TM_NORMAL;
   seconds = compute_seconds(tmp->tm_hour, tmp->tm_min, 0);
   days = compute_days(tmp->tm_year % 100, tmp->tm_mon, tmp->tm_mday - 1);

   if (log_control & LOG_DEBUG)
   {
        char buffer[40];
        write_stamp(buffer, sizeof(buffer), TF_20YYMMDD_hhmm);
        cerr << "Using system date/time: " << buffer << "\r\n"
             << "year  = " << (tmp->tm_year % 100) << "\r\n"
             << "month = " << tmp->tm_mon << "\r\n"
             << "day   = " << tmp->tm_mday << "\r\n"
             << "hour  = " << tmp->tm_hour << "\r\n"
             << "min   = " << tmp->tm_min  << "\r\n";
   }

   return 1;
}
//-----------------------------------------------------------------------------
int
TimeDate::compute_seconds(int hh, int mm, int ss)
{
   return ss + mm*SECONDS_PER_MINUTE + hh*SECONDS_PER_HOUR;
}
//-----------------------------------------------------------------------------
bool
TimeDate::is_leap_year(int YY)
{
   bool ret;

   // a simple formula (see below).
   ret = !(YY & 3);

   // the following would be correct, but since the century is optional,
   // we cannot compute it properly.
   //
   /***
   if (YY & 3)          ret = false;
   else if (YY % 100)   ret = true;
   else                 ret = !((YY / 100) & 3);
   ***/

   return ret;
}
//-----------------------------------------------------------------------------
int
TimeDate::compute_days(int YY, int MM, int DD)
{
   YY %= 100;   // if e.g. 2001 is specified.

   // date calculations are error-prone, check input
   //
   assert(DD >=  0);
   assert(DD <= 30);
   assert(MM >=  0);
   assert(MM <= 11);
   assert(YY >=  0);
   assert(YY <= 99);

   const int days_in_year = DD + days_before[MM];
   const int leap_days_Jan_1  = YY ? 1 + (YY - 1)/4 : 0;
   const int days = YY*365 + leap_days_Jan_1 + days_in_year;

   if (is_leap_year(YY) && MM > 1)   return days + 1;
   return days;
}
//-----------------------------------------------------------------------------
/// return the number of days in month \b MM (0-11)
/// in year \b YY (2000 - 2099).
int
TimeDate::days_in_month(int YY, int MM)
{
   assert(MM >=  0);
   assert(MM <= 11);

   if (MM != 1)   return days_month[MM];   // not february
   return is_leap_year(YY) ? 29 : 28;      // february;
}
//-----------------------------------------------------------------------------
void
TimeDate::write_date(char * buffer, int buflen, TimestampFormat fmt) const
{
   assert(buflen >= 14);

   // special modes...
   //
   if (mode == TM_NO_STAMP)
   {
        strncpy(buffer, "NO-DATE", buflen);
        buffer[buflen - 1] = 0;
        return;
   }

   if (mode == TM_PAST)
   {
        strncpy(buffer, "PAST", buflen);
        buffer[buflen - 1] = 0;
        return;
   }

   if (mode == TM_FUTURE)
   {
        strncpy(buffer, "FUTURE", buflen);
        buffer[buflen - 1] = 0;
        return;
   }

   // normal time/date.
   //
   assert(mode == TM_NORMAL);

   int YY;
   int MM;
   int DD = days;

   // subtract full years.
   //
   for (YY = 0; YY <= 99; ++YY)
   {
         const int days_in_year = is_leap_year(YY) ? 366 : 365;
         if (DD >= days_in_year)   DD -= days_in_year;
         else                      break;
   }

   // subtract full months.
   //
   for (MM = 0; MM <= 11; ++MM)
   {
         const int d_in_month = days_in_month(YY, MM);
         if (DD >= d_in_month)   DD -= d_in_month;
         else                    break;
   }

   if (fmt == TF_20YYMMDD_hhmm || fmt == TF_YYMMDD)
   {
        buffer[0] = '0' + YY/10;         buffer[1] = '0' + YY%10;
        buffer[2] = '0' + (MM + 1)/10;   buffer[3] = '0' + (MM + 1)%10;
        buffer[4] = '0' + (DD + 1)/10;   buffer[5] = '0' + (DD + 1)%10;
        buffer[6] = 0;
        return;
   }

   assert(0 && "Bad time format");
}
//-----------------------------------------------------------------------------
void
TimeDate::write_time(char * buffer, int buflen, TimestampFormat fmt) const
{
   assert(buflen >= 14);

   // special modes...
   //
   if (mode == TM_NO_STAMP)
   {
        strncpy(buffer, "NO-TIME", buflen);
        buffer[buflen - 1] = 0;
        return;
   }

   if (mode == TM_PAST)
   {
        strncpy(buffer, "PAST", buflen);
        buffer[buflen - 1] = 0;
        return;
   }

   if (mode == TM_FUTURE)
   {
        strncpy(buffer, "FUTURE", buflen);
        buffer[buflen - 1] = 0;
        return;
   }

   // normal time/date.
   //
   assert(mode == TM_NORMAL);

   const int mm = (seconds / SECONDS_PER_MINUTE) % 60;
   const int hh = (seconds / SECONDS_PER_HOUR);

   if (fmt == TF_20YYMMDD_hhmm || fmt == TF_hhmm)
   {
        buffer[0] = '0' + hh/10;   buffer[1] = '0' + hh%10;
        buffer[2] = '0' + mm/10;   buffer[3] = '0' + mm%10;
        buffer[4] = 0;
        return;
   }

   assert(0 && "Bad time format");
}
//-----------------------------------------------------------------------------
bool
TimeDate::operator <(const TimeDate & other) const
{
   if (mode == TM_PAST)           return true;
   if (mode == TM_FUTURE)         return false;

   if (other.mode == TM_PAST)     return false;
   if (other.mode == TM_FUTURE)   return true;

   if (mode != TM_NORMAL)
   {
        cerr << "Mode = " << mode << endl;
        assert(0 && "comparison against bad TimeDate");
   }

   if (other.mode != TM_NORMAL)
   {
        cerr << "other mode = " << other.mode << endl;
        assert(0 && "comparison against bad TimeDate");
   }

   if (days < other.days)   return true;
   if (days == other.days)   return seconds < other.seconds;
   return false;
}
//-----------------------------------------------------------------------------
TimeDate
TimeDate::operator -(const int sec) const
{
   TimeDate ret = *this;
   ret.seconds -= sec;
   while (ret.seconds < 0)
   {
       --ret.days;
       ret.seconds += SECONDS_PER_DAY;
   }

   return ret;
}
//-----------------------------------------------------------------------------
void
TimeDate::write_stamp(char * buffer, int buflen, TimestampFormat fmt) const
{
   assert(buflen >= 14);

   // special modes...
   //
   if (mode == TM_NO_STAMP)
   {
        strncpy(buffer, "NO-STAMP", buflen);
        buffer[buflen - 1] = 0;
        return;
   }

   if (mode == TM_PAST)
   {
        strncpy(buffer, "PAST", buflen);
        buffer[buflen - 1] = 0;
        return;
   }

   if (mode == TM_FUTURE)
   {
        strncpy(buffer, "FUTURE", buflen);
        buffer[buflen - 1] = 0;
        return;
   }

   // normal time/date.
   //
   assert(mode == TM_NORMAL);

   write_date(buffer, buflen, fmt);
   int len = strlen(buffer);
   if (fmt == TF_20YYMMDD_hhmm)
   {
        buffer[len++] = '-';
        buffer[len] = 0;
        write_time(buffer + len, buflen - len, fmt);
   }
}
//-----------------------------------------------------------------------------
void
TimeDate::set_days(int d)
{
   assert(days == 0);
   days = d;
   mode = TM_NORMAL;
}
//-----------------------------------------------------------------------------
void
TimeDate::set_seconds(int s)
{
   assert(seconds == 0);
   seconds = s;
   mode = TM_NORMAL;
}
//-----------------------------------------------------------------------------
void
TimeDate::do_progress()
{
   progress_count = 0;

   enum { BAR_MS = 400 };

   const long long now = get_ms();
   if (now < (last_progress_ms + BAR_MS))   return;

   const char bar = "|/-\\"[bar_pos++ & 3];
   cerr << " (" << bar << ")\x08\x08\x08\x08";

   if (now < (last_progress_ms + 10*BAR_MS))   last_progress_ms += BAR_MS;
   else                                        last_progress_ms = now;
}
//-----------------------------------------------------------------------------
/// get milliseconds since Jan.1 1970 (linux) or 1601 (windows).
long long
TimeDate::get_ms()
{

   timeval tv = {0, 0};
   gettimeofday(&tv, 0);
   return 1000ULL*tv.tv_sec + tv.tv_usec/1000;


// - the following doesn't compile.
#if 0

FILETIME ft;
   GetSystemTimeAsFileTime(&ft);

long long ret = ft.dwHighDateTime;
   ret <= 32;
   ret |= ft.dwLowDateTime;
   return ret/10;
#endif

}
//-----------------------------------------------------------------------------

/*@}*/

