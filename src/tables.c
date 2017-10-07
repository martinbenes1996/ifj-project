
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
#include "tables.h"
#include "types.h"


/*-----------------------------------------------------------*/

        //CONSTANT DATA TABLE (not sure if public or private yet)

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
struct constantArray{
    size_t arr_size;    //array size
    size_t count;           //number of entities in array
    struct constant arr[];
} ConstArray;



/*-----------------------------------------------------------*/

                //CONST TABLE FUNCTIONS

     /*the rest of this file is most likely wrong and will be reworked*/

                    //FUNCTION TABLE FUNCTIONS

/**
 * @brief   Initialisation of table of functions.
 *
 * This function creates array of functions.
 * Returns NULL when unsuccessful, otherwise
 * returns pointer to array of size (10?).
 * @returns Pointer to array of functions.
 */
//FunctionData * functionInit(void);
// --- the same as what I type above

/**
 * @brief   Insert function.
 *
 * This function inserts function into array of functions.
 * Returns -1 when unsuccessful, otherwise returns 1
 * @param ptr    array pointer.
 * @param name   name of the function.
 * @returns -1 -> failure, 1 -> success.
 */
//int functionInsert(FunctionData * ptr, char * name);

/**
 * @brief   Insert parameter into an existing function.
 *
 * This function inserts parameter into a function.
 * Returns -1 when unsuccessful (function not found,...), otherwise returns 1
 * @param name   name of the function.
 * @param type   type of parameter.
 * @param paramName    name of parametre.
 * @returns -1 -> failure, 1 -> success.
 */
int paramInsert(char * name, DataType type, char * paramName);


/*TO DO:
    int numberOfParam(FunctionData * ptr, const char * name);
    freeTable
    findFunction
    findParametreName (finds one param of a function, takes index of param)
    findParametreType
    findParametreInd
*/
