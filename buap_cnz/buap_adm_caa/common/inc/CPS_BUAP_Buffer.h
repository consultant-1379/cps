
//******************************************************************************
// 
// .NAME
//  	CPS_BUAP_Buffer - Class defining a buffer
// .LIBRARY 3C++
// .PAGENAME CPS_BUAP_Buffer
// .HEADER  CPS  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE CPS_BUAP_Buffer.H

// .COPYRIGHT
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 1997.
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
//	This class implments a buffer.  Operators are defined
//	for adding byte values to and removing them from the
//	buffer. Other methods are defined for resetting (emptying)
//	and returning the length of the buffer.
//
//	Note that two constructors are provided.

// .ERROR HANDLING
//
//	General rule:
//	The error handling is specified for each method.
//
//	No methods initiate or send error reports unless specified.

// DOCUMENT NO
//  190 89-CAA 109 1412

// AUTHOR 
// 	1997/08/06 by U/B/SD David Wade

// .LINKAGE
//	-

// .SEE ALSO 
// 	-
//  101010 xchihoa     Ported to Linux for APG43L.
//******************************************************************************


#ifndef CPS_BUAP_Buffer_H
#define CPS_BUAP_Buffer_H

class CPS_BUAP_Buffer
{
public:

  CPS_BUAP_Buffer(int Size);
  // Description:
  // 	Class constructor to be used when the buffer is to be
  //	allocated by this class.
  // Parameters: 
  //    Size			Size of buffer to be allocated, in  
  // Return value: 
  //    -
  // Additional information:
  //    -

  CPS_BUAP_Buffer(int Size, char *buffer);
  // Description:
  // 	Class constuctor to be used when the buffer was pre-allocated
  //	by an external entity.
  // Parameters: 
  //    Size			Size of buffer to be allocated, in 
  //    *buffer			Pointer to actual buffer, in 
  // Return value: 
  //    -
  // Additional information:
  //    -

  virtual ~CPS_BUAP_Buffer();
  // Description:
  // 	Class destructor.  Will delete buffer object if allocated
  //	by this class.
  // Parameters: 
  //    -
  // Return value: 
  //    -
  // Additional information:
  //    -


  CPS_BUAP_Buffer& operator<<(char ch);
  // Description:
  // 	Input operator used to add a value to the buffer
  // Parameters: 
  //    ch			character to be added to the buffer, in
  // Return value: 
  //    CPS_BUAP_Buffer&	reference to this class
  // Additional information:
  //    -

  CPS_BUAP_Buffer& operator>>(char& ch);
  // Description:
  // 	Ouput operator used to remove a byte from the buffer.
  // Parameters: 
  //    ch&			character reference, out 
  // Return value: 
  //    CPS_BUAP_Buffer&	reference to this class
  // Additional information:
  //    -


  char *Data();
  // Description:
  // 	Method used to return a pointer to the buffer
  // Parameters: 
  //    -
  // Return value: 
  //    char *			pointer to start of buffer
  // Additional information:
  //    -

  void  Reset();
  // Description:
  // 	Method used to reset the buffer index
  // Parameters: 
  //    -
  // Return value: 
  //    -
  // Additional information:
  //    -

  int   Length();
  // Description:
  // 	Method used to determine the length of the buffer, i.e. the
  //	number of bytes in the buffer.
  // Parameters: 
  //    -
  // Return value: 
  //    int			number of bytes in buffer
  // Additional information:
  //    -

private:
  CPS_BUAP_Buffer(const CPS_BUAP_Buffer& );
  //:ag1
  // Description:
  //	Copy constructor
  // Parameters: 
  //    -
  // Return value: 
  //    -
  // Additional information:
  //	Declared to disallow copying
  CPS_BUAP_Buffer& operator=(const CPS_BUAP_Buffer& );
  // Description:
  //	Assignment operator
  // Parameters: 
  //    -
  // Return value: 
  //    -
  // Additional information:
  //   Declared to disallow assignment


protected:
  int         m_Index;		// index to next available char

private:
  const int   m_Size;		// size of the buffer
  int         m_Length;		// Encoded length    
  char       *m_Buffer;		// pointer to actual buffer
  const int   m_Delete;		// indicator of whether buffer should
				// be deleted by the class destructor.a
				// (i.e. was the buffer allocated by the
				// constructor)
};

#endif
