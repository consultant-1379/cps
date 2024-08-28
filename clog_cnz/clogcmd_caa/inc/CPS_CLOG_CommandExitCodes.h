#include <map>
#include <string>
#include "CPS_CLOG_CommandArg.h"
class Command_Exitcodes
{
 public:
   //constructors
   Command_Exitcodes();

   //destructor
   virtual ~Command_Exitcodes();

   //functions for handling exit codes and descriptions for them
   void setExitCode(NSCommand::Exit_Code exitCode);
   NSCommand::Exit_Code getExitCode();
   std::string getExitCodeDescription();

 private:
   typedef std::map<unsigned int, std::string> ExitCodeMapType;
   //function for initializing all exit codes and their descriptions
   void initializeExitCodeDescriptions();

   //members
   ExitCodeMapType exitCodeMap;   //map holding the exit code descriptions
   NSCommand::Exit_Code exitCode;                    //variable holding current exit code
};
