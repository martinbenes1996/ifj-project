
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

/** @}*/
/*---------------------------- OUTPUT ----------------------------------*/
/** @addtogroup Stack
 * Stack functions.
 * @{
 */
 
/**
 * @brief     Pushes data onto stack.
 * @param data         Data to be pushed.
 * @returns            True, if success. Fail otherwise.
 */
bool PushOntoStack(Phrasem data);

/**
 * @brief     Pops from the stack.
 * @returns            Data being pushed. NULL if error. END_PTR if empty.
 */
Phrasem PopFromStack();



/**
 * @brief     Clears all stack.
 */
void ClearStack();


/** @}*/
#endif // STACK_H