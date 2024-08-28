#ifndef _PLATFORMTYPES_H_
#define _PLATFORMTYPES_H_
/*
NAME
	PLATFORMTYPES - 
LIBRARY 3C++
PAGENAME PLATFORMTYPES
HEADER  CPS
LEFT_FOOTER Ericsson Utvecklings AB
INCLUDE PlatformTypes.h

COPYRIGHT
	COPYRIGHT Ericsson Utvecklings AB, Sweden 2011. All rights reserved.

	The Copyright to the computer program(s) herein is the property of
	Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
	copied only with the written permission from Ericsson Utvecklings AB
	or in accordance with the terms and conditions stipulated in the 
	agreement/contract under which the program(s) have been supplied.

DESCRIPTION 
   typedefs for basic types, etc, matches similar file in APZ-VM

ERROR HANDLING
   -

DOCUMENT NO
   190 89-CAA 109 1410

AUTHOR 
   2011-11-15 by XDT/DEK Tu, Do Hoang (xtuudoo) based on PlatformTypes.h of Windows version

LINKAGE
	-

SEE ALSO 


Revision history
----------------
2001-05-01  qabgill  Created
2005-09-09  uablan   Changed MAKEDDWORD. Cast hi to u_int64 before shift! TR HG18932
   
*/


#include <stdint.h>

typedef uint8_t         u_char;
typedef uint8_t         u_int8;
typedef int8_t          int8;
typedef uint16_t        u_int16;
typedef int16_t         int16;
typedef uint32_t        u_int32;
typedef int32_t         int32;
typedef int64_t         int64;
typedef uint64_t        u_int64;


// extract LOW 32-bits from double-double word
//#define LODWORD(val) ((DWORD)(val))
// extract HIGH 32-bits from double-double word
//#define HIDWORD(val) ((DWORD)(((u_int64)(val) >> 32) & 0xFFFFFFFF))

// create DOUBLE_DOUBLE_WORD (64 bits)
//#define MAKEDDWORD(hi, lo) ((u_int64) (((u_int64)(hi) << 32) | ((u_int64)(lo) & 0x00000000FFFFFFFF)))

#endif
