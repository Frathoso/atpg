/*
 * =====================================================================================
 *
 *       Filename:  parser_netlist.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07 December 2013
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  
 *   Organization:  New York University Abu Dhabi 
 *
 * =====================================================================================
 */

/* FILE, fprintf, fscanf, fopen, fclose, stderr, stdin, stdout */
#include <stdio.h> 

/* malloc, free, exit, abort, atexit, NULL */     
#include <stdlib.h>   

/* strcmp, strcpy, strtok */  
#include <string.h>    

#include "parser_netlist.h"


/*
 *  Allocates memory for the gate at location <totalGates>
 *  
 *  @param  CIRCUIT circuit - an circuit to be populated
 *	@param	int* 	total  	- total number of gates currently in the circuit
 *  @return BOOLEAN - TRUE -> parsing and population were successful, FALSE OTHERwise
 */
BOOLEAN appendNewGate( CIRCUIT circuit, int* total, char* name )
{
    // TODO implement
    return FALSE;
}

/*
 *  Reads circuit gates from a netlist stored in <filename> and populates the
 *  <circuit> with the netlist
 *  
 *  @param  CIRCUIT circuit  - an empty circuit to be populated
 *	@param	CIRCUIT_INFO* info - summary of circuit details
 *  @param  char*   filename - the filename storing the netlist
 *  @return BOOLEAN - TRUE -> parsing and population were successful, FALSE OTHERwise
 */
BOOLEAN populateCircuit( CIRCUIT circuit, CIRCUIT_INFO* info, char* filename )
{
    // TODO implement
	appendNewGate(circuit, &(info->numGates), "AND");

    FILE* fp = fopen(filename, "r");

    if(fp == NULL)
    {
        return FALSE;
    }

    return TRUE;
}

/*
 *  Finds the first occurence of the given gate's <name> in the <circuit>
 *
 *  @param  CIRCUIT circuit - the circuit to search
 *	@param	int* totalGates - total number of gates currently in the circuit
 *  @param  char*   name    - gate's name to search for
 *  @param  BOOLEAN init    - flag to initialize a new gate if none was found
 *  @return int - the index of the gate in the <circuit>
 */
int findIndex( CIRCUIT circuit, int* totalGates, char* name, BOOLEAN init )
{
    // TODO implement
    return 0;
}

/*
 * Prints the type name of the gate at position <index>
 *
 *  @param  CIRCUIT circuit - the circuit
 *  @param  int     index   - the position of the gate in the <circuit>
 *  @return nothing
 */
void printType( CIRCUIT circuit, int index )
{
	// TODO implement
    return;
}

/*
 *  Print the details of a gate
 *
 *  @param  CIRCUIT circuit - the circuit
 *  @param  int     index   - the position of the gate in the <circuit>
 *  @return nothing
 */
void printGateInfo( CIRCUIT circuit, int index )
{
	// TODO implement
    return;
}

/*
 *  Print the details of a circuit
 *
 *  @param  CIRCUIT circuit - the circuit
 *	@param	int totalGates  - total number of gates currently in the circuit
 *  @return nothing
 */
void printCircuitInfo( CIRCUIT circuit, int totalGates )
{
	// TODO implement
    return;
}

