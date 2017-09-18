
// io.c
// Input Output module
// IFJ project
// FIT VUT
// 2017/2018

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>

#include "io.h"


/*------------------ OUTPUT --------------------*/

void debug(const char * str, ...);
void out(const char * str, ...);

static bool outWasInit = false; /*< Wheater syslog was opened or not. */
/**
 * @brief   Initializes output (syslog).
 */
void initOut()
{
  openlog("ifj", LOG_NDELAY | LOG_CONS, LOG_LOCAL0);
  outWasInit = true;
  #if defined INIT_DEBUG || defined IO_DEBUG
    debug("Initializing log.");
  #endif

}
/**
 * @brief   Closes output (syslog).
 */
void closeOut()
{
  if(!outWasInit) return;
  #if defined INIT_DEBUG || defined IO_DEBUG
    debug("Closing log.");
  #endif
  closelog();
  outWasInit = false;
}

/**
 * @brief   Prints to syslog and to stderr. Flushes it afterwards.
 * @param str     String to be written.
 */
void debug(const char * str, ...)
{
  if(!outWasInit) initOut();

  va_list args;
  va_start(args, str);
  syslog(LOG_ERR, str, args);

  fprintf(stderr, str, args);
  fprintf(stderr, "\n");
  fflush(stderr);

}

/**
 * @brief   Prints to stdout. Flushes it afterwards.
 * @param str     String to be written.
 */
void out(const char * str, ...)
{
  va_list args;
  va_start(args, str);
  fprintf(stdout, str, args);
  fprintf(stdout, "\n");
  fflush(stdout);
}


/*----------------------- INPUT -------------------------*/
/**
 * @brief   Loads line from source of maximum size.
 * @param src     Read source.
 * @param buff    Target address.
 * @param max     Maximum size.
 * @returns       If EOF, it returns false. Otherwise true.
 */
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
