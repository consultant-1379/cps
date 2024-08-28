//******************************************************************************
// NAME
// CPS_Parameters.h
//
// COPYRIGHT Ericsson AB, Sweden 2012.
// All rights reserved.
//
// The Copyright to the computer program(s) herein 
// is the property of Ericsson AB, Sweden.
// The program(s) may be used and/or copied only with 
// the written permission from Ericsson AB or in 
// accordance with the terms and conditions stipulated in the 
// agreement/contract under which the program(s) have been 
// supplied.
//
// DESCRIPTION
// Handling of Permenant Exchange Parameters for BUSRV
//
// DOCUMENT NO
// 190 89-CAA 109 0387
//
// AUTHOR
// 2012-04-18 by DEK/XDTTHNG
//
// SEE ALSO
// -
//
// Revision history
// ----------------
//
//******************************************************************************


#ifndef CPS_PARAMETERS_H
#define CPS_PARAMETERS_H

#include "acs_apgcc_paramhandling.h"
#include <string>

class CPS_Parameters
{
public:

    enum PARAM_TYPE {
        DN_BACKUP_RELOAD,
        FTP_DUMP_ROOT,                       
        FTP_LOAD_ROOT,
        MAX_SIMULTANEOUS_LOAD,
        MAX_SIMULTANEOUS_DUMP,

        MIN_RELOAD_SUPERVISION_TIME,                       
        MAX_LOADING_SUPERVISION_TIME,
        MAX_DUMPING_SUPERVISION_TIME,
        PARAM_TYPE_END
    };

    CPS_Parameters() {}
    ~CPS_Parameters() {}
    
    int get(PARAM_TYPE param, int defaultValue);
    string get(PARAM_TYPE param, const char* defaultStr);
    
private:
    acs_apgcc_paramhandling m_phaObj;   
    static const char* s_Params[PARAM_TYPE_END];
    
};

#endif // CPS_PARAMETERS_H
