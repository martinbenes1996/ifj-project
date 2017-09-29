
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
} * Phrasem;

#define ALLOC_ERR 99

/** @} */
/*--------------------------------------------------*/
#endif // TYPES_H
