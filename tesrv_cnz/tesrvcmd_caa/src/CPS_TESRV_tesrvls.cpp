//      INCLUDE
//       CPS_TESRV_tesrvls.h
//
//      COPYRIGHT
//       Ericsson Utvecklings AB, Sweden 2007.
//       All rights reserved.
//
//       The Copyright to the computer program(s) herein
//       is the property of Ericsson Utvecklings AB, Sweden.
//       The program(s) may be used and/or copied only with
//       the written permission from Ericsson Utvecklings AB
//       or in accordance with the terms and conditions
//       stipulated in the agreement/contract under which the
//       program(s) have been supplied.
//
//      DESCRIPTION
//
//      ERROR HANDLING
//       General rule:
//       The error handling is specified for each method.
//       No methods initiate or send error reports unless
//       specified.
//
//      DOCUMENT NO
//       190 89-CAA 109 1375
//
//      AUTHOR
//       2007-03-01 EAB/FTE/DDM QSUJSIV
//
//	REVISION
//	 PA1  2007-03-01     QSUJSIV    First revision
//   PA2  2011-03-24     XQUYDAO    Update for APG43L Proj
//
//	LINKAGE
//

#include "CPS_TESRV_tesrvls.h"
#include "CPS_TESRV_errors.h"
#include "CPS_TESRV_utils.h"
#include "CPS_TESRV_trace.h"
#include "CPS_TESRV_common.h"

#include "acs_prc_api.h"
#include "pes_clhapi.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <iomanip>


using namespace std;
using namespace CPS_TESRV;

bool    CPS_TESRV_tesrvls::printHeader   = false;
string  CPS_TESRV_tesrvls::headerCpSide  = "";

CPS_TESRV_tesrvls::CPS_TESRV_tesrvls (int argc, char **argv) : m_argc(argc),
                                                               m_argv(argv),
                                                               m_cpName(""),
                                                               m_startTime(""),
                                                               m_stopTime(""),
                                                               m_startDate(""),
                                                               m_stopDate(""),
                                                               m_cpSide(""),
                                                               m_sysType(eOneCP),
                                                               m_printTime(false)
{

}

CPS_TESRV_tesrvls::~CPS_TESRV_tesrvls()
{

}

// Print usage
void CPS_TESRV_tesrvls::usage()
{
    if (m_sysType == eOneCP)
    {
        cout << "Usage: tesrvls [-s cp_side] [-a start_time] [-e start_date]" << endl;
        cout << "               [-b stop_time] [-f stop_date]" << endl;
        cout << endl;
        cout << "       tesrvls -d [-e start_date] [-f stop_date]" << endl;
        cout << endl;
    }
    else
    {
        cout << "Usage: tesrvls -cp cp_name [-s cp_side] [-a start_time] [-e start_date]" << endl;
        cout << "               [-b stop_time] [-f stop_date]" << endl;
        cout << endl;

        cout << "       tesrvls -d -cp cp_name [-e start_date] [-f stop_date]" << endl;
        cout << endl;
    }
}

// Parse the command
int CPS_TESRV_tesrvls::parse()
{
    newTRACE(("CPS_TESRV_tesrvls::parse()", 0));

    int opt(0);
    int result(TESRV_OK);

    int optionA(0);
    int optionB(0);
    int optionC(0);
    int optionD(0);
    int optionE(0);
    int optionF(0);
    int optionS(0);

    // Set opterr to 0 in order to avoid getopt printout diagnostic message
    opterr = 0;

    while ((opt = getopt(m_argc, m_argv, "c:a:b:e:f:s:d")) != -1)
    {
        switch (opt)
        {
        case 'c': // CP option
            // Check that option is "-cp"
            if (strcmp(m_argv[optind - 1], "-cp") != 0)
            {
                TRACE(("option is NOT -cp", 0));
                return TESRV_INCORRECT_USAGE;
            }

            if (m_sysType == eMultipleCP)
            {
                // Make sure it's not out of boundary
                if (optind >= m_argc)
                {
                    TRACE(("option -cp has NO argument", 0));
                    return TESRV_INCORRECT_USAGE;
                }

                // Marked option CP
                optionC++;

                // Get CP name
                m_cpName =  m_argv[optind];
            }
            else
            {
                // If One CP system then -cp option should not be used.
                return TESRV_ILLEGAL_OPTION_IN_THIS_SYSTEM;
            }

            break;

        case 'a': // Start time
            m_startTime = optarg;
            optionA++;

            break;

        case 'b': // Stop time
            m_stopTime = optarg;
            optionB++;

            break;

        case 'd': // list first & last log time
            optionD++;
            break;

        case 'e': // Start date
            m_startDate = optarg;
            optionE++;

            break;

        case 'f': // Stop date
            m_stopDate = optarg;
            optionF++;

            break;

        case 's': // CP Side (cpa or cpb)
            m_cpSide = optarg;
            // Fix HQ36048
            if ((m_cpSide != "cpa") && (m_cpSide != "cpb"))
            {
                return TESRV_ILLEGAL_CPSIDE;
            }
            optionS++;

            break;

        case '?':
        default:
            TRACE(("Unrecognized option", 0));
            return TESRV_INCORRECT_USAGE;
        } // switch
    } // while

    // Have redundant argument
    int num = (optionC) ? m_argc - 1 : m_argc;
    if (optind < num)
    {
        TRACE(("Redundant argument at index: %d", 0, optind));
        return TESRV_INCORRECT_USAGE;
    }

    if (optionA > 1 || optionB > 1 || optionC > 1 ||
        optionD > 1 || optionE > 1 || optionF > 1 || optionS > 1)
    {
        TRACE(("Repeated option", 0));
        return TESRV_INCORRECT_USAGE;
    }

    if (m_sysType == eMultipleCP && optionC == false)
    {
        TRACE(("Missing option -cp in multi CP System", 0));
        return TESRV_INCORRECT_USAGE;
    }

    // Case option -d
    if (optionD)
    {
        if (optionA || optionB || optionS)
        {
            // Option -d only goes with -e & -f
            TRACE(("Invalid options combined with option -d", 0));
            return TESRV_INCORRECT_USAGE;
        }
        m_printTime = true;
    }

    // Option -s is not allowed for single side CP.
    if ((optionS > 0) && (('b' == m_cpName[0]) || ('B' == m_cpName[0])))
       return TESRV_INCORRECT_USAGE;

    return result;
}

// Execute the command
int CPS_TESRV_tesrvls::execute()
{
    newTRACE(("CPS_TESRV_tesrvls::execute()", 0));

    int result(TESRV_OK);
    bool classic = false;
    Pes_clhapi api;

    // Set the system type
    result = CPS_TESRV_utils::getCPSysType(m_sysType, classic);

    if (result != TESRV_OK)
    {
        TRACE(("Failed to determine CP system type", 0));
        return result;
    }

    if (classic)
    {
        TRACE(("Not allowed to run on classic system", 0));
        result = TESRV_ILLEGAL_COMMAND_IN_THIS_SYSTEM;

        return result;
    }

    // Parse the info from the user input
    result = parse();

    if (result != TESRV_OK)
    {
        TRACE(("Failed to parse the command. Error code: %d", result));
        return result;
    }

    // Execute & print output
    CPS_TESRV_tesrvls::printHeader = false;
    if (m_cpSide == "")
    {
        string cpName = m_cpName;
        string startTime = m_startTime;
        string stopTime = m_stopTime;
        string startDate = m_startDate;
        string stopDate = m_stopDate;
        string cpSideAName = "cpa";

        // The CP side name must be empty for single side CP.
        if (('b' == m_cpName[0]) || ('B' == m_cpName[0]))
            cpSideAName = "";

        // For A Side
        if (m_printTime)
        {
            // Note: m_startDate, .... is changed after execution.
            result = api.readLog(m_cpName, cpSideAName, m_startDate, m_startTime, m_stopDate, m_stopTime);

            if (result == TESRV_OK)
            {
                CPS_TESRV_tesrvls::printHeader = true;
                CPS_TESRV_tesrvls::printFirstAndLastTraceLogTime("A", m_startDate, m_startTime, m_stopDate, m_stopTime);
            }
        }
        else
        {
            CPS_TESRV_tesrvls::printHeader = true;
            CPS_TESRV_tesrvls::headerCpSide = "A";
            // Note: m_startDate, .... is changed after execution.
            result = api.readLog(m_cpName, cpSideAName, m_startDate, m_startTime, m_stopDate, m_stopTime, &CPS_TESRV_tesrvls::readLogCallBack);
        }

        // Continue with side B on the dual sided CP only
        if (!((('b' == m_cpName[0]) || ('B' == m_cpName[0]))) && (result == TESRV_OK))
        {
            // For B Side
            if (m_printTime)
            {
                result = api.readLog(cpName, "B", startDate, startTime, stopDate, stopTime);

                if (result == TESRV_OK)
                {
                    CPS_TESRV_tesrvls::printFirstAndLastTraceLogTime("B", startDate, startTime, stopDate, stopTime);
                }
            }
            else
            {
                CPS_TESRV_tesrvls::printHeader = true;
                CPS_TESRV_tesrvls::headerCpSide = "B";
                result = api.readLog(cpName, "B", startDate, startTime, stopDate, stopTime, &CPS_TESRV_tesrvls::readLogCallBack);
            }
        }
    }
    else
    {
        result = api.readLog(m_cpName, m_cpSide, m_startDate, m_startTime, m_stopDate, m_stopTime, &CPS_TESRV_tesrvls::readLogCallBack);
    }

    if (result != TESRV_OK)
    {
        TRACE(("CLH API error: %d - %s", 0, result, api.getErrorText().c_str()));
    }

    return result;
}

void CPS_TESRV_tesrvls::readLogCallBack(const char* cptime,
                                        const char* aptime,
                                        size_t size,
                                        const char* evmsg)
{
    if (cptime == NULL || aptime == NULL || evmsg == NULL)
    {
        newTRACE(("CPS_TESRV_tesrvls::readLogCallBack: cptime or aptime or evmsg = NULL ", 0));
        return;
    }

    CPS_TESRV_tesrvls::printTraceLog(string(cptime), string(aptime), string(evmsg, size));
}

void CPS_TESRV_tesrvls::printTraceLog(string cpTime, string apTime, string traceMsg)
{
    if (CPS_TESRV_tesrvls::printHeader)
    {
        CPS_TESRV_tesrvls::printHeader = false;
        cout << "Trace log data from CP side " << CPS_TESRV_tesrvls::headerCpSide << " is" << endl << endl;
    }

    cout << cpTime << "  AP time: " << apTime << endl << endl;
    cout << traceMsg << endl;
}

// Print first and last log time (for option -d)
void CPS_TESRV_tesrvls::printFirstAndLastTraceLogTime(string cpSide, string firstLogDate, string firstLogTime,
                                                      string lastLogDate, string lastLogTime)
{
    // Save the iostream flags
    ios::fmtflags f(cout.flags());

    cout << setfill(' ');
    cout << std::left;

    if (CPS_TESRV_tesrvls::printHeader)
    {
        CPS_TESRV_tesrvls::printHeader = false;
        cout << setw(9) << "CP side"
             << setw(17) << "Start time"
             << setw(17) << "Stop time" << endl;
    }

    cout << setw(9) << cpSide
         << setw(8) << firstLogDate << "_" << setw(8) << firstLogTime
         << setw(8) << lastLogDate << "_" << setw(8) << lastLogTime << endl;

    // Restore the iostream flags
    cout.flags(f);
}

int main(int argc, char* argv[])
{
    initTRACE();
    newTRACE(("main() starts", 0));

    int result(TESRV_OK);

    //Check if command is invoked on AP1
    if (Common::AP1 != Common::getApNode())
    {
        cerr << CPS_TESRV_errors::getErrorText(TESRV_AP_NOT_SUPPORTED) << endl;
        return TESRV_AP_NOT_SUPPORTED;

    }

    // Check that we are running on the active node
    ACS_PRC_API prcApi;
    int nodeState = prcApi.askForNodeState();

    if (nodeState != ACS_PRC_NODE_ACTIVE)
    {
        cerr << CPS_TESRV_errors::getErrorText(TESRV_EXECUTE_NOT_ACTIVE) << endl;
        return TESRV_EXECUTE_NOT_ACTIVE;
    }

    CPS_TESRV_tesrvls tesrvls(argc, argv);

    result = tesrvls.execute();

    // Convert CLH API to tesrv API if needed
    result = CPS_TESRV_errors::convertApiToTesrvErrorCode(result);

    if (result != TESRV_OK)
    {
        if (result == TESRV_ILLEGAL_OPTION_IN_THIS_SYSTEM || result == TESRV_INCORRECT_USAGE)
        {
            cerr << CPS_TESRV_errors::getErrorText(result) << endl << endl;
            tesrvls.usage();
        }
        else
        {
            cerr << CPS_TESRV_errors::getErrorText(result) << endl;
        }
    }

    TRACE(("main() exit: %d", 0, result));

    return result;
}
