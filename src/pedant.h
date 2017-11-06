#ifndef PEDANT_H
#define PEDANT_H

#include <stdbool.h>

/**
 * @brief     Returns, weather the variable was defined in function, or not.
 *
 * This function looks in the symbol table and it checks, if the variable,
 * named varname was defined in function functionId.
 * @param functionId      The function ID.
 * @param varname         Name of the variable.
 * @returns   True if defined. False if not.
 */
bool Sem_VariableDefined(long functionId, const char * varname);

/**
 * @brief     Returns, weather the function was defined, or not.
 *
 * This function looks in the symbol table and it checks, if the function,
 * named funcname was defined or declared.
 * @param varname         Name of the function.
 * @returns   True if defined. False if not.
 */
bool Sem_FunctionDefined(const char * funcname);

#endif // PEDANT_H
