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
//  CPS_BUAP_Events.cpp 
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

#include "CPS_BUAP_Events.h"

//SKA TAS BORT
//CPS_BUAP_Events CPSBUAPEvents; Kan inte se att det anv?nds


CPS_BUAP_EventStruct
CPS_BUAP_Events::no_server=
{
  /* specific prob  */ 22000,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "",
  /* problem text   */ "could not connect to server",
  /* problem data   */ ""
};

CPS_BUAP_EventStruct
CPS_BUAP_Events::jidrepreq_fail=
{
  /* specific prob  */ 22001,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "JTP",
  /* problem text   */ "could not report destinator to directoy services",
  /* problem data   */ "DESTINATOR\n%-12s"
};

CPS_BUAP_EventStruct
CPS_BUAP_Events::jexinitreq_fail=
{
  /* specific prob  */ 22002,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "JTP",
  /* problem text   */ "jexinitreq failed, could not open communication with server",
  /* problem data   */ "DESTINATOR\n%-12s"
};

CPS_BUAP_EventStruct
CPS_BUAP_Events::jexinitind_fail=
{
  /* specific prob  */ 22003,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "JTP",
  /* problem text   */ "jexinitind failed, conversation initiation expected, but not received",
  /* problem data   */ "DESTINATOR\n%-12s"
};

CPS_BUAP_EventStruct
CPS_BUAP_Events::jexinitrsp_fail=
{
  /* specific prob  */ 22004,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "JTP",
  /* problem text   */ "jexinitrsp failed, confirmation not received from server",
  /* problem data   */ "DESTINATOR\n%-12s"
};

CPS_BUAP_EventStruct
CPS_BUAP_Events::jexinitconf_fail=
{
  /* specific prob  */ 22005,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "JTP",
  /* problem text   */ "jexinitconf failed, confirmation not received from server",
  /* problem data   */ "DESTINATOR\n%-12s"
};

CPS_BUAP_EventStruct
CPS_BUAP_Events::jexdatareq_fail=
{
  /* specific prob  */ 22006,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "JTP",
  /* problem text   */ "jexdatareq failed, could not send data request to server",
  /* problem data   */ "DESTINATOR\n%-12s"
};

CPS_BUAP_EventStruct
CPS_BUAP_Events::jexdataind_fail=
{
  /* specific prob  */ 22007,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "JTP",
  /* problem text   */ "jexdataind failed, no data received from server",
  /* problem data   */ "DESTINATOR\n%-12s"
};

CPS_BUAP_EventStruct
CPS_BUAP_Events::jexdiscreq_fail=
{
  /* specific prob  */ 22008,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "JTP",
  /* problem text   */ "jexdiscreq failed, could not send disconnection request to server",
  /* problem data   */ "DESTINATOR\n%-12s"
};

CPS_BUAP_EventStruct
CPS_BUAP_Events::WaitForSingleObject_timeout=
{
  /* specific prob  */ 22009,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "JTP",
  /* problem text   */ "select() timeout, data not received within the required time",
  /* problem data   */ "DESTINATOR     TIMEOUT=%ums\n%-12s   %u"
};

CPS_BUAP_EventStruct
CPS_BUAP_Events::reladmpar_fail=
{
  /* specific prob  */ 22020,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "filesystem",
  /* problem text   */ "error when accessing RELADMPAR file",
  /* problem data   */ "ERROR\n%s\n\nFILE\n%s"
};

CPS_BUAP_EventStruct
CPS_BUAP_Events::reladmpar_create_fail=
{
  /* specific prob  */ 22021,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "filesystem",
  /* problem text   */ "error trying to create RELADMPAR file",
  /* problem data   */ "ERROR\n%s\n\nFILE\n%s"
};

CPS_BUAP_EventStruct
CPS_BUAP_Events::reladmpar_write_fail=
{
  /* specific prob  */ 22022,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "filesystem",
  /* problem text   */ "error trying to write RELADMPAR file",
  /* problem data   */ "ERROR\n%s\n\nFILE\n%s"
};

CPS_BUAP_EventStruct
CPS_BUAP_Events::reladmpar_seek_fail=
{
  /* specific prob  */ 22024,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "filesystem",
  /* problem text   */ "error trying to seek in RELADMPAR file",
  /* problem data   */ "ERROR\n%s\n\nFILE\n%s"
};

CPS_BUAP_EventStruct
CPS_BUAP_Events::reladmpar_read_fail=
{
  /* specific prob  */ 22023,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "filesystem",
  /* problem text   */ "error trying to read RELADMPAR file",
  /* problem data   */ "ERROR\n%s\n\nFILE\n%s"
};

CPS_BUAP_EventStruct
CPS_BUAP_Events::invalid_message=
{
  /* specific prob  */ 22030,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "JTP",
  /* problem text   */ "invalid message identifier received",
  /* problem data   */ "USERDATA1\n%u"
};

CPS_BUAP_EventStruct
CPS_BUAP_Events::no_such_command=
{
  /* specific prob  */ 22031,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "",
  /* problem text   */ "command is not recognised",
  /* problem data   */ "COMMAND\n%s"
};

CPS_BUAP_EventStruct
CPS_BUAP_Events::death_of_server=
{
  /* specific prob  */ 22032,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "",
  /* problem text   */ "server is exiting, check previous events for specific reason",
  /* problem data   */ "SERVER\n%s"
};

CPS_BUAP_EventStruct
CPS_BUAP_Events::no_env=
{
  /* specific prob  */ 22033,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "",
  /* problem text   */ "environment variable not defined",
  /* problem data   */ "VARIABLE\n%s"
};

CPS_BUAP_EventStruct
CPS_BUAP_Events::beginthreadex_fail=
{
  /* specific prob  */ 22040,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "NT",
  /* problem text   */ "beginthreadex failed",
  /* problem data   */ "ERROR CODE\n%u"
};

CPS_BUAP_EventStruct
CPS_BUAP_Events::GetExitCodeThread_fail=
{
  /* specific prob  */ 22041,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "NT",
  /* problem text   */ "GetExitCodeThread failed",
  /* problem data   */ "ERROR CODE\n%u"
};

CPS_BUAP_EventStruct
CPS_BUAP_Events::lost_handle=
{
  /* specific prob  */ 22034,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "",
  /* problem text   */ "lost handle",
  /* problem data   */ "THREAD\n%-30s"
};

CPS_BUAP_EventStruct
CPS_BUAP_Events::WaitForMultipleObjects_fail=
{
  /* specific prob  */ 22042,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "NT",
  /* problem text   */ "select() failed",
  /* problem data   */ "ERROR CODE\n%u"
};

CPS_BUAP_EventStruct
CPS_BUAP_Events::WaitForSingleObject_fail=
{
  /* specific prob  */ 22043,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "NT",
  /* problem text   */ "select() failed",
  /* problem data   */ "ERROR CODE\n%u"
};

CPS_BUAP_EventStruct
CPS_BUAP_Events::cannot_write_LRP=
{
  /* specific prob  */ 22035,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "NT",
  /* problem text   */ "Cannot write Last Reload Parameters",
  /* problem data   */ ""
};

CPS_BUAP_EventStruct
CPS_BUAP_Events::no_regval=
{
  /* specific prob  */ 22036,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "",
  /* problem text   */ "registry value not found",
  /* problem data   */ "VARIABLE\n%s"
};

CPS_BUAP_EventStruct
CPS_BUAP_Events::no_expand_path_val=
{
  /* specific prob  */ 22037,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "",
  /* problem text   */ "path value not found",
  /* problem data   */ "VARIABLE\n%s"
};
 

CPS_BUAP_EventStruct
CPS_BUAP_Events::registerOI_fail=
{
  /* specific prob  */ 22010,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "",
  /* problem text   */ "Failed to register as OI for owner class",
  /* problem data   */ ""
};

CPS_BUAP_EventStruct
CPS_BUAP_Events::dispatchIMMevent_fail=
{
  /* specific prob  */ 22011,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "",
  /* problem text   */ "Failed to dispatch IMM events",
  /* problem data   */ ""
};


CPS_BUAP_EventStruct
CPS_BUAP_Events::initializeOM_fail=
{
  /* specific prob  */ 22012,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "",
  /* problem text   */ "Failed to initialize interaction with IMM",
  /* problem data   */ ""
};


CPS_BUAP_EventStruct
CPS_BUAP_Events::OInotrunning=
{
  /* specific prob  */ 22013,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "",
  /* problem text   */ "OI for Classic not running",
  /* problem data   */ ""
};


CPS_BUAP_EventStruct
CPS_BUAP_Events::modifyAttr_failed=
{
  /* specific prob  */ 22014,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "",
  /* problem text   */ "Modify attributes for Classic parameters failed",
  /* problem data   */ ""
};


CPS_BUAP_EventStruct
CPS_BUAP_Events::initializeObj_failed=
{
  /* specific prob  */ 22015,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "",
  /* problem text   */ "Initialize objects for Classic failed",
  /* problem data   */ ""
};


CPS_BUAP_EventStruct
CPS_BUAP_Events::cannot_create_eventFD=
{
  /* specific prob  */ 22038,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "",
  /* problem text   */ "Cannot create event file descriptor",
  /* problem data   */ ""
};





