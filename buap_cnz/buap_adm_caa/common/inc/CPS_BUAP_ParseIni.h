/*
 * COPYRIGHT Ericsson Utvecklings AB, Sweden 2012.
 * All rights reserved.
 *
 * The Copyright to the computer program(s) herein
 * is the property of Ericsson Utvecklings AB, Sweden.
 * The program(s) may be used and/or copied only with
 * the written permission from Ericsson Utvecklings AB or in
 * accordance with the terms and conditions stipulated in the
 * agreement/contract under which the program(s) have been
 * supplied.
 *
 * NAME
 *  CPS_BUAP_ParseIni.h
 *
 * DESCRIPTION
 *  -
 *
 * DOCUMENT NO
 *  190 89-CAA 109 1414
 *
 * AUTHOR
 *  XDT/DEK XNGUDAN
 *
 * === Revision history ===
 * 2012-02-20   XNGUDAN    Create
 *
 */

#ifndef CPS_BUAP_PARSEINI_H_
#define CPS_BUAP_PARSEINI_H_

#include <string>
#include "boost/lexical_cast.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

namespace ParseINIFunx {

    int getPrivateProfileInt(const char* section, const char* key, int defaultValue,
            boost::property_tree::ptree &pt)
    {
        std::string keystr = section;
        keystr += ".";
        keystr += key;
        boost::optional<int> value = pt.get_optional<int>(keystr);
        return value ? *value : defaultValue;
    }

    std::string getPrivateProfileString(const char* section, const char* key, const char* defaultValue,
            boost::property_tree::ptree &pt)
    {
        std::string keystr = section;
        keystr += ".";
        keystr += key;
        boost::optional<std::string> value = pt.get_optional<std::string>(keystr);
        return value ? *value : defaultValue ? std::string(defaultValue) : std::string("");
    }
    template<typename T>
    void writePrivateProfile(const char* section, const char* key, T value,
            boost::property_tree::ptree &pt)
    {
        try
        {
            std::string keystr = section;
            keystr += ".";
            keystr += key;
            pt.put(keystr, value);
        }
        catch (...)
        {
            throw;
        }
    }



    void splitString(std::string &str, unsigned char &century, unsigned char &year, unsigned char &month,
            unsigned char &day, unsigned char &hour, unsigned char &minute, bool isDefault)
    {
        try
        {
             century = boost::lexical_cast<uint32_t>(str.substr(0, 2));
             year = boost::lexical_cast<uint32_t>(str.substr(2, 2));
             month = boost::lexical_cast<uint32_t>(str.substr(5, 2));
             day = boost::lexical_cast<uint32_t>(str.substr(8, 2));
             hour = boost::lexical_cast<uint32_t>(str.substr(11, 2));
             minute = boost::lexical_cast<uint32_t>(str.substr(14, 2));
        }
        catch (boost::bad_lexical_cast &)
        {
            if (isDefault)
            {
                century = 19;
                year = 90;
                month = 1;
                day = 1;
                hour = 0;
                minute = 0;
            }
            else
            {
                century = 0;
                year = 0;
                month = 0;
                day = 0;
                hour = 0;
                minute = 0;
            }
        }
    }

    void splitString(std::string &str, unsigned char& century, unsigned char& year, unsigned char& month,
            unsigned char& day)
    {
        try
        {
             century = boost::lexical_cast<uint32_t>(str.substr(0, 2));
             year = boost::lexical_cast<uint32_t>(str.substr(2, 2));
             month = boost::lexical_cast<uint32_t>(str.substr(5, 2));
             day = boost::lexical_cast<uint32_t>(str.substr(8, 2));
        }
        catch (boost::bad_lexical_cast &)
        {
            century = 19;
            year = 90;
            month = 1;
            day = 1;
        }
    }
}


#endif /* CPS_BUAP_PARSEINI_H_ */
