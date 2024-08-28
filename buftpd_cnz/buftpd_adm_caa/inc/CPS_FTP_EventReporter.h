/*
NAME
    File_name:CPS_FTP_EventReporter.h

Ericsson Utvecklings AB

    COPYRIGHT Ericsson Utvecklings AB, Sweden 1999, 2011. All rights reserved.

    The Copyright to the computer program(s) herein is the property of
    Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
    copied only with the written permission from Ericsson Utvecklings AB
    or in accordance with the terms and conditions stipulated in the
    agreement/contract under which the program(s) have been supplied.

DESCRIPTION
    This class is use for sending the AP Event

DOCUMENT NO
    190 89-CAA 109 1415

AUTHOR
    2003-03-31 by EAB/UKY/SF Mona Notterkvist (uabmnst, tag :mnst)
    2011-14-07 by XDT/DEK/XQUYDAO

SEE ALSO
    -

Revision history
----------------
2003-03-31 uabmnst Created
2011-14-07 xquydao Updated for APG43L

*/

#ifndef CPS_FTP_EventReporter_H
#define CPS_FTP_EventReporter_H

#include "acs_aeh_evreport.h"
#include "CPS_FTP_CS.h"

struct CPS_FTP_EventStruct
{
    long         specificProblem;      // a acs_aeh_specificProblem, defined in acs_aeh_evreport.h
    const char*  percSeverity;         // a acs_aeh_percSeverityy, -"-
    const char*  probableCause;        // a acs_aeh_probableCause, -"-
    const char*  objClassOfReference;  // a acs_aeh_objClassOfReference, -"-
    const char*  objectOfReference;    // a acs_aeh_objectOfReference, -"-
    const char*  problemText;          // a acs_aeh_problemText, -"-
    const char*  formatString;
};

class CPS_FTP_EventReporter
{
public:

    /**
     * Send an event to the event handler
     *
     * @param[in] eventStruct Event to be sent
     * @param[in] dummy       The argument to ensure the correct operation of va_start
     * @param[in] ...         Argument list. MUST HAVE ONE FOR EACH FIELD in formatString
     * @return N/A
     */
    void send(const CPS_FTP_EventStruct &eventStruct, int dummy, ...);

    /**
     * Return the singleton of event reporter
     *
     * @param N/A
     *
     * @return The singleton of event reporter
     */
    static CPS_FTP_EventReporter& instance() {return s_instance;}


private:
    acs_aeh_evreport m_evhr; // Event handler reporting
    CPS_FTP_CS s_cs; // Critical section
    static CPS_FTP_EventReporter s_instance; // Singleton of event reporter
    static char s_processName[32];
};

#define EVENT(p1)		\
        CPS_FTP_EventReporter::instance().send p1;

#endif
