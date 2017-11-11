
#include "io.h"
#include "pedant.h"
#include "stack.h"

#include <stdio.h>
#include <stdlib.h>

static Stack s = NULL;

bool P_VariableDefined(long functionId, const char * varname)
{
  #ifdef PEDANT_DEBUG
    debug("Pedant, Variable Defined?");
  #endif

  (void)functionId;
  (void)varname;
  return true;
}

bool P_FunctionDefined(const char * funcname)
{
  #ifdef PEDANT_DEBUG
    debug("Pedant, Function Defined?");
  #endif
  (void)funcname;
  return true;
}
//Docasne
void printfstack(Stack a)
{printf("\nPRINTING STACK\n");
    StackItem * pom = a->first;

    while(pom != NULL)
    {
        if(pom->data->table == TokenType_Symbol) printf("%s\n", pom->data->d.str);
            else printf("%d\n", pom->data->d.index);
        pom = pom->next;
    }
}

bool P_HandleOperand(Phrasem p)
{
  if(p == NULL)
  {
    // error
    // ...
    return false;
  }

  // conversion infix->postfix here??
  // ...

  // how to control?
  if(!PushOntoStack(s, p)) return false;
  printfstack(s);

  return true;
}
