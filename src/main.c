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

/* getopt_long, required_argument, no_argument, option */
#include <getopt.h>

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
void parse_fault_from_file( char* );
void generate_test_patterns( void );
void save_undetected_faults( void );
void display_statistics( void );


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
    if(options.isFaultListGiven == TRUE)
        parse_fault_from_file(options.faultListFilename);
    else
        generate_fault_list();

    /* Generate test patterns */
    generate_test_patterns();

    /* Save undetected faults if needed */
    if(options.isPrintUndetectedFaults == TRUE)
        save_undetected_faults();

    /* Display statistics */
    display_statistics();
    
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
    // Display any errors/success message
    switch(errno)
    {
        case 0:
            fprintf(stdout, "\nProgram successfully executed!\n");
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
            fprintf(stdout, "\nUsage:\n%s -b <benchmark filename> \t[-d] [-D <debug level>]\n"
                "\t[-f <fault list filename>] [-h] [--help] [--version]\n\t[-s <test pattern filename]"
                "[-u <undetected faults results filename>]\n\t[-x] [-X <don't cares filling option>]\n",
                GLOBAL_NAME);
            fprintf(stdout, "\nFor detailed help run:\n%s --help\n\n", GLOBAL_NAME);
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
 *  Displays the program's version details
 *
 *  @return nothing
 */
void displayVersionDetails()
{
    printf("%s version %s\n", GLOBAL_NAME, VERSION);

    exit(0);
}

/*
 *  Displays the program execution command line arguments
 *
 *  @return nothing
 */
void displayHelpDetails()
{
    fprintf(stdout, "\n");
    fprintf(stdout, "================================================================================\n");
    fprintf(stdout, "*                     (C) New York University Abu Dhabi, 2014                  *\n");
    fprintf(stdout, "*                                                                              *\n");
    fprintf(stdout, "*                     Automatic Test Pattern Generation System                 *\n");
    fprintf(stdout, "*                                                                              *\n");
    fprintf(stdout, "================================================================================\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "\nNAME\n\t%s - automatic test pattern generation system\n", GLOBAL_NAME);
    fprintf(stdout, "\nSYNOPSIS\n\t%s  -b <benchmark filename> \t[-d] [-D <debug level>] \n", GLOBAL_NAME);
    fprintf(stdout, "\t      [-f <fault list filename>] [-h] [-s <test pattern filename]\n");
    fprintf(stdout, "\t      [-u <undetected faults results filename>] [-x]\n");
    fprintf(stdout, "\t      [-X <don't cares filling option>]  [--help] [--version] \n");
    fprintf(stdout, "\nDESCRIPTION\n");
    fprintf(stdout, "\tTODO: Add the tool's description here\n");
    fprintf(stdout, "\nOPTIONS\n");
    fprintf(stdout, "\n\t-b\n\t    Specify the filename to parse the circuit netlist from\n");
    fprintf(stdout, "\n\t-d\n\t    Print debugging information with the default debugging level 0\n");
    fprintf(stdout, "\n\t-D\n\t    Print debugging information with the supplied debugging level\n");
    fprintf(stdout, "\n\t-f\n\t    Specify the filename for the faults list of interest\n");
    fprintf(stdout, "\n\t-h --help\n\t    Display the detailed help information\n");
    fprintf(stdout, "\n\t-s\n\t    Specify the filename to parse test patterns to simulate fault \n"
                          "\t    collapsing with\n");
    fprintf(stdout, "\n\t-u\n\t    Specify the filename to save undetected faults into\n");
    fprintf(stdout, "\n\t--version\n\t    Display the tools current version number\n");
    fprintf(stdout, "\n\t-x\n\t    Turn ON/OFF fault collapsing\n");
    fprintf(stdout, "\n\t-X\n\t    Option for filling in the don't cares with during fault simulation\n");
    fprintf(stdout, "\n\n");

    exit(0);
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
    // Initialize options structure
    options.benchmarkFilename = NULL;
    options.dontCareFilling = RANDOM;
    options.outputTestPatternFilename = NULL;
    options.isOneTestPerFault = FALSE;

    // Command line options list for the program
    char* SHORT_OPTS = "b:dD:f:hs:t:u:xX:";
    static struct option LONG_OPTS[] = {
        {"help",    no_argument, 0,  0},
        {"version", no_argument, 0,  0},
        {0,         0,           0,  0}
    };


    errno = opterr = 0;
    int opt, long_opt_index;
    while((opt = getopt_long(argc, argv, SHORT_OPTS, LONG_OPTS, &long_opt_index)) != -1)
    {
        switch(opt)
        {
            case 0:     // Parse long option
                if(strcmp("help", LONG_OPTS[long_opt_index].name) == 0) 
                    displayHelpDetails();
                else if(strcmp("version", LONG_OPTS[long_opt_index].name) == 0)
                    displayVersionDetails();
                break;
            case 'b':   // Define the benchmark filename
                options.isBenchmarkFileGiven = TRUE;
                options.benchmarkFilename = optarg;
                break;
            case 'd':   // Turn ON the display of debug details with default level
                options.isDebugMode = TRUE;
                options.debugLevel = 0;     // Default debug level is zero
                break;
            case 'D':   // Turn ON the display of debug details with the given level
                options.isDebugMode = TRUE;
                options.debugLevel = atoi(optarg);
                break;
            case 'f':   // Define the fault list filename
                options.isFaultListGiven = TRUE;
                options.faultListFilename = optarg;
                break;
            case 'h':   // Display help
                displayHelpDetails();
                break;
            case 's':   // Define the custom input test patterns filename
                options.isCustomFaultSimulation = TRUE;
                options.inputTestPatternFilename = optarg;
                break;
           	case 't':   // Define the custom output test patterns filename
            	options.outputTestPatternFilename = optarg;
            	break;
            case 'u':   // Define the undetected faults results filename
                options.isPrintUndetectedFaults = TRUE;
                options.undetectedFaultsFilename = optarg;
                break;
            case 'x':   // Turn ON one-test-per-fault feature
                options.isOneTestPerFault = TRUE;
                break;
            case 'X':   // Define the don't-cares filling option during fault simulation
                if(strcmp(optarg, "1") == 0) 
                    options.dontCareFilling = ONES;
                else if(strcmp(optarg, "0") == 0) 
                    options.dontCareFilling = ZEROS;
                else if(strcmp(optarg, "R") == 0 || strcmp(optarg, "r") == 0) 
                    options.dontCareFilling = RANDOM;
                else
                {
                    fprintf(stdout, "Option -X requires the Don't-Care Filling option [1, 0, R].\n");
                    errno = ERROR_COMMAND_LINE_ARGUMENTS;
                    exit(1);
                }
                break;
            case '?':   // Parse unknown command line argument
                if (optopt == 'D')
                    fprintf(stdout, "Error: Option -%c requires the debug level (0, 1, 2).\n", optopt);
                else if (optopt == 'b' || optopt == 'f' || optopt == 's' || optopt == 'u')
                    fprintf(stdout, "Error: Option -%c requires a filename.\n", optopt);
                else if (isprint(optopt))
                    fprintf(stdout, "Error: Unknown option '-%c'.\n", optopt);
                else 
                    fprintf(stdout, "Error: Unknown option.\n");
                errno = ERROR_COMMAND_LINE_ARGUMENTS;
                exit(1);
            default:
                errno = ERROR_COMMAND_LINE_ARGUMENTS;
                exit(1);
        }
    }

    // Confirm the required options have been supplied
    if(options.isBenchmarkFileGiven == FALSE)
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
		faultList.list[count]->detected = FALSE;
		count++;

		// Add stuck at one fault
		faultList.list[count] = (FAULT*) malloc(sizeof(FAULT));
		faultList.list[count]->index = K;
		faultList.list[count]->type = ST_1;
		faultList.list[count]->detected = FALSE;
		count++;
	}
	faultList.count = count;
}

/*
 *  Parse fault list from a file
 *  
 *  @return nothing
 */
void parse_fault_from_file(char* filename)
{
    // TODO: Implement function
}

/*
 *  Generates test patterns for the populate circuit
 *  
 *  @return nothing
 */
void generate_test_patterns()
{
	// Prepare file to save test patterns into
	char filename[MAX_WORD];
	if(options.outputTestPatternFilename != NULL)
    	strcpy(filename, options.outputTestPatternFilename);
    else
    	strcpy(filename, "test_patterns");
    strcat(filename, ".tvl");
    FILE* fp = fopen(filename, "w");

    startSW(&stopwatch);

    if(options.isDebugMode) 
    	fprintf(stdout, "Total Lines: %d\n\n", info.numGates);
    if(options.isDebugMode && options.debugLevel > 0) 
    	fprintf(stdout, "Test Vectors:\nFormat: <Pattern/Input> <Results/Output> <# Faults> {<List of Faults>}\n\n");
    fprintf(fp, "Test Vectors:\nFormat: <Pattern/Input> <Results/Output> <# Faults> {<List of Faults>}\n\n");
    
    BOOLEAN results;
    int K, L;
    TEST_VECTOR testVector;
    for(K = 0; K < faultList.count; K++)
    {
    	if(faultList.list[K]->detected == TRUE) continue;

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

            // Simulate other faults in the remaining fault list if fault collapsing is allowed
            if(options.isOneTestPerFault == FALSE)
        		simulateTestVector(circuit, &info, &faultList, &testVector, K+1);

            // Display and save results
            if(options.isDebugMode && options.debugLevel > 0) displayTestVector(circuit, &testVector);
            saveTestVector(circuit, &testVector, fp);

            // Clear the faults list memory for this pattern
            for(L = 0; L < testVector.faults_count; L++)
                if(testVector.faults_list[L]) free(testVector.faults_list[L]);

            // Mark the fault as detected
            faultList.list[K]->detected = TRUE;
        }
    }

    double duration = getElaspedTimeSW(&stopwatch);
        if(options.isDebugMode && options.debugLevel > 0) fprintf(stdout, "\nTest vectors successfully generated "
                "[ %.4f seconds ].\n\n", duration);
}

/*
 *  Save undected faults into a file
 *  
 *  @return nothing
 */
void save_undetected_faults()
{
    char filename[MAX_WORD];
    strcpy(filename, options.undetectedFaultsFilename);
    strcat(filename, FAULT_FILE_EXTENSION);
    FILE* fp = fopen(filename, "w");
    int K;
    for(K = 0; K < faultList.count; K++)
        if(faultList.list[K]->detected == FALSE)
            fprintf(fp, "%s /%c\n", circuit[faultList.list[K]->index]->name, (faultList.list[K]->type == ST_1 ? '1':'0'));

    fclose(fp);
}

/*
 *  Compute and display execution statistics
 *  
 *  @return nothing
 */
void display_statistics( )
{
	fprintf(stdout, "\nTotal faults:\t%d\n", faultList.count);

	int undetected_faults = 0, K;
    for(K = 0; K < faultList.count; K++)
        if(faultList.list[K]->detected == FALSE) undetected_faults++;

	fprintf(stdout, "\nDetected faults:\n\tCount:\t\t%d\n", (faultList.count-undetected_faults));
	fprintf(stdout, "\tPercentage:\t%.0f%%\n", ((float) (faultList.count-undetected_faults)*100/faultList.count));

    fprintf(stdout, "\nUndetected faults:\n\tTotal count:\t%d\n", undetected_faults);
    fprintf(stdout, "\tPercentage:\t\t%.0f%%\n", ((float) (undetected_faults)*100/faultList.count));
    fprintf(stdout, "\tOutput file:\t\"%s%s\"\n", options.undetectedFaultsFilename, 
    				FAULT_FILE_EXTENSION);

}