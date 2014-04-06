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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/queue.h>

#include "atpg_types.h"
#include "logic_tables.h"
#include "fault_simulation.h"
#include "test_generator.h"
#include "parser_netlist.h"
#include "pqueue.h"

int faultLine = -1;
LOGIC_VALUE stuck_at;

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

	//printf("Pattern: %s\n", inPattern);
	// Initialize the propagation priority queue list
	PQueue *pqList = pqueue_new(cmpGateLevels, 2*MAX_GATES);

	// Assign test pattern to input gates
	int K, L;
	for(K = 0; K < info->numPI; K++)
	{
		switch(inPattern[K])
		{
			case 'I': circuit[info->inputs[K]]->value = I; break;
			case 'O': circuit[info->inputs[K]]->value = O; break;
			case 'D': circuit[info->inputs[K]]->value = D; break;
			case 'B': circuit[info->inputs[K]]->value = B; break;
			case 'X': circuit[info->inputs[K]]->value = X; break;
		}

		// Add gate to the queue
		pqueue_enqueue(pqList, circuit[info->inputs[K]]);
	}
	circuit[faultLine]->value = stuck_at;

	//printf("Value: %c\n", logicName(circuit[faultLine]->value));

	/*
	while(!is_empty(pqList)){
		GATE* g = (GATE*) pqueue_dequeue(pqList);
		printf("\nTest %s: %d", g->name, g->level);
	}
	exit(0);
	*/

	// generate output values
	LOGIC_VALUE tValue;
	GATE * gate;
	while(!is_empty(pqList))
	{
		// Propagate results through the next gate with least level
		gate = (GATE*) pqueue_dequeue(pqList);

		int index = findIndex(circuit, &info->numGates, gate->name, FALSE);
		if(gate->type == PI)
			tValue = gate->value;
		else
			tValue = computeGateOutput(circuit, index);

		printf("\n\t--->%s (%+d): %c", gate->name, gate->level, logicName(tValue));

		if(gate->level >= 0)
		{
			for(L=0; L<gate->numIn; L++) printf(" (%s=%c) ", circuit[gate->in[L]]->name, logicName(circuit[gate->in[L]]->value));
			//printf(" %d, %s, %s --> ", index, circuit[4]->name, gate->name);
		}

		// Add new output lines into the queue
		for(L = 0; L < gate->numOut; L++)
		{
			if(circuit[gate->out[L]]->value == X)
			{
				circuit[gate->out[L]]->value = tValue;
				circuit[faultLine]->value = stuck_at;
				pqueue_enqueue(pqList, circuit[gate->out[L]]);
			}
		}
	}

	printf("\nDONE\n");
	// Clear the priority list heap
	pqueue_delete(pqList);

	// Retrieve output results;
	char outValues[MAX_OUTPUT_GATES];
	for(K = 0; K < info->numPO; K++)
		outValues[K] = logicName(circuit[info->outputs[K]]->value);
	outValues[K] = '\0';

	for(K = 0; K < info->numPI; K++)
		results.input[K] = logicName(circuit[info->inputs[K]]->value);
	results.input[K] = '\0';

	strcpy(results.output, outValues);

	return results;
}


/*
 *  Compares the levels of two gates
 *
 *  @param  lg 	- left gate's index
 *  @param  rg 	- right gate's index
 *  @return 0 if gate levels are the same, 1 if level of leftIndex is higher and
 *			-1 if the level of the right index is higher
 */
int cmpGateLevels(const void *lg, const void *rg)
{
	GATE *leftGate = (GATE*) lg;
	GATE *rightGate = (GATE*) rg;
	if(leftGate->level == rightGate->level) return 0;
	else if(leftGate->level > rightGate->level) return -1;
	else return 1;
}

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
						TEST_VECTOR* tv, int start)
{
	// Remove Don't-Cares by appending random I or 0
	int K, L;
	for(K = 0; K < strlen(tv->input); K++)
		if( tv->input[K] == 'X')
			tv->input[K] = 'X';

	// Simulate all remaining faults using the current pattern
	for(K = start; K < fList->count; K++)
	{
		if(fList->list[K] != NULL)
		{
			printf("\nSimulate %s stuck at %d", circuit[fList->list[K]->index]->name, fList->list[K]->type);
			clearPropagationValuesCircuit(circuit, info->numGates);
			circuit[fList->list[K]->index]->value = (fList->list[K]->type == ST_0 ? D : B);
			faultLine = fList->list[K]->index;
			stuck_at = (fList->list[K]->type == ST_0 ? D : B);
			

			SIM_RESULT results = test_pattern(circuit, info, tv->input);

			//printf("Results: %s stuck at %d: %s -> %s (%s)\n", circuit[faultLine]->name, stuck_at, tv->input, results.output, tv->output);

			BOOLEAN matches = FALSE;
			for(L = 0; L < strlen(tv->input); L++)
					circuit[faultLine]->value = (stuck_at == D ? I : O);
			if(strcmp(results.output, tv->output) == 0 && strcmp(results.input, tv->input) == 0)
			{ 
					matches = TRUE;
				/*
				for(L = 0; L < strlen(results.output); L++)
					if(!(results.output[L] == 'X' || results.output[L] == tv->output[L] ||
					     (results.output[L] == I && tv->output[L] == B) || (results.output[L] == O && tv->output[L] == D)))
					{
						matches = FALSE;
						break;
					}
				*/
			}


			// Remove fault from list if it can be detected
			if(matches == TRUE)
			{
				tv->faults_count = tv->faults_count + 1;
				printf("\t[%s]: Collapsed!\n", circuit[fList->list[K]->index]->name);
				free(fList->list[K]);
				fList->list[K] = NULL;
			}
		}
	}
}	