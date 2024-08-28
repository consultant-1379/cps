#ifndef CPS_CLOG_TimeDate_h
#define CPS_CLOG_TimeDate_h



/**
 ** \addtogroup cmdlls CMDLLS command
 **/
/*@{*/
class TimeDate;
//-----------------------------------------------------------------------------
/// the mode of a timestamp
enum TimestampMode
{
  TM_NO_STAMP  = 0,   ///< no timestamp
  TM_PAST      = 1,   ///< a timestamp after than all others
  TM_FUTURE    = 2,   ///< a timestamp before all others
  TM_NORMAL    = 3,   ///< date and time
  TM_INVALID   = 4,   ///< invalid time stamp (error in constructor).
};

/// the format of a timestamp (for scanning and printing it)
enum TimestampFormat
{
   TF_20YYMMDD_hhmm,     ///< [20]YYMMDD-hhmm, "none", "past", or "future"
   TF_YYMMDD,            ///< YYMMDD (DATE= parameter)
   TF_hhmm,              ///< hhmm   (TIME= parameter)
   TF_YY_MM_DD_hhmmss,   ///< YY-MM-DD hhmmss   (Time/Date: in log file)
};

/// a timestamp.
class TimeDate
{
public:
   /// Constructor.
   TimeDate()
   : mode(TM_INVALID),
     seconds(0),
     days(0) {}
   /// initialize \b this TimeDate from \b arg of format \b fmt
   bool parse(const char * arg, TimestampFormat fmt);

   /// initialize \b this TimeDate from the system clock
   int read_system_time();

   /// return the timestamp mode
   TimestampMode get_mode() const   { return mode; }

   /// return the days since 1.1.2000
   int get_days() const   { return days; }

   /// return the seconds since 00:00:00
   int get_seconds() const   { return seconds; }

   /// set the days since 1.1.2000
   void set_days(int d);

   /// set the seconds since 00:00:00.
   void set_seconds(int s);

   /// return true iff \b this time lies before \b other
   bool operator <(const TimeDate & other) const;

   /// return a TimeDate \b sec seconds before \b this TimeDate
   TimeDate operator -(const int sec) const;

   /// write date into \b buffer of size \b buflen with format \b fmt
   void write_date(char * buffer, int buflen, TimestampFormat fmt) const;

   /// write time into \b buffer of size \b buflen with format \b fmt
   void write_time(char * buffer, int buflen, TimestampFormat fmt) const;

   /// write date and time into \b buffer of size \b buflen with format \b fmt
   void write_stamp(char * buffer, int buflen, TimestampFormat fmt) const;

   /// seconds.
   enum Seconds
      {
        SECONDS_PER_MINUTE  = 60,                      ///<     = 60
        SECONDS_PER_HOUR    = 60*SECONDS_PER_MINUTE,   ///<  = 3,600
        SECONDS_PER_DAY     = 24*SECONDS_PER_HOUR,     ///< = 86,400
      };

   /// maybe advance a rotation bar.
   static void do_progress();

   /// a counter to limit the number of calls to do_progress();
   static int progress_count;

protected:
   /// days in months of non-leap years.
   enum DayInMonth
   {
     DAYS_IN_JAN = 31,   ///< 31 days in January
     DAYS_IN_FEB = 28,   ///< 28 days in February (non-leap year)
     DAYS_IN_MAR = 31,   ///< 31 days in March
     DAYS_IN_APR = 30,   ///< 30 days in April
     DAYS_IN_MAY = 31,   ///< 31 days in May
     DAYS_IN_JUN = 30,   ///< 30 days in June
     DAYS_IN_JUL = 31,   ///< 31 days in July
     DAYS_IN_AUG = 31,   ///< 31 days in August
     DAYS_IN_SEP = 30,   ///< 30 days in September
     DAYS_IN_OCT = 31,   ///< 31 days in October
     DAYS_IN_NOV = 30,   ///< 30 days in November
     DAYS_IN_DEC = 31,   ///< 31 days in December
   };

   /// days before months of non-leap years.
   enum DaysBeforeMonth
   {
     BEFORE_JAN    = 0,                          ///< days before January 1.
     BEFORE_FEB    = BEFORE_JAN + DAYS_IN_JAN,   ///< days before February 1.
     BEFORE_MAR    = BEFORE_FEB + DAYS_IN_FEB,   ///< days before March 1.
     BEFORE_APR    = BEFORE_MAR + DAYS_IN_MAR,   ///< days before April 1.
     BEFORE_MAY    = BEFORE_APR + DAYS_IN_APR,   ///< days before May 1.
     BEFORE_JUN    = BEFORE_MAY + DAYS_IN_MAY,   ///< days before June 1.
     BEFORE_JUL    = BEFORE_JUN + DAYS_IN_JUN,   ///< days before July 1.
     BEFORE_AUG    = BEFORE_JUL + DAYS_IN_JUL,   ///< days before August 1.
     BEFORE_SEP    = BEFORE_AUG + DAYS_IN_AUG,   ///< days before September 1.
     BEFORE_OCT    = BEFORE_SEP + DAYS_IN_SEP,   ///< days before October 1.
     BEFORE_NOV    = BEFORE_OCT + DAYS_IN_OCT,   ///< days before November 1.
     BEFORE_DEC    = BEFORE_NOV + DAYS_IN_NOV,   ///< days before December 1.
     DAYS_PER_YEAR = BEFORE_DEC + DAYS_IN_DEC
   };

   /// the type of timestamp.
   TimestampMode mode;

   /// seconds since 00:00:00
   int seconds;

   /// days since 1.1.2000
   int days;

   /// return seconds from 00:00:00 at time hh:mm:ss
   int compute_seconds(int hh, int mm, int ss);

   /// return days from 1.1.2000 at DD (0-30), MM (0-11), and YY (0-99).
   int compute_days(int YY, int MM, int DD);

   /// return true if YY is a leap year.
   static bool is_leap_year(int YY);

   /// return days in month MM in year YY.
   static int days_in_month(int YY, int MM);

   /// return two ASCII digits as integer (0-99).
    static int get_2(const char * arg);

   static long long get_ms();
   /// days before 1. of month (january == month 0).
   static const int days_before[12];

   /// days in month (january == month 0).
   static const int days_month[12];

   /// milliseconds of last progress bar change.
   static long long last_progress_ms;

   /// where the progress bar points (| / - or \)
   static int bar_pos;
};
//-----------------------------------------------------------------------------
/*@}*/

#endif // TimeDate_h
