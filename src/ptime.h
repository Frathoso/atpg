/*
 * =====================================================================================
 *
 *       Filename:  ptime.h
 *
 *    Description:  Cross platform time functions
 *
 *        Version:  1.0
 *        Created:  10 December 2013
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

#ifndef PTIME_H
#define PTIME_H

#define MILLION 1000000

#ifdef _WIN32
	/* Windows specific headers	*/
 	#include <windows.h>
#else
 	/* Unix specific headers */
 	#include <sys/time.h>

 	/* OS X specific headers */
 	#ifdef __MACH__
 		
 	#endif
#endif


/*
 *  Stopwatch - stores data for computing elapsed time
 */
typedef struct Stopwatch
{
	#ifdef _WIN32
	    LARGE_INTEGER start, stop, freq;
    #else
    	struct timeval start, stop;
    #endif
} STOP_WATCH;


/*
 *	Starts time
 *  @param  STOP_WATCH *sw - address of the stop watch to start
 *  @return nothing
 */
void startSW( STOP_WATCH *sw );

/*
 *	Retrieves elapsing time of the stopwatch
 *  @param  STOP_WATCH *sw - address of the stop watch
 *  @return double - the elapsed time in seconds
 */
double getElaspedTimeSW( STOP_WATCH *sw );


#endif
