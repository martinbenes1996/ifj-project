
/**
 * @file generator.h
 * @interface generator
 * @authors xbenes49 xbolsh00 xkrato47 xpolan09
 * @date 6th november 2017
 * @brief Generator interface
 *
 * This interface enables interactions with generator.
 */

#ifndef GENERATOR_H
#define GENERATOR_H

#include <stdbool.h>

#include "types.h"


/*-----------------------------------------------------------*/
/** @addtogroup Generator_main
 * Main generator functions.
 * @{
 */

/**
 * @brief   Generator initializer.
 *
 * This function will initialize the inner structures of
 * generator and will print the header of output file.
 */
void InitGenerator();

/** @} */
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
bool Send(Stack s);

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

/** @brief Announces variable declaration to generator. */
void G_VariableDeclaration();

/** @brief Announces end of a block to generator. */
void G_EndBlock();

/** @} */
/*-----------------------------------------------------------*/
/** @addtogroup Generators
 * Generation impulses.
 * @{
 */

/**
 * @brief Generates typecast.
 *
 * This function will generate the code, which will typecast the top of
 * the stack.
 * @param tc    Type of typecast.
 */
void GenerateTypeCast(TokenType tc);

/** @} */
/*-----------------------------------------------------------*/

#endif // GENERATOR_H
