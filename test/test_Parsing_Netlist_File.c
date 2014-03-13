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