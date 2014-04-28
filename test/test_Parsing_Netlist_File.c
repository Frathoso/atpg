/*
 * =====================================================================================
 *
 *       Filename:  test_Parsing_Netlist_Files.c
 *
 *    Description:  Unit tests for the "Parsing Netlist" module
 *
 *        Version:  1.0
 *        Created:  09 March 2014
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
#include <stdlib.h>

#include "seatest.h"
#include "parser_netlist.h"
#include "globals.h"

/*
 *	Tests prototypes
 */
void test_create_empty_circuit();
void test_append_a_gate_into_a_circuit();
void test_search_gate_index_in_a_circuit();

void test_populate_circuit_from_an_input_file();


/*
 *	Run all tests
 */
void test_fixture_Parsing_Netlist_File( void )
{
	// Starts a fixture
	test_fixture_start();               

	// Run tests
	run_test(test_create_empty_circuit);
	run_test(test_append_a_gate_into_a_circuit);
	run_test(test_search_gate_index_in_a_circuit);

	run_test(test_populate_circuit_from_an_input_file);

	// Ends a fixture
	test_fixture_end();                 
}

/*		*/
void test_create_empty_circuit()
{
	CIRCUIT circuit;
	assert_true((circuit[0] == NULL));
	assert_int_equal(sizeof(circuit[0]), sizeof(NULL));
}

/*		*/
void test_append_a_gate_into_a_circuit()
{
	CIRCUIT circuit;
	int total = 0;

	appendNewGate(circuit, &total, "joy");

	assert_false((circuit[0] == NULL));
	assert_int_equal(sizeof(*circuit[0]), sizeof(GATE));
	assert_int_equal(total, 1);
	assert_string_equal(circuit[0]->name, "joy");

	// Clean up
	if(circuit[0]) free(circuit[0]);
}

/*		*/
void test_search_gate_index_in_a_circuit()
{
	CIRCUIT circuit;
	int total = 0;

	// Search unavaible gate without adding
	int index = findIndex(circuit, &total, "hey", FALSE);
	assert_int_equal(total, 0);
	assert_int_equal(index, 0);

	// Search unavaible gate with adding
	index = findIndex(circuit, &total, "hey", TRUE);
	assert_int_equal(total, 1);
	assert_int_equal(index, 0);

	// Search an available gate
	index = findIndex(circuit, &total, "hey", TRUE);
	assert_int_equal(total, 1);
	assert_int_equal(index, 0);

	if(circuit[0]) free(circuit[0]);
}

/*		*/
void test_populate_circuit_from_an_input_file()
{ 					
	#define TOTAL_LINES 				9
	#define TOTAL_PRIMARY_INPUT_LINES	4
	#define TOTAL_PRIMARY_OUTPUT_LINES	1

	CIRCUIT circuit;
	CIRCUIT_INFO info;

	char* filename = "benchmarks/test-0.bench";

	if(populateCircuit(circuit, &info, filename) == FALSE)
		printf("ERROR\n");

	// Test for circuit statistics
	assert_int_equal(info.numGates, TOTAL_LINES);
	assert_int_equal(info.numPI, 	TOTAL_PRIMARY_INPUT_LINES);
	assert_int_equal(info.numPO, 	TOTAL_PRIMARY_OUTPUT_LINES);

	// Test for gate types flags
	int index_NOT = findIndex(circuit, &info.numGates, "J", FALSE);
	int index_AND = findIndex(circuit, &info.numGates, "H", FALSE);
	int index_OR  = findIndex(circuit, &info.numGates, "K", FALSE);
	
	assert_true((circuit[index_NOT]->type == BUF && circuit[index_NOT]->inv == TRUE));
	assert_true((circuit[index_AND]->type == AND && circuit[index_AND]->inv == FALSE));
	assert_true((circuit[index_OR]->type  == OR  && circuit[index_OR]->inv == FALSE));

	// Test for primary output and output flags
	int index_PI = findIndex(circuit, &info.numGates, "B", FALSE);
	int index_PO = findIndex(circuit, &info.numGates, "L", FALSE);

	assert_true((circuit[index_PI]->type == PI));
	assert_true((circuit[index_PO]->PO == TRUE));

	// Test for input and output lists to a gate
	#define TOTAL_INPUT_LINES 	2
	int index_IN   = findIndex(circuit, &info.numGates, "H", FALSE);
	int index_IN_1 = findIndex(circuit, &info.numGates, "A", FALSE);
	int index_IN_2 = findIndex(circuit, &info.numGates, "B", FALSE);
	int in_list[MAX_INPUT_GATES] = {index_IN_1, index_IN_2};

	assert_n_array_equal(in_list, circuit[index_IN]->in, TOTAL_INPUT_LINES);

	#define TOTAL_OUTPUT_LINES 	2
	int index_OUT   = findIndex(circuit, &info.numGates, "B", FALSE);
	int index_OUT_1 = findIndex(circuit, &info.numGates, "H", FALSE);
	int index_OUT_2 = findIndex(circuit, &info.numGates, "I", FALSE);
	int out_list[MAX_INPUT_GATES] = {index_IN_1, index_IN_2};

	assert_n_array_equal(out_list, circuit[index_IN]->in, TOTAL_OUTPUT_LINES);

	// Test for gate names flags
	char* gate_names[] = {"A", "B", "C", "E", "L", "J", "I", "H", "K"};
	int K = 0;

	for(; K < TOTAL_LINES; K++)
	{
		assert_string_equal(gate_names[K], circuit[K]->name);
	}

	// Clear memories
	for(K = 0; K < TOTAL_LINES; K++)
		if(circuit[K]) free(circuit[K]);
}
