/*
NAME
   File_name: BUParamsOI.h

COPYRIGHT Ericsson AB, Sweden 2012. All rights reserved.

   The Copyright to the computer program(s) herein is the property of Ericsson
   Utvecklings AB, Sweden.
   The program(s) may be used and/or copied only with the written permission from
   Ericsson Utvecklings AB or in accordance with the terms and conditions
   stipulated in the agreement/contract under which the program(s) have been
   supplied.

DESCRIPTION
   Object Implementer (OI) for Non Classic & Cluster CPReload Params.

DOCUMENT NO
   190 89-CAA 109 1409

AUTHOR
   2012-04-20 by Quyen Dao

SEE ALSO


Revision history
----------------
2012-04-20 xquydao Created

*/

#ifndef BUPARAMSOI_H_
#define BUPARAMSOI_H_

#include "acs_apgcc_oihandler_V3.h"
#include "acs_apgcc_objectimplementerinterface_V3.h"
#include "CPS_BUSRV_Event_FD.h"
#include "BUParamsCommon.h"
#include <string>
#include "boost/thread.hpp"

class BUParamsOI : public acs_apgcc_objectimplementerinterface_V3
{
public:
    /** @brief constructor method
     *
     */
    BUParamsOI();

    /** @brief destructor method
     *
     */
    virtual ~BUParamsOI();

    /** @brief run method
     *
     *  This method is the main method of this class, it does the following things:
     *  - Register as OI for class BUParamsNonClassic
     *  - Run the infinite loop to handle the IMM event for this OI
     */
    void run();
    
    /** @brief stop method
     *
     *  This method is to stop the OI
     */
    void stop();

    /** @brief create method
     *
     *  This method will be called as a callback when an Object is created as instance of a Class CpVolume
     *  All input parameters are input provided by IMMSV Application and have to be used by the implementer to perform
     *  proper actions.
     *
     *  @param oiHandle: the object implementer handle. This is an Input Parameter provided by IMMSV Application.
     *
     *  @param ccbId: the ID for the Configuration Change Bundle in which the creation of the Object is contained.
     *  This is an Input Parameter provided by IMMSV Application.
     *
     *  @param className: the name of the class. When an object is created as instance of this class this method is
     *  called if the application has registered as class implementer. This is an Input Parameter provided by IMMSV Application.
     *
     *  @param parentName: the name of the parent object for the object now creating.
     *  This is an Input Parameter provided by IMMSV Application.
     *
     *  @param attr: a pointer to a null terminated array of ACS_APGCC_AttrValues element pointers each one containing
     *  the info about the attributes belonging to the now creating class.
     *  This is an Input Parameter provided by IMMSV Application.
     *
     *  @return ACS_CC_ReturnType On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE.
     */
    virtual ACS_CC_ReturnType create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr);

    /** @brief deleted method
     *
     *  This method will be called as a callback when deleting an CpReloadParam Object
     *  Object Implementer. All input parameters are input provided by IMMSV Application and have to be used by
     *  the implementer to perform proper actions.
     *
     *  @param oiHandle: the object implementer handle. This is an Input Parametr provided by IMMSV Application.
     *
     *  @param ccbId: the ID for the Configuration Change Bundle in which the deletion of the Object is contained.
     *  This is an Input Parameter provided by IMMSV Application.
     *
     *  @param objName: the Distinguished name of the object that has to be deleted.
     *  This is an Input Parameter provided by IMMSV Application.
     *
     *  @return ACS_CC_ReturnType On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE.
     *
     *  @remarks Remarks
     */
    virtual ACS_CC_ReturnType deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName);

    /** @brief modify method
     *
     *  This method will be called as a callback when modifying an CpReloadParam Object
     *  All input parameters are input provided by IMMSV Application and have to be used by the implementer to perform
     *  proper actions.
     *
     *  @param oiHandle: the object implementer handle. This is an Input Parameter provided by IMMSV Application.
     *
     *  @param ccbId: the ID for the Configuration Change Bundle in which the modify of the Object is contained.
     *  This is an Input Parameter provided by IMMSV Application.
     *
     *  @param objName: the Distinguished name of the object that has to be modified.
     *  This is an Input Parameter provided by IMMSV Application.
     *
     *  @param attrMods: a NULL terminated array of pointers to ACS_APGCC_AttrModification elements containing
     *  the information about the modify to perform. This is an Input Parametr provided by IMMSV Application.
     *
     *  @return ACS_CC_ReturnType On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE.
     *
     *  @remarks Remarks
     */
    virtual ACS_CC_ReturnType modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods);

    /** @brief complete method
     *
     *  This method will be called as a callback when a Configuration Change Bundle is complete and can be applied
     *  regarding an CpReloadParam Object
     *
     *  @param oiHandle: the object implementer handle. This is an Input Parameter provided by IMMSV Application.
     *
     *  @param ccbId: the ID for the Configuration Change Bundle in which the modify of the Object is contained.
     *  This is an Input Parameter provided by IMMSV Application.
     *
     *  @return ACS_CC_ReturnType On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE.
     *
     *  @remarks Remarks
     */
    virtual ACS_CC_ReturnType complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

    /** @brief abort method
     *
     *  This method will be called as a callback when a Configuration Change Bundle, regarding  an CpReloadParam Object,
     *  has aborted. This method is called only if at least one complete method failed.
     *
     *  @param oiHandle: the object implementer handle. This is an Input Parameter provided by IMMSV Application.
     *
     *  @param ccbId: the ID for the Configuration Change Bundle containing actions on Objects for which the Application
     *  registered as Object Implementer. This is an Input Parameter provided by IMMSV Application.
     *
     *  @return N/A
     *
     *  @remarks Remarks
     */
    virtual void abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

    /** @brief apply method
     *
     *  This method will be called as a callback when a Configuration Change Bundle, regarding  an CpReloadParam Object, is complete and can be applied.
     *  This method is called only if all the complete method have been successfully executed.
     *
     *  @param oiHandle: the object implementer handle. This is an Input Parameter provided by IMMSV Application.
     *
     *  @param ccbId: the ID for the Configuration Change Bundle containing actions on Objects for which the Application
     *  registered as Object Implementer. This is an Input Parameter provided by IMMSV Application.
     *
     *  @return N/A
     *
     *  @remarks Remarks
     */
    virtual void apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

    /** @brief updateRuntime method
     *
     *  This method will be called as a callback when modifying a runtime not-cached attribute of a configuration Object
     *  for which the Application has registered as Object Implementer.
     *  All input parameters are input provided by IMMSV Application and have to be used by the implementer
     *  to perform proper actions.
     *
     *  @param p_objName: the Distinguished name of the object that has to be modified.
     *                    This is an Input Parameter provided by IMMSV Application.
     *
     *  @param p_attrName: the name of attribute that has to be modified.
     *                     This is an Input Parameter provided by IMMSV Application.
     *
     *  @return ACS_CC_ReturnType On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE.
     *
     *  @remarks Remarks
     */
    virtual ACS_CC_ReturnType updateRuntime(const char* p_objName, const char** p_attrName);


    /**
     * @brief adminOperationCallback method
     * adminOperationCallback method: This method will be called as a callback to manage an administrative operation invoked, on the
     * CpReloadParam Object.
     *
     * @param  p_objName:   the distinguished name of the object for which the administrative operation
     * has to be managed.
     *
     * @param  oiHandle : input parameter,ACS_APGCC_OiHandle this value has to be used for returning the callback
     *  result to IMM.
     *
     * @param invocation: input parameter,the invocation id used to match the invocation of the callback with the invocation
     * of result function
     *
     * @param  p_objName: input parameter,the name of the object
     *
     * @param  operationId: input parameter, the administrative operation identifier
     *
     * @param paramList: a null terminated array of pointers to operation params elements. each element of the list
     * is a pointer toACS_APGCC_AdminOperationParamType element holding the params provided to the Administretive operation..
     *
     * @return N/A
     */
    virtual void adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType** paramList);

    // Wait until BUParamsOI is started
    // This function must be called by thread different than the Thread of BUParamsOI
    void waitUntilRunning(void);

private:
    /**
     * @brief validateFFR method
     * validateFFR method: This method is used for validating the input First File Range from OM corresponding to the cp type.
     *
     * @param  ffr:   the validating First File Range
     *
     * @param  cpType: the type of CP (SINGLE, CLUSTER, SPX1, SPX2)
     *
     * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
     */
    ACS_CC_ReturnType validateFFR(uint32_t ffr, BUParamsCommon::IMM_CP_TYPE cpType = BUParamsCommon::CP_SINGLE);

    /**
     * @brief validateSFR method
     * validateSFR method: This method is used for validating the input Second File Range from OM corresponding to the cp type.
     *
     * @param  sfr:   the validating First File Range
     *
     * @param  cpType: the type of CP (SINGLE, CLUSTER, SPX1, SPX2)
     *
     * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
     */
    ACS_CC_ReturnType validateSFR(uint32_t sfr, BUParamsCommon::IMM_CP_TYPE cpType = BUParamsCommon::CP_SINGLE);

    /**
     * @brief validateSupTime method
     * validateSupTime method: This method is used for validating the input Supervision Time from OM
     *
     * @param  supTime: the validating Supervision Time
     *
     * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
     */
    ACS_CC_ReturnType validateSupTime(uint32_t supTime);

private:
    bool	m_running;
    // OI Handler
    acs_apgcc_oihandler_V3 m_oiHandler;

    // Stop Event
    CPS_BUSRV_Event_FD m_stopFD;

    // BUParam, cpType & update flag using for private file update
    struct BUParamUpdateInfo
    {
        BUParamsCommon::BUParam buParam;
        BUParamsCommon::IMM_CP_TYPE cpType;
        int updateFlag;

        void reset()
        {
           buParam.ffr     = 0;
           buParam.sfr     = 0;
           buParam.supTime = 0;
           updateFlag      = 0;
           cpType          = BUParamsCommon::CP_NONE;
        };
    } m_updateInfo;

    // For condition variable
    boost::mutex    			m_mutex;
    boost::condition_variable 	m_condition;

};

#endif /* BUPARAMSOI_H_ */
