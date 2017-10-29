
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

extern void ClearTables();
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
    setErrorLine(line);
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

bool VariableParse(Phrasem p)
{
  #ifdef PARSER_DEBUG
    debug("Variable parse.");
  #endif
  if(p == NULL) return false;

  if(p->table != TokenType_Symbol)
  {
    EndParser("Parser: VariableParse: variable name expected", p->line, ErrorType_Internal);
    return false;
  }

  // retyping
  p->table = TokenType_Variable;
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

bool ExpressionParse(Phrasem p)
{
  #ifdef PARSER_DEBUG
    debug("Expression parse.");
  #endif
  if(p != NULL)
  {
    // first phrasem given
  }

  // parsing expression
  return true;
}

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
    if(!ExpressionParse(NULL))
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
    if(!ExpressionParse(NULL)) return false;
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
      if(!ExpressionParse(q)) return false;
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
  /*if((p->table != TokenType_Operator) || (p->d.str != isOperator(";")))
  {
    return false;
  }*/
  free(p);

  PrintParse(false);

  return true;
}

bool LineParse()
{
  #ifdef PARSER_DEBUG
    debug("Line parse.");
  #endif
  Phrasem p = RemoveFromQueue();
  if(p == NULL) {
    EndParser("Parser: RunParser: queue error", p->line, ErrorType_Internal);
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
    case TokenType_Constant:
      EndParser("Parser: LineParse: line beginning with constant", p->line, ErrorType_Syntax);
      free(p);
      return false;

    // empty line
    case TokenType_Separator:
      break;
    case TokenType_Variable:
      // assignment
      break;
    case TokenType_Function:
      // function call
      break;
    case TokenType_Operator:
      // can be??
      break;

    default:
      EndParser("Parser: Unknown token type", p->line, ErrorType_Syntax);
      free(p);
      return false;
  }
  // here will be syntax analysis ---------------------------

  #ifdef PARSER_DEBUG
    PrintPhrasem(p);
  #endif

  free(p);
  return true;
}

bool RunParser()
{
  #ifdef PARSER_DEBUG
    debug("Init Parser.");
  #endif

  InitQueue();

  // running scanner
  pthread_create(&sc, NULL, InitScanner, NULL);

  // reading cycle
  while(ScannerIsScanning() && LineParse())
  {
    // something to do after each line
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
