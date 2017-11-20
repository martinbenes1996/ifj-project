
#include <stdlib.h>

#include "collector.h"
#include "functions.h"
#include "io.h"

static void ** mptrs = NULL;
static size_t msize = 0;
static size_t mcapacity = 0;

Phrasem allocPhrasem()
{
  #ifdef COLLECTOR_DEBUG
    debug("Collector, alloc phrasem.");
  #endif

  // first call
  if(mptrs == NULL)
  {
    #ifdef COLLECTOR_DEBUG
      debug("Alloc collector.");
    #endif
    mcapacity = 100;
    msize = 0;
    mptrs = malloc(sizeof(Phrasem)*mcapacity);
    if(mptrs == NULL) return NULL;
  }

  // too small
  if((msize + 1) >= mcapacity)
  {
    mcapacity += 100;
    #ifdef COLLECTOR_DEBUG
      debug("Realloc collector to %d.", mcapacity);
    #endif

    mptrs = realloc(mptrs, sizeof(Phrasem)*mcapacity);
    if(mptrs == NULL) return NULL;
  }

  // allocate the phrasem
  Phrasem p = malloc(sizeof(struct phrasem_data));
  if(p == NULL) return NULL;

  // saves the pointer to the inner array, increment msize and returns the pointer
  return mptrs[msize++] = p;

}

void freeCollector()
{
  #ifdef COLLECTOR_DEBUG
    debug("Free collector.", msize+1);
  #endif
  for(size_t i = 0; i < msize; i++)
  {
    freePhrasem(mptrs[i]);
  }

  free(mptrs);
}
