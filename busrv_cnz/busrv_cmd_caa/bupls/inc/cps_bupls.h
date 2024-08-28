
//******************************************************************************
// 
// .NAME
//  	CPS_BUPLS - Class containing functionality of bupset command
// .LIBRARY 3C++
// .PAGENAME CPS_BUPLS
// .HEADER  CPS_BUPLS
// .LEFT_FOOTER Ericsson Utvecklings AB

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
//  -

// .ERROR HANDLING
//
//	General rule:
//	Any error encountered will result in a eError
//	being thrown.

// DOCUMENT NO
//	190 89-CAA 109 0082

// AUTHOR 
// 	2003-01-21 by EAB/UKY/SF Lillemor Pettersson

// .CHANGES
//	 -

// .SEE ALSO 
// 	-

//******************************************************************************

#ifndef CPS_BUPLS_H
#define CPS_BUPLS_H


enum eError
{
	eNoError          = 0,
	eExecError          = 1,
	eSyntaxError      = 2, // incorrect usage
	eMandatoryOption	= 3,
	eUnknown          = 100,
	eIllegalCommand		= 115,
	eIllegalOption		= 116,
	eServerUnreachable	= 117,
	eConfigServiceError = 118
};

enum eSystemType
{
	eSingle		= 0,
	eMultiple	= 1
};


#endif
