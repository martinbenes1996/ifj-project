
/**
 * @file stack.h
 * @interface stack
 * @authors xbenes49 xbolsh00 xkrato47 xpolan09
 * @date 5th october 2017
 * @brief Stack interface.
 *
 * This interface declares stack.
 */

#ifndef STACK_H
#define STACK_H

#include "types.h"


/*---------------------------- STACK ----------------------------------*/
/** @addtogroup Stack
 * Stack functions.
 * @{
 */

Stack InitStack();

/**
 * @brief     Pushes data onto stack.
 * @param data         Data to be pushed.
 * @returns            True, if success. Fail otherwise.
 */
bool PushOntoStack(Stack, Phrasem);

/**
 * @brief     Pops from the stack.
 * @returns            Data being pushed. NULL if empty.
 */
Phrasem PopFromStack(Stack);


/**
 * @brief     Clears all stack.
 */
void ClearStack(Stack);


/** @}*/
#endif // STACK_H
