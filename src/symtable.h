
/**
 * @file symtable.h
 * @interface symtable
 * @authors xbenes49 xbolsh00 xkrato47 xpolan09
 * @date 6th november 2017
 * @brief Table library
 *
 * This interface enables interactions with symbol table.
 */


#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stdlib.h>
#include <stdio.h>
#include "types.h"

                /*in the process of creation*/

                /*DONE (interface):
                    Operator table -> types.h
                    Table of data types -> types.h
                    Keyword table -> tables.h
                    Constant table (almost) -> tables.h
                    Table of all tables -> types.h

                  TO BE DONE:
                    Symbol table. -> symtable.h
                */

                /*Ksenia will give you only the symbol names*/


/*-----------------------------------------------------------*/

                //VARIABLE TABLE FUNCTIONS


/**
 * @brief   Insert variable.
 *
 * This function inserts variable into array of variables and returns
 * an index into array of variable. (Hopefully)
 * @param name    Name of a variable.
 * @returns Array index. -1 if error
 */
int varInsert(const char * name);


#endif
