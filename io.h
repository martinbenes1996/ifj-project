
// io.h
// Input Output library
// IFJ project
// FIT VUT
// 2017/2018

#ifndef IO_H
#define IO_H

#include <stdio.h>


/*------------------ INPUT -------------------*/

/**
 * @brief   Loads line from source of maximum size.
 * @param src     Read source.
 * @param buff    Target address.
 * @param max     Maximum size.
 * @returns       Loaded characters, -1 if fail.
 */
long getLine(FILE * src, char buff[], long max);


/*------------------ OUTPUT --------------------*/

/**
 * @brief   Closes output (syslog).
 */
void closeOut();

/**
 * @brief   Prints to syslog and to stderr. Flushes it afterwards.
 * @param str     Format message.
 */
void debug(const char * str, ...);

/**
 * @brief   Prints to stdout. Flushes it afterwards.
 * @param str     Format message.
 */
void out(const char * str, ...);

#endif // IO_H
