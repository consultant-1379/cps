/*
    Take 2
    To run this
*/

#include "ParseIni.h"
#include "DateTime.h"
#include "CPS_BUSRV_Trace.h"
#include "FileFunx.h"

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

int ParseINIFunx::GetPrivateProfileInt(const char* section, const char* key, int default_val, const char* filename)
{
    using boost::property_tree::ptree;
    
    ptree pt;
    try {
        read_ini(filename, pt);    
    }
    catch (...) {
        return default_val;
    }
    std::string keystr = section;
    keystr += ".";
    keystr += key;
    boost::optional<int> value = pt.get_optional<int>(keystr); 
    
    return value? *value : default_val;
}

u_int32 ParseINIFunx::GetPrivateProfileString(const char* section, const char* key, const char* default_val,
                                              char* returned_val, u_int32 size, const char* filename)
{

    using boost::property_tree::ptree;

    string def_val(default_val);
    int tmpSize = 0;

    ptree pt;
    try {
        read_ini(filename, pt);
    }
    catch (...) {
        tmpSize = (size < def_val.length()) ? size : def_val.length();
        def_val.copy(returned_val, tmpSize);
        return tmpSize;
    }
    std::string keystr = section;
    keystr += ".";
    keystr += key;
    boost::optional<std::string> value = pt.get_optional<std::string>(keystr);

    if ((*value) != "")
    {
        tmpSize = (size < (*value).length()) ? size : (*value).length();
        (*value).copy(returned_val, tmpSize);
    }

    return tmpSize;
}

string ParseINIFunx::GetPrivateProfileString(const char* section, const char* key, 
    const char* defaultStr, const char* filename)

{
    using boost::property_tree::ptree;
    
    ptree pt;
    try {
        read_ini(filename, pt);    
    }
    catch (...) {
        return string(defaultStr);
    }
    std::string keystr = section;
    keystr += ".";
    keystr += key;
    boost::optional<string> value = pt.get_optional<string>(keystr); 
    
    return value? *value : defaultStr? string(defaultStr) : string("");
    
}

//
// Atomic reading of DateTime object. Return null DateTime object if fail to read any of DateTime data member
//
DateTime ParseINIFunx::GetPrivateProfile(const char* section, const PARSE_TIME_KEY& timeKey, const char* filename)
{
    using boost::property_tree::ptree;    
    try {
        ptree pt;
        read_ini(filename, pt);    
        std::string secStr = section;
        secStr += ".";
        boost::optional<int> value;
        u_int8 year = static_cast<u_int8>(pt.get<int>(secStr + timeKey.year));        
        u_int8 month = static_cast<u_int8>(pt.get<int>(secStr + timeKey.month));         
        u_int8 day = static_cast<u_int8>(pt.get<int>(secStr + timeKey.day));        
        u_int8 hour = static_cast<u_int8>(pt.get<int>(secStr + timeKey.hour));        
        u_int8 minute = static_cast<u_int8>(pt.get<int>(secStr + timeKey.minute));       
        u_int8 second = static_cast<u_int8>(pt.get<int>(secStr + timeKey.second));
                  
        return DateTime(year, month, day, 0, hour, minute, second);
    }
    catch (...) {
        return DateTime();
    }
}

bool ParseINIFunx::WritePrivateProfileString(const char* section, const char* key, const char* str, const char* filename)
{
    newTRACE((LOG_LEVEL_INFO, "ParseINIFunx::WritePrivateProfileString()", 0));
    using boost::property_tree::ptree;
    
    ptree pt;
    try {
        // Create empty file if it doesn't exist
        int fd = FileFunx::CreateFileX(filename);
        FileFunx::CloseFileX(fd);

        read_ini(filename, pt);
        std::string keystr = section;
        keystr += ".";
        keystr += key;    
        pt.put(keystr, str);
        write_ini(filename, pt);
    }
    catch (...) {
        return false;
    }
    
    return true;
}

bool ParseINIFunx::WritePrivateProfile(const char* section, const char* key, int value, const char* filename)
{
    newTRACE((LOG_LEVEL_INFO, "ParseINIFunx::WritePrivateProfile() for int", 0));
    using boost::property_tree::ptree;
    
    ptree pt;
    try {
        // Create empty file if it doesn't exist
        int fd = FileFunx::CreateFileX(filename);
        FileFunx::CloseFileX(fd);

        read_ini(filename, pt);
        std::string keystr = section;
        keystr += ".";
        keystr += key;    
        pt.put(keystr, value);
        write_ini(filename, pt);
    }
    catch (...) {
        return false;
    }
    
    return true;
}

//
// Atomic writing of DateTime object. Write will not be executed if fails to write any of the DateTime data member
//
bool ParseINIFunx::WritePrivateProfile(const char* section, const PARSE_TIME_KEY& timeKey, DateTime dt, const char* filename)
{
    newTRACE((LOG_LEVEL_INFO, "ParseINIFunx::WritePrivateProfile() for DateTime", 0));
    using boost::property_tree::ptree;
    
    ptree pt;
    try {
        // Create empty file if it doesn't exist
        int fd = FileFunx::CreateFileX(filename);
        FileFunx::CloseFileX(fd);

        read_ini(filename, pt);
        
        std::string secStr = section;
        secStr += ".";
        
        pt.put(secStr + timeKey.year, static_cast<int>(dt.year()));
        pt.put(secStr + timeKey.month, static_cast<int>(dt.month()));
        pt.put(secStr + timeKey.day, static_cast<int>(dt.day()));
        pt.put(secStr + timeKey.hour, static_cast<int>(dt.hour()));
        pt.put(secStr + timeKey.minute, static_cast<int>(dt.minute()));
        pt.put(secStr + timeKey.second, static_cast<int>(dt.second()));
        pt.put(secStr + timeKey.daylight, static_cast<int>(dt.daylight()));
        
        write_ini(filename, pt);
    }
    catch (...) {
        return false;
    }
    
    return true;
}
