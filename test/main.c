/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  Driver for the unit testing process
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

#include "seatest.h"


/*
 *	Prototypes of test suits
 */
void test_fixture_Parsing_Netlist_File( void );
void test_fixture_Generate_Test_Pattern( void );


/*
 *	Runs all test suits
 */
void all_test_suits( void )
{
	test_fixture_Parsing_Netlist_File();
	test_fixture_Generate_Test_Pattern();
}

/*
 *	Setup function that runs before every single test in the suite
 */
void my_suite_setup( void )
{

}

/*
 *	Clean up function that runs after every single test in the suite
 */
void my_suite_teardown( void )
{

}

/*
 *	Main function
 */
int main( int argc, char** argv )
{
	// Register suit setup and clean-up functions
	suite_setup(my_suite_setup);
	suite_teardown(my_suite_teardown);

	// Run all test suits
	run_tests(all_test_suits);	

	return 0;
}
