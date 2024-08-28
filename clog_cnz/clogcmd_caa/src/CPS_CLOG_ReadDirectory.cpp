#include "CPS_CLOG_CommandArg.h"
#include "CPS_CLOG_CommandImp.h"
#include <string.h>
#include <iostream>
#include <assert.h>
#include <stdlib.h>

using namespace std;

/**
 ** \addtogroup cmdlls CMDLLS command
 **/
/*@{*/

/* Note: the ways of reading directories are rather different in windows and
   linux. Instead of implementing both ways (which would be somewhat cleaner)
   we do a popen() on "dir /b" in windows and a "ls -1" on linux which haoppens
   to produce compatible results.

   Another reason was that the "proper" way of doing this in linux via
   opendir() did not compile on "linux managed workplaces" in Ericsson.
 */
//-----------------------------------------------------------------------------
static const char *
handle_file(const char * dir, const char * file, const TimeDate & backup)
{
   // remove leading path from file (happens only with ls -1)
   //
     {
     const char * p = 0;
     while ((p = strchr(file, '/')))    file = p + 1;
     while ((p = strchr(file, '\\')))   file = p + 1;
     }

     char buffer[MAX_LINE_LENGTH];
     sprintf(buffer, "%s%s", dir, file);
     return dup(buffer);
}
//-----------------------------------------------------------------------------

file_name_list *
read_directory(const char * dir_path, const TimeDate & backup)
{
	file_name_list * files = 0;
	char buffer[256],buffer2[256];
   	sprintf(buffer,  "%s %s%s", DIRCMD, dir_path, CLOGFILE);
	//HR86385
	sprintf(buffer2,"%s%s%s","ls ",dir_path,"|wc -l");
	char is_empty[100];
	FILE * output =  popen(buffer2,"r");
	fgets (is_empty, 100, output); //write to the char
	pclose (output);

	int check = atoi(is_empty);

	if (check == 0)
	{
	return 0;
	}
	//HR86385
	FILE * f = popen(buffer, "r");
   	if (f == 0)   // error in popen()
      	{
           if (log_control & LOG_CLOG)
           cerr << "popen(" << DIRCMD << ") failed.\r\n";
        return 0;
        }
   	for (;;)
        {
         NSCommand::Exit_Code error = Command::read_file_line(buffer, sizeof(buffer), f);
         if (error == NSCommand::Exit_EndOf_File)
	   break;   // dir/ls done

         const char * clog = handle_file(dir_path, buffer, backup);
         if (clog)
	   files = new file_name_list(clog, files);
         }

   	 pclose(f);
   	 return files;
}
//-----------------------------------------------------------------------------

/*@}*/

