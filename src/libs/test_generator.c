/*
 * =====================================================================================
 *
 *       Filename:  test_generator.c
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

#include <stdlib.h>
#include "globals.h"
#include "test_generator.h"
#include "stdio.h"
#include "strings.h"

extern volatile COMMAND_LINE_OPTIONS options;

/*
 *  Sets all gates value from the gate to the primary output as Don't-Cares (X)
 *
 *  @param  CIRCUIT 	circuit - the circuit
 *  @param  int 		index  	- the starting gate/wire
 *  @return nothing
 */
void clearPropagationValuesPath(CIRCUIT circuit, int index)
{
	// Clear all the input lines to the current gate
	int K, J;
	for(K = 0; K < circuit[index]->numIn; K++)
	{
		// Clear main segment
		circuit[circuit[index]->in[K]]->value = X;

		// Clear fan out segments
		if(circuit[circuit[index]->in[K]]->numOut > 1)
			for(J = 0; J < circuit[circuit[index]->in[K]]->numOut; J++)
				circuit[circuit[index]->in[K]]->values[J] = X;

		for(J = 0; J < MAX_LOGIC_VALUES; J++)
			circuit[circuit[index]->in[K]]->justified[J].state = FALSE;
	}

	// Stop when reaching a primary output or continue otherwise
	if(circuit[index]->PO == TRUE)
		return;
	else
		for(K = 0; K < circuit[index]->numOut; K++)
			clearPropagationValuesPath(circuit, circuit[index]->out[K]);
}

/*
 *  Clear propagation values for the entire circuit
 *
 *  @param  CIRCUIT circuit - the circuit
 *  @param  int 	info  	- the maximum number of gates
 *  @return nothing
 */
void clearPropagationValuesCircuit(CIRCUIT circuit, int numGates)
{
	int index, J;
	for(index = 0; index < numGates; index++)
	{
		// Clear main segment
		circuit[index]->value = X;

		// Clear fan out segments
		if(circuit[index]->numOut > 1)
			for(J = 0; J < circuit[index]->numOut; J++)
				circuit[index]->values[J] = X;
	}
}

/*
 *  Excites to the primary input the value given to a circuit line
 *
 *  @param  CIRCUIT 	circuit - the circuit
 *  @param  int 		index  	- the position of the gate in the <circuit>
 *  @param  int 		indexOut- the position of the fan out segment if present
 *  @param  LOGIC_VALUE	log_val	- the logical value to excite the given gate with
 *  @return BOOLEAN -  TRUE	 if the value can be excited and FALSE otherwise
 */
BOOLEAN excite(CIRCUIT circuit, int index, int indexOut, LOGIC_VALUE log_val)
{
	/*
	if(indexOut < 0)
		printf("Excite(%s with '%c')\n", circuit[index]->name, logicName(log_val, FALSE));
	else
		printf("Excite(%s->%s with '%c')\n", circuit[index]->name, circuit[indexOut]->name, 
				logicName(log_val, FALSE));
	*/

	int K, L;
	// Excite fanout segments if available
	if(circuit[index]->numOut > 1 && indexOut >= 0)
	{
		for(K = 0; K < circuit[index]->numOut; K++)
			if(circuit[index]->out[K] != indexOut)
				circuit[index]->values[K] = (log_val == X ? X : ((log_val == D || log_val == I) ? I : O));
		circuit[index]-> value = (log_val == X ? X : ((log_val == D || log_val == I) ? I : O));

		// Continue justifying the main segment
		indexOut = -1;
		log_val  = (log_val == X ? X : ((log_val == D || log_val == I) ? I : O));

		/*
		if(indexOut < 0)
			printf("Excite(%s with '%c')\n", circuit[index]->name, logicName(log_val, FALSE));
		else
			printf("Excite(%s->%s with '%c')\n", circuit[index]->name, circuit[indexOut]->name, 
					logicName(log_val, FALSE));
		*/
	}

	// A Primary Input does not need excitation
	if(circuit[index]->type == PI)
	{
		circuit[index]->value = log_val;

		if(circuit[index]->numOut > 1)
			for(K = 0; K < circuit[index]->numOut; K++)
				circuit[index]->values[K] = log_val;
		
		return TRUE;
	}

	// Excite a BUFFER
	BOOLEAN results;
	if(circuit[index]->type == BUF)
	{
		results = excite(circuit, (int) circuit[index]->in[0], -1,
					  (LOGIC_VALUE) negate(log_val, (BOOLEAN) circuit[index]->inv));
		
		circuit[index]->value = (results == TRUE ? log_val : X);

		if(circuit[index]->numOut > 1)
			for(L = 0; L < circuit[index]->numOut; L++)
				circuit[index]->values[L] = (results == TRUE ? log_val : X);

		return results;
	}

	// Logical value Don't-Care (X) does not need excitation
	// TODO check if this is true
	if(log_val == X) return TRUE;


	// Try exciting one of the current gates input
	int inLine = 0;
	for(; inLine < circuit[index]->numIn; inLine++)
	{
		if(excite(circuit, circuit[index]->in[inLine], -1, log_val) == TRUE)
		{
			LOGIC_VALUE other_value = X;
			switch(circuit[index]->type)
			{
				case AND: 
					if(circuit[index]->inv == FALSE) other_value = I;
					else other_value = O; 
					break;
				case OR : 
					if(circuit[index]->inv == FALSE) other_value = O;
					else other_value = I;
					break;
				default : other_value = X; break;
			}

			for(K = 0; K < circuit[index]->numIn; K++)
			{
				if(K == inLine) circuit[circuit[index]->in[K]]->value = log_val;
				else circuit[circuit[index]->in[K]]->value = other_value;
			}

			circuit[index]->value = log_val;
			
			if(circuit[index]->numOut > 1)
				for(L = 0; L < circuit[index]->numOut; L++)
					circuit[index]->values[L] = log_val;


			return TRUE;
		}
		else return FALSE;
	}
	return FALSE;
}

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
BOOLEAN justify(CIRCUIT circuit, int index, LOGIC_VALUE log_val)
{
	/*
	printf("---Justify(%s with '%c' found %c): \n", circuit[index]->name, logicName(log_val, FALSE), 
			logicName(circuit[index]->value, FALSE));
	*/

	// A Primary Input can be justified for any value
	if(circuit[index]->type == PI)
	{
		circuit[index]->justified[log_val].state = TRUE;
		circuit[index]->justified[log_val].value = TRUE;
		return TRUE;
	}

	// Logical value Don't-Care (X) does not need justification
	if(log_val == X) return TRUE;

	// Justify a BUFFER
	if(circuit[index]->type == BUF)
	{
		LOGIC_VALUE just_value = negate(log_val, circuit[index]->inv);
		if(circuit[circuit[index]->in[0]]->value == X )
			circuit[circuit[index]->in[0]]->value = just_value;

		// TODO Justify fanout segments
		
		return (justify(circuit, circuit[index]->in[0], just_value));
	}

	// Check if the current gate's output cannot be justified from its inputs
	LOGIC_VALUE result = computeGateOutput(circuit, index);
	while(result != log_val)
	{
		// Check if it is possible to justify if the Don't-Cares were manipulated
		if(isOutputPossible(circuit, index, log_val) == FALSE)
		{
			circuit[index]->justified[log_val].state = TRUE;
			circuit[index]->justified[log_val].value = FALSE;
			return FALSE;
		}
		else break;
	}

	// Justify the current gate's inputs
	int inLine = 0;
	for(; inLine < circuit[index]->numIn; inLine++)
		if(justify(circuit, circuit[index]->in[inLine], circuit[circuit[index]->in[inLine]]->value) == FALSE)
		{
			circuit[index]->justified[log_val].state = TRUE;
			circuit[index]->justified[log_val].value = FALSE;
			return FALSE;
		}
	return TRUE;
}

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
BOOLEAN propagate(CIRCUIT circuit, int index, int indexOut, LOGIC_VALUE log_val)
{
	/*
	if(indexOut < 0)
		printf("Propagate(%s with '%c')\n", circuit[index]->name, logicName(log_val, FALSE));
	else
		printf("Propagate(%s->%s with '%c')\n", circuit[index]->name,
				circuit[indexOut]->name, logicName(log_val, FALSE));
	*/

	int K;

	// Propagate through a fanout segment
	if(indexOut >= 0)
	{
		for(K = 0; K < circuit[index]->numOut; K++)
			if(circuit[index]->out[K] == indexOut)
				circuit[index]->values[K] = log_val;
		index = indexOut;
		indexOut = -1;

		/*
		if(indexOut < 0)
			printf("Propagate(%s with '%c')\n", circuit[index]->name, logicName(log_val, FALSE));
		else
			printf("Propagate(%s->%s with '%c')\n", circuit[index]->name,
					circuit[indexOut]->name, logicName(log_val, FALSE));
		*/
	}

	// Set this wire and it's fan-out segments to the propagated value
	circuit[index]->value = log_val;
	if(circuit[index]->numOut > 1 && indexOut < 0)
	{
		for(K = 0; K < circuit[index]->numOut; K++)
			circuit[index]->values[K] = log_val;
	}

	// Check if a Primary Output has been reached
	BOOLEAN results;
	if(circuit[index]->PO == TRUE)
	{
		results = justify(circuit, index, log_val);
		if(results == FALSE) 
		{
			bzero(circuit[index]->propagated, sizeof(PROP_OBJECT));
		}
		circuit[index]->propagated[log_val].state = TRUE;
		circuit[index]->propagated[log_val].value = results;
		return results;
	}

	// Logical value Don't-Care (X) does not need propagation
	if(log_val == X) {
		return TRUE;
	}

	// Try propagating the current gate's value into a Primary Output
	int outLine = 0, outIndex;
	for(; outLine < circuit[index]->numOut; outLine++)
	{
		LOGIC_VALUE other_value = X;
		outIndex = circuit[index]->out[outLine];
		switch(circuit[outIndex]->type)
		{
			case BUF:
			{
				LOGIC_VALUE prop_value = negate(log_val, circuit[outIndex]->inv);
				results = propagate(circuit, outIndex, -1, prop_value);
				circuit[outIndex]->value = (results == TRUE ? prop_value : X);
				if(results == TRUE) return TRUE;
				else continue;
			}
			case AND: other_value = I; break;
			case OR : other_value = O; break;
			default : other_value = X; break;
		}

		for(K = 0; K < circuit[outIndex]->numIn; K++)
			circuit[circuit[outIndex]->in[K]]->value = other_value;
		circuit[index]->value = log_val;

		results = propagate(circuit, outIndex, -1, negate(log_val, circuit[outIndex]->inv));
		if(results == TRUE)
		{
			return TRUE;
		}
		else {
			clearPropagationValuesPath(circuit, index);
			circuit[index]->value = log_val;
			if(circuit[index]->numOut > 1)
			for(K = 0; K < circuit[index]->numOut; K++)
				circuit[index]->values[K] = log_val;
		}
	}

	circuit[index]->propagated[log_val].state = TRUE;
	circuit[index]->propagated[log_val].value = FALSE;
	return FALSE;
}

/*
 *  Extracts a test pattern from the current input and output values
 *
 *  @param  CIRCUIT 	circuit - the circuit
 *  @param  CIRCUIT_INFO info  	- containing the list of inputs/outputs
 *  @param  TEST_VECTOR	tv 	- the test vector to output
 *  @return nothing
 */
void extractTestVector(CIRCUIT circuit, CIRCUIT_INFO* info, TEST_VECTOR *tv)
{
	bzero(tv->input, sizeof(tv->input));
	bzero(tv->output, sizeof(tv->output));
	tv->faults_count = 1;

	//	Get values of input gates
	int K;
	//printf("====> Inputs # %d\n", info->numPI);
	for(K = 0; K < info->numPI; K++)
	{
		switch(circuit[info->inputs[K]]->value)
		{
			case X: tv->input[K] = 'x'; break;
			case I:
			case D: tv->input[K] = '1'; break;
			case O:
			case B: tv->input[K] = '0'; break;
		}
	}

	// Get values of output gates
	for(K = 0; K < info->numPO; K++)
	{
		switch(circuit[info->outputs[K]]->value)
		{
			case X: tv->output[K] = 'x'; break;
			case I:
			case D: tv->output[K] = '1'; break;
			case O:
			case B: tv->output[K] = '0'; break;
		}
	}
}

/*
 *  Print to the standard output the given test vector
 *
 *  @param  CIRCUIT 	circuit - the circuit
 *  @param  TEST_VECTOR	tv 	- the test vector to output
 *  @param  int 	tpCount - current test patterns generated
 *  @return nothing
 */
void displayTestVector(CIRCUIT circuit, TEST_VECTOR* tv, int tpCount)
{
	if(options.isDebugMode) fprintf(stdout, "test %5d:\t%s\t%s\t%d", tpCount, tv->input, 
		tv->output, tv->faults_count);

	if(options.debugLevel < 2) fprintf(stdout, "\n");
	else
	{
		fprintf(stdout, "\t{");
		int K;
		for(K = 0; K < tv->faults_count; K++)
			if(tv->faults_list[K]->indexOut < 0)
				fprintf(stdout, " (%s, %d)", circuit[tv->faults_list[K]->index]->name, 
					tv->faults_list[K]->type);
			else
				fprintf(stdout, " (%s->%s, %d)", circuit[tv->faults_list[K]->index]->name, 
					circuit[tv->faults_list[K]->indexOut]->name, tv->faults_list[K]->type);
		fprintf(stdout, " }\n");
	}
}

/*
 *  Saves into the test pattern file the given test vector
 *
 *  @param  CIRCUIT 	circuit - the circuit
 *  @param  TEST_VECTOR	tv 	- the test vector to output
 *  @param  FILE*		fp 	- test patterns output file descriptor
 *  @param  int 	tpCount - current test patterns generated
 *  @return nothing
 */
void saveTestVector(CIRCUIT circuit, TEST_VECTOR* tv, FILE* fp, int tpCount)
{
	if(options.isDebugMode) fprintf(fp, "test %5d:\t%s\t%s\t%d", tpCount, tv->input, 
		tv->output, tv->faults_count);

	if(options.debugLevel < 2) fprintf(fp, "\n");
	else
	{
		fprintf(fp, "\t{");
		int K;
		for(K = 0; K < tv->faults_count; K++)
			if(tv->faults_list[K]->indexOut < 0)
				fprintf(fp, " (%s, %d)", circuit[tv->faults_list[K]->index]->name, 
					tv->faults_list[K]->type);
			else
				fprintf(fp, " (%s->%s, %d)", circuit[tv->faults_list[K]->index]->name, 
					circuit[tv->faults_list[K]->indexOut]->name, tv->faults_list[K]->type);
		fprintf(fp, " }\n");
	}
}
