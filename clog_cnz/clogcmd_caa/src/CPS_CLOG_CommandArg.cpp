
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include "CPS_CLOG_CommandArg.h"
#include "ACS_CS_API.h"
#include "CPS_CLOG_CommandImp.h"
#include "ACS_APGCC_Util.H"
using namespace std;
/// a string describing build details.
extern const char * buildtag = "137M Wed Oct 13 10:58:45 CEST 2010 deacxl029 eedjsa ";

/// filename where an error has occured (0 if none)
const char * error_filename = 0;

/// line number where an error has occured
int error_line = 0;

/// command line argument where an error has occured (0 if none)
const char * error_argv = 0;

/// command line argument number where an error has occured
int error_argc = 0;
LogItem log_control = LogItem(LOG_NORMAL | LOG_PROGRESS);

/// true if usage shall be shown
static bool show_usage = false;

/// true if result shall be hidden (e.g. when -h was used)
static bool hide_result = false;

//static void usage(const char * prog, bool brief);
//constructor

CommandArg::CommandArg(std::string currCmd)
   : commStruct(0)
{

   this->commStruct = new commandStruct;  //creates a pointer to the struct commandStruct
   this->clearStruct(this->commStruct);   //sets default values for the variables in the struct
   this->commStruct->currCommand = "CMDLLS"; //assigns the current command name to the struct var
   this->setExitCodes();    //sets the exit code descriptions for the current command

   
}

//destructor
 CommandArg::~CommandArg()
{
   delete this->commStruct;
   
}


bool CommandArg::faultyOption(NSCommand::Exit_Code exitCode, string faultyOptionArg)
{
   clearStruct(this->commStruct);         //clears the contents of the struct
   this->commStruct->exitCode = exitCode; //assigns the exit code to the commandstruct
   this->commStruct->exitCodeDescriptionArg = faultyOptionArg; //assigns faulty option/option arg to the commandstruct
   return false;
}

bool CommandArg::faultyOption(NSCommand::Exit_Code exitCode)
{
   this->commStruct->exitCode = exitCode;
   (void)this->faultyOption(exitCode, "");
   return false;
}


//analyzes the options given in the command
bool CommandArg::checkCmdllsOptions(int argc, char* argv[])
{
   const char * prog = argv[0];
   const char * in_file_name = 0;
   this->commStruct->outFile = false;
   const char * clogpath = CLOGDIR;
   this->commStruct->clogpath=clogpath;
   string arg = "";
   string optionString = "";
   string commandString = this->commStruct->currCommand;
   bool option_t = false;
   bool option_o = false;
   bool option_d = false;
    
   if (commandString == "CMDLLS")
   {
      if (argc ==1)
      {
         return faultyOption(NSCommand::Exit_Incorrect_Usage);
      }
   }

   string illegalOption = "";

   //check that only valid options have been used
       for (int i = 1; i < argc; ++i)
       {
         const char * sopt = argv[i];
         arg = argv[i];  
         error_argc = i;
         error_argv = sopt;
         const bool more = (i < (argc - 1));    
	
	   
      //        string currOption = optionString.substr(i,1);
	 if (!strcmp(sopt, "-h"))
         {		
	     this->commStruct->hide_result = true;
	     usage(prog, false);
	     return true;
	 }
	 	
	 else if (!strcmp(sopt, "--help"))
         {
	     this->commStruct->hide_result = true;
             usage(prog, false);
             return true;
         }
         else if(!strcmp(sopt, "-a"))
         {
              this->commStruct->clogpath = "";   // use ALOGFIND
              log_control = LogItem(log_control & ~LOG_PROGRESS);
         }
         else if (!strcmp(sopt, "-c")&& more) 
         {
              this->commStruct->clogpath = argv[++i];
         }
         else if (!strcmp(sopt, "-I")&& more)
         {
              this->commStruct->ini_file_name = argv[++i];
         }
         else if (!strcmp(sopt, "-i")&& more)
         {
              this->commStruct->in_file_name = argv[++i];
         }
         else if (!strcmp(sopt, "-o")&& more) 
         {
	      option_o = true;
	      std::string outdir = OUTDIR;
          this->commStruct->out_file_name = outdir + argv[++i];
          //std::cout<< "path:" << this->commStruct->out_file_name << std::endl;
	      this->commStruct->outFile = true;
         }
	 else if (!strcmp(sopt, "-d"))
         {
	      option_d = true;
              this->commStruct->nowStr = "future";
         }
         else if (!strcmp(sopt, "-n")	&& more) 
         {
	      
              this->commStruct->nowStr = argv[++i];
         }
         else if (!strcmp(sopt, "-t")&& more)
         {
	      option_t = true;
	      this->commStruct->date_timeStr = argv[++i];	
         }
         else if (!strcmp(sopt, "-v")&& more) 
         {
              log_control  = LogItem(atoi(argv[++i]));
         }
         else if (!strcmp(sopt, "-V"))
         {
              log_control = LOG_V;
         }
         else if (!strcmp(sopt, "-VV"))
         {
              log_control = LOG_ALL;
              cerr << argv[0] << ", build: " << buildtag << endl;
         }
	 else
	 {
		return faultyOption(NSCommand::Exit_Incorrect_Usage);

	 }
          
	 if (arg.substr(0,1) == "-" && arg.size() != 2) 	
         {
               return faultyOption(NSCommand::Exit_Incorrect_Usage);
         }
		
    }

	  if((option_t != true))// && ((option_o == true)||(option_d == true)))
          {
            return faultyOption(NSCommand::Exit_Incorrect_Usage);
          }
	  error_argv = 0;


	  if (clogpath && in_file_name)
          {
             show_usage = true;
             return faultyOption(NSCommand::Exit_Incorrect_Usage);
 	  }	


   	  if (! checkArgTime()) //CR GESB
            return faultyOption(NSCommand::Exit_Invalid_Option1,this->commStruct->date_timeStr.c_str());

	//check for more than one occurrence of an option
        if (optionString.find("t") != optionString.rfind("t") || optionString.find("o") != optionString.rfind("o") ||
        optionString.find("d") != optionString.rfind("d") )
        return faultyOption(NSCommand::Exit_Incorrect_Usage);
	if(!handleCmdlls(argc,  argv))
	return false;
	else 
	return true;
   return true;
}



void CommandArg::clearStruct(commandStruct* cStruct)
{
   cStruct->currCommand = "CMDLLS";
   TimeDate time;
   cStruct->date_time = time;
   cStruct->cmdfile="";
   cStruct->now = time;	
   cStruct->exitCode = NSCommand::Exit_Code(0);
   cStruct->exitCodeDescriptionArg = "";
}

void CommandArg::setExitCodes()
{
   exitCodeMap[0] = "Successful execution of cmdlls";
   exitCodeMap[20] = "General Fault";

   exitCodeMap[2] = (string)"Incorrect usage\n" +
         "Usage: cmdlls -t date_time [ -o cmdfile ] [ -d ]\n\n";
   exitCodeMap[7] = "No Reply from Configuration Service";
   exitCodeMap[3] = "Invalid format of -t option";
   exitCodeMap[4]= "Invalid -t value";	
   exitCodeMap[5] = "Invalid format of -o option";
   exitCodeMap[6] = "Cmdfile already exists";
   exitCodeMap[7] = "Cmdfile access error (open, write) ";
   exitCodeMap[8] = "cmdlls.ini file access error (open, read)";
   exitCodeMap[9] = "Command log file access error (open, read)";
   exitCodeMap[10] = "Only allowed from IO APG";
   exitCodeMap[11] = "No backup event found (SYBUP, AUTODUMP)";
   exitCodeMap[13] = "Conflicting MML commands found in command log file ";
   exitCodeMap[14] = "Unsuccessful call of alogfind ";
   exitCodeMap[21] = "END of file";
   exitCodeMap[22] = "No configuration commands found"; 
   exitCodeMap[NSCommand::Exit_Unable_Contact_Server] = "Unable to connect to server"; // Blade Cluster exit code
   exitCodeMap[NSCommand::Exit_NoReply_Cs]                = "No Reply from Configuration Service";
   exitCodeMap[NSCommand::Exit_Illegal_Cmd]                = "Illegal command in this system configuration";

}

unsigned short CommandArg::getExitCode()
{
   return this->commStruct->exitCode;
}

string CommandArg::getExitCodeDescription(const unsigned short exitCode)
{
   return this->exitCodeMap[exitCode];
}

string CommandArg::getExitCodeDescriptionArg()
{
   return this->commStruct->exitCodeDescriptionArg;
}

bool CommandArg::checkCmdllsFormat(int argc, char* argv[])
{
   bool retval = false;
   bool isMulti =  false;
   retval = isMultipleCpSystem(isMulti);
   if(!retval )
   {
        return faultyOption(NSCommand::Exit_NoReply_Cs);
   }
   else if(!isMulti)	
   {
	return faultyOption(NSCommand::Exit_Illegal_Cmd);
   }
   else
   {    
        //Check for Ap1
	if(!isAp1())
	return faultyOption(NSCommand::Exit_Cmd_Not_Allowed);
	//analyzes the options given in the command
	bool ret =  this->checkCmdllsOptions(argc, argv);  
	if(!ret)
	   return false;
   }
   if (log_control & LOG_DEBUG)   cerr << "Validation of Options OK.\r\n";

   return true;
}

void CommandArg::logging(int argc, char* argv[])
{
      if (log_control == LOG_NORMAL ||
       log_control == (LOG_NORMAL | LOG_PROGRESS))   // short form
      {
         if (!hide_result)
         {
		int exitCode = getExitCode();
		if(exitCode)
		cout<<endl<<getExitCodeDescription(exitCode)<<" "<<getExitCodeDescriptionArg()<<endl;

              if (Command::time_removed_count)
                 cerr << Command::time_removed_count
                      << " TIME/DATE parameters removed\r\n";

              if (Command::before_backup_count)
                 cerr << "  " << Command::before_backup_count
                      << " commands before backup\r\n";

         }

         if (show_usage)   usage(argv[0], true);
      }
     else                            // verbose form
     {
	short unsigned int exitCode = getExitCode();
        if (exitCode)
        {
             cerr << "Program " << argv[0] << " failed.";

             cerr << "\r\n  Error code: " << exitCode << "\r\n";
             if (exitCode < 1000)
                cerr << "  Error text: " << getExitCodeDescription(exitCode) << endl
                     << "  Exit code:  " <<exitCode  << endl;


             if (error_filename)
             {
                  cerr << "  File: " << error_filename;
                  if (error_line)
                     cerr <<  "  line " << error_line;
                  cerr <<  "\r\n";
             }

             if (error_argv)
             {
                  cerr << "  Arg[" << error_argc << "]     "
                       << error_argv << "\r\n";
             }
         }
      }

}

bool CommandArg::isMultipleCpSystem(bool &isMulti)
{
  ACS_CS_API_NS::CS_API_Result resultcs = ACS_CS_API_NetworkElement::isMultipleCPSystem(isMulti);
  if (resultcs == ACS_CS_API_NS::Result_Success)
  {
     return true;
  }
  else
  {
      return false;
  }
}
bool CommandArg::isAp1()
{
	ACS_CS_API_CommonBasedArchitecture::ApgNumber apgNumber;

	ACS_CS_API_NS::CS_API_Result resultcs = ACS_CS_API_NetworkElement::getApgNumber(apgNumber);

	if (ACS_CS_API_NS::Result_Success == resultcs
			&& ACS_CS_API_CommonBasedArchitecture::AP1 == apgNumber)
		return true;
	else
		return false;
}
bool CommandArg::handleCmdlls(int argc, char* argv[])
{
     if (log_control & LOG_DEBUG)   cerr << "Handling cmdlls\r\n";

     const char * in_file_name = this->commStruct->in_file_name.c_str();
     const char * ini_file_name = this->commStruct->ini_file_name.c_str();
     TimeDate timestamp = this->commStruct->date_time;
     const char * out_file_name = this->commStruct->out_file_name.c_str();
     bool outFile = this->commStruct->outFile;
     const char * clogpath =  this->commStruct->clogpath.c_str();
     TimeDate now_stamp = this->commStruct->now;
     this->commStruct->exitCode = Command::do_all(ini_file_name, in_file_name, clogpath, timestamp,
                          out_file_name, outFile, now_stamp,  argc, argv);


      if(this->commStruct->exitCode)
	return false;
      else
	return true;
}
bool CommandArg::checkArgTime()
{
      if (log_control & LOG_DEBUG)   cerr << "Checking Time parameters validity .\r\n";

    TimeDate backup_stamp;
    bool retVal = backup_stamp.parse(this->commStruct->date_timeStr.c_str(), TF_20YYMMDD_hhmm);
    if (backup_stamp.get_mode()  == TM_PAST ||
       backup_stamp.get_mode()  == TM_FUTURE ||
       backup_stamp.get_mode()  == TM_INVALID)
    {
        show_usage = true;
        return faultyOption(NSCommand::Exit_Invalid_Option1, this->commStruct->date_timeStr.c_str());
    }
    if(retVal)
    {
	this->commStruct->date_time = backup_stamp;
    }
    else
    {
	show_usage = true;
        return faultyOption(NSCommand::Exit_Invalid_Option1,this->commStruct->date_timeStr.c_str()); 
    }
    	
    TimeDate now_stamp;
    bool retval = false;
    if(this->commStruct->nowStr.size())
    {	
    	retval = now_stamp.parse(this->commStruct->nowStr.c_str(), TF_20YYMMDD_hhmm);

    }  
    else 
    {
	 retval = now_stamp.read_system_time();
    }
    if (!retval)
    {
        show_usage = true;
        return faultyOption(NSCommand::Exit_Invalid_Option1);
    }

   if (now_stamp.get_mode()  == TM_NO_STAMP ||
       now_stamp.get_mode()  == TM_INVALID)
      {
        cerr << "BAD -N option: " << this->commStruct->nowStr << endl;
        show_usage = true;
        return faultyOption(NSCommand::Exit_Invalid_Option1);
      }
   this->commStruct->now = now_stamp; 
    return retVal;
}


//-----------------------------------------------------------------------------
/** print the command line options to cerr. There is a brief form that
 ** displays the parameters normally used by an operator, and a long
 ** form that shows additional expert and debug options.
 **/
void CommandArg::
usage(const char * prog, bool brief)
{
   // the path separator is \ or /. We steal it from CLOGDIR.
   //
   const char * sep = CLOGDIR;
   sep += strlen(sep) - 1;

   // default help text is short and does not show debug options.
   //
   if (brief)
   {
        cerr <<
        "usage: " << prog << " -t date-time [ -o cmdfile ] [ -d ]\r\n\r\n";
        return;
   }

   cerr <<
    "Usage:" << endl <<
    "    " << prog << " [ option ... ]\r\n"
    "\r\n"
    "options:\r\n"
    "\r\n"
    "    -t YYMMDD-hhmm     write commands after backup YYMMDDhhmm)\r\n"
    "    -o output_file     write result to <output_file>     (default: stdout)\r\n"
    "    -d                 same as -n future\r\n"
    "    -a                 run ALOGFIND to create events\r\n"
    "\r\n"
    "debug options:\r\n"
    "\r\n"
    "    -i infile          read events from <infile>      (in ALOGFIND format)\r\n"
    "    -c clogdir         read events from clogdir" << sep << "*\r\n"
    "    -I ini_file        use INI file <ini_file>       (default: cmdlls.ini)\r\n"
    "    -v verbosity       verbosity (bitmask)\r\n"
    "    -V                 same as -v " << LOG_V << "\r\n"
    "    -VV                same as -v " << LOG_ALL << "\r\n"
    "    -t none            write all commands\r\n"
    "    -n past            'now' is before all commands\r\n"
    "    -n YYMMDD-hhmm     'now' is YYMMDD-hhmm\r\n"
    "    -n future          'now' is after all commands and TIME parameters\r\n"
    "    -h                 print this text\r\n"
    "    --help             print this text\r\n"
    "\r\n";
}
