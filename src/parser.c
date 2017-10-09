
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
#include "queue.h"
#include "scanner.h"
#include "stack.h"
#include "tables.h"

pthread_t sc;
static Phrasem p;

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

  free(p);
  ClearQueue();
  ClearStack();
  //ClearTables();
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
  while(ScannerIsScanning())
  {
    p = RemoveFromQueue();
    if(p == NULL) {
      EndParser("Parser: RunParser: queue error", p->line, ErrorType_Internal);
      return false;
    }
    else if(p == END_PTR) break;

    switch(p->table)
    {
      case TokenType_Keyword:
        if(p->d.index == isKeyword("if"))
        {
          // condition
        }
        else if(p->d.index == isKeyword("while"))
        {
          // while cycle
        }
        else if(p->d.index == isKeyword("function"))
        {
          // function
        }
        else if(p->d.index == isKeyword("declare"))
        {
          // declaration
        }
        else if(p->d.index == isKeyword("dim"))
        {
          // variable
        }
        else if(p->d.index == isKeyword("scope"))
        {
          // main
        }

        // etc ...

        else
        {
          EndParser("Parser: Syntax error", p->line, ErrorType_Syntax);
          return false;
        }
        break;
      case TokenType_Constant:
        EndParser("Parser: Syntax error", p->line, ErrorType_Syntax);
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
        return false;
    }
    // here will be syntax analysis ---------------------------

    #ifdef PARSER_DEBUG
      PrintPhrasem(p);
    #endif

    free(p);
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
