#ifndef PEDANT_H
#define PEDANT_H

#include <stdbool.h>

#include "types.h"

/**
 * @brief     Returns, weather the variable was defined in function, or not.
 *
 * This function looks in the symbol table and it checks, if the variable,
 * named varname was defined in function functionId.
 * @param functionId      The function ID.
 * @param varname         Name of the variable.
 * @returns   True if defined. False if not.
 */
bool P_VariableDefined(long functionId, const char * varname);

/**
 * @brief     Returns, weather the function was defined, or not.
 *
 * This function looks in the symbol table and it checks, if the function,
 * named funcname was defined or declared.
 * @param varname         Name of the function.
 * @returns   True if defined. False if not.
 */
bool P_FunctionDefined(const char * funcname);

/**
 * @brief     Pushes operand/operator to stack.
 *
 * This function pushes the given phrasem to stack, which is
 * then distributed to generator.
 * @param p         Phrasem being pushed.
 * @returns True, if success. False otherwise.
 */
bool P_HandleOperand(Phrasem p);

#endif // PEDANT_H
