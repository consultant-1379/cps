//*****************************************************************************
// 
// .NAME
//     CPS_Util - Internal Utility functions.
// .LIBRARY 3C++
// .PAGENAME
// .HEADER  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE CPS_Util.h

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



// String functions:
//-------------------


//    toUpper()
//          Converts a string to upper case

//    toLower()
//          Converts a string to lower case


//    trim()
//          Trims the string from starting and ending space characters

//    trimEnd()
//          Trims the string from ending space characters


//    before()
//          Returns the string before a given substring


//    after()
//          Returns the string after a given substring



// .ERROR HANDLING
//
//  General rule:
//  The error handling is specified for each method.
//
//  No methods initiate or send error reports unless specified.

// AUTHOR 
//      2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on CPS_Utils.h of Windows version

// CHANGES

//*****************************************************************************
#ifndef CPS_UTIL_H
#define CPS_UTIL_H

#pragma once

#include <string>

#ifndef INCLUDED_SET
#  include <set>
#  define INCLUDED_SET
#endif

using namespace std;

// General definitions
#define CPSCXCNAME      "cps/CXC1371204"



//-------------------------------------------------------

class CPS
{
public:


   typedef std::set<char> CHARSET;

   static void toUpper(string& Source);
   // Description:
   //    Converts string to captital characters.
   // Parameters:
   //    Source         Source string
   //
   // Return value: 
   //    -
   // Additional information:
   //    -

   static void toLower(string& Source);
   // Description:
   //    Converts string to lower case characters.
   // Parameters:
   //    Source         Source string
   //
   // Return value: 
   //    -
   // Additional information:
   //    -

   static void trim(string& Source);
   // Description:
   //    Removes space characters from the beginning and end
   //    of the source string.
   // Parameters:
   //    Source         Source string
   // Return value: 
   //    -
   // Additional information:
   //    Uses isspace() to determine if a character is space or not.

   static void trimEnd(string& Source);
   // Description:
   //    Removes space characters from the end
   //    of the source string.
   // Parameters:
   //    Source         Source string
   // Return value: 
   //    -
   // Additional information:
   //    Uses isspace() to determine if a character is space or not.



   static const char* before(const string& str, const string& pattern);
   static const char* before(const char* str, const char* pattern);
   // Description:
   //    Get the string that is before 'pattern'.
   // Parameters:
   //    str            Source string
   //    pattern        Pattern string
   //
   // Return value: 
   //    Returns the string that is before 'pattern', or
   //    the whole string if 'pattern' cannot be found.
   //
   // Additional information:
   //    -

   static const char* beforeLast(const char* str, const char* pattern);
   // Description:
   //    Get the string that is before the last occurances of 'pattern'.
   // Parameters:
   //    str            Source string
   //    pattern        Pattern string
   // Return value: 
   //    Returns the string that is before the last occurances if 'pattern',
   //    or the source string if 'pattern' cannot be found.
   // Additional information:
   //    -

   static const char* after(const char* str, const char* pattern);
   // Description:
   //    Get the string that is after 'pattern'.
   // Parameters:
   //    str            Source string
   //    pattern        Pattern string
   //
   // Return value: 
   //    Returns the string that is after 'pattern', or
   //    an empty string if 'pattern' cannot be found.
   //
   // Additional information:
   //    -
   
   static char** argvCpy(char**& dest, char const* const* src, int argc);

 
private:

   CPS();       // private constructor to avoid instances of this class.
   
   // Internal functions


};

#endif //CPS_UTIL_H
