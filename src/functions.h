#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdbool.h>
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

#endif // FUNCTIONS_H
