//      NAME
//       CPS_TESRV_tesrvtran.cpp
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
//	DOCUMENT NO
//	 190 89-CAA 109 1374   
//
//	AUTHOR
//	 2007-01-22    EAB/FTE/DDM    QSUJSIV
//
//	REVISION
//	 PA1  2007-01-22     QSUJSIV    First revision
//
//	LINKAGE
//
//  Source file: Z:\Gemini_CPS\tecp_cnz\tesrvadm_caa\src\CPS_TESRV_tesrvtran.cpp

#include "CPS_TESRV_tesrvtran.h"
#include "CPS_TESRV_errors.h"
#include "CPS_TESRV_trace.h"
#include "CPS_TESRV_common.h"

#include <iostream>
#include <unistd.h>
#include <string.h>

#include <boost/algorithm/string.hpp>
#include "ACS_CS_API.h"
#include "ACS_APGCC_CommonLib.h"
#include "acs_apgcc_paramhandling.h"
#include "acs_prc_api.h"

using namespace std;
using namespace CPS_TESRV;

// Class CPS_TESRV_tesrvtran 

/**
 *	CPS_TESRV_tesrvtran class constructor
 *
 *	@param[in] argc - number of arguments
 *	@param[in] argv - argument vector
 *	@return - N/A
 */
CPS_TESRV_tesrvtran::CPS_TESRV_tesrvtran (int argc, char **argv)
: m_argV(argv),
  m_argC(argc),
  m_startDate(""),
  m_stopDate(""),
  m_startTime(""),
  m_stopTime(""),
  m_cpName(""),
  m_destType(e_file),  
  m_sysType(eOneCP)
{
    memset(m_trace, 0, sizeof(m_trace));
}

/**
 *	CPS_TESRV_tesrvtran class desconstructor
 *	@return - N/A
 */
CPS_TESRV_tesrvtran::~CPS_TESRV_tesrvtran()
{

}

/**
 *	This function is used to parse the command's arguments
 *
 *	@return - error code
 */
int CPS_TESRV_tesrvtran::parse ()
{	
	bool optionC(false);
	int opt(0);
	string logTypes("");
	int caseA(0);
	int caseB(0);
	int caseC(0);
	int caseE(0);
	int caseF(0);
	int caseT(0);
	opterr = 0;

    // Command doesn't handle no option case
    if (m_argC == 1)
    {
        return TESRV_INCORRECT_USAGE;
    }
    
	while ((opt =  getopt(m_argC, m_argV, "t:c:a:b:e:f:")) != -1)
    {
        switch (opt)
        {
			case 'c':
				{
					// Check that option is "-cp"
					if (strcmp(m_argV[optind - 1], "-cp") != 0)
					{
						return TESRV_INCORRECT_USAGE;
					}

					if (m_sysType == eMultipleCP)
					{
						if (optind >= m_argC)
						{
							return TESRV_INCORRECT_USAGE;
						}

						// Marked option CP
						optionC = true;
						caseC++;
						// Get CP name
						m_cpName =  m_argV[optind];
					}
					else  //If One CP system then -cp option should not be used.
					{
						return TESRV_ILLEGAL_OPTION_IN_THIS_SYSTEM ;
					}
				}

				break;

            case 'a':       // Starttime 
                m_startTime = optarg;
                caseA++;

                break;

            case 'b':       // Stoptime
				m_stopTime = optarg;
				caseB++;

				break;

			case 'e':       // Startdate
				m_startDate = optarg;
				caseE++;

				break;

			case 'f':       // Stopdate
				m_stopDate = optarg;
				caseF++;

				break;

		    case 't':       // Transfer type (media or file)
                {
					string transType = optarg;
					CPS_TESRV_utils::toUpper(transType);					

					if (transType == "FILE")
                    {
                        m_destType = e_file;
                    }
                    else if (transType == "MEDIA")
                    {
                        m_destType = e_media;
                    }
                    else
                    {
                        return TESRV_ILLEGAL_TRANSTYPE;
                    }

                    caseT++;
                    break;
                }

			 case '?':
			 default:
				 return TESRV_INCORRECT_USAGE;
		} // switch
	} // while

    // Transfer type must be input
    if (caseT != 1)
    {
        return TESRV_INCORRECT_USAGE;
    }
    
    // User should not input option twice
	if ((caseA > 1) || (caseB > 1) || (caseC > 1) || (caseE > 1) || (caseF > 1) || (caseT > 1))
	{
		return TESRV_INCORRECT_USAGE;
	}

    // Needs to input -cp in multi cp system
	if (m_sysType == eMultipleCP && optionC == false)
	{
		return TESRV_INCORRECT_USAGE;
	}

    // Have redundant option
	if (m_sysType == eMultipleCP)
	{
		if (m_argC != optind + 1)
		{
			return TESRV_INCORRECT_USAGE;
		}
	}
	else
	{
		if (m_argC != optind)
		{   
			return TESRV_INCORRECT_USAGE;
		}
	}
	// TO DO ===================================================================

	CPTable cptable;

	temp_startDate = m_startDate;
	temp_stopDate = m_stopDate;
	temp_startTime = m_startTime;
	temp_stopTime = m_stopTime;
	temp_cpName = m_cpName;

	// Analyze CP name
	if(CPTable::isMultiCPSystem())
	{
	   // Multi CP system
	   if (temp_cpName.empty() == false)
	   {
	   // CP information
	   const string& cp = boost::to_lower_copy(temp_cpName);
	   CPTable::const_iterator iter = cptable.find(cp);
	   if (iter == cptable.end())
	   {
	    throw TESRV_CP_NOT_DEFINED;
	   }
	   }
	 }
	 else
	 {
	    if (temp_cpName.empty() == false)
	    {
	   // Convert from error code 100 to 1 as design rule
	      throw TESRV_EXECUTE_ERROR;
	    }
          }

	Period period(temp_startDate,temp_startTime,temp_stopDate,temp_stopTime);

	// Check if there is another instance of "tesrvtran -f file" is running
	if(m_destType == e_file)
	    if(CPS_TESRV_utils::multipleTesrvtranFile())
	        return TESRV_FILE_TRANSFER_ONGOING;

	// Check if there is another instance of "tesrvtran -f media" is running
    if(m_destType == e_media)
        if(CPS_TESRV_utils::multipleTesrvtranMedia())
            return TESRV_MEDIA_TRANSFER_ONGOING;

    return 0;
}

/**
 *	This function is used to print out the command's syntax
 *
 *	@return - N/A
 */
void CPS_TESRV_tesrvtran::usage ()
{
    if (m_sysType == eOneCP)
    {       
        cout << "Usage: tesrvtran -t media [-a start_time] [-e start_date]" << endl;
        cout << "                 [-b stop_time] [-f stop_date] " << endl;  // TESRV_CHANGE_1
        cout << endl;
        cout << "       tesrvtran -t file [-a start_time] [-e start_date]" << endl;
        cout << "                 [-b stop_time] [-f stop_date] " << endl;  // TESRV_CHANGE_1
        cout << endl;
    }
    else if(m_sysType == eMultipleCP)
    {
        // If Multiple CP system then -cp option is mandatory.       
        cout << "Usage: tesrvtran -cp cp_name -t media [-a start_time] [-e start_date]" << endl;
        cout << "                 [-b stop_time] [-f stop_date] " << endl;  // TESRV_CHANGE_1
        cout << endl;
        cout << "       tesrvtran -cp cp_name -t file [-a start_time] [-e start_date]" << endl;
        cout << "                 [-b stop_time] [-f stop_date] " << endl;  // TESRV_CHANGE_1
        cout << endl;
    }
}

/**
 *	This function is used to execute the command
 *
 *	@return - error code
 */
int CPS_TESRV_tesrvtran::execute ()
{
    newTRACE(("CPS_TESRV_tesrvtran::execute()", 0));

	int result(TESRV_OK);
	bool classic(false);

	result= CPS_TESRV_utils::getCPSysType(m_sysType, classic);

	if (result != TESRV_OK)
	{
		cout << CPS_TESRV_errors::getErrorText(result) << endl;
		return result;
	}

	if (classic)
    {
        result = TESRV_ILLEGAL_COMMAND_IN_THIS_SYSTEM;
        cout << CPS_TESRV_errors::getErrorText(result) << endl;

        return result;
    }
   try
   {
	// Parse the info from the user.
	result = parse();
   }   
   catch(CPS_TESRV_Errorcodes errorcode)
   {
	result = errorcode;
	cout << CPS_TESRV_errors::getErrorText(result) << endl << endl;
	return result;
   }
	if (result != TESRV_OK)
	{
		if (result == TESRV_INCORRECT_USAGE || result == TESRV_ILLEGAL_OPTION_IN_THIS_SYSTEM)
		{
			cout << CPS_TESRV_errors::getErrorText(result) << endl << endl;
			usage();
			return result;
		}
		else
		{
			cout << CPS_TESRV_errors::getErrorText(result) << endl;
			return result;
		}
	}

#if TESRVTRAN_DEBUG
	cout << "CP name: " << m_cpName << endl;
	cout << "Start date: " << m_startDate << ", time:" << m_startTime << endl;
	cout << "Stop date: " << m_stopDate << ", time:" << m_stopTime << endl;
	cout << "Destination type: " << m_destType << endl;
#endif

	Pes_clhapi clhAPI;
	result = clhAPI.transferLogs(m_cpName, m_startDate, m_startTime, m_stopDate, m_stopTime, m_destType);

	// Convert CLH API to tesrv API if needed
	result = CPS_TESRV_errors::convertApiToTesrvErrorCode(result);

	if (result != TESRV_OK)
	{
	    TRACE(("CLH API error: %s", 0, clhAPI.getErrorText().c_str()));
		cout << CPS_TESRV_errors::getErrorText(result, clhAPI.getErrorText()) << endl;
		return result;
	}



	return TESRV_OK;
}


/**
 *	This function is main entry
 *
 *	@return - error code
 */
int main(int argc, char* argv[])
{
    initTRACE();
    newTRACE(("main() starts", 0));

	CPS_TESRV_tesrvtran tesrvtran(argc, argv);

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
	    cout << CPS_TESRV_errors::getErrorText(TESRV_EXECUTE_NOT_ACTIVE) << endl;
	    return TESRV_EXECUTE_NOT_ACTIVE;
	}

	int result = tesrvtran.execute();

	TRACE(("main() exit: %d", 0, result));


	return result;

}
