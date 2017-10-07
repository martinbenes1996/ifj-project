
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

void EndParser(const char * msg, ErrorType errtype)
{
  if(msg != NULL)
  {
    #ifdef PARSER_DEBUG
      debug(msg);
    #endif
    setErrorType(errtype);
    setErrorMessage(msg);
  }
  else
  {
    #ifdef PARSER_DEBUG
      debug("Ending Parser.");
    #endif
    setErrorType(ErrorType_Ok);
    setErrorMessage("");
  }
}

bool RunParser()
{
  #ifdef PARSER_DEBUG
    debug("Init Parser.");
  #endif

  InitQueue();

  // running scanner
  pthread_t sc;
  pthread_create(&sc, NULL, InitScanner, NULL);

  // reading cycle
  Phrasem p;
  while(ScannerIsScanning())
  {
    p = RemoveFromQueue();
    if(p == NULL) {
      EndParser("Parser: RunParser: queue error", ErrorType_Internal);
      return false;
    }
    else if(p == END_PTR) break;

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

  EndParser(NULL, ErrorType_Ok);

  // end
  return true;
}
