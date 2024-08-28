//*****************************************************************************
// 
// .NAME
//  	CPS_BUAP_types - <Short description of the class>
// .LIBRARY 3C++
// .PAGENAME CPS_BUAP_types
// .HEADER  ACS  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE CPS_BUAP_Linux.h

// .COPYRIGHT
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 1996.
//  All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson Utvecklings AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.

// .DESCRIPTION 
// 	<General description of the class>

// .ERROR HANDLING
//
//	General rule:
//	The error handling is specified for each method.
//
//	No methods initiate or send error reports unless specified.

// DOCUMENT NO
//  190 89-CAA 109 1412

// AUTHOR 
// 	Thu Jun 12 10:47:54 1997 by UAB/I/T uabfln

//  CHANGES 
//  990608  uablan   changed byte order for struct ouputTime.
// 101215   xchihoa  Port to Linux for APG43L
//******************************************************************************
#ifndef CPS_BUAP_LINUX_H 
#define CPS_BUAP_LINUX_H 

//
//Type definition
//
typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef long unsigned int   ULONGLONG;
typedef long int   			LONGLONG;
typedef unsigned long       *LPDWORD;

typedef int				HANDLE;
typedef int				*PHANDLE;
typedef char			*LPTSTR;
typedef char			*LPCSTR;
typedef char			*LPSTR;

#if defined(MIDL_PASS)
typedef struct _ULARGE_INTEGER {
#else // MIDL_PASS
typedef union _ULARGE_INTEGER {
    struct {
        DWORD LowPart;
        DWORD HighPart;
    };
    struct {
        DWORD LowPart;
        DWORD HighPart;
    } u;
#endif //MIDL_PASS
    ULONGLONG QuadPart;
} ULARGE_INTEGER;

//
// Standard constants
//
#undef FALSE
#undef TRUE
#undef NULL

#define FALSE   0
#define TRUE    1
#define NULL    0

#define MAX_PATH          260

//
//Date time definition
//
#ifndef _FILETIME_
#define _FILETIME_
typedef struct _FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
} FILETIME, *PFILETIME, *LPFILETIME;
#endif

typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;

//
// Error definition
//
#define NO_ERROR 						 0L
#define ERROR_SUCCESS                    0L
#define ERROR_EXCEPTION                  1L
#define ERROR_PATH_NOT_FOUND             3L
#define ERROR_TOO_MANY_OPEN_FILES        4L
#define ERROR_INVALID_HANDLE             6L
#define ERROR_FILE_EXISTS                80L
#define ERROR_BAD_PATHNAME               161L
#define ERROR_ALREADY_EXISTS             183L
#define ERROR_FILENAME_EXCED_RANGE       206L
#define ERROR_CANCELLED                  1223L

//
// Service State -- for CurrentState
//
#define SERVICE_STOPPED                        0x00000001
#define SERVICE_START_PENDING                  0x00000002
#define SERVICE_STOP_PENDING                   0x00000003
#define SERVICE_RUNNING                        0x00000004
#define SERVICE_CONTINUE_PENDING               0x00000005
#define SERVICE_PAUSE_PENDING                  0x00000006
#define SERVICE_PAUSED                         0x00000007


//
// Access control
//
#define SUB_CONTAINERS_AND_OBJECTS_INHERIT  0x3


#define WINAPI      __stdcall

//
// File operation
//
#define _close      close
#define _lseek      lseek
#define _open       open
#define _read       read
//#define __O_BINARY  0



#endif	//CPS_BUAP_LINUX_H
