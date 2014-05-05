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
#include <stdio.h>
#include "atpg_types.h"
#include "logic_tables.h"


#ifndef TEST_GENERATOR_H
#define TEST_GENERATOR_H


/*
 *  Sets all gates value from the gate to the primary output as Don't-Cares (X)
 *
 *  @param  CIRCUIT 	circuit - the circuit
 *  @param  int 		index  	- the starting gate/wire
 *  @return nothing
 */
void clearPropagationValuesPath(CIRCUIT circuit, int index);

/*
 *  Clear propagation values for the entire circuit
 *
 *  @param  CIRCUIT circuit - the circuit
 *  @param  int 	info  	- the maximum number of gates
 *  @return nothing
 */
void clearPropagationValuesCircuit(CIRCUIT circuit, int numGates);

/*
 *  Excites to the primary input the value given to a circuit line
 *
 *  @param  CIRCUIT 	circuit - the circuit
 *  @param  int 		index  	- the position of the gate in the <circuit>
 *  @param  int 		indexOut- the position of the fan out segment if present
 *  @param  LOGIC_VALUE	log_val	- the logical value to excite the given gate with
 *  @return BOOLEAN -  TRUE	 if the value can be excited and FALSE otherwise
 */
BOOLEAN excite(CIRCUIT circuit, int index, int indexOut, LOGIC_VALUE log_val);

/*
 *  Justifies to the primary input the value given to a circuit line
 *
 * 	ASSUMPTIONS: 
 *		+ Input line values have already been set during propagation
 *
 *  @param  CIRCUIT 	circuit - the circuit
 *  @param  int 		index  	- the position of the gate in the <circuit>
 *  @param  LOGIC_VALUE	log_val	- the logical value to justify the given gate with
 *  @return BOOLEAN -  TRUE	 if the value can be justified and FALSE if the 
 *					   value causes conflicts
 */
BOOLEAN justify(CIRCUIT circuit, int index, LOGIC_VALUE log_val);

/*
 *  Propagates to the primary output the value given to a circuit line
 *
 *  @param  CIRCUIT 	circuit - the circuit
 *  @param  int 		index  	- the position of the gate in the <circuit>
 *  @param  int 		indexOut- the position of the fan out segment if present
 *  @param  LOGIC_VALUE	log_val	- the logical value to justify the given gate with
 *  @return BOOLEAN -  TRUE	 if the value can be justified and FALSE if the 
 *					   value causes conflicts
 */
BOOLEAN propagate(CIRCUIT circuit, int index, int indexOut, LOGIC_VALUE log_val);

/*
 *  Extracts a test pattern from the current input and output values
 *
 *  @param  CIRCUIT 	circuit - the circuit
 *  @param  CIRCUIT_INFO info  	- containing the list of inputs/outputs
 *  @param  TEST_VECTOR	tv 	- the test vector to output
 *  @return nothing
 */
void extractTestVector(CIRCUIT circuit, CIRCUIT_INFO* info, TEST_VECTOR *tv);

/*
 *  Print to the standard output the given test vector
 *
 *  @param  CIRCUIT 	circuit - the circuit
 *  @param  TEST_VECTOR	tv 	- the test vector to output
 *  @param  int 	tpCount - current test patterns generated
 *  @return nothing
 */
void displayTestVector(CIRCUIT circuit, TEST_VECTOR* tv, int tpCount);

/*
 *  Saves into the test pattern file the given test vector
 *
 *  @param  CIRCUIT 	circuit - the circuit
 *  @param  TEST_VECTOR	tv 	- the test vector to output
 *  @param  FILE*		fp 	- test patterns output file descriptor
 *  @param  int 	tpCount - current test patterns generated
 *  @return nothing
 */
void saveTestVector(CIRCUIT circuit, TEST_VECTOR* tv, FILE* fp, int tpCount);

#endif
