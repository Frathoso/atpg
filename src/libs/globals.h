/*
 * =====================================================================================
 *
 *       Filename:  globals.h
 *
 *    Description:  Defines global variables
 *
 *        Version:  1.0
 *        Created:  09 March 2014
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

#include "atpg_types.h"
#include "hash.h"


#ifndef GLOBALS_H
#define GLOBALS_H


char ERROR_MESSAGE[MAX_LINE_LENGTH];    // Holds custom error messages

volatile COMMAND_LINE_OPTIONS options;   // Command line options

HASH_ENTRY hashTableGates[MAX_GATES];	// hash table of gate names


#endif
