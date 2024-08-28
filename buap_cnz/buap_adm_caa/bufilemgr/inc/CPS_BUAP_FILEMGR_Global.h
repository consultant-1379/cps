//******************************************************************************
// COPYRIGHT Ericsson Utvecklings AB, Sweden 1999.
// All rights reserved. 
//
// The Copyright to the computer program(s) herein 
// is the property of Ericsson Utvecklings AB, Sweden.
// The program(s) may be used and/or copied only with 
// the written permission from Ericsson Utvecklings AB or in 
// accordance with the terms and conditions stipulated in the 
// agreement/contract under which the program(s) have been 
// supplied.
//
// NAME
// CPS_BUAP_FILEMGR_Global.H
//
// DESCRIPTION 
// This file contains global prototypes and definitions.
//
// DOCUMENT NO
// 190 89-CAA 109 1031
//
// AUTHOR 
// 990512 UAB/B/SD Birgit Berggren
//
//******************************************************************************
// === Revision history ===
// 990512 BIR PA1 Created.
// 990608 BIR PA2 Updated.
//******************************************************************************
#ifndef CPS_BUAP_FILEMGR_Global_H
#define CPS_BUAP_FILEMGR_Global_H

//******************************************************************************
// CPS_BUAP_FILEMGR_Main 
//******************************************************************************
DWORD WINAPI CPS_BUAP_FILEMGR_Main(PHANDLE hAbortEventPtr);

//******************************************************************************
// This macro simplifies the use of _beginthreadex.
// The call can be implemented the same way as a call to CreateThread, the same
// parameters can be used. 
// If this macro is used process.h have to be included and the code have to be 
// compiled for multi threading (one of /MD, /MDd, /MT or /MTd).
//******************************************************************************
typedef unsigned (WINAPI *ThreadStart_p) (void*);

#define CREATE_THREAD(lpThreadAttributes,		\
		      dwStackSize,		        \
		      lpStartAddress,			\
		      lpParameter,			\
		      dwCreationFlags,			\
		      lpThreadId)			\
							\
	((HANDLE) _beginthreadex(			\
		(void *)        (lpThreadAttributes),	\
		(unsigned)      (dwStackSize),		\
		(ThreadStart_p) (lpStartAddress),	\
		(void *)        (lpParameter),		\
		(unsigned)      (dwCreationFlags),	\
		(unsigned *)    (lpThreadId)))

//******************************************************************************
// This macro simplifies the use of _endthreadex.
// The call can be implemented the same way as a call to ExitThread, the same
// parameters can be used. 
// If this macro is used process.h have to be included and the code have to be 
// compiled for multi threading (on of /MD, /MDd, /MT or /MTd).
//******************************************************************************
#define EXIT_THREAD(dwExitCode) (_endthreadex((unsigned)dwExitCode))

//******************************************************************************
// For creation of threads
//******************************************************************************
#define SEQURITY_ATTRIBUTES		     NULL
#define THREAD_STACK_SIZE			 0

//******************************************************************************
// Internally generated error codes
//******************************************************************************
#define USER_ERROR 0x20000000

#define SERVICE_ABORTED            (USER_ERROR | 0x00000001)
#define DEATH_OF_SERVER            (USER_ERROR | 0x00000002)
#define INVALID_HANDLE             (USER_ERROR | 0x00000003)
#define LOST_HANDLE                (USER_ERROR | 0x00000004)
#define STOP_TIMEOUT               (USER_ERROR | 0x00000005)
#define SERVICE_SHUTDOWN           (USER_ERROR | 0x00000006)
#define COMMAND_THREAD_HANDLE_LOST (USER_ERROR | 0x00000007)

#endif

