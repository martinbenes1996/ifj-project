#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "io.h"
#include "tables.h"
#include "types.h"

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

inline Phrasem duplicatePhrasem(Phrasem p);
inline char * strdup(const char * str);

inline void freePhrasem(Phrasem p);

inline DataType getDataType(Phrasem p);

/**
 * @brief   Debug function to print phrasem.
 * @param p       Phrasem to be printed.
 */
inline void PrintPhrasem(Phrasem p);


/*------------------------------------------------*/
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

inline DataType getDataType(Phrasem p)
{
  if(matchesKeyword(p, "integer")) return DataType_Integer;
  else if(matchesKeyword(p, "double")) return DataType_Double;
  else if(matchesKeyword(p, "string")) return DataType_String;
  else return DataType_Unknown;
}

inline void PrintPhrasem(Phrasem p)
{
  if(p == NULL) return;
  int index = findConstType(p->d.index);
  switch(p->table)
  {
    case TokenType_Constant:

      switch(index)
      {
        case DataType_Double:
          debug("Phrasem [%s %f]", TokenTypeToString(p->table), getDoubleConstValue(index));
          break;
        case DataType_Integer:
          debug("Phrasem [%s %d]", TokenTypeToString(p->table), getIntConstValue(index));
          break;
        case DataType_String:
          debug("Phrasem [%s %s]", TokenTypeToString(p->table), getStringConstValue(index));
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
    default:
      break;
  }
}


#endif // FUNCTIONS_H
