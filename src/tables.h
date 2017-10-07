
/**
 * @file tables.h
 * @interface tables
 * @authors xbenes49 xbolsh00 xkrato47 xpolan09
 * @date 7th november 2017
 * @brief Interface of tables.
 *
 * This is interface of interactions with tables (apart from symbol table).
 */

#ifndef TABLES_H
#define TABLES_H
#include "types.h"

/*----------------------------------------------------------*/
/**
 * @brief   Keyword table.
 *
 * This array contains list af all possible keywords.
 */
const char keywords[35][9] = {"as", "asc", "declare", "dim", "do", "double", "else",
                              "end", "chr", "function", "if", "input", "integer",
                              "length", "loop", "print", "return", "scope", "string",
                              "substr", "then", "while", "and", "boolean", "continue",
                              "elseif", "exit", "false", "for", "next", "not", "or",
                              "shared", "static", "true"};

//----- yep, static, but this will be in symtable.c, so no one could reach it

//I think it will have to be accessible for syntax analysis
//you will need to know what keyword is in the token (there will be just an index into this array)


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
int constInsert(DataType type, DataUnion uni);
//dataUnion and dataType are in types.h


/*TO DO: (not needed for lexical analysis)
    delete_array
    delete_const   ?
    find_value
    find_type
*/

#endif //TABLES_H
