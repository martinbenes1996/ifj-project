
/**
 * @file types.h
 * @authors xbenes49 xbolsh00 xkrato47 xpolan09
 * @date 19th september 2017
 * @brief Definitions of public types.
 *
 * This module defines basic public types, used in whole project.
 */

#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>

/*--------------------------------------------------*/
/** @addtogroup Public_types
 * Basic public types, used in whole project.
 * @{
 */


/**
 * @brief   Structure representing arguments.
 *
 * This structure is filled in arguments of main, and then it indicates
 * wanted functionality.
 */
typedef struct
{
  bool help; /**< Help parameter. */
  /* will be added */
} args_t;


/**
 * @brief   Structure representing phrasem data.
 *
 * This structure is filled with data of phrasem, type etc.
 */
typedef struct phrasem_data
{
  int id; /**< Id of phrasem (only sample). */
  /* will be added */
}* Phrasem;
/**
 * @brief   Debug function to print phrasem.
 * @param p       Phrasem to be printed.
 */
inline void PrintPhrasem(Phrasem p)
{
  if(p != NULL) debug("Phrasem: %d", p->id);
}

/** @} */
/*--------------------------------------------------*/
/** @addtogroup Return_codes
 * Return codes.
 * @{
 */

#define OK 0
#define SYNTAX_ERROR 1
#define SEMANTIC1_ERROR 2
#define SEMANTIC2_ERROR 3
#define INTERNAL_ERROR 99

/** @} */
/*--------------------------------------------------*/

#define END_PTR (void *)1

#endif // TYPES_H
