//*****************************************************************************
// 
// .NAME
//  	CPS_BUAP_types - <Short description of the class>
// .LIBRARY 3C++
// .PAGENAME CPS_BUAP_types
// .HEADER  ACS  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE CPS_BUAP_types.H

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
//	CXA 110 4958

// AUTHOR 
// 	Thu Jun 12 10:47:54 1997 by UAB/I/T uabfln

// CHANGES
//  990608  uablan   changed byte order for struct ouputTime.
//  101215   xchihoa  Port to Linux for APG43L

// .LINKAGE
//	<link information: libraries needed and order>

// .SEE ALSO 
// 	<Related classes, (abstract) baseclasses, friend classes, ...>

//******************************************************************************

#ifndef CPS_BUAP_types_H 
#define CPS_BUAP_types_H

enum subfiletype {R0 = 0, R1, R2, R3, R4, R5};

#define DEL	127
#define NUL	0
#define BLANK	32
#define ZERO	48

const char RELVOL[16] = {'R','E','L','V','O','L',' ',' ',
                         ' ',' ',' ',' ',' ',' ',' ',' '};
const char RELFILE[16] = {'R','E','L','F','I','L','E',' ',
                          ' ',' ',' ',' ',' ',' ',' ',' '};

//
// The Tape structure definitions
// ==============================
struct sektorInformationBlock
{
    char identifier[4];  // 0000 for SIBs
    char SIBCheckSum[2]; // Checksum for the SIB
    char SIBLenght[2];   // Length of significant data in SIB
    char SBBLenght[4];   // Length of significant data in SBB
    char sektorType[2];  // Sektor type code
    char fillers[4];     // Filled with 0s R0, mem-adress info R1-5
};

struct checksumBlock
{
    char checkSum[2];
    char fillers[16]; // Filled with 0s
};

// File format of RO as written to disk by APZ
// ===========================================
struct controlInfoSector
{
    sektorInformationBlock SIB;
    char SIBfillers[2030];
    char loadingIndicator[2];
    char signalNumber[2];
    char blockNumber[2];
    char APZType[2];
    //				210 - APZ 210 nn
    //				211 - APZ 211 nn
    //				212 - APZ 212 02/11
    //				 20 - APZ 212 20
    //				 25 - APZ 212 25
    //				 30 - APZ 212 30
    char cmdLogSubFileR3[4];
    char cmdLogSubFileR4[4];
    char fillers[2];
    char CISfillers[2030];
    checksumBlock CSB;
    char CSBfillers[2030];
};

struct outputTime
{
    char year[1];
    char month[1];
    char day[1];
    char hour[1];
    char minute[1];
};

struct outputTimesSector
{
    sektorInformationBlock SIB;
    char SIBfillers[2030];
    outputTime dump;
    outputTime r1;
    outputTime r2;
    outputTime r3;
    outputTime r4;
    outputTime r5;
    char fillers[2018];
    checksumBlock CSB;
    char CSBfillers[2030];
};

struct exchangeIDSector
{
    sektorInformationBlock SIB;
    char SIBfillers[2030];
    char nameLen[1];
    char name[47];
    char fillers[2000];
    checksumBlock CSB;
    char CSBfillers[2030];
};

struct generationSector
{
    sektorInformationBlock SIB;
    char SIBfillers[2030];
    char RScheckSum[4];
    char fillers[14];
    char GSfillers[2030];
    checksumBlock CSB;
    char CSBfillers[2030];
};

struct R0File
{
    controlInfoSector CIS;
    outputTimesSector OTS;
    exchangeIDSector  EIS;
    generationSector  GS;
};

struct R0Info
{
    short APZtype;    // 210; 211; 212 = 212 02/11
                      // 20/25/30 = 212 20/25/30
    long subfile;     // file# of cmdlog subfile
    outputTime rpsrs; // R5    Prog/Ref Store
    outputTime sdd;   // R1/R2 Small Data dump
    outputTime ldd;   // R3/R4 Large ditto
};

//Forward declaration(s)

#endif
