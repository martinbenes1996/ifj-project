
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include "buffer.h"
#include "io.h"

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

  c = tolower(c);

  // ending 0
  if( c == '\0' )
  {
    buf[iter] = c;
    iter++;
  }
  // others
  else if( (c <= 32) || (c == 35) || (c == 92))
  {
    buf[iter] = '\\';
    iter++;
    AddToBuffer((c/100) + '0');
    AddToBuffer((c%100)/10 + '0');
    AddToBuffer(c%10 + '0');
  }
  // regular symbol
  else buf[iter++] = c;

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
