/*
 * =====================================================================================
 *
 *       Filename:  fault_simulation.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  25 March 2014
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
#include "logic_tables.h"
#include "fault_simulation.h"


/*
 *  Generates output gates output from the given pattern
 *
 *  @param  circuit - the circuit
 *  @param  info 	- gate information object
 *  @return SIM_RESULTS the simulation results
 */
SIM_RESULT test_pattern(CIRCUIT circuit, CIRCUIT_INFO* info, char* inPattern)
{
	SIM_RESULT results;


	return results;
}

/*  Simulates a given test vector to drop all the faults that can be detected
 *  using it.
 *
 *  @param  circuit - the circuit
 *  @param  info 	- gate information object
 *  @param  fList 	- fault list object
 *  @param  tv 		- test vector object
 *  @return nothing
 */
void simulateTestVector(CIRCUIT circuit, CIRCUIT_INFO* info, FAULT_LIST * fList,
						TEST_VECTOR* tv)
{
	

}	