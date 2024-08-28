//	NAME
//	 CPS_TESRV_error.h
//
//      COPYRIGHT
//       Ericsson Utvecklings AB, Sweden 2007.
//        All rights reserved.
//
//        The Copyright to the computer program(s) herein
//        is the property of Ericsson Utvecklings AB, Sweden.
//        The program(s) may be used and/or copied only with
//        the written permission from Ericsson Utvecklings AB
//        or in accordance with the terms and conditions
//        stipulated in the agreement/contract under which the
//        program(s) have been supplied.
//
//      DESCRIPTION
//
//      ERROR HANDLING
//       General rule:
//       The error handling is specified for each method.
//       No methods initiate or send error reports unless
//      specified.
//
//      DOCUMENT NO
//       190 89-CAA 109 1374
//
//	AUTHOR
//	 2007-01-22 EAB/FTE/DDM QSUJSIV
//
//	REVISION
//	 PA1  2007-01-22     QSUJSIV    First revision
//   PA2  2011-03-25     XQUYDAO    Updated for APG43L proj
//
//	LINKAGE
//


#ifndef _CPS_TESRV_ERRORS_H_
#define _CPS_TESRV_ERRORS_H_

#include <string>

typedef enum
{
    // Everything worked fine.
    TESRV_OK = 0,

    // Error when executing (General fault)
    TESRV_EXECUTE_ERROR = 1,

    // Not right specified command...
    TESRV_INCORRECT_USAGE = 2,

    // Error inside the command.
    // TESRV_ILLEGAL_OPTION = 3,

    // Command not executed on active node.
    TESRV_EXECUTE_NOT_ACTIVE = 4,

    // Can not open the specified file.
    //TESRV_FILE_ACCESS_ERROR = 6,

    // Command server is busy
    //TESRV_COMMAND_BUSY = 8,

    // CLH server is down or closing down
    //TESRV_SERVER_DOWN = 9,

    // Start time greater than stop time.
    TESRV_STARTTIME_GREATER_STOPTIME = 19,

    // Illegal value for date
    TESRV_ILLEGAL_DATE = 20,

    // Illegal value for time
    TESRV_ILLEGAL_TIME = 21,

    // Illegal value for CP side
    TESRV_ILLEGAL_CPSIDE = 22,

    // CP side not allowed
    TESRV_NOTALLOWED_CPSIDE = 23,

    // Illegal transfer type
    TESRV_ILLEGAL_TRANSTYPE = 24,

    // Illegal transfer queue name.
    TESRV_ILLEGAL_TRANSQUEUE = 25,

    // Disk is full where the .zip file will be temporary stored.
    //TESRV_DISK_FULL = 31,

    // Problems to send the file through OHI...
    TESRV_FTP_PROBLEM = 32,

    // Problems to back up information to a DVD tape.
    TESRV_BACKUP_PROBLEM = 33,

    // Problem in the command pkzipc.
    //TESRV_PKZIP_PROBLEM = 34,

    // No files to compress. No .zip file is created.
    TESRV_NO_FILES = 35,

    // Could not find the command pkzipc. Probably there is no path to the command.
    //TESRV_NOT_FIND_PKZIP = 36,

    // The given transfer queue is not defined.
    //TESRV_NO_TRANSFERQ = 37,

    // ACS_EMF_Server doesn't respond
    //TESRV_EMF_NOT_RESPONDING = 38,

    //No media has been inserted in drive
    //TESRV_NO_MEDIA_IN_DEVISE = 39,

    //The media is write protected
    //TESRV_MEDIA_WRITE_PROTECTED = 40,

    //The devise is busy
    //TESRV_DEVISE_BUSY = 41,

    // Parameter error
    TESRV_PARAMETER_ERROR = 100,

    // System error
    TESRV_SYSTEM_ERROR = 101,

    // Internal error
    TESRV_INTERNAL_ERROR = 102,

    // Illegal command in this system configuration
    TESRV_ILLEGAL_COMMAND_IN_THIS_SYSTEM = 115,

    // Illegal option in this system configuration
    TESRV_ILLEGAL_OPTION_IN_THIS_SYSTEM = 116,

    // Problem with communication over DSD
    //TESRV_DSD_PROBLEM = 117,

    // CP name not found
    TESRV_CP_NOT_DEFINED = 118,

    // Not supported for AP2
    TESRV_AP_NOT_SUPPORTED = 119,

    // Note: always insert error before TESRV_LAST_error
    // After insert, update the TESRV_LAST_ERROR

    TESRV_MEDIA_TRANSFER_ONGOING = 120,

    TESRV_FILE_TRANSFER_ONGOING = 121,

    // Last error
    TESRV_LAST_ERROR = TESRV_MEDIA_TRANSFER_ONGOING,

    // Unknown problem
    TESRV_UNKNOWN_PROBLEM = TESRV_LAST_ERROR + 1

} CPS_TESRV_Errorcodes;

class CPS_TESRV_errors
{
public:
    // Convert error code to message
    static std::string getErrorText(int errorCode, const std::string alterErrText = "");

    // Convert CLH API error code to TESRV error code
    static int convertApiToTesrvErrorCode(int errorCode);

private:
    CPS_TESRV_errors();
};
//  Class CPS_TESRV_Errorcodes 
#endif
