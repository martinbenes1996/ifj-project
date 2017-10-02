
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

void *InitScanner(void * v /*ignore this warning*/)
{
  #ifdef SCANNER_DEBUG
    debug("Init Scanner.");
  #endif

  int input;
  while((input = getchar()) != EOF)
  {

    Phrasem phr = malloc( sizeof(struct phrasem_data) );
    if( phr == NULL )
    {
      EndScanner("Scanner: InitScanner: couldn't allocate memory", ErrorType_Internal);
      return NULL;
    }

    // here will be lexical analysis

    phr->id = input;

    #ifdef SCANNER_DEBUG
      PrintPhrasem(phr);
    #endif

    AddToQueue(phr);
  }

  EndScanner(NULL, ErrorType_Ok);
  return NULL;
}
