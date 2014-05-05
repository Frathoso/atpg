/*
 * =====================================================================================
 *
 *       Filename:  logic_tables.h
 *
 *    Description:  Define the boolean five valued logic tables and logic operators
 *
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
static const                                //  O   I   D   B   X
LOGIC_VALUE TABLE_AND[5][5]  =  {/* O */    {   O,  O,  O,  O,  O   },
                                 /* I */    {   O,  I,  D,  B,  X   },
                                 /* D */    {   O,  D,  D,  O,  X   },
                                 /* B */    {   O,  B,  O,  B,  X   },
                                 /* X */    {   O,  X,  X,  X,  X   }
                                };

static const                                //  O   I   D   B   X
LOGIC_VALUE TABLE_NAND[5][5]  = {/* O */    {   I,  I,  I,  I,  I   },
                                 /* I */    {   I,  O,  B,  D,  X   },
                                 /* D */    {   I,  B,  B,  I,  X   },
                                 /* B */    {   I,  D,  I,  D,  X   },
                                 /* X */    {   I,  X,  X,  X,  X   }
                                };

static const                                //  O   I   D   B   X
LOGIC_VALUE TABLE_OR[5][5]   =  {/* O */    {   O,  I,  D,  B,  X   },
                                 /* I */    {   I,  I,  I,  I,  I   },
                                 /* D */    {   D,  I,  D,  I,  X   },
                                 /* B */    {   B,  I,  I,  B,  X   },
                                 /* X */    {   X,  I,  X,  X,  X   }
                                };

static const                                //  O   I   D   B   X
LOGIC_VALUE TABLE_NOR[5][5]  =  {/* O */    {   I,  O,  B,  D,  X   },
                                 /* I */    {   O,  O,  O,  O,  O   },
                                 /* D */    {   B,  O,  B,  O,  X   },
                                 /* B */    {   D,  O,  O,  D,  X   },
                                 /* X */    {   X,  O,  X,  X,  X   }
                                };

static const                                //  O   I   D   B   X
LOGIC_VALUE TABLE_XOR[5][5]  =  {/* O */    {   O,  I,  D,  B,  X   },
                                 /* I */    {   I,  O,  B,  D,  X   },
                                 /* D */    {   D,  B,  O,  I,  X   },
                                 /* B */    {   B,  D,  I,  O,  X   },
                                 /* X */    {   X,  X,  X,  X,  X   }
                                };

static const                                //  O   I   D   B   X
LOGIC_VALUE TABLE_XNOR[5][5]  = {/* O */    {   I,  O,  B,  D,  X   },
                                 /* I */    {   O,  I,  D,  B,  X   },
                                 /* D */    {   B,  D,  I,  O,  X   },
                                 /* B */    {   D,  B,  O,  I,  X   },
                                 /* X */    {   X,  X,  X,  X,  X   }
                                };

static const                        //  O   I   D   B   X
LOGIC_VALUE TABLE_INV [5]    =  {       I,  O,  B,  D,  X   };


/*
 *  Extracts a logic value from the given gate
 *
 *  @param  CIRCUIT circuit - the circuit containing the gates
 *  @param  int         index   - the target gate
 *  @return LOGIC_VALUE - the results of the operation
 */
inline LOGIC_VALUE getLogicValue( CIRCUIT circuit, int index, int inIndex);

/*
 *  Sets a logic value to the given gate's input line
 *
 *   @param  CIRCUIT circuit - the circuit containing the gates
 *  @param  int         index   - the target gate
 *  @return LOGIC_VALUE - the results of the operation
 */
inline void setInputLogicValue( CIRCUIT circuit, int index, int inIndex, LOGIC_VALUE log_val);

/*
 *  Computes the logic value of the inputs on passing through the gate
 *
 *  @param  CIRCUIT circuit - the circuit containing the gates
 *  @param  int 	index 	- the target gate
 *  @return LOGIC_VALUE	- the results of the operation
 */
LOGIC_VALUE computeGateOutput( CIRCUIT circuit, int index );

/*
 *  Checks if it is possible to generate the given output by manipulating 
 *  the Don't-Cares (X) input lines
 *
 *  @param  CIRCUIT circuit - the circuit containing the gates
 *  @param  int         index   - the target gate
 *  @param  LOGIC_VALUE output - the logical value output of interest 
 *  @return BOOLEAN TRUE if it is possible and FALSE otherwise
 */
BOOLEAN isOutputPossible( CIRCUIT circuit, int index, LOGIC_VALUE output );

/*
 *  Negates or passes the value as it is depending on the flag <inv>
 *
 *  @param  LOGIC_VALUE value - the value to be negated
 *  @param  BOOLEAN		inv   - marking if the value has to be negated or not
 *  @return LOGIC_VALUE	- the results of the operation
 */
LOGIC_VALUE negate( LOGIC_VALUE value, BOOLEAN inv );

/*
 *  Returns the name of the given logic value
 *
 *  @param  LOGIC_VALUE value - the value to be named
 *  @param  BOOLEAN isForOutput - use (1,0) instead of (I, O)
 *  @return char	- the name of the logic value
 */
char logicName( LOGIC_VALUE value, BOOLEAN isForOutput);

#endif
