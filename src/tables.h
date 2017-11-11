
/**
 * @file tables.h
 * @interface tables
 * @authors xbenes49 xbolsh00 xkrato47 xpolan09
 * @date 7th november 2017
 * @brief Interface of tables.
 *
 * This is interface of interactions with tables (apart from symbol table).
 */

#ifndef TABLES_H
#define TABLES_H
#include "types.h"

/*-----------------------------------------------------------*/

                //KEYWORD TABLE FUNCTIONS

/**
 * @brief   Keyword finder.
 *
 * This function matches given word with keyword.
 * Returns -1 if word is not a keyword, otherwise
 * returns keyword code (array index)
 * @param word    Unknown Word.
 * @returns Keyword code or -1.
 */
int isKeyword(const char * word);

/*-----------------------------------------------------------*/

                //OPERATOR TABLE FUNCTIONS

/**
 * @brief   Operator finder.
 *
 * This function matches given word with operators.
 * Returns -1 if word is not a operator, otherwise
 * returns operator code (array index)
 * @param word    Unknown Word.
 * @returns Operator code or -1.
 */
long getOperatorId(const char * word);

/*-----------------------------------------------------------*/

                //CONST TABLE FUNCTIONS


/**
 * @brief   Insert constant.
 *
 * This function inserts constant into array of constants.
 * Returns -1 when unsuccessful, otherwise
 * returns array index of added constant.
 * @param ptr    array pointer.
 * @param type   type of constant.
 * @param uni    value of constant.
 * @returns array index.
 */
int constInsert(DataType type, DataUnion uni);

/**
 * @brief   Initiation of table of constants.
 *
 * This function allocates table os a starting size.
 * @returns true -> ok, false -> error.
 */
bool constTableInit(void);

/**
 * @brief   Destroys table of constants.
 *
 * This function frees the whole table.
 */
void constTableFree(void);

/**
 * @brief   Returns type of the constant on a given index.
 *
 * @param index     index into the array of constants
 * @returns type of the constant -> ok, DtatType_Unknown -> fail.
 */
DataType findConstType(size_t index);

/**
 * @brief   Returns integer value of the constant on a given index.
 *
 * @param index     index into the array of constants
 * @returns ivalue of the constant -> ok, random number -> fail.
 */
int getIntConstValue(size_t index);

/**
 * @brief   Returns double value of the constant on a given index.
 *
 * @param index     index into the array of constants
 * @returns dvalue of the constant -> ok, random number -> fail.
 */
double getDoubleConstValue(size_t index);

/**
 * @brief   Returns char pointer on the svalue of constant on a given index.
 *
 * @param index     index into the array of constants
 * @returns char pointer on the string -> ok, random pointer -> fail.
 */
char * getStringConstValue(size_t index);

/**
 * @brief   Returns index to default value for integer.
 *
 * @returns index to default value for integer.
 */
size_t getIntDefaultValue();

/**
 * @brief   Returns index to default value for double.
 *
 * @returns index to default value for double.
 */
size_t getDoubleDefaultValue();

/**
 * @brief   Returns index to default value for string.
 *
 * @returns index to default value for string.
 */
size_t getStringDefaultValue();

/**
 * @brief   Changes the value of a constant on given index.
 *          You have to know the type of constant before you use this. (constants cannot be retyped)
 *
 * @param index     index into the array of constants
 * @param value     data union with value
 * @returns true -> ok, false -> fail.
 */
bool changeConstValue(size_t index, DataUnion value);



#endif //TABLES_H
