
#include "config.h"
#include "err.h"
#include "functions.h"
#include "generator.h"
#include "io.h"
#include "pedant.h"
#include "stack.h"
#include "symtable.h"
#include "types.h"

#include <stdio.h>
#include <stdlib.h>

void EndPedant(const char * msg, ErrorType errtype)
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
    setErrorLine(Config_getLine());
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
    EndPedant(msg, errtype);                                    \
    return false;                                               \
  } while(0)

/*------ DATA -------*/
static Stack mstack = NULL;
static DataType typeOfResult;
static DataType mdtmem = DataType_Unknown;      // memory of last stack datatype generated

// how to control [if a+1 < c-6 then] ??
//static Stack mstack2 = NULL;
//static DataType typeOfResult2;
/*--------------------*/

bool P_VariableDefined(Phrasem varname)
{
  #ifdef PEDANT_DEBUG
    debug("Pedant, Variable Defined?");
  #endif
  return findVariable(Config_getFunction(), varname->d.str);
}

bool P_DefineNewVariable(Phrasem varname, Phrasem datatype)
{
  #ifdef PEDANT_DEBUG
    debug("Pedant, Define New Variable");
  #endif

  DataType type = getDataType(datatype);
  if(type == DataType_Unknown)
  {
    RaiseError("unknown datatype", datatype, ErrorType_Semantic1);
  }

  if(!addVariable(Config_getFunction(), varname->d.str)
  || !addVariableType(Config_getFunction(), varname->d.str, type))
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
  return findFunctionInTable(funcname);
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
        if(pom->data->table == TokenType_Variable) printf("%s\n", pom->data->d.str);
        else if(pom->data->table == TypeCast_Double2Int) printf("double -> int\n");
        else if(pom->data->table == TypeCast_Int2Double) printf("int -> double\n");
        else if(pom->data->table == TokenType_Operator) printf("Oper: %d\n", pom->data->d.index);
            else printf("%d\n", pom->data->d.index);
        pom = pom->next;
    }
}
/**
 * @brief   Inserts a retype token into stack.
 *
 * @param where      double pointer to stackitem.
 * @returns True if success. False otherwise.
 */
bool RetypeToDouble(StackItem ** where)
{
    #ifdef PEDANT_DEBUG
        debug("Retyping phrasem to double");
    #endif
    Phrasem token;
    token = malloc(sizeof(struct phrasem_data));
    if(token == NULL)
    {
        RaiseError("Retypetodouble: could not allocate memory", *where, ErrorType_Internal);
        return false;
    }
    // allocation of new token
    token->table = TypeCast_Int2Double;

    StackItem * pom;
    pom = malloc(sizeof(StackItem));
    if(pom == NULL)
    {
        RaiseError("Retypetodouble: could not allocate memory", *where, ErrorType_Internal);
        return false;
    }
    // allocation of new stack item and linking the list
    pom->data = token;
    pom->next = *where;
    *where = pom;

    return true;
}
/**
 * @brief   Inserts a retype token into stack.
 *
 * @param where      double pointer to stackitem.
 * @returns True if success. False otherwise.
 */
bool RetypeToInt(StackItem ** where)
{
    #ifdef PEDANT_DEBUG
        debug("Retyping phrasem to int");
    #endif
    Phrasem token;
    token = malloc(sizeof(struct phrasem_data));
    if(token == NULL)
    {
        RaiseError("Retypetoint: could not allocate memory", *where, ErrorType_Internal);
        return false;
    }
    // allocation of new token
    token->table = TypeCast_Double2Int;

    StackItem * pom;
    pom = malloc(sizeof(StackItem));
    if(pom == NULL)
    {
        RaiseError("Retypetoint: could not allocate memory", *where, ErrorType_Internal);
        return false;
    }
    // allocation of new stack item and linking the list
    pom->data = token;
    pom->next = *where;
    *where = pom;

    return true;
}

#ifdef PEDANT_DEBUG
    static int debugRecursion = 0;
#endif
/**
 * @brief   Recursive retyping.
 * This function returns a type of operand or calls itself to find out the result of operation
 * it saves value of from into variable in expressionEnd function and where into variable in
 * the recursive parent function.
 * @param where     pointer to double pointer to stackitem.
 * @param from      pointer double pointer to stackitem, shows where in stack i currently am.
 * @returns type of subexpression if success. DataType_Unknown otherwise.
 */
DataType RetypeRecursive(StackItem *** where, StackItem *** from)
{
    #ifdef PEDANT_DEBUG
        for(int debugg = 0;debugg < debugRecursion;++debugg) {printf(" ");}
        printf("Rekurze stupne %d:\n", debugRecursion);
        PrintPhrasem((**from)->data);
        debugRecursion++;
    #endif

    if(*from == NULL) return DataType_Unknown;      //should not happen

    if((**from)->data->table == TokenType_Constant || (**from)->data->table == TokenType_Variable)
    {
        #ifdef PEDANT_DEBUG
            debugRecursion--;
        #endif
        // where is a pointer to previous stackitem->next (pointing to a current stackitem)
        // which i may modify by inserting retype token to the list (behind current token)
        *where = *from;

        //return data of the operand
        if((**from)->data->table == TokenType_Variable)
        {
            *from = &(**from)->next;          //set starting item to the next one for recursion
            return findVariableType(Config_getFunction(), (**where)->data->d.str);
        }

        else
        {
            *from = &(**from)->next;          //set starting item to the next one for recursion
            return findConstType((**where)->data->d.index);
        }
    }
    else if((**from)->data->table == TokenType_Operator)
    {
        Operators currentOperator = (**from)->data->d.index;
        *where = *from;
        *from = &(**from)->next;

        StackItem ** where1 = NULL;
        StackItem ** where2 = NULL;
        DataType typeOfResult1;
        DataType typeOfResult2;
        typeOfResult1 = RetypeRecursive(&where1, from);
        typeOfResult2 = RetypeRecursive(&where2, from);

        #ifdef PEDANT_DEBUG
            debugRecursion--;
        #endif

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
                            RaiseError("only one operand is string (add)", temp_from->data, ErrorType_Semantic2);
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
                            RaiseError("one operand is string (mul, sub)", temp_from->data, ErrorType_Semantic2);
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
                            RaiseError("one operand is string (divInt)", temp_from->data, ErrorType_Semantic2);
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
                            RaiseError("one operand is string (divDouble)", temp_from->data, ErrorType_Semantic2);
                            return DataType_Unknown;
                      }
                      else if(typeOfResult1 == DataType_Double && typeOfResult2 == DataType_Double)
                      {
                            // both doubles -> no need for retyping
                            return DataType_Double;
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
                      else if(typeOfResult1 == DataType_Integer && typeOfResult2 == DataType_Integer)
                      {
                            if(!RetypeToDouble(where1)) return DataType_Unknown;
                            if(!RetypeToDouble(where2)) return DataType_Unknown;
                            return DataType_Double;
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
        #ifdef PEDANT_DEBUG
            debugRecursion--;
        #endif
        RaiseError("token type is not constant or variable or operator", (**from)->data, ErrorType_Semantic1);
        *from = &(**from)->next;
        return DataType_Unknown;
    }
}
/**
 * @brief   Performs last operations upon expression before sending it to generator.
 * This function calls for retyping and turns the stack. It also sets global variable typeOfResult.
 *
 * @returns True if success. False otherwise.
 */
bool ExpressionEnd()
{
    StackItem ** where = NULL;        //place where to put a retype token
    StackItem ** from;                //where in the stack i currently am
    from = &mstack->first;

    //retyping
    typeOfResult = RetypeRecursive(&where, &from);
    if(typeOfResult == DataType_Unknown)
        return false;

    //turning of the stack
    mstack = TurnStack(mstack);

    #ifdef PEDANT_DEBUG
        debug("Expression processed, saved in pedant.");
    #endif

    return true;
}

#define TRY_PUSH_ONTO_STACK                         \
    if(PushOntoStack(mstack, p)) return true;       \
    else                                            \
    {                                               \
        freePhrasem(p);                             \
        return false;                               \
    }
/**
 * @brief   Token handling.
 * This function pushes received tokens onto a stack and checks if all symbols and constants are properly defined.
 * @param p     Received phrasem.
 * @returns true if success. false otherwise.
 */
bool P_HandleOperand(Phrasem p)
{
  #ifdef PEDANT_DEBUG
    debug("Phrasem recieved by operand");
    PrintPhrasem(p);
  #endif
  if(p == NULL)
  {
    RaiseError("Handle_operand: received a NULL pointer instead of token", p, ErrorType_Internal);
    return false;
  }

  if(mstack == NULL)
  {
    mstack = InitStack();
  }

  // checking if symbols are defined and retyping tokens
  if(p->table == TokenType_Symbol)
  {
    char * functionName;
    functionName = Config_getFunction();

    // checks if the token contains a name of function
    if(findFunctionInTable(p->d.str))
    {
        RaiseError("Handle_operand: function as an operand in expression! not allowed", p, ErrorType_Semantic1);
        freePhrasem(p);
        return false;
    }
    // checks if the token constains defined variable with known type
    if(findVariable(functionName, p->d.str))
    {
        p->table = TokenType_Variable;
        if(findVariableType(functionName, p->d.str) != DataType_Unknown)
        {
            TRY_PUSH_ONTO_STACK;
        }
        else
        {
            RaiseError("Handle_operand: variable has type of -> DataType_Unknown <-", p, ErrorType_Semantic1);
            freePhrasem(p);
            return false;
        }
    }
    else
    {
        RaiseError("Handle_operand: variable not found in symbol table", p, ErrorType_Semantic1);
        freePhrasem(p);
        return false;
    }
  }
  else if(p->table == TokenType_Constant)
  {
    if(findConstType(p->d.index) != DataType_Unknown)
    {
        TRY_PUSH_ONTO_STACK;
    }
    else
    {
        RaiseError("Handle_operand: constant is of an undefined type", p, ErrorType_Semantic1);
        freePhrasem(p);
        return false;
    }
  }
  else if(p->table == TokenType_Operator)
  {
        TRY_PUSH_ONTO_STACK;
  }
  else      // neither operand nor operator
  {
        RaiseError("token type is not constant or symbol or operator", p, ErrorType_Semantic1);
        freePhrasem(p);
        return false;
  }

  return true;
}

bool P_HandleTarget(Phrasem p)
{
  #ifdef PEDANT_DEBUG
    debug("Pedant, handle target!");
  #endif
  DataType target_dt = findVariableType(Config_getFunction(), p->d.str);
  DataType source_dt = typeOfResult;

  #ifdef TYPECAST_DEBUG
    PrintDataType(target_dt);
    PrintDataType(source_dt);
  #endif


  // same type
  if(target_dt == source_dt)
  {
    #ifdef TYPECAST_DEBUG
      debug("No typecast.");
    #endif
    if( !Send(mstack) ) return false;
    if( !HandlePhrasem(p) ) return false;
  }

  // source double->int
  else if((target_dt == DataType_Integer)
       && (source_dt == DataType_Double))
  {
    #ifdef TYPECAST_DEBUG
      debug("Source: [double]->[int].");
    #endif
    if( !Send(mstack) ) return false;
    GenerateTypeCast(TypeCast_Double2Int);
    if( !HandlePhrasem(p) ) return false;
  }

  // source int->double
  else if((target_dt == DataType_Double)
       && (source_dt == DataType_Integer))
  {
    #ifdef TYPECAST_DEBUG
      debug("Source: [int]->[double].");
    #endif
    if( !Send(mstack) ) return false;
    GenerateTypeCast(TypeCast_Int2Double);
    if( !HandlePhrasem(p) ) return false;
  }

  // error
  else RaiseError("incompatible type", p, ErrorType_Semantic2);

  return true;
}

bool P_HandleCompareOperator(Phrasem p) {
  #ifdef PEDANT_DEBUG
    debug("Pedant, handle compare operator.");
  #endif
  DataType generated = mdtmem;
  DataType waiting = typeOfResult;

  if (generated == waiting) {
    if (!Send(mstack)) return false;
    if (!HandlePhrasem(p)) return false;
    return true;
  }

  else if ((generated == DataType_Integer)&&(waiting == DataType_Double)) {
    GenerateTypeCast(TypeCast_Int2Double);
    if (!Send(mstack)) return false;
    if (!HandlePhrasem(p)) return false;
    return true;
  }

  else if((generated == DataType_Double)&&(waiting == DataType_Integer)) {
    if(!Send(mstack)) return false;
    GenerateTypeCast(TypeCast_Int2Double);
    if(!HandlePhrasem(p)) return false;
    return true;
  }

  else RaiseError("incompatible types", p, ErrorType_Semantic2);

}



bool P_MoveStackToGenerator()
{
  mdtmem = typeOfResult;
  return Send(mstack);
}
