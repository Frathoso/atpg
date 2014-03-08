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

#include "test_generator.h"
#include "stdio.h"
#include "strings.h"

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
					  (BOOLEAN) negate(log_val, (BOOLEAN) circuit[index]->inv));
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
				case AND: other_value = I; break;
				case OR : other_value = O; break;
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
	//printf("Justify(%s with '%c')\n", circuit[index]->name, logicName(log_val));

	// Check if the gate has already been justified
	//if(circuit[index]->justified[log_val].state == TRUE) 
		//return circuit[index]->justified[log_val].value;

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
		BOOLEAN result;
		if(circuit[circuit[index]->in[0]]->value == X)
		{
			circuit[circuit[index]->in[0]]->value = (LOGIC_VALUE) 
						negate(circuit[index]->value, circuit[index]->inv);
		}
		else
		{
			if(log_val != (LOGIC_VALUE) negate(circuit[circuit[index]->in[0]]->value, 
				circuit[index]->inv))
			{
				circuit[index]->justified[log_val].state = TRUE;
				circuit[index]->justified[log_val].value = FALSE;
				return FALSE;
			}
		}


		result = justify(circuit, circuit[index]->in[0], circuit[circuit[index]->in[0]]->value);
		if(result == TRUE)
		{
			circuit[index]->justified[log_val].state = TRUE;
			circuit[index]->justified[log_val].value = TRUE;
			return TRUE;
		}
		else
		{
			circuit[index]->justified[log_val].state = TRUE;
			circuit[index]->justified[log_val].value = FALSE;
			return FALSE;
		}
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

	// Check if the value has already been propagated through this gate
	//if(circuit[index]->propagated[log_val].state == TRUE) 
		//return circuit[index]->propagated[log_val].value;

	// Check if a Primary Output has been reached
	if(circuit[index]->PO == TRUE)
	{
		BOOLEAN results = justify(circuit, index, log_val);
		if(results == FALSE) 
		{
			//printf("--> Justification Failed\n");
			bzero(circuit[index]->propagated, sizeof(PROP_OBJECT));
		}
		circuit[index]->propagated[log_val].state = TRUE;
		circuit[index]->propagated[log_val].value = results;
		return results;
	}

	// Logical value Don't-Care (X) does not need propagation
	if(log_val == X) {
		printf("+++++++>NDIYO\n");
		return TRUE;
	}

	// Propagate through a BUFFER
	/*
	if(circuit[index]->type == BUF)
	{
		LOGIC_VALUE prop_log_val;
		if(circuit[index]->inv == TRUE)		// An INVERTER
		{
			prop_log_val = negate(circuit[circuit[index]->in[0]]->value, TRUE);
			circuit[index]->value =  prop_log_val;	// Modify the propagated value

			int K;
			int outIndex = circuit[index]->out[0];
			LOGIC_VALUE log_val_other;
			switch(circuit[outIndex]->type)
			{
				case AND:
					log_val_other = I;	// Other input lines need to be 1 for propagation
					for(K = 0; K < circuit[outIndex]->numIn; K++)
						if(circuit[outIndex]->in[K] != index)
							circuit[circuit[outIndex]->in[K]]->value = log_val_other;
					break;
				case OR:
				{
					log_val_other = O;	// Other input lines need to be O for propagation
					for(K = 0; K < circuit[outIndex]->numIn; K++)
						if(circuit[outIndex]->in[K] != index)
							circuit[circuit[outIndex]->in[K]]->value = log_val_other;
					break;
				}
				case XOR:
				{
					// TODO Implement XOR propagation
					break;
				}
				// TODO: Implement other types of gates
				default:
					break;
			}

			if(propagate(circuit, circuit[index]->out[0], prop_log_val) == TRUE)
			{
				circuit[index]->propagated[log_val].state = TRUE;
				circuit[index]->propagated[log_val].value = TRUE;
				return TRUE;
			}
			else
			{
				circuit[index]->propagated[log_val].state = TRUE;
				circuit[index]->propagated[log_val].value = FALSE;
				return FALSE;
			}
		}
		else	// NORMAL BUFFER 
		{
			if(propagate(circuit, circuit[index]->out[0], log_val) == TRUE)
			{
				circuit[index]->propagated[log_val].state = TRUE;
				circuit[index]->propagated[log_val].value = TRUE;
				return TRUE;
			}
			else
			{
				circuit[index]->propagated[log_val].state = TRUE;
				circuit[index]->propagated[log_val].value = FALSE;
				return FALSE;
			}
		}
	}
	*/

	// Try propagating the current gate's value into a Primary Output
	int outLine = 0, outIndex;
	for(; outLine < circuit[index]->numOut; outLine++)
	{
		// Retrieve the index of the output gate
		outIndex = circuit[index]->out[outLine];

		// Compute the logical value to be propagated based on the current value
		// and type of the gate.
		LOGIC_VALUE prop_log_val = log_val;	// Propagate the current value as-is
		LOGIC_VALUE log_val_other = X;
		int K;
		switch(circuit[outIndex]->type)
		{
			case AND:
				log_val_other = I;	// Other input lines need to be 1 for propagation
				for(K = 0; K < circuit[outIndex]->numIn; K++)
					if(circuit[outIndex]->in[K] != index)
						circuit[circuit[outIndex]->in[K]]->value = log_val_other;
				break;
			case OR:
			{
				log_val_other = O;	// Other input lines need to be O for propagation
				for(K = 0; K < circuit[outIndex]->numIn; K++)
					if(circuit[outIndex]->in[K] != index)
						circuit[circuit[outIndex]->in[K]]->value = log_val_other;
				break;
			}
			case XOR:
			{
				// TODO Implement XOR propagation
				break;
			}
			// TODO: Implement other types of gates
			default:
				break;
		}

		// Try propagating the value further to the Primary Output
		if(propagate(circuit, outIndex, prop_log_val) == TRUE)
		{
			circuit[index]->propagated[log_val].state = TRUE;
			circuit[index]->propagated[log_val].value = TRUE;
			return TRUE;
		}
		else
		{
			// Clear the propagation values
			clearPropagationValuesPath(circuit, outIndex);
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
 *  @return TEST_VECTOR - the generated test vector
 */
TEST_VECTOR extractTestVector(CIRCUIT circuit, CIRCUIT_INFO* info)
{
	TEST_VECTOR tv;
	bzero(tv.input, sizeof(tv.input));
	bzero(tv.output, sizeof(tv.output));
	tv.faults_count = 1;

	//	Get values of input gates
	int K;
	for(K = 0; K < info->numPI; K++)
		switch(circuit[info->inputs[K]]->value)
		{
			case X:
			case I:
			case O: tv.input[K] = logicName(circuit[info->inputs[K]]->value); break;
			case D: tv.input[K] = 'I'; break;
			case B: tv.input[K] = 'O'; break;
		}

	// Get values of output gates
	for(K = 0; K < info->numPO; K++)
		tv.output[K] = logicName(circuit[info->outputs[K]]->value);

	return tv;
}

/*
 *  Print to the standard output the given test vector
 *
 *  @param  TEST_VECTOR	tv 	- the test vector to output
 *  @return nothing
 */
void displayTestVector(TEST_VECTOR tv)
{
	printf("%s\t\t%s\t\t\t%d\n", tv.input, tv.output, tv.faults_count);
}