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
 *  BUParamsOM.h
 *
 * DESCRIPTION
 *  Define OM for modifying backup & reload parameters
 *
 * DOCUMENT NO
 *  190 89-CAA 109 1414
 *
 * AUTHOR
 *  XDT/DEK XNGUDAN
 *
 * === Revision history ===
 * 2012-02-20   XNGUDAN    Create
 * 2012-03-08   XNGUDAN    Update after model review
 *
 */

#ifndef BUPARAMSOM_H_
#define BUPARAMSOM_H_


#include "acs_apgcc_omhandler.h"
#include "BUParamsCommon.h"

class BUParamsOM
{

public:
    /**
     * @brief: BUParamsOM constructor
     *
     */
    BUParamsOM();

    /**
     * @brief: Destructor
     *
     */
    virtual ~BUParamsOM();

    /**
     * @brief: initialize method.
     *  Create initial objects for Classic class
     *
     */
    bool createInitialObjects();

    /**
     * @brief: init method
     *  Initialize interaction between the calling object
     *  and the IMM.
     *
     */
    void init();

    /**
     * @brief: finalized method
     *  Finalize interaction with IMM
     *
     */
    void finalize();

    /**
     * @brief: isInitialized method
     *  Check if the OM is initialized.
     *
     */
    bool isInitialized() const;


    /**
     * @brief: applyReq method
     *  Apply all request associated at the transaction
     */
    bool applyReq(const std::string& transName);

    /**
     * @brief: updateStaticParams method
     *  Write static parameters to IMM
     */
    void updateStaticParams(const BUParamsCommon::StaticParam_& staticPar,
            const std::string& transName);

    /**
     * @brief: updateDynamicParams method
     *  Write dynamic parameters to IMM
     */
    void updateDynamicParams(const BUParamsCommon::DynamicParam_& dynamicPar,
            const std::string& transName);

    /**
     * @brief: updateCmdLogParams method
     *  Write Command Log Reference parameters to IMM
     */
    void updateCmdLogParams(const BUParamsCommon::CommmandLogReference_& cmdLogPar,
            const std::string& transName);

    /**
     * @brief: updateDMeasuresParams method
     *  Write Delayed Measures parameters to IMM
     */
    void updateDMeasuresParams(const BUParamsCommon::DelayedMeasure_& dmPar,
            const std::string& transName);

    /**
     * @brief: updateLastReloadParams method
     *  Write Last Auto Reload parameters to IMM
     */
    void updateLastReloadParams(const BUParamsCommon::LastAutoReload_& lastReloadPar,
            const std::string& transName);

private:

    /**
     * @brief: createClassicObject method
     *  Create object for Classic class
     *
     */
    bool createClassicObject();

    /**
     * @brief:
     *  Create object for Structure Parameters
     *
     */
    bool createStaticObject();
    bool createDynamicObject();
    bool createCmdLogRefObject();
    bool createDelayMeasuresObject();
    bool createLastAutoReloadObject();

    /**
     * @brief: modifyIntAttrs method
     *  Modify the integer attribute in the IMM
     *
     */
    bool modifyIntAttrs(const std::string& objectName, const std::string& attrName,
                        int value, bool type, const std::string& transName);

    /**
     * @brief: modifyStringAttrs method
     *  Modify the string attribute in the IMM
     *
     */
    bool modifyStringAttrs(const std::string& objectName, const std::string& attrName,
                           const std::string& value, const std::string& transName);

private:
    // State of StructParamOM
    bool m_initialized;

    // OmHandler to create/modify objects.
    OmHandler m_omHandler;
};


#endif /* BUPARAMSOM_H_ */
