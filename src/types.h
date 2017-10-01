
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


/** @} */
/*--------------------------------------------------*/
/** @addtogroup Queue_types
 * Types used in Queue module and others.
 * @{
 */


/**
 * @brief   One item in queue.
 *
 * Queue is made by head (to remove from), tail (to add to) and linked
 * list of items, containing data and pointer to next item.
 * First item is addressed by pointer from head, last from tail and
 * the next element is set to NULL (nothing).
 */
struct queue_item{
  Phrasem data; /**< Data of item. */
  struct queue_item * next; /**< Pointer to next item. */
};

/**
 * @brief   Head of queue
 *
 * It points to the item, which is to be removed as first.
 */
struct queue_head
{
  struct queue_item * first; /**< First item in the queue (to be removed). */
};

/**
 * @brief   Tail of queue
 *
 * It points to the item, where to put new item (behind).
 */
struct queue_tail
{
  struct queue_item * last; /**< Last element in the queue (new items are added behind). */
};


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

#endif // TYPES_H
