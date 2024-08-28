#ifndef _DATETIME_H_
#define _DATETIME_H_
/**
 * xngudan - based on BUSRV's DateTime
 */

#include <ctime>
#include <assert.h>
#include <ostream>

#include <sys/types.h>
#include <sys/stat.h>

#include "PlatformTypes.h"

using namespace std;

#pragma pack(push, 1)

class Duration;

//
// Date and time data within special segment.
//========================================================================
class DateTime {
// types
public:
	enum LIMITS {
        MAX_YEAR = 99,
        MIN_MONTH = 1,
        MAX_MONTH = 12,
        MIN_DAY = 1,
        MAX_DAY = 31,
        MAX_HOUR = 23,
        MAX_MINUTE = 59,
        MAX_SECOND = 59
	};

    enum {
        HUNDRED_NANOSECS_PER_SEC = 10000000,
        SECS_PER_MINUTE = 60,
        MINUTES_PER_HOUR = 60,
        HOURS_PER_DAY = 24,
        SECS_PER_HOUR = 3600
    };

    enum { BREAK_YEAR = 80 };

public:
    DateTime() { setNull(); }
    DateTime(u_int8 year, u_int8 month, u_int8 day, u_int8 daylight,
            u_int8 hour, u_int8 minute, u_int8 second);
    explicit DateTime(const tm& info);
    DateTime(const DateTime&);
    DateTime operator=(const DateTime&);

    void set(u_int8 year, u_int8 month, u_int8 day, u_int8 daylight,
        u_int8 hour, u_int8 minute, u_int8 second);

    // access
    u_int8 year() const { return m_year; }
    u_int8 month() const { return m_month; }
    u_int8 day() const { return m_day; }
    u_int8 daylight() const { return m_daylight; }
    u_int8 hour() const { return m_hour; }
    u_int8 minute() const { return m_minute; }
    u_int8 second() const { return m_second; }

    // modifiers
    void set(const time_t& info);
    void year(u_int8 val) { m_year = val; }
    void month(u_int8 val) { m_month = val; }
    void day(u_int8 val) { m_day = val; }
    void daylight(u_int8 val) { m_daylight = val; }
    void hour(u_int8 val) { m_hour = val; }
    void minute(u_int8 val) { m_minute = val; }
    void second(u_int8 val) { m_second = val; }

    // funx compare
    int cmp(const DateTime&) const;
    bool operator>(const DateTime& rhs) const;
    bool operator>=(const DateTime& rhs) const;
    bool operator<(const DateTime& rhs) const;
    bool operator<=(const DateTime& rhs) const;
    bool operator==(const DateTime& rhs) const;
    bool operator!=(const DateTime& rhs) const;

    // modify
    void addSeconds(size_t);
    void addMinutes(u_int32);
    void addHours(u_int32);
    void addDays(u_int32);

    // misc
    void now(); // set to current system time
    bool valid() const;
    bool null() const;
    double difftime(const DateTime&) const;
    Duration operator-(const DateTime&) const;

    tm systemtime() const;
    void systemtime(const tm& );

    void setNull() { m_year = m_month = m_day = m_daylight = m_hour = m_minute = m_second = m_filler = 0; }
    friend ostream& operator<<(ostream& os, const DateTime& dt);

private:
    u_int8 m_year;     // Years within the interval 0-99.
    u_int8 m_month;    // Months within the interval 1-12.
    u_int8 m_day;      // Days within the interval 1-31.
    u_int8 m_daylight; // Daylight saving time, i.e. 0 or 1.
    u_int8 m_hour;     // Hours within the interval 0-23.
    u_int8 m_minute;   // Minutes within the interval 0-59.
    u_int8 m_second;   // Seconds within the interval 0-59.
    u_int8 m_filler;   // Filler for W32-alignment
};

class Duration {
public:
	explicit Duration(int64);
	friend ostream& operator<<(ostream& os, const Duration&);
private:
	bool    m_signed;
	u_int32 m_hour;
	u_int8  m_minute;
	u_int8  m_second;
};

#pragma pack(pop)

//
//
//===========================================================================
inline void DateTime::set(u_int8 year, u_int8 month, u_int8 day, u_int8 daylight,
    u_int8 hour, u_int8 minute, u_int8 second) {
    m_year = year;
    m_month = month;
    m_day = day;
    m_daylight = daylight;
    m_hour = hour;
    m_minute = minute;
    m_second = second;
    m_filler = 0;
}
//
// inlines
//===========================================================================
inline DateTime::DateTime(u_int8 year, u_int8 month, u_int8 day, u_int8 daylight,
    u_int8 hour, u_int8 minute, u_int8 second)
{
    set(year, month, day, daylight, hour, minute, second);
    // It is possible to construct a null DateTime. Remove from the base product
    // assert(valid());
}


// inlines
//===========================================================================
inline DateTime DateTime::operator=(const DateTime& dt) {
	set(dt.year(), dt.month(), dt.day(), dt.daylight(), dt.hour(), dt.minute(), dt.second());
	return *this;
}
//
//
//===========================================================================
inline DateTime::DateTime(const tm& info) {
    setNull();
    systemtime(info);
}
//
//
// basic check of valid data
inline bool DateTime::valid() const {
    return (
        m_year <= MAX_YEAR &&
        m_month >= MIN_MONTH && m_month <= MAX_MONTH &&
        m_day >= MIN_DAY && m_day <= MAX_DAY &&
        m_hour <= MAX_HOUR &&
        m_minute <= MAX_MINUTE &&
        m_second <= MAX_SECOND &&
        !m_filler);
}
// all fields zeroed out?
inline bool DateTime::null() const {
    return (!m_year && !m_month && !m_day && !m_hour && !m_minute && !m_second
        && !m_filler);
}

//
inline void DateTime::addMinutes(u_int32 val) { addSeconds(val * SECS_PER_MINUTE); }
//
inline void DateTime::addHours(u_int32 val) { addMinutes(val * MINUTES_PER_HOUR); }
//
inline void DateTime::addDays(u_int32 val) { addHours(val * HOURS_PER_DAY); }

inline bool DateTime::operator>(const DateTime& rhs) const
{ return (cmp(rhs) == 1); }
//
inline bool DateTime::operator>=(const DateTime& rhs) const
{ return (cmp(rhs) >= 0); }
//
inline bool DateTime::operator<(const DateTime& rhs) const
{ return (cmp(rhs) == -1); }
//
inline bool DateTime::operator<=(const DateTime& rhs) const
{ return (cmp(rhs) <= 0); }
//
inline bool DateTime::operator==(const DateTime& rhs) const
{ return (cmp(rhs) == 0); }
//
inline bool DateTime::operator!=(const DateTime& rhs) const
{ return (cmp(rhs) != 0); }


#endif  /* _DATETIME_H_ */
