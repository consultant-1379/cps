
//******************************************************************************
// 
// .NAME
//  	CPS_BUAP_Exception - Exception class
// .LIBRARY 3C++
// .PAGENAME CPS_BUAP_Exception
// .HEADER  CPS  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE CPS_BUAP_Exception.H

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
//	This class is used for the raising of all exceptions within
//	the process buparmgr and the programs bupset and bupprint.
//	The class constructor takes parameters showing where the 
//	exception was thrown, why it was thrown, and the file and line
//	number of where it was thrown.

// .ERROR HANDLING
//
//	General rule:
//	The error handling is specified for each method.
//
//	No methods initiate or send error reports unless specified.

// DOCUMENT NO
//  190 89-CAA 109 1412

// AUTHOR 
// 	1997/08/06 by U/B/SD David Wade

// .LINKAGE
//	-

// .SEE ALSO 
// 	-
//  101010 xchihoa     Ported to Linux for APG43L.
//******************************************************************************


#ifndef CPS_BUAP_EXCEPTION_H
#define CPS_BUAP_EXCEPTION_H

class CPS_BUAP_Exception
{
public:
  char  m_datetime[32]; // posix states must be at least 26 bytes
  char *m_file;		// pointer to filename with which the exception occurred
  int   m_line;		// line number with file at which the excetion was caught
  char *m_whatHappened; // pointer to string containing text of what happened

  CPS_BUAP_Exception(const char *file, int line, const char *whatHappened);
  // Description:
  // 	Class constructor    
  // Parameters: 
  //    file			name of file where constructor was called 
  //    line			line number in file where constructor was called
  //    context			name of function within which contructor was called
  //    whatHappened		text pertaining to the raising of the exception
  // Return value: 
  //    -
  // Additional information:
  //    -

  virtual ~CPS_BUAP_Exception();
  // Description:
  // 	Class destructor
  // Parameters: 
  //    -
  // Return value: 
  //    -
  // Additional information:
  //    -

  CPS_BUAP_Exception(const CPS_BUAP_Exception& );
  //:ag1
  // Description:
  // 	Copy constructor
  // Parameters: 
  //    -
  // Return value: 
  //    -
  // Additional information:
  //   -

  CPS_BUAP_Exception& operator=(const CPS_BUAP_Exception& );
  //:ag1
  // Description:
  // 	Assignment operator
  // Parameters: 
  //    -
  // Return value: 
  //    -
  // Additional information:
  //   -

  void Report();
  // Description:
  // 	Method used to report the exception occurance.
  // Parameters: 
  //    -
  // Return value: 
  //    -
  // Additional information:
  //    -
};

#define THROW(p1) throw CPS_BUAP_Exception(__FILE__, __LINE__, p1);

#endif
