//*****************************************************************************
//
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
//  CPS_BUAP_Loader_Main.cpp
//
//  DESCRIPTION 
//  Main program for the buloader process.
//
//  DOCUMENT NO
//  190 89-CAA 109 1029
//
//  AUTHOR 
//  990814 UAB/B/SF Birgit Berggren
//
//*****************************************************************************
// === Revision history ===
// 990914 BIR PA1 Created.
//******************************************************************************
#include <windows.h>
#include <process.h>

#include "CPS_BUAP_Trace.H"
#include "CPS_BUAP_Events.H"

#include "CPS_BUAP_Loader_Global.H"
#include "CPS_BUAP_Loader.H"



//==============================================================================
// CPS_BUAP_Loader_Main implements the main thread in CPS_BUAP_Loader.
//==============================================================================

DWORD WINAPI CPS_BUAP_Loader_Main(PHANDLE hAbortEventPtr)
{
  DWORD exit_code = NO_ERROR;                     

  initTRACE();

  newTRACE(("main()", 0));


  try 
    {
      CPS_BUAP_Loader Loader(hAbortEventPtr);
    
      Loader.run();

    }
  catch (...)
    {
      exit_code = DEATH_OF_SERVER; 
      EVENT((CPS_BUAP_Events::death_of_server, 0, "CPS_BUAP_loader"));
    }
    
  EXIT_THREAD(exit_code);
 
  return 0;
  
} //CPS_BUAP_Loader_Main


