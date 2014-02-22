/*
 * =====================================================================================
 *
 *       Filename:  fault_list_generator.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  15 February 2014
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

#include "defines.h"

#ifndef FAULT_LIST_GENERATOR_H
#define FAULT_LIST_GENERATOR_H


/*
 *  A fault type
 */
typedef enum
{
    ST_0,   // Stuck-at zero fault
    ST_1    // Stuck-at one fault
} FAULT_TYPE;


/*
 *  A fault structure
 */
typedef struct
{
    char name[MAX_WORD];    // The name of the wire stuck at fault
    FAULT_TYPE type;        // Either stuck at 1 or stuck at 0
} FAULT;


/*
 *  Creates a fault object
 *
 *  @param  char* _name - the name of the wire stuck-at fault
 *  @param  FAULT_TYPE _type  - the type of the fault
 *  @return FAULT - the created fault object
 */
FAULT createNewFault(char* _name, FAULT_TYPE _type);

/*
 *  Creates a fault object
 *
 *  @param  char* _name - the name of the wire stuck-at fault
 *  @param  FAULT_TYPE _type  - the type of the fault
 *  @return BOOLEAN - TRUE if the fault list was successfully generated and
 *                    FALSE otherwise
 */
//BOOLEAN createNewFault(char* _name, FAULT_TYPE _type);

#endif
