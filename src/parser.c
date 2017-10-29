
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


#define RaiseError(msg, phrasem, errtype)     \
  do {                                        \
    EndParser(msg, phrasem->line, errtype);\
    return false;                             \
  } while(0)


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
 * @brief   Checks, if phrasem is open bracket.
 *
 * @param p       Phrasem being checked.
 * @returns True, if success. False otherwise.
 */
bool OpenBracketParse(Phrasem p);

/**
 * @brief   Checks, if phrasem is close bracket.
 *
 * @param p       Phrasem being checked.
 * @returns True, if success. False otherwise.
 */
bool CloseBracketParse(Phrasem p);

/**
 * @brief   Parses expression.
 *
 * This function reads queue and parses input expression.
 * @returns True, if success. False otherwise.
 */
bool ExpressionParse();

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
bool LineParse();

/**
 * @brief   Parses global lines/blocks.
 *
 * This function will parse the global lines. It signs functions
 * to symbol table.
 * @returns True, if success. False otherwise.
 */
bool GlobalLineParse();

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
    if(!GlobalLineParse())
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
    RaiseError("Parser: VariableParse: variable expected", p, ErrorType_Syntax);
  }

  if(p->table != TokenType_Symbol)
  {
    EndParser("Parser: VariableParse: variable name expected", p->line, ErrorType_Internal);
    return false;
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

  return ((p->d.index == isKeyword("integer"))
        || (p->d.index == isKeyword("double"))
        || (p->d.index == isKeyword("string")));
}

bool OpenBracketParse(Phrasem p)
{
  if(p == NULL) {
    EndParser("Parser: OpenBracketParse: NULL pointer recieved", p->line, ErrorType_Internal);
    return false;
  }
  if(!isOperator(p, "(")) return false;

  free(p);
  return true;
}

bool CloseBracketParse(Phrasem p)
{
  if(p == NULL) {
    EndParser("Parser: CloseBracketParse: NULL pointer recieved", p->line, ErrorType_Internal);
    return false;
  }
  if(!isOperator(p, ")")) return false;

  free(p);
  return true;
}

bool ExpressionParse()
{
  #ifdef PARSER_DEBUG
    debug("Expression parse.");
  #endif

  // parsing expression

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
  Phrasem p = RemoveFromQueue();
  if(p == NULL)
  {
    EndParser("Parser: VariableDefinitionParse: queue error", p->line, ErrorType_Internal);
    return false;
  }
  if(!VariableParse(p))
  {
    // error
    status = false;
  }

  // getting keyword 'as'
  Phrasem q = RemoveFromQueue();
  if(q == NULL)
  {
    EndParser("Parser: VariableDefinitionParse: queue error", q->line, ErrorType_Internal);
    return false;
  }
  if( (q->table != TokenType_Keyword) || (q->d.index != isKeyword("as")) )
  {
    // error
    status = false;
  }
  free(q);

  // getting datatype keyword
  Phrasem r = RemoveFromQueue();
  if(r == NULL) {
    EndParser("Parser: VariableDefinitionParse: queue error", r->line, ErrorType_Internal);
    return false;
  }
  if(!DataTypeParse(r))
  {
    // error
  }

  // getting LF/=
  Phrasem s = RemoveFromQueue();
  if(s == NULL) {
    EndParser("Parser: VariableDefinitionParse: queue error", s->line, ErrorType_Internal);
    return false;
  }
  // LF
  if(s->table == TokenType_Separator)
  {
    // semantics - initialize to zero
  }
  // =
  else if((s->table == TokenType_Operator) && (s->d.index == isKeyword("=")))
  {
    // get expression
    if(!ExpressionParse())
    {
      // error
    }

    // getting LF
    Phrasem t = RemoveFromQueue();
    if(t == NULL) {
      EndParser("Parser: VariableDefinitionParse: queue error", t->line, ErrorType_Internal);
      return false;
    }
    if(t->table != TokenType_Separator)
    {
      // error
    }
    free(t);

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
  Phrasem p = RemoveFromQueue();
  if(p == NULL) {
    EndParser("Parser: InputParse: queue error", p->line, ErrorType_Internal);
    return false;
  }
  // error
  if(!VariableParse(p))
  {
    return false;
  }

  // control of previous definition
  Sem_VariableDefined(sc, p->d.str);

  // LF
  Phrasem q = RemoveFromQueue();
  if(q == NULL) {
    EndParser("Parser: InputParse: queue error", q->line, ErrorType_Internal);
    return false;
  }
  if(q->table != TokenType_Separator)
  {
    // error
    EndParser("Parser: InputParse: separator expected", q->line, ErrorType_Internal);
    return false;
  }
  free(q);

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
    Phrasem q = RemoveFromQueue();
    if(q == NULL) {
      EndParser("Parser: InputParse: queue error", q->line, ErrorType_Internal);
      return false;
    }
    if(q->table == TokenType_Separator)
    {
      free(q);
      return true;
    }
    else
    {
      if(!ReturnToQueue(q))
      {
        EndParser("Parser: InputParse: queue error", q->line, ErrorType_Internal);
        return false;
      }
      if(!ExpressionParse()) return false;
    }
    free(q);
  }

  // ;
  Phrasem p = RemoveFromQueue();
  if(p == NULL) {
    EndParser("Parser: InputParse: queue error", p->line, ErrorType_Internal);
    return false;
  }
  // error
  if(!isOperator(p, ";"))
  {
    return false;
  }

  free(p);

  // recursive call of the same function
  return PrintParse(false);
}

bool FunctionArgumentsParse()
{
  if(!ExpressionParse()) return false;

  Phrasem p = RemoveFromQueue();
  if(p == NULL) {
    EndParser("Parser: FunctionArgumentsParse: queue error", p->line, ErrorType_Internal);
    return false;
  }
  if(isOperator(p, ","))
  {
    return FunctionArgumentsParse();
  }
  else if(isOperator(p, ")"))
  {

    if(!ReturnToQueue(p))
    {
      EndParser("Parser: FunctionArgumentsParse: queue error", p->line, ErrorType_Internal);
      return false;
    }
  }
  return true;
}

bool SymbolParse(Phrasem p)
{

  if(/*isFunction(p->d.str)*/1)
  {
    Phrasem q = RemoveFromQueue();
    if(!OpenBracketParse(q)) {
      EndParser("Parser: SymbolParse: \'(\' token expected", p->line, ErrorType_Syntax);
      return false;
    }
    free(q);

    if(!FunctionArgumentsParse()) return false;

    Phrasem r = RemoveFromQueue();
    if(!CloseBracketParse(q)) {
      EndParser("Parser: SymbolParse: \')\' token expected", p->line, ErrorType_Syntax);
      return false;
    }
    free(r);

    Phrasem s = RemoveFromQueue();
    if(s) {
      EndParser("Parser: SymbolParse: \')\' token expected", p->line, ErrorType_Syntax);
      return false;
    }
  }
  else if(/*isVariable(p->d.str, function_id)*/1)
  {
    // assignment
  }

  return true;
}

bool LineParse()
{
  #ifdef PARSER_DEBUG
    debug("Line parse.");
  #endif
  Phrasem p = RemoveFromQueue();
  if(p == NULL) {
    EndParser("Parser: LineParse: queue error", p->line, ErrorType_Internal);
    return false;
  }

  switch(p->table)
  {
    case TokenType_Keyword:
      // variable declaration/definition
      if(p->d.index == isKeyword("dim"))
      {
        return VariableDefinitionParse();
      }
      // function declaration
      else if(p->d.index == isKeyword("declare"))
      {
      }
      // function definition
      else if(p->d.index == isKeyword("function"))
      {
      }
      // main
      else if(p->d.index == isKeyword("scope"))
      {
      }
      // end
      else if(p->d.index == isKeyword("end"))
      {
      }
      // condition
      else if(p->d.index == isKeyword("if"))
      {
      }
      // printing
      else if(p->d.index == isKeyword("print"))
      {
        PrintParse(true);
      }
      // loading
      else if(p->d.index == isKeyword("input"))
      {
        InputParse();
      }
      // cycle
      else if(p->d.index == isKeyword("do"))
      {
      }
      // end cycle
      else if(p->d.index == isKeyword("loop"))
      {
      }
      // function return
      else if(p->d.index == isKeyword("return"))
      {
      }
      // error
      else
      {
        EndParser("Parser: LineParse: unknown keyword", p->line, ErrorType_Syntax);
        free(p);
        return false;
      }
      break;
    // constant
    case TokenType_Constant:
    // operator
    case TokenType_Operator:
      EndParser("Parser: LineParse: line beginning with constant", p->line, ErrorType_Syntax);
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
  return true;
}

bool FunctionDefinitionParse()
{
  return true;
}

bool GlobalLineParse()
{
  if(function_id != -1)
  {
    EndParser("Parser: GlobalLineParse: nested functions not supported", -1, ErrorType_Internal);
    return false;
  }

  // read function
  Phrasem p = RemoveFromQueue();
  if(p == NULL) {
    EndParser("Parser: GlobalLineParse: queue error", p->line, ErrorType_Internal);
    return false;
  }
  if(p->table != TokenType_Keyword)
  {
    EndParser("Parser: GlobalLineParse: syntax error on global level", p->line, ErrorType_Internal);
    return false;
  }

  // function declaration
  if(p->d.index == isKeyword("declare"))
  {

  }
  // function definition
  else if(p->d.index == isKeyword("function"))
  {

  }
  // error (global not supported)
  else
  {
    EndParser("Parser: GlobalLineParse: syntax error on global level", p->line, ErrorType_Internal);
    return false;
  }
  free(p);

  do {
    if(!LineParse()) return false;
  } while(end_type == 0);
  return (end_type == END_FUNCTION);

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
