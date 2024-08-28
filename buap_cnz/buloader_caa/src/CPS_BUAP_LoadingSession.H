//*****************************************************************************
// 
// .NAME
//  	CPS_BUAP_LoadingSession - Manages a single APZ loading session
// .LIBRARY 3C++
// .PAGENAME CPS_BUAP_LoadingSession
// .HEADER  ACS  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE CPS_BUAP_LoadingSession.H

// .COPYRIGHT
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 1996.
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
// 	Manages a single APZ loading session.

// .ERROR HANDLING
//
//	General rule:
//	The error handling is specified for each method.
//
//	No methods initiate or send error reports unless specified.

// DOCUMENT NO
//	CAA 109 0081

// AUTHOR 
// 	Wed Sep 24 12:29:46 1997 by UAB/I/GM UABFLN

// CHANGES
// 990623 uablan - Removes class variable dumpName.
// 050114 uablan - Remove lockMsg.

// .LINKAGE
//	-

// .SEE ALSO 
// 	-

//******************************************************************************

#ifndef CPS_BUAP_LoadingSession_H 
#define CPS_BUAP_LoadingSession_H

#include	<string>

#include	"ACS_JTP.H"
#include	"CPS_BUAP_types.H"
#include	"CPS_BUAP_Client.H"
#include	"CPS_BUAP_MSG_static_parameters.H"
#include	"CPS_BUAP_MSG_dynamic_parameters.H"
#include	"CPS_BUAP_MSG_last_auto_reload.H"

using namespace std;

//Forward declaration(s)

//Member functions, constructors, destructors, operators

//==========================================================
// Public description for class CPS_BUAP_LoadingSession
// 
// CPS_BUAP_LoadingSession() - Class constructor. Initializes attributes and 
//                                creates dynamic objects 
// 
// ~CPS_BUAP_LoadingSession() - Class destructor. Disallocate memory
// 
// getFn() - Sets the correct filename to use for reload
//
// reportCmdlog() - Writes the cmdlog filenumber to reladmpar
//
// writeLastReloadParams() - Writes the Last Reload Parameters to reladmpar
//
//========================================================== 
 

class  CPS_BUAP_LoadingSession 
{
      
public:
      
  CPS_BUAP_LoadingSession();
  //---------------------------------------------------------- 
  // Description:
  //      Class default constructor . 
  //      Initializes all attributes and creates dynamic objects
  // Parameters: 
  //      None
  // Return value: 
  //      None 
  // Additional information:
  //      None 
  //---------------------------------------------------------- 
      

  ~CPS_BUAP_LoadingSession();
  //---------------------------------------------------------- 
  // Description:
  //      Class destructor . Disallocate memory for the class
  // Parameters: 
  //      None
  // Return value: 
  //      None 
  // Additional information:
  //      None 
  //---------------------------------------------------------- 
 


  void getFn(string& iname);
  //---------------------------------------------------------- 
  // Description:
  // 	Sets the correct filename to use for reload
  // Parameters: 
  //    iname		Name of dump, 
  // Return value: 
  //    None
  // Additional information:
  //    The value will be altered by this method
  //---------------------------------------------------------- 


  void reportCmdlog(R0Info r0);
  //---------------------------------------------------------- 
  // Description:
  // 	Writes the cmdlog filenumber to reladmpar
  // Parameters: 
  //    logNum		Value to write
  // Return value: 
  //    None
  // Additional information:
  //    None
  //---------------------------------------------------------- 

  void writeLastReloadParams();
  //---------------------------------------------------------- 
  // Description:
  // 	Writes the Last Reload Parameters to reladmpar
  // Parameters: 
  //    None
  // Return value: 
  //    None
  // Additional information:
  //    None
  //---------------------------------------------------------- 
      
      
protected:

private:

  void readStaticParams();
  //---------------------------------------------------------- 
  // Description:
  // 	Reads the Static parameters from reladmpar
  // Parameters: 
  //    None
  // Return value: 
  //    None
  // Additional information:
  //    None
  //---------------------------------------------------------- 
      
  void readDynamicParams();
  //---------------------------------------------------------- 
  // Description:
  // 	Reads the Dynamic parameters from reladmpar
  // Parameters: 
  //    None
  // Return value: 
  //    None
  // Additional information:
  //    None
  //---------------------------------------------------------- 
      
  void writeDynamicParams(CPS_BUAP_MSG_dynamic_parameters& wDP);
  //---------------------------------------------------------- 
  // Description:
  // 	Writes the Dynamic parameters to reladmpar
  // Parameters: 
  //    wDP	Object with new values to write to reladmpar
  // Return value: 
  //    None
  // Additional information:
  //    None
  //---------------------------------------------------------- 
      

  int 
  CPS_BUAP_LoadingSession::supTimeExpired(CPS_BUAP_MSG_dynamic_parameters& uDP);
  //--------------------------------------------------------- 
  // Description:
  // 	Checks if the supervision time has expired and sets a 
  //	new one.
  // Parameters: 
  //    uDP	Dynamic Parameters object to set new supervision time in
  // Return value: 
  //    0	Not expired
  //    1	Expired
  // Additional information:
  //	None
  //---------------------------------------------------------- 


private:
      
  CPS_BUAP_MSG_static_parameters	parStatic;
  CPS_BUAP_MSG_dynamic_parameters	parDynamic;
  CPS_BUAP_MSG_last_auto_reload		parLAR;
  CPS_BUAP_Client					parmgr;
  short								APZtype;	// 210; 211; 212 = 212 02/11
												// 20/25/30 = 212 20/25/30

};

 
#endif
