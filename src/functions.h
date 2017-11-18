
/**
 * @file functions.h
 * @interface functions
 * @authors xbenes49 xbolsh00 xkrato47 xpolan09
 * @date 5th october 2017
 * @brief Global functions.
 *
 * This module defines global inline functions, used in whole project
 * (print, indicators, etc.).
 */

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "io.h"
#include "tables.h"
#include "types.h"

/*------------------------------ DECLARATIONS --------------------------------*/
/** @addtogroup Covers
 * Cover functions.
 * @{
 */

/**
 * @brief   Checks phrasem if is operator.
 *
 * This function will take phrasem and string with operator.
 * It returns true, if it is the operator. (looks into tables).
 * @param p       Phrasem to be check.
 * @param op      Operator.
 * @returns True if success. False otherwise.
 */
inline bool isOperator(Phrasem p, const char * op);

/**
 * @brief   Checks phrasem if is separator.
 *
 * This function will take phrasem and checks if it is separator.
 * @returns True if success. False otherwise.
 */
inline bool isSeparator(Phrasem p);

/**
 * @brief   Checks phrasem if is keyword.
 *
 * This function will take phrasem and string with keyword.
 * It returns true, if it is the keyword. (looks into tables).
 * @param p       Phrasem to be check.
 * @param op      Keyword.
 * @returns True if success. False otherwise.
 */
inline bool matchesKeyword(Phrasem p, const char * kw);

/**
 * @brief   Datatype from variable.
 *
 * This function will return DataType from Phrasem given.
 * @param p       Phrasem.
 * @returns DataType of variable.
 */
inline DataType getDataType(Phrasem p);

/** @}*/
/*----------------------------------------------------*/
/** @addtogroup Tools
 * Tool functions.
 * @{
 */

/**
 * @brief    Phrasem duplicator.
 *
 * This function will do deep copy of given Phrasem structure.
 * @param p       Phrasem to be copied.
 * @returns Copied Phrasem, or NULL, if fail.
 */
inline Phrasem duplicatePhrasem(Phrasem p);

/**
 * @brief    String duplicator.
 *
 * This function is implementation of non-standard function of string.h
 * @param str    String to be copied.
 * @returns Copied string, or NULL, if fail.
 */
inline char * strdup(const char * str);

/**
 * @brief    Frees phrasem completely.
 *
 * This function will free the phrasem and all memory bind to it.
 * @param p     Phrasem to free.
 */
inline void freePhrasem(Phrasem p);

/** @}*/
/*----------------------------------------------------*/
/** @addtogroup Printers
 * Printers functions.
 * @{
 */

/**
 * @brief   Debug function to print phrasem.
 * @param p       Phrasem to be printed.
 */
inline void PrintPhrasem(Phrasem p);

/**
 * @brief   Debug function to print data type.
 * @param dt       DataType to be printed.
 */
inline void PrintDataType(DataType dt);

/** @}*/
/*----------------------------------------------------*/
























/*------------------------------ DEFINITIONS --------------------------------*/

/*--------------- COVERS -------------------*/
inline bool isOperator(Phrasem p, const char * op)
{
  return (p->table == TokenType_Operator) && (p->d.index == getOperatorId(op));
}

inline bool isSeparator(Phrasem p)
{
  return p->table == TokenType_Separator;
}

inline bool matchesKeyword(Phrasem p, const char * kw)
{
  return (p->table == TokenType_Keyword) && (p->d.index == isKeyword(kw));
}

inline DataType getDataType(Phrasem p)
{
  if(matchesKeyword(p, "integer")) return DataType_Integer;
  else if(matchesKeyword(p, "double")) return DataType_Double;
  else if(matchesKeyword(p, "string")) return DataType_String;
  else return DataType_Unknown;
}

/*------------------ TOOLS --------------------*/
inline char * strdup(const char * str)
{
  char * newstr = malloc(sizeof(char)*(strlen(str)+1));
  if(newstr == NULL) return NULL;

  strcpy(newstr, str);
  return newstr;
}

inline Phrasem duplicatePhrasem(Phrasem p)
{
  Phrasem dup = malloc(sizeof(struct phrasem_data));
  if(dup == NULL) return NULL;
  //
  dup->table = p->table;

  switch(dup->table)
  {
    case TokenType_Separator:
    case TokenType_EOF:
      break;

    // deep copy
    case TokenType_Variable:
    case TokenType_Function:
    case TokenType_Symbol:
      dup->d.str = strdup(p->d.str);
      if(dup->d.str == NULL) { free(dup); return NULL; }
      break;

    case TokenType_Constant:
    case TokenType_Operator:
    case TokenType_Keyword:
      dup->d.index = p->d.index;
      break;
    default:
      break;
  }
  return dup;
}

inline void freePhrasem(Phrasem p)
{
  switch(p->table)
  {
    case TokenType_Variable:
    case TokenType_Function:
    case TokenType_Symbol:
      free((void *)p->d.str);
      break;
    default:
      break;
  }
  free(p);
}

/*----------------- PRINTERS ---------------*/

inline void PrintPhrasem(Phrasem p)
{
  if(p == NULL) return;
  DataType type = findConstType(p->d.index);
  switch(p->table)
  {
    case TokenType_Constant:

      switch(type)
      {
        case DataType_Double:
          debug("Phrasem [%s double %ld->%f]", TokenTypeToString(p->table), p->d.index, getDoubleConstValue(p->d.index));
          break;
        case DataType_Integer:

          debug("Phrasem [%s int %ld->%d]", TokenTypeToString(p->table), p->d.index, getIntConstValue(p->d.index));
          break;
        case DataType_String:
          debug("Phrasem [%s string %ld->\"%s\"]", TokenTypeToString(p->table), p->d.index, getStringConstValue(p->d.index));
          break;
        default:
          debug("Phrasem [%s UNKNOWN]", TokenTypeToString(p->table));
          break;
      }

      break;
    case TokenType_Operator:
    case TokenType_Keyword:
      debug("Phrasem [%s %d]", TokenTypeToString(p->table), p->d.index);
      break;
    case TokenType_Variable:
    case TokenType_Function:
    case TokenType_Symbol:
      debug("Phrasem [%s \"%s\"]", TokenTypeToString(p->table), p->d.str);
      break;
    case TokenType_Separator:
    case TokenType_EOF:
      debug("Phrasem [%s]", TokenTypeToString(p->table));
      break;
    case TypeCast_Int2Double:
      debug("Phrasem [%s]", TokenTypeToString(p->table));
      break;
    case TypeCast_Double2Int:
      debug("Phrasem [%s]", TokenTypeToString(p->table));
      break;
    default:
      debug("---unknown phrasem---");
      break;
  }
}

inline void PrintDataType(DataType dt)
{
  switch(dt)
  {
    case DataType_String:
      debug("DataType [string]");
      break;

    case DataType_Double:
      debug("DataType [double]");
      break;

    case DataType_Integer:
      debug("DataType [int]");
      break;

    case DataType_Unknown:
      debug("DataType [UNKNOWN]");
      break;

    default:
      debug("---undefined datatype---");
      break;
  }
}


#endif // FUNCTIONS_H
