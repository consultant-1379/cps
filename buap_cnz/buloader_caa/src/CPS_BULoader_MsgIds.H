//*****************************************************************************
// 
// .NAME
//  	CPS_BUAP_Loader_MsgIDs - Message IDs for JTP communication
// .LIBRARY 3C++
// .PAGENAME CPS_BUAP_Loader_MsgIDs
// .HEADER  ACS  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE CPS_BUAP_Loader_MsgIDs.H

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
// 	Message IDs for JTP communication

// .ERROR HANDLING
//
//	General rule:
//	The error handling is specified for each method.
//
//	No methods initiate or send error reports unless specified.

// DOCUMENT NO
//	CAA 109 0081

// AUTHOR 
// 	Thu Sep 18 16:58:14 1997 by UAB/I/GM UABFLN

// .LINKAGE
//	-

// .SEE ALSO 
// 	-

//******************************************************************************

#ifndef CPS_BUAP_Loader_MsgIDs_H 
#define CPS_BUAP_Loader_MsgIDs_H

// This is external jtp messages that goes between the buloader server
// and the AXE-CP.  
//*** Do NOT change theese without confirming changes in the CP ***

const ushort CPS_BULoader_MsgId_CPLoadDone		= 22;	
const ushort CPS_BULoader_MsgId_CPLoadDone_ok		= 23;	


// This is internal jtp messages that goes between the buloader server
// and the bufile clients.

const ushort CPS_BULoader_MsgId_ConnectClient		= 100;	

const ushort CPS_BULoader_MsgId_getFn			= 110;	
const ushort CPS_BULoader_MsgId_getFn_ok		= 111;	
const ushort CPS_BULoader_MsgId_getFn_failed		= 112;	

const ushort CPS_BULoader_MsgId_reportCmdLog		= 120;
const ushort CPS_BULoader_MsgId_reportCmdLog_ok		= 121;
const ushort CPS_BULoader_MsgId_reportCmdLog_failed	= 122;


#endif
