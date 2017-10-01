
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

struct queue_item;
struct queue_head;
struct queue_tail;
typedef struct queue_item * QueueItem;
typedef struct queue_head * QueueHead;
typedef struct queue_tail * QueueTail;

/**
 * @brief   Initializes queue.
 * @returns     Queue structure, NULL if fail.
 */
QueueHead InitQueue();

/**
 * @brief   Adds to given queue.
 * @returns     True if success, false if fail.
 */
bool AddToQueue(QueueTail q, Phrasem data);

/**
 * @brief   Removes from given queue.
 * @returns     True if success, false if fail.
 */
Phrasem RemoveFromQueue(QueueHead q);

/**
 * @brief   Ends connection.
 *
 * Ends connection between head and tail (because of error etc.).
 */
void FinishConnectionToQueue();

void ClearQueue(QueueHead q);
void PrintQueue(QueueHead q);



#endif // QUEUE_H
