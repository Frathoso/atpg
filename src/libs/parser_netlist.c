/*
 * =====================================================================================
 *
 *       Filename:  parser_netlist.c
 *
 *    Description:  Netlist parser for the ATPG program. This parses a netlist
 *                  file and populates a circuit with the data. It defines the
 *                  Gate, Circuit and CircuitInfo data structures and all the
 *                  helper functions.
 *
 *        Version:  1.0
 *        Created:  07 December 2013
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

/* FILE, sprintf, fprintf, fscanf, fopen, fgets, fclose, stderr, stdin, stdout */
#include <stdio.h>

/* malloc, free, exit, abort, atexit, NULL */
#include <stdlib.h>

/* strcmp, strcpy, strtok */
#include <string.h>

/* errno */
#include <errno.h>

#include "parser_netlist.h"
#include "atpg_types.h"
#include "hash.h"


/*
 *  Allocates memory for the gate at location <totalGates>
 *
 *  @param  CIRCUIT circuit - a circuit to be populated
 *  @param  int*    total   - total number of gates currently in the circuit
 *  @return BOOLEAN - TRUE -> parsing and population were successful, FALSE otherwise
 */
BOOLEAN appendNewGate( CIRCUIT circuit, int* total, char* name )
{
    circuit[*total] = (GATE*) malloc(sizeof(GATE));
    strcpy(circuit[*total]->name, name);
    circuit[*total]->numIn  = 0;
    circuit[*total]->numOut = 0;
    circuit[*total]->PO     = 0;
    circuit[*total]->PPO    = 0;
    circuit[*total]->type   = OTHER;
    circuit[*total]->value  = X;
    circuit[*total]->level  = -1;
    bzero(circuit[*total]->justified, sizeof(JUST_OBJECT));
    bzero(circuit[*total]->propagated, sizeof(PROP_OBJECT));
    (*total)++;

    // Store the gate name and index in a hash table
    extern HASH_ENTRY hashTableGates[MAX_GATES];
    int K = hashStringToInt(name, MAX_GATES);

    // TODO drop the assumption that the hash table is not full
    while(hashTableGates[K].strKey != NULL) K = (K + 1) % MAX_GATES;

    hashTableGates[K].strKey = (char*) malloc(sizeof(name) + 1);
    strcpy(hashTableGates[K].strKey, name);
    hashTableGates[K].intKey = (*total) - 1;

    return TRUE;
}

/*
 *  Reads circuit gates from a netlist stored in <filename> and populates the
 *  <circuit> with the netlist
 *
 *  @param  CIRCUIT circuit     - an empty circuit to be populated
 *  @param  CIRCUIT_INFO* info  - summary of circuit details
 *  @param  char*   filename    - the filename storing the netlist
 *  @return BOOLEAN - TRUE -> parsing and population were successful, FALSE otherwise
 */
BOOLEAN populateCircuit( CIRCUIT circuit, CIRCUIT_INFO* info, char* filename )
{
    extern char ERROR_MESSAGE[MAX_LINE_LENGTH];
    extern volatile COMMAND_LINE_OPTIONS options;

    FILE* fp = fopen(filename, "r");
    if(fp == NULL) return FALSE;

    char line[MAX_LINE_LENGTH];
    char* tempBuffer   = NULL;
    char* tempName     = NULL;
    BOOLEAN outputSeen = FALSE; // True if at least one output gate has been parsed
    BOOLEAN isPseudo   = FALSE;
    int index, index2, dff;
    char* in;

    while(fgets(line, MAX_LINE_LENGTH, fp) != NULL)
    {
        if(strstr(line, "#")) continue;     // Comment line
        else if(strstr(line, "INPUT"))      // Input gate
        {
            // Extract input gate's name
            tempBuffer = strstr(line, "(");
            tempBuffer++;
            *strstr(tempBuffer, ")") = 0;

            // Check if PPI is the same as one of the existing POs
            BOOLEAN found = FALSE;
            index = findIndex(circuit, &(info->numGates), tempBuffer, FALSE);

            // Check if the gate is a pseudo gate
            if(outputSeen && (index < info->numGates) && circuit[index]->PO)
            {
                circuit[index]->PO = FALSE;
                found = TRUE;
            }

            // Append a new input gate and initialize its details
            index = info->numGates;
            appendNewGate(circuit, &(info->numGates), tempBuffer);

            circuit[index]->level = 0;

            if(isPseudo)
            {
                circuit[index]->type = PPI;
                info->numPPI++;
            }
            else
            {
                circuit[index]->type = PI;
                info->inputs[info->numPI] = index;
                info->numPI++;
            }

            if (found) circuit[index]->PO = 1;
            circuit[index]->PPO    = 0;

            if(options.isDebugMode && options.debugLevel > 2) printGateInfo(circuit, index);
        }
        else if(strstr(line, "OUTPUT"))     // Output gate
        {
            // Indicate an output gate has been seen
            outputSeen = 1;

            // Extract output gate's name
            tempBuffer = strstr(line, "(" );
            tempBuffer++;
            *strstr(tempBuffer, ")") = 0;

            // Append new output gate and initialize its details
            index = info->numGates;
            appendNewGate(circuit, &(info->numGates), tempBuffer);

            if(isPseudo)
            {
                circuit[index]->PPO = 1;
                info->numPPO++;
                circuit[index]->PO = 0;
            }
            else
            {
                circuit[index]->PO = 1;
                info->outputs[info->numPO] = index;
                info->numPO++;
                circuit[index]->PPO = 0;
            }

            if(options.isDebugMode && options.debugLevel > 2) printGateInfo(circuit, index);
        }
        else if(strstr(line, "="))  // Inner gate
        {
            // Extract gate's name
            tempBuffer = (char *) malloc(sizeof(char)*MAX_LINE_LENGTH);
            tempName   = (char *) malloc(sizeof(char)*MAX_LINE_LENGTH);

            strcpy(tempBuffer, line);
            *strstr(tempBuffer, " ") = 0;

            strcpy(tempName, tempBuffer);
            index = findIndex(circuit, &(info->numGates), tempBuffer , TRUE);


            // Check if gate is valid
            if ((index < info->numGates) && ((circuit[index]->type == PI) ||
                (circuit[index]->type == PPI)))
            {
                sprintf(ERROR_MESSAGE,
                        "Node %s (id:%d), which is a PI/PPI, is at the output"
                        " of a gate!", circuit[index]->name, index);
                errno = ERROR_PARSING_CIRCUIT;
                exit(1);
            }

            // Determine gate's type
            strcpy(tempBuffer, line);
            tempBuffer = strstr(tempBuffer, "=");
            tempBuffer += 2;
            *strstr(tempBuffer, "(") = 0;

            if( *tempBuffer == 'N' )
            {
                circuit[index]->inv = 1;
                tempBuffer++;
            }
            else circuit[index]->inv = 0;

            dff = 0;
            if((strcmp(tempBuffer, "OT") == 0) || (strcmp(tempBuffer, "BUF") == 0))
            {
                if ((circuit[index]->PO != 1) ||
                    (circuit[index]->type != OTHER)) circuit[index]->PO = 0;
                if ((circuit[index]->PPO != 1) ||
                    (circuit[index]->type != OTHER)) circuit[index]->PPO = 0;
                circuit[index]->type = BUF;
            }
            else if(strcmp(tempBuffer, "AND") == 0)
            {
                if ((circuit[index]->PO != 1) ||
                    (circuit[index]->type != OTHER)) circuit[index]->PO = 0;
                if ((circuit[index]->PPO != 1) ||
                    (circuit[index]->type != OTHER)) circuit[index]->PPO = 0;
                circuit[index]->type = AND;
            }
            else if(strcmp(tempBuffer, "OR") == 0)
            {
                if ((circuit[index]->PO != 1) ||
                    (circuit[index]->type != OTHER)) circuit[index]->PO = 0;
                if ((circuit[index]->PPO != 1) ||
                    (circuit[index]->type != OTHER)) circuit[index]->PPO = 0;
                circuit[index]->type = OR;
            }
            else if(strcmp(tempBuffer, "PI") == 0)
            {
                if ((circuit[index]->PO != 1) ||
                    (circuit[index]->type != OTHER)) circuit[index]->PO = 0;
                if ((circuit[index]->PPO != 1) ||
                    (circuit[index]->type != OTHER)) circuit[index]->PPO = 0;
                circuit[index]->type = PI;
            }
            else if(strcmp(tempBuffer, "DFF") == 0)
            {
                if ((circuit[index]->PO != 1) ||
                    (circuit[index]->type != OTHER)) circuit[index]->PO = 0;
                if ((circuit[index]->PPO != 1) ||
                    (circuit[index]->type != OTHER)) circuit[index]->PPO = 0;
                if (index < info->numGates-1)
                // Found a PO with the same name ===> need to add new PPI
                {
                    circuit[index]->PO = 0;
                    appendNewGate(circuit, &(info->numGates), tempName);
                    circuit[info->numGates]->type = PPI;
                    info->numPPI++;
                    circuit[info->numGates]->PO = TRUE;

                    dff = 1;
                }
                else
                {
                    circuit[index]->type = PPI;
                    info->numPPI++;
                    dff = 1;
                }
            }
            else if(strcmp(tempBuffer, "XOR") == 0)
            {
                if ((circuit[index]->PO != 1) ||
                    (circuit[index]->type != OTHER)) circuit[index]->PO = 0;
                if ((circuit[index]->PPO != 1) ||
                    (circuit[index]->type != OTHER)) circuit[index]->PPO = 0;
                circuit[index]->type = XOR;
            }
            else if(strcmp(tempBuffer, "XNOR") == 0)
            {
                if ((circuit[index]->PO != 1) ||
                    (circuit[index]->type != OTHER)) circuit[index]->PO = 0;
                if ((circuit[index]->PPO != 1) ||
                    (circuit[index]->type != OTHER)) circuit[index]->PPO = 0;
                circuit[index]->type = XOR;
                circuit[index]->inv = 1;
            }
            else if(strcmp(tempBuffer, "PPI") == 0)
            {
                if ((circuit[index]->PO != 1) ||
                    (circuit[index]->type != OTHER)) circuit[index]->PO = 0;
                if ((circuit[index]->PPO != 1) ||
                    (circuit[index]->type != OTHER)) circuit[index]->PPO = 0;
                circuit[index]->type = PPI;
            }
            else
            {
                sprintf(ERROR_MESSAGE,
                        "Unknown gate type \"%s\" encountered!", tempBuffer);
                errno = ERROR_PARSING_CIRCUIT;
                exit(1);
            }

            // Get all inputs and set output of those nodes
            tempBuffer = strstr(line, "(");
            tempBuffer++;
            *strstr(tempBuffer, ")") = 0;


            while((in = strtok_r(tempBuffer, ",", &tempBuffer)))
            {
                if(*in == ' ') in++;
                index2 = findIndex(circuit, &(info->numGates), in , TRUE);

                // Error check
                if ((index2 < info->numGates) && ((circuit[index2]->PO) ||
                    (circuit[index2]->PPO)))
                {
                    sprintf(ERROR_MESSAGE,
                        "Node %s (id:%d), which is a PO/PPO, is feeding gate"
                        " %s (id:%d)",
                        circuit[index2]->name, index2, circuit[index]->name, index);
                    errno = ERROR_PARSING_CIRCUIT;
                    exit(1);
                }

                // Update input of created node
                if (!dff)
                {
                    circuit[index]->in[circuit[index]->numIn] = index2;
                    (circuit[index]->numIn)++;
                }

                if (circuit[index]->numIn > MAX_INPUT_GATES)
                {
                    sprintf(ERROR_MESSAGE, "Too many inputs to gate %s",
                        circuit[index]->name);
                    errno = ERROR_IO_LIMIT_EXCEEDED;
                    exit(1);
                }

                // Update output of listed node
                if (!dff)
                {
                    circuit[index2]->out[circuit[index2]->numOut] = index;
                    (circuit[index2]->numOut)++;
                }
                else
                {
                    circuit[index2]->PPO = TRUE;
                    info->numPPO++;
                }

                if (circuit[index2]->numOut > MAX_OUTPUT_GATES)
                {
                    sprintf(ERROR_MESSAGE, "Too many outputs from gate %s",
                        circuit[index]->name);
                    errno = ERROR_IO_LIMIT_EXCEEDED;
                    exit(1);
                }
            }
            if(options.isDebugMode && options.debugLevel > 2) printGateInfo(circuit, index);
        }
    }
    // free(tempBuffer);
    // free(tempName);
    fclose(fp);

    return TRUE;
}

/*
 *  Compute gate levels in the entire circuit
 *
 *  @param  circuit - the circuit
 *  @param  info    - gate information object
 *  @return nothing
 */
void computeGateLevels(CIRCUIT circuit, CIRCUIT_INFO* info)
{
    int K, level;
    for(K = 0; K < info->numGates; K++)
    {
        // Skipping PI and already computed gates
        if(circuit[K]->level < 0)
        {
            level = computeGateLevel(circuit, K);
            circuit[K]->level = level;
        }
    }
}

/*
 *  Compute gate level for the given circuit
 *
 *  @param  circuit - the circuit
 *  @param  index - gate index of interest
 *  @return int - the gate's level
 */
int computeGateLevel(CIRCUIT circuit, int index)
{
    if(circuit[index]->level >= 0) return circuit[index]->level;
    else
    {
        int K, temp, level = -1;
        for(K = 0; K < circuit[index]->numIn; K++)
        {
            temp = computeGateLevel(circuit, circuit[index]->in[K]);
            if(temp > level) level = temp;
        }
        return (level+1);
    }
}

/*
 *  Finds the first occurence of the given gate's <name> in the <circuit>
 *
 *  @param  CIRCUIT circuit - the circuit to search
 *  @param  int* totalGates - total number of gates currently in the circuit
 *  @param  char*   name    - gate's name to search for
 *  @param  BOOLEAN init    - flag to initialize a new gate if none was found
 *  @return int - the index of the gate in the <circuit>
 */
int findIndex( CIRCUIT circuit, int* totalGates, char* name, BOOLEAN init )
{
    extern HASH_ENTRY hashTableGates[MAX_GATES];
    int K = hashStringToInt(name, MAX_GATES);

    while(TRUE)
    {
        if(hashTableGates[K].strKey == NULL)
            break;
        else if(strcmp(hashTableGates[K].strKey, name) == 0)
            return hashTableGates[K].intKey;

        K = (K+1) % MAX_GATES;
    }

    if(init == FALSE) return (*totalGates);

    appendNewGate(circuit, totalGates, name);
    return ((*totalGates) - 1);
}

/*
 * Prints the type name of the gate at position <index>
 *
 *  @param  GATE* gate - the gate to be printed
 *  @param  int     index   - the position of the gate in the <circuit>
 *  @return nothing
 */
void printType( GATE* gate )
{
    switch(gate->type)
    {
        case AND:   fprintf(stdout, "AND"); break;
        case OR:    fprintf(stdout, "OR"); break;
        case PI:    fprintf(stdout, "PI"); break;
        case PPI:   fprintf(stdout, "PPI"); break;
        case BUF:   fprintf(stdout, "BUF"); break;
        case FF:    fprintf(stdout, "FF"); break;
        case XOR:   fprintf(stdout, "XOR"); break;
        case OTHER: fprintf(stdout, "OTHER"); break;
        default:    fprintf(stdout, "Unknown");
    }
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
    fprintf(stdout, "Index: %d\t", index);
    fprintf(stdout, "Name: %s\t", circuit[index]->name);
    fprintf(stdout, "Type: ");
    printType(circuit[index]);
    fprintf(stdout, "\t");
    fprintf(stdout, "Inv: %d\t", circuit[index]->inv);

    fprintf(stdout, "\nNumIn: %d\t", circuit[index]->numIn);
    int i;
    fprintf(stdout, "{");
    for(i = 0; i<(circuit[index]->numIn); i++)
        fprintf(stdout, " %s,",circuit[circuit[index]->in[i]]->name);
    fprintf(stdout, "}");

    fprintf(stdout, "\nNumOut: %d\t", circuit[index]->numOut);
    fprintf(stdout, "{");
    for(i = 0; i<(circuit[index]->numOut); i++)
        fprintf(stdout, " %s,",circuit[circuit[index]->out[i]]->name);
    fprintf(stdout, "}");

    if(circuit[index]->PO)  fprintf(stdout, "\nThis gate is a PO also");
    if(circuit[index]->PPO) fprintf(stdout, "\nThis gate is a PPO also");

    // For debugging Justification and Propagation
    char logValName;
	switch(circuit[index]->value){
		case O: logValName = 'O'; break;
		case I: logValName = 'I'; break;
		case D: logValName = 'D'; break;
		case B: logValName = 'B'; break;
		case X: logValName = 'X'; break;
	};
	fprintf(stdout, "\nValue: '%c'", logValName);
    fprintf(stdout, "\nJustified:  ");
    fprintf(stdout, "{");
    for(i = 0; i<MAX_LOGIC_VALUES; i++)
        fprintf(stdout, " %d:%d,",circuit[index]->justified[i].state, 
        		circuit[index]->justified[i].value);
    fprintf(stdout, "}");

    fprintf(stdout, "\nPropagated: ");
    fprintf(stdout, "{");
    for(i = 0; i<MAX_LOGIC_VALUES; i++)
        fprintf(stdout, " %d:%d,",circuit[index]->propagated[i].state, 
        		circuit[index]->propagated[i].value);
    fprintf(stdout, "}");

    fprintf(stdout, "\n\n");
}

/*
 *  Print the details of a circuit
 *
 *  @param  CIRCUIT circuit - the circuit
 *  @param  int totalGates  - total number of gates currently in the circuit
 *  @return nothing
 */
void printCircuitInfo( CIRCUIT circuit, int totalGates )
{
    int index = 0;
    for( ; index < totalGates; index++)
    {
        printGateInfo(circuit, index);
    }
}
