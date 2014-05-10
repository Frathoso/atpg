/*
 * =====================================================================================
 *
 *       Filename:  logic_tables.c
 *
 *    Description:  Define the boolean five valued logic tables and logic operators
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

#include "logic_tables.h"

#include <stdio.h>


/*
 *  Extracts a logic value from the given gate
 *
 * 	@param  CIRCUIT circuit - the circuit containing the gates
 *  @param  int 	index 	- the target gate
 *  @return LOGIC_VALUE	- the results of the operation
 */
inline LOGIC_VALUE getLogicValue( CIRCUIT circuit, int index, int inPos)
{
	int L;
	if(circuit[circuit[index]->in[inPos]]->numOut > 1)
	{
		//printf("In: ");
		for(L = 0; L < circuit[circuit[index]->in[inPos]]->numOut; L++)
		{
			//printf("%s(%c) ", circuit[circuit[circuit[index]->in[inPos]]->out[L]]->name, logicName(circuit[circuit[circuit[index]->in[inPos]]->out[L]]->value, FALSE));
			if(circuit[circuit[index]->in[inPos]]->out[L] == index) break;
		}
		//printf("\n");
		return circuit[circuit[circuit[index]->in[inPos]]->out[L]]->value;
		//return circuit[circuit[index]->in[inPos]]->values[L];
	}
	else
		return circuit[circuit[index]->in[inPos]]->value;
}

/*
 *  Sets a logic value to the given gate's input line
 *
 * 	@param  CIRCUIT circuit - the circuit containing the gates
 *  @param  int 	index 	- the target gate
 *  @return LOGIC_VALUE	- the results of the operation
 */
inline void setInputLogicValue( CIRCUIT circuit, int index, int inIndex, LOGIC_VALUE log_val)
{
	int L;
	if(circuit[circuit[index]->in[inIndex]]->numOut > 1)
	{
		for(L = 0; L < circuit[circuit[index]->in[inIndex]]->numOut; L++)
			if(circuit[circuit[index]->in[inIndex]]->out[L] == index) break;
		circuit[circuit[index]->in[inIndex]]->values[L] = log_val;
	}
	else
		circuit[circuit[index]->in[inIndex]]->value = log_val;
}

/*
 *  Computes the logic value of the inputs on passing through the gate
 *
 * 	@param  CIRCUIT circuit - the circuit containing the gates
 *  @param  int 	index 	- the target gate
 *  @return LOGIC_VALUE	- the results of the operation
 */
LOGIC_VALUE computeGateOutput( CIRCUIT circuit, int index )
{
	int K;
	LOGIC_VALUE result, temp;
	switch(circuit[index]->type)
	{
		case AND:
			K = 0;
			//printf("\n\tYees (%s): ", circuit[index]->name);
			result = getLogicValue(circuit, index, K);
			while(++K < circuit[index]->numIn)
			{	
				//printf("[%c,", logicName(result, FALSE));
				temp = getLogicValue(circuit, index, K);
				//printf("%c]=", logicName(temp, FALSE));
				result = TABLE_AND[result][temp];
				//printf("%c, ", logicName(result, FALSE));
			}

			if(circuit[index]->inv == TRUE)	// NAND Gate
				return negate(result, TRUE);
			else	// AND Gate
				return result;
		case OR:
			K = 0;
			result = getLogicValue(circuit, index, K);
			while(++K < circuit[index]->numIn)
			{
				temp = getLogicValue(circuit, index, K);
				result = TABLE_OR[result][temp];
			}

			if(circuit[index]->inv == TRUE)	// NOR Gate
				return negate(result, TRUE);
			else	// OR Gate
				return result;

		case BUF:
			return (negate(circuit[circuit[index]->in[0]]->value, circuit[index]->inv));
		default:
			// TODO: Implement XOR and other gate types
			return X;
	}
}

/*
 *  Checks if it is possible to generate the given output by manipulating 
 *  the Don't-Cares (X) input lines
 *
 *	@param  CIRCUIT circuit - the circuit containing the gates
 *  @param  int         index   - the target gate
 * 	@param  LOGIC_VALUE output - the logical value output of interest 
 *  @return BOOLEAN TRUE if it is possible and FALSE otherwise
 */
BOOLEAN isOutputPossible( CIRCUIT circuit, int index, LOGIC_VALUE output )
{
	int K;
	BOOLEAN result = FALSE;

	// Check if there are any Don't-Cares to manipulate and set them to their probable values
	for(K = 0; K < circuit[index]->numIn; K++)
		if(getLogicValue(circuit, index, K) == X)
		{
			// Check for the special cases of AND/NAND and OR/NOR gates
			if(result == FALSE)
			{
				if(circuit[index]->type == AND)
				{
					if(circuit[index]->inv == FALSE && (output == O || output == B))
					{
						setInputLogicValue(circuit, index, K, O);
						return TRUE;
					}
					if(circuit[index]->inv == TRUE && (output == I || output == D))
					{
						setInputLogicValue(circuit, index, K, O);
						return TRUE;
					}
				}
				if(circuit[index]->type == OR)
				{
					if(circuit[index]->inv == FALSE && (output == I || output == D))
					{
						setInputLogicValue(circuit, index, K, I);
						return TRUE;
					}
					if(circuit[index]->inv == FALSE && (output == O || output == B))
					{
						setInputLogicValue(circuit, index, K, I);
						return TRUE;
					}
				}
			}

			// Other cases
			result = TRUE;
			switch(circuit[index]->type)
			{
				case AND: 
					if(circuit[index]->inv == FALSE) setInputLogicValue(circuit, index, K, I);
					else setInputLogicValue(circuit, index, K, O); 
					break;
				case OR:  
					if(circuit[index]->inv == FALSE) setInputLogicValue(circuit, index, K, O);
					else setInputLogicValue(circuit, index, K, I);
					break;
				case BUF: setInputLogicValue(circuit, index, K, negate(output, 
					circuit[circuit[index]->in[K]]->inv)); break;
				default: break;
			}
		}
	if(result == FALSE) return FALSE;
	else
	{
		LOGIC_VALUE jibu = computeGateOutput(circuit, index);
		return (jibu == output);
	}
}

/*
 *  Negates or passes the value as it is depending on the flag <inv>
 *
 * 	@param  LOGIC_VALUE value - the value to be negated
 *  @param  BOOLEAN		inv   - marking if the value has to be negated or not
 *  @return LOGIC_VALUE	- the results of the operation
 */
LOGIC_VALUE negate( LOGIC_VALUE value, BOOLEAN inv )
{
	if(inv == TRUE)
		switch(value)
		{
			case I: return O;
			case O: return I;
			case D: return B;
			case B: return D;
			default: return X;
		}
	else return value;
}

/*
 *  Returns the name of the given logic value
 *
 *  @param  LOGIC_VALUE value - the value to be named
 *  @param  BOOLEAN isForOutput - use (1,0) instead of (I, O)
 *  @return char	- the name of the logic value
 */
char logicName( LOGIC_VALUE value, BOOLEAN isForOutput)
{
	if(isForOutput == FALSE)
		switch(value){
			case O: return 'O';
			case I: return 'I';
			case D: return 'D';
			case B: return 'B';
			case X: return 'x';
			default: return ' ';
		}
	else
		switch(value){
		case I:
		case D: return '1';
		case O:
		case B: return '0';
		case X: return 'x';
		default: return ' ';
	}
}
