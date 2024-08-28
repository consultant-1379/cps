//*****************************************************************************
//
// .NAME
//     CPS_Util.cpp - Utility functions.
// .LIBRARY 3C++
// .PAGENAME 
// .HEADER  
// .LEFT_FOOTER Telefonaktiebolaget LM Ericsson
// .INCLUDE CPS_Util.H

// .COPYRIGHT
//  COPYRIGHT Telefonaktiebolaget LM Ericsson, Sweden 2001.
//  All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Telefonaktiebolaget LM Ericsson, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Telefonaktiebolaget LM Ericsson or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.

// .DESCRIPTION 
//    Contains utility functions.
//
// CHANGES
//
//    RELEASE REVISION HISTORY
//
//    DATE        NAME        DESCRIPTION
//    2006-10-24  UABLAN      First release
//
//    1012-11-04  XDTTHNG     Totally change the code for 43L
//*************************************************************************************

#include <cstring>

#include "CPS_Utils.h"
#include "CPCmdOptions.h"

char** cps_utils::argvCpy(char**& dest, char const* const* src, int argc)
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
