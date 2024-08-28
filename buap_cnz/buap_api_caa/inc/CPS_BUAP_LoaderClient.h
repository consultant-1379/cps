//*****************************************************************************
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 1999, 2011.
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
//  NAME
//  CPS_BUAP_LoaderClient.h
//
//  DESCRIPTION 
//  Interface for clients towards the buloader server
//
//  DOCUMENT NO
//   CXA 110 4958
//
//  AUTHOR 
//  990806 UAB/B/SF Birgit Berggren
//  090610 XTBAKLU  Added missing return type (int) on reportCmdLog to remove warning.
// 
//******************************************************************************
// === Revision history ===
// 990806 BIR      Created.
// 000302 QABGILL  Made some members local variables/func. parameters instead
//                 removed some members
// 101215 xchihoa  Port to Linux for APG43L
// 110809 xquydao  Updated for APG43L
//******************************************************************************

#ifndef CPS_BUAP_LOADERCLIENT_H
#define CPS_BUAP_LOADERCLIENT_H

#include "CPS_BUAP_Types.h"
#include "ACS_JTP.h"

class CPS_BUAP_LoaderClient
{
public:
    //----------------------------------------------------------
    // Description:
    //      Class default constructor.
    //      Initializes all attributes and creates dynamic objects
    // Parameters:
    //      None
    // Return value:
    //      None
    // Additional information:
    //      None
    //----------------------------------------------------------
    CPS_BUAP_LoaderClient();

    //----------------------------------------------------------
    // Description:
    //    Initiates a connection to the buloader process
    // Parameters:
    //    None
    // Return value:
    //    true       Connect successfully
    //    false      Connect unsuccessfully
    // Additional information:
    //    None
    //----------------------------------------------------------
    bool connect();

    //----------------------------------------------------------
    // Description:
    //    Closes connection to the buloader process
    // Parameters:
    //    None
    // Return value:
    //    None
    // Additional information:
    //    None
    //----------------------------------------------------------
    void disconnect();

    //----------------------------------------------------------
    // Description:
    //    Checks if there still is a connection to buloader
    // Parameters:
    //    fileName - source and destination of the filename
    // Return value:
    //    true      Connected
    //    false     Not connected
    // Additional information:
    //    None
    //----------------------------------------------------------
    bool isConnected();

    //----------------------------------------------------------
    // Description:
    //    Gets the real filename according to RELADMPAR
    // Parameters:
    //    None
    // Return value:
    //    true     Obtain the file name successfully
    //    false    Obtain the file name unsuccessfully
    // Additional information:
    //    None
    //----------------------------------------------------------
    bool getFileName(char* fileName);

    //----------------------------------------------------------
    // Description:
    //    Reports information from the R0 subfile to the server
    // Parameters:
    //    data  The information to report
    // Return value:
    //    true     Report successfully
    //    false    Report unsuccessfully
    // Additional information:
    //    None
    //----------------------------------------------------------
    bool reportCmdLog(R0Info &data);

    //----------------------------------------------------------
    // Description:
    //    Obtain last error text info
    // Parameters:
    //    None
    // Return value:
    //    Last error text
    // Additional information:
    //    None
    //----------------------------------------------------------
    const char* getLastErrorText();

    //----------------------------------------------------------
    // Description:
    //    Private function used to communicate with the loader
    // Parameters:
    //    u1      IN param 1 - typically the request, such as CPS_BUAP_Loader_MsgId_getFn
    //    u2      IN additional param - typically 0
    //    msgLen  IN sizeof of send buffer
    //    msg     IN data
    //    ru1     OUT param 1
    //    r2      OUT param 2
    //    rMsgLen OUT sizeof of return buffer
    //    rMsg    OUT data
    //
    // Return value:
    //    true   Sent successfully
    //    false  Sent unsuccessfully
    // Additional information:
    //    None
    //----------------------------------------------------------
   bool sendJTPReq(ushort u1, ushort u2, ushort msgLen, char* msg, ushort& ru1,
                   ushort& ru2, ushort& rMsgLen, char*& rMsg);

private:
    ACS_JTP_Conversation m_conversation;
    char m_lastErrorText[512];
};

#endif

