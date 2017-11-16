
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
    return false;                                               \
  } while(0)

/*------ DATA -------*/
static Stack mstack = NULL;
/*--------------------*/

bool P_VariableDefined(const char * funcname, Phrasem varname)
{
  #ifdef PEDANT_DEBUG
    debug("Pedant, Variable Defined?");
  #endif
  return findVariable(funcname, varname->d.str);
}

bool P_DefineNewVariable(const char * funcname, Phrasem varname, Phrasem datatype)
{
  #ifdef PEDANT_DEBUG
    debug("Pedant, Define New Variable");
  #endif

  DataType type = getDataType(datatype);
  if(type == DataType_Unknown)
  {
    RaiseError("unknown datatype", datatype, ErrorType_Semantic1);
  }

  if(!addVariable(funcname, varname->d.str) || !addVariableType(funcname, varname->d.str, type))
  {
    RaiseError("alloc variable error", varname, ErrorType_Semantic1);
  }

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
            else printf("%d\n", pom->data->d.index);
        pom = pom->next;
    }
}

bool RetypeToDouble(StackItem ** where)
{
    //alloc new phrasem and stackItem
    //type "retypeToDouble"
    //connect it to the list by using where:
    // newstackitem->next = (*where)->next;
    // *where = newstackitem;
(void **) where;
    return true;
}

bool RetypeToInt(StackItem ** where)
{
    //alloc new phrasem and stackItem
    //type "retypeToInt"
    //connect it to the list by using where:
    // newstackitem->next = (*where)->next;
    // *where = newstackitem;
(void **) where;
    return true;
}

// this function returns a type of operand or calls itself to find out the result of operation
// it saves value of from into variable in expressionEnd function
// and where into variable in the recursive parent function
DataType RetypeRecursive(StackItem *** where, StackItem ** from)
{
    if(*from == NULL) return DataType_Unknown;      //should not happen

    if((*from)->data->table == TokenType_Constant || (*from)->data->table == TokenType_Variable)
    {
        // where is a pointer to previous stackitem->next (pointing to current item)
        // which i may modify by inserting retype token to the list (behind current token)
        *where = from;
        *from = (*from)->next;          //set starting item to the next one for recursion
        return (**where)->data->table;  //return data of the operand
    }
    else if((*from)->data->table == TokenType_Operator)
    {
        Operators currentOperator = (*from)->data->d.index;
        StackItem * temp_from = *from;
        *where = from;
        *from = (*from)->next;

        StackItem ** where1 = NULL;
        StackItem ** where2 = NULL;
        DataType typeOfResult1;
        DataType typeOfResult2;
        typeOfResult1 = RetypeRecursive(&where1, from);
        typeOfResult2 = RetypeRecursive(&where2, from);

        switch(currentOperator)
        {
            case Add: if(typeOfResult1 == DataType_Unknown || typeOfResult2 == DataType_Unknown)
                      {
                            // some error occured
                            return DataType_Unknown;
                      }
                      else if(typeOfResult1 == typeOfResult2)   // <<<--- both operands are the same
                      {
                            // when operands are the same, no need for retyping
                            return typeOfResult1;
                      }
                      else if(typeOfResult1 == DataType_String || typeOfResult2 == DataType_String)
                      {
                            // when only one of them is string it is an error
                            RaiseError("only one operand is string (add)", temp_from->data, ErrorType_Semantic1);
                            return DataType_Unknown;
                      }
                      else if(typeOfResult1 == DataType_Integer && typeOfResult2 == DataType_Double)
                      {
                            if(!RetypeToDouble(where1)) return DataType_Unknown;
                            return DataType_Double;
                      }
                      else if(typeOfResult1 == DataType_Double && typeOfResult2 == DataType_Integer)
                      {
                            if(!RetypeToDouble(where2)) return DataType_Unknown;
                            return DataType_Double;
                      }
                      else
                      {
                            // error
                            return DataType_Unknown;
                      }
                      break;


            case Sub:
            case Mul: if(typeOfResult1 == DataType_Unknown || typeOfResult2 == DataType_Unknown)
                      {
                            // some error occured
                            return DataType_Unknown;
                      }
                      else if(typeOfResult1 == DataType_String || typeOfResult2 == DataType_String)
                      {
                            // when atleast one of them is string it is an error
                            RaiseError("one operand is string (mul, sub)", temp_from->data, ErrorType_Semantic1);
                            return DataType_Unknown;
                      }
                      else if(typeOfResult1 == typeOfResult2)   // <<<--- both operands are the same
                      {
                            // when operands are the same, no need for retyping
                            return typeOfResult1;
                      }
                      else if(typeOfResult1 == DataType_Integer && typeOfResult2 == DataType_Double)
                      {
                            if(!RetypeToDouble(where1)) return DataType_Unknown;
                            return DataType_Double;
                      }
                      else if(typeOfResult1 == DataType_Double && typeOfResult2 == DataType_Integer)
                      {
                            if(!RetypeToDouble(where2)) return DataType_Unknown;
                            return DataType_Double;
                      }
                      else
                      {
                            // error
                            return DataType_Unknown;
                      }
                      break;


            case DivInt:
                      if(typeOfResult1 == DataType_Unknown || typeOfResult2 == DataType_Unknown)
                      {
                            // some error occured
                            return DataType_Unknown;
                      }
                      else if(typeOfResult1 == DataType_String || typeOfResult2 == DataType_String)
                      {
                            // when atleast one of them is string it is an error
                            RaiseError("one operand is string (divInt)", temp_from->data, ErrorType_Semantic1);
                            return DataType_Unknown;
                      }
                      else if(typeOfResult1 == DataType_Integer && typeOfResult2 == DataType_Integer)
                      {
                            // both integers -> no need for retyping
                            return DataType_Integer;
                      }
                      else if(typeOfResult1 == DataType_Integer && typeOfResult2 == DataType_Double)
                      {
                            if(!RetypeToInt(where2)) return DataType_Unknown;
                            return DataType_Integer;
                      }
                      else if(typeOfResult1 == DataType_Double && typeOfResult2 == DataType_Integer)
                      {
                            if(!RetypeToInt(where1)) return DataType_Unknown;
                            return DataType_Integer;
                      }
                      else if(typeOfResult1 == DataType_Double && typeOfResult2 == DataType_Double)
                      {
                            if(!RetypeToInt(where1)) return DataType_Unknown;
                            if(!RetypeToInt(where2)) return DataType_Unknown;
                            return DataType_Integer;
                      }
                      else
                      {
                            // error
                            return DataType_Unknown;
                      }
                      break;


            case DivDouble:
                      if(typeOfResult1 == DataType_Unknown || typeOfResult2 == DataType_Unknown)
                      {
                            // some error occured
                            return DataType_Unknown;
                      }
                      else if(typeOfResult1 == DataType_String || typeOfResult2 == DataType_String)
                      {
                            // when atleast one of them is string it is an error
                            RaiseError("one operand is string (divDouble)", temp_from->data, ErrorType_Semantic1);
                            return DataType_Unknown;
                      }
                      else if(typeOfResult1 == DataType_Double && typeOfResult2 == DataType_Double)
                      {
                            // both doubles -> no need for retyping
                            return DataType_Integer;
                      }
                      else if(typeOfResult1 == DataType_Integer && typeOfResult2 == DataType_Double)
                      {
                            if(!RetypeToDouble(where1)) return DataType_Unknown;
                            return DataType_Integer;
                      }
                      else if(typeOfResult1 == DataType_Double && typeOfResult2 == DataType_Integer)
                      {
                            if(!RetypeToDouble(where2)) return DataType_Unknown;
                            return DataType_Integer;
                      }
                      else if(typeOfResult1 == DataType_Integer && typeOfResult2 == DataType_Integer)
                      {
                            if(!RetypeToDouble(where1)) return DataType_Unknown;
                            if(!RetypeToDouble(where2)) return DataType_Unknown;
                            return DataType_Integer;
                      }
                      else
                      {
                            // error
                            return DataType_Unknown;
                      }
                      break;


            default: RaiseError("invalid operator", temp_from->data, ErrorType_Semantic1);
                     return DataType_Unknown;
        }
    }
    else    // not variable or constant or operator -> error
    {
        RaiseError("token type is not constant or variable or operator", (*from)->data, ErrorType_Semantic1);
        *from = (*from)->next;
        return DataType_Unknown;
    }
}

bool ExpressionEnd()
{
    StackItem ** where = NULL;        //place where to put a retype token
    StackItem * from;                 //where in the stack i currently am
    from = mstack->first;
    DataType typeOfResult;

    //retyping
    typeOfResult = RetypeRecursive(&where, &from);
    if(typeOfResult == DataType_Unknown)
        return false;

    //otoceni stacku

    return true;
}

bool P_HandleOperand(Phrasem p)
{
  #ifdef PEDANT_DEBUG
    debug("Phrasem recieved by operand");
    PrintPhrasem(p);
  #endif
  if(p == NULL)
  {
    // error
    // ...
    return false;
  }

  if(mstack == NULL)
  {
    mstack = InitStack();
  }

  //return PushOntoStack(mstack, p);

  // conversion infix->postfix here??
  // ...

  // how to control?
  //if(!PushOntoStack(s, p)) return false;
  //printfstack(s);
  //printf("Token: %d\n", p->table);

  return true;
}
