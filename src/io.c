
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

#define RED "\033[0;31m"
#define RED_BOLD "\033[1;31m"
#define RED_INVERSE "\033[7;31m"
#define GREEN "\033[0;32m"
#define GREEN_BOLD "\033[1;32m"
#define GREEN_INVERSE "\033[7;32m"
#define YELLOW "\033[0;33m"
#define YELLOW_BOLD "\033[1;33m"
#define YELLOW_INVERSE "\033[7;33m"
#define BLUE "\033[0;34m"
#define BLUE_BOLD "\033[1;34m"
#define BLUE_INVERSE "\033[7;34"
#define RESET_COLOR "\033[0m"


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

  fprintf(stderr, "%s", YELLOW);
  vfprintf(stderr, str, args);
  fprintf(stderr, "%s\n", RESET_COLOR);
  fflush(stderr);

}

void out(const char * str, ...)
{
  va_list args;
  va_start(args, str);
  printf("%s", GREEN_INVERSE);
  vfprintf(stdout, str, args);
  fprintf(stdout, "%s\n", RESET_COLOR);
  fflush(stdout);
}

void err(const char * str, ...)
{
  va_list args;
  va_start(args, str);
  fprintf(stderr, "%s", RED_BOLD);
  vfprintf(stderr, str, args);
  fprintf(stderr, "%s\n", RESET_COLOR);
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
    #ifdef IO_DEBUG
      debug("Loose mem %d", mem);
    #endif
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
    #ifdef IO_DEBUG
      debug("Return %d", c);
    #endif
    return true;
  }
  else return false;
}

/*-------------------------------------------------------*/
