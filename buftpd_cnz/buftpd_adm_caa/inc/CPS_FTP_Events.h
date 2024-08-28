/*
NAME
    File_name:CPS_FTP_Events.h

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
#ifndef CPS_FTP_Events_H
#define CPS_FTP_Events_H

#include "CPS_FTP_EventReporter.h"

class CPS_FTP_Events
{
public:
	static CPS_FTP_EventStruct listener_init_fail;          // 22038
	static CPS_FTP_EventStruct select_call_fail;            // 22039
	static CPS_FTP_EventStruct listener_accept_fail;        // 22040
	static CPS_FTP_EventStruct datasock_open_fail;          // 22041
	static CPS_FTP_EventStruct datasock_connect_fail;       // 22042
	static CPS_FTP_EventStruct datafile_open_fail;          // 22043
	static CPS_FTP_EventStruct ctrlsock_send_fail;          // 22044
	static CPS_FTP_EventStruct datasock_send_fail;          // 22045
	static CPS_FTP_EventStruct ctrl_connection_add_fail;    // 22046
	static CPS_FTP_EventStruct data_connection_add_fail;    // 22047
	static CPS_FTP_EventStruct service_healthcheck_fail;    // 22048
	static CPS_FTP_EventStruct Unrecoverable_error_report;  // 22049
};

#endif
