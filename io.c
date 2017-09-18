
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

/*------------------ INPUT -------------------*/

/**
 * @brief   Loads line from source of maximum size.
 * @param src     Read source.
 * @param buff    Target address.
 * @param max     Maximum size.
 * @returns       Loaded characters, -1 if fail.
 */
long getLine(FILE * src, char buff[], long max)
{
  int c, i;
  for(i = 0; (c = getc(src)) != EOF
              && c != '\n'
              && i < max-1;           i++)
    buff[i] = c;

  buff[i++] = '\0';
  return i;

}


/*------------------ OUTPUT --------------------*/
static bool outWasInit = false; /*< Wheater syslog was opened or not. */

/**
 * @brief   Initializes output (syslog).
 */
void initOut()
{
  openlog("ifj", LOG_NDELAY | LOG_CONS, LOG_LOCAL0);
  #ifdef IO_DEBUG
    out("Initializing log.");
  #endif
  outWasInit = true;
}

/**
 * @brief   Closes output (syslog).
 */
void closeOut()
{
  #ifdef IO_DEBUG
    out("Closing log.");
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
