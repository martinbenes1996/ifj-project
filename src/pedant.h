
/**
 * @file pedant.h
 * @interface pedant
 * @authors xbenes49 xbolsh00 xkrato47 xpolan09
 * @date 5th october 2017
 * @brief Semantic analyser interface.
 *
 * This interface declares semantic analyzer (pedant).
 */

#ifndef PEDANT_H
#define PEDANT_H

#include <stdbool.h>

#include "types.h"

/*------------------------------- SEMANTICS ----------------------------------*/
/** @addtogroup Pedant
 * Pedant functions.
 * @{
 */

/**
 * @brief     Returns, weather the variable was defined in function, or not.
 *
 * This function looks in the symbol table and it checks, if the variable,
 * named varname was defined in function functionId.
 * @param functionId      The function ID.
 * @param varname         Name of the variable.
 * @returns   True if defined. False if not.
 */
bool P_VariableDefined(Phrasem varname);

/**
 * @brief     Returns, weather the function was defined, or not.
 *
 * This function looks in the symbol table and it checks, if the function,
 * named funcname was defined.
 * @param varname         Name of the function.
 * @returns   True if defined. False if not.
 */
bool P_FunctionDefined(Phrasem funcname);

/**
 * @brief     Returns, weather the function exists, or not.
 *
 * This function looks in the symbol table and it checks, if the function,
 * named funcname was defined or declared.
 * @param varname         Name of the function.
 * @returns   True if exists. False if not.
 */
bool P_FunctionExists(Phrasem p);

/**
 * @brief   Defines new variable.
 *
 * This function will write new variable to the symbol table.
 * Function is read from config.h.
 * @param varname     Name of the variable.
 * @param datatype    Data type of the variable.
 * @returns True if success. False otherwise.
 */
bool P_DefineNewVariable(Phrasem varname, Phrasem datatype);

/**
 * @brief   Declares new function.
 *
 * This function will write new record to symtable.
 * @param funcname      Phrasem with function name.
 * @param functype      Phrasem with function type.
 * @param params        Parameters.
 * @returns True if success. False otherwise.
 */
bool P_DeclareNewFunction(Phrasem, Phrasem, Parameters);

/**
 * @brief   Defines new function.
 *
 * This function will write new function to the symbol table.
 * @param funcname    Name of the function.
 * @returns True if success. False otherwise.
 */
bool P_DefineNewFunction(Phrasem funcname, Phrasem functype, Parameters params);

/**
 * @brief     Pushes operand/operator to stack.
 *
 * This function pushes the given phrasem to stack, which is
 * then distributed to generator.
 * @param p         Phrasem being pushed.
 * @returns True, if success. False otherwise.
 */
bool P_HandleOperand(Phrasem p);

/**
 * @brief     Gives target of assignment.
 *
 * This function is called after expression parse. It gives target variable
 * of assignment.
 * @param p     Target variable.
 * @returns True if success. False otherwise.
 */
bool P_HandleTarget(Phrasem p);

/**
 * @brief     Compare operator handler.
 *
 * This function handles the phrasem with comparison operator, checks types
 * of both sides and calls the generator to generate the comparison.
 * @param p       Given compare operator.
 * @returns True if compatible. False otherwise.
 */
bool P_HandleCompareOperator(Phrasem p);

/**
 * @brief     Moves stack to generator.
 *
 * This function sends stack to generator and returns status.
 * @returns True, if success. False otherwise.
 */
bool P_MoveStackToGenerator();

/**
 * @brief     Checks type of stack.
 *
 * This function checks type of stack in pedant, moves it to generator
 * and calls for generating implicit typecast.
 * @return True, if success. False otherwise.
 */
bool P_CheckType_MoveStackToGenerator(DataType);

/**
 * @brief     Hangs datatype.
 *
 * This function hangs datatype for later check.
 * @param dt    DataType hung.
 */
void P_HangDataType(DataType dt);

/**
 * @brief     Checks datatype.
 *
 * This function checks the datatype hung with the one given and typecasts.
 * @param dt    Datatype to check.
 * @returns True if fine. False if incompatible.
 */
bool P_CheckDataType(DataType);

/**
 * @brief     Performs last actions with stack after expression parse.
 *
 * This function handles retyping of operands.
 * @returns True, if success. False otherwise.
 */
bool ExpressionEnd();

/**
 * @brief     Pedant destructor.
 *
 * This function destructs the pedant and deallocates all its inner memory.
 */
void ClearPedant();


/** @}*/
/*----------------------------------------------------------------------------*/

#endif // PEDANT_H
