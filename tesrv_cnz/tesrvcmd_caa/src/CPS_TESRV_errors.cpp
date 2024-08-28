/*
 * CPS_TESRV_error.cpp
 *
 *  Created on: Mar 25, 2011
 *      Author: xquydao
 */

#include "CPS_TESRV_errors.h"

using namespace std;

struct ERROR_MSG
{
    CPS_TESRV_Errorcodes error;
    string errMsg;
};

const ERROR_MSG errorMsg[] =
{
    {TESRV_OK, ""},

    {TESRV_EXECUTE_ERROR, "Error when executing."},

    {TESRV_INCORRECT_USAGE, "Incorrect usage."},

    {TESRV_EXECUTE_NOT_ACTIVE, "This command must be executed in the active node."},

    {TESRV_STARTTIME_GREATER_STOPTIME, "Start time is greater than stop time."},

    {TESRV_ILLEGAL_DATE, "Illegal date value."},

    {TESRV_ILLEGAL_TIME, "Illegal time value."},

    {TESRV_ILLEGAL_CPSIDE, "Illegal CP side value."},

    {TESRV_NOTALLOWED_CPSIDE, "CP side is not allowed."},

    {TESRV_ILLEGAL_TRANSTYPE, "Illegal transfer type."},

    {TESRV_ILLEGAL_TRANSQUEUE, "Illegal transfer queue name."},

    {TESRV_FTP_PROBLEM, "Not able to transfer file."},

    {TESRV_BACKUP_PROBLEM, "Not able to transfer to media."},

    {TESRV_NO_FILES, "No file to compress."},

    {TESRV_ILLEGAL_COMMAND_IN_THIS_SYSTEM, "Illegal command in this system configuration."},

    {TESRV_ILLEGAL_OPTION_IN_THIS_SYSTEM, "Illegal option in this system configuration."},

    {TESRV_CP_NOT_DEFINED, "CP is not defined."},

    {TESRV_AP_NOT_SUPPORTED, "This command must be executed from AP1."},

    {TESRV_MEDIA_TRANSFER_ONGOING, "A media transfer is already ongoing.\nSee attribute resultOfOperation in MO ExternalMediaM for more details."},

    {TESRV_FILE_TRANSFER_ONGOING, "A file transfer is already ongoing."},

    // Note: MUST insert entry before TESRV_UNKNOWN_PROBLEM

    {TESRV_UNKNOWN_PROBLEM, "Unknown problem."}
};

/**
* This function is used to convert error code to error message
*
* @param[in] errorCode is tesrv error code
* @param[in] alternative error text
* @return:  error message
*/
string CPS_TESRV_errors::getErrorText(int errorCode, const string alterErrText)
{
    const ERROR_MSG* p = errorMsg;

    while (p)
    {
        if (p->error == CPS_TESRV_Errorcodes(errorCode))
        {
            // Need to print alternative error text for the following errors (quite abnormal case)
            if (errorCode == TESRV_FTP_PROBLEM || errorCode == TESRV_BACKUP_PROBLEM)
            {
                return alterErrText;
            }

            return p->errMsg;
        }

        ++p;
    }

    // Found nothing then return unknown problem
    return (--p)->errMsg;
}

/**
* This function is used to convert CLH API error code to TESRV error code
*
* @param[in] errorCode is CLH API error code
* @return:  TESRV error code
*/
int CPS_TESRV_errors::convertApiToTesrvErrorCode(int errorCode)
{
    switch (errorCode)
    {
    // Parameter error
    case TESRV_PARAMETER_ERROR: // 100
    // System error
    case TESRV_SYSTEM_ERROR: // 101
    // Internal error
    case TESRV_INTERNAL_ERROR: // 102
        return TESRV_EXECUTE_ERROR; // 1
    }

    return errorCode;
}
