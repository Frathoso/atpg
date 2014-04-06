/*
 * =====================================================================================
 *
 *       Filename:  parser_netlist.h
 *
 *    Description:  Header file for the netlist parser. The file also defines
 *                  the Gate and CircuitInfo structures.
 *
 *        Version:  1.0
 *        Created:  02 December 2013
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

#include "atpg_types.h"


#ifndef CIRCUIT_TRACE_H
#define CIRCUIT_TRACE_H


/*
 *  Allocates memory for the gate at location <totalGates>
 *
 *  @param  CIRCUIT circuit - a circuit to be populated
 *  @param  int*    total   - total number of gates currently in the circuit
 *  @return BOOLEAN - TRUE -> parsing and population were successful, FALSE otherwise
 */
BOOLEAN appendNewGate( CIRCUIT circuit, int* total, char* name );

/*
 *  Reads circuit gates from a netlist stored in <filename> and populates the
 *  <circuit> with the netlist
 *
 *  @param  CIRCUIT circuit  	- an empty circuit to be populated
 *  @param  CIRCUIT_INFO* info 	- summary of circuit details
 *  @param  char*   filename 	- the filename storing the netlist
 *  @return BOOLEAN - TRUE -> parsing and population were successful, FALSE otherwise
 */
BOOLEAN populateCircuit( CIRCUIT circuit, CIRCUIT_INFO* info, char* filename );

/*
 *  Compute gate levels in the entire circuit
 *
 *  @param  circuit - the circuit
 *  @param  info    - gate information object
 *  @return nothing
 */
void computeGateLevels(CIRCUIT circuit, CIRCUIT_INFO* info);

/*
 *  Compute gate level for the given circuit
 *
 *  @param  circuit - the circuit
 *  @param  index - gate index of interest
 *  @return int - the gate's level
 */
int computeGateLevel(CIRCUIT circuit, int index);

/*
 *  Finds the first occurence of the given gate's <name> in the <circuit>
 *
 *  @param  CIRCUIT circuit - the circuit to search
 *  @param  int* totalGates - total number of gates currently in the circuit
 *  @param  char*   name    - gate's name to search for
 *  @param  BOOLEAN init    - flag to initialize a new gate if none was found
 *  @return int - the index of the gate in the <circuit>
 */
int findIndex( CIRCUIT circuit, int* totalGates, char* name, BOOLEAN init );

/*
 * Prints the type name of the gate at position <index>
 *
 *  @param  GATE* gate - the gate to be printed
 *  @param  int     index   - the position of the gate in the <circuit>
 *  @return nothing
 */
void printType( GATE* gate );

/*
 *  Print the details of a gate
 *
 *  @param  CIRCUIT circuit - the circuit
 *  @param  int     index   - the position of the gate in the <circuit>
 *  @return nothing
 */
void printGateInfo( CIRCUIT circuit, int index );

/*
 *  Print the details of a circuit
 *
 *  @param  CIRCUIT circuit - the circuit
 *  @param  int totalGates  - total number of gates currently in the circuit
 *  @return nothing
 */
void printCircuitInfo( CIRCUIT circuit, int totalGates );

#endif
