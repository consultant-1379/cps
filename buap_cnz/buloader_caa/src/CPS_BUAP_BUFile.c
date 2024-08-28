//  SIO P1A
//*****************************************************************************
//
// NAME    CPS_BUAP_BUFile.C
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
// 	Interface to read AXE reload files. The file object will
//	manage that the correct physical file is chosen, reserved
//	from CPF.

// DOCUMENT NO
//	CAA 109 0081

// AUTHOR 
// 	Fri May 23 12:41:14 1997 by UAB/I/T uabfln

// CHANGES
// 9906023  uablan  Convert into NT code and remove all tape handling and
//                  make it a dll with functions (no class any more)
// 000215   qabgill Misc. fixes & changes

// SEE ALSO 
// 	-
//
//*****************************************************************************

#include	<windows.h>
#include    <windef.h>
#include    <assert.h>
#include    <io.h>               // _open,_read,_lseek

#include    <fcntl.h>            // _open - O_RDONLY
#include    <stddef.h>           // for offsetof

#include	"CPS_BUAP_BUFile.H"
#include	"CPS_BUAP_types.H"    // !!!!require changed struct outputTime!!!!!
#include	"CPS_BUAP_LoaderClient.H"
#include "CPS_syslog.h"
#include "CPS_BUAP_RepEvent.h"

#include	"ACS_JTP.H"
#include	"FMS_CPF_File.H"

extern HANDLE g_bufilelog;
//*****************************************************************************
// Local variables
//*****************************************************************************

CPS_BUAP_LoaderClient	buloader;
FMS_CPF_File			cpfSubfile;		// (Sub)file in CPFS
char					   pszFileName[80];	//:ag1 changed name - the file name (RELFSWxxx)
int						filep;         // File handle for subfile


//
// Function definitions:
// =====================


//*****************************************************************************
//	CPS_BUAP_attachFile(fNumber)
//
//  Calculate correct file number, connect to buloader.
//  Return values:  0 - OK
//                  1 - File number not ok
//                  2 - CPS_BUAP_BUFILE_READ_ERROR
//  Comment: similar functionality as a constructor
//*****************************************************************************
BYTE CPS_BUAP_attachFile(WORD fNumber)
{
   filep = 0;
	pszFileName[0] = '\0';
	
	DBGLOG((CPS_syslog::LVLDEBUG, "bufile attachFile(Fileno: %d) called.", fNumber));
	
	//-------------------------------------------------------------------------
	// Check file number, 255 is uniq and ok, odd numbers not allowed and
	// even numbers must be cut in half (LAB send the filenumber*2 or 255). 
	// ------------------------------------------------------------------------
	if (fNumber != 255)			  // 255 most common, go on unchanged
	{
		if ((fNumber > 255) || (fNumber%2 == 1))  // Too big or odd number
		{
			return CPS_BUAP_BUFILE_NO_FILE;
		}
		//---------------------------------------------------------------------
		// Cut number in half
		//---------------------------------------------------------------------
		fNumber /= 2;
	}
	
	//-------------------------------------------------------------------------
	// Initiate the JTP connection to BULOADER.
	// ------------------------------------------------------------------------
	if (!buloader.connect())
   {
		DBGTRACE("bufile: could not connect to buloader server.");
		return CPS_BUAP_BUFILE_READ_ERROR;
	}
	
  	try
	{
		//----------------------------------------------------------------------
		// Get real RELFSWname. 
		//----------------------------------------------------------------------
		sprintf(pszFileName, "RELFSW%i", fNumber);
		if (buloader.getFn(pszFileName) != 0)
		{
			DBGTRACE("bufile: could not get real RELFSW name.");
			throw CPS_BUAP_BUFILE_READ_ERROR;
		}
		assert(strlen(pszFileName) > 6);
		DBGLOG((CPS_syslog::LVLDEBUG, "bufile name transformed. New name: %s", pszFileName));
	}
	//---------------------------------------------------------------------------
	// Here is the thrown errors catched
	//---------------------------------------------------------------------------
	catch (BYTE error) //:ag1 int to BYTE
	{
		buloader.disconnect();
		pszFileName[0] = '\0';
		return error;
	}
	
	return CPS_BUAP_BUFILE_OK;
	
}  


//*****************************************************************************
//	CPS_BUAP_detachFile()
//
//  Close current subfile and unreserve the whole reload file (all subfiles).
//  Return values:  0 - OK
//  Comment: similar to destructor 
//*****************************************************************************
BYTE CPS_BUAP_detachFile()
{  
	DBGTRACE("bufile: detachFile.");
	
	if(filep != NULL)
	{
		_close(filep);
		filep = NULL;
	}
	
	if(cpfSubfile.isReserved())
	{
		cpfSubfile.unreserve();
	}
	// ------------------------------------------------------------------------
	// Disconnect from buloader
	// ------------------------------------------------------------------------
	if (buloader.isConnected())
	{
		buloader.disconnect();
	}
	
	strcpy(pszFileName, "");
	
	return CPS_BUAP_BUFILE_OK;
}  


//*****************************************************************************
//  CPS_BUAP_openSubfile(subfile)
//
//  Opens a reload subfile and reserves the reload subfile.
//  Return value: 0 - OK 
//                1 - File not found
//	              2 - Disk read error
//
//*****************************************************************************
BYTE CPS_BUAP_openSubfile(WORD subfile)
{
	assert(subfile >= R0 && subfile <= R5); //:ag1
	DBGLOG((CPS_syslog::LVLDEBUG, "bufile: openSubfile(No.: %d)", subfile));
	
	// ------------------------------------------------------------------------
	// Check if file already opened
	// ------------------------------------------------------------------------
	if (filep != NULL)
   {
		DBGTRACE("bufile: file already opened.");
		return CPS_BUAP_BUFILE_READ_ERROR; 
   }
	
	// ------------------------------------------------------------------------
	// Create the complete filename with subfile
	// ------------------------------------------------------------------------
	//:ag1 removed Rogue Wave
	char fullFilename[MAX_PATH];
	sprintf(fullFilename, "%s-R%c", pszFileName, '0' + subfile);
	DBGLOG((CPS_syslog::LVLDEBUG, "bufile: fullFilename is %s", fullFilename));
	
	try
	{
		// --------------------------------------------------------------------
		//  Apply name to CPFS
		// --------------------------------------------------------------------
		cpfSubfile = FMS_CPF_File(fullFilename);
		
		// --------------------------------------------------------------------
		// Get the physical path for the subfile from CPFS
		// --------------------------------------------------------------------
		try 
		{
         cpfSubfile.reserve(FMS_CPF_Types::R_XW_);
         
			DBGLOG((CPS_syslog::LVLDEBUG, "bufile: Reserved: %s.", fullFilename));
			
         strcpy(fullFilename, cpfSubfile.getPhysicalPath());
			
			DBGLOG((CPS_syslog::LVLDEBUG, "bufile: Physical filename: %s.", fullFilename));
		}
		
		catch (FMS_CPF_Exception& ex)
		{
			RepEvent(g_bufilelog, EVENTLOG_ERROR_TYPE, "CPF_File error: %d", ex.errorCode());
			DBGLOG((CPS_syslog::LVLDEBUG, "bufile: CPF error %d, %s.", ex.errorCode(), ex.errorText()));
			switch (ex.errorCode())
			{
			case FMS_CPF_Exception::FILENOTFOUND: //:ag1 added
			case FMS_CPF_Exception::INVALIDFILE: //:ag1 added
			case FMS_CPF_Exception::INVALIDREF:
				throw CPS_BUAP_BUFILE_NO_FILE;
				break;
			case FMS_CPF_Exception::ACCESSERROR: //:ag1 added
			case FMS_CPF_Exception::FILEISOPEN: //:ag1 added
			case FMS_CPF_Exception::INTERNALERROR: //:ag1 added
			case FMS_CPF_Exception::PHYSICALERROR:
			case FMS_CPF_Exception::SOCKETERROR:
			default:
				throw CPS_BUAP_BUFILE_READ_ERROR;
				break;
			}
		}
		
		// --------------------------------------------------------------------
		// Open the file for reading
		// --------------------------------------------------------------------
		filep = _open(fullFilename, O_RDONLY | _O_BINARY); //:ag1
		if (filep == -1)
		{
			DBGLOG((CPS_syslog::LVLDEBUG, "bufile::open() Physical file open failed: ErrNo: %d",
				errno));
			filep = NULL;
			if (errno == ENOENT)
			{
				throw CPS_BUAP_BUFILE_NO_FILE;
			}
			else
			{
				throw CPS_BUAP_BUFILE_READ_ERROR;
			}
		}
		
		// --------------------------------------------------------------------
		// If this is R0 we need to report commandlog subfile and 
		// subfile dates to the loader server.
		// --------------------------------------------------------------------
		
		if (subfile == R0) 
		{
			unsigned char li;
			R0Info	data;
			memset(&data, 0, sizeof(R0Info));
			
			// ----------------------------------------------------------------
			// Get the loadingIndicator who tells which subfile to load
			// and check that the loadingIndicator has a valid value 
			// (2,4,8 or 16).
			//-----------------------------------------------------------------
			_lseek(filep,
				offsetof(struct controlInfoSector, 
				loadingIndicator), 
				SEEK_SET);
			_read(filep, &li, 1);
			
			if ( !( ((li&2)^(li&4)) && ((li&8)^(li&16)) ) ) // Sanity check
			{
				DBGLOG((CPS_syslog::LVLDEBUG,
					"bufile: R0-File read error for loading indicator. fn: %s. res:%d",
					fullFilename, li));
				throw CPS_BUAP_BUFILE_READ_ERROR;
			}
			
			// ----------------------------------------------------------------
			// Get the APZ type 
			// ----------------------------------------------------------------
			_lseek(filep, offsetof(struct controlInfoSector, APZType), SEEK_SET);
			_read(filep, &(data.APZtype), sizeof(data.APZtype));
			
			//-----------------------------------------------------------------
			// loadingIndicator is R4 (li & <|0|0|R0|R1|R2|R3|*R4|R5|>)
			// Get output time for R4
			//-----------------------------------------------------------------
			if (li & 2) 
			{
				_lseek(filep, 
					offsetof(struct controlInfoSector, 
					cmdLogSubFileR4), 
					SEEK_SET);
				_read(filep, &(data.subfile), sizeof(data.subfile));
				
				_lseek(filep, 
					offsetof(struct R0File, OTS) +
					offsetof(struct outputTimesSector, r4), 
					SEEK_SET);
				_read(filep, &(data.ldd), sizeof(data.ldd));
			}
			
			//-----------------------------------------------------------------
			// loadingIndicator is R3 (li & <|0|0|R0|R1|R2|*R3|R4|R5|>)
			// Get output time for R3
			//-----------------------------------------------------------------
			if (li & 4)
			{
				_lseek(filep, 
					offsetof(struct controlInfoSector, 
					cmdLogSubFileR3), 
					SEEK_SET);
				_read(filep, &(data.subfile), sizeof(data.subfile));
				
				_lseek(filep, 
					offsetof(struct R0File, OTS) +
					offsetof(struct outputTimesSector, r3), 
					SEEK_SET);
				_read(filep, &(data.ldd), sizeof(data.ldd));
			}
			
			//-----------------------------------------------------------------
			// loadingIndicator is R2 (li & <|0|0|R0|R1|*R2|R3|R4|R5|>)
			// Get output time for R2
			//-----------------------------------------------------------------
			if (li & 8)
			{
				_lseek(filep, 
					offsetof(struct R0File, OTS) +
					offsetof(struct outputTimesSector, r2), 
					SEEK_SET);
				_read(filep, &(data.sdd), sizeof(data.sdd));
			}
			
			//-----------------------------------------------------------------
			// loadingIndicator is R1 (li & <|0|0|R0|*R1|R2|R3|R4|R5|>)
			// Get output time for R1
			//-----------------------------------------------------------------
			if (li & 16)
			{
				_lseek(filep, 
					offsetof(struct R0File, OTS) +
					offsetof(struct outputTimesSector, r1), 
					SEEK_SET);
				_read(filep, &(data.sdd), sizeof(data.sdd));
			}
			
			//-----------------------------------------------------------------
			// Get output time for R5
			//-----------------------------------------------------------------
			_lseek(filep, 
				offsetof(struct R0File, OTS) +
				offsetof(struct outputTimesSector, r5), 
				SEEK_SET);
			_read(filep, &(data.rpsrs), sizeof(data.rpsrs));
			
			//-----------------------------------------------------------------
			// Order writing of the backup generation number
			// and cmdlog information into reladmpar
			//-----------------------------------------------------------------
			DBGLOG((CPS_syslog::LVLDEBUG, "bufile: Sending R0 data to server, cmdlog: %ld",
				data.subfile));
			if (buloader.reportCmdLog(data) != 0)
			{
				throw CPS_BUAP_BUFILE_READ_ERROR;
			}
		}	
    }
	 //----------------------------------
	 // Here is the thrown errors catched
	 // This is fatal - we can't continue with the file batch
	 //----------------------------------
	 catch (BYTE error) //:ag1 int to BYTE
    {
		 RepEvent(g_bufilelog, EVENTLOG_ERROR_TYPE, "File error: %d", error);
		 CPS_BUAP_detachFile(); //:ag1 close everything
		 return error;
    }
	 
	 //-------------------------------------------------------------------------
	 // Set file pointer to point at start of file
	 //-------------------------------------------------------------------------
	 _lseek(filep, 0, SEEK_SET);
	 return CPS_BUAP_BUFILE_OK;
}

//*****************************************************************************
//  CPS_BUAP_closeSubfile()
//
//  Unreserves & closes a reload subfile.
//  Return values: 0 - OK
//                 1 - CPS_BUAP_BUFILE_NO_FILE
//                 2 - CPS_BUAP_BUFILE_READ_ERROR
//
// Comment: Only one subfile may be reserved at a time. Since we're using a global
// variable to keep track of the reserved file, this information will be overwritten
// each time openSubfile is called.
// 
//*****************************************************************************
BYTE CPS_BUAP_closeSubfile()
{
	DBGTRACE("bufile: closeSubfile()");
	
	assert(filep > NULL);
   // Close the actual file
	// ---------------------
	if (filep != NULL) //:ag1 changed from (filep <= NULL)
   {
		_close(filep); 
		filep = NULL;
   }
	
   //:ag1
	try 
	{
      assert(cpfSubfile.isReserved());	   
		// --------------------------------------------------------------------
		// unreserve the file
		// --------------------------------------------------------------------
      if(cpfSubfile.isReserved())
		{
			cpfSubfile.unreserve();
		}
		assert(!cpfSubfile.isReserved());
   }
	catch (FMS_CPF_Exception& ex)
	{
		RepEvent(g_bufilelog, EVENTLOG_ERROR_TYPE, "CPF File error: %d", ex.errorCode());
		DBGLOG((CPS_syslog::LVLDEBUG, "bufile: CPF error %d, %s.", ex.errorCode(), ex.errorText()));
		switch (ex.errorCode())
		{
		case FMS_CPF_Exception::FILENOTFOUND:
			return CPS_BUAP_BUFILE_NO_FILE; //:ag1 changed from throw
			break;
		case FMS_CPF_Exception::INTERNALERROR:
		case FMS_CPF_Exception::SOCKETERROR:
		default:
			return CPS_BUAP_BUFILE_READ_ERROR; //:ag1 changed from throw
			break;
		}
	}
	
	return CPS_BUAP_BUFILE_OK;
}


//*****************************************************************************
// CPS_BUAP_readSubfile(bufp,bufSize,size)
//
// Reads data from the file into the block pointed to by the user.
// Return values: 0 - OK 
//                2 - Disk read error
//                3 - End of file
//
//*****************************************************************************
BYTE CPS_BUAP_readSubfile(BYTE* bufp,DWORD bufSize, DWORD* size)
{
	BYTE rc = 0;
	
	// ------------------------------------------------------------------------
	// Check if loader server is connected.
	// ------------------------------------------------------------------------
	if(!buloader.isConnected())
   {
		RepEvent(g_bufilelog, EVENTLOG_ERROR_TYPE, "Connection lost");
		DBGTRACE("bufile: buloader server is not connected.");
		return CPS_BUAP_BUFILE_READ_ERROR;
	}
	
	// ------------------------------------------------------------------------
	// Do not make a read if bufSize is zero. 
	// ------------------------------------------------------------------------
	if (bufSize == 0) return CPS_BUAP_BUFILE_READ_ERROR;
	
	// ------------------------------------------------------------------------
	// Read from file into buffer.
	// ------------------------------------------------------------------------
	*size = _read(filep, bufp, bufSize);
	
	switch (*size)
	{
	case -1:    
		// ----------------------------------------------------------------
		// File read error
		// ----------------------------------------------------------------
		DBGLOG((CPS_syslog::LVLDEBUG, "bufile: file read() error. errno: %d.", errno));
		rc = CPS_BUAP_BUFILE_READ_ERROR;
		break;
	case 0:  
		// ----------------------------------------------------------------
		// Filedescriptor points at EOF
		// ----------------------------------------------------------------
		rc = CPS_BUAP_BUFILE_EOF;
		DBGTRACE("bufile: EOF from read.");
		break;
	default:
		// ----------------------------------------------------------------
		// Normal case
		// Check number of bytes read.
		// ----------------------------------------------------------------
		if ((*size > 0) && (*size < bufSize))
		{
			rc = CPS_BUAP_BUFILE_READ_ERROR;
			DBGTRACE("bufile: premature end");
		}
		else if (*size == bufSize)
		{
			rc = CPS_BUAP_BUFILE_OK;
		}
		else
		{
			rc = CPS_BUAP_BUFILE_READ_ERROR;
			DBGLOG((CPS_syslog::LVLDEBUG, "bufile: file read() error. errno: %d.", errno));
		}
		break;
	}

	if(rc == CPS_BUAP_BUFILE_READ_ERROR)
		RepEvent(g_bufilelog, EVENTLOG_ERROR_TYPE, "BUFile read error");

	return rc;
} 

//*****************************************************************************
// CPS_BUAP_moveSubfilePtr(offset)
//
// Moves the file pointer in the open reload subfile.
// Return value: 0 - OK
//               2 - Disk read error
//
//*****************************************************************************
BYTE CPS_BUAP_moveSubfilePtr(DWORD offset)
{
	DWORD currPos,newPos;
	
	// ------------------------------------------------------------------------
	// Get current position.
	// ------------------------------------------------------------------------
	currPos = _lseek(filep,(long) 0, SEEK_CUR);
	
	// ------------------------------------------------------------------------
	// Move the file pointer 'offset' position forward from current position.
	// ------------------------------------------------------------------------
	newPos = _lseek(filep, (long) offset, SEEK_CUR);
	
	switch(newPos)
	{
	case -1:
		DBGTRACE("bufile: file pointer handle invalid");
		RepEvent(g_bufilelog, EVENTLOG_ERROR_TYPE, "Invalid file pointer");
		return CPS_BUAP_BUFILE_READ_ERROR;
		break;
	default:
		if ( newPos != (currPos + offset) )
		{
			RepEvent(g_bufilelog, EVENTLOG_ERROR_TYPE, "File pointer move failed");
			DBGTRACE("bufile: file pointer not move requested offset");
			return CPS_BUAP_BUFILE_READ_ERROR;
		}
		else
		{
			return CPS_BUAP_BUFILE_OK;
		}
		break;
	}
}


//*****************************************************************************
//  CPS_BUAP_checkEndOfFile()
//
//  Checks if file pointer is at end of file.
//  Return value : 0 - Not end of file
//                 3 - End of file
//                 4 - Error moving file pointer
//
//*****************************************************************************
BYTE CPS_BUAP_checkEndOfFile()
{
	DWORD currPos;
	DWORD endPos;
	
	// ------------------------------------------------------------------------
	// Get current position and the end position of the file.
	// ------------------------------------------------------------------------
	currPos = _lseek(filep, 0L, SEEK_CUR);
	endPos  = _lseek(filep, 0L, SEEK_END);
	
	if ((currPos == -1L) || (endPos == -1L))
	{
		RepEvent(g_bufilelog, EVENTLOG_ERROR_TYPE, "Invalid file pointer");
		DBGLOG((CPS_syslog::LVLDEBUG, "bufile: file pointer move error. errno: %d.", errno));
		return CPS_BUAP_BUFILE_MOVE_ERROR;
	}
	
	// ------------------------------------------------------------------------
	// Compare the current position and the end position.
	// ------------------------------------------------------------------------
	if (currPos == endPos)
	{
		return CPS_BUAP_BUFILE_EOF;
	}
	else
	{
		// --------------------------------------------------------------------
		// Set the file pointer back to 'current' position.
		// --------------------------------------------------------------------
		_lseek(filep, (long) currPos, SEEK_SET);
		return CPS_BUAP_BUFILE_NOT_EOF;
	}
}
