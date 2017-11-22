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

Parameters paramInit();

bool paramAdd(Parameters * p, const char * name, DataType dt);



/**
 * @brief   Destroys parameters of a function.
 *
 */
void paramFree(Parameters parameter);

DataType paramPop(Parameters * p);

bool ParametersMatches(Parameters p1, Parameters p2);

#endif // LIST_H
