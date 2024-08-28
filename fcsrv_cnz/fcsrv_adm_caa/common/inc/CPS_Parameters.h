//*****************************************************************************
// 
// .NAME
//     CPS_Parameters - Parameter handling.
//
// .INCLUDE CPS_Parameters.h
//
// .COPYRIGHT
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 2011.
//  All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson Utvecklings AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.
//
// .DESCRIPTION 
//
//  Handling CPS parameters for FCSRV.
//
//
// DOCUMENT NO
// 190 89-CAA 109 0171
//
// AUTHOR
// 2012-08-28 by DEK/XNGUDAN
//
// SEE ALSO
// -
//
// Revision history
// ----------------
// 2012-08-28 XNGUDAN Create based on CPS_Parameters for BUSRV
//
//*****************************************************************************
#ifndef CPS_PARAMETERS_H
#define CPS_PARAMETERS_H

#include "acs_apgcc_paramhandling.h"
#include <string>

class CPS_Parameters
{
public:

	// TODO: check...
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
