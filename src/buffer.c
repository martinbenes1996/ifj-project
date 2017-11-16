
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"
#include "io.h"

/*------ globals ----------*/
static char *buf = NULL;
static long iter = -1;
static long max = -1;
/*-------------------------*/

// ---------------- GetBuffer -----------------
char * GetBuffer()
{
  #ifdef BUFFER_DEBUG
    debug("Get the buffer.");
  #endif

  // if no entry, return empty string
  if (buf == NULL) return calloc(1,sizeof(char));

  // add \0 to the end (C-string)
  AddToBuffer(0);

  // cut the capacity to the size
  buf = realloc(buf, iter);
  if(buf == NULL) return NULL;

  // reinit the inner variables
  char * svbuf = buf;
  buf = NULL;
  max = -1;
  iter = -1;

  // return
  return svbuf;
}


// ---------------- AddToBuffer -----------------
bool AddToBuffer(char c)
{
  #ifdef BUFFER_DEBUG
    debug("Adding to buffer.");
  #endif

  //--------------- alloc ---------------
  // not allocated yet
  if(buf==NULL)
  {
    #ifdef BUFFER_DEBUG
      debug("Allocating buffer.");
    #endif

    // inner variables initialized
    iter = 0;
    max = 50;

    // alloc
    buf = malloc((max+1) * sizeof(char));
    if (buf == NULL) return false;

  }
  // too small (realloc)
  else if (iter >= max)
  {
    #ifdef BUFFER_DEBUG
      debug("Reallocating buffer.");
    #endif

    // increase block
    max += 50;
    buf = realloc(buf, max);
    if(buf == NULL) return false;
  }

  //-------------- emplace ---------------
  // case-insensitive
  c = tolower(c);

  // adding 0
  if( c == '\0' )
  {
    buf[iter] = c;
    iter++;
  }

  // others non-visible
  else if( (c <= 32) || (c == 35) || (c == 92))
  {
    buf[iter] = '\\';
    iter++;
    AddToBuffer((c/100) + '0');
    AddToBuffer((c%100)/10 + '0');
    AddToBuffer(c%10 + '0');
  }

  // regular symbols
  else buf[iter++] = c;

  return true;
}

char * CutBuffer(char * buff)
{
  #ifdef BUFFER_DEBUG
    debug("Cutting buffer.");
  #endif

  // cut
  if(strlen(buff) > 128)
  {
    char * shorter = realloc(buff, sizeof(char) * 129);
    if(shorter == NULL) return NULL;
    shorter[128] = '\0';
    return shorter;
  }
  // not cut
  else return buff;

}

// ---------------- FreeBuffer -----------------
void FreeBuffer()
{
  #ifdef BUFFER_DEBUG
    debug("Freing debug.");
  #endif

  free(buf);
  max = -1;
  iter = -1;
}
