
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include "buffer.h"

static char *buf = NULL;
static long iter = -1, max = -1;

// ---------------- GetBuffer -----------------
char * GetBuffer()
{
  if (buf == NULL) return calloc(1,sizeof(char)); //vrat prazdny

  AddToBuffer(0); // na konci 0

  buf = realloc(buf, iter); // presne setjne velky

  char * svbuf = buf;
  buf = NULL;

  max=iter = -1;

  return svbuf;
}


// ---------------- AddToBuffer -----------------
bool AddToBuffer(char c)
{
  #ifdef BUFFER_DEBUG
    debug("Adding to buffer.");
  #endif

  if(buf==NULL)
  {
    #ifdef BUFFER_DEBUG
      debug("Allocating buffer.");
    #endif

    // values at the beginning
    iter = 0;
    max = 50;

    buf = malloc((max+1) * sizeof(char));
    if (buf == NULL)
      return false;
  }

  else if (iter >= max)
  {
    max += 50;
    buf = realloc(buf, max);
  }

  buf[iter] = tolower(c);
  iter++;

  return true;
}


// ---------------- FreeBuffer -----------------
void FreeBuffer()
{
  #ifdef BUFFER_DEBUG
    debug("Freing debug.");
  #endif


  free(buf);
  max=iter = -1;
}
