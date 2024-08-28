//******************************************************************************
// 
// .NAME
//  	CPS_BUAP_MSG_IDs - Message identity values
// .LIBRARY 3C++
// .PAGENAME CPS_BUAP_MSG_IDs
// .HEADER  CPS  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE CPS_BUAP_MSG_IDs.H

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
//	This header file simply defines all the message identity
//	values used within CPS_BUAP_PARMGR/PARMGR_CLIENT and should
//	be used by all clients wishing to communicate with PARMGR.

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

// CHANGES
// 050114 uablan - Remove messages for locking file reladmpar

// .LINKAGE
//	-

// .SEE ALSO 
// 	-
//  101010 xchihoa     Ported to Linux for APG43L.
//******************************************************************************

#ifndef CPS_BUAP_MSG_IDs_H
#define CPS_BUAP_MSG_IDs_H

//
// The following are the AP based JTP message in order of message ID
//
const unsigned short CPS_BUAP_MSG_ID_read_static = 100;
const unsigned short CPS_BUAP_MSG_ID_read_static_ok = 101;
const unsigned short CPS_BUAP_MSG_ID_read_static_fail = 102;

const unsigned short CPS_BUAP_MSG_ID_write_static = 200;
const unsigned short CPS_BUAP_MSG_ID_write_static_ok = 201;
const unsigned short CPS_BUAP_MSG_ID_write_static_fail = 202;

const unsigned short CPS_BUAP_MSG_ID_read_dynamic = 300;
const unsigned short CPS_BUAP_MSG_ID_read_dynamic_ok = 301;
const unsigned short CPS_BUAP_MSG_ID_read_dynamic_fail = 302;

const unsigned short CPS_BUAP_MSG_ID_write_dynamic = 400;
const unsigned short CPS_BUAP_MSG_ID_write_dynamic_ok = 401;
const unsigned short CPS_BUAP_MSG_ID_write_dynamic_fail = 402;

const unsigned short CPS_BUAP_MSG_ID_read_LAR = 500;
const unsigned short CPS_BUAP_MSG_ID_read_LAR_ok = 501;
const unsigned short CPS_BUAP_MSG_ID_read_LAR_fail = 502;

const unsigned short CPS_BUAP_MSG_ID_write_LAR = 600;
const unsigned short CPS_BUAP_MSG_ID_write_LAR_ok = 601;
const unsigned short CPS_BUAP_MSG_ID_write_LAR_fail = 602;

const unsigned short CPS_BUAP_MSG_ID_read_clogrefs = 700;
const unsigned short CPS_BUAP_MSG_ID_read_clogrefs_ok = 701;
const unsigned short CPS_BUAP_MSG_ID_read_clogrefs_fail = 702;

const unsigned short CPS_BUAP_MSG_ID_read_dmeasures = 800;
const unsigned short CPS_BUAP_MSG_ID_read_dmeasures_ok = 801;
const unsigned short CPS_BUAP_MSG_ID_read_dmeasures_fail = 802;

//const unsigned short CPS_BUAP_MSG_ID_lock           = 900;
//  const unsigned short CPS_BUAP_MSG_ID_lock_ok         = 901;
//  const unsigned short CPS_BUAP_MSG_ID_lock_fail       = 902;

//
// The following are the CP based JTP message in order of message ID
// DO NOT CHANGE THESE VALUES UNLESS CP IS UPDATED ALSO.

const unsigned short CPS_BUAP_MSG_ID_new_command = 1;
const unsigned short CPS_BUAP_MSG_ID_new_buffer = 4;
const unsigned short CPS_BUAP_MSG_ID_complete = 2;
const unsigned short CPS_BUAP_MSG_ID_ordered = 3;
const unsigned short CPS_BUAP_MSG_ID_no_answer = 5;

const unsigned short CPS_BUAP_MSG_ID_read_reladmpar = 8;
const unsigned short CPS_BUAP_MSG_ID_read_reladmpar_ok = 9;
const unsigned short CPS_BUAP_MSG_ID_read_reladmpar_fail = 13;

const unsigned short CPS_BUAP_MSG_ID_write_clog = 10;
const unsigned short CPS_BUAP_MSG_ID_write_clog_ok = 11;
const unsigned short CPS_BUAP_MSG_ID_write_clog_fail = 15;

const unsigned short CPS_BUAP_MSG_ID_write_dmr = 12;
const unsigned short CPS_BUAP_MSG_ID_write_dmr_ok = 17;
const unsigned short CPS_BUAP_MSG_ID_write_dmr_fail = 19;

#endif
