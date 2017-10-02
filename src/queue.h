
/**
 * @file queue.h
 * @interface queue
 * @authors xbenes49 xbolsh00 xkrato47 xpolan09
 * @date 28th september 2017
 * @brief Queue library.
 *
 * This module implements the queue.
 */

#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>

#include "types.h"

/**
 * @brief   Initializes queue.
 */
void InitQueue();

/**
 * @brief   Adds to given queue.
 * @returns     True if success, false if fail.
 */
bool AddToQueue(Phrasem data);

/**
 * @brief   Removes from given queue.
 * @returns     True if success, false if fail.
 */
Phrasem RemoveFromQueue();

/**
 * @brief   Ends connection.
 *
 * Ends connection between head and tail (because of error etc.).
 */
void FinishConnectionToQueue();

void ClearQueue();
void PrintQueue();



#endif // QUEUE_H
