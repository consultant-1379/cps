//******************************************************************************
//
// NAME
//      %File_name:CPS_BUAP_syslog.c%
//
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
//      This class acts as a container for an OPT unsigned 32 bit
//      integer.  The class has input/output operators defined
//      that enable the integer value to be written to and read
//      from a CPS_BUAP_Buffer in the byte order consistant with OPT.

// DOCUMENT NO
//  190 89-CAA 109 1412
//
//  101010 xchihoa     Ported to Linux for APG43L.

#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include <windows.h>

#pragma warning ( disable : 4786 ) // debug identifier truncated

#include "CPS_BUAP_syslog.h"

#ifdef _DEBUG
  extern bool g_consoleMode;
#endif

void syslog(int priority, char *logString, ...)
{
  va_list varArgList; // points to each unnamed arg in turn
  char *p;
  int iVal;
  double dVal;
  char *cStr;
  char intStr[8];
  char c;  char charStr[2];
  int res;
  static int eventId = 0;

  HANDLE hSysLog;
  char szSysFile [] = "AP_SysLog_BUAP"; // olika f?r de olika syslog
  char eventLogStr[512] = "";           //:ag1 up from 80
  const char *strings[8];

  // Register the handle to the event log file "AP_SysLog_BUAP".
  //--------------------------------------------------------------- 
  hSysLog = RegisterEventSource(NULL, szSysFile);
  if (hSysLog == NULL)
  {
	  printf("ERROR register event log\n");
    // fault handling.....
  }

  // Go through the logstring and the parameters to make
  // one string.
  //----------------------------------------------------
  va_start(varArgList, logString);  // make varArgList point to first unnamed arg

  for (p = logString; *p; p++)
  {
    if (*p != '%')
    {
      strncat(eventLogStr, p, 1);
    }
    else
    {
      switch (*++p)
      {
      case 'd':
		iVal = va_arg(varArgList, int);
		sprintf(intStr,"%d", iVal);
		strcat(eventLogStr, intStr);
		break;
      case 'f':
		dVal = va_arg(varArgList, double);
		sprintf(intStr,"%f", dVal);
		strcat(eventLogStr, intStr);
		break;
      case 's':
		cStr = va_arg(varArgList, char *);
		strcat(eventLogStr, cStr);
		break;
      default : // char
		c = va_arg(varArgList, char);
		sprintf(charStr,"%c", c);
		strncat(eventLogStr, charStr, 1);
		break;
      }
    }
  }

  // Write event to log 
  //-------------------
  strings[0] = eventLogStr;
  res = ReportEvent(hSysLog,           // handle return by RegisterEventSource
		    EVENTLOG_INFORMATION_TYPE, // event type to log
		    1,                         // event category
		    eventId++,                 // event identifier
		    NULL,                      // user security identifier (optional)
		    1,                         // number of strings to merge with message
		    0,                         // size of binary data, in bytes
		    strings,                   // array of strings to merge with message
		    NULL);                     // address of binary data

  //:ag1 release handle
  DeregisterEventSource(hSysLog);

  //:ag1
#ifdef _DEBUG
  if(g_consoleMode)
  {
	  puts(strings[0]);
  }
#endif

}

