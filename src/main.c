/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  Driver for the ATPG
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

/* fprintf, stdout, perror */
#include <stdio.h>

/* malloc, free, exit, atexit, NULL */
#include <stdlib.h>

/* strcmp, strcpy, strtok */
#include <string.h>

/* opterr, getopt, optopt, optarg */
#include <unistd.h>

/* errno */
#include <errno.h>

/* isprint */
#include <ctype.h>


#include "libs/parser_netlist.h"
#include "libs/fault_simulation.h"
#include "libs/test_generator.h"
#include "libs/hash.h"
#include "libs/ptime.h"
#include "libs/globals.h"

/*
 *  Command line options list for the program
 */
#define OPTIONS_LIST "dD:f:b:xu:X:s:"

/*
 *  GLOBAL Variables
 */
//extern char ERROR_MESSAGE[MAX_LINE_LENGTH];    // Holds custom error messages

CIRCUIT circuit;        // Graph containing all the gates nodes in the circuit
CIRCUIT_INFO info;      // Graphs metadata object
FAULT_LIST faultList;   // List of all faults in the cictuits

STOP_WATCH stopwatch;   // Stopwatch for measuring exectuting time

/*
 *  Module driving function prototypes
 */
void onProgramTermination( void );
void parse_command_line_arguments( int argc, char* argv[] );
void populate_circuit_from_file( void );
void generate_fault_list( void );
void generate_test_patterns( void );


/*
 *  Program entry point
 *
 *  @param  argc - count of command line arguments
 *  @param  argv - names of command line arguments
 *  @return      - execution exit status
 */
int main( int argc, char* argv[] )
{
    /* Register the program finalizing function */
    atexit(onProgramTermination);

    /* Parse command line arguments */
    parse_command_line_arguments(argc, argv);
    
    /* Populate the circuit and faults list from the passed netlist file */
    populate_circuit_from_file();

    /* Generate fault list */
    generate_fault_list();

    /* Generate test patterns */
    generate_test_patterns();
    
    exit(EXIT_SUCCESS);
}


/*
 *  Alerts the user of a successful execution of the program or the error
 *  causing an unsuccessful termination of the program
 *  
 *  @return nothing
 */
void onProgramTermination()
{
    if(options.isDebugMode == FALSE || options.debugLevel <= 0) return;

    // Display any errors/success message
    switch(errno)
    {
        case 0:
            fprintf(stdout, "Program successfully executed!\n");
            return;
            break;
        case ERROR_PARSING_NETLIST:
            fprintf(stdout, "Error: There was an error parsing the netlist file.\n");
            break;
        case ERROR_IO_LIMIT_EXCEEDED:
        case ERROR_PARSING_CIRCUIT:
            fprintf(stdout, "Error: %s\n", ERROR_MESSAGE);
            break;
        case ERROR_COMMAND_LINE_ARGUMENTS:
            fprintf(stdout, "Usage:\natpg -b <benchmark filename> [-d] [-D <debug level>] \n");
            break;
        default:
            perror("Error");
    }
    fprintf(stdout, "Program was terminated prematurely!\n");

    // Clean up allocated memories
    int K = 0;
    extern HASH_ENTRY hashTableGates[MAX_GATES];
    for(; K < MAX_GATES; K++)
        if(hashTableGates[K].strKey) free(hashTableGates[K].strKey);
    
    for(K = 0; K < info.numGates; K++)
        if(circuit[K]) free(circuit[K]);

    for(K = 0; K < faultList.count; K++) 
        if(faultList.list[K]) free(faultList.list[K]);
}

/*
 *  Parses the passed in command line arguments and configure right program options
 *
 *  @param  argc - count of command line arguments
 *  @param  argv - names of command line arguments
 *  @return nothing
 */
void parse_command_line_arguments(int argc, char* argv[])
{
    errno = opterr = 0;
    int opt;
    while((opt = getopt(argc, argv, OPTIONS_LIST)) != -1)
    {
        switch(opt)
        {
            case 'd':
                options.isDebugMode = TRUE;
                break;
            case 'D':
                options.isDebugMode = TRUE;
                options.debugLevel = atoi(optarg);
                break;
            case 'b':
                options.benchmarkFilename = optarg;
                break;
            case '?':
                if (optopt == 'D')
                    fprintf(stdout, "Option -%c requires the debug level (0, 1, 2).\n", optopt);
                else if (optopt == 'b')
                    fprintf(stdout, "Option -%c requires a filename.\n", optopt);
                else if (isprint(optopt))
                    fprintf(stdout, "Unknown option '-%c'.\n", optopt);
                else
                    fprintf(stdout, "Unknown option character '\\x%x'.\n", optopt);
                errno = ERROR_COMMAND_LINE_ARGUMENTS;
                exit(1);
            default:
                errno = ERROR_COMMAND_LINE_ARGUMENTS;
                exit(1);
        }
    }

    if(options.benchmarkFilename == NULL)
    {
        errno = ERROR_COMMAND_LINE_ARGUMENTS;
        exit(1);
    }
}

/*
 *  Parses the given file and builds a circuit from the file netlist
 *  
 *  @return nothing
 */
void populate_circuit_from_file()
{
    extern HASH_ENTRY hashTableGates[MAX_GATES];
    bzero(hashTableGates, sizeof(hashTableGates));

    if(options.isDebugMode && options.debugLevel > 0) fprintf(stdout, "Parsing: \"%s\"...\n", options.benchmarkFilename);

    startSW(&stopwatch);
    if(populateCircuit(circuit, &info, options.benchmarkFilename))
    {
        computeGateLevels(circuit, &info);

        double duration = getElaspedTimeSW(&stopwatch);
        if(options.isDebugMode && options.debugLevel > 0) fprintf(stdout, "Netlist file successfully parsed "
                "[ %.4f seconds ].\n\n", duration);
    }
    else
    {
        if(errno == 0) errno = ERROR_PARSING_NETLIST;
        exit(1);
    }

    int K; 
    if(options.isDebugMode) 
    {
        fprintf(stdout, "Input gates:\n");
        for(K = 0; K < info.numPI; K++) printf("%s  ", circuit[info.inputs[K]]->name);
        fprintf(stdout, "\nOutput gates:\n");
        for(K = 0; K < info.numPO; K++) printf("%s  ", circuit[info.outputs[K]]->name);
        fprintf(stdout, "\n\n");
    }   
}

/*
 *  Generates fault list for the populate circuit
 *  
 *  @return nothing
 */
void generate_fault_list()
{
	int K, count = 0;
	for(K = 0; K < info.numGates; K++)
	{
		// Add stuck at zero fault
		faultList.list[count] = (FAULT*) malloc(sizeof(FAULT));
		faultList.list[count]->index = K;
		faultList.list[count]->type = ST_0;
		count++;

		// Add stuck at one fault
		faultList.list[count] = (FAULT*) malloc(sizeof(FAULT));
		faultList.list[count]->index = K;
		faultList.list[count]->type = ST_1;
		count++;
	}
	faultList.count = count;
}

/*
 *  Generates test patterns for the populate circuit
 *  
 *  @return nothing
 */
void generate_test_patterns()
{
    startSW(&stopwatch);

    if(options.isDebugMode) fprintf(stdout, "Total Lines: %d\n\n", info.numGates);
    if(options.isDebugMode) fprintf(stdout, "Test Vectors:\nFormat: <Pattern/Input> <Results/Output> <# Faults> {<List of Faults>}\n\n");
    
    BOOLEAN results;
    int K, L;
    TEST_VECTOR testVector;
    for(K = 0; K < faultList.count; K++)
    {
    	if(faultList.list[K] == NULL) continue;

    	clearPropagationValuesCircuit(circuit, info.numGates);

        results = excite(circuit, faultList.list[K]->index, (faultList.list[K]->type == ST_1? B : D));
        if(results == FALSE) continue;

        results = propagate(circuit, faultList.list[K]->index, (faultList.list[K]->type == ST_1? B : D));
        if(results == TRUE)
        {
            extractTestVector(circuit, &info, &testVector);

            // Add the current fault into the patterns fault list
            testVector.faults_list[0] = (FAULT*) malloc(sizeof(FAULT));
            testVector.faults_list[0]->index = faultList.list[K]->index;
            testVector.faults_list[0]->type = faultList.list[K]->type;

            // Simulate other faults in the remaining fault list
        	simulateTestVector(circuit, &info, &faultList, &testVector, K+1);

            // Display results
            displayTestVector(circuit, &testVector);

            // Clear the faults list memory for this pattern
            for(L = 0; L < testVector.faults_count; L++)
                if(testVector.faults_list[L]) free(testVector.faults_list[L]);
        }
    }

    double duration = getElaspedTimeSW(&stopwatch);
        if(options.isDebugMode && options.debugLevel > 0) fprintf(stdout, "\nTest vectors successfully generated "
                "[ %.4f seconds ].\n\n", duration);
}