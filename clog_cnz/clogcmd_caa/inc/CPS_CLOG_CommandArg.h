#ifndef CPS_CLOG_CommandArg_h
#define CPS_CLOG_CommandArg_h
#include <map>
#include <string>
#include <vector>
#include "CPS_CLOG_Common.h"
#include "CPS_CLOG_TimeDate.h"
//struct for all data related to command options
struct commandStruct
{
	std::string currCommand;			//current command executed	
	std::string date_timeStr;
	TimeDate date_time;
	std::string cmdfile;
	std::string nowStr;
	TimeDate now;
	std::string out_file_name;
	bool outFile;
	std::string in_file_name;
	std::string ini_file_name;
	std::string clogpath;
	bool hide_result;
	NSCommand::Exit_Code exitCode;		//the exit code for the command
	std::string exitCodeDescriptionArg;	//the description argument for the command's exit code
};


class CommandArg
{
	public:			
		CommandArg(std::string currCommand);
		virtual ~CommandArg();
		
		
		bool checkCmdllsFormat(int argc, char* argv[]);
		//functions for handling exit codes and descriptions for them
		unsigned short getExitCode();
		std::string getExitCodeDescription(const unsigned short exitCode);
		std::string getExitCodeDescriptionArg();
		bool handleCmdlls(int argc, char* argv[]);
		static void usage(const char * prog, bool brief);
		void logging(int argc,char* argv[]);
	private:
		//functions to check command line arguments
	
		commandStruct* commStruct;				//struct holding option and option argument values
		bool checkFormat(int argc, char* argv[]);
		bool checkCmdllsOptions(int argc, char* argv[]);
		bool faultyOption(NSCommand::Exit_Code, std::string faultyOptionArg);
		bool faultyOption(NSCommand::Exit_Code);

		bool checkArgTime();
		void clearStruct(commandStruct* commStruct);
		void setExitCodes();
		
		typedef std::map<unsigned int, std::string> exitCodeMap1;	//map holding the exit code descriptions	
		exitCodeMap1 exitCodeMap;
		bool isMultipleCpSystem(bool &);
	        bool isAp1();	
};
#endif
