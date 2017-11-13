
#include "err.h"
#include "functions.h"
#include "io.h"
#include "pedant.h"
#include "stack.h"
#include "symtable.h"
#include "types.h"

#include <stdio.h>
#include <stdlib.h>

void EndPedant(const char * msg, long line, ErrorType errtype)
{
  #ifdef PEDANT_DEBUG
    debug("Ending Pedant.");
  #endif

  if(msg != NULL)
  {
    #ifdef PEDANT_DEBUG
      debug(msg);
    #endif
    setErrorType(errtype);
    setErrorMessage(msg);
    if(line != -1) setErrorLine(line);
  }
  else
  {
    setErrorType(ErrorType_Ok);
    setErrorMessage("");
  }
}

#define RaiseError(msg, phrasem, errtype)                       \
  do {                                                          \
    err("%s: %s: l.%d: %s", __FILE__, __func__, __LINE__, msg); \
    EndPedant(msg, phrasem ->line, errtype);                    \
    freePhrasem(phrasem);                                       \
    return false;                                               \
  } while(0)

//static Stack s = NULL;

bool P_VariableDefined(const char * funcname, const char * varname)
{
  #ifdef PEDANT_DEBUG
    debug("Pedant, Variable Defined?");
  #endif

  (void)funcname;
  (void)varname;
  return true;
}

bool P_DefineNewVariable(const char * funcname, Phrasem varname, Phrasem datatype)
{
  #ifdef PEDANT_DEBUG
    debug("Pedant, Define New Variable");
  #endif

  DataType type = getDataType(datatype);
  if(type == DataType_Unknown)
  {
    freePhrasem(varname);
    RaiseError("unknown datatype", datatype, ErrorType_Semantic1);
  }
  freePhrasem(datatype);

  if(!addVariable(funcname, varname->d.str) || !addVariableType(funcname, varname->d.str, type))
  {
    RaiseError("alloc variable error", varname, ErrorType_Semantic1);
  }

  freePhrasem(varname);
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
bool P_DefineNewFunction(const char * funcname)
{
  #ifdef PEDANT_DEBUG
    debug("Pedant, Define New Function.");
  #endif
  return addFunction(funcname);
}

//Docasne
void printfstack(Stack a)
{printf("\nPRINTING STACK\n");
    StackItem * pom = a->first;

    while(pom != NULL)
    {
        if(pom->data->table == TokenType_Symbol) printf("%s\n", pom->data->d.str);
            else printf("%ld\n", pom->data->d.index);
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
  //if(!PushOntoStack(s, p)) return false;
  //printfstack(s);
  //printf("Token: %d\n", p->table);

  return true;
}
