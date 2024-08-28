//*****************************************************************************
// COPYRIGHT Ericsson Utvecklings AB, Sweden 1999.
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
//  NAME
//  CPS_BUAP_LoaderClient.cpp     
//
//  DESCRIPTION 
//  Interface for clients towards the buloader server
//
//  DOCUMENT NO
//  190 89-CAA 109 1029
//
//  AUTHOR 
//  990806 UAB/B/SF Birgit Berggren
//
//*****************************************************************************
// === Revision history ===
// 990806 BIR PA1 Created.
//*****************************************************************************
#include <windows.h>
#include <assert.h>
#include <stdio.h>

#include "CPS_syslog.h"
#include "CPS_BUAP_LoaderClient.H"
#include "CPS_BUAP_Loader_MsgIDs.H"
#include "CPS_BUAP_RepEvent.h"

extern HANDLE g_bufilelog;
//
// Member function definitions:
// ============================

//*****************************************************************************
//      CPS_BUAP_LoaderClient()
//*****************************************************************************
CPS_BUAP_LoaderClient::CPS_BUAP_LoaderClient()
: buloader("BULOADER", JTPBUFSIZE)
{
}


//*****************************************************************************
//      connect()
//*****************************************************************************
int CPS_BUAP_LoaderClient::connect()
{
	ushort ru1 = 0, ru2 = 0, r = 0;
	// Initiate the JTP connection to BULOADER
	// ---------------------------------------
	if (!buloader.jexinitreq(CPS_BUAP_Loader_MsgId_ConnectClient, 0))
	{
		DBGTRACE("loaderClient::connect() -> Unable to connect to buloader");
		
      buloader.jexdiscind(ru1, ru2, r); // Gets error
		
		DBGLOG((CPS_syslog::LVLDEBUG,
			"loaderClient::connect()->jexdiscind: %d, %d, %d",
			ru1, ru2, r));
		
		RepEvent(g_bufilelog, EVENTLOG_ERROR_TYPE, "Connect failed: %d, %d, %d", ru1, ru2, r);
      return 0;
   }
	
	//buloader.setNoOfTries(3); //:ag1
	buloader.setTimeOut(30); //:ag1 - secs
	buloader.jexinitconf(ru1, ru2, r);  // Cannot fail now
	return 1;
}  


//*****************************************************************************
//      disconnect()
//*****************************************************************************
void CPS_BUAP_LoaderClient::disconnect()
{
	bool res = buloader.jexdiscreq(0, 0, 0); //:ag1 normal disconnect
	
	if(!res)
		RepEvent(g_bufilelog, EVENTLOG_WARNING_TYPE, "Disconnect failed");
	
	//assert(res); //:ag1 why does this fail
}  


//*****************************************************************************
//      isConnected()
//*****************************************************************************
int CPS_BUAP_LoaderClient::isConnected()
{
	
	DWORD pollResult;
	
	pollResult = WaitForSingleObject(buloader.getHandle(), 1); //:ag1 // We don't want to wait
	switch(pollResult)
   {
   case WAIT_OBJECT_0:                  // Got something from server, check
		break;                          // if it's a disconnection request
		
   case WAIT_TIMEOUT:                   // Nothing from server, that's good
		return 1;
		
   case WAIT_FAILED:
		pollResult = GetLastError();
		return 0;
		
   default:
		return 0;
		
   }                                                                  
	
   
	// If jexdataind returns false, disconnection of the conversation
	// has been requested from the server side.
	ushort ru1 = 0, ru2 = 0, r = 0, rLen = 0;
	char* rMsg;
	if (buloader.jexdataind(ru1, ru2, rLen, rMsg) == false)
	{
		buloader.jexdiscind(ru1, ru2, r);	// Got disconnected
      return 0;
	}
	else
	{
		DBGLOG((CPS_syslog::LVLDEBUG,
			"Ignoring data in loaderclient->isConnected, %d, %d, %d, <%s>",
			ru1, ru2, rLen, rMsg));
      return 1;				// Got some data, ignore it
	}  
}

//*****************************************************************************
// getFn()
//*****************************************************************************
int CPS_BUAP_LoaderClient::getFn(/* in/out param */ char* fileName)
{
	DBGLOG((CPS_syslog::LVLDEBUG,
		"loaderClient::getFn(<%s>) Msglen: %d",
		fileName, strlen(fileName) + 1));
	
	ushort ru1 = 0, ru2 = 0, rMsgLen = 0;
	char* rMsg = 0;
	if(!sendJTPReq(CPS_BUAP_Loader_MsgId_getFn, 0, strlen(fileName) + 1, fileName,
		ru1, ru2, rMsgLen, rMsg))
	{
		return ERROR_DEV_NOT_EXIST;
	}
	
	DBGLOG((CPS_syslog::LVLDEBUG, "loaderClient: got fn <%s> U1: %d", rMsg, ru1));
	
	switch (ru1)
	{
	case CPS_BUAP_Loader_MsgId_getFn_ok:
      break;
	case CPS_BUAP_Loader_MsgId_getFn_failed:
		RepEvent(g_bufilelog, EVENTLOG_ERROR_TYPE, "Get file name failed");
	default:
      return ERROR_DEV_NOT_EXIST;
      break;
	}  
	
	strcpy(fileName, rMsg);
	return 0;
}  

//*****************************************************************************
//      reportCmdLog()
//*****************************************************************************
int CPS_BUAP_LoaderClient::reportCmdLog(R0Info &data)
{
	ushort ru1 = 0, ru2 = 0, rMsgLen = 0;
	char* rMsg = 0;
	
	char msg[JTPBUFSIZE];
	memcpy(msg, &data, sizeof(data));
	ushort msgLen = sizeof(data);
	
	if (!sendJTPReq(CPS_BUAP_Loader_MsgId_reportCmdLog, 0, msgLen, msg,
		ru1, ru2, rMsgLen, rMsg))
   {
      return ERROR_DEV_NOT_EXIST;
   } 
	
	switch (ru1)
	{
	case CPS_BUAP_Loader_MsgId_reportCmdLog_ok:
		break;
	case CPS_BUAP_Loader_MsgId_reportCmdLog_failed:
		RepEvent(g_bufilelog, EVENTLOG_ERROR_TYPE, "Report to command log failed");
	default:
		DBGLOG((CPS_syslog::LVLDEBUG, "loaderclient: reportCmdLog failed, errMsg: %d", ru1));
		return ERROR_DEV_NOT_EXIST;
		break;
	}  
	return 0;
}

//*****************************************************************************
//      sendJTPReq()
//*****************************************************************************
int CPS_BUAP_LoaderClient::sendJTPReq(ushort u1, ushort u2, ushort msgLen, char* msg,
												  ushort& ru1, ushort& ru2, ushort& rMsgLen, char*& rMsg)
{
	
	//  Send conversation data
	if (!buloader.jexdatareq(u1, u2, msgLen, msg))
   {
		DBGLOG((CPS_syslog::LVLDEBUG,
			"loaderClient::sendReq() -> jexdatareq failed: %d, %d, %d, <%s>",
			u1, u2, msgLen, msg));
		RepEvent(g_bufilelog, EVENTLOG_ERROR_TYPE, "JTP failure: %d, %d, %d, <%s>",
			u1, u2, msgLen, msg);
      return 0;
	}
	
	//  Wait for answer
	DWORD pollResult = WaitForSingleObject(buloader.getHandle(), 15000); //:ag1
	
	switch(pollResult)
	{
	case WAIT_OBJECT_0:
		// Successful, continue execution.
		break;
		
	case WAIT_FAILED:
		// Unsuccessful.
		pollResult = GetLastError(); 
		RepEvent(g_bufilelog, EVENTLOG_WARNING_TYPE, "JTP Wait failed: %d", pollResult);
		DBGTRACE("loaderClient::WaitForSingleObject failed");
		return 0;
		
	case WAIT_TIMEOUT:
		// Unsuccessful.  
		RepEvent(g_bufilelog, EVENTLOG_WARNING_TYPE, "JTP Wait timed out");
		DBGTRACE("loaderClient::sendReq() -> WaitForSingleObject time out");
		return 0;
	}
	
	//  Read answer
	if (buloader.jexdataind(ru1, ru2, rMsgLen, rMsg) == false)
   {
		DBGLOG((CPS_syslog::LVLDEBUG, "loaderClient::sendReq() -> jexdataind failed: %d, %d, %d, <%s>",
			ru1, ru2, rMsgLen, rMsg));
		
		ushort r = 0;
      buloader.jexdiscind(ru1, ru2, r); // Gets error
		
      DBGLOG((CPS_syslog::LVLDEBUG, "loaderClient::sendReq() -> jexdiscind: %d, %d, %d", ru1, ru2, r));
		RepEvent(g_bufilelog, EVENTLOG_WARNING_TYPE, "JTP Read Data failed: %d, %d, %d", ru1, ru2, r);
      return 0;
	}
	
	return 1;
}

