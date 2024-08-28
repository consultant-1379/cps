// ****************************************************************************
//
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
//
//  NAME
//  CPS_BUAP_Loader_Exception.cpp
//
// .DESCRIPTION 
//	
//	This is the exception handler for CPS BULOADER
//
//
//  .ERROR HANDLING
//
//	-
//
//  DOCUMENT NO
//	190 89-CAA 109 1412
//
//  AUTHOR
// 	1997-05-15 	by UAB/I/GD  	QABLARG
//
//	CHANGE HISTORY
//
//	DATE 	NAME	DESCRIPTION
//	970515	QABLARG	1:st revision
//  111116  XTUAGNU Update the CAA number
//
//
// .LINKAGE
//	-
//
// .SEE ALSO 
//	-
//
//******************************************************************************

#ifndef CPS_BUAP_LOADER_EXCEPTION_H
#define CPS_BUAP_LOADER_EXCEPTION_H

#include <iostream.h> 
#include <rw/cstring.h>

//******************************************************************************
// Class declaration
//******************************************************************************

class RWbostream;
class RWbistream;

class CPS_BUAP_Loader_Exception
{
public:

  // Error codes
  // All error codes that are meaningful to a user of the API are
  // specified. Errors that are not meaningful for a user of the
  // API shall be specified with the FATALERROR code, such errors
  // are sometimes refered to as internal errors.

  // The first three error codes are defined for the purpose of 
  // being compatible with "Design Rule for AP Commands".

  enum errorCode {

    // Standard return codes for AP commands
    OK = 0,			// Compatibility: Design Rule for AP Commands
    GENERAL_FAULT = 1,		// Compatibility: Design Rule for AP Commands
    INCORRECT_USAGE = 2,	// Compatibility: Design Rule for AP Commands

    // General JTP errors
    JTP_JIDREPREQ_FAILED,	// The server could not register its service
    JTP_JEXINITIND_FAILED,	// Unable to initiate JTP connection
    JTP_JEXINITRSP_FAILED,	// Unable to send JTP connection reply 
    JTP_JEXDATAIND_FAILED,	// Unable to send JTP message
    JTP_JEXDATAREQ_FAILED,	// Unable to send JTP message response

    // Return codes for BULOADER
    JTP_DNS_INIT_FAILED,	// The BULOADER could not register its service
    INCOMING_CONNECT_FAILED,	// The BULOADER could not accept a new connect
    INCOMING_CONFIRM_FAILED,	// The BULOADER could not confirm a new connect
    UNEXPECTED_JTP_REPLY,	// Unexpected reply for the previous JTP msg
    RELADMPAR_LOCK_FAILED,	// Could not lock reladmparfile
    SERVER_DEAD,		// No contact with server part of software
    FATALERROR			// Fatal error, contact your system programmer    
  };

// Set to protected for guidance purpose. See the comment in
// Internal_Exception.
//protected:

  CPS_BUAP_Loader_Exception ();

  CPS_BUAP_Loader_Exception (errorCode error);
  // Description:
  //	Constructs the CPS_BUAP_Loader_Exception class.
  // Parameters:
  // 	error			Error code
  // Additional information:
  //	None

  CPS_BUAP_Loader_Exception (errorCode error, const char* comment);
  // Description:
  //	Constructs the CPS_BUAP_Loader_Exception class.
  // Parameters:
  // 	error			Error code
  //	comment			Optional textual information about error.
  // Additional information:
  //	None

  CPS_BUAP_Loader_Exception (errorCode error, int oserror, 
		     const char * file, int line, const char* comment);
  // Description:
  //	Constructs the CPS_BUAP_Loader_Exception class.
  // Parameters:
  // 	error			Error code
  //	oserror			Error code returned from os.
  //	file			File where error occured.
  //	line			Line where error occured.
  //	comment			Optional textual information about error.
  // Additional information:
  //	None

  CPS_BUAP_Loader_Exception (errorCode error, int oserror, 
		     const char * file, int line, istream & istr);
  // Description:
  //	Constructs the CPS_BUAP_Loader_Exception class.
  // Parameters:
  // 	error			Error code
  //	oserror			Error code returned from os.
  //	file			File where error occured.
  //	line			Line where error occured.
  //	comment			Optional textual information about error.
  // Additional information:
  //	None

  CPS_BUAP_Loader_Exception (const CPS_BUAP_Loader_Exception& ex);
  // Description:
  //	Copy constructor for the CPS_BUAP_Loader_Exception class.
  // Parameters:
  // 	ex		Reference to CPS_BUAP_Loader_Exception instance to be copied
  // Additional information:
  //	None

  ~CPS_BUAP_Loader_Exception ();
  // Description:
  //	Destructs the CPS_BUAP_Loader_Exception class.
  // Parameters:
  // 	errcode			Error code
  //	detail			Optional textual information
  // Additional information:
  //	None

  errorCode errCode () const;
  // Description:
  //	Read the error code.
  // Parameters:
  // 	None
  // Return value:
  //	errcode			Error code
  // Additional information:
  //	None

  const char * errorMessage () const;
  // Description:
  //	Read the error message.
  // Parameters:
  // 	None
  // Return value:
  //	Error message			
  // Additional information:
  //	None

  const char* detailInfo () const;
  // Description:
  //	Read the detailed information.
  // Parameters:
  // 	None
  // Return value:
  //	detail			Optional textual information
  // Additional information:
  //	None

  friend ostream& operator<< (ostream& s, const CPS_BUAP_Loader_Exception& ex); 
  // Description:
  //	Stream operator.
  // Parameters:
  // 	s			Output stream
  //	ex			Exception instance
  // Return value:
  //	Output stream
  // Additional information:
  //	None

  friend RWbostream& operator<< (RWbostream& bostr, const CPS_BUAP_Loader_Exception& ex);
  // Description:
  //	Stores the exception in freely exchangable binary format.
  // Parameters:
  // 	bostr			Output stream
  //	ex			Exception instance
  // Return value:
  //	Output stream
  // Additional information:
  //	The code for this member function is delivered in a separate C-file
  //	that is not accessible to API users.


  friend RWbistream& operator>> (RWbistream& bistr, CPS_BUAP_Loader_Exception& ex);
  // Description:
  //	Retrieves the exception from freely exchangable binary format.
  // Parameters:
  // 	bistr			Input stream
  //	ex			Exception instance
  // Return value:
  //	Input stream
  // Additional information:
  //	The code for this member function is delivered in a separate C-file
  //	that is not accessible to API users.

  CPS_BUAP_Loader_Exception& operator<< (const char* detail);
  // Description:
  //	Append character string to optional textual information.
  // Parameters:
  // 	detail			String to be appended
  // Return value:
  //	Reference to this
  // Additional information:
  //	None

  CPS_BUAP_Loader_Exception& operator<< (long detail);
  // Description:
  //	Append long to optional textual information.
  // Parameters:
  // 	detail			String to be appended
  // Return value:
  //	Reference to this
  // Additional information:
  //	None

  CPS_BUAP_Loader_Exception& operator<< (istream& s);
  // Description:
  //	Append istream to optional textual information.
  // Parameters:
  // 	s			Stream to be appended
  // Return value:
  //	Reference to this
  // Additional information:
  //	None

protected:
  errorCode	error_;		// Error code specified by AFP
  int		oserror_;	// Optional error code returned by OS primitive
  RWCString	file_;		// File name of throw statement
  int		line_;		// Line number of throw statement
  RWCString	comment_;	// Optional comment
};
 
#define LOADER_EX(ERROR,COMMENT) \
CPS_BUAP_Loader_Exception(ERROR,0, __FILE__, __LINE__,COMMENT)

#define LOADER_EX_OS(ERROR,OSERROR,COMMENT) \
CPS_BUAP_Loader_Exception(ERROR,OSERROR, __FILE__, __LINE__,COMMENT)

#endif
