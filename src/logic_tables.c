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

#include "stdio.h"
#include "logic_tables.h"


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
			result = circuit[circuit[index]->in[K]]->value;
			while(++K < circuit[index]->numIn)
			{	
				temp = circuit[circuit[index]->in[K]]->value;
				result = TABLE_AND[result][temp];
			}

			if(circuit[index]->inv == TRUE)	// NAND Gate
				return negate(result, TRUE);
			else	// AND Gate
				return result;
		case OR:
			K = 0;
			result = circuit[circuit[index]->in[K]]->value;
			while(++K < circuit[index]->numIn)
			{
				temp = circuit[circuit[index]->in[K]]->value;
				printf("OR(%c, %c) = ", logicName(result), logicName(temp));
				result = TABLE_OR[result][temp];
				printf("%c\n",  logicName(result));
			}

			if(circuit[index]->inv == TRUE)	// NOR Gate
				return negate(result, TRUE);
			else	// OR Gate
				return result;
		default:
			// TODO: Implement XOR and other gate types
			return X;
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
 * 	@param  LOGIC_VALUE value - the value to be named
 *  @return char	- the name of the logic value
 */
char logicName( LOGIC_VALUE value)
{
	switch(value){
		case O: return 'O';
		case I: return 'I';
		case D: return 'D';
		case B: return 'B';
		case X: return 'X';
		default: return ' ';
	}
}