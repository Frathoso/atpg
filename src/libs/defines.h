/*
 * =====================================================================================
 *
 *       Filename:  defines.h
 *
 *    Description:  Includes all defined constants
 *
 *        Version:  1.0
 *        Created:  02 December 2013
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

#ifndef DEFINES_H
#define DEFINES_H

/*
 *	Tool's details
 */
#define GLOBAL_NAME 			"atpg"
#define VERSION					"1.0"
#define FAULT_FILE_EXTENSION	".flt"
 
/*
 *  Custom error codes
 */
#define ERROR_COMMAND_LINE_ARGUMENTS    10000
#define ERROR_PARSING_NETLIST           10001
#define ERROR_IO_LIMIT_EXCEEDED		    10002
#define ERROR_PARSING_CIRCUIT		    10003
#define ERROR_PARSING_FAULT_LIST        10004

/*
 *  Gate contents limits
 */
#define MAX_WORD            32      // Maximum length of names
#define MAX_GATES           100000  // Maximum gates in a circuit
#define MAX_INPUT_GATES     2000    // Maximum input lines in a gate
#define MAX_OUTPUT_GATES    2000    // Maximum output lines in a gate
#define MAX_LINE_LENGTH     256     // Maximum length of an input file line
#define MAX_LOGIC_VALUES	5 		// Maximum number of logical values used

/*
 *
 */
#define MAX_FAULTS          1000000 // Maximum number of faults


#endif
