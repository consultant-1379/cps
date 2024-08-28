/*
NAME
	File_name:Key.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2002. All rights reserved.

The Copyright to the computer program(s) herein is the property of Ericsson
Utvecklings AB, Sweden.
The program(s) may be used and/or copied only with the written permission from
Ericsson Utvecklings AB or in accordance with the terms and conditions
stipulated in the agreement/contract under which the program(s) have been
supplied.

DESCRIPTION
	Unique identifier based on a GUID/UUID.

DOCUMENT NO
	190 89-CAA 109 0387

AUTHOR
	2002-05-31 by U/Y/SF Anders Gillgren (qabgill, tag :ag)

SEE ALSO
	Memory layout must match "File Format Specification".

Revision history
----------------
2002-05-31 qabgill Created
2002-11-04 uablan  Add ACS_Trace
*/

#include "Key.h"

#include "CPS_BUSRV_Trace.h"

#include <assert.h>
#include <string.h>

/// Use boost.uuid library for generating key
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <string>
#include <boost/lexical_cast.hpp>
//#include <uuid/uuid.h>

//
// ctor
//===========================================================================
Key::Key(bool init /* = true */) {

	newTRACE((LOG_LEVEL_INFO, "Key::Key(%d /* = true */)", 0, init));

	if(init)
		newKey();
	else
		memset(m_str, 0, sizeof(m_str));
}
//
// set from string
//===========================================================================
void Key::set(const char* pc) {

	newTRACE((LOG_LEVEL_INFO, "Key::set(%s)", 0, pc));

	if(pc && pc[0])
		strncpy(m_str, pc, LENGTH + 1);
	else
		memset(m_str, 0, sizeof(m_str));
}
//
//
//===========================================================================
void Key::newKey() {
	newTRACE((LOG_LEVEL_INFO, "Key::newKey()", 0));

    boost::uuids::uuid uuid;
    boost::uuids::random_generator generator;

    // Generate UUID from random generator
    uuid = generator();
    // Convert UUID to string
    std::string uuidString = boost::lexical_cast<std::string, boost::uuids::uuid>(uuid);
    // Check whether the UUID key length is 36
    assert(uuidString.size() == LENGTH);
    // Copy to m_str
    memset(m_str, 0, sizeof(m_str));
    uuidString.copy(m_str, LENGTH);

	TRACE((LOG_LEVEL_INFO, "Key created: %s", 0, m_str));
}
//
//
//===========================================================================
bool Key::operator==(const Key& rhs) const {

	newTRACE((LOG_LEVEL_INFO, "Key::operator==(%s)", 0, rhs.c_str()));

	return (0 == strcmp(m_str, rhs.c_str()));
}
//
//
//===========================================================================
bool Key::valid() const {

	newTRACE((LOG_LEVEL_INFO, "Key::valid()", 0));


	try
	{
	    // Covert string to uuid
	    boost::lexical_cast<boost::uuids::uuid, std::string>(string(m_str));
	}
	catch (boost::bad_lexical_cast &)
	{
	    return false;
	}

	return true;
}
//
//
//===========================================================================
ostream& operator<<(ostream& os, const Key& key) {

	os << "<";
	for(int i = 0; i < Key::LENGTH + 1 + Key::FILLER; ++i)
		os << ((key.m_str[i] == 0) ? ' ' : key.m_str[i]);
	return os << ">";
}

///////////////////////////////////////////////////////////////////////////////////////

/* This is FAKE Key
//
// ctor
//===========================================================================
Key_V4::Key_V4(bool init) {

	newTRACE(("Key_V4::Key(%d)", 0, init));

	if(init)
		newKey();
	else
		ZeroMemory(m_str, sizeof(m_str));
}
//
// set from string
//===========================================================================
void Key_V4::set(const char* pc) {

	newTRACE(("Key_V4::set(%s)", 0, pc));

	if(pc && pc[0])
		strncpy(m_str, pc, LENGTH + 1);
	else
		ZeroMemory(m_str, sizeof(m_str));
}
//
//
//===========================================================================
void Key_V4::newKey() {
	
	newTRACE(("Key_V4::newKey()", 0));

	// create UUID
	UUID uuid;
	UuidCreate(&uuid);
	// convert to string, and copy string to fixed length char buffer m_str
	unsigned char* uuidString;
	UuidToString(&uuid, &uuidString);
	assert(strlen(reinterpret_cast<const char*>(uuidString)) == LENGTH);
	TRACE(("Key length: %d", 0, strlen(reinterpret_cast<const char*>(uuidString))));
	strncpy(m_str, reinterpret_cast<const char*>(uuidString), LENGTH + 1);
	RpcStringFree(&uuidString); // Rpc weirdness

	TRACE(("Key created: %s", 0, m_str));
}
//
//
//===========================================================================
bool Key_V4::operator==(const Key_V4& rhs) const {

	newTRACE(("Key_V4::operator==(%s)", 0, rhs.c_str()));

	return (0 == strcmp(m_str, rhs.c_str()));
}
//
//
//===========================================================================
bool Key_V4::valid() const {

	newTRACE(("Key_V4::valid()", 0));

	UUID uuid;
	unsigned char* puc = const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(m_str));
	return RPC_S_OK == UuidFromString(puc, &uuid);
}
//
//
//===========================================================================
ostream& operator<<(ostream& os, const Key_V4& key) {

	os << "<";
	for(int i = 0; i < Key_V4::LENGTH + 1 + Key_V4::FILLER; ++i)
		os << key.m_str[i] == 0 ? ' ' : key.m_str[i];
	return os << ">";
}
*/
