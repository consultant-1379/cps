//	NAME
//	 CPS_TESRV_tesrvls.h
//
//	COPYRIGHT Ericsson Utvecklings AB, Sweden 2007.
//	 All rights reserved.
//	 The Copyright to the computer program(s) herein
//	  is the property of Ericsson Utvecklings AB, Sweden.
//	 The program(s) may be used and/or copied only with
//	 the written permission from Ericsson Utvecklings AB
//	 or in accordance with the terms and conditions
//	 stipulated in the agreement/contract under which the
//	 program(s) have been supplied.
//
//	DESCRIPTION
//
//	ERROR HANDLING
//	 General rule:
//	 The error handling is specified for each method.
//	 No methods initiate or send error reports unless
//	 specified.
//
//	DOCUMENT NO
//	 190 89-CAA 109 1375
//
//	AUTHOR
//	 2002-05-28 EAB/FTE/DDM QSUJSIV
//
//	REVISION
//	 PA1 2007-01-28     QSUJSIV  First revision
//	 PA2 2002-08-10     QSUJSIV
//   PA3 2011-03-24     XQUYDAO  Update for APG43L proj
//
//	LINKAGE
//
//	SEE ALSO


#ifndef _CPS_TESRV_TESRVLS_H_
#define _CPS_TESRV_TESRVLS_H_

#include "CPS_TESRV_utils.h"
#include <string>

class CPS_TESRV_tesrvls
{
public:

    // Constructor for the tesrvls command
    CPS_TESRV_tesrvls(int argc, char **argv);

    //## Destructor (generated)
    ~CPS_TESRV_tesrvls();

    //	Prints correct usage.
    void usage();

    //	Executes the tesrvls command.
    int execute();

    // Additional Public Declarations

protected:
    // Additional Protected Declarations

private:

    // Parses the supplied command line.
    int parse();

    // Callback for log messages
    static void readLogCallBack(const char* cptime,       // CP time, format is "YYYYMMDD_hhmmss_uuuuuu"
                         const char* aptime,       // AP time, format is "YYYYMMDD_hhmmss"
                         size_t size,              // Message size
                         const char* evmsg         // Log message
                        );

    // Print Trace Log
    static void printTraceLog(std::string cpTime, std::string apTime, std::string traceMsg);

    // Print first and last log time (for option -d)
    static void printFirstAndLastTraceLogTime(std::string cpSide, std::string firstLogDate, std::string firstLogTime,
                                              std::string lastLogDate, std::string lastLogTime);

private:
    int    m_argc;
    char** m_argv;
    std::string m_cpName;
    std::string m_startTime;
    std::string m_stopTime;
    std::string m_startDate;
    std::string m_stopDate;
    std::string m_cpSide;
    eSystemType m_sysType;
    bool m_printTime;

    static bool printHeader;
    static std::string headerCpSide;
};

// Class CPS_TESRV_tesrvls 

#endif
