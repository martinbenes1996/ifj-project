
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
 * @param st      Stack to push onto.
 * @param p       Phrasem to push.
 * @returns True, if success. Fail otherwise.
 */
bool PushOntoStack(Stack st, Phrasem p);

/**
 * @brief     Pops from the stack.
 *
 * This function pops top item (Phrasem) from the given stack and returns it.
 * @param st      Given stack
 * @returns Data (Phrasem) being popped. NULL if empty.
 */
Phrasem PopFromStack(Stack st);

/**
 * @brief     Turns the stack on its head.
 *
 * This function will turn the stack upside-down.
 * @param st      Stack being turned.
 * @returns True if success. False otherwise.
 */
bool TurnStack(Stack st);

/**
 * @brief     Clears whole stack.
 *
 * This function will clear the stack and free all memory binded to it.
 * @param st      Stack being freed.
 */
void ClearStack(Stack st);

/**
 * @brief     Prints stack.
 */
void PrintStack(Stack);




/** @}*/
/*----------------------------EXPRESSION_PARSER_STACK---------------------*/
/** @addtogroup EPStack
 * Expression Parse Stack functions.
 * @{
 */


/**
 * @brief     Initialises EPstack. It is stored inside (aka singleton)
 */
void InitEPStack();

/**
 * @brief     Pushes data onto stack.
 * @param data         Data to be pushed.
 * @returns            True, if success. Fail otherwise.
 */
bool PushOntoEPStack(char data);

/**
 * @brief     Pops data from the stack.
 * @returns Character or -1 (error).
 */
char PopFromEPStack();

/**
 * @brief     Clears stack.
 */
void ClearEPStack();

/**
 * @brief     Compares top three items on the stack with parametres (x1 top, x3 under it).
 * @param x1, x2, x3         Top three items on the stack.
 * @returns            True, if success. Fail otherwise.
 */
bool LookTripleAheadEPStack(char x1, char x2, char x3);

/**
 * @brief     Compares top item on the stack with parametres.
 * @param x1         Top item on the stack.
 * @returns            True, if success. Fail otherwise.
 */
bool LookOneAheadEPStack(char x1);

/**
 * @brief     Compares top item on the stack with op_E.
 * @returns            True, if success. Fail otherwise.
 */
bool LookEAheadEPStack();

/**
 * @brief     gets symbol from top of the stack, ignores E.
 * @returns            Returns character from top.
 */
char ExprOnTopOfEPStack();

/**
 * @brief     Finds out if the expression successfuly ended.
 * @returns            True, if end. Fail otherwise.
 */
bool LookEndAheadEPStack();

void printstackEP();

/** @}*/
/*----------------------------------------------------------------------------*/


#endif // STACK_H
