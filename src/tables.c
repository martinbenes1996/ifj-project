
/**
 * @file tables.c
 * @interface tables
 * @authors xbenes49 xbolsh00 xkrato47 xpolan09
 * @date 7th november 2017
 * @brief Source code of tables.
 *
 * This implements interactions with tables (apart from symbol table).
 */


#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "tables.h"
#include "types.h"
#include "err.h"
#include "io.h"
#include "functions.h"


/*-----------------------------------------------------------*/

                //KEYWORD TABLE FUNCTIONS

/**
 * @brief   Keyword table.
 *
 * This array contains list af all possible keywords.
 */
const char keywords[36][9] = {"as", "asc", "declare", "dim", "do", "double",
  "else", "end", "chr", "function", "if", "input", "integer", "length",
  "loop", "print", "return", "scope", "string", "substr", "then", "while",
  "and", "boolean", "continue", "elseif", "exit", "false", "for", "next",
  "not", "or", "shared", "static", "true", "else"};

int isKeyword(const char * word)
{
    int index = -1;

    for(int i = 0; i < 36 ;++i)
        if(strcmp(word, keywords[i]) == 0)
        {
            index = i;
            break;
        }
    return index;
}

const char *Keyword2Str(Phrasem p) {
    if (p->table != TokenType_Keyword) return NULL;

    switch(p->d.index) {
        case 0: return "as";
        case 1: return "asc";
        case 2: return "declare";
        case 3: return "dim";
        case 4: return "do";
        case 5: return "double";
        case 6: return "else";
        case 7: return "end";
        case 8: return "chr";
        case 9: return "function";
        case 10: return "if";
        case 11: return "input";
        case 12: return "integer";
        case 13: return "length";
        case 14: return "loop";
        case 15: return "print";
        case 16: return "return";
        case 17: return "scope";
        case 18: return "string";
        case 19: return "substr";
        case 20: return "then";
        case 21: return "while";
        case 22: return "and";
        case 23: return "boolean";
        case 24: return "continue";
        case 25: return "elseif";
        case 26: return "exit";
        case 27: return "false";
        case 28: return "for";
        case 29: return "next";
        case 30: return "not";
        case 31: return "or";
        case 32: return "shared";
        case 33: return "static";
        case 34: return "true";
        case 35: return "else";

        default: 
            return "???";
    }
}

/*-----------------------------------------------------------*/

              //OPERATOR TABLE FUNCTIONS

#define OPERATOR_ARRAY_SIZE 17
/**
 * @brief   Operator table.
 *
 * This array contains list af all possible operators.
 * DO NOT CHANGE THE ORDER OF THINGS IN IT!! Dollar and i are important.
 */
const char operators[OPERATOR_ARRAY_SIZE][3] = {"+", "-", "\\", "*", "/",
                                                "(", ")", "i", "$", "=",
                                                "<>", "<", "<=", ">", ">=",
                                                ",", ";"};

// returns -1 -> fail, index into array -> found
#define TABLE_ERROR -1
long getOperatorId(const char * word)
{
  if(word == NULL) return TABLE_ERROR;

  #ifdef OPERATOR_TABLE_DEBUG
    debug("Searching for %s", word);
  #endif

  int i = 0;
  for(;i < OPERATOR_ARRAY_SIZE;++i)
  {
    if(!strcmp(operators[i], word))
    {
      #ifdef OPERATOR_TABLE_DEBUG
        debug("Found!");
      #endif
      return i;
    }
  }


  #ifdef OPERATOR_TABLE_DEBUG
    debug("Not found!");
  #endif
  return -1;

}
#undef TABLE_ERROR

const char * Operator2Str(Phrasem p)
{
    if(p->table != TokenType_Operator) return NULL;

    switch(p->d.index)
    {
        case 0: return "+";
        case 1: return "-";
        case 2: return "\\";
        case 3: return "*";
        case 4: return "/";
        case 5: return "(";
        case 6: return ")";
        case 7: return "i";
        case 8: return "$";
        case 9: return "=";
        case 10: return "<>";
        case 11: return "<";
        case 12: return "<=";
        case 13: return ">";
        case 14: return ">=";
        case 15: return ",";
        case 16: return ";";

        default: 
            return "???";
    }


}

/*************************************************************/

                    //TABLE OF CONSTANTS

/*************************************************************/

//                  CONSTANT TABLE DATA


#define STARTING_CHUNK_CONSTANTS 10   //size of initialised arrays
#define RESIZE_RATE_CONSTANTS 30      //how much should it resize: +30


/**
 * @brief   Structure representing characteristics of constants.
 *
 * This structure is filled with type and value of the constant.
 * Value is represented by union.
 */
struct constant{
    DataType type;
    DataUnion data;
};

/**
 * @brief   Structure representing table of constants.
 *
 * This structure is filled with structures representing constants.
 * It knows its own size and resizes automatically by (+10?) entities.
 */
typedef struct constantArray{
    size_t arr_size;    //array size
    size_t count;           //number of entities in array
    struct constant * arr;  //non variadic array of constants (its easier)
} ConstArray;

static ConstArray consttable = {.arr_size = 0, .count = 0, .arr = NULL};

/*-----------------------------------------------------------*/

                //CONST TABLE FUNCTIONS


bool constTableResize(void);

/*-----------------------------------------------------------*/

                    //FUNCTION BODY

/**
 * @brief   Initiation of table of constants.
 *
 * This function allocates table os a starting size.
 * @returns true -> ok, false -> error.
 */
bool constTableInit(void)
{
    //allocation of the table
    if( (consttable.arr = malloc(STARTING_CHUNK_CONSTANTS * sizeof(struct constant)))  == NULL)
    {
        setErrorType(ErrorType_Internal);
        setErrorMessage("constantTableInit: could not allocate memory");
        return false;
    }
    consttable.arr_size = STARTING_CHUNK_CONSTANTS;
    consttable.count = 3;

    // setting first three constants to 0, 0.0 and "" (empty string)
    consttable.arr[0].type = DataType_Integer;
    consttable.arr[0].data.ivalue = 0;
    consttable.arr[1].type = DataType_Double;
    consttable.arr[1].data.dvalue = 0.0;
    consttable.arr[2].type = DataType_String;
    if((consttable.arr[2].data.svalue = calloc(1, sizeof(char))) == NULL)
    {
        setErrorType(ErrorType_Internal);
        setErrorMessage("constantTableInit: could not allocate memory");
        return false;
    }


    for(size_t i = 3; i < consttable.arr_size ;++i)         //not necessary
    {
        consttable.arr[i].type = DataType_Unknown;
    }

    return true;
}
/**
 * @brief   Destroys table of constants.
 *
 * This function frees the whole table.
 */
void constTableFree(void)
{
    // freeing strings of the constants
    for(size_t i = 2;i < consttable.count;++i)
    {
        if(consttable.arr[i].type == DataType_String)
            free(consttable.arr[i].data.svalue);
    }
    consttable.arr_size = 0;
    consttable.count = 0;

    //freeing array of constants
    free(consttable.arr);
}
/**
 * @brief   Reallocs array of constants.
 *
 * This function increases size of table of constants.
 * @returns true -> ok, false -> fail.
 */
bool constTableResize(void)
{
    if( (consttable.arr = realloc(consttable.arr, (consttable.arr_size + RESIZE_RATE_CONSTANTS)
                                                    * sizeof(struct constant)))  == NULL)
    {
        setErrorType(ErrorType_Internal);
        setErrorMessage("constantTableResize: could not allocate memory");
        return false;
    }
    consttable.arr_size = consttable.arr_size + RESIZE_RATE_CONSTANTS;

    return true;
}
/**
 * @brief   Inserts a constant in the array.
 *
 * @param type      type of the constant
 * @param uni       data of the constant
 * @returns index into the array -> ok, -1 -> fail.
 */
int constInsert(DataType type, DataUnion uni)
{
    //adding constant into array
    if(type == DataType_Integer)
    {
      #ifdef CONSTANT_TABLE_DEBUG
        debug("Insert int %d to index %d", uni.ivalue, consttable.count);
      #endif
        consttable.arr[consttable.count].type = DataType_Integer;
        consttable.arr[consttable.count].data.ivalue = uni.ivalue;
    }
    else if(type == DataType_Double)
    {
      #ifdef CONSTANT_TABLE_DEBUG
        debug("Insert double %f to index %d", uni.dvalue, consttable.count);
      #endif
        consttable.arr[consttable.count].type = DataType_Double;
        consttable.arr[consttable.count].data.dvalue = uni.dvalue;
    }
    else if(type == DataType_String)
    {
      #ifdef CONSTANT_TABLE_DEBUG
        debug("Insert string %s to index %d", uni.svalue, consttable.count);
      #endif
        consttable.arr[consttable.count].type = DataType_String;
        if((consttable.arr[consttable.count].data.svalue = malloc(strlen(uni.svalue) * sizeof(char)
                                                                    + sizeof(char))) == NULL)

        {
            setErrorType(ErrorType_Internal);
            setErrorMessage("constantTableInsert: could not allocate memory");
            return false;
        }
        strcpy(consttable.arr[consttable.count].data.svalue, uni.svalue);
    }
    consttable.count++;



    //resizing if needed
    if(consttable.count == consttable.arr_size)
        if(!constTableResize()) return -1;

    return consttable.count-1;     //count is an index of first empty slot
}
/**
 * @brief   Returns type of the constant on a given index.
 *
 * @param index     index into the array of constants
 * @returns type of the constant -> ok, DtatType_Unknown -> fail.
 */
DataType findConstType(size_t index)
{
    if(index >= consttable.count) return DataType_Unknown;
    return consttable.arr[index].type;
}
/**
 * @brief   Returns integer value of the constant on a given index.
 *
 * @param index     index into the array of constants
 * @returns ivalue of the constant -> ok, random number -> fail.
 */
int getIntConstValue(size_t index)
{
    return consttable.arr[index].data.ivalue;
}
/**
 * @brief   Returns double value of the constant on a given index.
 *
 * @param index     index into the array of constants
 * @returns dvalue of the constant -> ok, random number -> fail.
 */
double getDoubleConstValue(size_t index)
{
    return consttable.arr[index].data.dvalue;
}
/**
 * @brief   Returns char pointer on the svalue of constant on a given index.
 *
 * @param index     index into the array of constants
 * @returns char pointer on the string -> ok, random pointer -> fail.
 */
char * getStringConstValue(size_t index)
{
    return consttable.arr[index].data.svalue;
}
/**
 * @brief   Returns index to default value for integer.
 *
 * @returns index to default value for integer.
 */
size_t getIntDefaultValue()
{
    return 0;
}
/**
 * @brief   Returns index to default value for double.
 *
 * @returns index to default value for double.
 */
size_t getDoubleDefaultValue()
{
    return 1;
}
/**
 * @brief   Returns index to default value for string.
 *
 * @returns index to default value for string.
 */
size_t getStringDefaultValue()
{
    return 2;
}
/**
 * @brief   Changes the value of a constant on given index.
 *          You have to know the type of constant before you use this. (constants cannot be retyped)
 *
 * @param index     index into the array of constants
 * @param value     data union with value
 * @returns true -> ok, false -> fail.
 */
bool changeConstValue(size_t index, DataUnion value)
{
    if(consttable.arr[index].type == DataType_Integer)
    {
        consttable.arr[index].data.ivalue = value.ivalue;
    }
    else if(consttable.arr[index].type == DataType_Double)
    {
        consttable.arr[index].data.dvalue = value.dvalue;
    }
    else if(consttable.arr[index].type == DataType_String)
    {
        free(consttable.arr[index].data.svalue);
        consttable.arr[index].data.svalue = malloc(sizeof(char) * strlen(value.svalue) + sizeof(char));
        if(consttable.arr[index].data.svalue == NULL)
        {
            setErrorType(ErrorType_Internal);
            setErrorMessage("constantTable: ChangeValue: could not allocate memory");
            return false;
        }
        strcpy(consttable.arr[index].data.svalue, value.svalue);
        //consttable.arr[index].data.svalue = value.svalue;             <<-- if i should free pointers
    }
    else return false;

    return true;
}
