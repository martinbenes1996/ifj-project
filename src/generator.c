
#include <stdlib.h>

#include "generator.h"
#include "io.h"
#include "types.h"

/*------------------ GENERATOR STACK ----------------------*/
typedef enum
{
  State_Condition,
  State_Cycle,
  State_Assignment,
  State_FunctionCall,
  State_Input,
  State_Print,
  State_Expression,
  State_RelativeOperator,
  State_Empty
} State;

typedef struct stack_state
{
  State state;
  struct stack_state * next;
} * StackState;

typedef struct genstack { StackState first; } GeneratorStack;
static GeneratorStack mStack;

bool AssignGState(State newstate)
{
  StackState newitem = malloc(sizeof(struct stack_state));
  if(newitem == NULL) return false;

  newitem->state = newstate;
  newitem->next = mStack.first;
  mStack.first = newitem;
  return true;
}
void UnsignGState()
{
  StackState newitem = mStack.first;
  if(newitem != NULL) mStack.first = mStack.first->next;
  free(newitem);
}
State LookUpGState()
{
  StackState newitem = mStack.first;
  if(newitem != NULL) return newitem->state;
  else return State_Empty;
}
/*---------------------------------------------------------*/

bool Send(Stack s)
{
  #ifdef GENERATOR_DEBUG
    debug("Send to Generator");
  #endif

  (void)s;
  return true;
}

bool HandlePhrasem(Phrasem p)
{
  (void)p;
  return true;
}

void G_FunctionCall()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate function call");
  #endif

  AssignGState(State_FunctionCall);

}

void G_Condition()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate condition");
  #endif
  AssignGState(State_Condition);

}

void G_Cycle()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate cycle");
  #endif


}

void G_Assignment()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate assignment");
  #endif


}

void G_Print()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate print");
  #endif


}

void G_Input()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate input");
  #endif


}

void G_Logic()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate logic");
  #endif


}

void G_Expression()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate expression");
  #endif

}

void G_RelativeOperator()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate expression");
  #endif

}
