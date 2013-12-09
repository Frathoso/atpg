/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  Driver for the ATPG
 *
 *        Version:  1.0
 *        Created:  07 December 2013
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Francis Sowani <fts215@nyu.edu>
 *         Author:  Wei-Chung Lin <wcl262@nyu.edu>
 *     Supervisor:  Prof. Ozgur Sinanoglu <os22@nyu.edu>
 *     Supervisor:  Samah Saeed <sms22@nyu.edu>
 *   Organization:  New York University Abu Dhabi
 *
 * =====================================================================================
 */

/* fprintf, stdout, perror */
#include <stdio.h> 

/* malloc, free, exit, atexit, NULL */     
#include <stdlib.h>   

/* strcmp, strcpy, strtok */  
#include <string.h>    

/* opterr, getopt, optopt, optarg */ 
#include <unistd.h>    

/* errno */
#include <errno.h>

/* isprint */
#include <ctype.h>

/* Netlist files parsing functionality */
#include "parser_netlist.h"

/* Custom error codes */
#include "error.h"


/*
 *  Command line options list for the program
 *  d   - turn on debugging mode i.e. programs progress will be printed
 *  f   - specify the filename to parse the circuit netlist from
 */
#define OPTIONS_LIST "df:"

/*
 *  GLOBAL Variables
 */
char ERROR_MESSAGE[MAX_LINE_LENGTH];    // Holds custom error messages

BOOLEAN isDebugMode = FALSE;    // Turns ON/OFF the display of program progress


/*
 *  Alerts the user of a successful execution of the program or the errors
 *  causing an unsuccessful termination of the program
 */
void onProgramTermination(void)
{
    switch(errno)
    {
        case 0:
            fprintf(stdout, "Program successfully finished!\n\n");
            return;
            break;
        case ERROR_PARSING_NETLIST:
            fprintf(stdout, "Error: There was an error parsing the netlist file.\n");
            break;
        case ERROR_IO_LIMIT_EXCEEDED:
        case ERROR_PARSING_CIRCUIT:
            fprintf(stdout, "Error: %s\n", ERROR_MESSAGE);
            break;
        case ERROR_COMMAND_LINE_ARGUMENTS:
            fprintf(stdout, "Usage:\natpg -f <filename> [-d]\n");
            break;
        default:
            perror("Error");
    }
    fprintf(stdout, "Program was terminated prematurely!\n\n");
}

/*
 *  Program entry point
 *
 *  @param  argc - count of command line arguments
 *  @param  argv - names of command line arguments
 *  @return      - execution exit status
 */
int main( int argc, char* argv[] )
{
    /* Register the program finalizing function */
    atexit(onProgramTermination);

    /* Parse command line arguments */   
    char* filename = NULL;
    errno = opterr = 0;
    int opt;
    while((opt = getopt(argc, argv, OPTIONS_LIST)) != -1)
    {
        switch(opt)
        {
            case 'd':
                isDebugMode = TRUE;
                break;
            case 'f':
                filename = optarg;
                break;
            case '?':
                if (optopt == 'f')
                    fprintf(stdout, "Option -%c requires a filename.\n", optopt);
                else if (isprint(optopt))
                    fprintf(stdout, "Unknown option '-%c'.\n", optopt);
                else
                    fprintf(stdout, "Unknown option character '\\x%x'.\n", optopt);
                errno = ERROR_COMMAND_LINE_ARGUMENTS;
                exit(1);
            default:
                errno = ERROR_COMMAND_LINE_ARGUMENTS;
                exit(1);
        }
    }

    if(filename == NULL)
    {
        errno = ERROR_COMMAND_LINE_ARGUMENTS;
        exit(1);
    }

    /* Populate the circuit from the passed netlist file */
    CIRCUIT circuit;
    CIRCUIT_INFO info;
    
    if(isDebugMode) fprintf(stdout, "Parsing: \"%s\"...\n", filename);

    if(populateCircuit(circuit, &info, filename))
    {
        if(isDebugMode) fprintf(stdout, "Netlist file successfully parsed.\n\n");
    }
    else
    {
        if(errno == 0) errno = ERROR_PARSING_NETLIST;
        exit(1);
    }

    /* TODO: NEXT STEP: Generate test patterns */
    

    exit(0);
}
