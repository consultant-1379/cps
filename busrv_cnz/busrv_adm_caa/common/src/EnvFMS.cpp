/*
NAME
   File_name:EnvFMS.cpp

COPYRIGHT Ericsson Utvecklings AB, Sweden 2001. All rights reserved.

The Copyright to the computer program(s) herein is the property of Ericsson
Utvecklings AB, Sweden.
The program(s) may be used and/or copied only with the written permission from
Ericsson Utvecklings AB or in accordance with the terms and conditions
stipulated in the agreement/contract under which the program(s) have been
supplied.

.DESCRIPTION
   Encapsulates FMS calls. This is done for two purposes.
   1) Makes it easier to remove FMS.
   2) Allows us to skip making the FMS calls when running in "Local test" mode.

DOCUMENT NO
   190 89-CAA 109 0387

AUTHOR 
   2001-09-17 by U/Y/SF Anders Gillgren (qabgill, tag :ag)

SEE ALSO 
   -

Revision history
----------------
2001-09-17 Created
2002-10-20 uabmnst Add ACSTrace
2002-11-25 uablan  Change FMS lib from R2B to R3A
2007-         adapt to multiple cp system

*/


#include "EnvFMS.h"

#include "buap.h"
#include "Config.h"
#include "CodeException.h"
#include "BUPFunx.h"

#include "fms_cpf_file.h"
#include "fms_cpf_fileiterator.h"
#include "CPS_BUSRV_Trace.h"

#include <assert.h>

//
// 
//
void EnvFMS::rename(SBCId sbcId, const char* src, const char* dest, CPID cpId) {

	newTRACE((LOG_LEVEL_INFO, "EnvFMS::rename(%d, %s, %s)", 0, sbcId.id(), src, dest));

#ifndef LOCAL_BUILD

	try {				
		string srcName = sbcId.dirName() + '-';
		string destName = srcName;
		srcName += src;
		destName += dest;
				
		bool isMultipleCP = Config::instance().isMultipleCP();

		if (isMultipleCP && cpId != 0xFFFF){
			char cpName[25];
			int res = BUPFunx::convertCPIdtoCPName(cpId, cpName);
			if (res != 0)
				THROW_XCODE_LOG("EnvFMS::rename: Convertion of CPId to CP Name failed.", CodeException::CP_APG43_CONFIG_ERROR);
				
			FMS_CPF_File file(srcName.c_str(), cpName);				
			file.reserve(FMS_CPF_Types::XR_XW_);				
			file.rename(destName.c_str());
		}
		else if (!isMultipleCP && cpId == 0xFFFF){
			FMS_CPF_File file(srcName.c_str());
			file.reserve(FMS_CPF_Types::XR_XW_);				
			file.rename(destName.c_str());
		}
		else if (isMultipleCP && cpId == 0xFFFF) {
			// CP = Multi CP system, APG = One CP system
			THROW_XCODE_LOG("EnvFMS::rename: CP = Multi CP system, APG = One CP system.", CodeException::CP_APG43_CONFIG_ERROR);
		}
		else if (!isMultipleCP && cpId != 0xFFFF) {
			// CP = One CP system, APG = Multi CP system
			THROW_XCODE_LOG("EnvFMS::rename: CP = Multi CP system, APG = One CP system.", CodeException::CP_APG43_CONFIG_ERROR);
		}
	}
	catch(FMS_CPF_Exception& x) {
		//if (isTRACEABLE()) {
		stringstream str;
		str << "EnvFMS::rename(), FMS Errorcode : " << x.errorCode() << ", Text: " << x.errorText() << ", Details: "
	    	<< x.detailInfo() << endl;
		TRACE((LOG_LEVEL_ERROR, "%s", 0, str.str().c_str()));
		EventReporter::instance().write(str.str().c_str());	
                throw;	
		//}
	}
	catch(CodeException& x) {
		newTRACE((LOG_LEVEL_ERROR, "Text: %s. Errorcode: %d", 0, x.what().c_str(), x.errcode()));
		throw;
	}
	
#endif
}

size_t EnvFMS::cleanDir(SBCId sbcId, CPID cpId)
{
	newTRACE((LOG_LEVEL_INFO, "EnvFMS::cleanDir(%d, %d)", 0, sbcId.id(), cpId));

	try {
		bool isMultipleCP = Config::instance().isMultipleCP();
		string name = sbcId.dirName();
		if (isMultipleCP && cpId != 0xFFFF){
			char cpName[25];
			int res = BUPFunx::convertCPIdtoCPName(cpId, cpName);
			if (res != 0)
				THROW_XCODE_LOG("EnvFMS::cleanDir: Convertion of CPId to CP Name failed.", CodeException::CP_APG43_CONFIG_ERROR);

			return delSubFile(name.c_str(), cpName);			
		}
		else if (!isMultipleCP && cpId == 0xFFFF){
			return delSubFile(name.c_str());
		}
		else if (isMultipleCP && cpId == 0xFFFF) {
			// CP = Multi CP system, APG = One CP system
			THROW_XCODE_LOG("EnvFMS::cleanDir: CP = One CP system, APG = Multi CP system.", CodeException::CP_APG43_CONFIG_ERROR);
		}
		else if (!isMultipleCP && cpId != 0xFFFF) {
			// CP = One CP system, APG = Multi CP system
			THROW_XCODE_LOG("EnvFMS::cleanDir: CP = Multi CP system, APG = One CP system.", CodeException::CP_APG43_CONFIG_ERROR);
		}
	}
	catch(FMS_CPF_Exception& x) {
		stringstream str;
		str << "EnvFMS::cleanDir(), FMS Errorcode : " << x.errorCode() << ", Text: " << x.errorText() << ", Details: "
	    	<< x.detailInfo() << endl;
		TRACE((LOG_LEVEL_ERROR, "%s", 0, str.str().c_str()));
		EventReporter::instance().write(str.str().c_str());		
		throw;
	}
	catch(CodeException& x) {
		newTRACE((LOG_LEVEL_ERROR, "Text: %s. Errorcode: %d", 0, x.what().c_str(), x.errcode()));
		throw;
	}

	return 0xffff; // It is not possible to reach here
}
//
// 
//
bool EnvFMS::fileExists(SBCId sbcId, const char* filename, CPID cpId) 
{
	newTRACE((LOG_LEVEL_INFO, "EnvFMS::fileExists(%d, %s, %d)", 0, sbcId.id(), filename, cpId));
	
#ifndef LOCAL_BUILD
	try {
		char name[255];
		sprintf(&name[0], "%s-%s", sbcId.dirName().c_str(), filename);
				
		bool isMultipleCP = Config::instance().isMultipleCP();

		if (isMultipleCP && cpId != 0xFFFF){
			char cpName[25];
			int res = BUPFunx::convertCPIdtoCPName(cpId, cpName);
			if (res != 0)
				THROW_XCODE_LOG("EnvFMS::fileExists: Convertion of CPId to CP Name failed.", CodeException::CP_APG43_CONFIG_ERROR);

			//TRACE(("FMS_CPF_File(%s, %s)", 0, name, cpName));
			FMS_CPF_File file(name, cpName);
			return file.exists();
		}
		else if (!isMultipleCP && cpId == 0xFFFF){
			FMS_CPF_File file(name);						
			return file.exists();
		}
		else if (isMultipleCP && cpId == 0xFFFF) {
			// CP = Multi CP system, APG = One CP system
			THROW_XCODE_LOG("EnvFMS::fileExists: CP = One CP system, APG = MCP system.", CodeException::CP_APG43_CONFIG_ERROR);
		}
		else { //if (!isMultipleCP && cpId != 0xFFFF) {
			// CP = One CP system, APG = Multi CP system
			THROW_XCODE_LOG("EnvFMS::fileExists: CP = Multi CP system, APG = One CP system.", CodeException::CP_APG43_CONFIG_ERROR);
		}
	}	
	catch(FMS_CPF_Exception& x) {
    	//if (isTRACEABLE()) {
		stringstream str;
		str << "EnvFMS::fileExists(), FMS Errorcode : " << x.errorCode() << ", Text: " << x.errorText() << ", Details: "
	    	<< x.detailInfo() << endl;
		TRACE((LOG_LEVEL_ERROR, "%s", 0, str.str().c_str()));
		EventReporter::instance().write(str.str().c_str());		
		//}
		throw;
	}
	catch(CodeException& x) {
		newTRACE((LOG_LEVEL_ERROR, "Text: %s. Errorcode: %d", 0, x.what().c_str(), x.errcode()));
		throw;
	}
#endif	

	return false;
}
//
// 
//
void EnvFMS::createFile(SBCId sbcId, const char* filename, CPID cpId) 
{

	newTRACE((LOG_LEVEL_INFO, "EnvFMS::createFile(%d, %s, %d)", 0, sbcId.id(), filename, cpId));

#ifndef LOCAL_BUILD
	try {
		char name[255];
		sprintf(&name[0], "%s-%s", sbcId.dirName().c_str(), filename);
				
		bool isMultipleCP = Config::instance().isMultipleCP();

		//if (Config::instance().isMultipleCP() && cpId != 0xFFFF){
		if (isMultipleCP && cpId != 0xFFFF){
			char cpName[25];
			int res = BUPFunx::convertCPIdtoCPName(cpId, cpName);
			if (res != 0)
				THROW_XCODE_LOG("EnvFMS::createFile: Convertion of CPId to CP Name failed.", CodeException::CP_APG43_CONFIG_ERROR);

			//TRACE(("FMS_CPF_File(%s, %s)", 0, name, cpName));
			FMS_CPF_File file(name, cpName);
			//file.create(FMS_CPF_Types::NONE_);
			//file.create(FMS_CPF_Types::R_W_);
			//file.unreserve();
			file.create();
		}
		//else if (!Config::instance().isMultipleCP() && cpId == 0xFFFF){
		else if (!isMultipleCP && cpId == 0xFFFF){
			//TRACE(("***** EnvFMS::createFile(); FMS_CPF_File(%s)", 0, name));
			FMS_CPF_File file(name);
			//TRACE(("return from FMS_CPF_File() ctor", 0));
			
			file.create();			
			//file.create(FMS_CPF_Types::R_W_);
			
			//TRACE(("return from FMS_CPF_File::create()", 0));
			//file.unreserve();
			//TRACE(("***** EnvFMS::createFile(); returns from FMS_CPF_File::unreserve()", 0));
		}
		//else if (Config::instance().isMultipleCP() && cpId == 0xFFFF) {
		else if (isMultipleCP && cpId == 0xFFFF) {
			// CP = Multi CP system, APG = One CP system
			THROW_XCODE_LOG("EnvFMS::createFile: CP = One CP system, APG = MCP system.", CodeException::CP_APG43_CONFIG_ERROR);
		}
		//else if (!Config::instance().isMultipleCP() && cpId != 0xFFFF) {
		else { //if (!isMultipleCP && cpId != 0xFFFF) {
			// CP = One CP system, APG = Multi CP system
			THROW_XCODE_LOG("EnvFMS::createFile: CP = Multi CP system, APG = One CP system.", CodeException::CP_APG43_CONFIG_ERROR);
		}
	}
	catch(FMS_CPF_Exception& x) {
    	//if (isTRACEABLE()) {
		stringstream str;
		str << "EnvFMS::createFile(), FMS Errorcode : " << x.errorCode() << ", Text: " << x.errorText() << ", Details: "
	    	<< x.detailInfo() << endl;
		TRACE((LOG_LEVEL_ERROR, "%s", 0, str.str().c_str()));
		EventReporter::instance().write(str.str().c_str());		
		//}
		throw;
	}
	catch(CodeException& x) {
		newTRACE((LOG_LEVEL_ERROR, "Text: %s. Errorcode: %d", 0, x.what().c_str(), x.errcode()));
		throw;
	}

	// To be remove upon completing FMS CPS integration
	// This is 09B and its base
	// Linux does something else
	#if 0
	//catch(FMS_CPF_Exception& x) {
		newTRACE(("Errorcode: %d, Text: %s, Details: %s", 0, x.errorCode(), x.errorText(), x.detailInfo().c_str()));
		switch(x.errorCode()) {
			case FMS_CPF_Exception::FILEISOPEN: // fall thru
			case FMS_CPF_Exception::FILENOTFOUND:
			case FMS_CPF_Exception::VOLUMENOTFOUND:
			case FMS_CPF_Exception::ACCESSERROR:
				THROW_XCODE("FMS call failed: " << x.errorText(), CodeException::FMS_CREATE_FILE_FAILED);
				break;
			case FMS_CPF_Exception::PHYSICALERROR:
				THROW_XCODE("FMS call failed: " << x.errorText(), CodeException::FMS_PHYSICAL_FILE_ERROR);
				break;
			default:
				THROW_XCODE("FMS call failed: " << x.errorText(), CodeException::FMS_UNKNOWN_ERROR);
				break;
		}
		
		// When there is an FMS fault, the calling site will handle the fault
		//throw;
	}
	catch(CodeException& x) {
		newTRACE(("Configuration error. Errorcode: %d", 0, x.errcode()));
		throw x;
	}
	#endif
	
#endif
}

size_t EnvFMS::delSubFile(const char* fileName, const char* cpName)
{
	newTRACE((LOG_LEVEL_INFO, "EnvFMS::delSubFile(%s, %s)", 0, fileName, cpName));

	size_t count = 0;
	FMS_CPF_FileIterator iter(fileName, true, true, cpName);
	FMS_CPF_FileIterator::FMS_CPF_FileData fData;
	
	//cout << "\nDeleting all of them, one by one" << endl;
	while(iter.getNext(fData)) {
		//cout << "Now deleting this file: " << fData.fileName << endl;

		try {
			TRACE((LOG_LEVEL_INFO, "EnvFMS::delSubFile attempts to delete <%s>", 0, fData.fileName.c_str()));
			FMS_CPF_File file(fData.fileName.c_str(), cpName);				
			file.deleteFile(false);
		}
		catch (...) {
			++count;
		}
	}
	//cout << "\nDone; all deleted" << endl;
	TRACE((LOG_LEVEL_INFO, "EnvFMS::delSubFile() returns count = %d", 0, count));
	return count;
}

bool EnvFMS::dirExists(SBCId sbcId, FMS_CPF_File& tmpFile, CPID cpId)
{
	newTRACE((LOG_LEVEL_INFO, "EnvFMS::dirExists(%d, FMS_CPF_File&, %d)", 0, sbcId.id(), cpId));
	try {	
		bool isMultipleCP = Config::instance().isMultipleCP();
		string name = sbcId.dirName();
		//TRACE(("file name is %s", 0, name.c_str()));
		if (isMultipleCP && cpId != 0xFFFF){
			char cpName[25];
			int res = BUPFunx::convertCPIdtoCPName(cpId, cpName);
			if (res != 0)
				THROW_XCODE_LOG("EnvFMS::dirExists: Convertion of CPId to CP Name failed.", CodeException::CP_APG43_CONFIG_ERROR);

			//TRACE(("FMS_CPF_File(%s, %s)", 0, name.c_str(), cpName));
			FMS_CPF_File file(name.c_str(), cpName);
			tmpFile = file;
			return tmpFile.exists();
			
		}
		else if (!isMultipleCP && cpId == 0xFFFF){
			FMS_CPF_File file(name.c_str());
			tmpFile = file;
			bool exist = tmpFile.exists();
			TRACE((LOG_LEVEL_INFO, "EnvFMS::dirExists() returns %d", 0, exist));
			return exist;
		}
		else if (isMultipleCP && cpId == 0xFFFF) {
			// CP = Multi CP system, APG = One CP system
			THROW_XCODE_LOG("EnvFMS::dirExists: CP = One CP system, APG = MCP system.", CodeException::CP_APG43_CONFIG_ERROR);
		}
		else { // !isMultipleCP && cpId != 0xFFFF
			// CP = One CP system, APG = Multi CP system
			THROW_XCODE_LOG("EnvFMS::dirExists: CP = Multi CP system, APG = One CP system.", CodeException::CP_APG43_CONFIG_ERROR);
		}
	}
	catch(FMS_CPF_Exception& x) {
    	//if (isTRACEABLE()) {
		stringstream str;
		str << "EnvFMS::dirExists(), FMS Errorcode : " << x.errorCode() << ", Text: " << x.errorText() << ", Details: "
	    	<< x.detailInfo() << endl;
		TRACE((LOG_LEVEL_ERROR, "%s", 0, str.str().c_str()));
		EventReporter::instance().write(str.str().c_str());		
		//}
		throw;
	}
	catch(CodeException& x) {
		newTRACE((LOG_LEVEL_ERROR, "Text: %s. Errorcode: %d", 0, x.what().c_str(), x.errcode()));
		throw;
	}
	
	return false;
}

bool EnvFMS::dirExists(SBCId sbcId, FMS_CPF_File& tmpFile, CPID cpId, const char* cpName)
{
	newTRACE((LOG_LEVEL_INFO, "EnvFMS::dirExists(%d, FMS_CPF_File&, %s)", 0, sbcId.id(), cpName));
	try {
		bool isMultipleCP = Config::instance().isMultipleCP();
		string name = sbcId.dirName();
		//TRACE(("file name is %s", 0, name.c_str()));
		if (isMultipleCP && cpId != 0xFFFF){
			FMS_CPF_File file(name.c_str(), cpName);
			tmpFile = file;
			return tmpFile.exists();

		}
		else if (!isMultipleCP && cpId == 0xFFFF){
			FMS_CPF_File file(name.c_str());
			tmpFile = file;
			bool exist = tmpFile.exists();
			TRACE((LOG_LEVEL_INFO, "EnvFMS::dirExists() returns %d", 0, exist));
			return exist;
		}
		else if (isMultipleCP && cpId == 0xFFFF) {
			// CP = Multi CP system, APG = One CP system
			THROW_XCODE_LOG("EnvFMS::dirExists: CP = One CP system, APG = MCP system.", CodeException::CP_APG43_CONFIG_ERROR);
		}
		else { // !isMultipleCP && cpId != 0xFFFF
			// CP = One CP system, APG = Multi CP system
			THROW_XCODE_LOG("EnvFMS::dirExists: CP = Multi CP system, APG = One CP system.", CodeException::CP_APG43_CONFIG_ERROR);
		}
	}
	catch(FMS_CPF_Exception& x) {
    	//if (isTRACEABLE()) {
		stringstream str;
		str << "EnvFMS::dirExists(), FMS Errorcode : " << x.errorCode() << ", Text: " << x.errorText() << ", Details: "
	    	<< x.detailInfo() << endl;
		TRACE((LOG_LEVEL_ERROR, "%s", 0, str.str().c_str()));
		EventReporter::instance().write(str.str().c_str());
		//}
		throw;
	}
	catch(CodeException& x) {
		newTRACE((LOG_LEVEL_ERROR, "Text: %s. Errorcode: %d", 0, x.what().c_str(), x.errcode()));
		throw;
	}

	return false;
}

bool EnvFMS::dirExists(SBCId sbcId, CPID cpId)
{
	newTRACE((LOG_LEVEL_INFO, "EnvFMS::dirExists(%d, %d)", 0, sbcId.id(), cpId));

	try {	
		bool isMultipleCP = Config::instance().isMultipleCP();
		string name = sbcId.dirName();
		if (isMultipleCP && cpId != 0xFFFF){
			char cpName[25];
			int res = BUPFunx::convertCPIdtoCPName(cpId, cpName);
			if (res != 0)
				THROW_XCODE_LOG("EnvFMS::dirExists: Convertion of CPId to CP Name failed.", CodeException::CP_APG43_CONFIG_ERROR);

			//TRACE(("FMS_CPF_File(%s, %s)", 0, name.c_str(), cpName));
			FMS_CPF_File file(name.c_str(), cpName);
			bool exist = file.exists();
			TRACE((LOG_LEVEL_INFO, "EnvFMS::dirExists() returns %d", 0, exist));
			return exist;
			
		}
		else if (!isMultipleCP && cpId == 0xFFFF){
			FMS_CPF_File file(name.c_str());
			bool exist = file.exists();
			TRACE((LOG_LEVEL_INFO, "EnvFMS::dirExists() returns %d", 0, exist));
			return exist;
		}
		else if (isMultipleCP && cpId == 0xFFFF) {
			// CP = Multi CP system, APG = One CP system
			THROW_XCODE_LOG("EnvFMS::dirExists: CP = One CP system, APG = MCP system.", CodeException::CP_APG43_CONFIG_ERROR);
		}
		else { // !isMultipleCP && cpId != 0xFFFF
			// CP = One CP system, APG = Multi CP system
			THROW_XCODE_LOG("EnvFMS::dirExists: CP = Multi CP system, APG = One CP system.", CodeException::CP_APG43_CONFIG_ERROR);
		}
	}
	catch(FMS_CPF_Exception& x) {
    	//if (isTRACEABLE()) {
		stringstream str;
		str << "EnvFMS::dirExists(), FMS Errorcode : " << x.errorCode() << ", Text: " << x.errorText() << ", Details: "
	    	<< x.detailInfo() << endl;
		TRACE((LOG_LEVEL_ERROR, "%s", 0, str.str().c_str()));
		EventReporter::instance().write(str.str().c_str());		
		//}
		throw;
	}
	catch(CodeException& x) {
		newTRACE((LOG_LEVEL_ERROR, "Text: %s. Errorcode: %d", 0, x.what().c_str(), x.errcode()));
		throw;
	}
	
	return false;
}
//
// 
//
void EnvFMS::deleteFile(SBCId sbcId, const char* filename, CPID cpId) {

	newTRACE((LOG_LEVEL_INFO, "EnvFMS::deleteFile(%d, %s)", 0, sbcId.id(), filename));

#ifndef LOCAL_BUILD

	try {		
		char name[255];
		sprintf(&name[0], "%s-%s", sbcId.dirName().c_str(), filename);
				
		bool isMultipleCP = Config::instance().isMultipleCP();

		if (isMultipleCP && cpId != 0xFFFF){
			char cpName[25];
			int res = BUPFunx::convertCPIdtoCPName(cpId, cpName);
			if (res != 0)
				THROW_XCODE_LOG("EnvFMS::deleteFile: Convertion of CPId to CP Name failed.", CodeException::CP_APG43_CONFIG_ERROR);
				
			FMS_CPF_File file(name, cpName);				
			file.deleteFile(false); // not recursive
		}
		else if (!isMultipleCP && cpId == 0xFFFF){
			FMS_CPF_File file(name);				
			file.deleteFile(false); // not recursive
		}
		else if (isMultipleCP && cpId == 0xFFFF) {
			// CP = Multi CP system, APG = One CP system
			THROW_XCODE_LOG("EnvFMS::deleteFile: CP = One CP system, APG = MCP system.", CodeException::CP_APG43_CONFIG_ERROR);
		}
		else if (!isMultipleCP && cpId != 0xFFFF) {
			// CP = MCP system, APG = Multi CP system
			THROW_XCODE_LOG("EnvFMS::deleteFile: CP = Multi CP system, APG = One CP system.", CodeException::CP_APG43_CONFIG_ERROR);
		}
	}
	catch(FMS_CPF_Exception& x) {
		//if (isTRACEABLE()) {
		stringstream str;
		str << "EnvFMS::deleteFile(), FMS Errorcode : " << x.errorCode() << ", Text: " << x.errorText() << ", Details: "
	    	<< x.detailInfo() << endl;
		TRACE((LOG_LEVEL_ERROR, "%s", 0, str.str().c_str()));
		//}
		switch(x.errorCode()) {
			case FMS_CPF_Exception::FILENOTFOUND:
				break;
			default:
				EventReporter::instance().write(str.str().c_str());		
				throw;
				//break;
		}
	}
	catch(CodeException& x) {
		newTRACE((LOG_LEVEL_ERROR, "Text: %s. Errorcode: %d", 0, x.what().c_str(), x.errcode()));
		throw;
	}
	
	// To be remove upon completing FMS CPS integration
	// This is 09B and its base
	// Linux does something else
	#if 0
	catch(FMS_CPF_Exception& x) {
		newTRACE(("Errorcode: %d, Text: %s, Details: %s", 0, x.errorCode(), x.errorText(), x.detailInfo().c_str()));
		switch(x.errorCode()) {
			case FMS_CPF_Exception::FILEISOPEN: // fall thru
			case FMS_CPF_Exception::FILENOTFOUND:
			case FMS_CPF_Exception::VOLUMENOTFOUND:
			case FMS_CPF_Exception::ACCESSERROR:
				THROW_XCODE("FMS call failed: " << x.errorText(), CodeException::FMS_DELETE_FILE_FAILED);
				break;
			case FMS_CPF_Exception::PHYSICALERROR:
				THROW_XCODE("FMS call failed: " << x.errorText(), CodeException::FMS_PHYSICAL_FILE_ERROR);
				break;
			default:
				THROW_XCODE("FMS call failed: " << x.errorText(), CodeException::FMS_UNKNOWN_ERROR);
				break;
		}
	}
	catch(CodeException& x) {
		newTRACE(("Configuration error. Errorcode: %d", 0, x.errcode()));
		throw x;
	}
	#endif
	
#endif
}

/////////////////////////////////////////////////////////////////////////////////
//
// 
//==================================================================================
void EnvFMS::createFile(SBCId sbcId, const char* filename, const Config::CLUSTER&) {

	newTRACE((LOG_LEVEL_INFO, "EnvFMS::createFile(%d, %s, CLUSTER)", 0, sbcId.id(), filename));

#ifndef LOCAL_BUILD
	try {
		char name[255];
		sprintf(name, "%s-%s", sbcId.dirName().c_str(), filename);
		
		//string str = sbcId.dirName();
		//str += "-";
		//str += filename;
		//const char *name = str.c_str();

		///////////////////////////////////////////////////////////////////
		//FMS_CPF_File file(name, "CLUSTER");
		FMS_CPF_File file(name, Config::CLUSTER_STR);
		file.create();
		//file.create(FMS_CPF_Types::NONE_);
		//file.unreserve();
		//////////////////////////////////////////////////////////////////

	}
	catch(FMS_CPF_Exception& x) {
		newTRACE((LOG_LEVEL_ERROR, "Errorcode: %d, Text: %s, Details: %s", 0, x.errorCode(), x.errorText(), x.detailInfo().c_str()));
		switch(x.errorCode()) {
			case FMS_CPF_Exception::FILEISOPEN: // fall thru
			case FMS_CPF_Exception::FILENOTFOUND:
			case FMS_CPF_Exception::VOLUMENOTFOUND:
			case FMS_CPF_Exception::ACCESSERROR:
				THROW_XCODE("FMS call failed: " << x.errorText(), CodeException::FMS_CREATE_FILE_FAILED);
				break;
			case FMS_CPF_Exception::PHYSICALERROR:
				THROW_XCODE("FMS call failed: " << x.errorText(), CodeException::FMS_PHYSICAL_FILE_ERROR);
				break;
			default:
				THROW_XCODE("FMS call failed: " << x.errorText(), CodeException::FMS_UNKNOWN_ERROR);
				break;
		}
	}
	catch(CodeException& x) {
		newTRACE((LOG_LEVEL_ERROR, "Configuration error. Errorcode: %d", 0, x.errcode()));
		throw x;
	}
#endif
}

//
//
//
void EnvFMS::rename(SBCId sbcId, const char* src, const char* dest, const Config::CLUSTER&) {

	newTRACE((LOG_LEVEL_INFO, "EnvFMS::rename(%d, %s, %s, CLUSTER&)", 0, sbcId.id(), src, dest));

#ifndef LOCAL_BUILD

	try {
		string srcName = sbcId.dirName() + '-';
		string destName = srcName;
		srcName += src;
		destName += dest;

		if (Config::instance().isMultipleCP()){
			FMS_CPF_File file(srcName.c_str(), Config::CLUSTER_STR);
			file.reserve(FMS_CPF_Types::XR_XW_);
			file.rename(destName.c_str());
		}
		else {
			// CP = MCP system, APG = One CP system
			THROW_XCODE_LOG("EnvFMS::dirExists: CP = MCP system (CLUSTER), APG = One CP system.", CodeException::CP_APG43_CONFIG_ERROR);
		}
	}
	catch(FMS_CPF_Exception& x) {
		//if (isTRACEABLE()) {
		stringstream str;
		str << "EnvFMS::rename(), FMS Errorcode : " << x.errorCode() << ", Text: " << x.errorText() << ", Details: "
	    	<< x.detailInfo() << endl;
		TRACE((LOG_LEVEL_ERROR, "%s", 0, str.str().c_str()));
		EventReporter::instance().write(str.str().c_str());
                throw;
		//}
	}
	catch(CodeException& x) {
		newTRACE((LOG_LEVEL_ERROR, "Text: %s. Errorcode: %d", 0, x.what().c_str(), x.errcode()));
		throw;
	}

#endif
}


bool EnvFMS::dirExists(SBCId sbcId, const Config::CLUSTER&)
{
	newTRACE((LOG_LEVEL_INFO, "EnvFMS::dirExists(%d, CLUSTER)", 0, sbcId.id()));

	try {
		string name = sbcId.dirName();
		if (Config::instance().isMultipleCP()) {
			//FMS_CPF_File file(name.c_str(), "CLUSTER");
			FMS_CPF_File file(name.c_str(), Config::CLUSTER_STR);
			bool exist = file.exists();
			TRACE((LOG_LEVEL_INFO, "EnvFMS::dirExists() returns %d", 0, exist));
			return exist;

		}
		else {
			// CP = MCP system, APG = One CP system
			THROW_XCODE_LOG("EnvFMS::dirExists: CP = MCP system (CLUSTER), APG = One CP system.", CodeException::CP_APG43_CONFIG_ERROR);
		}
	}
	catch(FMS_CPF_Exception& x) {
    	//if (isTRACEABLE()) {
		stringstream str;
		str << "EnvFMS::dirExists(), FMS Errorcode : " << x.errorCode() << ", Text: " << x.errorText() << ", Details: "
	    	<< x.detailInfo() << endl;
		TRACE((LOG_LEVEL_ERROR, "%s", 0, str.str().c_str()));
		EventReporter::instance().write(str.str().c_str());
		//}
		throw;
	}
	catch(CodeException& x) {
		newTRACE((LOG_LEVEL_ERROR, "Text: %s. Errorcode: %d", 0, x.what().c_str(), x.errcode()));
		throw;
	}

	return false;
}

bool EnvFMS::dirExists(SBCId sbcId, FMS_CPF_File& tmpFile, const Config::CLUSTER&)
{
	newTRACE((LOG_LEVEL_INFO, "EnvFMS::dirExists(%d, FMS_CPF_File&, CLUSTER)", 0, sbcId.id()));

	try {
		bool isMultipleCP = Config::instance().isMultipleCP();
		string name = sbcId.dirName();
		//TRACE(("file name is %s", 0, name.c_str()));
		if (isMultipleCP){
			//TRACE(("FMS_CPF_File(%s, %s)", 0, name.c_str(), cpName));
			//FMS_CPF_File file(name.c_str(), "CLUSTER");
			FMS_CPF_File file(name.c_str(), Config::CLUSTER_STR);
			tmpFile = file;
			bool exist = tmpFile.exists();
			TRACE((LOG_LEVEL_INFO, "EnvFMS::dirExists() returns %d", 0, exist));
			return exist;

		}
		else {
			// CP = MCP system, APG = One CP system
			THROW_XCODE_LOG("EnvFMS::dirExists: CP = MCP system, APG = One CP system.", CodeException::CP_APG43_CONFIG_ERROR);
		}
	}
	catch(FMS_CPF_Exception& x) {
    	//if (isTRACEABLE()) {
		stringstream str;
		str << "EnvFMS::dirExists(), FMS Errorcode : " << x.errorCode() << ", Text: " << x.errorText() << ", Details: "
	    	<< x.detailInfo() << endl;
		TRACE((LOG_LEVEL_ERROR, "%s", 0, str.str().c_str()));
		EventReporter::instance().write(str.str().c_str());
		//}
		throw;
	}
	catch(CodeException& x) {
		newTRACE((LOG_LEVEL_ERROR, "Text: %s. Errorcode: %d", 0, x.what().c_str(), x.errcode()));
		throw;
	}

	return false;
}

size_t EnvFMS::cleanDir(SBCId sbcId, const Config::CLUSTER&)
{
	newTRACE((LOG_LEVEL_INFO, "EnvFMS::cleanDir(%d, const CLUSTER&)", 0));
	try {
		string name = sbcId.dirName();
		if (Config::instance().isMultipleCP()) {
			return delSubFile(name.c_str(), Config::CLUSTER_STR);
		}
		else {
			// CP = MCP system, APG = One CP system
			THROW_XCODE_LOG("EnvFMS::dirExists: CP = MCP system (CLUSTER), APG = One CP system.", CodeException::CP_APG43_CONFIG_ERROR);
		}
	}
	catch(FMS_CPF_Exception& x) {
		stringstream str;
		str << "EnvFMS::cleanDir(), FMS Errorcode : " << x.errorCode() << ", Text: " << x.errorText() << ", Details: "
	    	<< x.detailInfo() << endl;
		TRACE((LOG_LEVEL_ERROR, "%s", 0, str.str().c_str()));
		EventReporter::instance().write(str.str().c_str());
		throw;
	}
	catch(CodeException& x) {
		newTRACE((LOG_LEVEL_ERROR, "Text: %s. Errorcode: %d", 0, x.what().c_str(), x.errcode()));
		throw;
	}

	return 0xffff; // It is not possible to reach here
}
bool EnvFMS::fileExists(SBCId sbcId, const char* filename, const Config::CLUSTER&)
{
	newTRACE((LOG_LEVEL_INFO, "EnvFMS::fileExists(%d, %s, CLUSTER&)", 0, sbcId.id(), filename));
	try {
		string name = sbcId.dirName();
		name += '-';
		name += filename;
		//TRACE(("file name is %s", 0, name.c_str()));
		if (Config::instance().isMultipleCP()){
			FMS_CPF_File file(name.c_str(), Config::CLUSTER_STR);
			bool exist = file.exists();
			TRACE((LOG_LEVEL_INFO, "EnvFMS::fileExists() returns %d", 0, exist));
			return exist;

		}
		else {
			// CP = MCP system, APG = One CP system
			THROW_XCODE_LOG("EnvFMS::fileExists: CP = MCP system, APG = One CP system.", CodeException::CP_APG43_CONFIG_ERROR);
		}
	}
	catch(FMS_CPF_Exception& x) {
    	//if (isTRACEABLE()) {
		stringstream str;
		str << "EnvFMS::fileExists(), FMS Errorcode : " << x.errorCode() << ", Text: " << x.errorText() << ", Details: "
	    	<< x.detailInfo() << endl;
		TRACE((LOG_LEVEL_ERROR, "%s", 0, str.str().c_str()));
		EventReporter::instance().write(str.str().c_str());
		//}
		throw;
	}
	catch(CodeException& x) {
		newTRACE((LOG_LEVEL_ERROR, "Text: %s. Errorcode: %d", 0, x.what().c_str(), x.errcode()));
		throw;
	}

	return false;
}
// XXX - not yet tested at all!
//
void EnvFMS::deleteFile(SBCId sbcId, const char* filename, const Config::CLUSTER&) {

	newTRACE((LOG_LEVEL_INFO, "EnvFMS::deleteFile(%d, %s, CLUSTER)", 0, sbcId.id(), filename));

#ifndef LOCAL_BUILD
	try {
		string name = sbcId.dirName();
		name += '-';
		name += filename;
		if (Config::instance().isMultipleCP()){
			FMS_CPF_File file(name.c_str(), Config::CLUSTER_STR);
			file.deleteFile(false); // not recursive
		}
		else {
			// CP = MCP system, APG = One CP system
			THROW_XCODE_LOG("EnvFMS::dirExists: CP = MCP system, APG = One CP system.", CodeException::CP_APG43_CONFIG_ERROR);
		}
	}
	catch(FMS_CPF_Exception& x) {
		newTRACE((LOG_LEVEL_ERROR, "Errorcode: %d, Text: %s, Details: %s", 0, x.errorCode(), x.errorText(), x.detailInfo().c_str()));
		switch(x.errorCode()) {
			case FMS_CPF_Exception::FILEISOPEN: // fall thru
			case FMS_CPF_Exception::FILENOTFOUND:
			case FMS_CPF_Exception::VOLUMENOTFOUND:
			case FMS_CPF_Exception::ACCESSERROR:
				THROW_XCODE("FMS call failed: " << x.errorText(), CodeException::FMS_DELETE_FILE_FAILED);
				break;
			case FMS_CPF_Exception::PHYSICALERROR:
				THROW_XCODE("FMS call failed: " << x.errorText(), CodeException::FMS_PHYSICAL_FILE_ERROR);
				break;
			default:
				THROW_XCODE("FMS call failed: " << x.errorText(), CodeException::FMS_UNKNOWN_ERROR);
				break;
		}
	}
	catch(CodeException& x) {
		newTRACE((LOG_LEVEL_ERROR, "Configuration error. Errorcode: %d", 0, x.errcode()));
		throw x;
	}
#endif
}



