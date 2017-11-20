
/**
 * @file symtable.h
 * @interface symtable
 * @authors xbenes49 xbolsh00 xkrato47 xpolan09
 * @date 6th november 2017
 * @brief Table library
 *
 * This interface enables interactions with symbol table.
 */


#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stdlib.h>
#include <stdio.h>
#include "list.h"
#include "types.h"



/*-----------------------------------------------------------*/

                //SYMBOL TABLE FUNCTIONS

// -Odstranil jsem funkci VarInsert, pokud jste ji pouzili, zmente ji!



/**
 * @brief   Finds a variable in a function.
 *
 * Returns false when unsuccessful or true
 * @param functionName  name of the function
 * @param name          name of the variable
 * @returns True/false.
 */
bool findVariable(const char * functionName, const char * name);
/**
 * @brief   Finds a variable type.
 *
 * @param functionName  name of the function
 * @param name          name of the variable
 * @returns success -> DataType, failure -> datatype_unknown.
 */
DataType findVariableType(const char * functionName, const char * name);
/**
 * @brief   Adds a variable type.
 *
 * Returns false when unsuccessful or true
 * @param functionName  name of the function
 * @param name          name of the variable
 * @param type          type of the variable
 * @returns True/false.
 */
bool addVariableType(const char * functionName, const char * name, DataType type);
/**
 * @brief   Adds a variable into a  function.
 *
 * Returns false when unsuccessful or true
 * @param functionName  name of the function
 * @param name          name of the variable
 * @returns True/false.
 */
bool addVariable(const char * functionName, const char * name);
/**
 * @brief   Adds parameters of a function and saves them as variables.
 *
 *      DO NOT CHANGE THE LIST AFTER USING THIS FUNCTION (READ ONLY)
 *  (FUNCTION DOES NOT COPY THE LIST, YOU SHOULD SET YOUR POINTER TO NULL)
 *
 * Returns false when unsuccessful or true
 * @param functionName  name of the function
 * @param type          type of the parameter
 * @param definition    true -> function is being defined, false -> it is only a declaration
 * @returns True/false.
 */
bool addFunctionParameters(const char * functionName, struct paramFce * parametres, bool definition);
/**
 * @brief   finds all parameters of a function.
 *
 * DO NOT DO ANYTHING WITH RECEIVED POINTER APART FROM READING FROM IT!
 *
 * Returns pointer to a list of parameters or NULL
 * @param functionName  name of the function
 * @returns pointer or NULL.
 */
struct paramFce * findFunctionParameters(const char * functionName);
/**
 * @brief   Finds number of parametres of a function.
 *
 * Returns 0 when unsuccessful or number (!Cannot detect if function exists!)
 * @param functionName  name of the function
 * @returns success -> number of parametres, failure -> 0.
 */
size_t findFunctionNumberOfParametres(const char * functionName);
/**
 * @brief   Finds datatype of a function.
 *
 * @param functionName  name of the function
 * @returns success -> DataType, failure -> datatype_unknown.
 */
DataType findFunctionType(const char * functionName);
/**
 * @brief   Sets datatype of a function.
 *
 * Returns false when unsuccessful or true
 * @param functionName  name of the function
 * @param type          type of the function
 * @returns True/false.
 */
bool setFunctionType(const char * functionName, DataType type);
/**
 * @brief   Finds a function in the table.
 *
 * Returns false when unsuccessful or true
 * @param functionName  name of the function
 * @returns True/false.
 */
bool findFunctionInTable(const char * functionName);
/**
 * @brief   Adds a function in the table.
 *
 * Returns false when unsuccessful or true
 * @param name  name of the function
 * @returns True/false.
 */
bool addFunction(const char * name);
/**
 * @brief   Frees the function table.
 *          Use in the end.
 */
void functionTableEnd();




/**
 * @brief   Prints the whole symtable.
 *          Use for testing. (not sure if it will work like that :))
 */
void printfunction(void);

#endif
