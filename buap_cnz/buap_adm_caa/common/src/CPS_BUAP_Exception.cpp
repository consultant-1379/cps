//******************************************************************************
//
// NAME
//      %File_name:CPS_BUAP_Exception.C%
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
//      This class is used for the raising of all exceptions within
//      the process buparmgr and the programs bupset and bupprint.
//      The class constructor takes parameters showing where the
//      exception was thrown, why it was thrown, and the file and line
//      number of where it was thrown.

// DOCUMENT NO
//  190 89-CAA 109 1412

// AUTHOR 
// 	1997/08/06 by U/B/SD David Wade

// SEE ALSO 
// 	<Related classes, (abstract) baseclasses, friend classes, ...>
//
//******************************************************************************
// === Revision history ===
// 061003 uablan      Added [] after delete in destructor.
//  101010 xchihoa     Ported to Linux for APG43L.
//******************************************************************************


#include <time.h>
#include <string.h>
#include <stdio.h>

#include "CPS_BUAP_Trace.h"
#include "CPS_BUAP_Exception.h"


//******************************************************************************
//	CPS_BUAP_Exception()
//******************************************************************************
CPS_BUAP_Exception::CPS_BUAP_Exception(const char *file, int line, const char *whatHappened)
{
  time_t tm;
  time(&tm);

  strcpy(m_datetime, ctime(&tm));
  m_datetime[24]=0;

  m_file         = new char[strlen(file)+1];
  strcpy(m_file, file);

  m_line         = line;

  m_whatHappened = new char[strlen(whatHappened)+1];
  strcpy(m_whatHappened, whatHappened);

};

//******************************************************************************
//	~CPS_BUAP_Exception()
//******************************************************************************
CPS_BUAP_Exception::~CPS_BUAP_Exception()
{ 
  delete [] m_file;
  delete [] m_whatHappened;
};

//******************************************************************************
//	Copy constructor
//:ag1
//******************************************************************************
CPS_BUAP_Exception::CPS_BUAP_Exception(const CPS_BUAP_Exception& o)
{
  strcpy(m_datetime, o.m_datetime);
  m_line         = o.m_line;

  m_file         = new char[strlen(o.m_file) + 1];
  strcpy(m_file, o.m_file);

  m_whatHappened = new char[strlen(o.m_whatHappened) + 1];
  strcpy(m_whatHappened, o.m_whatHappened);
}

//******************************************************************************
//	Assignment operator
//:ag1
//******************************************************************************
CPS_BUAP_Exception& CPS_BUAP_Exception::operator=(const CPS_BUAP_Exception& rhs)
{
	if(this != &rhs)
	{
		strcpy(m_datetime, rhs.m_datetime);
		m_line         = rhs.m_line;
		
		m_file         = new char[strlen(rhs.m_file) + 1];
		strcpy(m_file, rhs.m_file);
		
		m_whatHappened = new char[strlen(rhs.m_whatHappened) + 1];
		strcpy(m_whatHappened, rhs.m_whatHappened);
	}
	return *this;
}

//******************************************************************************
//	Report()
//******************************************************************************
void CPS_BUAP_Exception::Report()
{ 
  printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
  printf("!!!                           E X C E P T I O N                             !!!\n");
  printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
  printf("!!! FILE    : %60s  !!!\n", m_file);
  printf("!!! LINE    : %60d  !!!\n", m_line);
  printf("!!! REASON  : %60s  !!!\n", m_whatHappened);
  printf("!!! DATE    : %60s  !!!\n", m_datetime);
  printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
};
