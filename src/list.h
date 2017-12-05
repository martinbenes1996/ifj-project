
/**
 * @file list.h
 * @interface generator
 * @authors xbenes49 xbolsh00 xpolan09
 * @date 6th november 2017
 * @brief List interface
 *
 * This interface declares functions to implement parameters list.
 */

#ifndef LIST_H
#define LIST_H

#include "types.h"

/**
 * @brief   Structure representing type and name of parametres.
 *          Used for sending lists of parametres.
 * List.
 */
typedef struct paramFce{
    DataType type;
    char * name;
    struct paramFce *next;
} * Parameters;

/*-----------------------------------------------------------*/
/** @addtogroup Parameter_functions
 * Main parameter functions.
 * @{
 */

/**
 * @brief     Parameter list ctor.
 *
 * This function constructs the parameter structure.
 * @returns NULL.
 */
Parameters paramInit();

/**
 * @brief     Adds item to list.
 *
 * This function adds item to parameter list, with name and datatype given.
 * @param p     List to add to.
 * @param name  Name of the parameter (NULL, if declaration).
 * @param dt    DataType of the parameter.
 * @returns True if success. False if fail.
 */
bool paramAdd(Parameters * p, const char * name, DataType dt);

/**
 * @brief     Prints parameters.
 *
 * This function prints the parameter list structure. It is used
 * when debugging.
 * @param p     Parameter list being printed.
 */
void PrintParameters(Parameters p);


/**
 * @brief   Destroys parameters of a function.
 *
 * This function destroys the given parameter list.
 * @param parameter   The list being destroyed.
 */
void paramFree(Parameters parameter);

/**
 * @brief   Pops one item.
 *
 * This function pops one list item (parameter) and returns its type.
 * @param p     The altered list.
 * @returns DataType of the top parameter, or DataType_Unknown, if empty.
 */
DataType paramPop(Parameters * p);

/**
 * @brief   List comparison.
 *
 * This function compares given lists, checks if they match in DataTypes of
 * parameters.
 * @param p1      First parameter list.
 * @param p2      Second parameter list.
 * @returns True if same. False if not.
 */
bool ParametersMatches(Parameters p1, Parameters p2);

bool findParamName(Parameters parameter, const char * name);

/** @} */
/*-----------------------------------------------------------*/

#endif // LIST_H
