
/**
 * @file parser.c
 * @authors xbenes49 xbolsh00 xkrato47 xpolan09
 * @date 2nd october 2017
 * @brief Syntax parser module.
 *
 * This module implements Syntax Analyser.
 */

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "err.h"
#include "io.h"
#include "parser.h"
#include "pedant.h"
#include "queue.h"
#include "scanner.h"
#include "tables.h"
#include "stack.h"

pthread_t sc;
long function_id = -1; /**< Id of actual function (for symbol table). */

bool end = false; /**< Set to true, if keyword end reached. */

/*-------------------------- ERROR MACROS --------------------------------*/
/**
 * @brief  Error raiser.
 *
 * This macro writes debug info to err function. It also calls EndParser
 * and returns false from a function, where it is used.
 * @param msg         Message to write.
 * @param phrasem     Bad phrasem.
 * @param errtype     Type of error.
 */
#define RaiseError(msg, phrasem, errtype)                       \
  do {                                                          \
    err("%s: %s: l.%d: %s", __FILE__, __func__, __LINE__, msg); \
    EndParser(msg, phrasem ->line, errtype);                    \
    free(phrasem);                                              \
    return false;                                               \
  } while(0)

/**
 * @brief Queue error raiser.
 *
 * This macro raises specialized Queue error(when it returns NULL pointer).
 * @param phrasem     Target memory.
 */
#define RaiseQueueError(phrasem)                                \
  do {                                                          \
    RaiseError("queue error", phrasem, ErrorType_Internal);     \
  } while(0)

/**
 * @brief -expected- error raiser.
 *
 * This macro raises specialized error -what expected-.
 * @param what        Token expected (in quotes)
 * @param phrasem     Target memory.
 */
#define RaiseExpectedError(what, phrasem)                       \
  do {                                                          \
    RaiseError(what " expected", phrasem, ErrorType_Syntax);    \
  } while(0)

/**
 * @brief checks separator in the queue
 *
 * This macro raises specialized error -separator expected-.
 * @param phrasem     Target memory.
 */
#define CheckSeparator()                                      \
  do {                                                        \
    Phrasem p = CheckQueue(p);                                \
    if(!isSeparator(p))                                       \
    {                                                         \
      RaiseExpectedError("separator", p);                     \
    }                                                         \
    free(p);                                                  \
  } while(0)

/**
 * @brief checks operator in the queue
 *
 * This macro checks, if there is operator in the queue.
 * It raises error, if it isn't.
 * @param op          Operator.
 */
#define CheckOperator(op)                                     \
  do {                                                        \
    Phrasem p = CheckQueue(p);                                \
    if(!isOperator(p, op))                                    \
    {                                                         \
      RaiseExpectedError("operator \'" op "\'", p);           \
    }                                                         \
    free(p);                                                  \
  } while(0)

/**
 * @brief checks separator in the queue
 *
 * This macro raises specialized error -separator expected-.
 * @param phrasem     Target memory.
 */
#define CheckKeyword(kw)                                      \
  do {                                                        \
    Phrasem p = CheckQueue(p);                                \
    if(!matchesKeyword(p, kw))                                \
    {                                                         \
      RaiseExpectedError("keyword \'" kw "\'", p);            \
    }                                                         \
    free(p);                                                  \
  } while(0)

/**
 * @brief Secure Queue getter.
 *
 * This function will call RemoveFromQueue. And if it returns NULL,
 * it will raise the queue error (with macro RaiseQueueError).
 * @param phrasem       Target memory.
 * @returns phrasem reached
 */
#define CheckQueue(phrasem) RemoveFromQueue();  \
      if(phrasem == NULL) {                     \
        if(!ScannerIsScanning()) {              \
          return false;                         \
        }                                       \
        RaiseQueueError(phrasem);               \
      }

/*----------------------------------------------------------------*/
/** @addtogroup Parser_tools
 * Parser tools.
 * @{
 */

/*------------- CONDITION SIMPLIFIERS -------------*/
bool isOperator(Phrasem p, const char * op)
{
  (void)op;
  return (p->table == TokenType_Operator) /*&& (p->d.index == getOperatorId(op))*/;
}

bool isSeparator(Phrasem p)
{
  return p->table == TokenType_Separator;
}



bool matchesKeyword(Phrasem p, const char * kw)
{
  return (p->table == TokenType_Keyword) && (p->d.index == isKeyword(kw));
}

/*-------------------- CLEAR ----------------------*/


/**
 * @brief   Ends parser.
 *
 * This function ends scanner, and it will sets the error message to print,
 * if it is NULL, it will end well. It also deallocates queue and symbol table.
 * @param msg       Message to print.
 * @param errtype   Error type.
 */
void EndParser(const char * msg, int line, ErrorType errtype);

/** @} */
/*-----------------------------------------------------------*/
/** @addtogroup Lowlevel_parsers
 * Syntax parser help functions.
 * @{
 */

/**
 * @brief   Parses symbol to variable.
 *
 * This function will get the parameter, it will check, if it is symbol,
 * then it retypes it to variable.
 * @param p       Phrasem with variable.
 * @returns True, if success, false otherwise.
 */
bool VariableParse(Phrasem p);

/**
 * @brief   Parses symbol to function.
 *
 * This function will get the parameter, it will check, if it is symbol,
 * then it retypes it to function.
 * @param p       Phrasem with function.
 * @returns True, if success, false otherwise.
 */
bool FunctionParse(Phrasem p);

/**
 * @brief   Checks, if phrasem is datatype keyword.
 *
 * @param p       Phrasem with keyword.
 * @returns True, if it is. False if not.
 */
bool DataTypeParse(Phrasem p);

/**
 * @brief   Parses expression.
 *
 * This function reads queue and parses input expression.
 * @returns True, if success. False otherwise.
 */
bool ExpressionParse();

/**
 * @brief   Parses condition.
 *
 * This function will parse the condition inside loop and if statement.
 * @returns True, if success. False otherwise.
 */
bool LogicParse();

/** @} */
/*-----------------------------------------------------------*/
/** @addtogroup Highlevel_parsers
 * Syntax parser functions.
 * @{
 */

/**
 * @brief   Parses variable definition.
 *
 * This function will parse the line with variable definition. It will write
 * it to symbol table too.
 * @returns True if success. False otherwise.
 */
bool VariableDefinitionParse();

/**
 * @brief   Parses input command.
 *
 * This function will parse the line with input command.
 * @returns True, if success. False otherwise.
 */
bool InputParse();

/**
 * @brief   Parses print command.
 *
 * This function will parse the line with print command.
 * @param first   Tells, if this is recursive call, or from outside (send true).
 * @returns True if success. False otherwise.
 */
bool PrintParse(bool first);

/**
 * @brief   Parses arguments of function call.
 *
 * This function will parse the arguments of function.
 * @returns True, if success. False otherwise.
 */
bool FunctionArgumentsParse();

/**
 * @brief   Parses line with symbol as first.
 *
 * This function will parse the line beginning with the symbol.
 * @returns True, if success. False otherwise.
 */
bool SymbolParse();

/**
 * @brief   Parses function declaration.
 *
 * This function will parse the line, containing the function declaration.
 * @returns True if success. False otherwise.
 */
bool FunctionDeclarationParse();

/**
 * @brief   Parses function declaration.
 *
 * This function will parse the block of function declaration.
 * @returns True if success. False otherwise.
 */
bool FunctionDefinitionParse();

/**
 * @brief   Parses scope.
 *
 * This function will parse the scope block (main).
 * @returns True if success. False otherwise.
 */
bool ScopeParse();

/**
 * @brief   Parses line/block with no prejustice.
 *
 * This function will parse the line/block inside the function and is
 * main state machine, which divides lines into parse functions.
 * @returns True, if line/block ended well. False otherwise.
 */
bool BlockParse();

/**
 * @brief   Parses global lines/blocks.
 *
 * This function will parse the global lines. It signs functions
 * to symbol table.
 * @returns True, if success. False otherwise.
 */
bool GlobalBlockParse();

/**
 * @brief   Parses cycle.
 *
 * This function will parse the cycle.
 * @returns True, if success. False otherwise.
 */
bool CycleParse();

/** @} */
/*----------------------------------------------------*/
/** @addtogroup End_parsers
 * Parsers of end lines.
 * @{
 */

/**
 * @brief   Parses end function.
 *
 * @returns True if success. False otherwise.
 */
bool EndFunctionParse();

/**
 * @brief   Parses end scope.
 *
 * @returns True if success. False otherwise.
 */
bool EndScopeParse();

/**
 * @brief   Parses end if.
 *
 * @returns True if success. False otherwise.
 */
bool EndIfParse();

/**
 * @brief   Parses end cycle.
 *
 * @returns True if success. False otherwise.
 */
bool EndCycleParse();

/** @} */
/*----------------------------- MAIN RUN ---------------------------*/

bool RunParser()
{
  #ifdef PARSER_DEBUG
    debug("Init Parser.");
  #endif

  InitQueue();

  // running scanner
  pthread_create(&sc, NULL, InitScanner, NULL);

  // reading cycle
  while(ScannerIsScanning())
  {
    if(!GlobalBlockParse())
    {
        // error
    }

    // something to do after each function
  }

  // ending scanner
  pthread_join(sc, NULL);

  // to be removed-----
  PrintQueue();
  ClearQueue();
  //------------------

  EndParser(NULL, -1, ErrorType_Ok);

  // end
  return true;
}

/*-------------------------- LOW LEVEL PARSER FUNCTIONS --------------------*/
bool VariableParse(Phrasem p)
{
  #ifdef PARSER_DEBUG
    debug("Variable parse.");
  #endif
  if(p == NULL)
  {
    RaiseExpectedError("variable", p);
  }

  if(p->table != TokenType_Symbol)
  {
    RaiseExpectedError("variable name", p);
  }

  // retyping
  p->table = TokenType_Variable;
  return true;
}

bool FunctionParse(Phrasem p)
{
  #ifdef PARSER_DEBUG
    debug("Function parse.");
  #endif
  if(p == NULL)
  {
    RaiseExpectedError("function", p);
  }

  if(p->table != TokenType_Symbol)
  {
    RaiseExpectedError("function name", p);
  }

  // retyping
  p->table = TokenType_Function;
  return true;
}

bool DataTypeParse(Phrasem p)
{
  #ifdef PARSER_DEBUG
    debug("Data type parse.");
  #endif
  if(p == NULL) return false;

  if(p->table != TokenType_Keyword) return false;

  return (matchesKeyword(p, "integer") || matchesKeyword(p, "double") || matchesKeyword(p, "string"));
}


/*-----------------expression parse----------------------------*/

/*
//returns an index into ExprParseArray
int decodeToken(...)
{
}
*/

#define MODIFY_STACK()              \
  do {                              \
    PopFromEPStack();               \
    PopFromEPStack();               \
    PopFromEPStack();               \
    PopFromEPStack();               \
    PushOntoEPStack('E');           \
  } while(0)

// can be shortened
//tries to perform stack modifications based on rules (called when '>')
int checkEPRules(Stack returnStack, Stack temporaryOpStack)
{
    #ifdef PARSER_DEBUG
        debug("EP: checkEPRules: executing reduction rules.");
    #endif

    if(LookTripleAheadEPStack(op_E, Add, op_E))
    {
        MODIFY_STACK();
        PushOntoStack(returnStack, PopFromStack(temporaryOpStack)); //it should pop an operator
        return 1;
    }
    else if(LookTripleAheadEPStack(op_E, Sub, op_E))
    {
        MODIFY_STACK();
        PushOntoStack(returnStack, PopFromStack(temporaryOpStack));
        return 1;
    }
    else if(LookTripleAheadEPStack(op_E, DivInt, op_E))
    {
        MODIFY_STACK();
        PushOntoStack(returnStack, PopFromStack(temporaryOpStack));
        return 1;
    }
    else if(LookTripleAheadEPStack(op_E, Mul, op_E))
    {
        MODIFY_STACK();
        PushOntoStack(returnStack, PopFromStack(temporaryOpStack));
        return 1;
    }
    else if(LookTripleAheadEPStack(op_E, DivDouble, op_E))
    {
        MODIFY_STACK();
        PushOntoStack(returnStack, PopFromStack(temporaryOpStack));
        return 1;
    }
    else if(LookTripleAheadEPStack(OpenBracket, op_E, CloseBracket))
    {
        MODIFY_STACK();
        return 1;
    }
    else if(LookOneAheadEPStack(op_i))
    {
        PopFromEPStack();
        PopFromEPStack();
        PushOntoEPStack(op_E);
        return 1;
    }
    else if(LookEndAheadEPStack())
    {
        return 0;       //empty stack
    }
    else return -1;     //cannot find a rule and it is not the end of expression
}



// Tahle funkce bude komplikovana, budu potrebovat spoustu pomocnych funkci a specialni zasobniky.
// Snad to nezruinuje tenhle krasne vypadajici kod.
bool ExpressionParse()
{
  #ifdef PARSER_DEBUG
    debug("Expression parse.");
  #endif

  //creating a stack that will be given to pedant for semantical EP analysis
  Stack returnStack = NULL;
  returnStack = InitStack();

  //creating a stack for storing operator tokens
  Stack temporaryOpStack = NULL;
  temporaryOpStack = InitStack();

  //determines what to do for every input
  char ExprParseArray[9][9] = {
       // +    -    \    *    /    (    )    i    $
/* + */ {'>', '>', '<', '<', '<', '<', '>', '<', '>'},
/* - */ {'>', '>', '<', '<', '<', '<', '>', '<', '>'},
/* \ */ {'>', '>', '>', '<', '<', '<', '>', '<', '>'},
/* * */ {'>', '>', '>', '>', '>', '<', '>', '<', '>'},
/* / */ {'>', '>', '>', '>', '>', '<', '>', '<', '>'},
/* ( */ {'<', '<', '<', '<', '<', '<', '=', '<', '#'},
/* ) */ {'>', '>', '>', '>', '>', '#', '>', '#', '>'},
/* i */ {'>', '>', '>', '>', '>', '#', '>', '#', '>'},
/* $ */ {'<', '<', '<', '<', '<', '<', '#', '<', '#'},
    };

    char x;
    Phrasem p;//pozdeji zrusit
    PushOntoEPStack(op_$);     //start of the stack

    //get token hopefully
    //Phrasem p = CheckQueue(p);

    while(1)
    {
        if(p->table == TokenType_Symbol || p->table == TokenType_Constant)
        {
            //if symbol ...
            //zeptat se symtable, jestli je to fce nebo prom
            // + zavolat variable/fce parser
            //pokud je to fce -> chyba (mozne rozsireni)
            // ...
            PushOntoStack(returnStack, p);
        }
        else if(p->table == TokenType_Operator)
        {
            //x is operation from array [top of stack][number of operator in token]
            x = ExprParseArray[ExprOnTopOfEPStack()][p->d.index];
            if(x == '<')
            {
                if(LookEAheadEPStack())         //E correction  <E+ ...
                {
                    PopFromEPStack();           //E
                    PushOntoEPStack(op_les);    //<
                    PushOntoEPStack(op_E);      //E
                    PushOntoEPStack(p->d.index);//operator
                    //p = CheckQueue(p);
                }
                else
                {
                    PushOntoEPStack(op_les);        //<
                    PushOntoEPStack(p->d.index);    //operator
                    PushOntoStack(temporaryOpStack, p);
                    //p = CheckQueue(p);
                }

            }
            else if(x == '>')
            {
                int pom;
                pom = checkEPRules(returnStack, temporaryOpStack);
                if(pom == 1)
                {
                    continue;       //successfuly managed to execute a rule
                }
                else if(pom == 0)
                {
                    //chyba
                }
                else    // pom == -1
                {
                    //chybove hlaseni
                }
            }
            else if(x == '=')
            {
                PushOntoEPStack(p->d.index);
                PushOntoStack(temporaryOpStack, p);
                //p = CheckQueue(p);
            }
            else    // x == '#'
            {
                //chybove hlaseni
            }
        }
        else    //it is not my symbol
        {
            //vratit token do fronty a vyprazdnit zasobnik + ukoncit
        }
    }





    return true;
}

bool LogicParse()
{
  #ifdef PARSER_DEBUG
    debug("Expression parse.");
  #endif

  if(!ExpressionParse())
  {
    // error
  }

  // parse the sign
  // ...

  if(!ExpressionParse())
  {
      // error
  }

  return true;
}

/*--------------------- ENDER FUNCTIONS -------------------*/

bool EndFunctionParse()
{
  #ifdef PARSER_DEBUG
    debug("End function parse.");
  #endif

  // keyword end
  CheckKeyword("end");

  // keyword function
  CheckKeyword("function");

  // separator
  CheckSeparator();

  end = false;
  return true;
}

bool EndIfParse()
{
  #ifdef PARSER_DEBUG
    debug("End function parse.");
  #endif

  // keyword end
  CheckKeyword("end");

  // keyword if
  CheckKeyword("if");

  // separator
  CheckSeparator();

  end = false;
  return true;
}

bool EndCycleParse()
{
  #ifdef PARSER_DEBUG
    debug("End function parse.");
  #endif

  // keyword loop
  CheckKeyword("loop");

  // separator
  CheckSeparator();

  end = false;
  return true;
}

bool EndScopeParse()
{
  #ifdef PARSER_DEBUG
    debug("End function parse.");
  #endif

  // keyword end
  CheckKeyword("end");

  // keyword scope
  CheckKeyword("scope");

  // separator
  CheckSeparator();

  end = false;
  return true;
}

/*------------------ HIGH LEVEL PARSER FUNCTIONS ---------------------------*/

bool VariableDefinitionParse()
{
  #ifdef PARSER_DEBUG
    debug("Variable definition parse.");
  #endif

  bool status = true;

  // getting symbol
  Phrasem p = CheckQueue(p);

  if(!VariableParse(p))
  {
    RaiseExpectedError("variable", p);
  }

  // getting keyword 'as'
  CheckKeyword("as");

  // getting datatype keyword
  Phrasem r = CheckQueue(r);

  if(!DataTypeParse(r))
  {
    RaiseExpectedError("datatype keyword", r);
  }

  // getting LF/=
  Phrasem s = CheckQueue(s);

  // LF
  if( isSeparator(s) )
  {
    // semantics - initialize to zero
  }
  // =
  else if(isOperator(s, "="))
  {
    // get expression
    if(!ExpressionParse())
    {
      RaiseError("invalid expression", s, ErrorType_Syntax);
    }

    // getting LF
    CheckSeparator();

  }
  else
  {
    // error
  }
  free(s);

  // write to symbol table
  // semantics
  return status;
}

bool InputParse()
{
  #ifdef PARSER_DEBUG
    debug("Input parse.");
  #endif

  // variable
  Phrasem p = CheckQueue(p);

  // error
  if(!VariableParse(p))
  {
    return false;
  }

  // control of previous definition
  if( !P_VariableDefined(sc, p->d.str)) return false;

  // LF
  CheckSeparator();

  // semantics call
  return true;
}

bool PrintParse(bool first)
{
  #ifdef PARSER_DEBUG
    debug("Print parse.");
  #endif
  // expression
  // first parameter
  if(first)
  {
    if(!ExpressionParse()) return false;
  }
  // non first print parameter (may not exist)
  else
  {
    Phrasem q = CheckQueue(q);

    if( isSeparator(q) )
    {
      free(q);
      return true;
    }

    else
    {
      if(!ReturnToQueue(q)) RaiseQueueError(q);

      if(!ExpressionParse()) return false;
    }
    free(q);
  }

  // ;
  CheckOperator(";");

  // recursive call of the same function
  return PrintParse(false);
}

bool FunctionArgumentsParse()
{
  #ifdef PARSER_DEBUG
    debug("Arguments parse.");
  #endif
  if(!ExpressionParse()) return false;

  Phrasem p = CheckQueue(p);

  if(isOperator(p, ","))
  {
    return FunctionArgumentsParse();
  }
  else if(isOperator(p, ")"))
  {
    if(!ReturnToQueue(p)) RaiseQueueError(p);
  }
  return true;
}

bool SymbolParse(Phrasem p)
{
  #ifdef PARSER_DEBUG
    debug("Symbol parse.");
  #endif

  if( P_VariableDefined(function_id, p->d.str) )
  {
    // assignment
  }
  else if( P_FunctionDefined(p->d.str) )
  {
    // '(' token
    CheckOperator("(");

    // arguments
    if(!FunctionArgumentsParse()) return false;

    // ')' token
    CheckOperator(")");

    // LF
    CheckSeparator();

  }
  else
  {
    // raise semantic error
  }

  return true;
}

bool BlockParse()
{
  #ifdef PARSER_DEBUG
    debug("Block parse.");
  #endif

  Phrasem p = CheckQueue(p);
  switch(p->table)
  {
    case TokenType_Keyword:
      // variable declaration/definition
      if(matchesKeyword(p, "dim"))
      {
        return VariableDefinitionParse();
      }
      // end
      else if(matchesKeyword(p, "end"))
      {
        ReturnToQueue(p);
        end = true;
        return true;
      }
      // condition
      else if(matchesKeyword(p, "if"))
      {
      }
      // printing
      else if(matchesKeyword(p, "print"))
      {
        PrintParse(true);
      }
      // loading
      else if(matchesKeyword(p, "input"))
      {
        InputParse();
      }
      // cycle
      else if(matchesKeyword(p, "do"))
      {
      }
      // end cycle
      else if(matchesKeyword(p, "loop"))
      {
      }
      // function return
      else if(matchesKeyword(p, "return"))
      {
      }
      // function declaration
      else if(matchesKeyword(p, "declare"))
      {
        RaiseError("declaring function inside function", p, ErrorType_Syntax);
      }
      // function definition
      else if(matchesKeyword(p, "function"))
      {
        RaiseError("defining function inside function", p, ErrorType_Syntax);
      }
      // main
      else if(matchesKeyword(p, "scope"))
      {
        RaiseError("defining scope inside function", p, ErrorType_Syntax);
      }
      // error
      else
      {
        RaiseError("unknown keyword", p, ErrorType_Syntax);
        free(p);
        return false;
      }
      break;
    // constant
    case TokenType_Constant:
    // operator
    case TokenType_Operator:
      EndParser("Parser: BlockParse: line beginning with constant", p->line, ErrorType_Syntax);
      free(p);
      return false;

    // empty line
    case TokenType_Separator:
      break;
    // assignment, function call
    case TokenType_Symbol:
      break;
    case TokenType_Variable:
      // assignment
      break;
    case TokenType_Function:
      // function call
      break;


    default:
      EndParser("Parser: Unknown token type", p->line, ErrorType_Syntax);
      free(p);
      return false;
  }

  #ifdef PARSER_DEBUG
    PrintPhrasem(p);
  #endif

  free(p);
  return true;
}

bool FunctionDeclarationParse()
{
  #ifdef PARSER_DEBUG
    debug("Declaration parse.");
  #endif

  // keyword function
  CheckKeyword("function");

  // function name
  Phrasem funcname = CheckQueue(funcname);
  if( !FunctionParse(funcname) ) return false;

  // operator (
  CheckOperator("(");

  // parameters declaration
  // ...

  // operator )
  CheckOperator(")");

  // LF
  CheckSeparator();

  return true;
}

bool CycleParse()
{
  #ifdef PARSER_DEBUG
    debug("Cycle parse.");
  #endif

  if(!LogicParse()) return false;

  // keyword then
  CheckKeyword("then");
  // LF
  CheckSeparator();

  // cycle body
  do {
    if(!BlockParse()) return false;
  } while(!end);

  if(!EndCycleParse()) return false;

  end = false;
  return true;
}

bool FunctionDefinitionParse()
{
  #ifdef PARSER_DEBUG
    debug("Function definition parse.");
  #endif

  // function name
  Phrasem funcname = CheckQueue(funcname);
  if( !FunctionParse(funcname) ) return false;

  // operator (
  CheckOperator("(");

  // parameters declaration
  // ...

  // operator )
  CheckOperator(")");

  // LF
  CheckSeparator();

  // check symbol table
  // in the semantics
  // ...

  CheckKeyword("begin");

  do {
      if(!BlockParse()) return false;
  } while(!end);

  // end function
  if(!EndFunctionParse()) return false;


  return true;
}

bool ScopeParse()
{
  if(function_id != -1)
  {
    EndParser("Parser: GlobalBlockParse: nested functions not supported", -1, ErrorType_Internal);
    return false;
  }

  // LF
  CheckSeparator();

  do {
    if(!BlockParse()) return false;
  } while(!end);

  // end scope parse
  EndScopeParse();

  return true;
}

bool GlobalBlockParse()
{
  #ifdef PARSER_DEBUG
    debug("Global block parse.");
  #endif

  if(function_id != -1)
  {
    EndParser("Parser: GlobalBlockParse: nested functions not supported", -1, ErrorType_Internal);
    return false;
  }

  // read function
  Phrasem p = CheckQueue(p);

  if(p->table != TokenType_Keyword) RaiseError("syntax error on global level", p, ErrorType_Syntax);

  // function declaration
  if(matchesKeyword(p, "declare"))
  {
    if(!FunctionDeclarationParse()) return false;
  }
  // function definition
  else if(matchesKeyword(p, "function"))
  {
    if(!FunctionDefinitionParse()) return false;
  }
  // function definition
  else if(matchesKeyword(p, "scope"))
  {
    if(!ScopeParse()) return false;
  }
  // error (global not supported)
  else
  {
    RaiseError("syntax error on global level", p, ErrorType_Syntax);
  }
  free(p);

  // body of function
  do {
    if(!BlockParse()) return false;
  } while(!end);

  // end function
  Phrasem q = CheckQueue(q);
  if(!EndFunctionParse(q))
  {
    RaiseExpectedError("end of function", q);
  }

  return true;
}

/*---------------------------- CLEAR --------------------------------*/
void EndParser(const char * msg, int line, ErrorType errtype)
{
  // end second thread
  AskScannerToEnd(); /**< Symetric end. Better assymetric. */
  pthread_join(sc, NULL);

  if(msg != NULL)
  {
    #ifdef PARSER_DEBUG
      debug(msg);
    #endif
    setErrorType(errtype);
    setErrorMessage(msg);
    if(line != -1) setErrorLine(line);
  }
  else
  {
    #ifdef PARSER_DEBUG
      debug("Ending Parser.");
    #endif
    setErrorType(ErrorType_Ok);
    setErrorMessage("");
  }

  ClearQueue();
  //ClearTables();
}
