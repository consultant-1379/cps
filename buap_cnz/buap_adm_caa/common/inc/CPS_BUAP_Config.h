#ifndef _CPS_BUAP_CONFIG_H_
#define _CPS_BUAP_CONFIG_H_
/*
NAME
	CPS_BUAP_CONFIG -
LIBRARY 3C++
PAGENAME CPS_BUAP_CONFIG
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE CPS_BUAP_CONFIG.H

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION
	BUAP configuration data.

DOCUMENT NO
//  190 89-CAA 109 1412

AUTHOR
	2003-01-07 by EAB/KY/SF Mona N???tterkvist (uabmnst, tag :mnst)

SEE ALSO
	-

Revision history
----------------
2003-01-07 uabmnst Created
//  101010 xchihoa     Ported to Linux for APG43L.

*/
#pragma once

#include "CPS_BUAP_Linux.h"

#include <assert.h>
#include <string>

using namespace std;

class CPS_BUAP_Config {
// types
private:

public:
	//
	// keys, sections, items & such
	//
	// registry
	static const char* REG_ROOT_KEY;
	static const char* REGITEM_RELADMPAR_DIR;
	//
	// add-ons & common constants
	//
	static const char* CONFIG_FILE;
	static const char* LOGICAL_NAME_CPS_DATA;

//foos
public:
	static const CPS_BUAP_Config& instance() {
		if(!s_initialized)
			s_instance.init();
		return s_instance;
	}
// access
	// [Paths]
	const string& cpsReladmpar() const { return m_cpsReladmpar; }
	bool isClassicCP() const;

// modify
	// [Paths]
	void cpsReladmpar(const char* val) { m_cpsReladmpar = val; }

private:
	CPS_BUAP_Config();
	CPS_BUAP_Config(const CPS_BUAP_Config& rhs);
	CPS_BUAP_Config& operator=(const CPS_BUAP_Config& rhs);
	void init();
	void expandPath(string& orig_s);
	string getPath();
//attr
private:
	// [Paths]
	string m_cpsReladmpar;
	static bool s_initialized; // have we read the values?
	static CPS_BUAP_Config s_instance;
};

#endif
