
/**
 * @file symtable.h
 * @interface symtable
 * @authors xbenes49 xbolsh00 xkrato47 xpolan09
 * @date 6th november 2017
 * @brief Table library
 *
 * This interface enables interactions with tables
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
                    Keyword table -> symtable.h and symtable.c
                    Constant table (almost) -> symtable.h
                    Table of all tables -> types.h

                  TO BE DONE:
                    Stupid symbol table. I need to think about that a lot more...
                */

                /*Ksenia will give you only the symbol names*/


/*-----------------------------------------------------------*/

                //KEYWORD TABLE FUNCTIONS

/**
 * @brief   Keyword finder.
 *
 * This function matches given word with keyword.
 * Returns -1 if word is not a keyword, otherwise
 * returns keyword code (array index)
 * @param word    Unknown Word.
 * @returns Keyword code or -1.
 */
int isKeyword(const char * word);


/*-----------------------------------------------------------*/

                //CONST TABLE FUNCTIONS


/**
 * @brief   Insert constant.
 *
 * This function inserts constant into array of constants.
 * Returns -1 when unsuccessful, otherwise
 * returns array index of added constant.
 * @param ptr    array pointer.
 * @param type   type of constant.
 * @param uni    value of constant.
 * @returns array index.
 */
int constInsert(dataType type, dataUnion uni);
//dataUnion and dataType are in types.h


/*TO DO: (not needed for lexical analysis)
    delete_array
    delete_const   ?
    find_value
    find_type
*/
/*-----------------------------------------------------------*/

                //VARIABLE TABLE FUNCTIONS


/**
 * @brief   Insert variable.
 *
 * This function inserts variable into array of variables.
 * Returns -1 when unsuccessful, otherwise
 * returns an index into array of variable. (Hopefully)
 * @param name    Name of a variable.
 * @returns array index.
 */
int varInsert(const char * name);


#endif
