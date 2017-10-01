
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

static QueueHead qh;


void ResetParserQueue(QueueItem qi) { qh->first = qi; }

bool RunParser()
{
  #ifdef SCANNER_DEBUG
    debug("Init Parser.");
  #endif

  qh = InitQueue();
  if(qh == NULL)
  {
    setErrorType(ErrorType_Internal);
    setErrorMessage("Parser: RunParser: couldn't allocate memory");
    return false;
  }

  pthread_t sc;
  pthread_create(&sc, NULL, InitScanner, NULL);

  Phrasem p;
  while(ScannerIsScanning())
  {
    p = RemoveFromQueue(qh);
    if(p == NULL) {
      setErrorType(ErrorType_Internal);
      setErrorMessage("Parser: RunParser: queue error");
      return false;
    }

    fprintf(stderr, "%d ", p->id);
  }
  fprintf(stderr, "\n");

  pthread_join(sc, NULL);

  PrintQueue(qh);

  ClearQueue(qh);
  free(qh);

  return true;
}
