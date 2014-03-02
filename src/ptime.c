/*
 * =====================================================================================
 *
 *       Filename:  ptime.c
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

/*  uint32_t */
#include <stdint.h>

/*  strlen  */
#include <string.h>


#include "ptime.h"


/*
 *  Initializes and starts stopwatch time
 *  @param  STOP_WATCH *sw - address of the stop watch to start
 *  @return nothing
 */
void startSW( STOP_WATCH *sw )
{
#ifdef _WIN32
    QueryPerformanceCounter(&(sw->start));
    QueryPerformanceFrequency(&(sw->freq));
#else
    gettimeofday(&(sw->start), 0);
#endif
}

/*
 *  Computes the elapsed time of the stopwatch
 *  @param  STOP_WATCH *sw - address of the stop watch
 *  @return double - the elapsed time in seconds
 */
double getElaspedTimeSW( STOP_WATCH *sw )
{
#ifdef _WIN32
    QueryPerformanceCounter(&(sw->stop));
    return (double) (sw->stop.QuadPart - sw->start.QuadPart) / sw->freq.QuadPart;
#else
    gettimeofday(&(sw->stop), 0);
    return (sw->stop.tv_sec - sw->start.tv_sec +
            (double)(sw->stop.tv_usec - sw->start.tv_usec) / MILLION);
#endif
}
