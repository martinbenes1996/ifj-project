
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

pthread_t sc;
long function_id; /**< Id of actual function (for symbol table). */

#define END_IF 0x01
#define END_SCOPE 0x02
#define END_FUNCTION 0x04
#define END_LOOP 0x08
char end_type = 0;

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
#define CheckQueue(phrasem) RemoveFromQueue(); if(phrasem == NULL) { RaiseQueueError(phrasem); }

/*---------------------------- CLEAR --------------------------------*/
/**
 * @brief   Ends parser.
 *
 * This function ends scanner, and it will sets the error message to print,
 * if it is NULL, it will end well. It also deallocates queue and symbol table.
 * @param msg       Message to print.
 * @param errtype   Error type.
 */
void EndParser(const char * msg, int line, ErrorType errtype);

/*------------------------ CONDITION SIMPLIFIERS ---------------------*/

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

/*-------------------------- LOW LEVEL PARSER FUNCTIONS --------------------*/
/**
 * @brief   Parses symbol to variable.
 *
 * This function will get the parameter, it will check, if it is symbol,
 * then it retypes it to variable and saves to symbol table.
 * @param p       Phrasem with variable.
 * @returns True, if success, false otherwise.
 */
bool VariableParse(Phrasem p);

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

/*------------------ HIGH LEVEL PARSER FUNCTIONS ---------------------------*/

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
  // write to symbol table
  // ...
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

bool ExpressionParse()
{
  #ifdef PARSER_DEBUG
    debug("Expression parse.");
  #endif

  // parsing expression

  return true;
}

bool LogicParse()
{
  #ifdef PARSER_DEBUG
    debug("Expression parse.");
  #endif

  ExpressionParse();

  // parse the sign

  ExpressionParse();

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
  Sem_VariableDefined(sc, p->d.str);

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

  if( Sem_VariableDefined(function_id, p->d.str) )
  {
    // assignment
  }
  else if( Sem_FunctionDefined(p->d.str) )
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
  return BlockParse();
}

bool FunctionDeclarationParse()
{
  #ifdef PARSER_DEBUG
    debug("Declaration parse.");
  #endif

  // keyword function
  CheckKeyword("function");

  return true;
}

bool CycleParse()
{
  #ifdef PARSER_DEBUG
    debug("Cycle parse.");
  #endif

  if(!LogicParse())
  {
    return false;
  }

  CheckKeyword("then");

  // LF
  CheckSeparator();

  // cycle body
  BlockParse();


  return true;
}

bool FunctionDefinitionParse()
{
  #ifdef PARSER_DEBUG
    debug("Function definition parse.");
  #endif



  return true;
}

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

  }
  // function definition
  else if(matchesKeyword(p, "function"))
  {

  }
  // function definition
  else if(matchesKeyword(p, "scope"))
  {

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
  } while(end_type == 0);

  // end function
  Phrasem q = CheckQueue(q);
  if( !EndFunctionParse(q) )
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
