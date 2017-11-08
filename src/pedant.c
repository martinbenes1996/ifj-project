
#include "io.h"
#include "pedant.h"
#include "stack.h"

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

  return true;
}
