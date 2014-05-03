/*
 * =====================================================================================
 *
 *       Filename:  fault_simulation.h
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


#ifndef FAULT_SIMULATION_H
#define FAULT_SIMULATION_H


/*
 *  Generates output gates output from the given pattern
 *
 *  @param  circuit - the circuit
 *  @param  info 	- gate information object
 *  @param  inPattern - input gates' values
 *  @param	fault 	- the fault to be excited as the pattern is generated
 *  @param  wasFaultExcited - for keeping track if the fault was excited during the execution
 *  @return SIM_RESULTS the simulation results
 */
SIM_RESULT test_pattern(CIRCUIT circuit, CIRCUIT_INFO* info, char* inPattern, 
						FAULT* fault, BOOLEAN* wasFaultExcited);

/*
 *  Generates output gates output from the given pattern
 *
 *  @param  circuit - the circuit
 *  @param  info 	- gate information object
 *  @param  inPattern - input gates' values
 *  @return SIM_RESULTS the simulation results
 */
SIM_RESULT generate_output(CIRCUIT circuit, CIRCUIT_INFO* info, char* inPattern);


/*
 *  Compares the levels of two gates
 *
 *  @param  lg 	- left gate's index
 *  @param  rg 	- right gate's index
 *  @return 0 if gate levels are the same, -1 if level of leftIndex is higher and
 *			1 if the level of the right index is higher
 */
int cmpGateLevels(const void *lg, const void *rg);


/*  Simulates a given test vector to drop all the faults that can be detected
 *  using it.
 *
 *  @param  circuit - the circuit
 *  @param  info 	- gate information object
 *  @param  fList 	- fault list object
 *  @param  tv 		- test vector object
 *  @param  start 	- starting point in the fault list
 *  @return nothing
 */
void simulateTestVector(CIRCUIT circuit, CIRCUIT_INFO* info, FAULT_LIST * fList,
						TEST_VECTOR* tv, int start);

#endif
