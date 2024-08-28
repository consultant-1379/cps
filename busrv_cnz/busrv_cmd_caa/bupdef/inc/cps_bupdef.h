
//******************************************************************************
// 
// .NAME
//  	CPS_BUPDEF - Class containing functionality of bupset command
// .LIBRARY 3C++
// .PAGENAME CPS_BUPDEF
// .HEADER  CPS  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE CPS_BUPDEF.H

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
//	 20070112 uablan Add changes due to multple system

// .SEE ALSO 
// 	-

//******************************************************************************

#ifndef CPS_BUPDEF_H
#define CPS_BUPDEF_H

enum eError
{
	eNoError                    = 0,
	eExecError                  = 1,
	eSyntaxError                = 2, // incorrect usage
	eMandatoryOption	        = 3,
	eInvalidValue               = 4,
	eMissingSbcInSequence       = 5,
	eDumpLoadorSwitchInProgress = 10,
	eUnknown                    = 100,
	eIllegalCommand             = 115,
	eIllegalOption		        = 116,
	eServerUnreachable	        = 117,
	eConfigServiceError         = 118
	
};

enum eSystemType
{
	eSingle		= 0,
	eMultiple	= 1
};

class Argument
{
public:
	Argument(void (*f)(const char*), const char* m) : mF(f), mArg(m) {}
	void operator()() {
		mF(mArg);
	}
private:
	void (*mF)(const char*);
	const char* mArg;
};

class FileSequenceError
{
public:
	FileSequenceError(char opt) : mOpt(opt) {}
	char operator()() { return mOpt; }
private:
	char mOpt;
};

#endif
