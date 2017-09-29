
/**
 * @file parser.h
 * @interface parser
 * @authors xbenes49 xbolsh00 xkrato47 xpolan09
 * @date 29th september 2017
 * @brief Lexical Parser library.
 *
 * This module implements Lexical Parser and its operation.
 */

#ifndef PARSER_H
#define PARSER_H

#include "queue.h"

/**
 * @brief   Gives new head of queue.
 *
 * Parser communicates with syntax analyser by queue. This is how they
 * initialize their communication.
 * @param h       New head.
 */
void NewQueueToParser(QueueTail h);

#endif // PARSER_H
