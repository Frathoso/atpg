/*
 * =====================================================================================
 *
 *       Filename:  fault_list_generator.c
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

/* strcpy */
#include <string.h>

#include "globals.h"
#include "fault_list_generator.h"

/*
 *  Creates a fault object
 *
 *  @param  char* _name - the name of the wire stuck-at fault
 *  @param  FAULT_TYPE _type  - the type of the fault
 *  @return FAULT - the created fault object
 */
FAULT createNewFault(char* _name, FAULT_TYPE _type)
{
    FAULT fault;
    strcpy(fault.name, _name);
    fault.type = _type;

    return (fault);
}


