/*
 * COPYRIGHT Ericsson Utvecklings AB, Sweden 2012.
 * All rights reserved.
 *
 * The Copyright to the computer program(s) herein
 * is the property of Ericsson Utvecklings AB, Sweden.
 * The program(s) may be used and/or copied only with
 * the written permission from Ericsson Utvecklings AB or in
 * accordance with the terms and conditions stipulated in the
 * agreement/contract under which the program(s) have been
 * supplied.
 *
 * NAME
 *  ClassicOI.h
 *
 * DESCRIPTION
 *  OI handler for Classic class
 *
 * DOCUMENT NO
 *  190 89-CAA 109 1414
 *
 * AUTHOR
 *  XDT/DEK XNGUDAN
 *
 * === Revision history ==========================
 * 2012-04-06   XNGUDAN    Create
 * ===============================================
 *
 */

#ifndef CLASSICOI_H_
#define CLASSICOI_H_

#include "CPS_BUAP_RPF.h"
#include "CPS_BUAP_EventFD.h"
#include "acs_apgcc_oihandler_V3.h"
#include "acs_apgcc_objectimplementerinterface_V3.h"

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>


class ClassicOI : public acs_apgcc_objectimplementerinterface_V3
{
public:
    /** @brief constructor method
     *
     */
    ClassicOI();

    /** @brief destructor method
     *
     */
    virtual ~ClassicOI();

    /** @brief run method
     *
     */
    void run();

    /** @brief stop method
     *
     */
    void stop();


    bool isRunning() const;

    /**
     * Method for blocking the calling thread until ClassicOI finishes start up
     *
     */
    void waitUntilRunning();

    /**
     * Method for notifying the Classic OI thread to update runtime attributes
     *
     */
    void notifyToUpdateIsValidAttrs();


    /** @brief create method
     *
     *  This method will be called as a callback when an Object is created as instance of a Class ClassicOI
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
     *  @param parentname: the name of the parent object for the object now creating.
     *  This is an Input Parameter provided by IMMSV Application.
     *
     *  @param attr: a pointer to a null terminated array of ACS_APGCC_AttrValues element pointers each one containing
     *  the info about the attributes belonging to the now creating class.
     *  This is an Input Parameter provided by IMMSV Application.
     *
     *  @return ACS_CC_ReturnType On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE.
     */
    virtual ACS_CC_ReturnType create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId,
                                      const char *className, const char* parentName,
                                      ACS_APGCC_AttrValues **attr);

    /** @brief deleted method
     *
     *  This method will be called as a callback when deleting an StaticParams Object
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
    virtual ACS_CC_ReturnType deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId,
                                       const char *objName);

    /** @brief modify method
     *
     *  This method will be called as a callback when modifying an StaticParams Object
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
    virtual ACS_CC_ReturnType modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId,
                                      const char *objName, ACS_APGCC_AttrModification **attrMods);

    /** @brief complete method
     *
     *  This method will be called as a callback when a Configuration Change Bundle is complete and can be applied
     *  regarding  an StaticParams Object
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
     *  This method will be called as a callback when a Configuration Change Bundle, regarding  an StaticParams Object,
     *  has aborted. This method is called only if at least one complete method failed.
     *
     *  @param oiHandle: the object implementer handle. This is an Input Parameter provided by IMMSV Application.
     *
     *  @param ccbId: the ID for the Configuration Change Bundle containing actions on Objects for which the Application
     *  registered as Object Implementer. This is an Input Parameter provided by IMMSV Application.
     *
     *  @return ACS_CC_ReturnType On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE.
     *
     *  @remarks Remarks
     */
    virtual void abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

    /** @brief apply method
     *
     *  This method will be called as a callback when a Configuration Change Bundle, regarding  an StaticParams Object, is complete and can be applied.
     *  This method is called only if all the complete method have been successfully executed.
     *
     *  @param oiHandle: the object implementer handle. This is an Input Parameter provided by IMMSV Application.
     *
     *  @param ccbId: the ID for the Configuration Change Bundle containing actions on Objects for which the Application
     *  registered as Object Implementer. This is an Input Parameter provided by IMMSV Application.
     *
     *  @return ACS_CC_ReturnType On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE.
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
     * StaticParams object.
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
     * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
     */
    virtual void adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation,
                                         const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,
                                         ACS_APGCC_AdminOperationParamType** paramList);

    /** @brief getOIHandler method to get the instance of class acs_apgcc_oihandler_V3
     *
     *  @return acs_apgcc_oihandler_V3
     */
    const acs_apgcc_oihandler_V3 &getOIHandler() const;

    /**
     * @brief set the runtime's flag to update the corresponding attribute
     *
     */
    void setAttrFlag(unsigned char flag);

private:
    /**
     * @brief copy constructor method
     *        Declare to disallow copying
     *
     */
    ClassicOI(const ClassicOI &);

    /**
     * @brief Assignment operator method
     *        Declare to disallow copying
     *
     */
    ClassicOI& operator=(const ClassicOI &);


    /** @brief resetStatic method : set all the staticParams to default values
     *
     * @param: errorCode returns in case Failure
     *
     * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
     */
    ACS_CC_ReturnType resetStatic(int &errorCode);


    /**
     * @brief: synchronizeData method
     *   Synchronize data between local file reladmpar and IMM database
     */
    void synchronizeData();

    /**
     * @brief updateRuntimeAttr method
     */
    ACS_CC_ReturnType updateRuntimeAttr(const std::string& objectName,
                               const std::string& attrName, int value);


private:

    // OI Handler
    acs_apgcc_oihandler_V3      m_oiHandler;

    bool                        m_running;

    boost::mutex                m_mutex;        // For condition variable
    boost::condition_variable   m_condition;

    // Check if the attributes is being configured
    bool                        m_isConfig;
    // Flag to mark which attribute is updated
    unsigned char               m_Flag;

    // Pointer to RELADMPAR file object
    CPS_BUAP_RPF    *m_pRPF;

    // Variables to store the "isValid" value from local file
    int m_isStaticValid;
    int m_isDynamicValid;
    int m_isLAReloadValid;
    int m_isCmdLogRefValid;
    int m_isDelayedMValid;

    // Stop Event FD
    CPS_BUAP_EventFD            m_stopEvent;
    // Event used to notify updating isValid attribute
    CPS_BUAP_EventFD            m_updateEvent;

    // Critical Session
    static CPS_BUAP_CS s_CSession;

};


#endif /* CLASSICOI_H_ */
