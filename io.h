
// io.h
// Input Output library (including logging)
// IFJ project
// FIT VUT
// 2017/2018

#ifndef IO_H
#define IO_H

#include <stdbool.h>
#include <stdio.h>

/*------------------------ DEBUG -------------------------*/

/**
 * HOWTO:
 * This module ensures the logging system. All the project (all parts)
 * should be commented as shown:

#ifdef ???
  debug("-activity-");
#endif

 * It must have this line-alignment.
 *
 * If you do so, then - when you uncomment the one of lines below, the
 * programme will log about wanted activity.
 */

  /*********************** SYMBOLS *************************/
  #ifdef DEBUG_MODE

    //#define INIT_DEBUG /* Initialization of programme. */
    //#define IO_DEBUG /* IO module debug. */
    //#define ARGS_DEBUG /* Arguments debug. */

	  /*add your own*/

  #endif // DEBUG_MODE
  /********************************************************/



/*---------------------------- OUTPUT ----------------------------------*/

/**
 * @brief   Prints to syslog and to stderr with newline. Flushes it afterwards.
 * @param str     Format message.
 */
void debug(const char * str, ...);


/**
 * @brief   Prints to stdout with newline. Flushes it afterwards.
 * @param str     Format message.
 */
void out(const char * str, ...);

/**
 * @brief   Prints to stderr with newline. Flushes it afterwards.
 * @param str     Format message.
 */
void error(const char * str, ...);


/**
 * @brief   Closes output (syslog).
 */
void closeOut();



/*----------------------------- INPUT ---------------------------------*/
/**
 * @brief   Loads line from source of maximum size.
 * @param src     Read source.
 * @param buff    Target address.
 * @param max     Maximum size.
 * @returns       If EOF, it returns false. Otherwise true.
 */
bool getLine(FILE * src, char buff[], long max);



#endif // IO_H
