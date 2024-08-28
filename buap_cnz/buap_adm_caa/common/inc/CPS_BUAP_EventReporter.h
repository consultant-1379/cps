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
//  CPS_BUAP_EventReporter.h
//
//  DESCRIPTION 
//
//  DOCUMENT NO
//  190 89-CAA 109 1412
//
//  AUTHOR 
// 	1999/06/14 by UAB/B/SF Birgit Berggren
//
//******************************************************************************
// === Revision history ===
// 990614 BIR PA1 Created.
// 990618 BIR PA2 Updated after review.
//  101010 xchihoa     Ported to Linux for APG43L.
// 120419 xngudan     Updated.
//******************************************************************************


#ifndef CPS_BUAP_EventReporter_H
#define CPS_BUAP_EventReporter_H


#include "acs_aeh_evreport.h"
#include "CPS_BUAP_CS.h"

struct CPS_BUAP_EventStruct
{
  long         specificProblem;      // a ACS_AEH_SpecificProblem, defined in ACS_AEH_EvReport.H
  const char*  percSeverity;         // a ACS_AEH_PercSeverity, -"-
  const char*  probableCause;        // a ACS_AEH_ProbableCause, -"-
  const char*  objClassOfReference;  // a ACS_AEH_ObjClassOfReference, -"-
  const char*  objectOfReference;    // a ACS_AEH_ObjectOfReference, -"-
  const char*  problemText;          // a ACS_AEH_ProblemText, -"-
  const char*  formatString;
};

class CPS_BUAP_EventReporter
{
public:

  void send(const CPS_BUAP_EventStruct &eventStruct, int dummy, ...);
  // Description:
  // 	Method used to send an event to the event handle demon.
  // Parameters: 
  //	eventStruct	Structure defining a specific event.
  //	dummy		This argument is needed to ensure the 
  //			correct operation of va_start.
  //			See "STDARGS.h"and "va_start".
  //    ...		A variable number of parameters.  
  //			MUST HAVE ONE FOR EACH FIELD IN formatString
  // Return value: 
  //    -
  // Additional information:
  //    -

  static CPS_BUAP_EventReporter& instance() { return s_instance;}
  // Description:
  //   Method used to return the singleton of event reporter
  // Parameters:
  //   -
  // Return value:
  //   The singleton of event reporter
  // Additional information:
  //   -


private:
  acs_aeh_evreport  m_evhr;              // Event handler reporting
  CPS_BUAP_CS m_criSection;              // Critical Section
  static CPS_BUAP_EventReporter s_instance;    // Singleton of event reporter
  static char s_processName[32];
};


#define EVENT(p1)		\
        CPS_BUAP_EventReporter::instance().send p1;

#endif
