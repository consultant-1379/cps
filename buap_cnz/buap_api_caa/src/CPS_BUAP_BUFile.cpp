//  SIO P1A
//*****************************************************************************
//
// NAME    CPS_BUAP_BUFile.cpp
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
// 	Interface to _read AXE reload files. The file object will
//	manage that the correct physical file is chosen, reserved
//	from CPF.

// DOCUMENT NO
//	CXA 110 4958

// AUTHOR 
// 	Fri May 23 12:41:14 1997 by UAB/I/T uabfln
//

// CHANGES
// 1999-06-23  uablan   Convert into NT code and remove all tape handling and
//                      make it a dll with functions (no class any more)
// 2000-02-15  qabgill  Misc. fixes & changes
// 2010-12-15  xchihoa  Port to Linux for APG43L
// 2011-08-08  xquydao  Updated for APG43L

// SEE ALSO 
// 	-
//
//*****************************************************************************
#include "CPS_BUAP_BUFile.h"
#include "CPS_BUAP_Types.h"

#include "fms_cpf_file.h"

#include "CPS_BUAP_LoaderClient.h"
#include "trace.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <cstring>
#include <cassert>
#include <cstdio>
#include <cerrno>


CPS_BUAP_BUFile::CPS_BUAP_BUFile() : m_fileFD(-1)
{
    newTRACE(("CPS_BUAP_BUFile::CPS_BUAP_BUFile()", 0));
    m_fileName[0] = '\0';
    m_lastErrorText[0] = '\0';
}

CPS_BUAP_BUFile::~CPS_BUAP_BUFile()
{
    newTRACE(("CPS_BUAP_BUFile::~CPS_BUAP_BUFile()", 0));
}

//*****************************************************************************
// Calculate correct file number, reserve and attach
// to the reload file.
//
// Return values: CPS_BUAP_BUFILE_OK
//                CPS_BUAP_BUFILE_NO_FILE
//*****************************************************************************
CPS_BUAP_BUFILE_RETCODE CPS_BUAP_BUFile::attachFile(uint16_t fileNumber)
{
    newTRACE(("CPS_BUAP_BUFile::attachFile(%u)", 0, fileNumber));

    m_fileFD = -1;
    m_fileName[0] = '\0';

    //-------------------------------------------------------------------------
    // Check file number, 255 is unique and ok, odd numbers not allowed and
    // even numbers must be cut in half (LAB send the fileNumber*2 or 255).
    // ------------------------------------------------------------------------
    if (fileNumber != 255) // 255 most common, go on unchanged
    {
        if ((fileNumber > 255) || (fileNumber % 2 == 1)) // Too big or odd number
        {
            TRACE(("File number %u is invalid.", 0, fileNumber));
            sprintf(m_lastErrorText, "BUFile: Can't attach due to invalid file number %u.", fileNumber);

            return CPS_BUAP_BUFILE_NO_FILE;
        }
        //---------------------------------------------------------------------
        // Cut number in half
        //---------------------------------------------------------------------
        fileNumber /= 2;
    }

    //-------------------------------------------------------------------------
    // Initiate the JTP connection to BULOADER.
    // ------------------------------------------------------------------------
    if (!m_buloader.connect())
    {
        TRACE(("Could not connect to buloader server.!!!", 0, fileNumber));
        sprintf(m_lastErrorText, "BUFile: Could not connect to buloader server. Detail: %s", m_buloader.getLastErrorText());

        return CPS_BUAP_BUFILE_READ_ERROR;
    }

    try
    {
        //----------------------------------------------------------------------
        // Get real RELFSWname.
        //----------------------------------------------------------------------
        sprintf(m_fileName, "RELFSW%i", fileNumber);

        if (!m_buloader.getFileName(m_fileName))
        {
            TRACE(("Could not get real RELFSW name.!!!", 0));
            sprintf(m_lastErrorText, "BUFile: Could not get real RELFSW name. Detail: %s", m_buloader.getLastErrorText());

            throw CPS_BUAP_BUFILE_READ_ERROR;
        }

        assert(strlen(m_fileName) > 6);

        TRACE(("BU File name transformed. New name: %s", 0, m_fileName));
    }
    catch (CPS_BUAP_BUFILE_RETCODE error)
    {
        m_buloader.disconnect();

        m_fileName[0] = '\0';
        return error;
    }

    return CPS_BUAP_BUFILE_OK;
}

//*****************************************************************************
// Close current subfile and un-reserve the whole
// reload file (all subfiles).
//
// Return values: CPS_BUAP_BUFILE_OK
//*****************************************************************************
CPS_BUAP_BUFILE_RETCODE CPS_BUAP_BUFile::detachFile()
{
    newTRACE(("CPS_BUAP_BUFile::detachFile()", 0));

    // ------------------------------------------------------------------------
    // Close file
    // ------------------------------------------------------------------------
    if (m_fileFD != -1)
    {
        close(m_fileFD);
        m_fileFD = -1;
    }

    // ------------------------------------------------------------------------
    // Unreserve subfile
    // ------------------------------------------------------------------------
    if (m_cpfSubfile.isReserved())
    {
        try {
             m_cpfSubfile.unreserve();
        }
        catch (FMS_CPF_Exception& ex)
        {
            TRACE(("CPF error %d, %s.", 0, ex.errorCode(), ex.errorText()));
            sprintf(m_lastErrorText, "BUFile: Failed to unreserve file. Detail: %s", ex.errorText());

            switch (ex.errorCode())
            {
            case FMS_CPF_Exception::FILENOTFOUND:
            case FMS_CPF_Exception::INVALIDFILE:
                return CPS_BUAP_BUFILE_NO_FILE;
                break;

            case FMS_CPF_Exception::ACCESSERROR:
            case FMS_CPF_Exception::FILEISOPEN:
            case FMS_CPF_Exception::INTERNALERROR:
            case FMS_CPF_Exception::PHYSICALERROR:
            case FMS_CPF_Exception::SOCKETERROR:
            default:
                return CPS_BUAP_BUFILE_READ_ERROR;
                break;
            }
        }
    }

    // ------------------------------------------------------------------------
    // Disconnect from buloader
    // ------------------------------------------------------------------------
    if (m_buloader.isConnected())
    {
        m_buloader.disconnect();
    }

    m_fileName[0] = '\0';

    return CPS_BUAP_BUFILE_OK;
}

//*****************************************************************************
// Open a reload subfile and reserves the reload subfile.
//
// Return values: CPS_BUAP_BUFILE_OK
//                CPS_BUAP_BUFILE_NO_FILE
//                CPS_BUAP_BUFILE_READ_ERROR
//*****************************************************************************
CPS_BUAP_BUFILE_RETCODE CPS_BUAP_BUFile::openSubfile(uint16_t subfile)
{
    newTRACE(("CPS_BUAP_BUFile::openSubfile(%u)", 0, subfile));

    // ------------------------------------------------------------------------
    // Check if file already opened
    // ------------------------------------------------------------------------
    if (m_fileFD != -1)
    {
        TRACE(("File already opened!!!", 0));
        sprintf(m_lastErrorText, "BUFile: Subfile %u already opened.", subfile);

        return CPS_BUAP_BUFILE_READ_ERROR;
    }

    // ------------------------------------------------------------------------
    // Create the complete filename with subfile
    // ------------------------------------------------------------------------
    char fullFileName[256];
    sprintf(fullFileName, "%s-R%c", m_fileName, '0' + subfile);

    TRACE(("Full filename is: %s", 0, fullFileName));

    try
    {
        // --------------------------------------------------------------------
        //  Apply name to CPFS
        // --------------------------------------------------------------------
        m_cpfSubfile = FMS_CPF_File(fullFileName);

        // --------------------------------------------------------------------
        // Get the physical path for the subfile from CPFS
        // --------------------------------------------------------------------
        try
        {
            m_cpfSubfile.reserve(FMS_CPF_Types::R_XW_);
            TRACE(("Reserved: %s", 0, fullFileName));

            strncpy(fullFileName, m_cpfSubfile.getPhysicalPath().c_str(), sizeof(fullFileName) - 1);
            fullFileName[sizeof(fullFileName) - 1] = '\0';

            TRACE(("Physical filename: %s", 0, fullFileName));
        }
        catch (FMS_CPF_Exception& ex)
        {
            TRACE(("CPF error %d, %s.", 0, ex.errorCode(), ex.errorText()));
            sprintf(m_lastErrorText, "BUFile: Failed to reserve file %s. Detail: %s", fullFileName, ex.errorText());

            switch (ex.errorCode())
            {
            case FMS_CPF_Exception::FILENOTFOUND:
            case FMS_CPF_Exception::INVALIDFILE:
                throw CPS_BUAP_BUFILE_NO_FILE;
                break;

            case FMS_CPF_Exception::ACCESSERROR:
            case FMS_CPF_Exception::FILEISOPEN:
            case FMS_CPF_Exception::INTERNALERROR:
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
        m_fileFD = open(fullFileName, O_RDONLY);
        if (m_fileFD == -1)
        {
            TRACE(("Physical file open failed: ErrNo: %d", 0, errno));
            sprintf(m_lastErrorText, "BUFile: Failed to open file %s. Detail: %s", fullFileName, strerror(errno));

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
            uint8_t li;
            R0Info data;
            memset(&data, 0, sizeof(R0Info));

            // ----------------------------------------------------------------
            // Get the loadingIndicator who tells which subfile to load
            // and check that the loadingIndicator has a valid value
            // (2,4,8 or 16).
            //-----------------------------------------------------------------
            lseek(m_fileFD, offsetof(struct controlInfoSector, loadingIndicator), SEEK_SET);
            read(m_fileFD, &li, 1);

            if (!(((li & 2) ^ (li & 4)) && ((li & 8) ^ (li & 16)))) // Sanity check
            {
                TRACE(("R0-File read error for loading indicator. fn: %s. res:%d", 0, fullFileName, li));
                sprintf(m_lastErrorText, "BUFile: Failed to Sanity check file %s. res: %d", fullFileName, li);

                throw CPS_BUAP_BUFILE_READ_ERROR;
            }

            // ----------------------------------------------------------------
            // Get the APZ type
            // ----------------------------------------------------------------
            lseek(m_fileFD, offsetof(struct controlInfoSector, APZType), SEEK_SET);
            read(m_fileFD, &(data.APZtype), sizeof(data.APZtype));

            //-----------------------------------------------------------------
            // loadingIndicator is R4 (li & <|0|0|R0|R1|R2|R3|*R4|R5|>)
            // Get output time for R4
            //-----------------------------------------------------------------
            if (li & 2)
            {
                lseek(m_fileFD, offsetof(struct controlInfoSector, cmdLogSubFileR4), SEEK_SET);
                read(m_fileFD, &(data.subfile), sizeof(data.subfile));

                lseek(m_fileFD, offsetof(struct R0File, OTS) + offsetof(struct outputTimesSector, r4), SEEK_SET);
                read(m_fileFD, &(data.ldd), sizeof(data.ldd));
            }

            //-----------------------------------------------------------------
            // loadingIndicator is R3 (li & <|0|0|R0|R1|R2|*R3|R4|R5|>)
            // Get output time for R3
            //-----------------------------------------------------------------
            if (li & 4)
            {
                lseek(m_fileFD, offsetof(struct controlInfoSector, cmdLogSubFileR3), SEEK_SET);
                read(m_fileFD, &(data.subfile), sizeof(data.subfile));

                lseek(m_fileFD, offsetof(struct R0File, OTS) + offsetof(struct outputTimesSector, r3), SEEK_SET);
                read(m_fileFD, &(data.ldd), sizeof(data.ldd));
            }

            //-----------------------------------------------------------------
            // loadingIndicator is R2 (li & <|0|0|R0|R1|*R2|R3|R4|R5|>)
            // Get output time for R2
            //-----------------------------------------------------------------
            if (li & 8)
            {
                lseek(m_fileFD, offsetof(struct R0File, OTS) + offsetof(struct outputTimesSector, r2), SEEK_SET);
                read(m_fileFD, &(data.sdd), sizeof(data.sdd));
            }

            //-----------------------------------------------------------------
            // loadingIndicator is R1 (li & <|0|0|R0|*R1|R2|R3|R4|R5|>)
            // Get output time for R1
            //-----------------------------------------------------------------
            if (li & 16)
            {
                lseek(m_fileFD, offsetof(struct R0File, OTS) + offsetof(struct outputTimesSector, r1), SEEK_SET);
                read(m_fileFD, &(data.sdd), sizeof(data.sdd));
            }

            //-----------------------------------------------------------------
            // Get output time for R5
            //-----------------------------------------------------------------
            lseek(m_fileFD, offsetof(struct R0File, OTS) + offsetof(struct outputTimesSector, r5), SEEK_SET);
            read(m_fileFD, &(data.rpsrs), sizeof(data.rpsrs));

            //-----------------------------------------------------------------
            // Order writing of the backup generation number
            // and cmdlog information into reladmpar
            //-----------------------------------------------------------------
            TRACE(("Sending R0 data to server, cmdlog: %ld", 0, data.subfile));

            if (!m_buloader.reportCmdLog(data))
            {
                sprintf(m_lastErrorText, "BUFile: Failed to report command log on file %s. Detail: %s",
                        fullFileName, m_buloader.getLastErrorText());

                throw CPS_BUAP_BUFILE_READ_ERROR;
            }
        }
    }
    catch (CPS_BUAP_BUFILE_RETCODE error)
    {
        //----------------------------------
        // Here is the thrown errors catched
        // This is fatal - we can't continue with the file batch
        //----------------------------------
        this->detachFile();

        return error;
    }

    //-------------------------------------------------------------------------
    // Set file pointer to point at start of file
    //-------------------------------------------------------------------------
    lseek(m_fileFD, 0, SEEK_SET);    return CPS_BUAP_BUFILE_OK;
}

//*****************************************************************************
//  Unreserve & close a reload subfile.
//
//  Return values: CPS_BUAP_BUFILE_OK
//                 CPS_BUAP_BUFILE_NO_FILE
//                 CPS_BUAP_BUFILE_READ_ERROR
//*****************************************************************************
CPS_BUAP_BUFILE_RETCODE CPS_BUAP_BUFile::closeSubfile()
{
    newTRACE(("CPS_BUAP_BUFile::closeSubfile()", 0));

    assert(m_fileFD > -1);
    // Close the actual file
    // ---------------------
    if (m_fileFD != -1)
    {
        close(m_fileFD);
        m_fileFD = -1;
    }

    try
    {
        assert(m_cpfSubfile.isReserved());
        // --------------------------------------------------------------------
        // unreserve the file
        // --------------------------------------------------------------------
        if (m_cpfSubfile.isReserved())
        {
            m_cpfSubfile.unreserve();
        }
        assert(!m_cpfSubfile.isReserved());
    }
    catch (FMS_CPF_Exception& ex)
    {
        TRACE(("bufile: CPF error %d, %s.", 0, ex.errorCode(), ex.errorText()));
        sprintf(m_lastErrorText, "BUFile: Failed to unreserve file %s. Detail: %s", m_cpfSubfile.getCPFname(), ex.errorText());

        switch (ex.errorCode())
        {
        case FMS_CPF_Exception::INVALIDREF:
            return CPS_BUAP_BUFILE_NO_RESERVE;
            break;

        case FMS_CPF_Exception::INTERNALERROR:
        case FMS_CPF_Exception::SOCKETERROR:
        default:
            return CPS_BUAP_BUFILE_READ_ERROR;
            break;
        }
    }

    return CPS_BUAP_BUFILE_OK;
}

//*****************************************************************************
// Reads data from the file into the block pointed
// to by the user.
//
// Return values: CPS_BUAP_BUFILE_OK
//                CPS_BUAP_BUFILE_READ_ERROR
//                CPS_BUAP_BUFILE_EOF
//*****************************************************************************
CPS_BUAP_BUFILE_RETCODE CPS_BUAP_BUFile::readSubfile(char* buf, uint32_t bufSize, uint32_t& numOfBytes)
{
    // Should not have trace for this function, otherwise the trautil will be full
    // by many duplicate trace of this function
    //newTRACE(("CPS_BUAP_BUFile::readSubfile()", 0));
    CPS_BUAP_BUFILE_RETCODE rc = CPS_BUAP_BUFILE_OK;

    // ------------------------------------------------------------------------
    // Do not make a _read if bufSize is zero.
    // ------------------------------------------------------------------------
    if (bufSize == 0)
    {
        {
        newTRACE(("Buffer size to read is 0", 0));
        }
        sprintf(m_lastErrorText, "BUFile: read data with 0 byte buffer - file %s.", m_cpfSubfile.getCPFname());

        return CPS_BUAP_BUFILE_READ_ERROR;
    }

    // ------------------------------------------------------------------------
    // Read from file into buffer.
    // ------------------------------------------------------------------------
    ssize_t num = read(m_fileFD, buf, bufSize);

    switch (num)
    {
    case -1:
        // ----------------------------------------------------------------
        // File read error
        // ----------------------------------------------------------------
        {
        newTRACE(("File read() error no: %d.", 0, errno));
        }
        sprintf(m_lastErrorText, "BUFile: Failed to read file %s. Error: %s", m_cpfSubfile.getCPFname(), strerror(errno));

        rc = CPS_BUAP_BUFILE_READ_ERROR;
        break;

    case 0:
        // ----------------------------------------------------------------
        // File descriptor points at EOF
        // ----------------------------------------------------------------
        {
        newTRACE(("EOF from read.", 0));
        }
        rc = CPS_BUAP_BUFILE_EOF;
        break;

    default:
        // ----------------------------------------------------------------
        // Normal case
        // ----------------------------------------------------------------
        numOfBytes = num;
        rc = CPS_BUAP_BUFILE_OK;
        break;
    }

    return rc;
}

//*****************************************************************************
// Moves the file pointer in the opened reload subfile.
//
// Return values: CPS_BUAP_BUFILE_OK
//                CPS_BUAP_BUFILE_READ_ERROR
//*****************************************************************************
CPS_BUAP_BUFILE_RETCODE CPS_BUAP_BUFile::moveSubfilePtr(uint32_t offset)
{
    // Mark out this trace due to it's called in a loop
    //newTRACE(("CPS_BUAP_moveSubfilePtr(%lu)", 0, offset));

    off_t currPos, newPos;

    // ------------------------------------------------------------------------
    // Get current position.
    // ------------------------------------------------------------------------
    currPos = lseek(m_fileFD, 0, SEEK_CUR);

    // ------------------------------------------------------------------------
    // Move the file pointer 'offset' position forward from current position.
    // ------------------------------------------------------------------------
    newPos = lseek(m_fileFD, offset, SEEK_SET);

    switch (newPos)
    {
    case -1:
    {
        newTRACE(("File pointer handle invalid!!!", 0));
        sprintf(m_lastErrorText, "%s", "BUFile: Failed to move sub file pointer. Error: invalid FD");

        return CPS_BUAP_BUFILE_READ_ERROR;
        break;
    }

    default:
        if (newPos != offset)
        {
            newTRACE(("File pointer not move requested offset!!!", 0));
            sprintf(m_lastErrorText, "BUFile: File %s pointer not move to requested offset %u", m_cpfSubfile.getCPFname(), offset);
            return CPS_BUAP_BUFILE_READ_ERROR;
        }

        break;
    }

    return CPS_BUAP_BUFILE_OK;
}

//*****************************************************************************
// Checks if file pointer is at end of file.
//
// Return values: CPS_BUAP_BUFILE_NOT_EOF
//                CPS_BUAP_BUFILE_EOF
//                CPS_BUAP_BUFILE_MOVE_ERROR
//*****************************************************************************
CPS_BUAP_BUFILE_RETCODE CPS_BUAP_BUFile::checkEndOfFile()
{
    newTRACE(("CPS_BUAP_BUFile::checkEndOfFile()", 0));

    off_t currPos;
    off_t endPos;

    // ------------------------------------------------------------------------
    // Get current position and the end position of the file.
    // ------------------------------------------------------------------------
    currPos = lseek(m_fileFD, 0, SEEK_CUR);
    endPos = lseek(m_fileFD, 0, SEEK_END);

    if ((currPos == -1) || (endPos == -1))
    {
        TRACE(("File pointer move error!!! error no: %d.", 0, errno));
        sprintf(m_lastErrorText, "BUFile: File %s pointer move error: %s", m_cpfSubfile.getCPFname(), strerror(errno));
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
        lseek(m_fileFD, currPos, SEEK_SET);
        return CPS_BUAP_BUFILE_NOT_EOF;
    }
}

//*****************************************************************************
// Obtain last error text info
//
// Return values: char*  Last error text
//*****************************************************************************
const char* CPS_BUAP_BUFile::getLastErrorText()
{
    return m_lastErrorText;
}

//*****************************************************************************
// Obtain the real attached file name
//
// Return values: char*  file name
// Note: this method should only be called after the SUCCESSFUL attached
//*****************************************************************************
const char* CPS_BUAP_BUFile::getAttachedFileName()
{
    return m_fileName;
}
