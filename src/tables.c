
/**
 * @file tables.c
 * @interface tables
 * @authors xbenes49 xbolsh00 xkrato47 xpolan09
 * @date 7th november 2017
 * @brief Source code of tables.
 *
 * This implements interactions with tables (apart from symbol table).
 */


#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "tables.h"
#include "types.h"


/*-----------------------------------------------------------*/

                //KEYWORD TABLE FUNCTIONS

/**
 * @brief   Keyword table.
 *
 * This array contains list af all possible keywords.
 */
const char keywords[35][9] = {"as", "asc", "declare", "dim", "do", "double",
  "else", "end", "chr", "function", "if", "input", "integer", "length",
  "loop", "print", "return", "scope", "string", "substr", "then", "while",
  "and", "boolean", "continue", "elseif", "exit", "false", "for", "next",
  "not", "or", "shared", "static", "true"};

int isKeyword(const char * word)
{
    int index = -1;

    for(int i = 0; i < 35 ;++i)
        if(strcmp(word, keywords[i]) == 0)
        {
            index = i;
            break;
        }
    return index;
}

/*-----------------------------------------------------------*/

              //OPERATOR TABLE FUNCTIONS

/**
 * @brief   Operator table.
 *
 * This array contains list af all possible operators.
 * DO NOT CHANGE THE ORDER OF THINGS IN IT!!
 */
const char operators[13][3] = {"+", "-", "\\", "*", "/", "(", ")", "=", "<>", "<", "<=", ">", ">=",};

#define TABLE_ERROR -1
long getOperatorId(const char * word)
{
  if(word == NULL) return TABLE_ERROR;

  // look up

  return 1;

}
#undef TABLE_ERROR

/*************************************************************/

                    //TABLE OF CONSTANTS

/*************************************************************/

        //CONSTANT TABLE DATA (not sure if public or private yet)

#ifndef NAMED_CONSTANTS
#define NAMED_CONSTANTS

#define STARTING_CHUNK 10   //size of initialised arrays
#define PORTION_OF_TABLE 2  //when should table resize (count > arr_size/PORTION_OF_TABLE)
#define RESIZE_RATE 2       //how much should it resize

#endif
/**
 * @brief   Structure representing characteristics of constants.
 *
 * This structure is filled with type and value of the constant.
 * Value is represented by union.
 */
struct constant{
    DataType type;
    DataUnion data;
};

/**
 * @brief   Structure representing table of constants.
 *
 * This structure is filled with structures representing constants.
 * It knows its own size and resizes automatically by (+10?) entities.
 */
typedef struct constantArray{
    size_t arr_size;    //array size
    size_t count;           //number of entities in array
    struct constant * arr;  //non variadic array of constants (its easier)
} ConstArray;

static ConstArray consttable = {.arr_size = 0, .count = 0, .arr = NULL};

/*-----------------------------------------------------------*/

                //CONST TABLE FUNCTIONS

                    //DECLARATIONS

/*-----------------------------------------------------------*/
/**
 * @brief   Initiation of table of constants.
 *
 * This function allocates table os a starting size.
 * @returns true -> ok, false -> error.
 */
bool constTableInit(void);
/**
 * @brief   Destroys table of constants.
 *
 * This function frees the whole table.
 */
void constTableFree(void);
/**
 * @brief   Reallocs array of constants.
 *
 * This function increases size of table of constants.
 * @returns true -> ok, false -> fail.
 */
bool constTableResize(void);
/*-----------------------------------------------------------*/

                    //FUNCTION BODY - needs testing

bool constTableInit(void)
{
    //allocation of the table
    if( (consttable.arr = malloc(STARTING_CHUNK * sizeof(struct constant)))  == NULL)
    {
        //error message
        return false;
    }
    consttable.arr_size = STARTING_CHUNK;
    consttable.count = 0;

    for(size_t i = 0; i < consttable.arr_size ;++i)
    {
        consttable.arr[i].type = DataType_Unknown;
    }

    return true;
}

void constTableFree(void)
{
    consttable.arr_size = 0;
    consttable.count = 0;
    free(consttable.arr);
}

bool constTableResize(void)
{
    if( (consttable.arr = realloc(consttable.arr, consttable.arr_size * RESIZE_RATE
                                                    * sizeof(struct constant)))  == NULL)
    {
        //error message
        return false;
    }

    //for(size_t i = 0;i < consttable.arr_size;++i)     incomplete, may not be needed
        //consttable.arr[i]->type = DataType_Unknown;

    consttable.arr_size = consttable.arr_size * RESIZE_RATE;

    return true;
}

/*TO DO:
    finding
    adding + resizing controll

    Need to know more information about interaction requirements.
*/
