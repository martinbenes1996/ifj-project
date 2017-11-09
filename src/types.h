
/**
 * @file types.h
 * @authors xbenes49 xbolsh00 xkrato47 xpolan09
 * @date 19th september 2017
 * @brief Definitions of public types.
 *
 * This module defines basic public types, used in whole project.
 */

#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>

#include "io.h"

/*--------------------------------------------------*/
/** @addtogroup Symbol_types
 * Types used in symbol table etc.
 * @{
 */

/**
 * @brief   Data type enumeration for symbols.
 *
 * This enum contains list of all possible types for symbols,
 * integer, double, string or function.
 */
typedef enum{
  DataType_Integer,
  DataType_Double,
  DataType_String,
  DataType_Function,
  DataType_Unknown  //i need that for symtable
} DataType;

/**
 * @brief   Operators enumeration.
 *
 * This enum contains list of all possible operators.
 */
typedef enum{
  Mul,        // *    PRIORITY 1
  DivDouble,  // /

  DivInt,     // '\'  PRIORITY 2

  Add,        // +    PRIORITY 3
  Sub,        // -

  Equal,      // =    PRIORITY 4
  Nequal,     // <>
  Lesser,     // <
  LesserEq,   // <=
  Bigger,     // >
  BiggerEq,    // >=

  OpenBracket,  // (   NOT OPERATOR SYMBOLS
  CloseBracket, // )
  Comma,        // ,
} Operators;

/**
 * @brief   Structure representing values of constants/variables.
 *
 * This structure contains an int/double/string value.
 */
typedef union dataUnion{
    int ivalue;
    double dvalue;
    char * svalue;
} DataUnion;

/**
 * @brief   Table types.
 *
 * This enum contains list of certain tables.
 */
typedef enum{
    Table_SymbolTable,
    Table_ConstantTable,
    Table_OperatorTable,
    Table_KeywordTable
} Table;
inline const char * TableToString(Table tb)
{
  switch(tb)
  {
    case Table_SymbolTable: return "Symbol Table";
    case Table_ConstantTable: return "Table of Constants";
    case Table_OperatorTable: return "Operator Table";
    case Table_KeywordTable: return "Keyword Table";
    default: return "Unknown TableType!";
  }
}

/** @} */
/*--------------------------------------------------*/
/** @addtogroup Token_types
 * Types used in token.
 * @{
 */

/**
 * @brief   Enumeration for token types.
 *
 * This enumeration contains possible types of tokens. Their datatype
 * is then saved in symbol table.
 */
typedef enum{
  TokenType_Constant,
  TokenType_Operator,
  TokenType_Keyword,
  TokenType_Symbol, // for lexical - in syntax, it will be changed to variable/function
  TokenType_Variable,
  TokenType_Function,
  TokenType_Separator,
  TokenType_EOF
} TokenType;
inline const char * TokenTypeToString(TokenType tt)
{
  switch(tt)
  {
    case TokenType_Constant: return "Constant";
    case TokenType_Operator: return "Operator";
    case TokenType_Keyword: return "Keyword";
    case TokenType_Symbol: return "Symbol";
    case TokenType_Function: return "Function";
    case TokenType_Variable: return "Variable";
    case TokenType_Separator: return "Separator";
    default: return "Unknown TokenType!";
  }
}

typedef union
{
  const char * str; /**< If phrasem id. */
  long index; /**< If phrasem keyword, operator etc. */
} PhrasemData;

/**
 * @brief   Structure representing phrasem data.
 *
 * This structure is filled with data of phrasem, type etc.
 */
typedef struct phrasem_data
{
  TokenType table; /**< Token type. */
  PhrasemData d; /**< Table line or pointer. */

  long line; /**< Line */
}* Phrasem;
/**
 * @brief   Debug function to print phrasem.
 * @param p       Phrasem to be printed.
 */
inline void PrintPhrasem(Phrasem p)
{
  if(p != NULL) debug("Phrasem: %s %d", TokenTypeToString(p->table), p->d.index);
}


typedef struct exprParserItem
{
    char data;
    struct exprParserItem * next;
} ExprParserItem;

typedef struct exprParserStack
{
    int itemCount;
    struct exprParserItem * first;
} ExprParserStack;

//ExprParserStack EPStack = {.itemCount = 0, .first = NULL};

/** @} */
/*--------------------------------------------------*/
/** @addtogroup Public_types
 * Basic public types, used in whole project.
 * @{
 */

/**
 * @brief   Structure representing arguments.
 *
 * This structure is filled in arguments of main, and then it indicates
 * wanted functionality.
 */
typedef struct
{
  bool help; /**< Help parameter. */
  /* will be added */
} args_t;



/** @} */
/*--------------------------------------------------*/
/** @addtogroup Stack_types
 * Types for stack.
 * @{
 */

/**
 * @brief   Stack item.
 *
 * This structure represents item of stack.
 */
typedef struct stack_item
{
  Phrasem data; /**< Data part. */
  struct stack_item * next; /**< Pointer to next item. */
} StackItem;

/**
 * @brief   Stack head.
 */
typedef struct stack
{
  StackItem * first; /**< Pointer to first. */
} * Stack;

/** @} */
/*--------------------------------------------------*/
/** @addtogroup Return_codes
 * Return codes.
 * @{
 */

#define OK 0
#define SYNTAX_ERROR 1
#define SEMANTIC1_ERROR 2
#define SEMANTIC2_ERROR 3
#define INTERNAL_ERROR 99

/** @} */
/*--------------------------------------------------*/

#endif // TYPES_H
