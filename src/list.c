
#include <stdlib.h>

#include "list.h"


Parameters paramInit() { return NULL; }

bool paramAdd(Parameters * p, DataType dt)
{
  struct paramFce * first = (*p);
  struct paramFce * newparam = malloc( sizeof(struct paramFce) );
  if(newparam == NULL) return false;

  newparam->type = dt;
  newparam->name = NULL;
  newparam->next = first;

  p = &newparam;
  return true;
}

void paramFree(struct paramFce * parameter)
{
    struct paramFce * pom;

    while(parameter != NULL)
    {
        pom = parameter;
        parameter = parameter->next;
        free(pom);
    }
    #ifdef SYMTABLE_DEBUG
        debug("Freeing parameters.");
    #endif
}
