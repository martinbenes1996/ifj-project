#ifndef SCANNER_SINGLETHRD_H
#define SCANNER_SINGLETHRD_H

#include "types.h"

#ifndef MULTITHREAD
/**
 * @brief   Removes from given queue.
 *
 * This function removes one phrasem from queue. It may wait,
 * if the queue has been emptied.
 * @returns       True if success, false if fail.
 */
Phrasem RemoveFromQueue();

/**
 * @brief   Returns phrasem back to queue (from parser).
 * @param p       Phrasem to return.
 * @returns True, if success. False otherwise.
 */
bool ReturnToQueue(Phrasem p);

#endif //MULTITHREAD

inline void dontcallthisfunction() {} /*< it cant be blank*/

#endif // SCANNER_SINGLETHRD_H
