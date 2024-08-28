//******************************************************************************
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 2012.
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
//  CPS_BUAP_RPF.h
//
//  DESCRIPTION 
//  This class implements the RELADMPAR file.  It creates it
//  filling default field values if it doesn't exist. It
//  implements read and write methods for each of the records
//  within the file.  Note that access is on a record basis
//  which means that if a single field is to be updated then
//  then whole record must be read first, the field updated,
//  and then the whole record written.
//
//  Note that on instantiation the contents of the RELADMPAR file
//  are read and held in memory.  Subsequent reads will use the
//  copy in memory rather then in the file.  Methods updating
//  records are also implemented updating the memory copy.  Only
//  when the FinishUpdate method is invoked are the values actually
//  written back to the file.
//
//  The file is organised in 6 records. Each record is 32 bytes
//  in length.  However, read and writes to the file are made
//  to the whole file, i.e. 6*32bytes is the size of all file
//  operations.
//
//  DOCUMENT NO
//  190 89-CAA 109 1412
//
//
//  AUTHOR
//  1999/11/02 by  UAB/B/SF Birgit Berggren  
//                 (Removal of method StartUpdate. 
//                 m_fd (file descriptor) changed to m_fh (file handle). )                
//
//******************************************************************************
// === Revision history ===
//  991103 BIR  PA1    Created.
//  101010 xchihoa     Ported to Linux for APG43L.
//  120314 xngudan     implement Object Manager.
//******************************************************************************

#ifndef CPS_BUAP_RPF_H
#define CPS_BUAP_RPF_H

const unsigned char Valid                 = 1;  // Boolean true value
const unsigned char Invalid               = 0;  // Boolean false value

const unsigned char General               = 1;  // Is file valid (always true)
const unsigned char StaticPars            = 2;  // Bit flag for validity of static pars
const unsigned char DynamicPars           = 4;  // Bit flag for validity of dynamic pars
const unsigned char LastAutoReload        = 8;  // Bit flag for validity of LAR
const unsigned char CommandLogReferences  = 16;  // Bit flag for validity of clogrefs
const unsigned char DelayedMeasures       = 32;  // Bit flag for validity of measures

const int MAX_MEASURES = 8;

//
// Default values for the static parameters
//
const unsigned char c_ManAuto      = 1;
const unsigned char c_NTAZ         = 0;
const unsigned char c_NTCZ         = 10;
const unsigned char c_LOAZ         = 0;

const unsigned char c_INCLAcentury = 19;
const unsigned char c_INCLAyear    = 90;
const unsigned char c_INCLAmonth   = 1;
const unsigned char c_INCLAday     = 1;

const unsigned char c_INCL1        = 0;
const unsigned char c_INCL2        = 0;
const unsigned char c_SUP          = 60/10;


struct sValidities
{
  unsigned char  Value;         // Bit encoded validity flags
  unsigned char  unused[31];        // Padding
};


struct sStaticParameters
{
  unsigned char ManAuto;        // Manual (1) or Automatic (0)
  unsigned char NTAZ;           // Number of trucation attempts
  unsigned char NTCZ;           // Number of commands to be truncated
  unsigned char LOAZ;           // Log subfile ommision attempt

  // Date of Oldest accepted on-line generation file included in reload attempts
  unsigned char INCLAcentury;       // Century (-1!!! i.e. 19 for 20th)
  unsigned char INCLAyear;      // Year (89 for 1989)
  unsigned char INCLAmonth;     // Month 1 - 12
  unsigned char INCLAday;       // Day 1 - 31

  unsigned char INCL1;          // Files from first file range 0 / 1
  unsigned char INCL2;          // Files from second file range
  unsigned char SUP;            // Supervision time in tens of minutes
  unsigned char unused[21];     // Padding
};


struct sDate
{
  unsigned char century;        // Century (-1!!! i.e. 19 for 20th)
  unsigned char year;           // Year (89 for 1989)
  unsigned char month;          // Month 1 - 12
  unsigned char day;            // Day 1 - 31
  unsigned char hour;           // Hour 0 - 23
  unsigned char minute;         // Minute 0 - 59
};


struct sDynamicParameters
{
  unsigned char State;          // State of current reload 1 - 6
  unsigned char CTA;            // Current truncation attempt
  unsigned char NumFFR;         // File number in FFR
  unsigned char NumSFR;         // File number in SFR
  unsigned char NOld;           // Highest file number in FFR
  unsigned char KOld;           // Highest accepted file 0=invlaid, 100 -
  unsigned char NCT;            // Number of current truncated commands
  sDate         EXP;            // Expiring time for supervision
  unsigned char LPF;            // Last permission file (boolean), only valid
                    // when State = 6
  unsigned char Omission;       // Ommision
  unsigned char unused[17];     // Padding
};


struct sLastAutoReload
{
  unsigned char ManAuto;        // Manual (1) or Automatic (0)
  unsigned char LRG;            // Generation file
  sDate         OPSRS;          // PSRS output time
  sDate         ODSS;           // DS small output time
  sDate         ODSL;           // DS large output time
  unsigned char LRGF;           // Last possible generation file loaded 1=true
  unsigned char NCT;            // Number of truncated commands
  unsigned char Omission;       // Omission
  unsigned char SUP;            // Supervision time in tens of minutes
  unsigned long AssClogNum;     // Associated command log subfile number
  unsigned char unused[4];      // Padding
};


struct sCommandLogReferences
{
  unsigned long Current;        // Current command log subfile number
  unsigned long Prepared;       // Prepared command log subfile number
  unsigned long Main;           // Main store associated clog subfile number
  unsigned char unused[20];     // Padding
};


struct sDelayedMeasures
{
  unsigned char Measures[MAX_MEASURES]; // Delayed measures, 1=true
  unsigned char unused[24];     // Padding
};


struct sReladmpar
{
  sValidities              Validities;      // Validity flags record
  sStaticParameters        StaticPar;       // Static parameters record
  sDynamicParameters       DynamicPar;      // Dynamic parameters record
  sLastAutoReload          LastAutoReload;  // Last Auto Reload record
  sCommandLogReferences    CLogRefs;        // Command log references record
  sDelayedMeasures         DelayedMeasures; // Delayed Measures record
};


class CPS_BUAP_RPF
{
public:

  CPS_BUAP_RPF(const char *filename);
  // Description:
  //    Class constructor
  // Parameters: 
  //    -
  // Return value: 
  //    -
  // Additional information:
  //    -


  virtual ~CPS_BUAP_RPF();
  // Description:
  //    Class destructor
  // Parameters: 
  // Return value: 
  //    -
  // Additional information:
  //    -


  void ReadValidities(sValidities &validities);
  // Description:
  //    Method used to read the validity flags record
  // Parameters: 
  //    validities      Validities record, out
  // Return value: 
  //    -
  // Additional information:
  //    -


  void ReadStaticParameters (sStaticParameters &staticPar);
  // Description:
  //    Method used to read the static parameters record
  // Parameters: 
  //    staticPar       Static parameters record, out
  // Return value: 
  //    -
  // Additional information:
  //    -


  void WriteStaticParameters(sStaticParameters &staticPar);
  // Description:
  //    Method used to write the static parameters record
  // Parameters: 
  //    staticPar       Static parameters record, in
  // Return value: 
  //    -
  // Additional information:
  //    -


  void ReadDynamicParameters (sDynamicParameters &dynamicPar);
  // Description:
  //    Method used to read the dynamic parameters record
  // Parameters: 
  //    dynamicPar      Dynamic parameters record, out
  // Return value: 
  //    -
  // Additional information:
  //    -


  void WriteDynamicParameters(sDynamicParameters &dynamicPar);
  // Description:
  //    Method used to write the dynamic parameters record
  // Parameters: 
  //    dynamicPar      Dynamic parameters record, in
  // Return value: 
  //    -
  // Additional information:
  //    -


  void ReadLastAutoReload (sLastAutoReload &lastAutoReload);
  // Description:
  //    Method used to read the last auto reload record
  // Parameters: 
  //    lastAutoReload      Last auto reload record, out
  // Return value: 
  //    -
  // Additional information:
  //    -


  void WriteLastAutoReload(sLastAutoReload &lastAutoReload);
  // Description:
  //    Method used to write the last auto reload record
  // Parameters: 
  //    lastAutoReload      Last auto reload record, in
  // Return value: 
  //    -
  // Additional information:
  //    -


  void ReadCommandLogReferences (sCommandLogReferences &clogRefs);
  // Description:
  //    Method used to read the command log references record
  // Parameters: 
  //    clogRefs        Command log references record, out
  // Return value: 
  //    -
  // Additional information:
  //    -


  void WriteCommandLogReferences(sCommandLogReferences &clogRefs);
  // Description:
  //    Method used to write the command log references record
  // Parameters: 
  //    clogRefs        Command log references record, in
  // Return value: 
  //    -
  // Additional information:
  //    -


  void ReadDelayedMeasures (sDelayedMeasures &delayedMeasures);
  // Description:
  //    Method used to read the delayed measures record
  // Parameters: 
  //    delayedMeasures     Delayed measures record, out
  // Return value: 
  //    -
  // Additional information:
  //    -


  void WriteDelayedMeasures(sDelayedMeasures &delayedMeasures);
  // Description:
  //    Method used to write the delayed measures record
  // Parameters: 
  //    delayedMeasures     Delayed measures record, in
  // Return value: 
  //    -
  // Additional information:
  //    -


  void FinishUpdate();
  // Description:
  //    Method used to indicate the end of an update to the file. This
  //    actually implements the update to the file.
  // Parameters: 
  //    -
  // Return value: 
  //    -
  // Additional information:
  //    -


  const sReladmpar& getReloadFile() const;
  // Description:
  //    Method used to get the reference of RELADMPAR file.
  // Parameters:
  //    -
  // Return value:
  //    A reference of sReladmpar
  // Additional information:
  //    -

private:
  CPS_BUAP_RPF(const CPS_BUAP_RPF& );
  // Description:
  //    Copy constructor
  // Parameters: 
  // Return value: 
  //    -
  // Additional information:
  //    Declared to disallow copying

  CPS_BUAP_RPF& operator=(const CPS_BUAP_RPF& );
  // Description:
  //    Assignment operator
  // Parameters: 
  // Return value: 
  //    -
  // Additional information:
  //    Declared to disallow assignment

  bool readReladmpar(const char* fileName, sReladmpar& reladmpar);
  // Description:
  //   Read the information from reladmpar file
  // Parameters:
  //   sReladmpar
  // Return value:
  //    - true/false
  // Additional information:
  //    -

  bool writeReladmpar(const char* fileName, sReladmpar& reladmpar);
  // Description:
  //   Write the information to reladmpar file
  // Parameters:
  //   fileName
  //   reladmpar
  // Return value:
  //    - true/false
  // Additional information:
  //    -

protected:

private:
  char          *m_Filename;        //  Full name of the RELADMPAR file
  sReladmpar     m_Reladmpar;       //  Declare structure of the RELADMPAR file
  int            m_fh;              //  File handle of open RELADMPAR

};

#endif
