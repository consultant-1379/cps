/*
 * CPS_BUSRV_API_Sbc_R1.cpp
 *
 *  Created on: May 27, 2012
 *      Author: xdtthng
 */

#include "CPS_BUSRV_API_Sbc_R1.h"
#include "CPS_BUSRV_API_Sbc_Impl.h"

//----------------------------------------------------------------------------
CPS_BUSRV_API_Sbc_R1::CPS_BUSRV_API_Sbc_R1(): m_impl(new CPS_BUSRV_API_Sbc_Impl)
{
}

//----------------------------------------------------------------------------
CPS_BUSRV_API_Sbc_R1::~CPS_BUSRV_API_Sbc_R1()
{
}


//----------------------------------------------------------------------------
CPS_BUSRV_API_Sbc::Result CPS_BUSRV_API_Sbc_R1::setSbcReloadFile(int filenumber)
{
	return m_impl->setSbcReloadFile(filenumber);
}

//----------------------------------------------------------------------------
CPS_BUSRV_API_Sbc::Result CPS_BUSRV_API_Sbc_R1::sbcFileExists(int  filenumber)
{
	return m_impl->sbcFileExists(filenumber);
}

