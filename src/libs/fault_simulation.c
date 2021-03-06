/*
 * =====================================================================================
 *
 *       Filename:  fault_simulation.c
 *
 *    Description:	Defines functions that are used to perform circuit’s simulations
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
#include <time.h>
#include <sys/queue.h>

#include "atpg_types.h"
#include "logic_tables.h"
#include "fault_simulation.h"
#include "test_generator.h"
#include "parser_netlist.h"
#include "pqueue.h"


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
						FAULT* fault, BOOLEAN* wasFaultExcited)
{
	SIM_RESULT results;

	//printf("Pattern: %s\n", inPattern);
	// Initialize the propagation priority queue list
	PQueue *pqList = pqueue_new(cmpGateLevels, 3*MAX_GATES);

	// Assign test pattern to input gates
	int K, L;
	for(K = 0; K < info->numPI; K++)
	{
		switch(inPattern[K])
		{
			case 'I':
			case '1': circuit[info->inputs[K]]->value = I; break;
			case 'O':
			case '0': circuit[info->inputs[K]]->value = O; break;
			case 'D': circuit[info->inputs[K]]->value = D; break;
			case 'B': circuit[info->inputs[K]]->value = B; break;
			case 'x': circuit[info->inputs[K]]->value = X; break;
		}

		// Add gate to the queue
		pqueue_enqueue(pqList, circuit[info->inputs[K]]);
	}

	//printf("Value: %c\n", logicName(circuit[faultLine]->value));

	// generate output values
	LOGIC_VALUE tValue;
	GATE * gate;
	while(!is_empty(pqList))
	{
		// Select the next gate to propagate the values (i.e. the gate with the least level)
		gate = (GATE*) pqueue_dequeue(pqList);

		int index = findIndex(circuit, &info->numGates, gate->name, FALSE);

		// Determine the gate's new logical value
		if(gate->type == PI)
			tValue = gate->value;
		else
			tValue = computeGateOutput(circuit, index);

		// Check if this gate is the one stuck-at fault and see if the fault was excited
		if(index == fault->index)
		{
			if(fault->type == ST_0 && (tValue == I || tValue == D))
			{
				*wasFaultExcited = TRUE;
				tValue = D;
			}
			else if(fault->type == ST_1 && (tValue == O || tValue == B))
			{
				*wasFaultExcited = TRUE;
				tValue = B;
			}
			else
			{
				*wasFaultExcited = FALSE;
				// Stop execution, the fault cannot be excited with the current pattern
				//printf("\nFault %s /%d cannon be excited\n", )
				return results;
			}
		}

		gate->value = tValue;

		/*
		printf("\n\t--->%s (%+d): %c", gate->name, gate->level, logicName(tValue));
		if(gate->level >= 0)
		{
			for(L=0; L<gate->numIn; L++) printf(" (%s=%c) ", circuit[gate->in[L]]->name, logicName(circuit[gate->in[L]]->value));
			//printf(" %d, %s, %s --> ", index, circuit[4]->name, gate->name);
		}
		*/

		// Add new output lines into the queue
		for(L = 0; L < gate->numOut; L++)
		{
			if(circuit[gate->out[L]]->value == X)
			{
				circuit[gate->out[L]]->value = tValue;
				pqueue_enqueue(pqList, circuit[gate->out[L]]);
			}
		}
	}

	//printf("\nDONE\n");
	// Clear the priority list heap
	pqueue_delete(pqList);

	// Retrieve output results;
	for(K = 0; K < info->numPO; K++)
		results.output[K] = logicName(circuit[info->outputs[K]]->value, FALSE);
	results.output[K] = '\0';

	// Retrieve input values
	for(K = 0; K < info->numPI; K++)
		results.input[K] = logicName(circuit[info->inputs[K]]->value, FALSE);
	results.input[K] = '\0';

	return results;
}


/*
 *  Generates output gates output from the given pattern
 *
 *  @param  circuit - the circuit
 *  @param  info 	- gate information object
 *  @param  inPattern - input gates' values
 *  @return SIM_RESULTS the simulation results
 */
SIM_RESULT generate_output(CIRCUIT circuit, CIRCUIT_INFO* info, char* inPattern)
{
	SIM_RESULT results;

	// TODO take fanout into consideration

	//printf("Pattern: %s\n", inPattern);
	// Initialize the propagation priority queue list
	PQueue *pqList = pqueue_new(cmpGateLevels, 3*MAX_GATES);

	// Assign test pattern to input gates
	int K, L;
	for(K = 0; K < info->numPI; K++)
	{
		switch(inPattern[K])
		{
			case '1': circuit[info->inputs[K]]->value = I; break;
			case '0': circuit[info->inputs[K]]->value = O; break;
			case 'd':
			case 'D': circuit[info->inputs[K]]->value = D; break;
			case 'b':
			case 'B': circuit[info->inputs[K]]->value = B; break;
			case 'x':
			case 'X': circuit[info->inputs[K]]->value = X; break;
		}

		// Add gate to the queue
		pqueue_enqueue(pqList, circuit[info->inputs[K]]);
	}

	// generate output values
	LOGIC_VALUE tValue;
	GATE * gate;
	while(!is_empty(pqList))
	{
		// Select the next gate to propagate the values (i.e. the gate with the least level)
		gate = (GATE*) pqueue_dequeue(pqList);

		int index = findIndex(circuit, &info->numGates, gate->name, FALSE);

		// Determine the gate's new logical value
		if(gate->type == PI)
			tValue = gate->value;
		else
			tValue = computeGateOutput(circuit, index);

		gate->value = tValue;

		/*
		printf("\n\t--->%s (%+d): %c", gate->name, gate->level, logicName(tValue, FALSE));
		if(gate->level >= 0)
		{
			for(L=0; L<gate->numIn; L++) printf(" (%s=%c) ", circuit[gate->in[L]]->name, logicName(circuit[gate->in[L]]->value, FALSE));
			//printf(" [%d, %s, %s --> ", index, circuit[4]->name, gate->name);
		}
		*/
		

		// Add new output lines into the queue
		for(L = 0; L < gate->numOut; L++)
		{
			if(circuit[gate->out[L]]->value == X)
			{
				circuit[gate->out[L]]->value = tValue;
				pqueue_enqueue(pqList, circuit[gate->out[L]]);
			}
		}
	}

	//printf("\nDONE\n");
	// Clear the priority list heap
	pqueue_delete(pqList);

	// Retrieve output results;
	for(K = 0; K < info->numPO; K++)
		results.output[K] = logicName(circuit[info->outputs[K]]->value, TRUE);
	results.output[K] = '\0';

	// Retrieve input values
	for(K = 0; K < info->numPI; K++)
		results.input[K] = logicName(circuit[info->inputs[K]]->value, TRUE);
	results.input[K] = '\0';

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
	// Remove Don't-Cares
	int K;
	extern COMMAND_LINE_OPTIONS options;
	if(options.dontCareFilling == ONES)
	{
		for(K = 0; K < info->numPI; K++)
			if( tv->input[K] == 'x') tv->input[K] = '1';
	}
	else if(options.dontCareFilling == ZEROS)
	{
		for(K = 0; K < info->numPI; K++)
			if( tv->input[K] == 'x') tv->input[K] = '0';
	}
	else
	{
		srand(time(NULL));
		for(K = 0; K < info->numPI; K++)
			if( tv->input[K] == 'x')
			{
				if(rand() % 2)	tv->input[K] = '1';
				else tv->input[K] = '0';
			}
	}

	// Simulate all remaining faults using the current pattern
	BOOLEAN wasFaultExcited;
	//printf("\nSimulate %s stuck at %d\n", circuit[fList->list[K]->index]->name, fList->list[K]->type);
	for(K = start; K < fList->count; K++)
	{
		if(fList->list[K]->detected == FALSE)
		{
			// Prepare the circuit for simulation
			//printf("\nSimulate %s stuck at %d", circuit[fList->list[K]->index]->name, fList->list[K]->type);
			clearPropagationValuesCircuit(circuit, info->numGates);

			// Simulate the fault
			wasFaultExcited = FALSE;
			SIM_RESULT results = test_pattern(circuit, info, tv->input, fList->list[K], &wasFaultExcited);

			//if(wasFaultExcited == TRUE)	printf("\t%s -> %s\n", results.input, results.output);

			// Remove fault from list if it can be detected
			if(wasFaultExcited == TRUE)
			{ 
				int L;
				BOOLEAN valid = FALSE;
				for(L = 0; L < strlen(results.output) && valid == FALSE; L++)
				{
					//printf("[%s->%s, %s->%s] ", tv->input, tv->output, results.input, results.output);
					switch(results.output[L])
					{
						case '1':
						case 'I':
							if(tv->output[L] == '0' || tv->output[L] == 'B') valid = TRUE;
							break;
						case '0':
						case 'O':
							if(tv->output[L] == '1' || tv->output[L] == 'D') valid = TRUE;
							break;
						case 'D':
						case 'B':
							valid = TRUE;
							break;
					}
				}

				if(valid == TRUE)
				{
					// Add the fault into the patterns fault list first
					tv->faults_list[tv->faults_count] = (FAULT*) malloc(sizeof(FAULT));
					tv->faults_list[tv->faults_count]->index 	= fList->list[K]->index;
					tv->faults_list[tv->faults_count]->indexOut = fList->list[K]->indexOut;
					tv->faults_list[tv->faults_count]->type 	= fList->list[K]->type;
					tv->faults_count = tv->faults_count + 1;

					// Remove the fault from the list of undetected faults
					//printf("\t[%s]: Collapsed!\n", circuit[fList->list[K]->index]->name);
					fList->list[K]->detected = TRUE;
				}
			}
		}
	}
	//printf("\n");
}	