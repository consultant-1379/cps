//******************************************************************************
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 1999.
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
//  CPS_BUAP_EventReporter.cpp
//
//  DESCRIPTION 
//  Class implements a debug context class used in the
//  development of the reset of the code.  Note that no
//  methods are to be called directly, rather by use of
//  macros so that the debug code may be conditionally
//  compiled out.
//
//  DOCUMENT NO
//  190 89-CAA 109 1412
//
//  AUTHOR 
// 	1999/06/14 by U/B/SF Birgit Berggren
//  101010 xchihoa     Ported to Linux for APG43L.
//
//******************************************************************************
// === Revision history ===
// 990614 BIR PA1 Created.
// 990618 BIR PA2 Updated after review.
// 120419 xngudan     Updated.
//******************************************************************************


#include "CPS_BUAP_EventReporter.h"
#include "CPS_BUAP_Trace.h"

#include <string>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>


CPS_BUAP_EventReporter CPS_BUAP_EventReporter::s_instance;
char CPS_BUAP_EventReporter::s_processName[32] = {0};


void CPS_BUAP_EventReporter::send(const CPS_BUAP_EventStruct &eventStruct,
                                    int dummy, ...)
{
   newTRACE((LOG_LEVEL_WARN, "CPS_BUAP_EventReporter::send()", 0));
   AutoCS a(m_criSection);

   char l_problemData[256];

   if (s_processName[0] == 0)
   {
       pid_t pid = getpid();
       sprintf(s_processName, "%s:%u", "cps_buapd", pid);
   }

   va_list l_args;
   va_start(l_args, dummy);
   vsprintf(l_problemData, eventStruct.formatString, l_args);
   va_end(l_args);

   TRACE((LOG_LEVEL_WARN, "EVENT REPORT= *PROBLEM:%ld *PROBLEM TEXT:%s",
           0, eventStruct.specificProblem, eventStruct.problemText));

   if (m_evhr.sendEventMessage(s_processName, eventStruct.specificProblem,
         eventStruct.percSeverity, eventStruct.probableCause,
         eventStruct.objClassOfReference, eventStruct.objectOfReference,
         eventStruct.problemText, l_problemData) != ACS_AEH_ok)
   {
      TRACE((LOG_LEVEL_ERROR, "ERROR in ACS AEH API sendEventMessage(): %s", 0, m_evhr.getErrorText()))
   }
}
