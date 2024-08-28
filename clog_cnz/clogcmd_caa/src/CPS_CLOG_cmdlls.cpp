
// cmdlls.cpp : Defines the entry point for the console application.
//
#include "CPS_CLOG_CommandArg.h"
#include <iostream>
using namespace std;
int main(int argc, char* argv[])
{
	int exitCode = 0;
	try
	{
		CommandArg cmdlls("cmdlls");
		cmdlls.checkCmdllsFormat(argc, argv);
		cmdlls.logging(argc,argv);
		/*if(!cmdlls.handleCmdlls(argc,argv))
		{
		exitCode = cmdlls.getExitCode();
		//cout<<endl<<endl<<cmdlls.getExitCodeDescription(exitCode)<<cmdlls.getExitCodeDescriptionArg()<<endl<<endl;
		}*/

        }
	catch(...)
	{
		exitCode = NSCommand::Exit_Error_Executing;
		cout<<endl<<endl<<"Error when executing (general fault)"<<endl<<endl;
	}
	return exitCode;
}
