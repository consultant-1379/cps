#include "CPS_CLOG_CommandExitCodes.h"

#include <sstream>
#include <string>
using namespace std;

//constructor
Command_Exitcodes::Command_Exitcodes()
   :  exitCode(NSCommand::Exit_Success)
{
   this->initializeExitCodeDescriptions();   //initialize the exit code descriptions
}

//destructor
Command_Exitcodes::~Command_Exitcodes()
{
}

void Command_Exitcodes::initializeExitCodeDescriptions()
{
   exitCodeMap[NSCommand::Exit_Success]         = "Successful execution of cmdlls";
   exitCodeMap[NSCommand::Exit_Error_Executing] = "General Fault";
   exitCodeMap[NSCommand::Exit_Incorrect_Usage] = (string)"Incorrect usage\n" +
         "Usage: cmdlls -t date_time [ -o cmdfile ] [ -d ]\n\n";
   exitCodeMap[NSCommand::Exit_NoReply_Cs]                = "No Reply from Configuration Service";
   exitCodeMap[NSCommand::Exit_Invalid_Option1]          = "Invalid format of -t option";
   exitCodeMap[NSCommand::Exit_Invalid_value]            = "Invalid -t value";	
   exitCodeMap[NSCommand::Exit_Invalid_Option2]           = "Invalid format of -o option"; 
   exitCodeMap[NSCommand::Exit_CmdFile_Exists]        = "Cmdfile already exists"; 
   exitCodeMap[NSCommand::Exit_CmdFile_AccessErr]            = "Cmdfile access error (open, write)";
   exitCodeMap[NSCommand::Exit_IniFile_AccessErr]  = "cmdlls.ini file access error (open, read)";
   exitCodeMap[NSCommand::Exit_LogFile_AccessErr]            = "Command log file access error (open, read)"; 
   exitCodeMap[NSCommand::Exit_Cmd_Not_Allowed]  = "Only allowed from IO APG"; 
   exitCodeMap[NSCommand::Exit_UnsuccCmdCall]  = "Unsuccessful call of alogfind"; 
   exitCodeMap[NSCommand::Exit_Event_NotFound]            = "No backup event found (SYBUP, AUTODUMP)";
   exitCodeMap[NSCommand::Exit_Conflict_Err]                    = "Conflicting MML commands found in command log file";
   exitCodeMap[NSCommand::Exit_EndOf_File]                = "END of file";
   exitCodeMap[NSCommand::Exit_NoConfig_Found]                = "No configuration commands found";
   exitCodeMap[NSCommand::Exit_Unable_Contact_Server]           = "Unable to connect to server"; 
   exitCodeMap[NSCommand::Exit_NoReply_Cs]                = "No Reply from Configuration Service";
   exitCodeMap[NSCommand::Exit_Illegal_Cmd]                = "Illegal command in this system configuration";
   }
NSCommand::Exit_Code Command_Exitcodes::getExitCode()
{
   return this->exitCode;
}

string Command_Exitcodes::getExitCodeDescription()
{
   ExitCodeMapType::const_iterator pos = this->exitCodeMap.find(this->exitCode);
   if( pos != exitCodeMap.end() )
   {
      return pos->second;
   }
   else
   {
      return exitCodeMap[NSCommand::Exit_Error_Executing];
   }

}

void Command_Exitcodes::setExitCode(NSCommand::Exit_Code exitCodeToSet)
{
   this->exitCode = exitCodeToSet;
}
