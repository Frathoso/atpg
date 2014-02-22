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
	printf("justify(%s with '%c'  ['%c'])\n", circuit[index]->name, logicName(log_val), logicName(circuit[index]->value));

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

	// Justify a BUFFER
	if(circuit[index]->type == BUF)
	{
		if(circuit[index]->inv == TRUE)		// An INVERTER
		{
			BOOLEAN result = FALSE;
			switch(circuit[circuit[index]->in[0]]->value)
			{
				case I: circuit[index]->value == O? result = TRUE : FALSE; break;
				case O: circuit[index]->value == I? result = TRUE : FALSE; break;
				case D: circuit[index]->value == B? result = TRUE : FALSE; break;
				case B: circuit[index]->value == D? result = TRUE : FALSE; break;
				default: result = FALSE;
			}
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
		else	// NORMAL BUFFER 
		{
			if(circuit[circuit[index]->in[0]]->value == circuit[index]->value)
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
	}

	// Logical value Don't-Care (X) does not need justification
	if(log_val == X) return TRUE;



	// Check if the current gate's output cannot be justified from its inputs
	LOGIC_VALUE result = computeGateOutput(circuit, index);
	if(result != log_val)
	{
		circuit[index]->justified[log_val].state = TRUE;
		circuit[index]->justified[log_val].value = FALSE;
		printf("faaaaiiilll: %c  %c\n", logicName(result), logicName(log_val));
		return FALSE;
	}

	// Justify the current gate's inputs
	int inLine = 0;
	for(; inLine < circuit[index]->numIn; inLine++)
	{
		if(justify(circuit, circuit[index]->in[inLine], circuit[index]->value) == FALSE)
		{
			circuit[index]->justified[log_val].state = TRUE;
			circuit[index]->justified[log_val].value = FALSE;
			return FALSE;
		}
	}
	return TRUE;
}

/*
 *  Propagates to the primary output the value given to a circuit line
 *
 *	TODO:
 *		+ Work on the deadly assumption that the gates have two inputs
 *
 * 	ASSUMPTIONS: 
 *		+ The gates have at most two input lines
 *
 *  @param  CIRCUIT 	circuit - the circuit
 *  @param  int 		index  	- the position of the gate in the <circuit>
 *  @param  LOGIC_VALUE	log_val	- the logical value to justify the given gate with
 *  @return BOOLEAN -  TRUE	 if the value can be justified and FALSE if the 
 *					   value causes conflicts
 */
BOOLEAN propagate(CIRCUIT circuit, int index, LOGIC_VALUE log_val)
{
	printf("propagate(%s with '%c')\n", circuit[index]->name, logicName(log_val));

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
			printf("justification: [failed]\n");
			bzero(circuit[index]->propagated, sizeof(PROP_OBJECT));
		}
		else printf("justification: [succeeded]\n");
		circuit[index]->propagated[log_val].state = TRUE;
		circuit[index]->propagated[log_val].value = results;
		return results;
	}

	// Logical value Don't-Care (X) does not need propagation
	if(log_val == X) return TRUE;

	// Propagate through a BUFFER
	if(circuit[index]->type == BUF)
	{
		LOGIC_VALUE prop_log_val;
		if(circuit[index]->inv == TRUE)		// An INVERTER
		{
			switch(circuit[circuit[index]->in[0]]->value)
			{
				case I: prop_log_val = O; break;
				case O: prop_log_val = I; break;
				case D: prop_log_val = B; break;
				case B: prop_log_val = D; break;
				default: prop_log_val = X;;
			}
			circuit[index]->value = prop_log_val;	// Modify the propagated value

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
	}
	circuit[index]->propagated[log_val].state = TRUE;
	circuit[index]->propagated[log_val].value = FALSE;
	return FALSE;
}