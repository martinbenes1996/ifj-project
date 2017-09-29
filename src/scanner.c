
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "io.h"
#include "scanner.h"
#include "queue.h"

int errcode;
int isEnd() { return errcode; }
static QueueTail queue;

bool InitScannerQueue()
{
  errcode = 0;
  queue = malloc( sizeof(struct queue_tail) );
  if(queue == NULL) return false;

  queue->last = NULL;
  return true;
}

void *InitScanner(void * v)
{
  #ifdef SCANNER_DEBUG
    debug("Init Scanner.");
  #endif

  if(!InitScannerQueue())
  {
    errcode = ALLOC_ERR;
    return NULL;
  }

  int input;
  while((input = getchar()) != EOF)
  {
    Phrasem phr = malloc( sizeof(struct phrasem_data) );
    if( phr == NULL )
    {
      errcode = ALLOC_ERR;
      return NULL;
    }

    phr->id = input;
    //fprintf(stderr, "%d\n", input);
    AddToQueue(queue, phr);
  }

  #ifdef SCANNER_DEBUG
    debug("End Scanner.");
  #endif
  free(queue);
  return NULL;
}
