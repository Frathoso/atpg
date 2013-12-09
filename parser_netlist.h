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

#ifndef CIRCUIT_TRACE_H
#define CIRCUIT_TRACE_H


#define MAX_WORD            32      // Maximum length of names
#define MAX_GATES           50000   // Maximum gates in a circuit
#define MAX_INPUT_GATES     10      // Maximum input lines in a gate
#define MAX_OUTPUT_GATES    500     // Maximum output lines in a gate
#define MAX_LINE_LENGTH     256     // Maximum length of an input file line

#define TRUE                1   
#define FALSE               0
typedef unsigned short BOOLEAN;     // For TRUE or FALSE conditions


/*
 *  Gate abstraction
 */
struct Gate
{
    char    name[MAX_WORD];   // Name given to the gate
    BOOLEAN numIn;      // Count of connected input gates
    BOOLEAN numOut;     // Count of connected output gates
    BOOLEAN in[MAX_INPUT_GATES];     // List of connected input gates
    BOOLEAN out[MAX_OUTPUT_GATES];   // List of connected output gates
    BOOLEAN inv;        // 1 -> the gate is inverted, 0 otherwise
    enum AbSym { AND, OR, PI, BUF, FF, XOR, PPI, OTHER } type;  // Gate type
    BOOLEAN PO;         // 1 -> the gate is a primary output, 0 otherwise
    BOOLEAN PPO;        // 1 -> the gate is a pseudo PO, 0 otherwise
    BOOLEAN flag1;      // Used for tracing
    BOOLEAN flag2;      // Used for IIR
    BOOLEAN flag3;      // Used for OIR
    BOOLEAN flag4;      // Used for exclude
    BOOLEAN flag5;      // Used for tracing
    BOOLEAN flag6;      // Used for IIR
    BOOLEAN flag7;      // Used for OIR
};

typedef struct Gate* CIRCUIT[MAX_GATES];
typedef struct Gate  GATE;


/*
 *  Details for the circuit
 */
struct CircuitInfo
 {
    int numGates;       // Total number of gates
    int numPI;          // Total number of primary inputs
    int numPO;          // Total number of primary outputs
    int numPPI;         // TOtal number of pseudo primary inputs
    int numPPO;         // Total number of pseudo primary outputs
    int numIIR;         // Total number of input interface registers
    int numOIR;         // Total number of output interface registers
    int numINT_2_OIR;   //
 };

 typedef struct CircuitInfo CIRCUIT_INFO;


/*
 *  Allocates memory for the gate at location <totalGates>
 *  
 *  @param  CIRCUIT circuit - an circuit to be populated
 *  @param  int*    total   - total number of gates currently in the circuit
 *  @return BOOLEAN - TRUE -> parsing and population were successful, FALSE otherwise
 */
BOOLEAN appendNewGate( CIRCUIT circuit, int* total, char* name );

/*
 *  Reads circuit gates from a netlist stored in <filename> and populates the
 *  <circuit> with the netlist
 *  
 *  @param  CIRCUIT circuit  - an empty circuit to be populated
 *  @param  CIRCUIT_INFO* info - summary of circuit details
 *  @param  char*   filename - the filename storing the netlist
 *  @return BOOLEAN - TRUE -> parsing and population were successful, FALSE otherwise
 */
BOOLEAN populateCircuit( CIRCUIT circuit, CIRCUIT_INFO* info, char* filename );

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
