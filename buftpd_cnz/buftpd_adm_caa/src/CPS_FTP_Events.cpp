/*
NAME
    File_name:CPS_FTP_Events.cpp

Ericsson Utvecklings AB

    COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

    The Copyright to the computer program(s) herein is the property of
    Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
    copied only with the written permission from Ericsson Utvecklings AB
    or in accordance with the terms and conditions stipulated in the
    agreement/contract under which the program(s) have been supplied.

DESCRIPTION
    Class is just the container or events

DOCUMENT NO
    190 89-CAA 109 1415

AUTHOR
    2011-14-07 by XDT/DEK/XQUYDAO


SEE ALSO
    -

Revision history
----------------
2011-14-07 xquydao Created

*/

#include "CPS_FTP_Events.h"

CPS_FTP_EventStruct
CPS_FTP_Events::listener_init_fail=
{
  /* specific prob  */ 22038,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "",
  /* problem text   */ "All listener failed to init",
  /* problem data   */ "Create listener at port: %u, errno = %d text = %s"
};

CPS_FTP_EventStruct
CPS_FTP_Events::select_call_fail=
{
  /* specific prob  */ 22039,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "",
  /* problem text   */ "Select call failed",
  /* problem data   */ "Select: errno = %d text = %s"
};

CPS_FTP_EventStruct
CPS_FTP_Events::listener_accept_fail=
{
  /* specific prob  */ 22040,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "",
  /* problem text   */ "Listener failed to accept connection",
  /* problem data   */ "Listener %d accept new connection: errno = %d text = %s"
};
 
CPS_FTP_EventStruct
CPS_FTP_Events::datasock_open_fail=
{
  /* specific prob  */ 22041,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "",
  /* problem text   */ "Data socket failed to open",
  /* problem data   */ "Open data socket: errno = %d text = %s"
};

CPS_FTP_EventStruct
CPS_FTP_Events::datasock_connect_fail=
{
  /* specific prob  */ 22042,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "",
  /* problem text   */ "Data socket failed to connect",
  /* problem data   */ "Connect data socket: errno = %d text = %s"
};

CPS_FTP_EventStruct
CPS_FTP_Events::datafile_open_fail=
{
  /* specific prob  */ 22043,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "",
  /* problem text   */ "Data transfer file failed to open",
  /* problem data   */ "Open data transfer file %s: errno = %d text = %s"
};

CPS_FTP_EventStruct
CPS_FTP_Events::ctrlsock_send_fail=
{
  /* specific prob  */ 22044,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "",
  /* problem text   */ "Control socket failed to send data",
  /* problem data   */ "Control socket %d send: errno = %d text = %s"
};

CPS_FTP_EventStruct
CPS_FTP_Events::datasock_send_fail=
{
  /* specific prob  */ 22045,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "",
  /* problem text   */ "Data socket failed to send data",
  /* problem data   */ "Data socket %d send: errno = %d text = %s"
};

CPS_FTP_EventStruct
CPS_FTP_Events::ctrl_connection_add_fail=
{
  /* specific prob  */ 22046,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "",
  /* problem text   */ "Failed to add control connection",
  /* problem data   */ "Add control connection: FD = %d"
};

CPS_FTP_EventStruct
CPS_FTP_Events::data_connection_add_fail=
{
  /* specific prob  */ 22047,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "",
  /* problem text   */ "Failed to add data connection",
  /* problem data   */ "Add data connection: control FD = %d, data FD = %d, data transfer FD = %d"
};

CPS_FTP_EventStruct
CPS_FTP_Events::service_healthcheck_fail=
{
  /* specific prob  */ 22048,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "",
  /* problem text   */ "Failed to do the health check for BUFTP server",
  /* problem data   */ "BUFTPD stopped, service health check failed"
};

CPS_FTP_EventStruct
CPS_FTP_Events::Unrecoverable_error_report=
{
  /* specific prob  */ 22049,
  /* severity       */ "EVENT",
  /* probable cause */ "AP INTERNAL FAULT",
  /* class of ref   */ "APZ",
  /* object of ref  */ "",
  /* problem text   */ "Major error occurred, report to AMF for the application retart",
  /* problem data   */ "BUFTPD server stopped, reporting to AMF"
};
