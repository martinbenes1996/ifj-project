
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


/*--------------------------------EXPRESSION PARSE STACK-----------------------------*/

void InitEPStack()
{
  // initialization
  EPStack.first = NULL;
  return;
}

bool PushOntoEPStack(char data)
{
  // allocation
  ExprParserItem * it = malloc(sizeof(struct ExprParserItem));
  if(it == NULL)
  {
    EndStack("EPStack: PushOntoEPStack: couldn't allocate memory", ErrorType_Internal);
    return false;
  }

  // filling up
  it->data = data;
  it->next = EPStack.first;
  EPStack.itemCount++;

  // moving pointer
  EPStack.first = it;
  #ifdef STACK_DEBUG
    debug("EPStack recieved an item.");
  #endif
  return true;
}

char PopFromEPStack()
{
  // selecting the first
  ExprParserItem * it = EPStack.first;
  if(it == NULL)
  {
    #ifdef STACK_DEBUG
      debug("PopFromEPStack: empty EPstack");
    #endif
    return -1;
  }

  // moving the head
  EPStack.first = it;
  EPStack.itemCount--;

  // getting the data
  char p = it->data;

  // free the item, return data
  free(it);
  #ifdef STACK_DEBUG
    debug("EPStack popped an item.");
  #endif
  return p;
}

void ClearEPStack()
{
  char p;
  while((p = PopFromStack(st)) != -1) {}
  EPStack.itemCount = 0;
  EndStack(NULL, ErrorType_Ok);
}

bool LookTripleAheadEPStack(char x1, char x2, char x3)
{
    //peeking into stack
    if( EPStack.first->data == x1 &&
        EPStack.first->next->data == x2 &&
        EPStack.first->next->next->data == x3 &&
        EPStack.first->next->next->next->data == '<')
    {
        return true;
    }
    return false;
}

bool LookOneAheadEPStack(char x1)
{
    //peeking into stack
    if( EPStack.first->data == x1 &&
        EPStack.first->next->data == '<')
    {
        return true;
    }
    return false;
}

bool LookEndAheadEPStack()
{
    //peeking into stack
    if((EPStack.first->data == 'E' &&
        EPStack.first->next->data == '$') ||
        EPStack.first->data == '$'          )
    {
        return true;
    }
    return false;
}



