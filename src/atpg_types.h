/*
 * =====================================================================================
 *
 *       Filename:  atpg_types.h
 *
 *    Description:  Includes all defined constants
 *
 *        Version:  1.0
 *        Created:  20 February 2013
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

#include "defines.h"


#ifndef ATPG_TYPES_H
#define ATPG_TYPES_H


/* 
 * =====================================================================================
 *									BOOLEAN TYPES
 * =====================================================================================
 */

/*
 *  Boolean type for True and False conditions
 */
typedef enum
{
    FALSE = 0,  // Boolean False
    TRUE  = 1   // Boolean True
} BOOLEAN;


/* 
 * =====================================================================================
 *									LOGIC VALUES TYPES
 * =====================================================================================
 */

/*
 *  Logic values for the five-valued logic
 */
typedef enum{
    O,	// 0 in true and faulty circuits
    I,  // 1 in true and faulty circuits
    D,  // 1 in true circuits and 0 in faulty circuits
    B,  // 0 in true circuits and 1 in faulty circuits
    X   // Unknown value in either true or faulty circuits
} LOGIC_VALUE;


/* 
 * =====================================================================================
 *									GATES TYPES
 * =====================================================================================
 */

/*
 *  Gate types
 */
typedef enum
{
    AND,    // Logical AND Gate
    OR,     // Logical OR Gate
    PI,     // Primary Input
    BUF,    // Buffer
    FF,     //
    XOR,    // Logical XOR Gate
    PPI,    //
    OTHER   // Other gate type
} GATE_TYPE;

/*
 *  Gate abstraction
 */
typedef struct
{
    char    name[MAX_WORD];   // Name given to the gate
    BOOLEAN numIn;      // Number of connected input gates
    BOOLEAN numOut;     // Number of connected output gates
    BOOLEAN in[MAX_INPUT_GATES];     // List of connected input gates
    BOOLEAN out[MAX_OUTPUT_GATES];   // List of connected output gates
    BOOLEAN inv;        // 1 -> the gate is inverted, 0 otherwise
    GATE_TYPE type;     // Gate type
    BOOLEAN PO;         // 1 -> the gate is a primary output, 0 otherwise
    BOOLEAN PPO;        // 1 -> the gate is a pseudo PO, 0 otherwise
    BOOLEAN justified[MAX_LOGIC_VALUES];   // Results of justifying this wire with a logical value
    BOOLEAN propagated[MAX_LOGIC_VALUES];  // Results of propagating this wire with a logical value
    BOOLEAN flag3;      // Used for OIR
    BOOLEAN flag4;      // Used for exclude
    BOOLEAN flag5;      // Used for tracing
    BOOLEAN flag6;      // Used for IIR
    BOOLEAN flag7;      // Used for OIR
} GATE;

typedef GATE* CIRCUIT[MAX_GATES];   // A circuit is a graph of gates


/*
 *  Details for the circuit
 */
typedef struct
{
    int numGates;       // Total number of gates
    int numPI;          // Total number of primary inputs
    int numPO;          // Total number of primary outputs
    int numPPI;         // TOtal number of pseudo primary inputs
    int numPPO;         // Total number of pseudo primary outputs
    int numIIR;         // Total number of input interface registers
    int numOIR;         // Total number of output interface registers
    int numINT_2_OIR;   //
} CIRCUIT_INFO;


/* 
 * =====================================================================================
 *									TEST VECTORS TYPES
 * =====================================================================================
 */

/*
 *  A test vector structure
 */
typedef struct
{
    char input[MAX_INPUT_GATES];    // Input gates values
    char output[MAX_OUTPUT_GATES];   // Output gates values
    int  faults_count;              // Total stuck-at faults the test can detect
} TEST_VECTOR;


#endif
