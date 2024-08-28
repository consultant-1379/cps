
//******************************************************************************
// 
// .NAME
//  	CPS_BUAP_BUPSET - Class containing functionality of bupset command
// .LIBRARY 3C++
// .PAGENAME CPS_BUAP_BUPSET
// .HEADER  CPS  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE CPS_BUAP_BUPSET.H

// .COPYRIGHT
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 1997.
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
//	Implements the unix function getopt.
//

// .ERROR HANDLING
//

// DOCUMENT NO
//  190 89-CAA 109 1412

// AUTHOR 
// 	1997/07/02 by U/B/SF Lillemor Pettersson

// .LINKAGE
//	-

// .SEE ALSO 
// 	-
//  101010 xchihoa     Ported to Linux for APG43L.
//******************************************************************************


#ifndef CPS_BUAP_GETOPT_H
#define CPS_BUAP_GETOPT_H

extern char *optarg;    // pointer to the start of the option argument 
extern int   optind;       // number of the next argv[] to be evaluated 
extern int   opterr;       // non-zero if a question mark should be returned


extern int buap_getopt(int argc, char* argv [], const char* optstring);

#endif
