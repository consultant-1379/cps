/*
 * utility.cpp
 *
 *  Created on: Mar 23, 2011
 *      Author: xtuangu
 */

#include "CPS_TESRV_utils.h"
#include "CPS_TESRV_errors.h"

#include "ACS_CS_API.h"
#include "ACS_APGCC_CommonLib.h"
#include "acs_apgcc_paramhandling.h"

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <sys/file.h>

using namespace std;

/**
 * This function is used to get CP system type
 *
 * @param[in, out]  str is the source string
 * @return      N/A
 */
void CPS_TESRV_utils::toUpper(string& str)
{
    string::iterator it;

    for (it = str.begin(); it != str.end(); ++it)
    {
        (*it) = (char) ::toupper(*it);
    }
}

/**
 * This function is used to get CP system type
 *
 * @param[out]	cpSystemType cp system type
 * @param[out]  classic      classic system?
 * @return 		Error code.
 */
int CPS_TESRV_utils::getCPSysType(eSystemType& cpSystemType, bool& classic)
{
    int result = TESRV_OK;
    bool isMultipleCPSystem;

    do
    {
        ACS_CS_API_NS::CS_API_Result cs_result =
                ACS_CS_API_NetworkElement::isMultipleCPSystem(isMultipleCPSystem);

        if (cs_result != ACS_CS_API_NS::Result_Success)
        {
            result = TESRV_EXECUTE_ERROR;
            break;
        }

        if (isMultipleCPSystem)
        {
            cpSystemType = eMultipleCP;
            classic = false;
            break;
        }

        // One cp system
        cpSystemType = eOneCP;

        // Check whether system is classic
        // Get the AxeFunctions DN
        OmHandler immHandle;
        ACS_CC_ReturnType ret;
        std::vector<std::string> dnList;

        ret = immHandle.Init();
        if (ret != ACS_CC_SUCCESS)
        {
            result = TESRV_EXECUTE_ERROR;
            break;
        }

        ret = immHandle.getClassInstances("AxeFunctions", dnList);

        if (ret != ACS_CC_SUCCESS)
        {
            result = TESRV_EXECUTE_ERROR;
            break;
        }

        // Obtain the apzProtocolType value
        acs_apgcc_paramhandling phaObject;
        const char *pAttrName = "apzProtocolType";
        int32_t apzProtocolTypeValue = 0;

        ret = phaObject.getParameter(dnList[0], pAttrName, &apzProtocolTypeValue);

        if (ret != ACS_CC_SUCCESS)
        {
            result = TESRV_EXECUTE_ERROR;
            break;
        }

        if ((apzProtocolTypeValue == 0) || (apzProtocolTypeValue == 1))
        {
            classic = true;
        }
        else
        {
            classic = false;
        }
    }
    while (false);

    return result;
}


/**
 * This function is used to determine if the tesrvtran -f file is already running or not
 *
 * @return      true: is running, false: not.
 */
bool CPS_TESRV_utils::multipleTesrvtranFile()
{

    bool rc = false;
    int pid_file = open("/var/tmp/tesrvtranfile.tmp", O_CREAT | O_RDWR, 0666);

    if(flock(pid_file, LOCK_EX | LOCK_NB))
        if(EWOULDBLOCK == errno)
            rc = true;

    return rc;
}


/**
 * This function is used to determine if the tesrvtran -f media is already running or not
 *
 * @return      true: is running, false: not.
 */
bool CPS_TESRV_utils::multipleTesrvtranMedia()
{

    bool rc = false;
    int pid_file = open("/var/tmp/tesrvtranmedia.tmp", O_CREAT | O_RDWR, 0666);

    if(flock(pid_file, LOCK_EX | LOCK_NB))
        if(EWOULDBLOCK == errno)
            rc = true;

    return rc;
}
