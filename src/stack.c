
#include <stdbool.h>
#include <stdlib.h>

#include "err.h"
#include "stack.h"
#include "types.h"


/*--------------------------------------------------*/
/** @addtogroup Stack_functions
 * Functions for interaction with stack.
 * @{
 */

Stack InitStack()
{
  // allocation
  Stack st = malloc(sizeof(struct stack));
  if(st == NULL) return NULL;

  // initialization
  st->first = NULL;
  return st;
}

void EndStack(const char * msg, ErrorType errtype)
{
  // error
  if(msg != NULL)
  {
    #ifdef STACK_DEBUG
      debug(msg);
    #endif
    setErrorType(errtype);
    setErrorMessage(msg);
  }
  // ok
  else
  {
    #ifdef STACK_DEBUG
      debug("End Stack.");
    #endif
  }
}

bool PushOntoStack(Stack st, Phrasem data)
{
  // control
  if(st == NULL)
  {
    EndStack("Stack: PushOntoStack: recieved NULL pointer", ErrorType_Internal);
    return false;
  }

  // allocation
  StackItem * it = malloc(sizeof(struct stack_item));
  if(it == NULL)
  {
    EndStack("Stack: PushOntoStack: couldn't allocate memory", ErrorType_Internal);
    return false;
  }

  // filling up
  it->data = data;
  it->next = st->first;

  // moving pointer
  st->first = it;
  #ifdef STACK_DEBUG
    debug("Stack recieved an item.");
  #endif
  return true;
}

Phrasem PopFromStack(Stack st)
{
  // control
  if(st == NULL)
  {
    EndStack("Stack: PopFromStack: recieved NULL pointer", ErrorType_Internal);
    return NULL;
  }

  // selecting the first
  StackItem * it = st->first;
  if(it == NULL)
  {
    #ifdef STACK_DEBUG
      debug("PopFromStack: empty stack");
    #endif
    return NULL;
  }

  // moving the head
  st->first = it;

  // getting the data
  Phrasem p = it->data;

  // free the item, return data
  free(it);
  #ifdef STACK_DEBUG
    debug("Stack popped an item.");
  #endif
  return p;
}

void ClearStack(Stack st)
{
  Phrasem p;
  while((p = PopFromStack(st)) != NULL) { free(p); }
  free(st);
  EndStack(NULL, ErrorType_Ok);
}
