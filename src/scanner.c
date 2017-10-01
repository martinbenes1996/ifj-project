
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "err.h"
#include "io.h"
#include "scanner.h"
#include "queue.h"

static bool isscanning = true;
bool ScannerIsScanning() { return isscanning; }
static QueueTail queue;

bool InitScannerQueue()
{
  queue = malloc( sizeof(struct queue_tail) );
  if(queue == NULL) return false;

  queue->last = NULL;
  return true;
}

void *InitScanner(void * v /*ignore this warning*/)
{

  #ifdef SCANNER_DEBUG
    debug("Init Scanner.");
  #endif

  if(!InitScannerQueue())
  {
    setErrorType(ErrorType_Internal);
    setErrorMessage("Scanner: InitScannerQueue: couldn't allocate memory");
    FinishConnectionToQueue();
    return NULL;
  }

  int input;
  while((input = getchar()) != EOF)
  {
    Phrasem phr = malloc( sizeof(struct phrasem_data) );
    if( phr == NULL )
    {
      setErrorType(ErrorType_Internal);
      setErrorMessage("Scanner: InitScanner: couldn't allocate memory");
      FinishConnectionToQueue();
      return NULL;
    }

    phr->id = input;
    fprintf(stderr, "%d\n", input);
    AddToQueue(queue, phr);
  }

  isscanning = false;
  #ifdef SCANNER_DEBUG
    debug("End Scanner.");
  #endif
  FinishConnectionToQueue();
  free(queue);
  return NULL;
}
