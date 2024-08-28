#include "CPS_CLOG_CommandImp.h"
#include "CPS_CLOG_TimeDate.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string.h>
using namespace std;
/**
 ** \addtogroup cmdlls CMDLLS command
 **/
/*@{*/
Genre * Genre::all = 0;
Command * Command::first_in_file = 0;
Command * Command::first_group_in_file = 0;
Command * Command::last_group_in_file = 0;
int Command::conflict_number = 0;

int Command::total_command_count = 0;
int Command::NOCAT_command_count = 0;
int Command::AI_command_count = 0;
int Command::AR_command_count = 0;
int Command::PI_command_count = 0;
int Command::PE_command_count = 0;
int Command::CEI_command_count = 0;
int Command::LI_command_count = 0;
int Command::ZI_command_count = 0;
int Command::before_backup_count = 0;
int Command::time_removed_count = 0;
int Command::conflicts_detected_count = 0;
int Command::commands_without_date = 0;
int Command::AI_without_key = 0;
int Command::AI_without_arg = 0;
//=============================================================================
Genre::Genre(const char * section)
   : next(0),
     AI(0),
     AR(0),
     PI(0),
     PE(0),
     LI(0),
     ZI(0),
     key_name(0)
{
   for (int c = 0; c < MAX_CEI; ++c)   CEI[c] = 0;

   const int size = strlen(section) + 1;
   char * s = new char[size];
   strncpy(s, section, size);
   AI = s;
   uses_imsis = !strncmp(AI, "MGIAI", 6);
}
//-----------------------------------------------------------------------------
NSCommand::Exit_Code
Genre::read_ini_file(const char * ini_file_name)
{
   // try opening an cmdlls.ini. We start with the user supplied (-I) filename
   // (if any) then the default location, and then the current directory.
   //
   const char * file_names[] = { ini_file_name, ALL_INI_PATHS };

   FILE * f = 0;
   for (unsigned int ff = 0; ff < sizeof(file_names)/sizeof(*file_names); ++ff)
   {
        const char * file_name = file_names[ff];
        if (file_name)   f = fopen(file_name, "r");
        if (f)           break;   // success
   }

   if (f == 0)   // no ini file found.
   {
        if (log_control & LOG_DEBUG)
        {
             cerr << "Can't open INI file\r\n"
                  << "Returning error code " << NSCommand::Exit_IniFile_AccessErr << "\r\n";
        }

        return NSCommand::Exit_IniFile_AccessErr;
   }

   NSCommand::Exit_Code error = NSCommand::Exit_Success;
   char buffer[MAX_LINE_LENGTH];
   for (int line = 1; ; ++line)
   {
        error = Command::read_file_line(buffer, sizeof(buffer), f);
         
   if (error == NSCommand::Exit_EndOf_File)   // file done.
   {
        error = NSCommand::Exit_Success;
        break;
   }
   if (*buffer == 0)   // empty line
       continue;

   if (*buffer == '[')   // next section
   {
       char * end = strchr(buffer, ']');
       if (end == 0)
       {
           error = NSCommand::Exit_IniFile_AccessErr;
           break;
       }

       *end = 0;
       Genre * genre = new Genre(buffer + 1);
       if (all) 
           genre->next = all;
       all = genre;
       continue;
    }

    if (!strncmp(buffer, "PI=", 3))
    {
        if (all == 0) 
        {
          error = NSCommand::Exit_IniFile_AccessErr;
          break;
        }
        all->PI = dup(buffer + 3);
        continue;
    }

    if (!strncmp(buffer, "PE=", 3))
    {
              if (all == 0)   { error = NSCommand::Exit_IniFile_AccessErr;   break; }
              all->PE = dup(buffer + 3);
              continue;
    }

    if (!strncmp(buffer, "AR=", 3))
    {
         if (all == 0)   { error = NSCommand::Exit_IniFile_AccessErr;   break; }
              all->AR = dup(buffer + 3);
              continue;
    }

    if (!strncmp(buffer, "CEI=", 4))
    {
              if (all == 0)   { error =  NSCommand::Exit_IniFile_AccessErr;   break; }

              char * end = buffer + strlen(buffer);
              int cei_count = 0;
              for (char * b = buffer + 4; b < end;)
              {
                    char * blank = strchr(b, ' ');
                    if (blank == 0)   blank = end;
                    else              *blank++ = 0;

                    if (cei_count < MAX_CEI)   all->CEI[cei_count++] = dup(b);
                    else                       error = NSCommand::Exit_IniFile_AccessErr;
                    b = blank;
              }

              if (error != NSCommand::Exit_Success)   break;
              continue;
    }

    if (!strncmp(buffer, "Key=", 4))
    {
              if (all == 0)   { error =  NSCommand::Exit_IniFile_AccessErr; break; }
              all->key_name = dup(buffer + 4);
              continue;
    }

    if (!strncmp(buffer, "Logging=", 8))
    {
              if (all == 0)   { error =  NSCommand::Exit_IniFile_AccessErr; break; }
              all->LI = dup(buffer + 8);
              continue;
    }

    if (!strncmp(buffer, "ZI=", 3))
    {
              if (all == 0)   { error =  NSCommand::Exit_IniFile_AccessErr; break; }
              all->ZI = dup(buffer + 3);
              continue;
    }

    break;
    }

   fclose(f);
   return error;
}
//-----------------------------------------------------------------------------
CommandCategory
Genre::get_CommandDetails(const char * cmd, const Genre * & s)
{
   // search cmd in genre table
   //
   for (s = all; s; s = s->next)
   {
         if (s->AI && !strcmp(cmd, s->AI))   return CAT_AI;
         if (s->AR && !strcmp(cmd, s->AR))   return CAT_AR;
         if (s->LI && !strcmp(cmd, s->LI))   return CAT_LI;
         if (s->PI && !strcmp(cmd, s->PI))   return CAT_PI;
         if (s->PE && !strcmp(cmd, s->PE))   return CAT_PE;
         if (s->ZI && !strcmp(cmd, s->ZI))   return CAT_ZI;

         for (int c = 0; c < MAX_CEI; ++c)
             if (s->CEI[c] && !strcmp(cmd, s->CEI[c]))   return CAT_CEI;
   }

   return CAT_NONE;
}
//-----------------------------------------------------------------------------
void
Genre::print_ini_file(ostream & out)
{
   const char * h1 = "AI     Key   LI    AR    ZI    PI    PE    CEI ...";
   const char * l1 = "-----+------+-----+-----+-----+-----+-----+-------";
   const char * s1 = "==================================================";

   const char * l2 = "----------------------------";
   const char * s2 = "============================";

   out << l1 << l2 << endl << h1 << endl << l1 << l2 << endl;

   for (const Genre * p = all; p; p = p->next)   p->print(out);

   out << s1 << s2 << endl;
}
//-----------------------------------------------------------------------------
void
Genre::print(ostream & out) const
{
   // we dont care for MaximumLimit.
   if (!strcmp(AI, "MaximumLimit"))   return;

   out << left << setw(5) << (AI       ? AI       : "-")
       << " "  << setw(6) << (key_name ? key_name : "-")
       << " "  << setw(5) << (LI       ? LI       : "-")
       << " "  << setw(5) << (AR       ? AR      : "-")
       << " "  << setw(5) << (ZI       ? ZI      : "-")
       << " "  << setw(5) << (PI       ? PI      : "-")
       << " "  << setw(5) << (PE       ? PE       : "-");

   for (int c = 0; c < MAX_CEI; ++c)
   {
         if (CEI[c])   out << " "  << setw(5) << CEI[c];
   }

   out << right << endl;
}
//=============================================================================
Command::Command(const char * input_file, int line, const char * cmd,
                 const char * args, const TimeDate & stamp, const char * comm)
   : in_file_name (input_file),
     line_number  (line),
     timestamp    (stamp),
     original     (0),
     group_number (0),
     is_command   (true),
     conflict_ref (0),
     state        (ST_NORMAL),
     command      (cmd),
     category     (CAT_NONE),
     genre      (0),
     arguments    (args),
     date_arg     (0),
     time_arg     (0),
     imsis_arg    (0),
     imsis_cloned (false),
     key_all      (false),
     keys         (0),
     comment      (comm),
     prev_in_file (0),
     next_in_file (0),
     prev_group_in_file(0),
     next_group_in_file(0),
     first_in_group(0),
     last_in_group(0)
{
   original = this;

   category = Genre::get_CommandDetails(command, genre);

   ++total_command_count;
   if      (category == CAT_NONE)  ++NOCAT_command_count;
   else if (category == CAT_AI)    ++AI_command_count;
   else if (category == CAT_AR)    ++AR_command_count;
   else if (category == CAT_PI)    ++PI_command_count;
   else if (category == CAT_PE)    ++PE_command_count;
   else if (category == CAT_CEI)   ++CEI_command_count;
   else if (category == CAT_LI)    ++LI_command_count;
   else if (category == CAT_ZI)    ++ZI_command_count;
   else assert(0 && "Bad command category");

   // compute key value.
   //
   date_arg  = get_parameter_value("DATE");
   time_arg  = get_parameter_value("TIME");
   imsis_arg = get_parameter_value("IMSIS");

   if (genre && genre->key_name)   // this genre supports keys.
   {
        const char * kval = get_parameter_value(genre->key_name);

        if (kval == 0)                       state |= ST_KEY_BUT_NO_VAL;
        else if (!strncmp(kval, "ALL", 3))   key_all = true;
        else                                 keys = new KeySet(kval);
   }
}
//-----------------------------------------------------------------------------
Command::Command(Command * other, const KeySet * our_keys)
   : line_number  (other->line_number),
     timestamp    (other->timestamp),
     original     (other->original),
     group_number (0),
     is_command   (true),
     conflict_ref (other->conflict_ref),
     state        (ST_SPLIT_REST | ST_HIDE),
     command      (other->command),
     category     (other->category),
     genre      (other->genre),
     arguments    (other->arguments),
     date_arg     (other->date_arg),
     time_arg     (other->time_arg),
     imsis_arg    (other->imsis_arg),
     imsis_cloned (other->imsis_cloned),
     key_all      (false),
     keys         (our_keys),
     comment      (other->comment),
     prev_in_file (0),
     next_in_file (0),
     prev_group_in_file(0),
     next_group_in_file(0),
     first_in_group(other->first_in_group),
     last_in_group(0)
{
   // update forward chain
   //
   next_in_file = other->next_in_file;
   other->next_in_file = this;

   // update backward chain
   //
   prev_in_file = other;
   if (next_in_file)   next_in_file->prev_in_file = this;

   // if other was the last command in the group, then this is the last now.
   //
   assert(first_in_group);
   if (first_in_group->last_in_group == other)
      first_in_group->last_in_group = this;

   other->state |= ST_SPLIT_COMMON;
}
//-----------------------------------------------------------------------------
const char *
Command::get_parameter_value(const char * parameter_name)
{
   if (parameter_name == 0)   return 0;

   const int parameter_name_len = strlen(parameter_name);

   for (const char * a = arguments; a; )
   {
         // skip whitespace before parameter name.
         //
         while (*a && *a <= ' ')   ++a;

         // check for parameter_name
         //
         if (strncmp(a, parameter_name, parameter_name_len))   // no match
         {
              // next parameter.
              a = strchr(a, ',');
              if (a == 0)   return 0;   // no ore parameters.

              a++;   // skip ','
              continue;
         }

         // skip parameter name and following whitespace.
         //
         a += parameter_name_len;
         while (*a && *a <= ' ')   ++a;   // skip whitespaces

         if (*a++ != '=')   continue;

         while (*a && *a <= ' ')   ++a;   // skip whitespaces

         const char * end = a;
         while (isalnum(*end) || (*end == '&') || (*end == ' '))   ++end;
         const char * ret = dup(a, end);

         if (log_control & LOG_KEY_DEBUG_2)
            cerr << "VALUE=" << ret << endl;

         return ret;
    }

   return 0;
}
//-----------------------------------------------------------------------------
/** Read the input file which consists of entries like the following:

Event type:          MML command log
Date/time:           2010-05-26 160314
User:                MSCSBC024AP1D\\ADMINISTRATOR
Session ID:          243447
Command Seq Num:     3463
AP node number:      1
Local host:          MSCSBC024AP1B
Remote host:         127.0.0.1
CP name/group:
CP ID:               1,2,3,4,5
Device:              AD-4
PRCA:
Data:                CHTSI:PNUM=1,NSP=3,TDS=10,PLENGTH=20;

EXECUTED

There can be other crap in the file like:

C:\>alogfind -e 20100526 -g 20100526 -a 1555 -b 1610 -t mcl^M

We ignore everything but the Date/time: field and the Data: field.

 **/
Command *
Command::read_input_file(NSCommand::Exit_Code & error, const char * in_file_name,
                         const TimeDate & backup, bool clog_format,
                         int & line)
{
FILE * f = 0;
bool use_popen;

   if (in_file_name)   // read input from file
   {
        use_popen = false;
        f = fopen(in_file_name, "r");
        if (f == 0)
        {
             error =  NSCommand::Exit_IniFile_AccessErr;
             return 0;
        }
   }
   else   // read input from alogfind command
   {
        in_file_name = "ALOGFIND";
        use_popen = true;
        char alogfind[100];

        if (backup.get_mode() != TM_NO_STAMP)
        {
             char start_date[20];
             char start_time[20];
             TimeDate backup_48 = backup - OUTDATED;
             backup_48.write_date(start_date, sizeof(start_date), TF_YYMMDD);
             backup_48.write_time(start_time, sizeof(start_time), TF_hhmm);

            sprintf(alogfind, 
                      "alogfind -e 20%s -a %s -t mcl", start_date, start_time);
        }
        else
        {
             strncpy(alogfind, "alogfind -t mcl", sizeof(alogfind) - 2);
        }

        if (log_control & LOG_ALOGFIND)
           cerr << "running " << alogfind << "\r\n";

        f = popen(alogfind, "r");
        if (f == 0)   // error in popen()
        {
             if (log_control & LOG_ALOGFIND)
                cerr << "popen() failed.\r\n";
             error = NSCommand::Exit_UnsuccCmdCall;
             return 0;
        }
        else
        {
             if (log_control & LOG_ALOGFIND)
                cerr << "popen() succeeded.\r\n";
        }
   }

   TimeDate stamp;
   Command * first = 0;
   Command * last  = 0;

   for (line = 1; ; ++line)
   {
         char buffer[MAX_LINE_LENGTH];
         error = Command::read_file_line(buffer, sizeof(buffer), f);

         if (error == NSCommand::Exit_EndOf_File)   // file done (not a real error).
         {
              error = NSCommand::Exit_Success;
              break;
         }

         if (error)   return 0;    // a real error

         char * cmd_name = 0;

         if (clog_format)
         {
              if (buffer[0] == '#')   continue;
              if (buffer[6] != '-')
              {
                   cerr << "Bad (1) TF_20YYMMDD_hhmm: " << buffer << endl;
                   continue;
              }
              if (buffer[11] != ' ') 
              {
                   cerr << "Bad (2) TF_20YYMMDD_hhmm: " << buffer << endl;
                   continue;
              }
		buffer[11] = 0;
	
              stamp.parse(buffer,  TF_20YYMMDD_hhmm);
              if (stamp.get_mode() == TM_INVALID)
              {
                   cerr << "Bad (3) TF_20YYMMDD_hhmm: " << buffer << endl;
                   continue;
              }
	       cmd_name = buffer + 12;
          }
          else
          {
              // Date/time line ? (format: YY-MM-DD hhmmss)
              //
              if (!strncmp(buffer, "Date/time:", 10))
              {
                   stamp.parse(buffer + 10, TF_YY_MM_DD_hhmmss);
                   if (stamp.get_mode() == TM_INVALID)
                   {
                        cerr << "Bad TF_YY_MM_DD_hhmmss: " << buffer << endl;
                   }
                   continue;
               }

              if (strncmp(buffer, "Data:", 5))   continue;   // not Data:

              cmd_name = buffer + 6;
          }


         // if we have a backup time and the time of this command is long
         // before the backup, then we ignore the command in order to keep
         // our memory small even if the input file is big.
         //
         if (backup.get_mode() != TM_NO_STAMP)   // we have a backup.
         {
              if (stamp < (backup - OUTDATED))   continue;
         }


         // skip whitespaces before command.
         //
         while (*cmd_name && *cmd_name <= ' ')   ++cmd_name;
         if (*cmd_name == 0)   continue;   // empty line

         // handle comment (if any).
         //
         char * comment = strchr(cmd_name, '!');
         if (comment)
         {
              *comment++ = 0;
              while (*comment && *comment <= ' ')   ++comment;
              char * comment_end = strchr(comment, '!');
              if (comment_end)
              {
                   *comment_end = 0;
                   while ((comment < comment_end) && (comment_end[-1] <= ' '))
                    *--comment_end = 0;
              }
         }

         bool is_command = true;
         char * cmd_arg = 0;
         
         if (!strncmp(cmd_name, "AUTODUMP", 8))
         {
              is_command = false;
              cmd_name[8] = 0;
              cmd_arg = cmd_name + 9;
         }
         else if (!strncmp(cmd_name, "SYBUP", 5))
         {
              is_command = false;
              cmd_name[5] = 0;
              cmd_arg = cmd_name + 6;
         }
         else
         {
              char * semi = strchr(cmd_name, ';');
              if (!semi)   continue;
              *semi = 0;

              cmd_arg = strchr(cmd_name, ':');
              if (cmd_arg)   *cmd_arg++ = 0;
         }

         if (stamp.get_mode() == TM_INVALID)   // but no (valid) date.
         {
              ++commands_without_date;
              stamp.set_seconds(0);
              stamp.set_days(0);
         }

         Command * cmd = new Command(in_file_name, line, dup(cmd_name),
                                     dup(cmd_arg), stamp, dup(comment));

         cmd->is_command = is_command;
         if (!is_command)   cmd->state |= ST_HIDE;

         if (first)   // subsequent command
         {
              cmd->prev_in_file = last;
              last->next_in_file = cmd;
         }
         else         // first command
         {
              first = cmd;
         }

         last = cmd;
       }

       if (use_popen)
       {
        const int ret = pclose(f);
        if (ret == -1)
        {
             error = NSCommand::Exit_UnsuccCmdCall;
             return 0;
        }
       }
       else
       {
        fclose(f);
       }

   // if alogfind is missing then popen() succeeds in windows,
   // but no output is provided.
   //
      if (line < 10 && !in_file_name)   // a normal command has ~ 12 lines
      {
        if (log_control & LOG_ALOGFIND)
           cerr << "alogfind failed ( < 10 lines)\r\n";

        error = NSCommand::Exit_UnsuccCmdCall;
        return 0;
      }

      if (log_control & LOG_ALOGFIND)
      {
        if (in_file_name)
           cerr << "file " << in_file_name << " has "
                << (line - 1) << " lines\r\n";
        else
           cerr << "alogfind returned " << (line - 1) << " lines\r\n";
      }

   return first;
}
//-----------------------------------------------------------------------------
void
Command::print_all_commands(ostream & out, PrintMode pm)
{
    int out_line = 1;

    const char * format =
    "          State          | Line |Group|O-Grp| #REF |   Time    | Key "
    "|     Value      |     IMSIS      |Cat|Command                       ";

    if (pm == PR_TABLE)   // debug output
    {
        for (const char * f = format; *f; ++f)
        {
              if (*f == '|')   out << '+';
              else             out << '-';
        }
        out << endl;

        for (const char * f = format; *f; ++f)
        {
              if (*f == '|')   out << ' ';
              else             out << *f;
        }
        out << endl;

        for (const char * f = format; *f; ++f)
        {
              if (*f == '|')   out << '+';
              else             out << '-';
        }

        out << endl;

    }
    else                  // normal output
    {
        if (conflicts_detected_count == 0)
           out << "  Successful execution\r\n";
        else
           out << "  Conflicts detected, please check the result\r\n";

        if (time_removed_count)
           out << "  " << time_removed_count
               << " TIME/DATE parameters removed\r\n";

        if (before_backup_count)
           out << "  " << before_backup_count
               << " commands before backup\r\n";

        ++out_line;
     }

     for (const Command * p = first_in_file; p; p = p->next_in_file)
       p->print(out, pm, out_line++);

     const char * rr =
     "| | | | | | | | | | | | |   "                         "\n"
     "| | | | | | | | | | | | V -- Key but no value"         "\n"
     "| | | | | | | | | | | `t- -- Bad time syntax"          "\n"
     "| | | | | | | | | | | d - -- Bad date syntax"          "\n"
     "| | | | | | | | | | `k- - -- Keyset conflict"          "\n"
     "| | | | | | | | | | b - - -- Backup conflict"          "\n"
     "| | | | | | | | | `s- - - -- Sequence conflict"        "\n"
     "| | | | | | | | | t   - - -- Time removal conflict"    "\n"
     "| | | | | | | | `i- - - - -- IMSIS= conflict"          "\n"
     "| | | | | | | | Z - - - - -- Group has ZI"             "\n"
     "| | | | | | | `p- - - - - -- Group has PI"             "\n"
     "| | | | | | | C - - - - - -- Group has CEI"            "\n"
     "| | | | | | `P- - - - - - -- Group has PE"             "\n"
     "| | | | | | A - - - - - - -- Group has AI"             "\n"
     "| | | | | `R- - - - - - - -- Group has AR"             "\n"
     "| | | | | L - - - - - - - -- Group has LI"             "\n"
     "| | | | `r- - - - - - - - -- AR follows"               "\n"
     "| | | | p - - - - - - - - -- this AI is in the past"   "\n"
     "| | | `f- - - - - - - - - -- this AI is in the future" "\n"
     "| | | c - - - - - - - - - -- Common of keyset split"   "\n"
     "| | `r- - - - - - - - - - -- Rest of keyset split"     "\n"
     "| | # - - - - - - - - - - -- Show #REF"                "\n"
     "| `C- - - - - - - - - - - -- Comment out"              "\n"
     "| A - - - - - - - - - - - -- Auto LI"                  "\n"
     "`T- - - - - - - - - - - - -- Remove TIME/DATE"         "\n"
     "H - - - - - - - - - - - - -- Hide"                     "\n";

     if (pm == PR_TABLE)
     {
        for (const char * f = format; *f; ++f)   out << '=';
        out << endl;
        out << rr << endl;
     }
}
//-----------------------------------------------------------------------------
void
Command::print(ostream & out, PrintMode pm, int out_line) const
{
   if      (pm == PR_TABLE)    print_table(out);
   else if (pm == PR_NORMAL)   print_normal(out, out_line);
   else                        assert(0 && "Bad print mode");
}
//-----------------------------------------------------------------------------
void
Command::print_table(ostream & out) const
{
   out << ((state & ST_HIDE)             ? "H" : "-");
   out << ((state & ST_REMOVE_TIME_DATE) ? "T" : "-");
   out << ((state & ST_AUTO_LI)          ? "A" : "-");
   out << ((state & ST_COMMENT_OUT)      ? "C" : "-");
   out << ((state & ST_SHOW_REF)         ? "#" : "-");
   out << ((state & ST_SPLIT_REST)       ? "r" : "-");
   out << ((state & ST_SPLIT_COMMON)     ? "c" : "-");
   out << ((state & ST_AI_IN_FUTURE)     ? "f" : "-");
   out << ((state & ST_AI_IN_PAST)       ? "p" : "-");
   out << ((state & ST_AR_FOLLOWS)       ? "r" : "-");
   out << ((state & ST_HAS_LI)           ? "L" : "-");
   out << ((state & ST_HAS_AR)           ? "R" : "-");
   out << ((state & ST_HAS_AI)           ? "A" : "-");
   out << ((state & ST_HAS_PE)           ? "P" : "-");
   out << ((state & ST_HAS_CEI)          ? "C" : "-");
   out << ((state & ST_HAS_PI)           ? "p" : "-");
   out << ((state & ST_HAS_ZI)           ? "Z" : "-");
   out << ((state & ST_IMSIS_CONFLICT)   ? "i" : "-");
   out << ((state & ST_TIME_CONFLICT)    ? "t" : "-");
   out << ((state & ST_SEQ_CONFLICT)     ? "s" : "-");
   out << ((state & ST_BACKUP_CONFLICT)  ? "b" : "-");
   out << ((state & ST_KEYSET_CONFLICT)  ? "k" : "-");
   out << ((state & ST_BAD_DATE_SYNTAX)  ? "d" : "-");
   out << ((state & ST_BAD_TIME_SYNTAX)  ? "t" : "-");
   out << ((state & ST_KEY_BUT_NO_VAL)   ? "V" : "-");
   out << " ";

   char time_buf[40];
   timestamp.write_stamp(time_buf, sizeof(time_buf), TF_20YYMMDD_hhmm);

   out << right
       << setw( 6) << line_number                   << " "
       << setw( 5) << group_number                  << " "
       << setw( 5) << original->group_number        << " "
       << setw( 5) << conflict_ref                  << " "
       << setw(12) << time_buf                      << " "
       << left
       << setw( 5) << ((genre && genre->key_name) ?
                         genre->key_name : "-")   << " ";

   int key_len;
   if (keys)
   {

        keys->print(out, KPF_PRETTY);
        key_len = keys->print_len(KPF_PRETTY);
   }
   else if (key_all)
   {
        out << "ALL";
        key_len = 3;
   }
   else
   {
        out << "-";
        key_len = 1;
   }

   for (; key_len < 16; ++key_len)   out << " "; 

   out                                              << " "
       << setw(16) << left << (imsis_arg ? imsis_arg : "-") << " ";

   if      (category == CAT_NONE)   out << "??? ";
   else if (category == CAT_AI)     out << "AI  ";
   else if (category == CAT_AR)     out << "AR  ";
   else if (category == CAT_PI)     out << "PI  ";
   else if (category == CAT_PE)     out << "PE  ";
   else if (category == CAT_LI)     out << "LI  ";
   else if (category == CAT_ZI)     out << "ZI  ";
   else if (category == CAT_CEI)    out << "CEI ";
   else assert(0 && "Bad command category");

   assert(command);
   out << command;

   if (arguments)
   {
        enum { MAX_OUT = 40 };
        out << (is_command ? ":" : " ");
        if (strlen(arguments) < MAX_OUT)
        {
             out << arguments;
        }
        else
        {
             for (int l = 0; l < (MAX_OUT - 6); ++l)   out << arguments[l];
             out << " <...>";
        }
   }

   if (is_command)   out << ";";
   out << endl;
}
//-----------------------------------------------------------------------------
void
Command::print_normal(ostream & out, int out_line) const
{
   if (state & ST_HIDE)   return;

   bool in_comment = false;

   // maybe comment out the entire command.
   //
   if (state & ST_COMMENT_OUT)
   {
        out << "! ";
        in_comment = ! in_comment;
   }
   else
   {
        out << "  ";
   }

   // the command and its paraeters.
   //
   out << command;
   print_arguments(out);
   if (is_command)   out << ";";

   // comments after the command.
   //
   enum { hints = ST_TIME_CONFLICT
                | ST_REMOVE_TIME_DATE
                | ST_SEQ_CONFLICT
                | ST_IMSIS_CONFLICT
                | ST_BACKUP_CONFLICT
                | ST_SHOW_REF };

   if (comment || (state & hints))
   {
        out << " ";
        if (!in_comment)   out << "!";   // now we are.
        in_comment = true;
   }


   if (comment)    out << " " << comment;

   if (state & ST_REMOVE_TIME_DATE)
   {
        assert(time_arg || date_arg);
        if (time_arg)   out << " TIME=" << time_arg;
        if (date_arg)   out << " DATE=" << date_arg;
        out << " removed";
   }

   if (state & ST_BACKUP_CONFLICT)
      out << " Before backup ";

   if (state & ST_TIME_CONFLICT)
      out << " conflict #" << conflict_ref << " ";

   if (state & ST_SHOW_REF)
   {
        out << " REF #" << conflict_ref;
        if (imsis_arg)   out << "." << imsis_arg;
   }

   if (state & ST_SEQ_CONFLICT)
      out << " Conflict with activation command at reference: #"
          << conflict_ref;

   if (state & ST_IMSIS_CONFLICT)
   {
        out << " IMSIS conflict with reference: #"
            << conflict_ref;
        if (imsis_arg)   out << "." << imsis_arg;
   }

   if (in_comment)   out << " !";
   out << "\r\n";

   // insert LI after this AI command (unless an AR follows.
   //
   if ((state & ST_AUTO_LI) && !(state & ST_AR_FOLLOWS))
   {
        assert(genre);
        assert(genre->LI);
        out << "  " << genre->LI;
        if (genre->key_name && (keys || key_all))
        {
             out << ":" << genre->key_name << "=";
             if (key_all)   out << "ALL";
             else           keys->print(out, KPF_AWKWARD);
        }

        out << "; ! Logging command inserted !\r\n";
   }
}
//-----------------------------------------------------------------------------
void
Command::print_arguments(ostream & out) const
{
   if (!arguments)   return;

   bool first = true;

   for (const char * a = arguments; *a; )
   {
         while (*a && *a < ' ')  ++a;   // skip leading whitespaces.

         int name_len = strlen(a);
         const char * eq = strchr(a, '=');
         if (eq)
         {
              while (eq > a && eq[-1] < 32)   --eq;
              name_len = eq - a;
         }

         bool want_param = true;
         if (state & ST_REMOVE_TIME_DATE)
         {
               if ((name_len == 4) && !strncmp(a, "TIME", 4))
                  want_param = false;
               if ((name_len == 4) && !strncmp(a, "DATE", 4))
                  want_param = false;
         }

         const char * end = strchr(a, ',');

         if (!want_param)
         {
              if (end == 0)   break;   // no more parameters
              a = end + 1;             // next parameter
              continue;
         }

         if (first && !is_command)   out << " ";
         else if (first)             out << ":";
         else                        out << ",";
         first = false;

         if (end == 0)   // no more parameters
         {
              out << a;
              break;
         }
         else
         {
              for (; a < end;)   out << *a++;
              ++a;
         }
       }
}
//-----------------------------------------------------------------------------
/**
 ** Search backup timestamp in commands and return:
 **
 ** NSCommand::Exit_Success             - if no backup timestamp was provided (-t none)
 ** NSCommand::Exit_Event_NotFound   - if the backup timestamp was not found
 ** E_NO_CONFIG_COMMANDS - if there are no commands after backup timestamp
 ** NSCommand::Exit_Success             - if the backup timestamp was found.
 **/
NSCommand::Exit_Code
Command::search_backup(const TimeDate & backup)
{
   if (backup.get_mode() == TM_NO_STAMP)     return NSCommand::Exit_Success;

   char expected_date[20];
   char expected_time[20];

   backup.write_date(expected_date, sizeof(expected_date), TF_YYMMDD);
   backup.write_time(expected_time, sizeof(expected_time), TF_hhmm);

   if (log_control & LOG_BACKUPS)
   {
        cerr << "Searching backup timestamp " << expected_date
             << " " << expected_time << "\r\n";
   }

   for (Command * cmd = first_in_file; cmd; cmd = cmd->next_in_file)
   {
         if (strncmp(cmd->command, "AUTODUMP", 9)
          && strncmp(cmd->command, "SYBUP", 6))      continue;

         if (log_control & LOG_BACKUPS)
         {
              cerr << "Seeing " << cmd->command << " with: "
                   << cmd->arguments << "\r\n";
         }

         assert(cmd->arguments);
         if (strstr(cmd->arguments, expected_date) == 0)   continue;
         if (strstr(cmd->arguments, expected_time) == 0)   continue;

         if (log_control & LOG_BACKUPS)
         {
              cerr << "Found matching " << cmd->command
                   << " " << cmd->arguments << "\r\n";
         }

         cmd->state &= ~ST_HIDE;
         cmd->state |= ST_COMMENT_OUT;

          // we found the backup timestamp. If there are no more configuration
          // commands after the backup then we return E_NO_CONFIG_COMMANDS,
          // otherwiese NSCommand::Exit_Success.
          //
          for (cmd = cmd->next_in_file; cmd; cmd = cmd->next_in_file)
          {
                if (!strncmp(cmd->command, "AUTODUMP", 9))   continue;
                if (!strncmp(cmd->command, "SYBUP", 6))      continue;

                // found command (other than AUTODUMP or SYBUP).
                //
                return NSCommand::Exit_Success;
           }
          return NSCommand::Exit_NoConfig_Found;
    }

   return NSCommand::Exit_Event_NotFound;
}
//-----------------------------------------------------------------------------
NSCommand::Exit_Code
Command::process_commands(const TimeDate & now_stamp, const TimeDate & backup)
{
   assert(now_stamp.get_mode() != TM_NO_STAMP);
   assert(now_stamp.get_mode() != TM_INVALID);
   assert(backup.get_mode()    != TM_INVALID);

   // first we copy keys from PI commands to the following command until
   // the closing PE is reached
   //
   for (Command * cmd = first_in_file; cmd; cmd = cmd->next_in_file)
   {
         clone_PI_keysets(cmd);
   }

   // then we create a linked list of groups (commands belonging together).
   //
   {
     int group = 10;
     for (Command * cmd = first_in_file; cmd; cmd = cmd->next_in_file)
     {
           progress(100);
           mark_group(cmd, group);
     }
   }

   // then we clone the KEY=ALL commands
   //
   for (Command * cmd = first_group_in_file; cmd; cmd = cmd->next_group_in_file)
   {
         clone_KEY_ALL(cmd);
   }

   // maybe mark conflicts by incompatible keysets.
   //
   if (MARK_CONFLICTING_KEYSETS)
   {
        for (Command * cmd = first_group_in_file; cmd;
             cmd = cmd->next_group_in_file)
        {
              mark_keyset_conflicts(cmd);
        }
   }

   // maybe mark IMSIS conflicts.
   //
   // for (Command * cmd = first_in_file; cmd; cmd = cmd->next_in_file)
   // not needed:    mark_IMSIS_conflicts(cmd);

   /// then we check the proper order of commands within a group.
   //
   for (Command * cmd = first_group_in_file; cmd; cmd = cmd->next_group_in_file)
   {
         check_command_order(cmd);
   }

   // then we correct time/date handling in AI commands.
   //
   for (Command * cmd = first_group_in_file; cmd; cmd = cmd->next_group_in_file)
   {
         time_handling(cmd, now_stamp);
   }

   // then we mark conflicts possibly caused by time_handling()
   //
   for (Command * cmd = first_group_in_file; cmd; cmd = cmd->next_group_in_file)
   {
         mark_time_remove_conflicts(cmd);
   }

   // then we hide commands in group 1 before the backup time.
   //
   for (Command * cmd = first_in_file; cmd; cmd = cmd->next_in_file)
   {
         if (cmd->group_number <= 1 && cmd->is_command)
         {
              if (backup.get_mode() == TM_NORMAL && cmd->timestamp < backup)
                 cmd->state |= ST_HIDE;
         }
    }

   // then we hide commands in group starting before the backup time
   // and ending after the backup time.
   //
   for (Command * cmd = first_group_in_file; cmd; cmd = cmd->next_group_in_file)
   {
         hide_before_after(cmd, backup);
   }

   return NSCommand::Exit_Success;
}
//-----------------------------------------------------------------------------
/** clone keys from PI command to subsequent commands until the closing
 ** PE command is reached.
 **/
NSCommand::Exit_Code
Command::clone_PI_keysets(Command * first)
{
   if (first->genre == 0)         return NSCommand::Exit_Success;
   if (first->category != CAT_PI)   return NSCommand::Exit_Success;

   assert(first->keys || first->key_all);

   for (Command * cmd = first->next_in_file; cmd; cmd = cmd->next_in_file)
   {
         if (first->genre != cmd->genre)   continue;   // another group.

         if (!(cmd->keys || cmd->key_all))
         {
               if (first->keys)   cmd->keys = new KeySet(*first->keys);
               cmd->key_all = first->key_all;
         }

         if (cmd->category == CAT_PE)   break;
   }

   return NSCommand::Exit_Success;
}
//-----------------------------------------------------------------------------
/** mark all commands that belong to the same group with the same group_number.
 **/
NSCommand::Exit_Code
Command::mark_group(Command * first, int & group)
{
   if (first->group_number)   return NSCommand::Exit_Success;   // already in a group.

   if (first_group_in_file)   // subsequent group
   {
        assert(last_group_in_file);
        first->prev_group_in_file = last_group_in_file;
        last_group_in_file->next_group_in_file = first;
   }
   else                       // first group command
   {
        first_group_in_file = first;
   }
   last_group_in_file = first;

   first->first_in_group = first;
   first->last_in_group = first;

   // all commands that are not mentioned in the INI file are put in group 1.
   //
   if (first->genre == 0)
   {
        first->group_number = 1;
        return NSCommand::Exit_Success;
   }

   // we collect all LI commands in group 2.
   //
   if (first->category == CAT_LI)
   {
        first->group_number = 2;
        return NSCommand::Exit_Success;
   }

   // Here we have a command from the INI file, which means that it needs
   // needs genre handling. We move forward
   // in the file and collect all commands belonging to this command.
   //
   first->group_number = group++;

   // compute common keys (the intersection of all keysets) in this group.
   //
   KeySet common(0, MAX_KEY_VALUE);   // all keys.

   if (DO_KETSPLIT && first->genre->key_name && first->keys)
   {
        for (Command * cmd = first; cmd; cmd = cmd->next_in_file)
        {
              if (cmd->genre != first->genre)   continue;   // not related.
              if (cmd->key_all)          continue;   // all keys
              if (!cmd->keys)            continue;   // no key

              const SetRelation rel = common.relation(cmd->keys);
              switch(rel)
              {
                   case A_AND_B_DISJOINT: continue;   // not related
                   case A_AND_B_EQUAL:    continue;   // same

                   case A_CONTAINS_B:
                   case B_CONTAINS_A:
                   case A_AND_B_OVERLAP:  break;      // related

                   default:
                        cerr << "Unexpected relation = " << rel << endl;
                        assert(0 && "Unexpected set relation");
               }

              common.intersect_with(cmd->keys);
         }

        if (log_control & LOG_KEY_DEBUG)
        {
            cerr << "INTERSECTION = ";
            common.print(cerr, KPF_PRETTY);
            cerr << endl;
        }
   }

   int group_state = first->category << 12;
   Command * last_AI = 0;

   for (Command * cmd = first; cmd; cmd = cmd->next_in_file)
   {
         if (cmd->group_number && (cmd != first))   continue; // already marked
         if (!first->same_group(cmd, false))        continue;   // not related.

         cmd->first_in_group = first;
         cmd->group_number = first->group_number;
         first->last_in_group = cmd;

         if (DO_KETSPLIT && first->genre->key_name && cmd->keys)
         {
              const SetRelation rel = common.relation(cmd->keys);
              switch(rel)
              {
                   case A_AND_B_EQUAL:
                   case A_AND_B_DISJOINT:
                        break;

                   case A_AND_B_OVERLAP:
                   case A_CONTAINS_B:
                   case B_CONTAINS_A:
                        cmd->split_keys(common);
                        break;

                   default:
                        cerr << "Unexpected relation = " << rel << endl;
                        assert(0 && "Unexpected set relation");
              }
         }

         group_state |= cmd->category << 12;
         cmd->state |= group_state;

         if (cmd->category == CAT_AI)
         {
              group_state &= ~ST_HAS_AR;   // reset the previous AR (if any)
              cmd->state  &= ~ST_HAS_AR;   // dito.
              last_AI = cmd;
         }
         else if (cmd->category == CAT_AR)
         {
              group_state &= ~ST_HAS_AI;   // reset the previous AI (if any)
              cmd->state  &= ~ST_HAS_AI;   // dito.
              if (last_AI)   last_AI->state |= ST_AR_FOLLOWS;
         }
         else if (cmd->category == CAT_LI)
         {
              // this group has reached LI. We put subsequent commands of
              // the same genre into a different group.
              //
              cmd->group_number = 0;   // reset group number.
              cmd->state |= ST_HIDE;
              break;
         }

       }

   return NSCommand::Exit_Success;
}
//-----------------------------------------------------------------------------
/** clone KEY=ALL to all keysets of the same Genre
 **/
NSCommand::Exit_Code
Command::clone_KEY_ALL(Command * first)
{
   if (first->genre == 0)     return NSCommand::Exit_Success;
   if (first->genre->LI)      return NSCommand::Exit_Success;
   if (!first->key_all)       return NSCommand::Exit_Success;

   const Command * last = first->last_in_group;
   bool done = false;
   for (Command * all_cmd = first; all_cmd && !done;
        all_cmd = all_cmd->next_in_file)
   {
         done = (all_cmd == last);
         if (all_cmd->group_number != first->group_number)   continue;

         if (!all_cmd->key_all)
         {
            cerr << "LOC = " << all_cmd->in_file_name << ":" << all_cmd->line_number << endl;
            print_all_commands(cerr, PR_TABLE);
         }
         assert(all_cmd->key_all);
         for (Command * cmd = first_group_in_file; cmd;
              cmd = cmd->next_group_in_file)
         {
               if (cmd->genre != first->genre)   continue;   // different genre
               if (cmd->key_all)                 continue;   // also KEY=ALL
               if (!cmd->keys)                   continue;   // no key

               // here all_cmd and cmd have the same Genre, all_cmd has KEY=ALL
               // and cmd has KEY=value. We clone all_cmd with KEY=value and
               // put it into the group of cmd.
               //
               KeySet * cloned_keys = new KeySet(*cmd->keys);
               Command * cloned_all = new Command(all_cmd, cloned_keys);
               cloned_all->state |= ST_HIDE;
               cloned_all->group_number = cmd->group_number;
         }
    }

   return NSCommand::Exit_Success;
}
//-----------------------------------------------------------------------------
/** mark commands with incompatible keysets in a group.
 **/
NSCommand::Exit_Code
Command::mark_keyset_conflicts(Command * first)
{
   if (first->group_number <= 1)   return NSCommand::Exit_Success;

   // if first contains KEY=ALL or no key, then it does not conflict
   // with subsequent commands.
   //
   if (first->key_all)   return NSCommand::Exit_Success;
   if (!first->keys)     return NSCommand::Exit_Success;

   const Command * last = first->last_in_group;
   const KeySet * key = 0;

   bool conflict = false;
   bool done = false;
   for (Command * cmd = first; cmd && !done; cmd = cmd->next_in_file)
   {
         done = (cmd == last);

         if (cmd->group_number != first->group_number)   continue;

         if (cmd->key_all)
         {
              continue;
         }
         else if (cmd->keys)
         {
              if (key == 0)  // first command with key.
              {
                   key = cmd->keys;
                   continue;
              }

              const SetRelation rel = key->relation(cmd->keys);
              switch(rel)
              {
                   case A_AND_B_EQUAL:                       break;
                   case A_CONTAINS_B:     conflict = true;   break;
                   case B_CONTAINS_A:     conflict = true;   break;
                   case A_AND_B_OVERLAP:  conflict = true;   break;
                   case A_AND_B_DISJOINT: assert(0);         break;
                   case A_AND_B_EMPTY:                       break;
                   case A_EMPTY:          conflict = true;   break;
                   case B_EMPTY:          conflict = true;   break;
                   default:
                        cerr << "Unexpected relation = " << rel << endl;
                        assert(0 && "Unexpected set relation");
              }
         }
    }

   if (!conflict)   return NSCommand::Exit_Success;

   // mark all commands in group.
   //
   done = false;
   for (Command * cmd = first; cmd && !done; cmd = cmd->next_in_file)
   {
         done = (cmd == last);

         if (cmd->group_number != first->group_number)   continue;

          cmd->state |= ST_KEYSET_CONFLICT;
          cmd->state |= ST_COMMENT_OUT;
   }

   return NSCommand::Exit_Success;
}
//-----------------------------------------------------------------------------
/** mark commands with the same IMSIS
 **/
NSCommand::Exit_Code
Command::mark_IMSIS_conflicts(Command * first)
{
   if (first->group_number <= 1)   return NSCommand::Exit_Success;

   assert(first->genre);
   if (!first->genre->uses_imsis)   return NSCommand::Exit_Success;
   if (!first->imsis_arg)             return NSCommand::Exit_Success;

   const Command * last = first->last_in_group;

   Command * last_AI = 0;
   bool done = false;
   for (Command * cmd = first->next_in_file; cmd && !done;
        cmd = cmd->next_in_file)
   {
         done = (cmd == last);

         if (cmd->group_number != first->group_number)   continue;

         if (cmd->category == CAT_AI)
         {
              last_AI = cmd;
              continue;
         }

         if (!cmd->imsis_arg)                            continue;
         if (strcmp(first->imsis_arg, cmd->imsis_arg))   continue;
         if (last_AI == 0)                               continue;

          // same IMSIS.
          cmd->state |= ST_IMSIS_CONFLICT;
          cmd->state |= ST_COMMENT_OUT;

          first->original->state |= ST_SHOW_REF;
          if (first->original->conflict_ref == 0)
             first->original->conflict_ref = ++conflict_number;
          cmd->conflict_ref = first->original->conflict_ref;
    }

   return NSCommand::Exit_Success;
}
//-----------------------------------------------------------------------------
/** check the order of commands within a group.
 **/
NSCommand::Exit_Code
Command::check_command_order(Command * first)
{
   if (first->group_number <= 1)   return NSCommand::Exit_Success;

   // mark conflicts in groups that do not support LI.
   //
   assert(first->genre);
   if (first->genre->LI)   return NSCommand::Exit_Success;   // group supports LI: OK.

   Command * last_cmd = first;
   CommandCategory last_category = first->category;

   const Command * last = first->last_in_group;
   bool done = false;
   for (Command * cmd = first->next_in_file; cmd && !done;
        cmd = cmd->next_in_file)
   {
         done = (cmd == last);

         if (cmd->group_number != first->group_number)   continue;

         bool conflict = !valid_sequence(last_category, cmd->category);

         // some conflicts can be resolved.
         //
         if (conflict)
         {
              if ((last_category == CAT_AI) && (cmd->category == CAT_AI))
              {
                   if (cmd->key_all)   conflict = false;
              }
         }

         if (conflict)   // still in conflict.
         {
              // we mark subsequent commands and return.
              //
              int conflict_ref = 0;
              if (last_cmd)
              {
                   last_cmd->original->state |= ST_SHOW_REF;
                   if (last_cmd->original->conflict_ref)
                      conflict_ref = last_cmd->original->conflict_ref;
                   else
                      conflict_ref = ++conflict_number;
                      last_cmd->original->conflict_ref = conflict_ref;
              }

              if (conflict_ref == 0)   conflict_ref = ++conflict_number;;

              for (; cmd; cmd = cmd->next_in_file)
              {
                     if (cmd->group_number == first->group_number)
                     {
                          cmd->state |= ST_SEQ_CONFLICT;
                          cmd->state |= ST_COMMENT_OUT;
                          cmd->conflict_ref = conflict_ref;
                          ++conflicts_detected_count;
                          if (cmd == last)   break;
                     }
              }

              return NSCommand::Exit_Success;
         }

         // here we have no conflicts.
         //
         last_cmd = cmd;
         if (cmd->category == CAT_AR)   last_category = CAT_ZI;
         else                           last_category = cmd->category;
     }

   return NSCommand::Exit_Success;
}
//-----------------------------------------------------------------------------
/**
 ** \b valid_sequence() returns \b false iff one of the following
 ** (illegal) command sequences is detected:
 **
 ** <br><p><img src="command_state_machine.png">
 **/
bool
Command::valid_sequence(CommandCategory from, CommandCategory to)
{
   switch(from)
   {
        case CAT_ZI:
             switch(to)
             {
                  case CAT_ZI:   return true;
                  case CAT_PI:   return true;
                  case CAT_CEI:  return true;
                  case CAT_PE:   return false;   // missing PI
                  case CAT_AI:   return true;
                  case CAT_AR:   return false;   // missing AI
                  case CAT_LI:   return true;
                  case CAT_NONE: break;  // make compiler happy.
             }
             break;

        case CAT_PI:
             switch(to)
             {
                  case CAT_ZI:   return true;   // 
                  case CAT_PI:   return false;   // missing PE
                  case CAT_CEI:  return true;
                  case CAT_PE:   return true;
                  case CAT_AI:   return false;   // missing PE
                  case CAT_AR:   return false;   // missing AI
                  case CAT_LI:   return true;
                  case CAT_NONE: break;  // make compiler happy.
             }
             break;

        case CAT_CEI:
             switch(to)
             {
                  case CAT_ZI:   return true;
                  case CAT_PI:   return true;
                  case CAT_CEI:  return true;
                  case CAT_PE:   return true;
                  case CAT_AI:   return true;
                  case CAT_AR:   return false;   // missing AI
                  case CAT_LI:   return true;
                  case CAT_NONE: break;  // make compiler happy.
             }
             break;

        case CAT_PE:
             switch(to)
             {
                  case CAT_ZI:   return true;
                  case CAT_PI:   return true;
                  case CAT_CEI:  return true;
                  case CAT_PE:   return false;   // missing PI
                  case CAT_AI:   return true;
                  case CAT_AR:   return false;   // missing AI
                  case CAT_LI:   return true;
                  case CAT_NONE: break;  // make compiler happy.
             }
             break;

        case CAT_AI:
             switch(to)
             {
                  case CAT_ZI:   return false;   // protection timer
                  case CAT_PI:   return false;   // protection timer
                  case CAT_CEI:  return false;   // protection timer
                  case CAT_PE:   return false;   // protection timer
                  case CAT_AI:   return false;   // AI conflict
                  case CAT_AR:   return true;
                  case CAT_LI:   return true;
                  case CAT_NONE: break;  // make compiler happy.
             }
             break;

        case CAT_AR:
             switch(to)
             {
                  case CAT_ZI:   return true;
                  case CAT_PI:   return true;   // missing AI
                  case CAT_CEI:  return true;   // missing AI
                  case CAT_PE:   return true;   // missing AI
                  case CAT_AI:   return true;
                  case CAT_AR:   return false;   // missing AI
                  case CAT_LI:   return false;   // missing AI
                  case CAT_NONE: break;  // make compiler happy.
             }
             break;

        case CAT_LI:
             switch(to)
             {
                  case CAT_ZI:   return true;
                  case CAT_PI:   return true;
                  case CAT_CEI:  return true;
                  case CAT_PE:   return true;
                  case CAT_AI:   return true;
                  case CAT_AR:   return false;   // missing AI
                  case CAT_LI:   return true;
                  case CAT_NONE: break;  // make compiler happy.
             }
             break;

        case CAT_NONE: break;  // make compiler happy.
      }

   return false;
}
//-----------------------------------------------------------------------------
/** handle time/date parameters in AI commands.
 **/
NSCommand::Exit_Code 
Command::time_handling(Command * cmd, const TimeDate & now_stamp)
{
   if (cmd->group_number <= 1)   return NSCommand::Exit_Success;

   const Command * last = cmd->last_in_group;

   if (!(cmd->last_in_group->state & ST_HAS_AI))   // no AI in this group
   {
       if (log_control & LOG_BAD_SEQUENCE)
          cerr << "group " << cmd->group_number << " has no AI command\r\n";
        return NSCommand::Exit_Success;
   }

   for (bool done = false; cmd && !done; cmd = cmd->next_in_file)
   {
         done = (cmd == last);

         if (cmd->category != CAT_AI)   continue;

         assert(cmd->genre);

         // if the command has no TIME or DATE parameter then
         // we maybe add an LI command and continue with the next command
         // in the group.
         //
         if (cmd->date_arg == 0 && cmd->time_arg == 0)
         {
              if (cmd->genre->LI)   // this group has an LI command.
              {
                   cmd->state |= ST_AUTO_LI;
              }
              continue;
          }

          // here cmd has a TIME= parameter, or TIME= and DATE= parameters

          // compute when the command was (or will be) executed
          //
          TimeDate parameter_stamp;

          assert(cmd->time_arg);
//          parameter_stamp.parse(cmd->time_arg, TF_hhmm);

          if (cmd->date_arg == 0)
          {
               // There was no DATE= parameter. We take the day from the
               // command time stamp to compute the parameter_stamp.
               // If the TIME= in the parameter is before the time in the
               // command Date/time: stamp, then we take the next day.
               //
               if (parameter_stamp.get_seconds() < cmd->timestamp.get_seconds())
                  parameter_stamp.set_days(cmd->timestamp.get_days() + 1);
               else
                  parameter_stamp.set_days(cmd->timestamp.get_days());
          }
          else
          {
            //   parameter_stamp.parse(cmd->date_arg, TF_YYMMDD);
          }
          assert(parameter_stamp.get_mode() == TM_NORMAL);

          if (now_stamp < (parameter_stamp - WAITING_PERIOD))
          {
              cmd->state |= ST_AI_IN_FUTURE;
          }
          else
          {
               cmd->state |= ST_AI_IN_PAST;
               cmd->state |= ST_REMOVE_TIME_DATE;
               ++time_removed_count;
               if (cmd->genre->LI)   cmd->state |= ST_AUTO_LI;
          }
       }

   return NSCommand::Exit_Success;
}
//-----------------------------------------------------------------------------
/** mark conflicts caused by removal of a TIME= parameter.
 ** A conflict occurs when for an AI command
 **
 ** 1. no LI command is defined, and
 ** 2. The command has TIME/DATE parameters, and
 ** 3. the parameters are to be removed since their time in the past.
 **
 ** Under these conditions, when a command A is replayed with the TIME or DATE
 ** parameter removed, then a command B (which was originally executed after
 ** the time indicated by TIME/DATE would now be executed earlier and could
 ** conflict with command A.
 **
 ** Both commands A and B are commented out if they are in conflict.
 **
 ** We first find groups with the same activation command and then check
 ** for conflicts between them.
 **/
NSCommand::Exit_Code 
Command::mark_time_remove_conflicts(Command * first)
{
   if (first->group_number <= 1)   return NSCommand::Exit_Success;

   assert(first->genre);
   if (first->genre->LI)   return NSCommand::Exit_Success;   // LI supported - ok.

   const Command * last = first->last_in_group;
   bool done = false;
   for (Command * A = first; A && !done; A = A->next_in_file)
   {
         done = (A == last);

         if (A->category != CAT_AI)               continue;
         if (!(A->state & ST_REMOVE_TIME_DATE))   continue;

         // Here A is an AI command without LI support and TIME/DATE removed.
         // It could therefore conflict with subsequent AI commands.
         //
         for (Command * B = A->next_in_file; B; B = B->next_in_file)
         {
               if (B->group_number != A->group_number)   continue;

               assert(B->genre);
               assert(A->genre == B->genre);
                  continue;

               const NSCommand::Exit_Code error = mark_time_remove_conflicts(first, A, B);
               if (error)   return error;
         }
    }

   return NSCommand::Exit_Success;
}
//-----------------------------------------------------------------------------
/** see description for mark_time_remove_conflicts(Command * first) **/
NSCommand::Exit_Code 
Command::mark_time_remove_conflicts(Command * first_A, Command * AI_A,
                                    Command * first_B)
{
   bool conflict = false;

   assert(AI_A->genre);
   assert(AI_A->genre == first_A->genre);

   const Command * last_B = first_B->last_in_group;
   bool done = false;
   for (Command * B = first_B; B && !done; B = B->next_in_file)
   {
         done = (B == last_B);

         if (B->group_number != first_B->group_number)   continue;
         if (B->category == CAT_AI)                      continue;

         assert(B->genre);

         if (AI_A->genre->key_name == 0)
         {
              conflict = true;
         }
         else
         {
              assert(AI_A->keys);
              assert(B->keys);

              const SetRelation rel = AI_A->keys->relation(B->keys);
              switch(rel)
              {
                   case A_AND_B_EQUAL:    conflict = true;   break;
                   case A_CONTAINS_B:     conflict = true;   break;
                   case B_CONTAINS_A:     conflict = true;   break;
                   case A_AND_B_OVERLAP:  conflict = true;   break;
                   case A_AND_B_DISJOINT:                    break;
                   case A_AND_B_EMPTY:    conflict = true;   break;
                   case B_EMPTY:          conflict = true;   break;
                   default:
                        cerr << "line " << AI_A->line_number
                             << "Bad set relation " << rel << endl;
                             assert(0 && "Bad set relation");
              }
          }

         if (conflict)   break;
   }

   if (!conflict)   return NSCommand::Exit_Success;

   done = false;
   for (Command * B = first_B; B && !done; B = B->next_in_file)
   {
         done = (B == last_B);
         if (B->group_number != first_B->group_number)   continue;

         if (AI_A->conflict_ref == 0)
             AI_A->conflict_ref = ++conflict_number;
         B->conflict_ref = AI_A->conflict_ref;
         B->state |= ST_TIME_CONFLICT;
         B->state |= ST_COMMENT_OUT;
   }

   const Command * last_A = first_A->last_in_group;
   done = false;
   for (Command * A = first_A; A && !done; A = A->next_in_file)
   {
         done = (A == last_A);
         if (A->group_number != first_A->group_number)   continue;

         if (first_A->conflict_ref == 0)
             first_A->conflict_ref = ++conflict_number;
         A->conflict_ref = first_A->group_number;
         A->state |= ST_TIME_CONFLICT;
         A->state |= ST_COMMENT_OUT;
   }

   return NSCommand::Exit_Success;
}
//-----------------------------------------------------------------------------
NSCommand::Exit_Code 
Command::hide_before_after(Command * first, const TimeDate & backup)
{
   if (first->group_number <= 1)   return NSCommand::Exit_Success;

   const Command * last = first->last_in_group;
   assert(last);

   if (backup.get_mode() != TM_NORMAL)   return NSCommand::Exit_Success;  // no timestamp.
   if (backup < first->timestamp)        return NSCommand::Exit_Success;  // starts after b'up
   if (last->timestamp < backup)         return NSCommand::Exit_Success;  // ends before b'up

   // the backup is between the start and the end of the group.
   // mark all commands before the backup.
   //
   bool done = false;
   for (Command * cmd = first; cmd && !done; cmd = cmd->next_in_file)
   {
         done = (cmd == last);
         if (cmd->group_number != first->group_number)   continue;

         if (backup < cmd->timestamp)   break;

         cmd->state |= ST_COMMENT_OUT;
         cmd->state |= ST_BACKUP_CONFLICT;
         if (cmd->is_command)   ++before_backup_count;   // not AUTODUMP/SYBUP
   }

   return NSCommand::Exit_Success;
}
//-----------------------------------------------------------------------------
bool
Command::same_group(Command * cmd, bool split_done)
{
   if (cmd->genre != genre)   return false;

   // At this point cmd belongs to the group of this command. If this command
   // supports keys then we check if the key in cmd (if any) matches the
   // key in this command (if any).
   //
   assert(genre);

   if (!genre->key_name)     return true;   // this genre doesn't support keys.
   if (!(keys || key_all))             return true;   // no key matches all
   if (!(cmd->keys || cmd->key_all))   return true;   // no key matches all

   // all KEYS=ALL (and only those) are in the same group.
   //
   if (key_all)        return cmd->key_all;
   if (cmd->key_all)   return true;

   if (cmd->keys == 0)   // command has no key.
   {
        cmd->state |= ST_KEY_BUT_NO_VAL;
        return true;
   }

   const SetRelation rel = keys->relation(cmd->keys);

   switch(rel)
   {
        case A_AND_B_EQUAL:    return true;   // same
        case A_CONTAINS_B:     assert(!DO_KETSPLIT || !split_done);
                               return true;   // same
        case B_CONTAINS_A:     assert(!DO_KETSPLIT || !split_done);
                               return true;   // same
        case A_AND_B_OVERLAP:  assert(!DO_KETSPLIT || !split_done);
                               return true;   // same
        case A_AND_B_DISJOINT: return false;              // different groups

             // both keys exist, so the empty relations
             // should not happen.
             //
        case A_AND_B_EMPTY:
             cerr << "At " << __FILE__ << " line " << __LINE__ << endl;
             cerr << "AI  line "   << line_number    << endl
                  << "    args: "  << arguments      << endl
                  << "cmd line "  << cmd->line_number << endl
                  << "    args: " << cmd->arguments << endl;
             assert(0);
             return false;

         case B_EMPTY:
             // if cmd is a change command without key and
             // AI has a key, then the cmd shall be copied to
             // the output file. (Actually there was a PI
             //  command before the change command that
             //  had the keys).
             //
             cerr << "At " << __FILE__ << " line " << __LINE__ << endl;
             cerr << "AI  line "   << line_number    << endl
                  << "    args: "  << arguments      << endl
                  << "cmd line "  << cmd->line_number << endl
                  << "    args: " << cmd->arguments << endl;
             assert(0);
             return true;

         default:
             cerr << "line " << line_number
                  << "Bad set relation " << rel << endl;
                  assert(0 && "Bad set relation");
     }

   // not reached.
   return NSCommand::Exit_Success;
}
//-----------------------------------------------------------------------------
NSCommand::Exit_Code
Command::split_keys(const KeySet & common)
{
   if (key_all)   return NSCommand::Exit_Success;

   // we split this command into two commands.
   // The first command gets the keys in this->keys that are also in common.
   // The second command gets the remaining keys (which are not in common).
   //
   KeySet * common_keys = new KeySet;   // this->keys in common
   keys->intersection(*common_keys, common);

   KeySet * rest_keys = new KeySet;    // this->keys not in common
   keys->diff(*rest_keys, common);

   assert(rest_keys->get_count() > 0);

   if (log_control & LOG_KEY_DEBUG_2)
   {
        cerr << "*** split_keys(";
        common.print(cerr, KPF_PRETTY);
        cerr << ") : this->keys = ";
        keys->print(cerr, KPF_PRETTY);
        cerr << " split into common ";
        common_keys->print(cerr, KPF_PRETTY);
        cerr << " and rest ";
        rest_keys->print(cerr, KPF_PRETTY);
        cerr << endl;
   }

   // Clone this command with rest_keys as KEY= value. The constructor links
   // the new command as the next command into our command list. Therefore
   // we don't need the result of new().
   //
   new Command(this, rest_keys);

   delete keys;
   keys = common_keys;

   return NSCommand::Exit_Success;
}
//-----------------------------------------------------------------------------
NSCommand::Exit_Code
Command::write_output_file(const char * out_file_name, bool outFile, int argc, 
                           char ** argv)
{
   if (!outFile)
   {
        Command::print_all_commands(cout, PR_NORMAL);
   }
   else
   {
        // if the output file exists then don't override it.
        //
        {
          FILE * f = fopen(out_file_name, "r");
          if (f)
          {
               fclose(f);
               return NSCommand::Exit_CmdFile_Exists;
          }
        }

        ofstream out;
        out.open(out_file_name);
        if (!out.is_open())   return NSCommand::Exit_Invalid_Option2;

        // print command line arguments.
        //
        //out << "!";
        //for (int i = 0; i < argc; ++i)   out << " " << argv[i];
        //out << " !\r\n";

        Command::print_all_commands(out, PR_NORMAL);
        out.close();
    }

   return NSCommand::Exit_Success;
}
//-----------------------------------------------------------------------------
NSCommand::Exit_Code
Command::read_file_line(char * buffer, size_t size, FILE * file)
{
   //progress(10);
   bool leading_spaces = true;
   for (size_t b = 0; b < (size - 2); ++b)
   {
         buffer[b] = 0;
         const int cc = fgetc(file);
         if (cc == EOF)    return NSCommand::Exit_EndOf_File;
         if (cc == '\r')   continue;
         if (cc == '\n')   return NSCommand::Exit_Success;

         if (leading_spaces && (cc <= ' '))   ;
         else   { leading_spaces = false;   buffer[b] = cc; }
   }

   buffer[size - 1] = 0;
   return NSCommand::Exit_Error_Executing;
}
//=============================================================================
NSCommand::Exit_Code
Command::do_all(const char * ini_file_name, const char * in_file_name,
                const char * clog_path, const TimeDate & backup,
                const char * out_file_name, bool outFile, const TimeDate & now_stamp,
                int argc, char ** argv)
{
   assert(now_stamp.get_mode() != TM_INVALID);
   assert(backup.get_mode()    != TM_INVALID);

   NSCommand::Exit_Code error = NSCommand::Exit_Success;

   // read the cmdlls.ini file. The ini file consists if several sections.
   // Each section defined the properties of one Genre.
   //
   error = Genre::read_ini_file(ini_file_name);
   if (log_control & LOG_LIST_INI)   Genre::print_ini_file(cout);
   if (error)
   {
        if (log_control & LOG_DEBUG)
           cerr << "opening INI file failed, returning " <<  error << "\r\n";

        return error;
   }
   if (log_control & LOG_DEBUG)   cerr << "INI file OK.\r\n";

   // read commands file(s).
   //
   if (clog_path)   // directory containing CLOGs
   {
	
        for (file_name_list * files = read_directory(clog_path, backup);
             files; files = files->next)
        {
              error = NSCommand::Exit_Success;
              int line = 0;
              Command * cmds = read_input_file(error, files->file_name,
                                               backup, true, line);
              if (error)
              {
                   cerr << "Error reading file " << files->file_name << " line " << line << endl;
                   return error;
              }

              if (!cmds)   continue;

              first_in_file = merge(first_in_file, cmds);
         }
    }
    else             // single input file (-i) or ALOGFIND
    {
        int line = 0;
        first_in_file = read_input_file(error, in_file_name, backup, false, line);
    }
    if (error)   return error;

    // search backup timestamp
    //
    error = search_backup(backup);
    if (error)   return error;

    // process the commands.
    //
    error = process_commands(now_stamp, backup);
    if (error)   return error;

    if (log_control & LOG_LIST_INPUT)
      Command::print_all_commands(cout, PR_TABLE);

   // write the result file.
   //
    error = write_output_file(out_file_name,outFile, argc, argv);
    if (error)   return error;

    if (log_control & LOG_STATISTICS)   statistics(cout);

    if (conflicts_detected_count)   return NSCommand::Exit_Conflict_Err;

    return NSCommand::Exit_Success;
}
//-----------------------------------------------------------------------------
Command *
Command::merge(Command * list, Command * new_list)
{
   if (list == 0)       return new_list;
   if (new_list == 0)   return list;

   // currently all testcases run in prarallel, so sorting them by time
   // creates interleaved command sequences (which doesn't really work.
   // For the time being we append the later command list to the earlier
   // command list. In reality this interleaving does not (or is not supposed
   // to) happen.
   //
   {
     Command * l1 = list;
     Command * l2 = new_list;
     if (new_list->timestamp < list->timestamp)
     {
          l1 = new_list;
          l2 = list;
     }

     Command * ret = l1;
     while (l1->next_in_file)   l1 = l1->next_in_file;
     l1->next_in_file = l2;

     return ret;
   }

   /* not reached - remove block above if testcases fixed */

   Command * l = list;
   while (new_list)
   {
        if (l->timestamp < new_list->timestamp)   // l before new_list
        {
             if (l->next_in_file)                 // l not last
             {
                  l = l->next_in_file;
             }
             else                                 // l is last
             {
                  l->next_in_file = new_list;
                  break;
             }
        }
        else                                      // l after new_list
        {
             Command * n = new_list;
             new_list = new_list->next_in_file;

             Command * prev = l->prev_in_file;
             if (prev)   prev->next_in_file = n;

             n->next_in_file = l;
             l->prev_in_file = n;
             n->prev_in_file = prev;
             l = n;
        }
   }

   return list;
}
//-----------------------------------------------------------------------------
void
Command::statistics(ostream & out)
{
   out << "-----------------------------------------------\n"
       << "AI commands:          " << AI_command_count      << endl
       << "AR commands:          " << AR_command_count      << endl
       << "CEI commands:         " << CEI_command_count     << endl
       << "LI commands:          " << LI_command_count      << endl
       << "ZI commands:          " << ZI_command_count      << endl
       << "commands w/o date:    " << commands_without_date << endl
       << "AI commands w/o key:  " << AI_without_key        << endl
       << "AI commands w/o arg:  " << AI_without_arg        << endl
       << "other commands:       " << NOCAT_command_count   << endl
       << "Total command count:  " << total_command_count   << endl
       << "Commands before b'up: " << before_backup_count   << endl
       << "Conflicts detected:   " << conflicts_detected_count << endl
       << "Time params removed   " << time_removed_count    << endl
       << "===============================================\n";
}
//=============================================================================
//  local helper functions...

/// like strdup(), but avoiding malloc() and better const-ness, and returning
/// 0 for empty strings.
const char *
dup(const char * p)
{
   if (p == 0)    return 0;
   if (*p == 0)   return 0;

   const int size = strlen(p) + 1;
   char * ret = new char[size];
   strncpy(ret, p, size);
   return ret;
}
//-----------------------------------------------------------------------------
/// like dup(), but with start and end for strings that are not 0-terminated.
const char *
dup(const char * from, const char * to)
{
   assert(from < to);
   if (from == 0)    return 0;
   if (from == to)   return 0;

   const int len = to - from;
   char * ret = new char[len + 1];
   memcpy(ret, from, len);
   ret[len] = 0;
   return ret;
}
//-----------------------------------------------------------------------------
void
do_assert(const char * cond, const char * file, int line)
{
   if (log_control & LOG_ASSERTIONS)
   {
        cerr << "\r\n\r\n*** Assertion assert(" << cond << ") in " << file
             << ":" << line << " failed ***\r\n\r\n";
   }

   throw NSCommand::Exit_Error_Executing;
}
//=============================================================================
/*@}*/ 

