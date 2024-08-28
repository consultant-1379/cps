// SIO P1A
//*****************************************************************************
// 
// .NAME
//  	CPS_BUAP_BUFile - Interface to read AXE reload files
// .LIBRARY 3C++
// .PAGENAME CPS_BUAP_BUFile
// .HEADER  ACS  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE CPS_BUAP_BUFile.h

// .COPYRIGHT
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 1996, 2011.
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
// 	Interface to read AXE reload files. The file object will
//	manage that the correct physical file is chosen, reserved
//	from CPF.

// .ERROR HANDLING
//
//	General rule:
//	The error handling is specified for each method.
//
//	No methods initiate or send error reports unless specified.

// DOCUMENT NO
//	  CXA 110 4958

// AUTHOR 
// 	  1997-05-23   uabfln   Created
//    2010-12-15   xchihoa  Port to Linux for APG43L
//    2011-08-08   xquydao  Updated for APG43L
//******************************************************************************

#ifndef CPS_BUAP_BUFILE_H
#define CPS_BUAP_BUFILE_H

#include <stdint.h>
#include "fms_cpf_file.h"
#include "CPS_BUAP_LoaderClient.h"

#include "boost/shared_ptr.hpp"

//******************************************************************************
// Constants
//******************************************************************************
enum CPS_BUAP_BUFILE_RETCODE
{
    CPS_BUAP_BUFILE_OK         = 0,
    CPS_BUAP_BUFILE_NO_FILE    = 1,
    CPS_BUAP_BUFILE_READ_ERROR = 2,
    CPS_BUAP_BUFILE_EOF        = 3,
    CPS_BUAP_BUFILE_MOVE_ERROR = 4,
    CPS_BUAP_BUFILE_NO_RESERVE = 5,
    CPS_BUAP_BUFILE_NOT_EOF    = 0
};

class CPS_BUAP_BUFile
{
public:
    // ctor
    CPS_BUAP_BUFile();

    // dtor
    ~CPS_BUAP_BUFile();

    // Calculate correct file number, reserve and attach
    // to the reload file.
    //
    // Return values: CPS_BUAP_BUFILE_OK
    //                CPS_BUAP_BUFILE_NO_FILE
    CPS_BUAP_BUFILE_RETCODE attachFile(uint16_t fileNumber);

    // Close current subfile and un-reserve the whole
    // reload file (all subfiles).
    //
    // Return values: CPS_BUAP_BUFILE_OK
    CPS_BUAP_BUFILE_RETCODE detachFile();

    // Opens a reload subfile and reserves the reload subfile.
    //
    // Return values: CPS_BUAP_BUFILE_OK
    //                CPS_BUAP_BUFILE_NO_FILE
    //                CPS_BUAP_BUFILE_READ_ERROR
    CPS_BUAP_BUFILE_RETCODE openSubfile(uint16_t subfile);

    // Unreserve & close a reload subfile
    //
    // Return values: CPS_BUAP_BUFILE_OK
    //                CPS_BUAP_BUFILE_NO_RESERVE
    //                CPS_BUAP_BUFILE_READ_ERROR
    CPS_BUAP_BUFILE_RETCODE closeSubfile();

    // Reads data from the file into the block pointed
    // to by the user.
    //
    // Return values: CPS_BUAP_BUFILE_OK
    //                CPS_BUAP_BUFILE_READ_ERROR
    //                CPS_BUAP_BUFILE_EOF
    CPS_BUAP_BUFILE_RETCODE readSubfile(char* buf, uint32_t bufSize, uint32_t& numOfBytes);

    // Moves the file pointer in the opened reload subfile.
    //
    // Return values: CPS_BUAP_BUFILE_OK
    //                CPS_BUAP_BUFILE_READ_ERROR
    CPS_BUAP_BUFILE_RETCODE moveSubfilePtr(uint32_t offset);

    // Checks if file pointer is at end of file.
    //
    // Return values: CPS_BUAP_BUFILE_NOT_EOF
    //                CPS_BUAP_BUFILE_EOF
    //                CPS_BUAP_BUFILE_MOVE_ERROR
    CPS_BUAP_BUFILE_RETCODE checkEndOfFile();

    // Obtain last error text info
    //
    // Return values: char*  Last error text
    const char* getLastErrorText();

    // Obtain the real attached file name
    //
    // Return values: char*  file name
    // Note: this method should only be called after the SUCCESSFUL attached
    const char* getAttachedFileName();

private:
    int m_fileFD;
    char m_fileName[256];
    char m_lastErrorText[768];
    FMS_CPF_File m_cpfSubfile;
    CPS_BUAP_LoaderClient m_buloader;
};

typedef boost::shared_ptr<CPS_BUAP_BUFile> CPS_BUAP_BUFile_Ptr;
#endif // CPS_BUAP_BUFILE_H


