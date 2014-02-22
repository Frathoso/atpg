/*
 * =====================================================================================
 *
 *       Filename:  logic_tables.h
 *
 *    Description:  Define the boolean five valued logic tables
 *        Version:  1.0
 *        Created:  15 February 2014
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

#include "atpg_types.h"


#ifndef LOGIC_TABLES_H
#define LOGIC_TABLES_H


/*
 *  Logic tables
 */
static const                                //  0   1   D   B   X
LOGIC_VALUE TABLE_AND[5][5]  =  {/* 0 */    {   0,  0,  0,  0,  0   },
                                 /* 1 */    {   0,  1,  D,  B,  X   },
                                 /* D */    {   0,  D,  D,  0,  X   },
                                 /* B */    {   0,  B,  0,  B,  X   },
                                 /* X */    {   0,  X,  X,  X,  X   }
                                };

static const                                //  0   1   D   B   X
LOGIC_VALUE TABLE_NAND[5][5]  = {/* 0 */    {   1,  1,  1,  1,  1   },
                                 /* 1 */    {   1,  0,  B,  D,  X   },
                                 /* D */    {   1,  B,  B,  1,  X   },
                                 /* B */    {   1,  D,  1,  D,  X   },
                                 /* X */    {   1,  X,  X,  X,  X   }
                                };

static const                                //  0   1   D   B   X
LOGIC_VALUE TABLE_OR[5][5]   =  {/* 0 */    {   0,  1,  D,  B,  X   },
                                 /* 1 */    {   1,  1,  1,  1,  1   },
                                 /* D */    {   D,  1,  D,  1,  X   },
                                 /* B */    {   B,  1,  1,  B,  X   },
                                 /* X */    {   X,  1,  X,  X,  X   }
                                };

static const                                //  0   1   D   B   X
LOGIC_VALUE TABLE_NOR[5][5]  =  {/* 0 */    {   1,  0,  B,  D,  X   },
                                 /* 1 */    {   0,  0,  0,  0,  0   },
                                 /* D */    {   B,  0,  B,  0,  X   },
                                 /* B */    {   D,  0,  0,  D,  X   },
                                 /* X */    {   X,  0,  X,  X,  X   }
                                };

static const                                //  0   1   D   B   X
LOGIC_VALUE TABLE_XOR[5][5]  =  {/* 0 */    {   0,  1,  D,  B,  X   },
                                 /* 1 */    {   1,  0,  B,  D,  X   },
                                 /* D */    {   D,  B,  0,  1,  X   },
                                 /* B */    {   B,  D,  1,  0,  X   },
                                 /* X */    {   X,  X,  X,  X,  X   }
                                };

static const                                //  0   1   D   B   X
LOGIC_VALUE TABLE_XNOR[5][5]  = {/* 0 */    {   1,  0,  B,  D,  X   },
                                 /* 1 */    {   0,  1,  D,  B,  X   },
                                 /* D */    {   B,  D,  1,  0,  X   },
                                 /* B */    {   D,  B,  0,  1,  X   },
                                 /* X */    {   X,  X,  X,  X,  X   }
                                };

static const                        //  0   1   D   B   X
LOGIC_VALUE TABLE_INV [5]    =  {       1,  0,  B,  D,  X   };


#endif
