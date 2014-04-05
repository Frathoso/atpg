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
#include "parser_netlist.h"


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

	// Initialize the propagation queue
	TAILQ_HEAD(tailhead, entry) head;
    //struct tailhead *headp;                 /* Tail queue head. */
    struct entry
    {
        GATE *gatep;
        TAILQ_ENTRY(entry) entries;         /* Tail queue. */
    }*np;

    TAILQ_INIT(&head);  

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

		struct entry* node = malloc(sizeof(struct entry));
		node->gatep = circuit[info->inputs[K]];

		// Add node to the queue
		TAILQ_INSERT_TAIL(&head, node, entries);
	}

	// generate output values
	LOGIC_VALUE tValue;
	while(head.tqh_first != NULL)
	{
		// Propagate results through the current node
		np = head.tqh_first;

		if(np->gatep->type != PI)
		{
			for(L = 0; L < np->gatep->numOut; L++)
			{
				
			}
			tValue = computeGateOutput(circuit, findIndex(circuit, &info->numGates, np->gatep->name, FALSE));
		}
		else
			tValue = np->gatep->value;

		printf("\n--->%s: %c", np->gatep->name, logicName(tValue));

		// Add output lines into the queue
		for(L = 0; L < np->gatep->numOut; L++)
		{
			struct entry* node = malloc(sizeof(struct entry));
			node->gatep = circuit[np->gatep->out[L]];
			TAILQ_INSERT_TAIL(&head, node, entries);
		}


		TAILQ_REMOVE(&head, np, entries);
	}

	// Retrieve output results;
	char outValues[MAX_OUTPUT_GATES];
	for(K = 0; K < info->numPO; K++)
	{

		outValues[K] = logicName(circuit[info->outputs[K]]->value);
	}
	outValues[K] = '\0';

	strcpy(results.output, outValues);

	return results;
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
			SIM_RESULT results = test_pattern(circuit, info, tv->input);

			BOOLEAN matches = TRUE;
			if(strcmp(results.output, tv->output) != 0)
			{
				for(L = 0; L < strlen(results.output); L++)
					if(results.output[L] == 'X' || results.output[L] != tv->output[L])
					{
						matches = FALSE;
						break;
					}
			}


			// Remove fault from list if it can be detected
			if(matches == TRUE)
			{
				tv->faults_count = tv->faults_count + 1;
			free(fList->list[K]);
			fList->list[K] = NULL;
			}
		}
	}
}	