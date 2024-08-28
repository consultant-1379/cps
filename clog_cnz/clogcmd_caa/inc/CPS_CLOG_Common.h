#include "CPS_CLOG_TimeDate.h"
#define DIRCMD "ls -1"
#define CLOGDIR "//data//opt//ap//internal_root//audit_logs//command_logs//"
#define CLOGFILE "CLogfile-*" 
/// location to search for cmdlls.ini
//#define INI_PATH_0 "./"
///data/opt/ap/internal_root/audit_logs/CLOG/cmdlls.ini"

/// alternative location to search for cmdlls.ini
#define INI_PATH_1 "//opt//ap//cps//conf//cmdlls.ini"

/// location to search for cmdlls.ini
//#define INI_PATH_2 ""

/// all paths to search for cmdlls.ini
#define ALL_INI_PATHS INI_PATH_1
//, INI_PATH_1, INI_PATH_2

#define OUTDIR "/data/opt/ap/internal_root/cp/mml/"
namespace NSCommand
{	
	//HWC Command Exit Codes
	enum Exit_Code
	{
		Exit_Success =					0,
		Exit_Incorrect_Usage =			2,
		Exit_Invalid_Option1 =			3,
		Exit_Invalid_value =			4,
		Exit_Invalid_Option2 =			5,
		Exit_CmdFile_Exists =			6,	
		Exit_CmdFile_AccessErr =		7,
		Exit_IniFile_AccessErr =		8,
		Exit_LogFile_AccessErr =		9,
		Exit_Cmd_Not_Allowed   =       10,
		Exit_Event_NotFound    =        11,
		Exit_Conflict_Err      =        13,
	        Exit_UnsuccCmdCall     =       14,
		Exit_NoReply_Cs        =        15,
		Exit_Error_Executing =			20,
		Exit_EndOf_File        =               21,
		Exit_NoConfig_Found     =               22,
		Exit_Illegal_Cmd        = 	115,
		Exit_Unable_Contact_Server =	117 // Blade Cluster exit code
		
		
	};
	enum CommandName
   	{
		CMDLLS =0
	};

	
}
enum
{
   /// input lines lines longer than this cause an error.
   MAX_LINE_LENGTH = 256,

   /// max. number of change commands in .INI file
   MAX_CEI = 10,

   /// commands more than this before a backup are completely ignored
   /// when reading the input file.
   OUTDATED = 48*TimeDate::SECONDS_PER_HOUR,

   /// a time by which 'now' is shifted into the future to give the
   /// operator a little time to fisnish her job.
   WAITING_PERIOD = 15*TimeDate::SECONDS_PER_MINUTE,

   /// incomplete groups older than this before a backup are not
   /// commented out, but not written at all.
   RATHER_OLD = 30*TimeDate::SECONDS_PER_HOUR,

   /// if \b DO_KETSPLIT == 0 then incompatible keysets are (maybe) marked.
   /// Otherwise they are split into disjoint sets (duplicating the command).
   DO_KETSPLIT = 1,

   /// if \b MARK_CONFLICTIG_KEYSETS == 0 then we ignore incompatible keysets.
   /// Otherwise we mark them as conflict.
   MARK_CONFLICTING_KEYSETS = 0,
};

