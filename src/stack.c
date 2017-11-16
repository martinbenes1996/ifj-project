
#include <stdbool.h>
#include <stdlib.h>

#include "err.h"
#include "functions.h"
#include "io.h"
#include "stack.h"
#include "types.h"

/*-------------------------- Phrasem Stack -----------------------------------*/

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
  st->first = st->first->next;

  // getting the data
  Phrasem p = it->data;

  // free the item, return data
  free(it);
  #ifdef STACK_DEBUG
    debug("Stack popped an item.");
  #endif
  return p;
}

bool TurnStack(Stack st)
{
    #ifdef STACK_DEBUG
        debug("Stack was turned.");
    #endif

    // control
    if(st == NULL)
    {
        EndStack("Stack: PopFromStack: recieved NULL pointer", ErrorType_Internal);
        return NULL;
    }

    Stack st2;
    st2 = InitStack();
    Phrasem pom = NULL;

    //puts items from top of one stack to the bottom of another stack
    while((pom = PopFromStack(st)) != NULL)
        if(PushOntoStack(st2, pom) == false)
        {
            ClearStack(st);
            ClearStack(st2);
            return false;
        }

    // st -> new stack
    ClearStack(st);
    st = st2;

    return true;
}

void ClearStack(Stack st)
{
  Phrasem p;
  while((p = PopFromStack(st)) != NULL) { free(p); }
  free(st);
  EndStack(NULL, ErrorType_Ok);
}


void PrintStack(Stack st)
{
  // control
  if(st == NULL)
  {
    EndStack("Stack: PrintStack: recieved NULL pointer", ErrorType_Internal);
    return;
  }

  // selecting the first
  StackItem * it = st->first;
  debug("---STACK---");
  if(it == NULL)
  {
    debug("empty stack");
  }
  else
  {
    do {
      PrintPhrasem(it->data);
      it = it->next;
    } while(it != NULL);
  }
  debug("-----------");
}


/*--------------------------------EXPRESSION PARSE STACK-----------------------------*/

ExprParserStack EPStack = {.itemCount = 0, .first = NULL};

//function for debuging expression parsing
void printstackEP()
{
  printf("\nPRINTING EP_STACK\n");
    ExprParserItem * pom = EPStack.first;

    while(pom != NULL)
    {
        if(pom->data == 0)
            printf("%s\n", "+");
        else if(pom->data == 1)
            printf("%s\n", "-");
        else if(pom->data == 2)
            printf("%s\n", "\\");
        else if(pom->data == 3)
            printf("%s\n", "*");
        else if(pom->data == 4)
            printf("%s\n", "/");
        else if(pom->data == 5)
            printf("%s\n", "(");
        else if(pom->data == 6)
            printf("%s\n", ")");
        else if(pom->data == 7)
            printf("%s\n", "i");
        else if(pom->data == 8)
            printf("%s\n", "$");
        else if(pom->data == 9)
            printf("%s\n", "E");
        else if(pom->data == 10)
            printf("%s\n", "<");
        pom = pom->next;
    }
}

void InitEPStack()
{
  // initialization
  EPStack.first = NULL;
  return;
}

bool PushOntoEPStack(char data)
{
  // allocation
  ExprParserItem * it = malloc(sizeof(ExprParserItem));
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
  EPStack.first = EPStack.first->next;
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
  while((p = PopFromEPStack()) != -1) {}
  EPStack.itemCount = 0;
  EndStack(NULL, ErrorType_Ok);
}

bool LookTripleAheadEPStack(char x1, char x2, char x3)
{
    #ifdef STACK_DEBUG
        debug("EPStack Looks triple ahead.");
    #endif

    if(EPStack.itemCount < 4) return false;
    //peeking into stack
    if( EPStack.first->data == x1 &&
        EPStack.first->next->data == x2 &&
        EPStack.first->next->next->data == x3 &&
        EPStack.first->next->next->next->data == op_les)
    {
        return true;
    }
    return false;
}

bool LookOneAheadEPStack(char x1)
{
    #ifdef STACK_DEBUG
        debug("EPStack Looks one ahead.");
    #endif

    if(EPStack.itemCount < 2) return false;
    //peeking into stack
    if( EPStack.first->data == x1 &&
        EPStack.first->next->data == op_les)
    {
        return true;
    }
    return false;
}

bool LookEAheadEPStack()
{
    #ifdef STACK_DEBUG
        debug("EPStack Looks for E ahead.");
    #endif

    if(EPStack.itemCount < 2) return false;
    //peeking into stack
    if( EPStack.first->data == op_E)
    {
        return true;
    }
    return false;
}

char ExprOnTopOfEPStack()
{
    #ifdef STACK_DEBUG
        debug("EPStack Looks for a symbol on top of the stack.");
    #endif

    //peeking into stack
    if( EPStack.first->data == op_E)
    {
        return EPStack.first->next->data;
    }
    return EPStack.first->data;
}

bool LookEndAheadEPStack()
{
    #ifdef STACK_DEBUG
        debug("EPStack looks for end.");
    #endif

    if(EPStack.itemCount < 1) return false;
    //peeking into stack
    if((EPStack.first->data == op_E &&
        EPStack.first->next->data == op_$) ||
        EPStack.first->data == op_$          )
    {
        return true;
    }
    return false;
}
