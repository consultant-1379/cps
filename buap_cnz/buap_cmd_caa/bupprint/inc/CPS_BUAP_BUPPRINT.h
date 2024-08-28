
//******************************************************************************
// 
// .NAME
//  	CPS_BUAP_BUPPRINT - Class for bupprint command functionality
// .LIBRARY 3C++
// .PAGENAME CPS_BUAP_BUPPRINT
// .HEADER  CPS  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE CPS_BUAP_BUPPRINT.H

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
//	This class contains the methods required for the implementation
//	of the bupprint command.  The first method to call is initialise
//	which creates a client of the parmgr process.  The next method to
//	call is ParseCommandLine which checks and interprets the command
//	-line options.  Finally, if no exception occured, the method 
//	ExecuteCommandLine should be called to carry out the command.

// .ERROR HANDLING
//
//	General rule:
//	If and error is encountered at any stage a CPS_BUAP_Exception
//	is thrown.

// DOCUMENT NO
//	190 89-CAA 109 0082

// AUTHOR 
// 	1997/08/06 by U/B/SD David Wade

// .LINKAGE
//	-

// .SEE ALSO 
// 	-

//******************************************************************************


#ifndef CPS_BUAP_BUPPRINT_H
#define CPS_BUAP_BUPPRINT_H


#include "CPS_BUAP_Exception.h"
#include "CPS_BUAP_MSG_static_parameters.h"
#include "CPS_BUAP_Client.h"

class CPS_BUAP_BUPPRINT
{

public:
  enum eError
  {
    eNoError          = 0,
    eSyntaxError      = 2,
    eNoServer         = 16,
    eCPFSerror        = 18,
    eUnknown          = 100,
    eIllegalCommand   = 115
  };

  CPS_BUAP_BUPPRINT();
  // Description:
  // 	Class constructor 
  // Parameters: 
  //    -
  // Return value: 
  //    -
  // Additional information:
  //    -

  virtual ~CPS_BUAP_BUPPRINT();
  // Description:
  // 	Class destructor
  // Parameters: 
  //    -
  // Return value: 
  //    -
  // Additional information:
  //    -


  void Initialise();
  // Description:
  // 	Method creates client of parmgr process.
  // Parameters: 
  //    -
  // Return value: 
  //    -
  // Additional information:
  //    My throw CPS_BUAP_Exception

  void ParseCommandLine(int argc, char** argv);
  // Description:
  // 	Method checks the command line to determine which of
  //    the records in the reladmpar file are to displayed.
  // Parameters: 
  //    argc			number of command line arguments
  //    argv			array of pointers to command line arguments
  // Return value: 
  //    -
  // Additional information:
  //    May throw CPS_BUAP_Exception

  void ExecuteCommand();
  // Description:
  // 	Method to control the processing of the command.
  // Parameters: 
  //    -
  // Return value: 
  //    -
  // Additional information:
  //    May throw CPS_BUAP_Exception


protected:

private:
  
  CPS_BUAP_Client         *m_pClient;	// pointer to client object

  int m_Static;				// indicator to display static pars
  int m_Dynamic;			// indicator to display dynamic pars
  int m_ClogRefs;			// indicator to display clog pars
  int m_LAR;				// indicator to display lar pars
  int m_DMeasures;			// indicator to display delayed pars
  int m_OldFormat;			// indicator to display pars like SYGPP

  CPS_BUAP_BUPPRINT(const CPS_BUAP_BUPPRINT& );
  //:ag1
  // Description:
  // 	Copy constructor
  // Parameters: 
  //	-
  // Return value: 
  //	-
  // Additional information:
  //	Declared to disallow copying

  CPS_BUAP_BUPPRINT& operator=(const CPS_BUAP_BUPPRINT& );
  //:ag1
  // Description:
  // 	Assignment operator
  // Parameters: 
  //	-
  // Return value: 
  //	-
  // Additional information:
  //	Declared to disallow copying

  void DoStaticPars();
  // Description:
  // 	Method reads and displays the static parameters.
  // Parameters: 
  //    -
  // Return value: 
  //    -
  // Additional information:
  //    -

  void DoDynamicPars();
  // Description:
  // 	Method reads and displays the dynamic parameters.
  // Parameters: 
  //    -
  // Return value: 
  //    -
  // Additional information:
  //    -

  void DoClogRefs();
  // Description:
  // 	Method reads and displays the command log references.
  // Parameters: 
  //    -
  // Return value: 
  //    -
  // Additional information:
  //    -

  void DoLAR();
  // Description:
  // 	Method reads and displays the last auto reload parameters.
  // Parameters: 
  //    -
  // Return value: 
  //    -
  // Additional information:
  //    -

  void DoDMeasures();
  // Description:
  // 	Method reads and displays the delayed measures.
  // Parameters: 
  //    -
  // Return value: 
  //    -
  // Additional information:
  //    -

  void DoOldFormat();
  // Description:
  // 	Method Displays parameters in a similar mannor 
  //	to the old SYGPP command.
  // Parameters: 
  //    -
  // Return value: 
  //    -
  // Additional information:
  //    -


  char *FormatDate(const unsigned withTime,
                   const unsigned century, 
                   const unsigned year, 
                   const unsigned month,
                   const unsigned day,     
                   const unsigned hour = 0, 
                   const unsigned minute = 0);
  // Description:
  // 	Method for formating a date and time into a fixed format.  Note the
  //	first parameter is a boolean flag indicating whether a time is to be
  //	included in the formatted text.
  // Parameters: 
  //	withTime		Boolean indication of whether time is to used
  //	century			century number (-1!!) eg 19 = 20th century
  //	year			year eg 89 for 1989
  //	month			month number
  //	day			day number
  //	hour			hour (24 hour clock)
  //	minute			minute
  // Return value: 
  //    -
  // Additional information:
  //    -

};

#endif
