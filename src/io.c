
/**
 * @file io.c
 * @authors xbenes49 xbolsh00 xkrato47 xpolan09
 * @date 19th september 2017
 * @brief Input Output module.
 *
 * This module consists of definitions of input output operation, that are
 * used throughout the project. It can be then sent into file, syslog etc.
 */

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

#include "io.h"


/*------------------ OUTPUT --------------------*/

void debug(const char * str, ...);
void out(const char * str, ...);
void err(const char * str, ...);

static bool outWasInit = false; /*< Wheater syslog was opened or not. */

void initOut()
{
  outWasInit = true;

  #ifdef USE_SYSLOG
  openlog("ifj", LOG_NDELAY | LOG_CONS, LOG_LOCAL0);

  #ifdef IO_DEBUG
    debug("Initializing log.");
  #endif

  #endif

}

void closeOut()
{
  #ifdef USE_SYSLOG
  if(!outWasInit) return;

  #ifdef IO_DEBUG
    debug("Closing log.");
  #endif

  closelog();
  #endif

  outWasInit = false;
}

void debug(const char * str, ...)
{
  #ifdef USE_SYSLOG
  if(!outWasInit) initOut();
  #endif

  va_list args;
  va_start(args, str);

  #ifdef USE_SYSLOG
  syslog(LOG_ERR, str, args);
  #endif

  vfprintf(stderr, str, args);
  fprintf(stderr, "\n");
  fflush(stderr);

}

void out(const char * str, ...)
{
  va_list args;
  va_start(args, str);
  vfprintf(stdout, str, args);
  fprintf(stdout, "\n");
  fflush(stdout);
}

void err(const char * str, ...)
{
  va_list args;
  va_start(args, str);
  vfprintf(stderr, str, args);
  fprintf(stderr, "\n");
  fflush(stderr);
}


/*----------------------- INPUT -------------------------*/
bool getLine(FILE * src, char buff[], long max)
{
  int c, i;
  for(i = 0; (c = fgetc(src)) != EOF
              && c != '\n'
              && i < max-1;           i++)
    buff[i] = c;

  buff[i++] = '\0';

  if(c == EOF) return false;
  #ifdef IO_DEBUG
    debug("Getting line.");
  #endif

  return true;
}

static int mem = -1;
int getByte()
{
  if(mem != -1)
  {
    int ret = mem;
    mem = -1;
    return ret;
  }
  else return getchar();
}

bool returnByte(char c)
{
  if(mem == -1)
  {
    mem = c;
    return true;
  }
  else return false;
}

/*-------------------------------------------------------*/
