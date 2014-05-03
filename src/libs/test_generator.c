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
		circuit[circuit[index]->in[K]]->value = X;

		for(J = 0; J < MAX_LOGIC_VALUES; J++)
			circuit[circuit[index]->in[K]]->justified[J].state = FALSE;
	}

	// Stop when reaching a primar output or continue clearing the output lines otherwise
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
	int index;
	for(index = 0; index < numGates; index++)
		circuit[index]->value = X;
}

/*
 *  Excites to the primary input the value given to a circuit line
 *
 *  @param  CIRCUIT 	circuit - the circuit
 *  @param  int 		index  	- the position of the gate in the <circuit>
 *  @param  LOGIC_VALUE	log_val	- the logical value to excite the given gate with
 *  @return BOOLEAN -  TRUE	 if the value can be excited and FALSE otherwise
 */
BOOLEAN excite(CIRCUIT circuit, int index, LOGIC_VALUE log_val)
{
	//printf("Excite(%s with '%c')\n", circuit[index]->name, logicName(log_val));

	// A Primary Input does not need excitation
	if(circuit[index]->type == PI)
	{
		circuit[index]->value = log_val;
		return TRUE;
	}

	// Excite a BUFFER
	BOOLEAN results;
	if(circuit[index]->type == BUF)
	{
		results = excite(circuit, (int) circuit[index]->in[0], 
					  (LOGIC_VALUE) negate(log_val, (BOOLEAN) circuit[index]->inv));
		circuit[index]->value = (results == TRUE ? log_val : X);
		return results;
	}

	// Logical value Don't-Care (X) does not need excitation
	// TODO check if this is true
	if(log_val == X) return TRUE;


	// Try exciting one of the current gates input
	int inLine = 0, K;
	for(; inLine < circuit[index]->numIn; inLine++)
	{
		if(excite(circuit, circuit[index]->in[inLine], log_val) == TRUE)
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
	//printf("---Justify(%s with '%c' found %c): \n", circuit[index]->name, logicName(log_val), 
	//		logicName(circuit[index]->value));

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
		

		return (justify(circuit, circuit[index]->in[0], just_value));
	}

	// Check if the current gate's output cannot be justified from its inputs
	LOGIC_VALUE result = computeGateOutput(circuit, index);
	if(result != log_val)
	{
		// Check if it is possible to justify if the Don't-Cares were manipulated
		if(isOutputPossible(circuit, index, log_val) == FALSE)
		{
			circuit[index]->justified[log_val].state = TRUE;
			circuit[index]->justified[log_val].value = FALSE;
			return FALSE;
		}
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
 *  @param  LOGIC_VALUE	log_val	- the logical value to justify the given gate with
 *  @return BOOLEAN -  TRUE	 if the value can be justified and FALSE if the 
 *					   value causes conflicts
 */
BOOLEAN propagate(CIRCUIT circuit, int index, LOGIC_VALUE log_val)
{
	//printf("Propagate(%s with '%c')\n", circuit[index]->name, logicName(log_val));

	// Set this wire to the propagated value
	circuit[index]->value = log_val;

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
	int outLine = 0, outIndex, K;
	for(; outLine < circuit[index]->numOut; outLine++)
	{
		LOGIC_VALUE other_value = X;
		outIndex = circuit[index]->out[outLine];
		switch(circuit[outIndex]->type)
		{
			case BUF:
			{
				LOGIC_VALUE prop_value = negate(log_val, circuit[outIndex]->inv);
				results = propagate(circuit, outIndex, prop_value);
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

		results = propagate(circuit, outIndex, negate(log_val, circuit[outIndex]->inv));
		if(results == TRUE)
		{
			return TRUE;
		}
		else {
			clearPropagationValuesPath(circuit, index);
			circuit[index]->value = log_val;
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
			case X: tv->input[K] = 'X'; break;
			case I:
			case D: tv->input[K] = 'I'; break;
			case O:
			case B: tv->input[K] = 'O'; break;
		}
	}

	// Get values of output gates
	for(K = 0; K < info->numPO; K++)
	{
		switch(circuit[info->outputs[K]]->value)
		{
			case X: tv->output[K] = 'X'; break;
			case I:
			case D: tv->output[K] = 'I'; break;
			case O:
			case B: tv->output[K] = 'O'; break;
		}
	}
}

/*
 *  Print to the standard output the given test vector
 *
 *  @param  CIRCUIT 	circuit - the circuit
 *  @param  TEST_VECTOR	tv 	- the test vector to output
 *  @return nothing
 */
void displayTestVector(CIRCUIT circuit, TEST_VECTOR* tv)
{
	if(options.isDebugMode) fprintf(stdout, "%s\t%s\t%d", tv->input, tv->output, tv->faults_count);

	if(options.debugLevel < 2) fprintf(stdout, "\n");
	else
	{
		fprintf(stdout, "\t{");
		int K;
		for(K = 0; K < tv->faults_count; K++)
			printf(" (%s, %d)", circuit[tv->faults_list[K]->index]->name, tv->faults_list[K]->type);
		printf(" }\n");
	}
}

/*
 *  Saves into the test pattern file the given test vector
 *
 *  @param  CIRCUIT 	circuit - the circuit
 *  @param  TEST_VECTOR	tv 	- the test vector to output
 *  @param  FILE*		fp 	- test patterns output file descriptor
 *  @return nothing
 */
void saveTestVector(CIRCUIT circuit, TEST_VECTOR* tv, FILE* fp)
{
	fprintf(fp, "%s\t%s\t%d\t{", tv->input, tv->output, tv->faults_count);
	int K;
	for(K = 0; K < tv->faults_count; K++)
		fprintf(fp, " (%s, %d)", circuit[tv->faults_list[K]->index]->name, tv->faults_list[K]->type);
	fprintf(fp, " }\n");
}