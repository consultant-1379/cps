/*
 * CPS_BUSRV_API_Cbc_R1.h
 *
 *  Created on: May 27, 2012
 *      Author: xdtthng
 */

#ifndef CPS_BUSRV_API_SBC_R1_H_
#define CPS_BUSRV_API_SBC_R1_H_

#include "boost/shared_ptr.hpp"

class CPS_BUSRV_API_Sbc_Impl;

 //----------------------------------------------------------------------------
 class CPS_BUSRV_API_Sbc_R1
 {
 public:

    CPS_BUSRV_API_Sbc_R1();
    virtual ~CPS_BUSRV_API_Sbc_R1();

    // Result 
    //--------
	enum Result {
		RC_SUCCESS,					// Request was executed
		RC_NOCONTACT,				// No contact with server
		RC_FILE_NOT_FOUND,			// Requested file not found
		RC_INTERNAL_ERROR,			// Error due to DSD, FMS, Linux, ...
		RC_FILE_OUT_OF_RANGE,		// File out of Cluster Reload Range
		RC_FUNCTION_NOT_SUPPORTED	// Interface is with not suported APZ Profiles
    };

    // Interface methods
    // -----------------
	Result setSbcReloadFile(int);	// Aligns all filepointers to the given file number
	Result sbcFileExists(int);		// Verifies if  the given file number exists


 private:

    boost::shared_ptr<CPS_BUSRV_API_Sbc_Impl> m_impl;

};


#endif /* CPS_BUSRV_API_SBC_R1_H_ */
