#ifndef GENERATOR_H
#define GENERATOR_H

#include <stdbool.h>

#include "types.h"

/*-----------------------------------------------------------*/
/** @addtogroup Generator_handle
 * Data recievers.
 * @{
 */

/**
 * @brief   Generates the code.
 *
 * This function recieves stack from pedant with expression, which
 * has to be transferred from postfix to assembly generated code.
 * @param s         Stack with expression.
 * @returns True, if success. False otherwise.
 */
bool SendStack(Stack s);

/**
 * @brief   Gives phrasem.
 *
 * This function gives phrasem to generator.
 * @param p         Phrasem.
 * @returns True if success. False otherwise.
 */
bool HandlePhrasem(Phrasem p);

/** @} */
/*-----------------------------------------------------------*/
/** @addtogroup Announcers
 * Announces, what will be given.
 * @{
 */

/** @brief Announces function call to generator. */
void G_FunctionCall();

/** @brief Announces condition to generator. */
void G_Condition();

/** @brief Announces cycle to generator. */
void G_Cycle();

/** @brief Announces assignment to generator. */
void G_Assignment();

/** @brief Announces print command to generator. */
void G_Print();

/** @brief Announces input command to generator. */
void G_Input();

/** @brief Announces logic to generator. */
void G_Logic();

/** @brief Announces expression to generator. */
void G_Expression();

/** @brief Announces expression to generator. */
void G_RelativeOperator();

/** @} */
/*-----------------------------------------------------------*/

#endif // GENERATOR_H
