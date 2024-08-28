//*****************************************************************************
//
// NAME    CPS_BUAP_LoadingSession.cpp
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
//
// DESCRIPTION 
//      Manages a single APZ loading session.
//
// DOCUMENT NO
// 190 89-CAA 109 1412
//
// AUTHOR 
// 	Wed Sep 24 12:31:03 1997 by UAB/I/GM UABFLN
//
// CHANGES 
//  990623  uablan - Changed function getFn to want file name
//                   without subfile.
//  990913  uabbir - Class CPS_BUAP_Loader_Exception replaced by
//                   CPS_BUAP_Exception
//  001017  qabgill - fixed daylight saving bug in supTimeExpired
//  050114  uablan  - Remove lock for reladmpar.
//  050623  uablan  - fixed daylight saving bug in supTimeExpired TR-HF82616
//  111116  xtuangu - Update CAA number
//  120306  xngudan - fixed TR-HP56832
//  120706  xngudan - fixed TRs HQ16627, HQ16649
//
// SEE ALSO 
// 	-
//
//*****************************************************************************

#include "CPS_BUAP_Linux.h"
#include "CPS_BUAP_LoadingSession.h"
#include "CPS_BUAP_Exception.h"
#include "CPS_BUAP_MSG.h"
#include "CPS_BUAP_MSG_IDs.h"
#include "CPS_BUAP_Trace.h"
#include "CPS_BUAP_Events.h"
#include "fms_cpf_file.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sstream>
#include <iomanip>
#include <sys/stat.h>
#include <fcntl.h>
using namespace std;
//
// Member function definitions:
// ============================

//*****************************************************************************
//	CPS_BUAP_LoadingSession()
//*****************************************************************************
CPS_BUAP_LoadingSession::CPS_BUAP_LoadingSession() :
   parmgr("BUPARMGR", 10, 2), APZtype(0)
{
   newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_LoadingSession::CPS_BUAP_LoadingSession()", 0));

   // Clear the Last reload parameter entries
   parLAR.m_ManAuto << 0;
   parLAR.m_NCT << 0;
   parLAR.m_Omission << 0;
   parLAR.m_SUP << 0;
   // Fixed TR HP56832 - LPFG value not being reset when reload sequence reset.
   parLAR.m_LRGF << 0;
}
//*****************************************************************************
//	~CPS_BUAP_LoadingSession()
//*****************************************************************************
CPS_BUAP_LoadingSession::~CPS_BUAP_LoadingSession()
{
   newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_LoadingSession::~CPS_BUAP_LoadingSession()", 0));

   // when PARMGR_Client goes out of scope the connection will
   // disconnect.
}
//*****************************************************************************
//	getFn()
//*****************************************************************************
void CPS_BUAP_LoadingSession::getFn(string& iname)
{
   newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_LoadingSession::getFn(%s)", 0, iname.data()));

   if (iname != "RELFSW255")
   {
      string fileNum = strpbrk(iname.data(), "0123456789");
      parLAR.m_LRG = atoi(fileNum.data());
      TRACE((LOG_LEVEL_INFO, "parLAR.m_LRG : %d ", 0, (int)parLAR.m_LRG()));
      return;
   }

   // Get reladmpar values
   readStaticParams();
   readDynamicParams();

   // Check if supervision time has expired,
   // (will also set new expiring time)
   CPS_BUAP_MSG_dynamic_parameters updDynamic; // obj to store values
   if (supTimeExpired(updDynamic))
   {
      parDynamic.m_State << 1; // Reset state
      TRACE(("State reset to 1", 0));
      // Reset counter values
      parDynamic.m_CTA << 0; // This session still looks at this sessions
      parDynamic.m_NumFFR << 0; // parameters, so we must reset them
      parDynamic.m_NumSFR << 99;
      parDynamic.m_NCT << 0;
      parDynamic.m_LPF << 0;
      parDynamic.m_Omission << 0;
      updDynamic.m_CTA << 0; // Next turns dynamic parameters
      updDynamic.m_NumFFR << 0;
      updDynamic.m_NumSFR << 99;
      updDynamic.m_NCT << 0;
      updDynamic.m_LPF << 0;
      updDynamic.m_Omission << 0;
   }

   //
   // Find current reload file
   // ------------------------
   int dumpNo = 0;

   TRACE((LOG_LEVEL_INFO, "Current dump state: %d", 0, parDynamic.m_State()));
   switch (parDynamic.m_State())
   {
   case 1:
      parLAR.m_LRGF << 0; // Not last permissible file anymore
      break;
   case 2:
      break;
   case 3:
      parDynamic.m_State << 4; // Always a 1-time stop
      break;
   case 4:
      dumpNo = parDynamic.m_NumFFR();
      break;
   case 5:
      dumpNo = parDynamic.m_NumSFR();
      break;
   case 6:
      dumpNo = parDynamic.m_LPF();
      parLAR.m_LRGF << 1;
      break;
   default:
      break;
   }
   char buff[32];
   sprintf(buff, "RELFSW%d", dumpNo);
   iname = buff;
   TRACE((LOG_LEVEL_INFO, "Current dump name: %s", 0, iname.data()));

   //
   // Set values for latest reload
   // ----------------------------
   parLAR.m_ManAuto << parStatic.m_ManAuto();
   parLAR.m_LRG << dumpNo;
   parLAR.m_NCT << parDynamic.m_NCT();
   parLAR.m_Omission << parDynamic.m_Omission();
   parLAR.m_SUP << parStatic.m_SUP();

   // If, at any truncation attempt, the number of truncated commands is greater than or equal
   // to the number of commands in the latest command log subfile, then the entire latest
   // command log subfile will be omitted. In this case, if the value of parameter LOAZ is 0 or 1,
   // it will be counted as 0. This means that a reload attempt within supervision time (SUP) will
   // use next older generation file. If value of parameter LOAZ is 2, then total omission attempt
   // will be done

   // Flag used to mark if the number of truncated command >= number of commands in the
   // latest command log subfile
   bool _flag = false;
   // Variable stores the number of command log in the command log subfile
   int currentCmdLogNums = 0;

   //
   // Calculate new values for next attempt
   // -------------------------------------

   switch (parDynamic.m_State())
   {
   case 1:
      // Reload RELFSW0
      // -----------------
      updDynamic.m_State << 2;

   case 2:
      // Command Log truncations
      // -----------------------
      // In case no truncations
      if ((parStatic.m_ManAuto() == 1) || (parStatic.m_NTAZ() == 0))
      {
         updDynamic.m_State << 3;
      }
      else // In case having truncations
      {
          if (parStatic.m_NTAZ() == parDynamic.m_CTA())
          {
              // Need to update currentTruncatedCommands
              currentCmdLogNums = getNumberOfCurrentCmdLog();
              if (parDynamic.m_NCT() >= currentCmdLogNums)
              {
                  _flag = true;
                  parLAR.m_NCT << currentCmdLogNums;
              }
              updDynamic.m_State << 3;
          }
          else
          {
              int n = parDynamic.m_CTA(); // Truncation attempt
              updDynamic.m_CTA << ++n;
              int nct = parDynamic.m_NCT(); // # of cmds truncated
              int ntcz = parStatic.m_NTCZ(); // # of cmds to truncate
              int nct_temp = nct + ntcz;
              // The maximum size of the numberOfTruncatedCommand is 255
              if (nct_temp >= 255)
              {
                  nct_temp = 255;
              }
              updDynamic.m_NCT << nct_temp;
              updDynamic.m_State << 2;

              // Fixing TR HQ16649
              currentCmdLogNums = getNumberOfCurrentCmdLog();
              if (parDynamic.m_NCT() >= currentCmdLogNums)
              {
                  // Flag marked if m_NCT >= number of commands in the current command log subfile
                  _flag = true;
                  // Update new value for currentTruncatedCommands in LAR
                  parLAR.m_NCT << currentCmdLogNums;
                  updDynamic.m_State << 3;

              }
              else
              {
                  break;      // Done for now
              }
             // End of TR HQ16649
          }
      }
   case 3:
       // Reset command Log truncations & current truncation attempt & fixing TR HQ16627
       updDynamic.m_NCT << 0;
       updDynamic.m_CTA << 0;
      // Command Log omission attempt
      // ----------------------------
      if ((parStatic.m_ManAuto() == 1) || (parStatic.m_LOAZ() == 0))
      {
         updDynamic.m_State << 4;
      }
      else
      {
         updDynamic.m_Omission << parStatic.m_LOAZ();
         updDynamic.m_State << 3;

         // TR HQ16649
         if (_flag)
         {
             if ((updDynamic.m_Omission() == 1) || (updDynamic.m_Omission() == 0))
             {
                 updDynamic.m_Omission << 0;
                 updDynamic.m_State << 4;
             }

             if (updDynamic.m_Omission() == 2)
             {
                 updDynamic.m_State << 3;
                 break;
             }
         }
         else
         {
             break; // Done for now
         }
         // End of TR HQ16649
      }

   case 4:
      updDynamic.m_Omission << 0; // Reset omission state
      // Reload First File Range
      // -----------------------
      if ((parStatic.m_INCL1() == 0) || (parDynamic.m_NumFFR()
            == parDynamic.m_NOld()))
      {
         updDynamic.m_State << 5;
      }
      else
      {
         int j = parDynamic.m_NumFFR();
         updDynamic.m_NumFFR << ++j;
         updDynamic.m_LPF << j;
         updDynamic.m_State << 4;
         break; // Done for now
      }

   case 5:
      // Reload Second File Range
      // ------------------------
      if ((parStatic.m_INCL2() == 0) || (parDynamic.m_KOld() == 0)
            || (parDynamic.m_NumSFR() == parDynamic.m_KOld()))
      {
         updDynamic.m_State << 6;
      }
      else
      {
         int k = parDynamic.m_NumSFR();
         updDynamic.m_NumSFR << ++k;
         updDynamic.m_LPF << k;
         updDynamic.m_State << 5;
         break; // Done for now
      }

   case 6:
      // Last permissable file
      // ---------------------
      updDynamic.m_State << 6;
      break;

   default:
      break;
   }

   //
   // Write new dynamic values
   // ------------------------
   TRACE((LOG_LEVEL_INFO, "Writing new dynamic parameters\n State \t%d\n CTA \t%d\n NumFFR \t%d\n NumSFR \t%d\n NOld \t%d\n KOld \t%d\n NCT \t%d\n EXPcentury \t%d\n EXPyear \t%d\n EXPmonth \t%d\n EXPday \t%d\n EXPhour \t%d\n EXPminute \t%d\n LPF \t%d\n Omission \t%d\n"
               , 0,
               updDynamic.m_State(),
               updDynamic.m_CTA(),
               updDynamic.m_NumFFR(),
               updDynamic.m_NumSFR(),
               updDynamic.m_NOld(),
               updDynamic.m_KOld(),
               updDynamic.m_NCT(),
               updDynamic.m_EXPcentury(),
               updDynamic.m_EXPyear(),
               updDynamic.m_EXPmonth(),
               updDynamic.m_EXPday(),
               updDynamic.m_EXPhour(),
               updDynamic.m_EXPminute(),
               updDynamic.m_LPF(),
               updDynamic.m_Omission()
         ));
   writeDynamicParams(updDynamic);

   return;
}
//*****************************************************************************
//	reportCmdlog()
//*****************************************************************************
void CPS_BUAP_LoadingSession::reportCmdlog(R0Info r0)
{
   newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_LoadingSession::reportCmdlog()", 0));

   APZtype = r0.APZtype;

   parLAR.m_AssClogNum << r0.subfile;

   parLAR.m_OPSRScentury << ((*r0.rpsrs.year > 90) ? 19 : 20);
   parLAR.m_OPSRSyear << *r0.rpsrs.year;
   parLAR.m_OPSRSmonth << *r0.rpsrs.month;
   parLAR.m_OPSRSday << *r0.rpsrs.day;
   parLAR.m_OPSRShour << *r0.rpsrs.hour;
   parLAR.m_OPSRSminute << *r0.rpsrs.minute;

   parLAR.m_ODSScentury << ((*r0.sdd.year > 90) ? 19 : 20);
   parLAR.m_ODSSyear << *r0.sdd.year;
   parLAR.m_ODSSmonth << *r0.sdd.month;
   parLAR.m_ODSSday << *r0.sdd.day;
   parLAR.m_ODSShour << *r0.sdd.hour;
   parLAR.m_ODSSminute << *r0.sdd.minute;

   parLAR.m_ODSLcentury << ((*r0.ldd.year > 90) ? 19 : 20);
   parLAR.m_ODSLyear << *r0.ldd.year;
   parLAR.m_ODSLmonth << *r0.ldd.month;
   parLAR.m_ODSLday << *r0.ldd.day;
   parLAR.m_ODSLhour << *r0.ldd.hour;
   parLAR.m_ODSLminute << *r0.ldd.minute;
}
//*****************************************************************************
//	writeLastReloadParams()
//*****************************************************************************
void CPS_BUAP_LoadingSession::writeLastReloadParams()
{
   newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_LoadingSession::writeLastReloadParams()", 0));
   CPS_BUAP_MSG writeLARReply;
   parLAR.UserData1() = CPS_BUAP_MSG_ID_write_LAR;
   parmgr.TXRX(parLAR, writeLARReply);
}
//*****************************************************************************
//	readStaticParams()
//*****************************************************************************
void CPS_BUAP_LoadingSession::readStaticParams()
{
   newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_LoadingSession::readStaticParams()", 0));
   CPS_BUAP_MSG readOrder;
   readOrder.UserData1() = CPS_BUAP_MSG_ID_read_static;
   parmgr.TXRX(readOrder, parStatic);
}
//*****************************************************************************
//	readDynamicParams()
//*****************************************************************************
void CPS_BUAP_LoadingSession::readDynamicParams()
{
   newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_LoadingSession::readDynamicParams()", 0));
   CPS_BUAP_MSG readOrder;
   readOrder.UserData1() = CPS_BUAP_MSG_ID_read_dynamic;
   parmgr.TXRX(readOrder, parDynamic);
}
//*****************************************************************************
//	writeDynamicParams()
//*****************************************************************************
void CPS_BUAP_LoadingSession::writeDynamicParams(
      CPS_BUAP_MSG_dynamic_parameters& wDP)
{
   newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_LoadingSession::writeDynamicParams()", 0));
   CPS_BUAP_MSG wDPReply;
   wDP.UserData1() = CPS_BUAP_MSG_ID_write_dynamic;
   parmgr.TXRX(wDP, wDPReply);
}
//
// This function will always update the supervision time
//
//*****************************************************************************
//  supTimeExpired()
//*****************************************************************************
int CPS_BUAP_LoadingSession::supTimeExpired(
      CPS_BUAP_MSG_dynamic_parameters& uDP)
{
   newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_LoadingSession::supTimeExpired()", 0));

   bool hasExpired = false;
   tm tmNow;
   time_t time_tNow;
   // * get current time
   time(&time_tNow);

   memcpy(&tmNow, localtime(&time_tNow), sizeof(tm));
   TRACE(("Get Current time : %d-%d-%d %d:%d",0 , tmNow.tm_year,tmNow.tm_mon,tmNow.tm_mday,tmNow.tm_hour,tmNow.tm_min));
   TRACE(("Get Current daylight : %d",0 , tmNow.tm_isdst));

   // * set up expire time
   tm tmExpire;
   memset(&tmExpire, 0, sizeof(tm));
   // since we don't persist the daylight, assume same as tmNow
   tmExpire.tm_isdst = tmNow.tm_isdst;

   tmExpire.tm_year
         = (parDynamic.m_EXPcentury() * 100 + parDynamic.m_EXPyear()) - 1900;
   tmExpire.tm_mon = parDynamic.m_EXPmonth() - 1; // tm counts months since jan
   tmExpire.tm_mday = parDynamic.m_EXPday();
   tmExpire.tm_hour = parDynamic.m_EXPhour();
   tmExpire.tm_min = parDynamic.m_EXPminute();

   // Fix for TR HF82616 - daylight==1 and EXP-date == 1970-01-01 makes mktime crash!
   // Change EXP-date to 1990-01-01 instead of 1970-01-01
   if ((tmExpire.tm_year == 70) && (tmExpire.tm_mon == 0) && (tmExpire.tm_mday
         == 1))
   {
      tmExpire.tm_year = 90;
   }

   time_t time_tExpire = mktime(&tmExpire);
   TRACE(("mktime EXP time: %d%d%d %d:%d", 0, tmExpire.tm_year,tmExpire.tm_mon,tmExpire.tm_mday,tmExpire.tm_hour,tmExpire.tm_min));

   if (time_tNow >= time_tExpire)
   {
      TRACE((LOG_LEVEL_WARN, "Supervisiontime has expired", 0));
      hasExpired = true;
   }

   // * always set new expire time here (which is kinda' weird)
   // now + (supervision time * tens of minutes * secs per minute)
   time_tExpire = time_tNow + (parStatic.m_SUP() * 10 * 60);
   memcpy(&tmExpire, localtime(&time_tExpire), sizeof(tm));

   TRACE((LOG_LEVEL_INFO, "New expire date/time calculated", 0));

   uDP.m_EXPcentury << (1900 + tmExpire.tm_year) / 100;
   uDP.m_EXPyear << (1900 + tmExpire.tm_year) % 100;
   uDP.m_EXPmonth << tmExpire.tm_mon + 1; // tm counts months since jan
   uDP.m_EXPday << tmExpire.tm_mday;
   uDP.m_EXPhour << tmExpire.tm_hour;
   uDP.m_EXPminute << tmExpire.tm_min;

   return hasExpired ? 1 : 0;
}

//*****************************************************************************
//  readCmdLogRefParams()
//*****************************************************************************
void CPS_BUAP_LoadingSession::readCmdLogRefParams()
{
   newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_LoadingSession::readCmdLogRefParams()", 0));
   CPS_BUAP_MSG readOrder;
   readOrder.UserData1() = CPS_BUAP_MSG_ID_read_clogrefs;
   parmgr.TXRX(readOrder, parCmdLogRef);
}

#if 0
//*****************************************************************************
//  checkNumberOfTruncCommands()
//*****************************************************************************
bool CPS_BUAP_LoadingSession::checkNumberOfTruncCommands(CPS_BUAP_OPT_UINT8 &nct)
{
    newTRACE(("CPS_BUAP_LoadingSession::checkNumberOfTruncCommands()", 0));

    bool result = false;
    FMS_CPF_File *relcmdFile = 0;
    FMS_CPF_Types::fileStat fileStatus;
    int cmdLogSubfileNum = 0;

    try
    {
        //string fileName("RELCMDHDF");

        // Read reladmpar values
        readCmdLogRefParams();
        if (parCmdLogRef.m_Validity() == 0)
        {
            // Command Log References not valid
            TRACE(("Command Log References not valid", 0));
            throw;
        }
        // Get the current command log subfile from Command Log References parameters
        int n = parCmdLogRef.m_Current();
        stringstream ss;
        ss << setfill('0') << setw(7) << n;
        char fileName[32];
        std::sprintf(fileName, "RELCMDHDF-%s", ss.str().c_str());


        // Construct a FMS_CPF instance for RELCMDHDF-XXXXXXX file
        relcmdFile = new FMS_CPF_File(fileName);

        // Check if RELCMDHDF exists
        if (!relcmdFile->exists())
        {
            TRACE(("%s file not exist", 0, fileName));
            throw;
        }

        // Reserve and get the status information
        relcmdFile->reserve(FMS_CPF_Types::XR_);
        relcmdFile->getStat(fileStatus);

        if (fileStatus.size >= 1)
        {
            // Get the number of command log subfiles in RELCMDHDF-XXXXXXX file
            cmdLogSubfileNum = getNumberOfCmdLogSubfile(relcmdFile->getPhysicalPath(),
                                                        (fileStatus.size - 1)*512 + 1);
        }
        else
        {
            cmdLogSubfileNum = 0;
        }

        TRACE(("number commands in the %s file : %d", 0,
                relcmdFile->getPhysicalPath().c_str(), cmdLogSubfileNum));

        // Compare the number of record in Command Log subfile with the
        // number of truncated commands in Dynamic parameters
        if (nct() >= cmdLogSubfileNum)
        {
            result = true;
        }
    }
    catch (FMS_CPF_Exception &e)
    {
        TRACE(("FMS_CPF_Exception", 0))
        TRACE(("   errorType: %d", 0, e.errorCode()))
        TRACE(("   errorText: %s", 0, e.errorText()))
        TRACE(("   errorInfo: %s", 0, e.detailInfo().c_str()))
    }
    catch (...)
    {
        TRACE(("Exception!", 0));
    }

    if (relcmdFile)
    {
        if (relcmdFile->isReserved())
        {
            relcmdFile->unreserve();
        }
        delete relcmdFile;
        relcmdFile = 0;
    }

    return result;
}
#endif


//*****************************************************************************
//  getNumberOfCurrentCmdLog()
//*****************************************************************************
int CPS_BUAP_LoadingSession::getNumberOfCurrentCmdLog()
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_LoadingSession::getNumberOfCurrentCmdLog()", 0));

    FMS_CPF_File *relcmdFile = 0;
    //FMS_CPF_Types::fileStat fileStatus;
    int cmdLogSubfileNum = 0;

    try
    {
        // Read reladmpar values
        readCmdLogRefParams();

        if (parCmdLogRef.m_Validity() == 0)
        {
            // Command Log References not valid
            TRACE((LOG_LEVEL_WARN, "Command Log References not valid", 0));
            throw 1;
        }
        // Get the current command log subfile from Command Log References parameters
        int n = parCmdLogRef.m_Current();
        stringstream ss;
        ios::fmtflags f(ss.flags());
        ss << setfill('0') << setw(7) << n;
        char fileName[32];
        sprintf(fileName, "RELCMDHDF-%s", ss.str().c_str());
        // Restore the ostream format
        ss.flags(f);

        TRACE(("Construct a FMS instance for %s", 0, fileName));
        // Construct a FMS_CPF instance for RELCMDHDF-XXXXXXX file
        relcmdFile = new FMS_CPF_File(fileName);

        // Check if RELCMDHDF exists
        if (!relcmdFile->exists())
        {
            TRACE((LOG_LEVEL_WARN, "%s file not exist", 0, fileName));
            throw 1;
        }

        // Reserve and get the status information
        relcmdFile->reserve(FMS_CPF_Types::R_);

#if 0
        //relcmdFile->getStat(fileStatus);      // Not yet implemented by FMS_CPF

        if (fileStatus.size >= 1)
        {
            // Get the number of command log subfiles in RELCMDHDF-XXXXXXX file
            cmdLogSubfileNum = getNumberOfCmdLogSubfile(relcmdFile->getPhysicalPath(),
                                                        (fileStatus.size - 1)*512 + 1);
        }
        else
        {
            cmdLogSubfileNum = 0;
        }
#endif

        // Get the number of command log in the current command log subfile
        cmdLogSubfileNum = getNumberOfCmdLogSubfile(relcmdFile->getPhysicalPath());

        TRACE((LOG_LEVEL_INFO, "number commands in the %s file : %d", 0,
                relcmdFile->getPhysicalPath().c_str(), cmdLogSubfileNum));

    }
    catch (FMS_CPF_Exception &e)
    {
        TRACE((LOG_LEVEL_ERROR, "FMS_CPF_Exception", 0));
        TRACE((LOG_LEVEL_ERROR, "   errorType: %d", 0, e.errorCode()));
        TRACE((LOG_LEVEL_ERROR, "   errorText: %s", 0, e.errorText()));
        TRACE((LOG_LEVEL_ERROR, "   errorInfo: %s", 0, e.detailInfo().c_str()));
    }
    catch (...)
    {
        TRACE((LOG_LEVEL_ERROR, "Exception!", 0));
    }

    if (relcmdFile)
    {
        if (relcmdFile->isReserved())
        {
            relcmdFile->unreserve();
        }
        delete relcmdFile;
        relcmdFile = 0;
    }

    TRACE((LOG_LEVEL_INFO, "CPS_BUAP_LoadingSession::getNumberOfCurrentCmdLog() returns", 0));

    return cmdLogSubfileNum;
}

//*****************************************************************************
//  getNumberOfCmdLogSubfile()
//*****************************************************************************
int CPS_BUAP_LoadingSession::getNumberOfCmdLogSubfile(const std::string& fileName, size_t offset)
{
    newTRACE((LOG_LEVEL_INFO, "CPS_BUAP_LoadingSession::getNumberOfCmdLogSubfile(%s)", 0, fileName.c_str()));

    int numberCmdLog = 255;
    const int off = -511;
    const size_t maxSize = 9;
    char buf[maxSize];
    // Open RELCMDHDF file
    int file = ::open(fileName.c_str(), O_RDONLY);
    if (file != -1)
    {
        long new_pos = -1L;
        if (offset == 0)
        {
            // Seek to the last block
            new_pos = ::lseek(file, off, SEEK_END);
        }
        else
        {
            // Seek to the given offset position
            new_pos = ::lseek(file, offset, SEEK_CUR);
        }

        if (new_pos == -1L)
        {
            TRACE((LOG_LEVEL_WARN, "::lseek failed!", 0));
        }
        else
        {
            // Read the number of command log subfiles in hex type
            long bytesRead = ::read(file, buf, maxSize - 1);
            if (bytesRead == -1L)
            {
                TRACE((LOG_LEVEL_WARN, "::read failed!", 0));
            }
            else
            {
                // Convert from hex to integer
                buf[maxSize - 1] = '\0';
                stringstream ss;
                ss << std::hex << buf;
                ss >> numberCmdLog;
            }
        }

        ::close(file);
    }
    else
    {
        TRACE((LOG_LEVEL_WARN, "File: %s not exist!", 0, fileName.c_str()));
    }

    return numberCmdLog;
}
