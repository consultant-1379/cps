/* CPS_BUAP_getopt.c
******************************************************************************
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 1999.
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
//  DESCRIPTION 
//	Implements the unix function getopt.
//
//  DOCUMENT NO
//  190 89-CAA 109 1412
//
//  AUTHOR 
// 	1999/07/02 by UAB/B/SF Lillemor Pettersson
******************************************************************************
// === Revision history ===
// 990702 LAN PA1 Created.
//  101010 xchihoa     Ported to Linux for APG43L.
******************************************************************************

******************************************************************************
* getopt()  
*  
* The getopt() function is a command line parser.  It returns the next  
* option character in argv that matches an option character in opstring.  
*  
* The argv argument points to an array of argc+1 elements containing argc  
* pointers to character strings followed by a null pointer.  
*  
* The opstring argument points to a string of option characters; if an  
*
* option character is followed by a colon, the option is expected to have  
* an argument that may or may not be separated from it by white space.  
* The external variable optarg is set to point to the start of the option  
* argument on return from getopt().  
*  
* The getopt() function places in optind the argv index of the next argument  
* to be processed.  The system initializes the external variable optind to  
* 1 before the first call to getopt().  
*  
* When all options have been processed (that is, up to the first nonoption  
* argument), getopt() returns EOF.  The special option "--" may be used to  
* delimit the end of the options; EOF will be returned, and "--" will be  
* skipped.  
*  
* The getopt() function returns a question mark (?) when it encounters an  
* option character not included in opstring.  This error message can be  
* disabled by setting opterr to zero.  Otherwise, it returns the option  
* character that was detected.  
*  
* If the special option "--" is detected, or all options have been  
* processed, EOF is returned.  
*  
* Options are marked by either a minus sign (-) or a slash (/).  
*  
* No errors are defined.  
*****************************************************************************/  
#include <stdio.h>                  // for EOF
#include <string.h>                 // for strchr()   
//#include "CPS_BUAP_getopt.H"                   

// static (global) variables that are specified as exported by getopt() 
char *optarg = NULL;    // pointer to the start of the option argument 
int   optind = 1;       // number of the next argv[] to be evaluated 
int   opterr = 1;       // non-zero if a question mark should be returned
                        // when a non-valid option character is detected  

// handle possible future character set concerns by putting this in a macro
#define _next_char(string)  (char)(*(string+1))

int buap_getopt(int argc, char *argv[], const char *opstring)
{
	static char *pIndexPosition = NULL; // place inside current argv string
	char *pArgString = NULL;            // where to start from next
	char *pOptString;                   // the string in our program 

	if (pIndexPosition != NULL) 
    { 
		// we last left off inside an argv string 
        if (*(++pIndexPosition)) 
        {  
	       // there is more to come in the most recent argv
		   pArgString = pIndexPosition;         
		}
	} 

    if (pArgString == NULL) 
    {  
		// we didn't leave off in the middle of an argv string
        if (optind >= argc) 
        {  
			// more command-line arguments than the argument count
            pIndexPosition = NULL;   // not in the middle of anything
            return EOF;              // used up all command-line arguments
        }

        //--------------------------------------------------------------------- 
        // If the next argv[] is not an option, there can be no more options.
        //---------------------------------------------------------------------
        pArgString = argv[optind++]; // set this to the next argument ptr
        if (('/' != *pArgString) &&  // doesn't start with a slash or a dash?
            ('-' != *pArgString)) 
        {
			--optind;                // point to current arg once we're done
            optarg = NULL;           // no argument follows the option
            pIndexPosition = NULL;   // not in the middle of anything
            return EOF;              // used up all the command-line flags
        }

        // check for special end-of-flags markers
        if ((strcmp(pArgString, "-") == 0) ||
            (strcmp(pArgString, "--") == 0))
        {
			optarg = NULL;           // no argument follows the option 
            pIndexPosition = NULL;   // not in the middle of anything 
            return EOF;              // encountered the special flag 
		}

        pArgString++;                // look past the / or - 
	}

    if (':' == *pArgString) // is it a colon?
    {  
		//---------------------------------------------------------------------
		// Rare case: if opterr is non-zero, return a question mark;
        // otherwise, just return the colon we're on.
        //---------------------------------------------------------------------
        return (opterr ? (int)'?' : (int)':');
	}
    else if ((pOptString = strchr(opstring, *pArgString)) == 0)
    {
        //---------------------------------------------------------------------
        // The letter on the command-line wasn't any good.
        //---------------------------------------------------------------------
        optarg = NULL;               // no argument follows the option 
        pIndexPosition = NULL;       // not in the middle of anything 
        return (opterr ? (int)'?' : (int)*pArgString);
	}
    else 
    {
		//---------------------------------------------------------------------
        // The letter on the command-line matches one we expect to see
        //---------------------------------------------------------------------
        if (':' == _next_char(pOptString))  // is the next letter a colon? 
		{
			// It is a colon.  Look for an argument string.
            if ('\0' != _next_char(pArgString))
			{  
				// argument in this argv?
                optarg = &pArgString[1];    // Yes, it is
			}
            else
            {
                //-------------------------------------------------------------
                // The argument string must be in the next argv.
                // But, what if there is none (bad input from the user)?
                // In that case, return the letter, and optarg as NULL.
                //-------------------------------------------------------------
                if (optind < argc)
					optarg = argv[optind++];
                else
                {
                    optarg = NULL;
                    return (opterr ? (int)'?' : (int)*pArgString);
				}
			}
            pIndexPosition = NULL;   // not in the middle of anything 
		}
        else
		{
			// it's not a colon, so just return the letter 
            optarg = NULL;                  // no argument follows the option
            pIndexPosition = pArgString;    // point to the letter we're on
		}
        return (int)*pArgString;     // return the letter that matched
	}
 } 
 
