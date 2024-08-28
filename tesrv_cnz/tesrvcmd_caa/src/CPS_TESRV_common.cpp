//#<heading>
//----------------------------------------------------------------------------------------
//
//  FILE
//      CPS_TESRV_common.cpp
//
//  AUTHOR
//          XTUNGVU
//
//  REVISION HISTORY
//      Rev.   Date         Prepared    Description
//      ----   ----         --------    -----------
//      PA1    2013-11-27   XTUNGVU      TR HR97275
//
//  SEE ALSO
//      -
//
//----------------------------------------------------------------------------------------
//#</heading>

#include "CPS_TESRV_common.h"
#include "CPS_TESRV_errors.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include <acs_apgcc_paramhandling.h>
#include <ACS_APGCC_CommonLib.h>
#include <ACS_DSD_Client.h>

#include <iostream>
#include <string>
#include <iomanip>
#include <cassert>
#include <algorithm>
#include <errno.h>

using namespace std;

namespace CPS_TESRV
{
const Time Time::s_mintime = Time::min();       // 1601-01-01 00:00:00.000000
const Time Time::s_maxtime = Time::max();       // 10000-01-01 00:00:00.000000

const time_t Time::s_second = 1;                // Second
const time_t Time::s_minute = 60;               // Minute
const time_t Time::s_hour = 3600;               // Hour
const time_t Time::s_day = 86400;               // Day

    
//========================================================================================
//	Class CPInfo
//========================================================================================

//----------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------
CPInfo::CPInfo() :
m_cpId(~CPID()),
m_cpInstance(0)
{
   if (CPTable::isMultiCPSystem() == false)
   {
      m_cpId = 1001;
   }
}

CPInfo::CPInfo(CPID cpId, ACS_CS_API_CP* m_cpInstance) :
m_cpId(cpId),
m_cpInstance(m_cpInstance)
{
}

//----------------------------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------------------------
CPInfo::~CPInfo()
{
}

//----------------------------------------------------------------------------------------
// Get CPID
//----------------------------------------------------------------------------------------
CPID CPInfo::getCPID() const
{
   return m_cpId;
}
//========================================================================================
// Class CPTable::const_iterator
//========================================================================================

//----------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------
CPTable::const_iterator::const_iterator():
m_iter(),
m_cpInfo()
{
}

CPTable::const_iterator::const_iterator(CPIDLISTCITER iter, ACS_CS_API_CP* m_cpInstance):
m_iter(iter),
m_cpInfo()
{
   m_cpInfo.m_cpInstance = m_cpInstance;
}

//----------------------------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------------------------
CPTable::const_iterator::~const_iterator()
{
}

//----------------------------------------------------------------------------------------
// Reference operator
//----------------------------------------------------------------------------------------
const CPInfo* CPTable::const_iterator::operator->()
{
   m_cpInfo.m_cpId = *m_iter;
   return &m_cpInfo;
}

//----------------------------------------------------------------------------------------
// Pointer operator
//----------------------------------------------------------------------------------------
const CPInfo& CPTable::const_iterator::operator*()
{
   m_cpInfo.m_cpId = *m_iter;
   return m_cpInfo;
}

//----------------------------------------------------------------------------------------
// Increment operator (prefix)
//----------------------------------------------------------------------------------------
CPTable::const_iterator& CPTable::const_iterator::operator++()
{
   ++m_iter;
   return *this;
}

//----------------------------------------------------------------------------------------
// Equality operator
//----------------------------------------------------------------------------------------
bool CPTable::const_iterator::operator==(const const_iterator& iter) const
{
   return m_iter == iter.m_iter;
}

//----------------------------------------------------------------------------------------
// Unequality operator
//----------------------------------------------------------------------------------------
bool CPTable::const_iterator::operator!=(const const_iterator& iter) const
{
   return m_iter != iter.m_iter;
}


//========================================================================================
// Class CPTable
//========================================================================================

boost::tribool CPTable::s_multiCPSystem(boost::indeterminate);

//----------------------------------------------------------------------------------------
//   Constructors
//----------------------------------------------------------------------------------------
CPTable::CPTable():
m_cpInstance(),
m_cpIdList(),
m_instptr(0),
m_callback(0)
{
   if (isMultiCPSystem() == true)
   {
      // Initialize CP table
      init();
   }
}

//----------------------------------------------------------------------------------------
//   Destructor
//----------------------------------------------------------------------------------------
CPTable::~CPTable()
{
   if (s_multiCPSystem == true)
   {
      ACS_CS_API::deleteCPInstance(m_cpInstance);
      m_cpInstance = NULL;
   }
}

//----------------------------------------------------------------------------------------
// Initialize CP table
//----------------------------------------------------------------------------------------
void CPTable::init()
{
   // Create CP instance
   m_cpInstance = ACS_CS_API::createCPInstance();
   if (m_cpInstance == 0)
   {
   	   // Convert from error code 101 (System error) to 1 as design rule
   	      throw TESRV_EXECUTE_ERROR;
   }

   // Get CP ID list
   ACS_CS_API_IdList cpIdList;
   ACS_CS_API_NS::CS_API_Result result = m_cpInstance->getCPList(cpIdList);
   if (result != ACS_CS_API_NS::Result_Success)
   {
   	   // Convert from error code 101 (System error) to 1 as design rule
   	      throw TESRV_EXECUTE_ERROR;
   }

   for (size_t index = 0; index < cpIdList.size(); ++index)
   {
      m_cpIdList.insert(cpIdList[index]);
   }
}

//----------------------------------------------------------------------------------------
// Find CPInfo for CP name in a multi CP system
//----------------------------------------------------------------------------------------
CPTable::const_iterator CPTable::find(const string& name) const
{
   if (s_multiCPSystem == true)
   {
      ACS_CS_API_NS::CS_API_Result result;
      CPID cpId;

      // Get CP name
      result = m_cpInstance->getCPId(boost::to_upper_copy(name).c_str(), cpId);
      if (result == ACS_CS_API_NS::Result_Success)
      {
         return const_iterator(m_cpIdList.find(cpId), m_cpInstance);
      }
      else if (result == ACS_CS_API_NS::Result_NoEntry)
      {
         return end();
      }
      else
      {
      	   // Convert from error code 101 (System error) to 1 as design rule
      	      throw TESRV_EXECUTE_ERROR;
      }
   }
   else
   {
   	   // Convert from error code 102 (Internal error) to 1 as design rule
   	      throw TESRV_EXECUTE_ERROR;
   }
}

//----------------------------------------------------------------------------------------
// Find CPInfo for CPID in a multi CP system
//----------------------------------------------------------------------------------------
CPTable::const_iterator CPTable::find(CPID cpId) const
{
   if (s_multiCPSystem == true)
   {
      return const_iterator(m_cpIdList.find(cpId), m_cpInstance);
   }
   else
   {
   	   // Convert from error code 102 (Internal error) to 1 as design rule
   	      throw TESRV_EXECUTE_ERROR;
   }
}
//----------------------------------------------------------------------------------------
// Get first element in list
//----------------------------------------------------------------------------------------
CPTable::const_iterator CPTable::begin() const
{
   if (s_multiCPSystem == true)
   {
      return const_iterator(m_cpIdList.begin(), m_cpInstance);
   }
   else
   {
   	   // Convert from error code 102 (Internal error) to 1 as design rule
   	      throw TESRV_EXECUTE_ERROR;
   }
}

//----------------------------------------------------------------------------------------
// Get beyond last element in list
//----------------------------------------------------------------------------------------
CPTable::const_iterator CPTable::end() const
{
   if (s_multiCPSystem == true)
   {
      return const_iterator(m_cpIdList.end(), m_cpInstance);
   }
   else
   {
   	   // Convert from error code 102 (Internal error) to 1 as design rule
   	      throw TESRV_EXECUTE_ERROR;
   }
}

//----------------------------------------------------------------------------------------
//   Check if this is a multiple CP system
//----------------------------------------------------------------------------------------
bool CPTable::isMultiCPSystem()
{
   if (boost::indeterminate(s_multiCPSystem))
   {
      bool multiCPSystem;
      ACS_CS_API_NS::CS_API_Result result;
      result = ACS_CS_API_NetworkElement::isMultipleCPSystem(multiCPSystem);
      if (result != ACS_CS_API_NS::Result_Success)
      {
   	   // Convert from error code 101 (System error) to 1 as design rule
   	      throw TESRV_EXECUTE_ERROR;
      }
      s_multiCPSystem = multiCPSystem;
   }
   return s_multiCPSystem;
}

//----------------------------------------------------------------------------------------
// Notification that the HWC table has been updated
//----------------------------------------------------------------------------------------
void CPTable::update(const ACS_CS_API_CpTableChange& observer)
{
   ACS_CS_API_CpTableData *data = observer.cpData;
   bool needReset = false;
   CPID cpid_obs = data->cpId;
   uint16_t apzsys_obs = data->apzSystem;
   const_iterator citer = find(cpid_obs);

   if (citer != end())
   {
      CPInfo cpinfo = *citer;
      CPID cpid = cpinfo.getCPID();
      ACS_CS_API_NS::CS_API_Result result;
      uint16_t tapzsystem;
      result = m_cpInstance->getAPZSystem(cpid, tapzsystem);
      if (result != ACS_CS_API_NS::Result_Success)
      {
         if (result != ACS_CS_API_NS::Result_NoValue)
         {
            tapzsystem = 0;
         }
      }
      if (cpid == cpid_obs && tapzsystem != apzsys_obs)
      {
         needReset = true;
         char *buff = new char[1024];
         size_t size = sizeof(buff);

         data->cpName.getName(buff, size);
      }
   }

   if (needReset)
   {
      // Execute callback function
      if (m_callback != 0)
      {
         m_callback(m_instptr);
      }
   }
}

//========================================================================================
// Class Time
//========================================================================================

//----------------------------------------------------------------------------------------
//	Constructors
//----------------------------------------------------------------------------------------
Time::Time() :
m_time(),
m_empty(true)
{
}

Time::Time(timeval time) :
m_time(time),
m_empty(false)
{
   validate();
}

Time::Time(time_t tv_sec, suseconds_t tv_usec) :
m_time(),
m_empty(false)
{
   m_time.tv_sec = tv_sec;
   m_time.tv_usec = tv_usec;
   validate();
}

//----------------------------------------------------------------------------------------
//   Destructor
//----------------------------------------------------------------------------------------
Time::~Time()
{
}

//----------------------------------------------------------------------------------------
//   Equality operator
//----------------------------------------------------------------------------------------
bool Time::operator==(const Time& time) const
{
   validate();
   time.validate();
   return timercmp(&m_time, &time.m_time, ==);
}

//----------------------------------------------------------------------------------------
//   Less than operator
//----------------------------------------------------------------------------------------
bool Time::operator<(const Time& time) const
{
   validate();
   time.validate();
   return timercmp(&m_time, &time.m_time, <);
}

//----------------------------------------------------------------------------------------
//   Greater than operator
//----------------------------------------------------------------------------------------
bool Time::operator>(const Time& time) const
{
   validate();
   time.validate();
   return timercmp(&m_time, &time.m_time, >);
}

//----------------------------------------------------------------------------------------
//   Less than or equality operator
//----------------------------------------------------------------------------------------
bool Time::operator<=(const Time& time) const
{
   validate();
   time.validate();
   return !timercmp(&m_time, &time.m_time, >);
}

//----------------------------------------------------------------------------------------
//   Greater than or equality operator
//----------------------------------------------------------------------------------------
bool Time::operator>=(const Time& time) const
{
   validate();
   time.validate();
   return !timercmp(&m_time, &time.m_time, <);
}

//----------------------------------------------------------------------------------------
// timeval operator
//----------------------------------------------------------------------------------------
Time::operator timeval() const
{
   validate();
   return m_time;
}

//----------------------------------------------------------------------------------------
//   Add a time value
//----------------------------------------------------------------------------------------
Time Time::operator+(const time_t& time) const
{
   validate();
   timeval res1 = {time, 0};
   timeval res2;
   timeradd(&m_time, &res1, &res2);

   return Time(res2.tv_sec, res2.tv_usec);
}

//----------------------------------------------------------------------------------------
//   Subtract a time value
//----------------------------------------------------------------------------------------
Time Time::operator-(const time_t& time) const
{
   validate();
   timeval res1 = {time, 0};
   timeval res2;
   timersub(&m_time, &res1, &res2);

   return Time(res2.tv_sec, res2.tv_usec);
}

//----------------------------------------------------------------------------------------
//   Add a time value, modify object
//----------------------------------------------------------------------------------------
Time& Time::operator+=(const time_t& time)
{
   validate();
   timeval res = {time, 0};
   timeradd(&m_time, &res, &m_time);

   return *this;
}

//----------------------------------------------------------------------------------------
//   Subtract a time value, modify object
//----------------------------------------------------------------------------------------
Time& Time::operator-=(const time_t& time)
{
   validate();
   timeval res = {time, 0};
   timersub(&m_time, &res, &m_time);

   return *this;
}

//----------------------------------------------------------------------------------------
// Calculate difference between two time objects
//----------------------------------------------------------------------------------------
time_t Time::operator-(const Time& time) const
{
   validate();
   time.validate();
   timeval res;
   timersub(&m_time, &time.m_time, &res);

   return res.tv_sec;
}

//----------------------------------------------------------------------------------------
//   Set date - time will be set to 00:00:00
//----------------------------------------------------------------------------------------
void Time::setDate(const string& date)
{
   // Expected format: YYYYMMDD
   string tdate = date;
   if (date.size() == 8)
   {
      tdate.insert(6, 1, '-');
      tdate.insert(4, 1, '-');
   }
   else
   {
      throw TESRV_ILLEGAL_DATE;
   }

   tm tmtime;
   tmtime.tm_isdst = -1;
   tmtime.tm_hour = 0;
   tmtime.tm_min = 0;
   tmtime.tm_sec = 0;

   const char* ptr = strptime(tdate.c_str(), "%F", &tmtime);
   if (!ptr)
   {
      throw TESRV_ILLEGAL_DATE;
   }
   if (*ptr != 0)
   {
      throw TESRV_ILLEGAL_DATE;
   }
   if (checkInvalidDate(tmtime))
   {
      throw TESRV_ILLEGAL_DATE;
   }

   m_time.tv_sec = mktime(&tmtime);
   m_time.tv_usec = 0;

   // Validate date
   if (timercmp(&m_time, &s_mintime.m_time, <) ||
       !timercmp(&m_time, &s_maxtime.m_time, <))
   {
      throw TESRV_ILLEGAL_DATE;
   }
   m_empty = false;
}

//----------------------------------------------------------------------------------------
//   Set time - date will be set to today
//----------------------------------------------------------------------------------------
void Time::setTime(const string& time, bool seconds)
{
   tm tmtime;

   // Expected format: HHMM[SS]
   string ttime = time;
   if (time.size() == 4)
   {
      ttime.insert(2, 1, ':');
      ttime.append(":00");
   }
   else if (time.size() == 6 && seconds)
   {
      ttime.insert(4, 1, ':');
      ttime.insert(2, 1, ':');
   }
   else
   {
      throw TESRV_ILLEGAL_TIME;
   }

   timeval tval;

   // Get current date and time
   int res = gettimeofday(&tval, NULL);
   if (res != 0)
   {
      // Convert from 102 error code (Internal) to 1
      throw TESRV_EXECUTE_ERROR;;
   }

   tm* tmtimep = localtime_r(&tval.tv_sec, &tmtime);
   if (tmtimep == 0)
   {
      throw TESRV_ILLEGAL_TIME;
   }

   const char* ptr = strptime(ttime.c_str(), "%T", &tmtime);
   if (!ptr)
   {
      throw TESRV_ILLEGAL_TIME;
   }

   m_time.tv_sec = mktime(&tmtime);
   m_time.tv_usec = 0;

   m_empty = false;
}

//----------------------------------------------------------------------------------------
// Set date and time
//----------------------------------------------------------------------------------------
void Time::set(const string& date, const string& time, bool seconds)
{
    // Expected format: YYYYMMDD
    string tdate = date;
    if (date.size() == 8)
    {
       tdate.insert(6, 1, '-');
       tdate.insert(4, 1, '-');
    }
    else
    {
       throw TESRV_ILLEGAL_DATE;
    }

    const char* ptr;
    tm tmtime;
    tmtime.tm_isdst = -1;
    ptr = strptime(tdate.c_str(), "%F", &tmtime);
    if (!ptr)
    {
       throw TESRV_ILLEGAL_DATE;
    }
    if (*ptr != 0)
    {
       throw TESRV_ILLEGAL_DATE;
    }
    if (checkInvalidDate(tmtime))
    {
    	throw TESRV_ILLEGAL_DATE;
    }

    // Expected format: HHMM[SS]
    string ttime = time;
    if (time.size() == 4)
    {
       ttime.insert(2, 1, ':');
       ttime.append(":00");
    }
    else if (time.size() == 6 && seconds)
    {
       ttime.insert(4, 1, ':');
       ttime.insert(2, 1, ':');
    }
    else
    {
       throw TESRV_ILLEGAL_TIME;
    }

    ptr = strptime(ttime.c_str(), "%T", &tmtime);
    if (!ptr)
    {
       throw TESRV_ILLEGAL_TIME;
    }

    m_time.tv_sec = mktime(&tmtime);

    // Validate date
    if (timercmp(&m_time, &s_mintime.m_time, <) ||
        !timercmp(&m_time, &s_maxtime.m_time, <))
    {
       throw TESRV_ILLEGAL_DATE;
    }
    m_empty = false;
}
//----------------------------------------------------------------------------------------
// Check leap year
//----------------------------------------------------------------------------------------
bool Time::isLeapYear(int year)
{
   if((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)
      return true; /* leap */
    else
       return false; /* no leap */
}

//----------------------------------------------------------------------------------------
// Check if the date is invalid
//----------------------------------------------------------------------------------------
bool Time::checkInvalidDate(tm tmdate)
{
   int year = tmdate.tm_year + 1900;
   int month = tmdate.tm_mon + 1;
   int day = tmdate.tm_mday;

   switch (month)
   {
   case 2:
      if (isLeapYear(year))
      {
         if (day > 29)
         {
            return true;
         }
      }
      else
      {
         if (day > 28)
         {
            return true;
         }
      }
      break;
   case 4:
   case 6:
   case 9:
   case 11:
      if (day > 30)
      {
         return true;
      }
      break;
   }

   return false;
}

//----------------------------------------------------------------------------------------
// Clear the time
//----------------------------------------------------------------------------------------
void Time::clear()
{
   m_empty = true;
}
//----------------------------------------------------------------------------------------
// Get min time (1601-01-01 00:00:00.000000)
//----------------------------------------------------------------------------------------
Time Time::min()
{
   tm tmtime;
   tmtime.tm_isdst = -1;
   tmtime.tm_year = 1601 - 1900;
   tmtime.tm_mon = 0;
   tmtime.tm_mday = 1;
   tmtime.tm_hour = 0;
   tmtime.tm_min = 0;
   tmtime.tm_sec = 0;

   time_t tv_sec = mktime(&tmtime);
   if (tv_sec == -1)
   {
      // Convert from 102 error code (Internal) to 1
      throw ;
   }

   return Time(tv_sec, 0);
}

//----------------------------------------------------------------------------------------
// Get max time (10000-01-01 00:00:00.000000)
//----------------------------------------------------------------------------------------
Time Time::max()
{
   tm tmtime;
   tmtime.tm_isdst = -1;
   tmtime.tm_year = 10000 - 1900;
   tmtime.tm_mon = 0;
   tmtime.tm_mday = 1;
   tmtime.tm_hour = 0;
   tmtime.tm_min = 0;
   tmtime.tm_sec = 0;

   time_t tv_sec = mktime(&tmtime);
   if (tv_sec == -1)
   {
      // Convert from 102 error code (Internal) to 1
      throw TESRV_EXECUTE_ERROR;
   }

   return Time(tv_sec, 0);
}
//----------------------------------------------------------------------------------------
// Check if time is empty
//----------------------------------------------------------------------------------------
bool Time::empty() const
{
   return m_empty;
}

//----------------------------------------------------------------------------------------
// Assert that time is valid
//----------------------------------------------------------------------------------------
void Time::validate() const
{
   if (empty())
   {
      // Convert from 102 error code (Internal) to 1
      throw TESRV_EXECUTE_ERROR;
   }
   if (timercmp(&m_time, &s_mintime.m_time, <) ||
       timercmp(&m_time, &s_maxtime.m_time, >))
   {
      // Convert from 100 error code (Param ) to 1
      throw TESRV_EXECUTE_ERROR;
   }
}
//========================================================================================
// Class Period
//========================================================================================

//----------------------------------------------------------------------------------------
//   Constructors
//----------------------------------------------------------------------------------------
Period::Period():
m_first(Time::s_mintime),
m_last(Time::s_maxtime)
{
   m_last -= Time::s_second;
   m_last.m_time.tv_usec = 999999;
}


Period::Period(
         const string& startdate,
         const string& starttime,
         const string& stopdate,
         const string& stoptime
         ) :
m_first(),
m_last()
{
   // Start time
   if (!starttime.empty() && !startdate.empty())
   {
      // Start time and date provided
      m_first.set(startdate, starttime, false);
   }
   else if (starttime.empty() && !startdate.empty())
   {
      // Start date provided
      m_first.setDate(startdate);
   }
   else if (!starttime.empty() && startdate.empty())
   {
      // Start time provided
      m_first.setTime(starttime, false);
   }
   else
   {
      // No start time or date provided
      m_first = Time::s_mintime;
   }

   // Stop time
   if (!stoptime.empty() && !stopdate.empty())
   {
      // Stop time and date provided
      m_last.set(stopdate, stoptime, false);
      m_last += Time::s_minute - Time::s_second;
      m_last.m_time.tv_usec = 999999;
   }
   else if (stoptime.empty() && !stopdate.empty())
   {
      // Stop date provided
      m_last.setDate(stopdate);
      m_last += Time::s_day - Time::s_second;
      m_last.m_time.tv_usec = 999999;
   }
   else if (!stoptime.empty() && stopdate.empty())
   {
      // Stop time provided
      m_last.setTime(stoptime, false);
      m_last += Time::s_minute - Time::s_second;
      m_last.m_time.tv_usec = 999999;
   }
   else
   {
      // No stop time or date provided
      m_last = Time::s_maxtime;
      m_last -= Time::s_second;
      m_last.m_time.tv_usec = 999999;
   }

   // Stop time must not be smaller than start time
   if (m_first > m_last)
   {
      throw TESRV_STARTTIME_GREATER_STOPTIME;
   }
}

//----------------------------------------------------------------------------------------
//   Destructor
//----------------------------------------------------------------------------------------
Period::~Period()
{
}
//======================================================================================================
// Class Common
//======================================================================================================

Common::Common()
{
}

Common::~Common()
{
}

//----------------------------------------------------------------------------------------
// Get AP node name
//----------------------------------------------------------------------------------------
Common::ApNodeName Common::getApNode(void)
{
   ApNodeName apName;
   ACS_DSD_Client dsdClient;
   ACS_DSD_Node ownNode;
   int ret = dsdClient.get_local_node(ownNode);
   if (ret == 0)
   {
      switch (ownNode.system_id)
      {
      case 2001:
      {
         apName = AP1;
         break;
      }
      case 2002:
      {
         apName = AP2;
         break;
      }
      default:
      {
         apName = UNKNOW;
         break;
      }
      }
   }
   else
   {
      apName = UNKNOW;
   }
   return apName;
}

}
