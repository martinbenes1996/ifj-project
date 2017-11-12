#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

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

#endif // FUNCTIONS_H
