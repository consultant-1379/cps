//*****************************************************************************
//
// NAME    CPS_BUAP_LoaderClient.C
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

// DESCRIPTION 
//      Interface for clients towards the buloader server

// DOCUMENT NO
//      CAA 109 0081

// AUTHOR 
//      Tue Sep 16 14:32:26 1997 by UAB/I/GM UABFLN

// SEE ALSO 
//      CPS_BUAP_LoaderServer
//
//*****************************************************************************

#include <errno.h>
#include <stdio.h>
//#include <syslog.h> lan
#include "CPS_BUAP_syslog.h"

#include "CPS_BUAP_LoaderClient.H"
#include "CPS_BUAP_Loader_MsgIDs.H"


//
// Member function definitions:
// ============================

//*****************************************************************************
//      CPS_BUAP_LoaderClient()
//*****************************************************************************
CPS_BUAP_LoaderClient::CPS_BUAP_LoaderClient()
  : connected(0),
    msgLen(0),
    buloader("BULOADER", JTPBUFSIZE)
{
  pfd           = new pollfd;
  pfd[0].events = POLLIN;
}  


//*****************************************************************************
//      ~CPS_BUAP_LoaderClient()
//*****************************************************************************
CPS_BUAP_LoaderClient::~CPS_BUAP_LoaderClient()
{
  delete pfd;
}  


//*****************************************************************************
//      connect()
//*****************************************************************************
int CPS_BUAP_LoaderClient::connect()
{
  // Initiate the JTP connection to BULOADER
  // ---------------------------------------
  U1 = CPS_BULoader_MsgId_ConnectClient; 
  U2 = 0;
  if (!buloader.jexinitreq(U1,U2))
    {
      buloader.jexdiscind(U1,U2,R); // Gets error
#ifdef DEBUG
      syslog(LOG_DEBUG,"loaderClient::connect() -> Unable to connect to buloader");
#endif
      return 0;
    }
  buloader.jexinitconf(RU1,RU2,R);  // Can not fail now
  pfd[0].fd = buloader.getFd();  
  return 1;
}  


//*****************************************************************************
//      disconnect()
//*****************************************************************************
void CPS_BUAP_LoaderClient::disconnect()
{
  U1 = U2 = 0;
  buloader.jexdiscreq(U1,U2,0); 
}  


//*****************************************************************************
//      isConnected()
//*****************************************************************************
int CPS_BUAP_LoaderClient::isConnected()
{
  if (poll(pfd, 1, 0) != 1)
    {
      return 1;				// Nothing from server, that is good
    }
  
  //  Read answer
  if (buloader.jexdataind(RU1, RU2, Rlen, RMsg) == false)
    {
      buloader.jexdiscind(U1,U2,R);	// Got disconnected
      return 0;
    }
  else
    {
      return 1;				// Got something, ignore it
    }  
}

//*****************************************************************************
//      getFn()
//*****************************************************************************
int CPS_BUAP_LoaderClient::getFn(RWCString& fileName)
{
  U1 = CPS_BULoader_MsgId_getFn;

  strcpy(Msg, fileName.data());
  msgLen = strlen(Msg)+1;

#ifdef DEBUG
  syslog(LOG_DEBUG,"loaderClient::getFn(%s) Msglen: %d\n" ,Msg, msgLen);
#endif


  if (!sendJTPReq())
    {
      return ENXIO;
    }

#ifdef DEBUG
  syslog(LOG_DEBUG,"loaderClient: got fn >%s< U1: %d\n" ,RMsg, RU1);
#endif

  switch (RU1)
    {
    case CPS_BULoader_MsgId_getFn_ok:
      break;
    case CPS_BULoader_MsgId_getFn_failed:
    default:
      return ENXIO;
      break;
    }  

  fileName = RMsg;
  return 0;
}  

//*****************************************************************************
//      reportCmdLog()
//*****************************************************************************
int CPS_BUAP_LoaderClient::reportCmdLog(R0Info &data)
{
  U1 = CPS_BULoader_MsgId_reportCmdLog;

  memcpy(Msg, &data, sizeof(data));
  msgLen = sizeof(data);

  if (!sendJTPReq())
    {
      return ENXIO;
    } 

  switch (RU1)
    {
    case CPS_BULoader_MsgId_reportCmdLog_ok:
      break;
    case CPS_BULoader_MsgId_reportCmdLog_failed:
    default:
#ifdef DEBUG
	  syslog(LOG_DEBUG,"loaderclient: reportCmdLog failed, errMsg: %d\n", RU1);
		 
#endif
      return ENXIO;
      break;
    }  
  return 0;
}

//*****************************************************************************
//      sendReq()
//*****************************************************************************
int CPS_BUAP_LoaderClient::sendJTPReq()
{
  //  Send req
  if (!buloader.jexdatareq(U1, U2, msgLen, Msg))
    {
#ifdef DEBUG
      syslog(LOG_DEBUG,"loaderClient::sendReq() -> jexdatarec failed\n");
#endif
      return 0;
    }
  
  //  Wait for answer
  if (poll(pfd,1, 15000) != 1) // Timeout
    {
#ifdef DEBUG
      syslog(LOG_DEBUG,"loaderClient::sendReq() -> jexdatarec timed out\n");
#endif
      return 0;
    }

  //  Read answer
  if (buloader.jexdataind(RU1, RU2, Rlen, RMsg) == false)
    {
      buloader.jexdiscind(U1,U2,R); // Gets error
#ifdef DEBUG
      syslog(LOG_DEBUG,"loaderClient::sendReq() -> jexdataind failed\n");
#endif
      return 0;
    }
 
  return 1;
}
//*****************************************************************************




