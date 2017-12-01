
/**
 * @file parser.h
 * @authors xbenes49 xbolsh00 xkrato47 xpolan09
 * @date 29th september 2017
 * @brief Syntax parser interface.
 *
 * This interface declares Syntax Analyser and its function. It reads
 * from a queue.
 */

#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>

#include "queue.h"

/*-----------------------------------------------------------*/
/** @addtogroup Parser
 * Parser function.
 * @{
 */

/**
 * @brief   Launches parser.
 *
 * This function launches the parser, scanner in separated thread and starts
 * scanning at all.
 * @returns     False if fail, true if success.
 */
bool RunParser();

/** @} */
/*-----------------------------------------------------------*/

#endif // PARSER_H
