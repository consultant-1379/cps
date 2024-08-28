//*****************************************************************************
// 
// .NAME
//     CPS_Util - Internal Utility functions.
// .LIBRARY 3C++
// .PAGENAME
// .HEADER  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE CPS_Util.H

// .COPYRIGHT
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 1998-2001.
//  All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson Utvecklings AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.

// .DESCRIPTION 
//
// A collection of useful utility functions within CPS subsystem.
//
//
//  General rule:
//  The error handling is specified for each method.
//
//  No methods initiate or send error reports unless specified.

// AUTHOR 
//      1998-03-18 by UAB/I/LN  UABSJEN

// CHANGES
//
//       RELEASE REVISION HISTORY
//
//       DATE        NAME        DESCRIPTION
//       2006-10-24  UABLAN      First revision.
//
//       1012-11-04  XDTTHNG     Totally change the code for 43L
//*****************************************************************************
#ifndef CPS_UTIL_H
#define CPS_UTIL_H

#pragma once

#include <iostream>
#include <string>
#include "boost/lexical_cast.hpp"

namespace cps_utils {

    char** argvCpy(char**& dest, char const* const* src, int argc);

    template <typename T> 
    inline std::string to_string(const T& arg)
    {
        try {
            return boost::lexical_cast<std::string>(arg);
        }
        catch (...) {
            return "";
        }
    }
}

#endif //CPS_UTIL_H
