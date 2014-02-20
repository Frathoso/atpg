/*
 * =====================================================================================
 *
 *       Filename:  test_generator.h
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

#ifndef TEST_GENERATOR_H
#define TEST_GENERATOR_H


/*
 *  A test vector structure
 */
typedef struct
{
    char input[MAX_INPUT_GATES];    // Input gates values
    char output[MAX_OUTPUT_GATES];   // Output gates values
    int  faults_count;              // Total stuck-at faults the test can detect
} TEST_VECTOR;



/*
 *  Print the details of a circuit
 *
 *  @param  CIRCUIT circuit - the circuit
 *  @param  int totalGates  - total number of gates currently in the circuit
 *  @return nothing
 */

#endif
