/*
NAME
   File_name: BUParamsOM.h

COPYRIGHT Ericsson AB, Sweden 2012. All rights reserved.

   The Copyright to the computer program(s) herein is the property of Ericsson
   Utvecklings AB, Sweden.
   The program(s) may be used and/or copied only with the written permission from
   Ericsson Utvecklings AB or in accordance with the terms and conditions
   stipulated in the agreement/contract under which the program(s) have been
   supplied.

DESCRIPTION
   Object Manager (OM) for Non Classic & Cluster CPReload Params.

DOCUMENT NO
   190 89-CAA 109 1409

AUTHOR
   2012-04-20 by Quyen Dao

SEE ALSO


Revision history
----------------
2012-04-20 xquydao Created

*/

#ifndef BUPARAMSCOM_H_
#define BUPARAMSCOM_H_

#include "acs_apgcc_omhandler.h"
#include "ACS_CS_API.h"
#include "BUParamsCommon.h"
#include <string>

class BUParamsOM
{    
public:
    /** @brief constructor method
     *
     */
    BUParamsOM();

    /** @brief destructor method
     *
     */
    virtual ~BUParamsOM();

    /** @brief isInitialized method
     *
     *  Check if OM initialized
     *
     *  @return true   Initialized
     *          false  Not yet Initialized
     */
    bool isInitialized() const;

    /** @brief createAndSyncBUParamObject method
     *
     *  Create CPReloadParam object(s) instance depending on the system type then
     *  sync the buparam data from busrv.ini to IMM
     *  - Classic: nothing
     *  - NCD: 1 NonClassic obj
     *  - Cluster: 2 NonClassic as SPX1/SPX2 obj, 1 Cluster obj
     *
     *  @return true   Success
     *          false  Failure
     */
    bool createAndSyncBUParamObject();

    /** @brief createObject method
     *
     *  Create CPReloadParam object instance with instance ID & class name
     *
     *  @return true   Success
     *          false  Failure
     */
    bool createObject(const std::string& instanceID, const std::string& className);

    /** @brief getBUParam method
     *
     *  Obtaining the BUParam corresponding to the CP type
     *
     *  @param param: param to be obtained
     *
     *  @param cpType: CP type
     *
     *  @return true   Success
     *          false  Failure
     */
    bool getBUParam(BUParamsCommon::BUParam &param, BUParamsCommon::IMM_CP_TYPE cpType);

    /** @brief setBUParam method
     *
     *  Settig the BUParam corresponding to the CP type
     *
     *  @param param: param to be set
     *
     *  @param cpType: CP type
     *
     *  @return true   Success
     *          false  Failure
     */
    bool setBUParam(const BUParamsCommon::BUParam &param, BUParamsCommon::IMM_CP_TYPE cpType);

    /*
    enum RC_BUP {
    	RC_SUCCESS = 0,
    	RC_CONFIG_ERROR,
    	RC_IMM_ERROR
    };
    */
    /** @brief getBUParam method
     *
     *  Obtaining the BUParam corresponding to the CP type
     *
     *  @param param: param to be obtained
     *
     *  @param cpType: CP type
     *
     *  @return RC_SUCCESS      Success
     *          RC_CONFIG_ERROR Failure due to configuration error
     *          RC_IMM_ERROR    Failure due to IMM
     */
    //RC_BUP getBUParam(BUParamsCommon::BUParam &param, CPID cpId=0xFFFF);
    
    /** @brief getLastError method
     *
     *  Get Last error code
     *
     *  @return int   Last error code
     */
    int getLastError();

    /** @brief getLastErrorText method
     *
     *  Get Last error text
     *
     *  @return string   Last error string
     */
    const std::string& getLastErrorText();

private:
    /** @brief OM initialization method
     *
     *  This method is used to initialize the OM, it must be called prior to any IMM operation
     *
     *  @return true   Success
     *          false  Failure
     */
    bool init();

    /** @brief finalize method
     *
     *  Finalize and clean up the OM
     *
     *  @return true   Success
     *          false  Failure
     */
    bool finalize();

private:
    // Initialized indication
    bool m_initialized;

    // OM Handler
    OmHandler m_omHandler;

    // Error code & string
    int m_lastErrorCode;
    std::string m_lastErrorText;
};

#endif /* BUPARAMSCOM_H_ */
