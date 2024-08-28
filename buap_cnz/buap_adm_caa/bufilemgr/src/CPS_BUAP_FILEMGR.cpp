//******************************************************************************
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 2011.
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
//  CPS_BUAP_FILEMGR.cpp
//
//  DESCRIPTION 
//  This class, which inherits from CPS_BUAP_Server,
//  implements the controlling logic for the bufilemgr process.
//
//  DOCUMENT NO
//    190 89-CAA 109 1414
//
//  AUTHOR 
//     1999/05/13 by UAB/B/SF Birgit Berggren
//
//******************************************************************************
// === Revision history ===
// 990513 BIR  PA1 Created.
// 990607 BIR  PA2 Updated.
// 990617 BIR  PA3 Updated after review.
// 050207 LAN      At switch GetTime creates a DateTime object without hour and
//                 minute. Correct that. TR hf47251
// 110912 DANH     Updated (migrated from Win2k3 to Linux).
//******************************************************************************


#include "CPS_BUAP_FILEMGR.h"
#include "CPS_BUAP_MSG_IDs.h"
#include "CPS_BUAP_Trace.h"
#include "CPS_BUAP_Events.h"
#include "CPS_BUAP_Server.h"
#include "CPS_BUAP_DateTime.h"
#include "CPS_BUAP_Types.h"
#include "CPS_BUAP_Service.h"

#include "fms_cpf_file.h"

#include <stddef.h>
#include <string.h>
#include <boost/bind.hpp>
#include <boost/thread.hpp>


const char ORDERED[] = "ORDERED";
const char NOT_EXECUTED[] = "COMMAND NOT EXECUTED";
const char EXECUTED[] = "COMMAND EXECUTED";
const char FAULT_CODE[] = "FAULT CODE";
const char END[] = "END";

const int NFILE = -1;
const int FFRfirst = 0;
const int FFRlast = 99;
const int SFRfirst = 100;
const int SFRlast = 127;

const int FirstSubfile = 0;
const int LastSubfile = 5;

#define RELFSW                  "RELFSW"

using namespace CPS_BUAP;

//=============================================================================
// Extern variable used to report error.
//=============================================================================
extern CPS_BUAP_Service* pService;
extern bool reportErrorFlag;
extern boost::recursive_mutex mutex;


//******************************************************************************
//    CPS_BUAP_FILEMGR()
//******************************************************************************
CPS_BUAP_FILEMGR::CPS_BUAP_FILEMGR():
        m_FaultCode(0), m_CmdResult(IDLE), m_pService(0),
        m_running(false)
{
	// Change for Tracing Order
   //newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_FILEMGR::CPS_BUAP_FILEMGR()", 0));

   // Assign the derive name
   m_deriveName = string("FILEMGR");

   //**************************************************************************
   // Initiate the array of thread control objects.
   //***************************************************************************
   //TRACE(("initiating hNTObjectArray(BUFILEMGR)", 0))
   hNTObjectArray[ABORT_EVENT] = m_stopEvent.getFd(); // There's always a
   noOfNTHandles = 1;                                 // handle to the AbortEvent

   hNTObjectArray[COMMAND_THREAD] = 0; // No command is executing yet.

   m_Command[0] = '\0';
   m_CommandOrder[0] = '\0';

}

//******************************************************************************
//    ~CPS_BUAP_FILEMGR()
//******************************************************************************
CPS_BUAP_FILEMGR::~CPS_BUAP_FILEMGR()
{
	// Change for Tracing Order
   //newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_FILEMGR::~CPS_BUAP_FILEMGR()", 0))

   // Delete the pointer to JTP service objects
   if (m_pService)
   {
       delete m_pService;
       m_pService = 0;
   }

}

//******************************************************************************
//    RXTX()
//******************************************************************************
void CPS_BUAP_FILEMGR::RXTX(HANDLE handle, unsigned short rxUserData1,
      unsigned short rxUserData2, CPS_BUAP_Buffer &rxBuffer,
      unsigned short &txUserData1, unsigned short &txUserData2,
      CPS_BUAP_Buffer &txBuffer)
{
   newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_FILEMGR::RXTX(handle=%x, rxUserData1=%d, rxUserData2=%d)",
               0, handle, rxUserData1, rxUserData2))

   CPS_BUAP_MSG *txMsg;

   switch (rxUserData1)
   {
   case CPS_BUAP_MSG_ID_new_command:
   {
      CPS_BUAP_MSG_text rxMsg;
      rxMsg.UserData1() = rxUserData1;
      rxMsg.UserData2() = rxUserData2;
      rxMsg.Decode(rxBuffer);

      Process_new_command(handle, rxMsg, &txMsg);
   }
      break;

   case CPS_BUAP_MSG_ID_new_buffer:
   {
      Process_new_buffer(&txMsg);
   }
      break;

   default:
      EVENT((CPS_BUAP_Events::invalid_message, 0, rxUserData1))
      THROW("Unknown message type received");
      break;
   }

   txMsg->Encode(txBuffer);
   txUserData1 = txMsg->UserData1();
   txUserData2 = txMsg->UserData2();
}

//******************************************************************************
//    Process_new_command()
//******************************************************************************
void CPS_BUAP_FILEMGR::Process_new_command(HANDLE handle, CPS_BUAP_MSG_text &rxMsg,
                                           CPS_BUAP_MSG **txMsg)
{
   newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_FILEMGR::Process_new_command(%d)", 0, handle))

   static CPS_BUAP_MSG_text l_txMsg;
   char printout[256];

   TRACE((LOG_LEVEL_INFO, "Command received <%s>", 0, rxMsg.m_Text()))

   l_txMsg.UserData1() = CPS_BUAP_MSG_ID_ordered;
   l_txMsg.UserData2() = 1;

   TRACE(("CPS_BUAP_FILEMGR_ CMD_THREAD handle: %x", 0, hNTObjectArray[COMMAND_THREAD]));
   if (hNTObjectArray[COMMAND_THREAD] == 0)
   {
      //***************************************************************************
      // Only one command at a time will be executed.
      //
      // A SYBII is ordered if the CP thinks that a SYTUC has been interrupted.
      // The only thing that the CP knows is that the JTP connection has been
      // disconnected during a SYTUC. If the handle for the command thread has
      // a value /= 0, SYTUC has not been interrupted and is still being
      // processed and there's no need for a SYBII.
      //***************************************************************************

      TRACE(("rxMsg.m_Text: %s", 0, rxMsg.m_Text()));

      rxMsg.m_Text >> m_CommandOrder;

      TRACE(("m_CommandOrder: %s", 0,m_CommandOrder));

      rxMsg.m_Text >> m_Command;

      TRACE(("m_Command: %s", 0,m_Command));

      m_Command[5] = 0;

      //****************************************************************************
      // Create a thread that will execute the command.
      //****************************************************************************

#if 0
      m_commandThreadEvent = eventfd(0, 0);
      if (m_commandThreadEvent == -1)
      {
          TRACE((LOG_LEVEL_ERROR, "Create event failed!!!", 0));
          //return EXIT_FAIfLURE;
          return; 
      }
#endif

      TRACE(("Create new thread to process new command %s", 0, m_CommandOrder ));

      boost::thread cmdThread(boost::bind(&CPS_BUAP_FILEMGR::Process_command, this, m_CommandOrder));

      // Store the command event FD into the object array
      m_commandThreadEvent.resetEvent();
      hNTObjectArray[COMMAND_THREAD] = m_commandThreadEvent.getFd();
      noOfNTHandles++;

      m_CmdResult = COMMAND_ORDERED;

   }

   std::sprintf(printout, "%s\n", ORDERED);

   l_txMsg.m_Text << printout;
   *txMsg = &l_txMsg;

}


//******************************************************************************
//    Process_command()
//******************************************************************************
bool CPS_BUAP_FILEMGR::Process_command(const char command[])
{
   newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_FILEMGR::Process_command(%s)", 0, command))

   m_FaultCode = 0; // OK

   // Check the ordered command and proceed
   if (::strcmp(command, "SYNIC;") == 0)
   {
      this->Process_SYNIC(FFRfirst, FFRlast, RELFSW "X");
   }
   else if (::strcmp(command, "SYNIC:SFR;") == 0)
   {
      this->Process_SYNIC(SFRfirst, SFRlast, RELFSW "Y");
   }
   else if (::strcmp(command, "SYTUC;") == 0)
   {
      this->Process_SYTUC(FFRfirst, FFRlast, RELFSW "X");
   }
   else if (::strcmp(command, "SYTUC:SFR;") == 0)
   {
      this->Process_SYTUC(SFRfirst, SFRlast, RELFSW "Y");
   }
   else if (::strcmp(command, "SYBII;") == 0)
   {
      this->Process_SYBII();
   }
   else if (::strcmp(command, "SYSFT;") == 0)
   {
      this->Process_SYSFT();
   }
   else
   {
      EVENT((CPS_BUAP_Events::no_such_command, 0, command))

   }

   TRACE ((LOG_LEVEL_INFO, "Command finished : set event notification %d", 0,
                                   m_commandThreadEvent.getFd()));
   m_commandThreadEvent.setEvent();

   return true;
}

//******************************************************************************
//    Process_SYNIC()
//******************************************************************************
void CPS_BUAP_FILEMGR::Process_SYNIC(const int low, const int high,
      const char tempName[])
{
   newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_FILEMGR::Process_SYNIC(%d, %d, %s)", 0, low, high, tempName))

   int gennum;
   int highest = NOFILE;
   char filename[16];
   DateTime lowTime;
   DateTime highestTime;
   FMS_CPF_File *lowFile = 0;
   FMS_CPF_File *highestFile = 0;

   enum FAULT_CODE
   //***************************************************************************
   // The fault code is sent to the CP in case of an unsuccessful command
   // execution. The fault codes correspond to the faults that can occur in
   // a Support Processor since the operator interface hasn't been changed
   // for for the Adjunct Processor.
   // The meaning of every fault code is explained in the Command Description
   // for SYNIC.
   //****************************************************************************
   {
      unsuccessful = 50,
      fileAges = 51,
      insufficientGenerations = 52,
      subfilesMissing = 53,
      fileInExclusiveUse = 54,
      hardDiscAccessError = 55,
      tempFileExists = 58
   };

   try
   {

      // Find highest consecutive file

      for (gennum = low; gennum <= high; gennum++)
      {
         std::sprintf(filename, RELFSW "%d", gennum);
         FMS_CPF_File file(filename);

         if (!file.exists())
         {
            break;
         }
         highest = gennum;
      }

      if (highest < low + 1)
      {
         throw FAULT_CODE(insufficientGenerations);
      }

      // CHECK IF RELFSWX/Y exists 
      {
         FMS_CPF_File tempfile(tempName);

         if (tempfile.exists())
         {
            throw FAULT_CODE(tempFileExists);
         }
      }

      std::sprintf(filename, RELFSW "%d", low);
      lowFile = new FMS_CPF_File(filename);

      TRACE((LOG_LEVEL_INFO, "CPS_BUAP_FILEMGR::Process_SYNIC with lowfile = RELFSW%d", 0, low));

      std::sprintf(filename, RELFSW "%d", highest);
      highestFile = new FMS_CPF_File(filename);

      TRACE((LOG_LEVEL_INFO, "CPS_BUAP_FILEMGR::Process_SYNIC with highestfile = RELFSW%d", 0, highest));

      try
      {
         lowFile->reserve(FMS_CPF_Types::XR_XW_);
         highestFile->reserve(FMS_CPF_Types::XR_XW_);
      } catch (...)
      {
         throw FAULT_CODE(fileInExclusiveUse);
      }

      // CHECK SUBFILES FOR HIGHEST
      for (int subfilenum = FirstSubfile; subfilenum <= LastSubfile; subfilenum++)
      {
         char subfilename[16];
         std::sprintf(subfilename, RELFSW "%d-R%d", highest, subfilenum);

         FMS_CPF_File subfile(subfilename);

         if (!subfile.exists())
         {
            throw FAULT_CODE(subfilesMissing);
         }
      }

      //    
      // CHECK TIMES but only if low R0 exists
      //    
      char subfilename[16];
      std::sprintf(subfilename, RELFSW "%d-R0", low);

      FMS_CPF_File subfile(subfilename);

      if (subfile.exists())
      {
         try
         {

            GetTime(lowFile->getPhysicalPath(), lowTime);
            GetTime(highestFile->getPhysicalPath(), highestTime);

         } catch (...)
         {
            throw FAULT_CODE(hardDiscAccessError);
         }

         if (highestTime < lowTime)
         {
            throw FAULT_CODE(fileAges);
         }
      }

      // RELFSW0/100  -> RELFSWX/Y
      TRACE(("rename %s -> %s", 0, lowFile->getCPFname(), tempName))

      lowFile->rename(tempName);

      // RELFSWn      -> RELFSW0/100
      std::sprintf(filename, RELFSW "%d", low);
      TRACE(("rename %s -> %s", 0,highestFile->getCPFname(), filename))

      highestFile->rename(filename);

      // RELFSWX/Y    -> RELFSWn
      std::sprintf(filename, RELFSW "%d", highest);
      TRACE(("rename %s -> %s", 0,lowFile->getCPFname(), filename))

      lowFile->reserve(FMS_CPF_Types::XR_XW_);
      lowFile->rename(filename);

   } catch (FMS_CPF_Exception& EE) //:ag1
   {
      m_FaultCode = 50;
      TRACE((LOG_LEVEL_ERROR, "FMS_CPF_Exception", 0))
      TRACE((LOG_LEVEL_ERROR, "   errorType: %d", 0, EE.errorCode()))
      TRACE((LOG_LEVEL_ERROR, "   errorText: %s", 0, EE.errorText()))
      TRACE((LOG_LEVEL_ERROR, "   errorInfo: %s", 0, EE.detailInfo().c_str()))
   } catch (FAULT_CODE &FaultCode)
   {
      m_FaultCode = FaultCode;
   } catch (...)
   {
      m_FaultCode = FAULT_CODE(unsuccessful);
   }
   if (lowFile)
   {
      if (lowFile->isReserved())
      {
         lowFile->unreserve();
      }
      delete lowFile;
   }
   if (highestFile)
   {
      if (highestFile->isReserved())
      {
         highestFile->unreserve();
      }
      delete highestFile;
   }
}

//******************************************************************************
//    Process_SYTUC()
//******************************************************************************
void CPS_BUAP_FILEMGR::Process_SYTUC(const int low, const int high, const char tempName[])
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_FILEMGR::Process_SYTUC(%d, %d, %s)", 0, low, high, tempName));

    int gennum;
    //int highest = NOFILE;
    bool _flag = false;
    int highest = 0;
    FMS_CPF_File *file[SFRlast + 1];
    char filename[16];
    char subfilename[16];
    DateTime lowTime;
    DateTime highestTime;

    enum FAULT_CODE
    //***************************************************************************
    // The fault code is sent to the CP in case of an unsuccessful command
    // execution. The fault codes correspond to the faults that can occur in
    // a Support Processor since the operator interface hasn't been changed
    // for for the Adjunct Processor.
    // The meaning of every fault code is explained in the Command Description
    // for SYTUC.
    //****************************************************************************
    {
       unsuccessful = 50,
       fileAges = 51,
       insufficientGenerations = 52,
       subfilesMissing = 53,
       fileInExclusiveUse = 54,
       hardDiscAccessError = 55,
       missingLowR0 = 57,
       tempFileExists = 58
    };

    try
    {

       // Find highest consecutive file, reserving each file
       // in the consecutive range

       try
       {
          for (gennum = low; gennum <= high; gennum++)
          {
             std::sprintf(filename, RELFSW "%d", gennum);
             file[gennum] = new FMS_CPF_File(filename);

             if (!file[gennum]->exists())
             {
                delete file[gennum];
                file[gennum] = 0;
                break;
             }
             _flag = true;
             highest = gennum;
             file[gennum]->reserve(FMS_CPF_Types::XR_XW_);
          }
       } catch (...)
       {
          throw FAULT_CODE(fileInExclusiveUse);
       }

       if ( _flag && (highest < low + 1))
       {
          throw FAULT_CODE(insufficientGenerations);
       }

       // CHECK IF RELFSWX/Y exists
       {
          FMS_CPF_File tempfile(tempName);

          if (tempfile.exists())
          {
             throw FAULT_CODE(tempFileExists);
          }
       }

       //
       // NEED TO CHECK TIMES HERE
       //
       try
       {
          if (file[low] == 0)
          {
             std::sprintf(filename, RELFSW "%d", low);
             file[low] = new FMS_CPF_File(filename);
          }
          GetTime(file[low]->getPhysicalPath(), lowTime);

       } catch (...)
       {
          throw FAULT_CODE(missingLowR0);
       }

       try
       {

          GetTime(file[highest]->getPhysicalPath(), highestTime);

       } catch (...)
       {
          throw FAULT_CODE(subfilesMissing);
       }

       if (highestTime < lowTime)
       {
          throw FAULT_CODE(fileAges);
       }

       // Check all subfiles in range
       for (gennum = low; gennum <= highest; gennum++)
       {
          for (int subfilenum = FirstSubfile; subfilenum <= LastSubfile; subfilenum++)
          {
             std::sprintf(subfilename, RELFSW "%d-R%d", gennum, subfilenum);

             FMS_CPF_File subfile(subfilename);

             if (!subfile.exists())
             {
                throw FAULT_CODE(subfilesMissing);
             }
          }
       }

       //
       // RELFSW0/100 -> RELFSWX/Y
       //
       TRACE(("rename %s -> %s", 0, file[low]->getCPFname(), tempName))
       file[low]->rename(tempName);

       //
       // RELFSWn     -> RELFSW0/100
       //
       std::sprintf(filename, RELFSW "%d", low);
       TRACE(("rename %s -> %s", 0, file[highest]->getCPFname(), filename))
       file[highest]->rename(filename);

       //
       // RELFSWn-1   -> RELFSWn
       // RELFSWn-2   -> RELFSWn-1
       //         .........
       // RELFSW1/101 -> RELFSW2/102
       //
       for (gennum = highest - 1; gennum > low; gennum--)
       {
          std::sprintf(filename, RELFSW "%d", gennum + 1);
          TRACE(("rename %s -> %s", 0, file[gennum]->getCPFname(), filename))
          file[gennum]->rename(filename);
       }

       //
       // RELFSWX/Y   -> RELFSW1/101
       //
       std::sprintf(filename, RELFSW "%d", low + 1);
       TRACE(("rename %s -> %s", 0, file[low]->getCPFname(), filename))

       file[low]->reserve(FMS_CPF_Types::XR_XW_);
       file[low]->rename(filename);

       m_FaultCode = 0;

    } catch (FMS_CPF_Exception& EE) //:ag1
    {
       m_FaultCode = 50;
       TRACE((LOG_LEVEL_ERROR, "FMS_CPF_Exception", 0))
       TRACE((LOG_LEVEL_ERROR, "   errorType: %d", 0, EE.errorCode()))
       TRACE((LOG_LEVEL_ERROR, "   errorText: %s", 0, EE.errorText()))
       TRACE((LOG_LEVEL_ERROR, "   errorInfo: %s", 0, EE.detailInfo().c_str()))

    } catch (FAULT_CODE &FaultCode)
    {
       m_FaultCode = FaultCode;
       TRACE(("exception caught, m_FaultCode = %d", 0, m_FaultCode ))
    }

    for (gennum = low; gennum <= highest; gennum++)
    {
         if (file[gennum])
         {
             if (file[gennum]->isReserved())
             {
                 file[gennum]->unreserve();
             }
             delete file[gennum];
         }

    }
}

//******************************************************************************
//    Process_SYSFT()
//******************************************************************************
void CPS_BUAP_FILEMGR::Process_SYSFT()
{
   newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_FILEMGR::Process_SYSFT()", 0))

   FMS_CPF_File RELFSW0(RELFSW "0");
   FMS_CPF_File RELFSW1(RELFSW "1");
   FMS_CPF_File *destination = 0;
   char destfilename[16];
   char subfilename[16];

   enum FAULT_CODE
   //***************************************************************************
   // The fault code is sent to the CP in case of an unsuccessful command
   // execution. The fault codes correspond to the faults that can occur in
   // a Support Processor since the operator interface hasn't been changed
   // for for the Adjunct Processor.
   // The meaning of every fault code is explained in the Command Description
   // for SYSFT.
   //****************************************************************************
   {
      sourceUnavailable = 10,
      sourceMissing = 36,
      unsuccessful = 50,
      insufficientGenerations = 52,
      subfilesMissing = 53,
      fileInExclusiveUse = 54,
      hardDiscAccessError = 55
   };

   int i;
   bool exists;
   int highest = NOFILE;

   try
   {
      // Find highest consecutive file in the second file range
      // in the consecutive range

      for (i = SFRfirst; i <= SFRlast; i++)
      {
         std::sprintf(destfilename, RELFSW "%d", i);
         destination = new FMS_CPF_File(destfilename);

         exists = destination->exists();
         delete destination;
         destination = 0;

         if (!exists)
         {
            break;
         }

         highest = i;
      }

      if ((highest == NOFILE) || (!RELFSW0.exists()))
      {
         throw FAULT_CODE(insufficientGenerations);
      }

      if (!RELFSW1.exists())
      {
         throw FAULT_CODE(sourceMissing);
      }

      //
      // Check all subfiles are present in source
      //
      for (int subfilenum = FirstSubfile; subfilenum <= LastSubfile; subfilenum++)
      {
         std::sprintf(subfilename, RELFSW "1-R%d", subfilenum);

         FMS_CPF_File subfile(subfilename);

         if (!subfile.exists())
         {
            throw FAULT_CODE(subfilesMissing);
         }
      }

      std::sprintf(destfilename, RELFSW "%d", highest);

      try
      {
         //RELFSW1.reserve(FMS_CPF_Types::XR_XW_);

      } catch (...)
      {
         throw FAULT_CODE(fileInExclusiveUse);
      }

      TRACE((LOG_LEVEL_INFO, "copy %s -> %s", 0, RELFSW1.getCPFname(), destfilename));
      RELFSW1.copy(destfilename, FMS_CPF_Types::cm_OVERWRITE);
      //RELFSW1.copy(destfilename, FMS_CPF_Types::cm_NORMAL);

   } catch (FMS_CPF_Exception& EE) //:ag1
   {
      m_FaultCode = unsuccessful;
      TRACE((LOG_LEVEL_ERROR, "FMS_CPF_Exception", 0))
      TRACE((LOG_LEVEL_ERROR, "   errorType: %d", 0, EE.errorCode()))
      TRACE((LOG_LEVEL_ERROR, "   errorText: %s", 0, EE.errorText()))
      TRACE((LOG_LEVEL_ERROR, "   errorInfo: %s", 0, EE.detailInfo().c_str()))

   } catch (FAULT_CODE &FaultCode)
   {
      m_FaultCode = FaultCode;
   }

   if (RELFSW1.isReserved())
   {
      RELFSW1.unreserve();
   }

   if (destination)
   {
      if (destination->isReserved())
      {
         destination->unreserve();
      }
      delete destination;
   }
}

//******************************************************************************
//    Process_SYBII()
//******************************************************************************
void CPS_BUAP_FILEMGR::Process_SYBII()
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_FILEMGR::Process_SYBII()", 0))

    int low = 0;
    int high = 0;
    FMS_CPF_File *tempfile = 0;
    int gennum = 0;
    //int highest = NOFILE;
    int highest = 0;
    FMS_CPF_File *file[SFRlast + 1];
    char filename[16];

    enum FAULT_CODE
    {
       unsuccessful = 50,
       fileAges = 51,
       insufficientGenerations = 52,
       subfilesMissing = 53,
       fileInExclusiveUse = 54,
       hardDiscAccessError = 55,
       missingLowR0 = 57,
       tempFileExists = 58
    };

    for (gennum = FFRfirst; gennum <= SFRlast; gennum++)
       file[gennum] = 0;

    try
    {
       // Determine if recovering a SYTUC; or SYTUC:SFR;

       tempfile = new FMS_CPF_File(RELFSW "X");

       if (tempfile->exists())
       {
          TRACE((LOG_LEVEL_INFO, RELFSW "X exists", 0))
          // SYTUC in FFR
          low = FFRfirst;
          high = FFRlast;
          tempfile->reserve(FMS_CPF_Types::XR_XW_);
       }
       else
       {
          delete tempfile;
          tempfile = 0;

          tempfile = new FMS_CPF_File(RELFSW "Y");

          if (tempfile->exists())
          {
             TRACE((LOG_LEVEL_INFO, RELFSW "Y exists", 0))
             // SYTUC in SFR
             low = SFRfirst;
             high = SFRlast;
             tempfile->reserve(FMS_CPF_Types::XR_XW_);
          }
          else
          {
             delete tempfile;
             tempfile = 0;
          }
       }

       if (tempfile)
       {
          // A recovery must be made
          TRACE((LOG_LEVEL_INFO, "recovery must be made", 0))

          //
          // reserve all files upto the first gap
          //

          try
          {
             for (gennum = low + 1; gennum <= high; gennum++)
             {
                std::sprintf(filename, RELFSW "%d", gennum);
                file[gennum] = new FMS_CPF_File(filename);

                if (!file[gennum]->exists())
                {
                   delete file[gennum];
                   file[gennum] = 0;
                   break;
                }

                highest = gennum;
                TRACE(("reserving %s", 0, filename))
                file[gennum]->reserve(FMS_CPF_Types::XR_XW_);
             }
          } catch (...)
          {
             throw FAULT_CODE(fileInExclusiveUse);
          }

          std::sprintf(filename, RELFSW "%d", low);
          file[low] = new FMS_CPF_File(filename);

          if (!file[low]->exists())
          {
             //
             // RELFSWn     -> RELFSW0/100
             //
             std::sprintf(filename, RELFSW "%d", low);
             TRACE(("rename %s -> %s", 0, file[highest]->getCPFname(), filename))
             file[highest]->rename(filename);
          }
          else
          {
             TRACE((LOG_LEVEL_INFO, "no RELFSW0/100", 0))
             highest++;
          }

          //
          // RELFSWn-1   -> RELFSWn
          // RELFSWn-2   -> RELFSWn-1
          //         .........
          // RELFSW1/101 -> RELFSW2/102
          //
          for (gennum = highest - 1; gennum > low; gennum--)
          {
             std::sprintf(filename, RELFSW "%d", gennum + 1);
             TRACE(("rename %s -> %s", 0, file[gennum]->getCPFname(), filename))
             file[gennum]->rename(filename);
          }

          //
          // RELFSWX/Y   -> RELFSW1/101
          //
          std::sprintf(filename, RELFSW "%d", low + 1);
          TRACE(("rename %s -> %s", 0, tempfile->getCPFname(), filename))
          tempfile->rename(filename);
       }
    } catch (FMS_CPF_Exception& EE) //:ag1
    {
       m_FaultCode = unsuccessful;
       TRACE((LOG_LEVEL_ERROR, "FMS_CPF_Exception", 0))
       TRACE((LOG_LEVEL_ERROR, "   errorType: %d", 0, EE.errorCode()))
       TRACE((LOG_LEVEL_ERROR, "   errorText: %s", 0, EE.errorText()))
       TRACE((LOG_LEVEL_ERROR, "   errorInfo: %s", 0, EE.detailInfo().c_str()))

    } catch (FAULT_CODE &FaultCode)
    {
       m_FaultCode = FaultCode;
    }

    if (tempfile)
    {
       if (tempfile->isReserved())
       {
          tempfile->unreserve();
       }
       delete tempfile;
    }

    for (gennum = FFRfirst; gennum <= SFRlast; gennum++)
    {
       if (file[gennum])
       {
          if (file[gennum]->isReserved())
          {
             file[gennum]->unreserve();
          }
          delete file[gennum];
       }
    }
}

//******************************************************************************
//    Process_new_buffer()
//******************************************************************************
void CPS_BUAP_FILEMGR::Process_new_buffer(CPS_BUAP_MSG **txMsg)
{
   newTRACE(("CPS_BUAP_FILEMGR::Process_new_buffer()", 0))

   static CPS_BUAP_MSG_text l_txMsg;
   char printout[256];

   if (m_CmdResult == COMMAND_ORDERED)
   //**************************************************************************
   // Command is still executing.
   //**************************************************************************
   {
      l_txMsg.UserData1() = CPS_BUAP_MSG_ID_no_answer;
      l_txMsg.UserData2() = 1;

      printout[0] = 0;

   }

   else
   //**************************************************************************
   // Command has been completed.
   //**************************************************************************
   {
      TRACE((LOG_LEVEL_INFO, "*** command complete ***", 0))
      l_txMsg.UserData1() = CPS_BUAP_MSG_ID_complete;
      l_txMsg.UserData2() = 1;

      if (m_CmdResult == NO_ERROR)
      //**********************************************************************
      // Successful.
      //**********************************************************************
      {
         std::sprintf(printout, "%s\n%s\n%s\n", EXECUTED, m_Command, END);
      }
      else
      //**********************************************************************
      // Unsuccessful.
      //**********************************************************************
      {
         std::sprintf(printout, "%s\n%s\n%s %lu\n%s\n", NOT_EXECUTED, m_Command,
               FAULT_CODE, m_CmdResult, END);
      }

      m_CmdResult = IDLE;
   }

   l_txMsg.m_Text << printout;
   *txMsg = &l_txMsg;
}

//******************************************************************************
//    Run()
//******************************************************************************
void CPS_BUAP_FILEMGR::Run()
{
   newTRACE ((LOG_LEVEL_INFO, "(CPS_BUAP_FILEMGR::Run())", 0));

   // Use condition variable to notify other thread that server is started
   {
       boost::lock_guard<boost::mutex> lock(m_mutex);
       m_running = true;
   }
   m_condition.notify_one();

   try
   {
       // Start ACS_JTP Service
       if (!this->initiateJTPService())
       {
           m_running = false;
           TRACE((LOG_LEVEL_INFO, "CPS_BUAP_FILEMGR server stopped", 0));
           return;
       }

       TRACE((LOG_LEVEL_INFO, "CPS_BUAP_FILEMGR server started", 0));

       // Enter the select loop to wait for incoming object
       while (!Poll())
          ;

   }
   catch(...)
   {
       TRACE((LOG_LEVEL_ERROR, "BUAP_FileMgr runs with Exception!!!", 0));

       {
           // Lock the session to protect
           boost::recursive_mutex::scoped_lock scoped_lock(mutex);
           if (pService && (!reportErrorFlag))
           {
               TRACE((LOG_LEVEL_FATAL, "FILEMGR -Report error to AMF.", 0));
               reportErrorFlag = true;
               pService->componentReportError(ACS_APGCC_COMPONENT_RESTART);
           }
       }

   }

   m_running = false;

   TRACE((LOG_LEVEL_INFO, "CPS_BUAP_FILEMGR server stopped gracefully", 0));
}

//******************************************************************************
//    GetTime()
//******************************************************************************
void CPS_BUAP_FILEMGR::GetTime(const std::string &filename, DateTime &time)
{
   string fileName(filename);
   newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_FILEMGR::GetTime(%s)", 0, fileName.c_str()))

   int l_fd = -1;
   int l_century;
   outputTimesSector l_outputTimes;
   char l_subfilename[256];

   std::sprintf(l_subfilename, "%s/R0", fileName.c_str());

   try
   {
      TRACE(("Opening subfile %s", 0, l_subfilename))
      l_fd = ::_open(l_subfilename, O_RDONLY);
      if (l_fd == -1)
      {
         throw 1;
      }

      if (::_lseek(l_fd, sizeof(controlInfoSector), SEEK_CUR) == -1)
      {
         throw 1;
      }

      if (::_read(l_fd, &l_outputTimes, sizeof(outputTimesSector)) == -1)
      {
         throw 1;
      }

      ::_close(l_fd);

      if (l_outputTimes.dump.year[0] < 90)
      {
         l_century = 20;
      }
      else
      {
         l_century = 19;
      }

      TRACE((LOG_LEVEL_INFO, "%s @ %04d-%02d-%02d, %02d:%02d", 0,
                  fileName.c_str(),
                  int(l_outputTimes.dump.year[0]) + l_century*100,
                  int(l_outputTimes.dump.month[0]),
                  int(l_outputTimes.dump.day[0]),
                  int(l_outputTimes.dump.hour[0]),
                  int(l_outputTimes.dump.minute[0])))

      time = DateTime(int(l_outputTimes.dump.day[0]), int(
            l_outputTimes.dump.month[0]), int(l_outputTimes.dump.year[0]
            + l_century * 100), int(l_outputTimes.dump.hour[0]), int(
            l_outputTimes.dump.minute[0]));
   } catch (FMS_CPF_Exception& EE) //:ag1
   {

      TRACE((LOG_LEVEL_ERROR, "FMS_CPF_Exception", 0))
      TRACE((LOG_LEVEL_ERROR, "   errorType: %d", 0, EE.errorCode()))
      TRACE((LOG_LEVEL_ERROR, "   errorText: %s", 0, EE.errorText()))
      TRACE((LOG_LEVEL_ERROR, "   errorInfo: %s", 0, EE.detailInfo().c_str()))
      ::_close(l_fd);
      throw;
   } catch (int )
   {
      ::_close(l_fd);
      throw;
   }
}

//******************************************************************************
//    Process_thread_control()
//******************************************************************************
DWORD CPS_BUAP_FILEMGR::Process_thread_control(DWORD objectHandle)

{
   newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_FILEMGR::Process_thread_control, todo...", 0))

   DWORD result = NO_ERROR;

   if (objectHandle == (DWORD)hNTObjectArray[ABORT_EVENT])
   {
       TRACE((LOG_LEVEL_INFO, "CPS_BUAP_FILEMGR::Process_thread_control(ABORT_EVENT)", 0));

       if (!hNTObjectArray[COMMAND_THREAD])
       {
           //*********************************************************************
           // If no command is being executed the main thread is terminated.
           // It is not possible to receive an abort reguest in the command thread
           // if a copying of the entire RELFSW1 is being processed (SYSFT), and
           // SYNIC and SYTUC mustn't be interrupted.
           // In the case that a command is being processed, CPS_BUAP_FILEMGR_-
           // ServiceThread terminates the entire CPS_BUAP_filemgr process after
           // timeout.
           //*********************************************************************

           result = SERVICE_STOPPED;

           // Reset the stop event.
           m_stopEvent.resetEvent();

           // Clear the JTP Handlers list.
           if (!m_ConversationList.empty())
           {
               // delete all conversations from the list
               TRACE(("%s - Delete all conversation from the list", 0, m_deriveName.c_str()));
               for (CPS_BUAP_Conversation_LIST_ITERATOR iConversation =
                       m_ConversationList.begin(); iConversation
                       != m_ConversationList.end(); ++iConversation)
               {
                   //delete *iConversation;
		   (*iConversation).reset();
               }
           }

           m_ConversationList.clear();

           // Server closed, clear the pointer to JTP Service
           if (m_pService)
           {
               delete m_pService;
               m_pService = 0;
           }

           TRACE((LOG_LEVEL_INFO, "CPS_BUAP_FILEMGR::Process_thread_control(): going to exit", 0));
       }


       //**********************************************************************
       // So far there is only one thread control event.
       //**********************************************************************

       return result;
   }
   else if (objectHandle == (DWORD)hNTObjectArray[COMMAND_THREAD])
   {
       TRACE((LOG_LEVEL_INFO, "CPS_BUAP_FILEMGR::Process_thread_control(COMMAND_THREAD - %d)", 0, objectHandle));
       //TRACE(("CPS_BUAP_FILEMGR::COMMAND_THREAD_TERMINATED", 0));

       if (hNTObjectArray[COMMAND_THREAD])
       {

          TRACE((LOG_LEVEL_INFO, "*** command processed ***", 0));

          //*********************************************************************
          // Get the result of the command execution. The result will be sent
          // to the CP in Process_new_buffer().
          //*********************************************************************

          m_CmdResult = m_FaultCode;

          //*********************************************************************
          // Close the command thread.
          //*********************************************************************
          TRACE((LOG_LEVEL_INFO, "CPS_BUAP_FILEMGR Command Thread closed with handle: %d and m_CmdResult = %d", 0, hNTObjectArray[COMMAND_THREAD], m_CmdResult));

          // Reset the command event FD
          m_commandThreadEvent.resetEvent();
          hNTObjectArray[COMMAND_THREAD] = 0;
          noOfNTHandles--;

       }

       else
       //*********************************************************************
       // Got no handle to the command thread.
       //*********************************************************************
       {
          m_CmdResult = ERROR_INVALID_HANDLE;

          EVENT((CPS_BUAP_Events::lost_handle, 0, "CPS_BUAP_FILEMGR_CMD_THREAD"));
          THROW("lost handle to CPS_BUAP_FILEMGR_CMD_THREAD");
       }

   }
   else
   {
       //TODO: if invalid handle, need to stop server ??
       TRACE((LOG_LEVEL_ERROR, "CPS_BUAP_FILEMGR::Process_thread_control(): Invalid handle", 0));
       result = INVALID_HANDLE;
   }


   return result;

}


//***************************************************************************
//      stop()
//***************************************************************************

void CPS_BUAP_FILEMGR::stop()
{
   newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_FILEMGR::stop()", 0));

   //Set event stop
   m_stopEvent.setEvent();

}

//***************************************************************************
//      initiateJTPService()
//***************************************************************************
bool CPS_BUAP_FILEMGR::initiateJTPService()
{

    CPS_BUAP_Conversation_Ptr l_pConversation;
    int noOfServerHandles = 15;
    HANDLE *serverHandles;
    serverHandles = new HANDLE[15];
    bool jtpResult = false;

    //***************************************************************************
    // Start the BUFILEMGR server.
    //***************************************************************************

    newTRACE((LOG_LEVEL_INFO, "new ACS_JTP_Service(BUFILEMGR)", 0));

    char acServiceName[] = "BUFILEMGR";

    uint64_t u;
    ssize_t num;

    bool isTrace = false;

    do
    {
        // Create a JTP to BUFILEMGR server.
        if (m_pService)
        {
            delete m_pService;
            m_pService = 0;
        }
        m_pService = new ACS_JTP_Service(acServiceName);
        TRACE(("CPS_BUAP_FILEMGR - calling jidrepreq", 0));
        // try to connect to JTP Name Service (DSD at the end) until it not fails.
        jtpResult = m_pService->jidrepreq();

        if (jtpResult == false)
        {
            if (!isTrace)
            {
                TRACE((LOG_LEVEL_ERROR, "ACS_JTP_Service(BUFILEMGR)->jidrepreq: failed to register to DSD", 0));
                EVENT((CPS_BUAP_Events::jidrepreq_fail, 0, "BUFILEMGR"));
                isTrace = true;
            }

            // Check if the stop event is set.
            num = ::read(m_stopEvent.getFd(), &u, sizeof(uint64_t));
            if (num == -1)
            {
                if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
                {
                    // Not receive stop event, go to sleep.
                    ::sleep((unsigned int) 1);
                    continue;
                    //THROW("Register to DSD failed.");
                }

                TRACE((LOG_LEVEL_ERROR, "CPS_BUAP_FILEMGR failed to read stop event", 0));
                if (serverHandles)
                {
                    delete [] serverHandles;
                    serverHandles = 0;
                }
                //return false;
                THROW("Reading stop event failed");
            }
            else
            {
                TRACE((LOG_LEVEL_INFO, "CPS_BUAP_FILEMGR receives stop event %d ", 0, m_stopEvent.getFd()));
                m_stopEvent.resetEvent();

                if (serverHandles)
                {
                    delete [] serverHandles;
                    serverHandles = 0;
                }
                return false;
            }
        }
        else
        {
            TRACE((LOG_LEVEL_INFO, "ACS_JTP_Service(BUFILEMGR) register successfully", 0));
        }
    }
    while (jtpResult == false);

    //TRACE(("ACS_JTP_Service(BUFILEMGR)->jidrepreq: successfully register to DSD after %d trial", 0, count+1));

    //****************************************************************************
    // Get JTP handles for service and link them into conversation list.
    //****************************************************************************
    m_pService->getHandles(noOfServerHandles, serverHandles);
    TRACE(("$$$$: %d, %d", 0, noOfServerHandles, noOfServerHandles))

    for (int i = 0; i < noOfServerHandles; i++)
    {
        l_pConversation.reset(new CPS_BUAP_Conversation(serverHandles[i], m_pService));

        AddConversation(l_pConversation);
    }

    if (serverHandles)
    {
        delete[] serverHandles;
        serverHandles = 0;
    }

    TRACE((LOG_LEVEL_INFO, "CPS_BUAP_FILEMGR initiateJTPService returned true", 0));
    return true;
}

//***************************************************************************
//    isRunning()
//***************************************************************************
bool CPS_BUAP_FILEMGR::isRunning() const
{
    //newTRACE(("CPS_BUAP_FILEMGR::isRunning() - %d", 0, (int)m_running));
    return m_running;
}

//***************************************************************************
//    waitUntilRunning()
//***************************************************************************
void CPS_BUAP_FILEMGR::waitUntilRunning()
{
    newTRACE(("CPS_BUAP_FILEMGR::waitUntilRunning()", 0));

    boost::unique_lock<boost::mutex> lock(m_mutex);

    while (m_running == false)
    {
        TRACE((LOG_LEVEL_INFO, "Wait until CPS_BUAP_FILEMGR server Thread signal", 0));
        m_condition.wait(lock);
        TRACE((LOG_LEVEL_INFO, "CPS_BUAP_FILEMGR server Thread signaled", 0));
    }
}

