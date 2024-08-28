#ifndef CPS_CLOG_CommandImp_h
#define CPS_CLOG_CommandImp_h

#include <ostream>
#include <stdio.h>

#include "CPS_CLOG_KeySet.h"
#include "CPS_CLOG_TimeDate.h"
#include "CPS_CLOG_CommandArg.h"
#define MAX_CEI 10
/**
 ** \addtogroup cmdlls CMDLLS command
 **/
/*@{*/

/// duplicate a string.
extern const char * dup(const char * p);
extern const char * dup(const char * from, const char * to);

using namespace std;

//-----------------------------------------------------------------------------
/// logging verbosity
enum LogItem
{
   LOG_NORMAL       = 0x00000001,   ///< normal output (commands)
   LOG_STATISTICS   = 0x00000002,   ///< show command statistics
   LOG_LIST_INI     = 0x00000004,   ///< show the result of parsing the ini file
   LOG_LIST_INPUT   = 0x00000008,   ///< show the result of parsing the input
   LOG_VERBOSE_ERR  = 0x00000010,   ///< show more error information
   LOG_BAD_SEQUENCE = 0x00000020,   ///< show sequence problems.
   LOG_DEBUG        = 0x00000040,   ///< print debug info
   LOG_BACKUPS      = 0x00000080,   ///< show backup timestamps
   LOG_ALOGFIND     = 0x00000100,   ///< show alogfind details.
   LOG_CLOG         = 0x00000200,   ///< show Clog
   LOG_ASSERTIONS   = 0x00000400,   ///< show assertion details
   LOG_KEY_DEBUG    = 0x00000800,   ///< show keysplit debugging
   LOG_KEY_DEBUG_2  = 0x00001000,   ///< show more keysplit debugging
   LOG_PROGRESS     = 0x00002000,   ///< show progress bar
   LOG_XX,                          ///< last log bit + 1
   LOG_ALL = 2*LOG_XX - 3,          ///< all log bits (-VV log level)
   LOG_V   = 0                      ///< -V log level
           | LOG_NORMAL
           | LOG_STATISTICS
           | LOG_LIST_INI
           | LOG_LIST_INPUT
           | LOG_VERBOSE_ERR
           | LOG_BAD_SEQUENCE
           | LOG_DEBUG
           | LOG_BACKUPS
           | LOG_ALOGFIND
           | LOG_ASSERTIONS
//         | LOG_KEY_DEBUG
//         | LOG_KEY_DEBUG_2
};

/// the items to be logged (can be changed with -v command line flag)
extern LogItem log_control;

//-----------------------------------------------------------------------------
/// command category
enum CommandCategory
{
   CAT_NONE = 0x00,
   CAT_ZI   = 0x01,
   CAT_PI   = 0x02,
   CAT_CEI  = 0x04,
   CAT_PE   = 0x08,
   CAT_AI   = 0x10,
   CAT_AR   = 0x20,
   CAT_LI   = 0x40,
};
//-----------------------------------------------------------------------------
/// the status of a command
enum CommandState
{
   ST_NORMAL           = 0x00000000,      ///< nothing notable
   ST_KEY_BUT_NO_VAL   = 0x00000001,      ///< dito
   ST_BAD_TIME_SYNTAX  = 0x00000002,      ///< bad TIME= parameter
   ST_BAD_DATE_SYNTAX  = 0x00000004,      ///< bad DATE= parameter

   ST_KEYSET_CONFLICT  = 0x00000010,      ///< conflicting key sets
   ST_BACKUP_CONFLICT  = 0x00000020,      ///< conflict with backup time
   ST_SEQ_CONFLICT     = 0x00000040,      ///< sequence conflict within group
   ST_TIME_CONFLICT    = 0x00000080,      ///< conflict due to TIME= removal
   ST_IMSIS_CONFLICT   = 0x00000100,      ///< conflict due to TIME= removal

   ST_HAS_ZI           = CAT_ZI  << 12,   ///< \b saw a ZI before this command
   ST_HAS_PI           = CAT_PI  << 12,   ///< \b saw a PI before this command
   ST_HAS_CEI          = CAT_CEI << 12,   ///< \b saw a CEI before this command
   ST_HAS_PE           = CAT_PE  << 12,   ///< \b saw a PE before this command
   ST_HAS_AI           = CAT_AI  << 12,   ///< \b saw an AI before this command
   ST_HAS_AR           = CAT_AR  << 12,   ///< \b saw an AR before this command
   ST_HAS_LI           = CAT_LI  << 12,   ///< \b saw a LI before this command
   ST_AR_FOLLOWS       = 0x00080000,      ///< this AI is followed by AR

   ST_AI_IN_PAST       = 0x00100000,      ///< AI has a TIME/DATE in the past
   ST_AI_IN_FUTURE     = 0x00200000,      ///< AI has a TIME/DATE in the future
   ST_SPLIT_COMMON     = 0x00400000,      ///< common part of split
   ST_SPLIT_REST       = 0x00800000,      ///< resy of split

   // actions
   ST_SHOW_REF         = 0x08000000,      ///< show group number
   ST_COMMENT_OUT      = 0x10000000,      ///< comment when printing
   ST_AUTO_LI          = 0x20000000,      ///< add an LI after \b this AI
   ST_REMOVE_TIME_DATE = 0x40000000,      ///< remove TIME and DATE parameters
   ST_HIDE             = 0x80000000,      ///< don't print \b this command
};
//-----------------------------------------------------------------------------
/// name of file that generated an error.
extern const char * error_filename;

//-----------------------------------------------------------------------------
/// line in file that generated an error.
extern int error_line;

//-----------------------------------------------------------------------------
/// A list of (log-) file names
struct file_name_list
{
   file_name_list(const char * name, file_name_list * nxt)
   : file_name(name), next(nxt) {}

   const char * file_name;
   file_name_list * next;
};

extern file_name_list * read_directory(const char * dir_path,
                                       const TimeDate & backup);
//-----------------------------------------------------------------------------
/// An output format.
enum PrintMode
{
   PR_TABLE,    ///< table format for debugging
   PR_NORMAL,   ///< the final format
};
//=============================================================================
/** A Genre is a a number of related commands. One Genre corresponds to one
 ** section in the cmdlls.ini file. For example, the commands PNAZI, PNACI,
 ** PNASI PNASE, PNAAR, PNAAI, and PNALI form one Genre.
 **
 ** There are commands that do not belong to a Genre; these commands are
 ** copied unmodified to the output file.
 **/
class Genre
{
public:
   /// constructor for section (== AI command) section.
   Genre(const char * section);

   /// list of all sections.
   static Genre * all;

   /// read the INI file which contains command exceptions.
   static NSCommand::Exit_Code read_ini_file(const char * ini_file_name);

   /// print the parsed content of the ini file.
   static void print_ini_file(ostream & out);

   /// get the \b CommandCategory for \b cmd . Store the category in
   /// \b category and a pointer to the group in \b gen
   static CommandCategory get_CommandDetails(const char * cmd,
                                             const Genre * & gen);

   /// true if \b this group uses IMISIs as keys (i.e. MGIAI and friends).
   bool uses_imsis;

   /// print \b this entry.
   void print(ostream & out) const;

   /// the next entry in a linked list of all entries
   Genre * next;

   /// the section in the ini file (== the activation command).
   const char * AI;

   /// the AR command.
   const char * AR;

   /// the PI command.
   const char * PI;

   /// the CEI commands.
   const char * CEI[MAX_CEI];

   /// the PE command.
   const char * PE;

   /// the logging command.
   const char * LI;

   /// the ZI command.
   const char * ZI;

   /// the key for \b this command (0 if none)
   const char * key_name;
};
//=============================================================================
/// A single command line.
//
class Command
{
public:

   /// the top level algorithm: read input file(s), process them, and write
   /// the output file. Return 0 on sucess and an error code otherwise.
   static NSCommand::Exit_Code do_all(const char * ini_file_name, const char * in_file_name,
                       const char * clog_path, const TimeDate & backup,
                       const char * out_file, bool outFile, const TimeDate & now_stamp,
                       int argc, char ** argv);

   /// read a line from file \b file into buffer \b buffer of
   /// size \b size . Remove \\r and \\n from the line and 0-terminate
   /// the buffer.
   static NSCommand::Exit_Code read_file_line(char * buffer, size_t size, FILE * file);

   /// print statistics to \b out
   static void statistics(ostream & out);

   /// number of commands before backup (and activated after).
   static int before_backup_count;

   /// number of TIME/DATE parameters removed.
   static int time_removed_count;

   /// number of conflicts detected
   static int conflicts_detected_count;

protected:
   /// constructor
   Command(const char * input_file, int line, const char * cmd,
           const char * args, const TimeDate & stamp, const char * comm);

   /// constructor for keyset split
   Command(Command * other, const KeySet * our_keys);

   /// get value of \b parameter_name
   const char * get_parameter_value(const char * parameter_name);

   /// clone keys from PI commands to subsequent commands until PE
   static NSCommand::Exit_Code clone_PI_keysets(Command * cmd);

   /// collect all commands belonging to an AI command.
   static NSCommand::Exit_Code mark_group(Command * cmd, int & group);

   /// clone KEY=ALL to all keys in Genre.
   static NSCommand::Exit_Code clone_KEY_ALL(Command * cmd);

   /// mark keyset conflicts.
   static NSCommand::Exit_Code mark_keyset_conflicts(Command * cmd);

   /// mark IMSIS conflicts.
   static NSCommand::Exit_Code mark_IMSIS_conflicts(Command * cmd);

   /// check order of commands in a group
   static NSCommand::Exit_Code check_command_order(Command * cmd);

   /// handle time/date parameters in AI commands.
   static NSCommand::Exit_Code time_handling(Command * cmd, const TimeDate & t_option);

   /// mark conflicts between groups without LI commands
   static NSCommand::Exit_Code mark_time_remove_conflicts(Command * cmd);

   /// mark conflicts between command \b AI_A (in group starting at
   /// \b first_A) and \b first_B
   static NSCommand::Exit_Code mark_time_remove_conflicts(Command * first_A, Command * AI_A,
                               Command * first_B);

   /// comment out groups conflicting with the backup time
   static NSCommand::Exit_Code hide_before_after(Command * cmd, const TimeDate & backup);

   /// split a key set into the keys inside and outside the keys of AI
   NSCommand::Exit_Code split_keys(const KeySet & common);

   /// return true iff \b this AI command is related to \b cmd
   bool same_group(Command * cmd, bool split_done);

   /// the input file containing the command (or "ALOGFIND")
   const char * in_file_name;

   /// the line number in the file.
   const int line_number;

   /// the timestamp in the file (from the Date/time: line).
   const TimeDate timestamp;

   /// the group to which \b this command belongs.
   Command * original;

   /// the group to which \b this command belongs.
   int group_number;

   /// true if this is a command (as oppoesed to an event like AUTODUMP, SYBUP)
  bool is_command;

   /// a conflict number.
   int conflict_ref;

   /// state of the command
   int state;

   /// the command string
   const char * command;

   /// the command category (AI, PI, PE, LI, AR, ZI, CEI) of \b this command
   CommandCategory category;

   /// the command group for \b this command (0 if none)
   const Genre * genre;

   /// command arguments (0 if none)
   const char * arguments;

   /// DATE= arguments (0 if none)
   const char * date_arg;

   /// TIME= arguments (0 if none)
   const char * time_arg;

   /// IMSIS= arguments (0 if none)
   const char * imsis_arg;

   /// true if IMSIS= was cloned from another command.
   bool imsis_cloned;

   /// true if KEY= parameter was present and has value ALL.
   bool key_all;

   /// key values for \b this command (0 if none)
   const KeySet * keys;

   /// comment from the input file (0 if none)
   const char * comment;

   /// the previous command in the file (0 if \b this is the first command).
   Command * prev_in_file; 

   /// the next command in the file (0 if \b this is the last command).
   Command * next_in_file;

   /// the previous command in the file (0 if \b this is the first command).
   Command * prev_group_in_file; 

   /// the next command in the file (0 if \b this is the last command).
   Command * next_group_in_file;

   /// the first command in \b this group (valid in all commands)
   Command * first_in_group;

   /// the last command in \b this group (valid only in the first command)
   Command * last_in_group;

   /// return true for valid (conflict-free) sequences, like ZI -> CEI.
   static bool valid_sequence(CommandCategory from, CommandCategory to);

   /// read the commands to be processed.
   static Command * read_input_file(NSCommand::Exit_Code & error, const char * in_file_name,
                                const TimeDate & backup, bool clog_format, int & line);

   /// read the commands to be processed.
   static NSCommand::Exit_Code search_backup(const TimeDate & backup);

   /// process the commands.
   static NSCommand::Exit_Code  process_commands(const TimeDate & now_stamp,
                                 const TimeDate & backup);

   /// write the output file.
   static NSCommand::Exit_Code write_output_file(const char * out_file_name, bool outFile, int argc,
                                  char ** argv);

   /// the first command in the input file.
   static Command * first_in_file;

   /// the first AI command in the input file.
   static Command * first_group_in_file;

   /// the last AI command in the input file.
   static Command * last_group_in_file;

   /// print the parsed input file.
   static void print_all_commands(ostream & out, PrintMode pm);

   /// statistics: number of commands.
   static int total_command_count;

   /// statistics: number of commands without category.
   static int NOCAT_command_count;

   /// statistics: number of AI commands.
   static int AI_command_count;

   /// statistics: number of AI commands.
   static int PI_command_count;

   /// statistics: number of AI commands.
   static int PE_command_count;

   /// statistics: number of AI commands.
   static int AR_command_count;

   /// statistics: number of AI commands.
   static int CEI_command_count;

   /// statistics: number of AI commands.
   static int LI_command_count;

   /// statistics: number of AI commands.
   static int ZI_command_count;

   /// statistics: number of AI commands.
   static int commands_without_date;

   /// statistics: number of AI commands without key.
   static int AI_without_key;

   /// statistics: number of AI commands without key.
   static int AI_without_arg;

private:
   /// print \b this command according to print mode \b pm.
   void print(ostream & out, PrintMode pm, int out_line) const;

   /// print \b this command in table mode (debug form);
   void print_table(ostream & out) const;

   /// print \b this command in normal mode.
   void print_normal(ostream & out, int out_line) const;

   /// print \b arguments of this command in normal mode.
   void print_arguments(ostream & out) const;

   /// destructor (must not be used).
   ~Command();   // not implemented

   /// merge two command lists.
   static Command * merge(Command * list, Command * new_list);

   /// reference number for conflicts.
   static int conflict_number;
};
//-----------------------------------------------------------------------------

/// maybe show some progress on cerr
inline void
progress(int x)
{
  // if (!(log_control & LOG_PROGRESS))   return;
   if (x <= ++TimeDate::progress_count)   TimeDate::do_progress();
}
//-----------------------------------------------------------------------------
/// print assertion details (condition, file, line number) and throw an error.
extern void do_assert(const char * cond, const char * file, int line);

/// define assert() to throw an error (rather than exiting with 134).
#define assert(x) if (!(x))  do_assert(#x, __FILE__, __LINE__)

/*@}*/ 

#endif //CommandImp_h 
