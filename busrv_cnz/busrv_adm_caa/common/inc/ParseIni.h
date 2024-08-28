#ifndef PARSE_INI_H
#define PARSE_INI_H

#include "PlatformTypes.h"
#include <string>

class DateTime;

namespace ParseINIFunx {

    struct PARSE_TIME_KEY {
       const char* year;
       const char* month;
       const char* day;
       const char* hour;
       const char* minute;
       const char* second;
       const char* daylight;
    };
    
    int GetPrivateProfileInt(const char* section, const char* key, int default_val, const char* filename);
    u_int32 GetPrivateProfileString(const char* section, const char* key, const char* default_val,
                                    char* returned_val, u_int32 size, const char* filename);
    DateTime GetPrivateProfile(const char* section, const PARSE_TIME_KEY& timeKey, const char* filename);
    std::string GetPrivateProfileString(const char* section, const char* key, const char* defaultStr, const char* filename);
    
    bool WritePrivateProfileString(const char* section, const char* key, const char* str, const char* filename); 
    bool WritePrivateProfile(const char* section, const char* key, int value, const char* filename);
    bool WritePrivateProfile(const char* section, const PARSE_TIME_KEY& timeKey, DateTime dt, const char* filename);

} 

#endif  // _PARSE_INI_H_
