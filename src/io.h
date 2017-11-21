
/**
 * @file io.h
 * @interface io
 * @authors xbenes49 xbolsh00 xkrato47 xpolan09
 * @date 19th september 2017
 * @brief Input Output library.
 *
 * This module consists of declaration of input output operation, that are
 * used throughout the project. It can be then sent into file, syslog etc.
 * It also has preprocessor symbols for enabling selective logging,
 * which is useful for development.
 */

#ifndef IO_H
#define IO_H

#include <stdbool.h>
#include <stdio.h>

/*------------------------ DEBUG -------------------------*/
/** @addtogroup Debug_macros
 * Macros and symbols for selective debugging.
 * @{
 */

/*
 HOWTO:
 This module ensures the logging system. All the project (all parts)
 should be commented as shown:

#ifdef ???
  debug("-activity-");
#endif

 It must have this line-alignment.

 If you do so, then - when you uncomment the one of lines below, the
 programme will log about wanted activity.
*/

  /*********************** SYMBOLS *************************/
  #ifdef DEBUG_MODE

    //#define INIT_DEBUG /* Initialization of programme. */
    //#define IO_DEBUG /* IO module debug. */
    //#define ARGS_DEBUG /* Arguments debug. */
    //#define QUEUE_DEBUG /* Scanner-Parser queue connection debug. */
    //#define SCANNER_DEBUG /* Lexical analysis debug. */
    //#define PARSER_DEBUG /* Syntactic analysis debug. */
    //#define STACK_DEBUG /* Stack debug. */
    //#define PEDANT_DEBUG /* Semantic analysis debug. */
    //#define TYPECAST_DEBUG /* Typecast debug. */
    #define GENERATOR_DEBUG /* Code generator debug. */
    //#define KEYWORD_TABLE_DEBUG /* Table of keywords debug. */
    //#define BUFFER_DEBUG /* Buffer debug. */
    //#define CONFIG_DEBUG /* Configuration module debug. */
    //#define CONSTANT_TABLE_DEBUG /* Table of constants debug. */
    //#define GSTATE_STACK_DEBUG /* Generator state stack debug. */
    //#define EXPRESSION_DEBUG /* Expression parse stack debug. */
    //#define COLLECTOR_DEBUG /* Garbage collector for Phrasem structures. */
    //#define OPERATOR_TABLE_DEBUG /* Operator table debug. */

	  /*add your own*/

  #endif // DEBUG_MODE
  /********************************************************/


/** @}*/
/*---------------------------- OUTPUT ----------------------------------*/
/** @addtogroup Output
 * Functions for output.
 * @{
 */

/**
 * @brief   Prints to syslog and to stderr.
 *
 * This function prints given message with variadic parameters
 * to syslog (/var/log/syslog) and to stderr, it adds linefeed afterwards.
 * Then flushes it.
 * @param str     Format message.
 */
void debug(const char * str, ...);


/**
 * @brief   Prints to stdout.
 *
 * This function prints given message with variadic parameters to stdout,
 * it adds linefeed afterwards. Then flushes it.
 * @param str     Format message.
 */
void out(const char * str, ...);

/**
 * @brief   Prints to stderr.
 *
 * This function prints given message with variadic parameters to stderr,
 * it adds linefeed afterwards. Then flushes it.
 * @param str     Format message.
 */
void err(const char * str, ...);


/**
 * @brief   Closes output.
 *
 * This function ends communication of programme with syslog.
 */
void closeOut();


/** @} */
/*----------------------------- INPUT ---------------------------------*/
/** @addtogroup Input
 * Functions for input.
 * @{
 */

/**
 * @brief   Loads line from source.
 *
 * This function gets line from given source (ended with EOF, or linefeed)
 * within given maximum length, and saves it to the given buffer with '\0'
 * in the end.
 * @param src     Read source.
 * @param buff    Target address.
 * @param max     Maximum size.
 * @returns       If EOF, it returns false. Otherwise true.
 */
bool getLine(FILE * src, char buff[], long max);

/**
 * @brief   Returns byte from input file, or EOF.
 * @returns       Byte, or EOF.
 */
int getByte();

/**
 * @brief   Gives byte back to input pipe.
 *
 * This function will return the given byte into memoy of input and
 * at next getByte() call, it will be returned.
 * @param c       Given byte.
 * @returns       False, if fail.
 */
bool returnByte(char c);



/** @} */
#endif // IO_H
