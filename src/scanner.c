
/**
 * @file scanner
 * @authors xbenes49 xbolsh00 xkrato47 xpolan09
 * @date 2nd october 2017
 * @brief Lexical Scanner module.
 *
 * This module implements Lexical Scanner.
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "err.h"
#include "io.h"
#include "scanner.h"
#include "queue.h"
#include "types.h"

static bool isscanning = true;
bool done = false;
int line = 1;

bool ScannerIsScanning() { return isscanning; }

void EndScanner(const char * msg, ErrorType errtype)
{
  if(msg != NULL)
  {
    #ifdef SCANNER_DEBUG
      debug(msg);
    #endif
    setErrorType(errtype);
    setErrorMessage(msg);
  }
  else
  {
    #ifdef SCANNER_DEBUG
      debug("End Scanner.");
    #endif
  }
  FinishConnectionToQueue();
  isscanning = false;
}
/*
getString(int input) {
  int state = 1;

  switch (state) {
    case 1:
      if (input == '"') {state = 2; break;}

    case 2: 
      if(input == '/') {
        returnByte(input);
          if (input != '/') {
            getByte(input);
            state = 3;

          }
          else { AddToBuffer(input); getByte(input);} 
        break;
      }
    
    case 3:
      if(input != '"') {AddToBuffer(input); break;}
  }

}
*/
void *InitScanner(void * v /*not used*/)
{
  (void)v; /* to avoid compiler warning */

  #ifdef SCANNER_DEBUG
    debug("Init Scanner.");
  #endif

  int input;
  while(!done)
  {
    input = getByte();
    Phrasem phr = malloc( sizeof(struct phrasem_data) );
    if( phr == NULL )
    {
      EndScanner("Scanner: InitScanner: couldn't allocate memory", ErrorType_Internal);
      return NULL;
    }

    // here will be lexical analysis ----------------------------------
/*
    if (input == '!') {
      getString(input);
    }
*/
    phr->d.index = input;

    #ifdef SCANNER_DEBUG
      PrintPhrasem(phr);
    #endif

    AddToQueue(phr);
  }

  EndScanner(NULL, ErrorType_Ok);
  return NULL;
}
