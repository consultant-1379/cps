//	NAME
//	 CPS_TESRV_tesrvtran.h
//
//	COPYRIGHT
//	 Ericsson Utvecklings AB, Sweden 2007.
//	 All rights reserved.
//
//	 The Copyright to the computer program(s) herein
//	 is the property of Ericsson Utvecklings AB, Sweden.
//	 The program(s) may be used and/or copied only with
//	 the written permission from Ericsson Utvecklings AB
//	 or in accordance with the terms and conditions
//	 stipulated in the agreement/contract under which the
//	 program(s) have been supplied.
//
//	DESCRIPTION
//
//	ERROR HANDLING
//	 General rule:
//	 The error handling is specified for each method.
//	 No methods initiate or send error reports unless
//	 specified.
//
//	DOCUMENT NO
//	 190 89-CAA 109 1374
//
//	AUTHOR
//	 2007-01-22 EAB/FTE/DDM QSUJSIV
//
//	REVISION
//	 PA1  2007-01-22     QSUJSIV    First revision
//
//	LINKAGE
//
//  Source file: Z:\Gemini_CPS\tecp_cnz\tesrvadm_caa\inc\CPS_TESRV_tesrvtran.h

#ifndef CPS_TESRV_tesrvtran_h
#define CPS_TESRV_tesrvtran_h 1

#include "CPS_TESRV_utils.h"
#include "pes_clhapi.h"
#include <string>


class CPS_TESRV_tesrvtran
{
  public:
	//	Constructor.
	CPS_TESRV_tesrvtran (int argc, char **argv);

	//## Destructor (generated)
	~CPS_TESRV_tesrvtran();


	//	Executes the command.
	int execute ();

  private:

	//	Parses the supplied command line.
	int parse ();

	//	Prints the usage information.
	void usage ();

  private: //## implementation
	// Data Members for Class Attributes

	char m_trace[400];

	//	Pointer to char strings for command options.
	char **m_argV;
	//	Number of supplied options.
	int m_argC;
	//	Start date for log listing.
	std::string m_startDate, temp_startDate;
	//	Stop date for log listing.
	std::string m_stopDate, temp_stopDate;
	//	Start time for log listing.
	std::string m_startTime, temp_startTime;
	//	Stop time for log listing.
	std::string m_stopTime, temp_stopTime;
	//	CP Name for listing.
	std::string m_cpName, temp_cpName;
	// Destination type
	desttype_t m_destType;	
	// CP system type
	eSystemType m_sysType;

};
// Class CPS_TESRV_tesrvtran 

#endif
