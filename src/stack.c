
#include <stdbool.h>
#include <stdlib.h>

#include "err.h"
#include "stack.h"
#include "types.h"

/*--------------------------------------------------*/
/** @addtogroup Stack_types
 * Types for stack.
 * @{
 */

/**
 * @brief   Stack item.
 *
 * This structure represents item of stack.
 */
typedef struct stack_item
{
  Phrasem data; /**< Data part. */
  struct stack_item * next; /**< Pointer to next item. */
} StackItem;

/**
 * @brief   Stack head.
 */
typedef struct stack_head
{
  StackItem * first; /**< Pointer to first. */
} Stack;

/** @} */
/*--------------------------------------------------*/
/** @addtogroup Stack_data
 * Data for stack.
 * @{
 */

static Stack mstack; /**< Stack. */


/** @} */
/*--------------------------------------------------*/
/** @addtogroup Stack_functions
 * Functions for interaction with stack.
 * @{
 */

void ClearStack();
void EndStack(const char * msg, ErrorType errtype)
{
  if(msg != NULL)
  {
    #ifdef STACK_DEBUG
      debug(msg);
    #endif
    setErrorType(errtype);
    setErrorMessage(msg);
  }
  else
  {
    #ifdef STACK_DEBUG
      debug("End Stack.");
    #endif
  }
}

bool PushOntoStack(Phrasem data)
{
  StackItem * it = malloc(sizeof(struct stack_item));
  if(it == NULL)
  {
    EndStack("Stack: PushOntoStack: couldn't allocate memory", ErrorType_Internal);
    return false;
  }

  // filling up
  it->data = data;
  it->next = mstack.first;

  // moving head
  mstack.first = it;
  #ifdef STACK_DEBUG
    debug("Stack recieved an item.");
  #endif
  return true;
}

Phrasem PopFromStack()
{
  StackItem * it = mstack.first;
  if(it == NULL)
  {
    #ifdef STACK_DEBUG
      debug("PopFromStack: empty stack");
    #endif
    return NULL;
  }

  mstack.first = it;

  Phrasem p = it->data;
  free(it);
  #ifdef STACK_DEBUG
    debug("Stack popped an item.");
  #endif
  return p;
}

void ClearStack()
{
  Phrasem p;
  while((p = PopFromStack()) != NULL) { free(p); }
  EndStack(NULL, ErrorType_Ok);
}
