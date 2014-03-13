/*
 * =====================================================================================
 *
 *       Filename:  hash.h
 *
 *    Description:  Hash functions
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
 *   Organization:  New York University Abu Dhab
 *
 * =====================================================================================
 */

#include "defines.h"

#ifndef HASH_H
#define HASH_H


/*
 *  Hashes the given string <key> to an whole number bounded by <size> using
 *  Jenkin's one-at-a-time hash (By Bob Jenkins
 *  <http://en.wikipedia.org/wiki/Jenkins_hash_function> )
 *
 *  @param  char*   key  - string to be hashed
 *  @param  int     size - the size of the hash list
 *  @return int	    hashed value for the given <key>
 */
int hashStringToInt( char* key, int size );


#endif
