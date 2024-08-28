//******************************************************************************
//
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
//  CPS_BUAP_syslog.h
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
//  101010 xchihoa     Ported to Linux for APG43L.

#ifndef CPS_BUAP_syslog_h
#define CPS_BUAP_syslog_h

#define LOG_DEBUG 1

extern void syslog(int priority, char *logString, ...);

#endif // CPS_BUAP_syslog_h
