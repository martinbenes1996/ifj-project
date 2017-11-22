
#include <stdlib.h>

#include "functions.h"
#include "list.h"


Parameters paramInit() { return NULL; }

bool paramAdd(Parameters * p, const char * name, DataType dt)
{
  struct paramFce * first = (*p);
  struct paramFce * newparam = malloc( sizeof(struct paramFce) );
  if(newparam == NULL) return false;

  newparam->type = dt;
  newparam->name = strdup(name);
    if(newparam->name == NULL) return false;
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

DataType paramPop(Parameters * p)
{
  // control for NULL
  if((*p) == NULL) return DataType_Unknown;

  // first set
  struct paramFce * pom;
  pom = (*p);

  // next
  (*p) = (*p)->next;
  // get datatype
  DataType dt = pom->type;
  // free memory
  if(pom->name != NULL) free(pom->name);
  free(pom);
  // return
  return dt;
}

inline bool ParametersMatches(Parameters p1, Parameters p2)
{
  struct paramFce * ptr1 = p1;
  struct paramFce * ptr2 = p2;
  // all parameters
  while((ptr1 != NULL) && (ptr2 != NULL))
  {
    // not matching
    if(ptr1->type != ptr2->type) return false;

    // next
    ptr1 = ptr1->next;
    ptr2 = ptr2->next;
  }

  return (ptr1 == NULL) && (ptr2 == NULL);
}
