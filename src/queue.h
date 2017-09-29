
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

typedef struct queue_item{
  Phrasem data;
  struct queue_item * next;
} * QueueItem;

typedef struct queue_head
{
  QueueItem first;
} *QueueHead;
typedef struct queue_tail
{
  QueueItem last;
} *QueueTail;

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

void ClearQueue(QueueHead q);
void PrintQueue(QueueHead q);



#endif // QUEUE_H
