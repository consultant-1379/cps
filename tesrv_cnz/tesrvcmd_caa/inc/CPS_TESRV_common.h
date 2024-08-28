//#<heading>
//----------------------------------------------------------------------------------------
//
//  FILE
//      CPS_TESRV_common.h
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

#ifndef CPS_TESRV_common_H_
#define CPS_TESRV_common_H_

#include <string>
#include <iostream>
#include <sys/time.h>
#include <stdint.h>
#include <string>
#include <set>

#include <boost/logic/tribool.hpp>
#include <ACS_CS_API.h>

namespace CPS_TESRV {

class Common
{
public:
   enum ApNodeName
   {
       UNKNOW = -1,
       AP1 = 0,
       AP2 = 1
   };
   Common();
   ~Common();
   static ApNodeName getApNode(void);

private:

};

//========================================================================================
// Class Time
//========================================================================================

class Time
{
   friend class Period;

public:

   Time();
   ~Time();

   bool operator==(
         const Time& time
         ) const;

   bool operator<(
         const Time& time
         ) const;

   bool operator>(
         const Time& time
         ) const;

   bool operator<=(
         const Time& time
         ) const;

   bool operator>=(
         const Time& time
         ) const;

   Time operator+(
         const time_t& time
         ) const;

   Time operator-(
         const time_t& time
         ) const;

   Time& operator+=(
         const time_t& time
         );

   Time& operator-=(
         const time_t& time
         );

   time_t operator-(const Time& time) const;

   operator std::string() const;
   operator timeval() const;
   bool empty() const;

   void clear();

   void setDate(
         const std::string& date
         );

   void setTime(
         const std::string& time,
         bool seconds = true
         );

   void set(
         const std::string& date,
         const std::string& time,
         bool seconds = true
         );
   static const Time s_mintime;
   static const Time s_maxtime;

   static const time_t s_second;
   static const time_t s_minute;
   static const time_t s_hour;
   static const time_t s_day;

private:

   Time(
         timeval time
         );

   Time(
         time_t tv_sec,
         suseconds_t tv_usec
         );


   void validate() const;

   bool isLeapYear(int year);

   bool checkInvalidDate(tm tmdate);

   friend std::ostream& operator<<(
         std::ostream& s, const Time& time
         );

   static Time min();
   static Time max();

   timeval m_time;
   bool m_empty;

};

//========================================================================================
// Class Period
//========================================================================================

class Period
{
   friend std::ostream& operator<<(
         std::ostream& s,
         const Period& period
         );

public:
   Period();
   Period(
         const std::string& startdate,
         const std::string& starttime,
         const std::string& stopdate,
         const std::string& stoptime
         );

   ~Period();

private:
   Time m_first;
   Time m_last;
};


class CPTable;

//========================================================================================
// Class CPInfo
//========================================================================================

class CPInfo
{
   friend class CPTable;

public:
   // Constructor
   CPInfo();

   // Destructor
   virtual ~CPInfo();

   // Get CP identity
   CPID getCPID() const;                   // Returns CP identity

private:
   // Constructor
   CPInfo(
         CPID cpId,                       // CP identity
         ACS_CS_API_CP* m_cpInstance      // CP table instance
         );

   CPID m_cpId;                           // CP identity
   ACS_CS_API_CP* m_cpInstance;           // CP table instance
};

//========================================================================================
// Class CPTable
//========================================================================================

class CPTable: public ACS_CS_API_CpTableObserver
{
   friend class CPInfo;

   typedef std::set<CPID> CPIDLIST;
   typedef CPIDLIST::const_iterator CPIDLISTCITER;
   typedef void (*t_callback)(void*);

public:
   // Class const_iterator
   class const_iterator
   {
      friend class CPTable;

   public:
      // Constructor
      const_iterator();

      // Destructor
      ~const_iterator();

      // Reference operator
      const CPInfo* operator->();

      // Pointer operator
      const CPInfo& operator*();

      // Increment operator (prefix)
      const_iterator& operator++();

      // Equality operator
      bool operator==(
            const const_iterator& iter
            ) const;

      // Unequality operator
      bool operator!=(
            const const_iterator& iter
            ) const;

   private:
      // Constructor
      const_iterator(
            CPIDLISTCITER iter,
            ACS_CS_API_CP* m_cpInstance
            );

      CPIDLISTCITER m_iter;
      CPInfo m_cpInfo;
   };

   // Constructors
   CPTable();

   // Destructor
   virtual ~CPTable();

   // Find CP info for CP name in a multi CP system
   const_iterator find(                   // Returns iterator
         const std::string& name          // CP name
         ) const;

   // Find CP info for CPID in a multi CP system
   const_iterator find(                   // Returns iterator
         CPID cpId                        // CP id
         ) const;


   // Get first element in list
   const_iterator begin() const;

   // Get beyond last element in list
   const_iterator end() const;

   // Inquire if this is a multiple CP system
   static bool isMultiCPSystem();          // Returns true if multiple CP system,
                                          // false otherwise
   void update(const ACS_CS_API_CpTableChange& observer);                                       

private:
   // Initialize CP table
   void init();
   
   ACS_CS_API_CP* m_cpInstance;           // CP table instance
   CPIDLIST m_cpIdList;                   // List of CP ID:s
   void* m_instptr;                       // Instance pointer
   t_callback m_callback;                 // Callback function
   bool m_issubscribed;                   // Check if CP is subscribed

   static boost::tribool s_multiCPSystem; // True if multi CP system
};
   
}

#endif
