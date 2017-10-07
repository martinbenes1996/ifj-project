
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
/** @addtogroup Public_types
 * Basic public types, used in whole project.
 * @{
 */


/**
 * @brief   Data type enumeration.
 *
 * This enum contains list of all possible types.
 */
typedef enum{
    Integer,    //in const structure...
    Double,
    String,
    Constant,   //token...
    Function,   //Martin required that...
    Id,
    Operator
} dataType;


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
    // i added new operators
    OpenBracket,  // (
    CloseBracket, // )
} Operators;

/**
 * @brief   Tables enumeration.
 *
 * This enum contains list of certain tables. Use for tokens.
 */
typedef enum{
    IdTab,
    ConstTab,
    OperatorTab,
    KeywordTab,
    /*to be continued*/
} Tables;
inline const char * TablesToString(Tables tb)
{
  switch(tb)
  {
    case IdTab: return "Symbol Table";
    case ConstTab: return "Table of Constants";
    case OperatorTab: return "Operator Table";
    case KeywordTab: return "Keyword Table";
    default: return "Unknown TableType!";
  }
}

/*</Public tables>*/

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


/**
 * @brief   Structure representing phrasem data.
 *
 * This structure is filled with data of phrasem, type etc.
 */
typedef struct phrasem_data
{
  Tables table;
  long index;
}* Phrasem;
/**
 * @brief   Debug function to print phrasem.
 * @param p       Phrasem to be printed.
 */
inline void PrintPhrasem(Phrasem p)
{
  if(p != NULL) debug("Phrasem: %d %d", TablesToString(p->table), p->index);
}

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

#define END_PTR (void *)1

#endif // TYPES_H
