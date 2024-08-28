/*
NAME
	File_name:CPS_UTils.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

The Copyright to the computer program(s) herein is the property of Ericsson
Utvecklings AB, Sweden.
The program(s) may be used and/or copied only with the written permission from
Ericsson Utvecklings AB or in accordance with the terms and conditions
stipulated in the agreement/contract under which the program(s) have been
supplied.

DESCRIPTION
	FCSRV configuration data.

DOCUMENT NO
	190 89-CAA 109 0670

AUTHOR
	2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on CPS_Utils of FCSRV in Windows

SEE ALSO
	-

Revision history
----------------

*/

#include "CPS_Utils.h"
#include "CPCmdOptions.h"

#include <sstream>
#include <cstring>

// Definition of Static variables
static char* g_pthr_str = 0;

static const char* NullString = "\0";

#define MAX_SIZE   1024

#include <iostream>
using namespace std;


//-----------------------------------------------------------------------------
const char* CPS::before(const char* str, const char* pattern)
{
   g_pthr_str = new char[MAX_SIZE];
   g_pthr_str[0] = 0;
   
   if (str != NULL && pattern != NULL) {
   
      const char* cp = strstr(str, pattern);
   
      if (cp != NULL) {

		   const char* sp = str;
		   int i=0;

		   while (sp != cp) {
			   g_pthr_str[i++] = *sp;
			   sp++;
		   }
         g_pthr_str[i] = 0;

      }
      else {
         return str;
      }
      
   }
   else {
      return str;
   }
   
   return g_pthr_str;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
const char* CPS::after(const char* str, const char* pattern)
{
   const char* cp = NULL;
   
   if (str != NULL && pattern != NULL) {
   
      cp = strstr(str, pattern);
   
      if (cp != NULL) {
   
         int len = strlen(pattern);
      
         while (cp && *cp != '\0' && len > 0) {
            cp++;
            len--;
         }
      }   
   }
   
   return (cp ? cp : NullString);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CPS::trim(string& str)
{
	string::size_type pos = 0;

	// Strip from the beginning first
	while ( isspace(str[0]) )
			str.erase(pos,1);

	// Strip from the end now
   trimEnd(str);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CPS::trimEnd(string& str)
{
	string::size_type pos;

	// Strip from the end
	pos = str.length();

	while (pos > 0 && isspace(str[pos-1]) ) {
		str.erase(--pos,1);
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CPS::toUpper(string& str)
{
	string::iterator it;

   for (it = str.begin(); it!=str.end(); ++it) {
      (*it) = (char)::toupper( *it );
   }
}

char** CPS::argvCpy(char**& dest, char const* const* src, int argc)
{
    dest = new char*[argc];     	
    char **pal = dest;
    int len;                                      
    char *v;          
    for(; argc; --argc, ++src, ++pal) {
        len = strlen(*src);
        if (strcmp(*src, CPCmdOption::cpOption) == 0) {
            v = new char[len+2];
            strcpy(v, CPCmdOption::cpOptionLong);
        }
        else if (strcmp(*src, CPCmdOption::cpOptionLong) == 0) {
            v = new char[strlen(CPCmdOption::cpOptionLongFake) + 1];
            strcpy(v, CPCmdOption::cpOptionLongFake);
        }
        else {
            v = new char[len+1];
            strcpy(v, *src);
        }
        *pal = v;
	}

	return dest;
}
