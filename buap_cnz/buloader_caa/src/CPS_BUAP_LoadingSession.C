//*****************************************************************************
//
// NAME    CPS_BUAP_LoadingSession.C
//      
//
// COPYRIGHT Ericsson Utvecklings AB, Sweden 1996.
// All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson Utvecklings AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.
//
// DESCRIPTION 
//      Manages a single APZ loading session.
//
// DOCUMENT NO
//	CAA 109 0081
//
// AUTHOR 
// 	Wed Sep 24 12:31:03 1997 by UAB/I/GM UABFLN
//
// CHANGES 
//  990623  uablan - Changed function getFn to want file name
//                   without subfile.
//  990913  uabbir - Class CPS_BUAP_Loader_Exception replaced by
//                   CPS_BUAP_Exception
//  001017  qabgill - fixed daylight saving bug in supTimeExpired
//  050114  uablan  - Remove lock for reladmpar.
//  050623  uablan  - fixed daylight saving bug in supTimeExpired TR-HF82616
//
// SEE ALSO 
// 	-
//
//*****************************************************************************
#include    <windows.h>

#include	<stdio.h>
#include	<string>
#include    <time.h>

#include	"CPS_BUAP_LoadingSession.H"
#include	"CPS_BUAP_Exception.H"

#include	"CPS_BUAP_MSG.H"
#include	"CPS_BUAP_MSG_IDs.H" 
#include	"CPS_BUAP_Trace.H"
#include	"CPS_BUAP_Events.H"

using namespace std;
//
// Member function definitions:
// ============================

//*****************************************************************************
//	CPS_BUAP_LoadingSession()
//*****************************************************************************
CPS_BUAP_LoadingSession::CPS_BUAP_LoadingSession()
: parmgr("BUPARMGR",10 ,2),
APZtype(0)
{
	newTRACE(("CPS_BUAP_LoadingSession::CPS_BUAP_LoadingSession()", 0));
	
	// Clear the Last reload parameter entries
	parLAR.m_ManAuto	<< 0;
	parLAR.m_NCT		<< 0;
	parLAR.m_Omission	<< 0;
	parLAR.m_SUP		<< 0;
}  
//*****************************************************************************
//	~CPS_BUAP_LoadingSession()
//*****************************************************************************
CPS_BUAP_LoadingSession::~CPS_BUAP_LoadingSession()
{
	newTRACE(("CPS_BUAP_LoadingSession::~CPS_BUAP_LoadingSession()", 0));
	
	// when PARMGR_Client goes out of scope the connection will
	// disconnect.
}  
//*****************************************************************************
//	getFn()
//*****************************************************************************
void CPS_BUAP_LoadingSession::getFn(string& iname)
{
	newTRACE(("CPS_BUAP_LoadingSession::getFn(%s)", 0, iname.data()));
	
	if (iname != "RELFSW255")
	{ 
	  string fileNum = strpbrk(iname.data(), "0123456789");
	  parLAR.m_LRG = atoi(fileNum.data());
	  TRACE(("parLAR.m_LRG : %d ", 0, parLAR.m_LRG));
      return;
	}                     
	
	// Get reladmpar values
	readStaticParams();
	readDynamicParams();
	
	// Check if supervision time has expired, 
	// (will also set new expiring time) 
	CPS_BUAP_MSG_dynamic_parameters	updDynamic; // obj to store values
	if (supTimeExpired(updDynamic))
	{
		parDynamic.m_State << 1;	// Reset state
		TRACE(("State reset to 1", 0));
		// Reset counter values
		parDynamic.m_CTA	<< 0;	// This session still looks at this sessions
		parDynamic.m_NumFFR	<< 0;   // parameters, so we must reset them 
		parDynamic.m_NumSFR	<< 99;
		parDynamic.m_NCT	<< 0;
		parDynamic.m_LPF	<< 0;
		parDynamic.m_Omission << 0;
		updDynamic.m_CTA	<< 0;	// Next turns dynamic parameters
		updDynamic.m_NumFFR	<< 0;
		updDynamic.m_NumSFR	<< 99;
		updDynamic.m_NCT	<< 0;
		updDynamic.m_LPF	<< 0;
		updDynamic.m_Omission << 0;
	}
	
	//
	// Find current reload file
	// ------------------------
	int dumpNo = 0;
	
	TRACE(("Current dump state: %d", 0, parDynamic.m_State()));
	switch (parDynamic.m_State())
	{
	case 1:
		parLAR.m_LRGF << 0;   // Not last permissible file anymore 
		break;
	case 2:
		break;
	case 3:
		parDynamic.m_State << 4; // Always a 1-time stop
		break;
	case 4:
		dumpNo = parDynamic.m_NumFFR();
		break;
	case 5:
		dumpNo = parDynamic.m_NumSFR();
		break;
	case 6:
		dumpNo = parDynamic.m_LPF();
		parLAR.m_LRGF     << 1;
		break;
	default:
		break;
	}
	char buff[32];
	sprintf(buff, "RELFSW%d", dumpNo);
	iname = buff;
	TRACE(("Current dump name: %s", 0, iname.data()));
	
	//
	// Set values for latest reload
	// ----------------------------
	parLAR.m_ManAuto  << parStatic.m_ManAuto();
	parLAR.m_LRG	<< dumpNo;
	parLAR.m_NCT	<< parDynamic.m_NCT();
	parLAR.m_Omission << parDynamic.m_Omission();
	parLAR.m_SUP	<< parStatic.m_SUP();
	
	//
	// Calculate new values for next attempt
	// -------------------------------------
	
	switch (parDynamic.m_State())
	{
	case 1:
		// Reload RELFSW0
		// -----------------
		updDynamic.m_State << 2;
		
	case 2:
		// Command Log truncations
		// -----------------------
		if ((parStatic.m_ManAuto() == 1) ||
			(parStatic.m_NTAZ()    == 0) || 
			(parStatic.m_NTAZ()    == parDynamic.m_CTA()))
		{
			updDynamic.m_State << 3;
		}
		else
		{
			int n    = parDynamic.m_CTA();	// Truncation attempt
			updDynamic.m_CTA << ++n;
			int nct  = parDynamic.m_NCT();	// # of cmds truncated
			int ntcz = parStatic.m_NTCZ();	// # of cmds to truncate
			updDynamic.m_NCT << nct + ntcz;
			updDynamic.m_State << 2;
			break;				// Done for now
		}	      
		
	case 3:
		// Command Log omittion attempt
		// ----------------------------
		if ((parStatic.m_ManAuto() == 1) || 
			(parStatic.m_LOAZ()    == 0))
		{
			updDynamic.m_State << 4;	      
		}
		else
		{
			updDynamic.m_Omission << parStatic.m_LOAZ();
			updDynamic.m_State << 3;
			break;				// Done for now
		}
		
	case 4:
		updDynamic.m_Omission << 0;		// Reset omission state
		// Reload First File Range
		// -----------------------
		if ((parStatic.m_INCL1() == 0) || 
			(parDynamic.m_NumFFR() == parDynamic.m_NOld()))
		{
			updDynamic.m_State << 5;
		}
		else
		{
			int j = parDynamic.m_NumFFR();
			updDynamic.m_NumFFR << ++j;
			updDynamic.m_LPF << j;
			updDynamic.m_State << 4;
			break;				// Done for now
		}	      
		
	case 5:
		// Reload Second File Range
		// ------------------------
		if ((parStatic.m_INCL2() == 0)  || 
			(parDynamic.m_KOld() == 0 ) ||
			(parDynamic.m_NumSFR() == parDynamic.m_KOld()))
		{
			updDynamic.m_State << 6;
		}
		else
		{
			int k = parDynamic.m_NumSFR();
			updDynamic.m_NumSFR << ++k;
			updDynamic.m_LPF << k;
			updDynamic.m_State << 5;
			break;				// Done for now
		}	      
		
	case 6:
		// Last permissable file
		// ---------------------
		updDynamic.m_State << 6;
		break;
		
	default:
		break;
	}
	
	//
	// Write new dynamic values
	// ------------------------
	TRACE(("Writing new dynamic parameters\n State \t%d\n CTA \t%d\n NumFFR \t%d\n NumSFR \t%d\n NOld \t%d\n KOld \t%d\n NCT \t%d\n EXPcentury \t%d\n EXPyear \t%d\n EXPmonth \t%d\n EXPday \t%d\n EXPhour \t%d\n EXPminute \t%d\n LPF \t%d\n Omission \t%d\n"
		, 0,
		updDynamic.m_State(),
		updDynamic.m_CTA(),
		updDynamic.m_NumFFR(),
		updDynamic.m_NumSFR(),
		updDynamic.m_NOld(),
		updDynamic.m_KOld(),
		updDynamic.m_NCT(),
		updDynamic.m_EXPcentury(),
		updDynamic.m_EXPyear(),
		updDynamic.m_EXPmonth(),
		updDynamic.m_EXPday(),
		updDynamic.m_EXPhour(),
		updDynamic.m_EXPminute(),
		updDynamic.m_LPF(),
		updDynamic.m_Omission()
		));
	writeDynamicParams(updDynamic);
	
	return;
}  
//*****************************************************************************
//	reportCmdlog()
//*****************************************************************************
void CPS_BUAP_LoadingSession::reportCmdlog(R0Info r0)
{  
	newTRACE(("CPS_BUAP_LoadingSession::reportCmdlog()", 0));
	
	APZtype = r0.APZtype;
	
	parLAR.m_AssClogNum << r0.subfile;
	
	parLAR.m_OPSRScentury << ((*r0.rpsrs.year > 90)?19:20);
	parLAR.m_OPSRSyear    << *r0.rpsrs.year;
	parLAR.m_OPSRSmonth   << *r0.rpsrs.month;
	parLAR.m_OPSRSday     << *r0.rpsrs.day;
	parLAR.m_OPSRShour    << *r0.rpsrs.hour;
	parLAR.m_OPSRSminute  << *r0.rpsrs.minute;
	
	parLAR.m_ODSScentury  << ((*r0.sdd.year > 90)?19:20);
	parLAR.m_ODSSyear     << *r0.sdd.year;
	parLAR.m_ODSSmonth    << *r0.sdd.month;
	parLAR.m_ODSSday      << *r0.sdd.day;
	parLAR.m_ODSShour     << *r0.sdd.hour;
	parLAR.m_ODSSminute   << *r0.sdd.minute;
	
	parLAR.m_ODSLcentury  << ((*r0.ldd.year > 90)?19:20);
	parLAR.m_ODSLyear     << *r0.ldd.year;
	parLAR.m_ODSLmonth    << *r0.ldd.month;
	parLAR.m_ODSLday      << *r0.ldd.day;
	parLAR.m_ODSLhour     << *r0.ldd.hour;
	parLAR.m_ODSLminute   << *r0.ldd.minute;
}
//*****************************************************************************
//	writeLastReloadParams()
//*****************************************************************************
void CPS_BUAP_LoadingSession::writeLastReloadParams()
{
	newTRACE(("CPS_BUAP_LoadingSession::writeLastReloadParams()", 0));
	CPS_BUAP_MSG	writeLARReply;
	parLAR.UserData1() = CPS_BUAP_MSG_ID_write_LAR;
	parmgr.TXRX(parLAR, writeLARReply);
}
//*****************************************************************************
//	readStaticParams()
//*****************************************************************************
void CPS_BUAP_LoadingSession::readStaticParams()
{  
	newTRACE(("CPS_BUAP_LoadingSession::readStaticParams()", 0));
	CPS_BUAP_MSG	readOrder;
	readOrder.UserData1() = CPS_BUAP_MSG_ID_read_static;
	parmgr.TXRX(readOrder, parStatic);
}  
//*****************************************************************************
//	readDynamicParams()
//*****************************************************************************
void CPS_BUAP_LoadingSession::readDynamicParams()
{
	newTRACE(("CPS_BUAP_LoadingSession::readDynamicParams()", 0));
	CPS_BUAP_MSG	readOrder;
	readOrder.UserData1() = CPS_BUAP_MSG_ID_read_dynamic;
	parmgr.TXRX(readOrder, parDynamic);
}  
//*****************************************************************************
//	writeDynamicParams()
//*****************************************************************************
void CPS_BUAP_LoadingSession::writeDynamicParams(CPS_BUAP_MSG_dynamic_parameters& wDP)
{
	newTRACE(("CPS_BUAP_LoadingSession::writeDynamicParams()", 0));
	CPS_BUAP_MSG	wDPReply;
	wDP.UserData1() = CPS_BUAP_MSG_ID_write_dynamic;
	parmgr.TXRX(wDP, wDPReply);
}  
//
//:ag - bugfix, old version didn't take consider daylight savings
// note: this func will always update the supervision time
//
int CPS_BUAP_LoadingSession::supTimeExpired(CPS_BUAP_MSG_dynamic_parameters& uDP)
{
	newTRACE(("CPS_BUAP_LoadingSession::supTimeExpired()", 0));
	
	bool hasExpired = false;
	tm tmNow;
	time_t time_tNow;
	// * get current time
	time(&time_tNow);

	memcpy(&tmNow, localtime(&time_tNow), sizeof(tm));
	TRACE(("Get Current time : %d-%d-%d %d:%d",0 , tmNow.tm_year,tmNow.tm_mon,tmNow.tm_mday,tmNow.tm_hour,tmNow.tm_min));
	TRACE(("Get Current daylight : %d",0 , tmNow.tm_isdst));
	
	// * set up expire time
	tm tmExpire;
	memset(&tmExpire, 0, sizeof(tm));
	// since we don't persist the daylight, assume same as tmNow
	tmExpire.tm_isdst = tmNow.tm_isdst;
	
	tmExpire.tm_year = (parDynamic.m_EXPcentury() * 100 + parDynamic.m_EXPyear()) - 1900;
	tmExpire.tm_mon  = parDynamic.m_EXPmonth() - 1; // tm counts months since jan 
	tmExpire.tm_mday = parDynamic.m_EXPday();
	tmExpire.tm_hour = parDynamic.m_EXPhour();
	tmExpire.tm_min  = parDynamic.m_EXPminute();

	// Fix for TR HF82616 - daylight==1 and EXP-date == 1970-01-01 makes mktime crash!
	// Change EXP-date to 1990-01-01 instead of 1970-01-01
	if ((tmExpire.tm_year == 70) && (tmExpire.tm_mon == 0) && (tmExpire.tm_mday == 1))
	{
		tmExpire.tm_year = 90;
	}
	
	time_t time_tExpire = mktime(&tmExpire);
	TRACE(("mktime EXP time: %d%d%d %d:%d", 0, tmExpire.tm_year,tmExpire.tm_mon,tmExpire.tm_mday,tmExpire.tm_hour,tmExpire.tm_min));

	if(time_tNow >= time_tExpire)
	{
      TRACE(("Supervisiontime has expired", 0));
		hasExpired = true;
	}
	
	// * always set new expire time here (which is kinda' weird)
	// now + (supervision time * tens of minutes * secs per minute)
	time_tExpire = time_tNow + (parStatic.m_SUP() * 10 * 60);
	memcpy(&tmExpire, localtime(&time_tExpire), sizeof(tm));

	TRACE(("New expire date/time calculated", 0));
	
	uDP.m_EXPcentury << (1900 + tmExpire.tm_year) / 100;
	uDP.m_EXPyear << (1900 + tmExpire.tm_year) % 100;
	uDP.m_EXPmonth << tmExpire.tm_mon + 1; // tm counts months since jan 
	uDP.m_EXPday << tmExpire.tm_mday;
	uDP.m_EXPhour << tmExpire.tm_hour;
	uDP.m_EXPminute << tmExpire.tm_min;
	
	return hasExpired ? 1 : 0;
}
