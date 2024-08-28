//******************************************************************************
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 2011.
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
//  CPS_BUAP_Events.h
//
//  DESCRIPTION 
//  Class is just a container for event enumerations as
//  namespaces are not implemented.
//
//  DOCUMENT NO
//  190 89-CAA 109 1412
//
//  AUTHOR 
//  1999/05/31 by UAB/B/SF Birgit Berggren
//
//******************************************************************************
// === Revision history ===
// 990531 BIR PA1 Created.
// 990617 BIR PA2 Updated after review.
// 990913 BIR PA3 New event:22035.
// 101010 xchihoa     Ported to Linux for APG43L.
// 120308 xngudan     Updated with new OI events.
//******************************************************************************

#ifndef CPS_BUAP_Events_H
#define CPS_BUAP_Events_H

//#include <APGCC_Event.H>

#include "CPS_BUAP_EventReporter.h"

class CPS_BUAP_Events
{
public:

    //
    // JTP Error events
    //
    static CPS_BUAP_EventStruct jidrepreq_fail;                // 22001
    static CPS_BUAP_EventStruct jexinitreq_fail;               // 22002
    static CPS_BUAP_EventStruct jexinitind_fail;               // 22003
    static CPS_BUAP_EventStruct jexinitrsp_fail;               // 22004
    static CPS_BUAP_EventStruct jexinitconf_fail;              // 22005
    static CPS_BUAP_EventStruct jexdatareq_fail;               // 22006
    static CPS_BUAP_EventStruct jexdataind_fail;               // 22007
    static CPS_BUAP_EventStruct jexdiscreq_fail;               // 22008
    static CPS_BUAP_EventStruct WaitForSingleObject_timeout;   // 22009

    //
    // Windows NT Events
    //
    static CPS_BUAP_EventStruct beginthreadex_fail;            // 22040
    static CPS_BUAP_EventStruct GetExitCodeThread_fail;        // 22041
    static CPS_BUAP_EventStruct WaitForMultipleObjects_fail;   // 22042
    static CPS_BUAP_EventStruct WaitForSingleObject_fail;      // 22043


    //
    // RELADMPAR file errors
    //
    static CPS_BUAP_EventStruct reladmpar_fail;                // 22020
    static CPS_BUAP_EventStruct reladmpar_create_fail;         // 22021
    static CPS_BUAP_EventStruct reladmpar_write_fail;          // 22022
    static CPS_BUAP_EventStruct reladmpar_seek_fail;           // 22024
    static CPS_BUAP_EventStruct reladmpar_read_fail;           // 22023

    //
    // Object Implementer/Manager errors
    //
    static CPS_BUAP_EventStruct registerOI_fail;              // 22010
    static CPS_BUAP_EventStruct dispatchIMMevent_fail;        // 22011
    static CPS_BUAP_EventStruct initializeOM_fail;            // 22012
    static CPS_BUAP_EventStruct OInotrunning;                 // 22013
    static CPS_BUAP_EventStruct modifyAttr_failed;            // 22014
    static CPS_BUAP_EventStruct initializeObj_failed;         // 22015

    //
    // Other faults
    //
    static CPS_BUAP_EventStruct no_env;                        // 22033
    static CPS_BUAP_EventStruct no_server;                     // 22000
    static CPS_BUAP_EventStruct invalid_message;               // 22030
    static CPS_BUAP_EventStruct no_such_command;               // 22031
    static CPS_BUAP_EventStruct death_of_server;               // 22032
    static CPS_BUAP_EventStruct lost_handle;                   // 22034
    static CPS_BUAP_EventStruct cannot_write_LRP;              // 22035
    static CPS_BUAP_EventStruct no_regval;                     // 22036
    static CPS_BUAP_EventStruct no_expand_path_val;            // 22037
    static CPS_BUAP_EventStruct cannot_create_eventFD;         // 22038

};

#endif
