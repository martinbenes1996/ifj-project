
/**
 * @file symtable.c
 * @interface symtable
 * @authors xbenes49 xbolsh00 xkrato47 xpolan09
 * @date 6th november 2017
 * @brief Symbol table source code
 *
 * This implements interactions with tables.
 */

#include "symtable.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

    /*Work in progress, read with caution.*/

/*NOTES:
    --malloc memory needed to store string from dataUnion in various functions
      (i guess there is no max length of a constant)
*/

/*----------------------------------------------------------*/
/**
 * @brief   Keyword table.
 *
 * This array contains list af all possible keywords.
 */
const char keywords[35][9] = {"as", "asc", "declare", "dim", "do", "double", "else",
                                     "end", "chr", "function", "if", "input", "integer",
                                     "length", "loop", "print", "return", "scope", "string",
                                     "substr", "then", "while", "and", "boolean", "continue",
                                     "elseif", "exit", "false", "for", "next", "not", "or",
                                     "shared", "static", "true"};

//----- yep, static, but this will be in symtable.c, so no one could reach it

//I think it will have to be accessible for syntax analysis
//you will need to know what keyword is in the token (there will be just an index into this array)

/*-----------------------------------------------------------*/

        //CONSTANT DATA TABLE (not sure if public or private yet)

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
struct constantArray{
    size_t arr_size;    //array size
    size_t count;           //number of entities in array
    struct constant arr[];
} ConstArray;

/*----------------------------------------------------------------*/

                    //FUNCTION TABLE DATA

/**
 * @brief   Structure representing type and name of parametres.
 *
 * List.
 */
struct paramFce{
    DataType type;
    char * name;
    struct paramFce *next;
};

/**
 * @brief   Structure representing function data.
 *
 * This structure is filled with informations about declared functions.
 * Contains structure with info about parametres.
 */
struct function{
    char * name;
    DataType type;
    size_t numberOfParam;
    struct paramFce * param;
};

/**
 * @brief   Structure representing (hash?) table of function names/parametres.
 *
 * This structure is filled with functions.
 * It knows its size and number of entities. Will resize automatically.
 */
typedef struct functionArray{
    size_t arr_size;    //array size
    size_t count;           //number of entities in array
    struct function arr[];
} FunctionArray;

/*-----------------------------------------------------------*/

                //VARIABLE TABLE DATA

/**
 * @brief   Structure representing a variable.
 *
 * This structure contains type, name and value of a variable.
 */
struct variable{
    DataType type;
    char * name;
    DataUnion value;
};
/*-----------------------------------------------------------*/

                //STACK OF MODULES DATA

/**
 * @brief   Structure representing a hash table of variables of a function.
 *
 * This structure contains index into array of functions, its size, number of entities
 * and variadic array of variables (identificators). Will resize automatically.
 * It is also an entity of stack.
 */
typedef struct symbolTableFrame{
    int functionIndex;
    struct symboltable_frame * next;
    size_t arr_size;    //array size
    size_t count;           //number of entities in array
    struct variable arr[];
} SymbolTableFrame;

/**
 * @brief   Structure representing stack made by array (or is list better?).
 *
 * This structure contains an array (pointer), number of allocated entities
 * and index of the first free entity.
 * Will resize automatically.
 */
typedef struct symbolTable{
    size_t size;
    size_t count;
    struct symbolTableFrame * frame;   //malloc array, potom realokovat
} SymbolTable;

//initialisation of symbol table
SymbolTable symtable = {.size = 0, .count = 0, .frame = NULL};




/*************************************************************/

                //SYMBOL TABLE FUNCTIONS

/*************************************************************/


                    //DECLARATIONS
unsigned int hashFunction(const char *str);

//size of initialised arrays
#define STARTING_CHUNK 10

/**
 * @brief   Initialisation of hash symbol table.
 *
 * This function creates a frame and sets its size on 10.
 * Returns NULL when unsuccessful, otherwise
 * returns pointer to the structure.
 * @param size  size of the table
 * @returns Pointer to array of functions or NULL.
 */
SymbolTableFrame * frameInit(size_t size);

/**
 * @brief   Destruction of hash symbol table.
 *
 * This function frees memory of a frame.
 */
void frameFree(SymbolTableFrame * frame);

/**
 * @brief   Returns the number of entities in table.
 *
 * @param frame pointer to a frame
 * @returns Number of entities in table.
 */
size_t frameEntityCount(SymbolTableFrame * frame);

/**
 * @brief   Returns the size of a table.
 *
 * @param frame pointer to a frame
 * @returns Size of the table.
 */
size_t frameTableSize(SymbolTableFrame * frame);

/**
 * @brief   Empties a symbol table frame.
 *
 * @param frame     pointer to a frame
 */
//void frameClear(SymbolTableFrame * frame);          <---DELETE

/**
 * @brief   Resizes a table, deletes the old one.

 * Creates a new resized table from the previous one.
 * Returns NULL when unsuccessful + frame2 is unchanged,
 * returns pointer to a new table and frame2 is deleted when successful.
 * @param newsize   new size of the table
 * @param frame2    pointer to a source table
 * @returns Pointer to a new frame or NULL.
*/
SymbolTableFrame * frameResize(size_t newsize, SymbolTableFrame * frame2);

/**
 * @brief   Finds variable with specified name.

 * Returns pointer to a variable or NULL
 * @param frame   pointer to a frame
 * @param name    name of the variable
 * @returns Pointer to a variable or NULL.
*/
//struct variable * frameFind(SymbolTableFrame * frame, const char * name);

/**
 * @brief   Adds variable into frame.

 * @param frame   pointer to a frame
 * @param name    name of the variable
 * @returns True if successful, false if not.
*/
//bool frameAddSymbol(SymbolTableFrame * frame, const char * name);

/**
 * @brief   Changes value of a variable.

 * @param frame   pointer to a frame
 * @param name    name of the variable
 * @param type    type of the variable
 * @param value   new value of the variable
 * @returns True if successful, false if not.
*/
bool frameChangeValue(SymbolTableFrame * frame, const char * name, DataType type, DataUnion value);

/*-----------------------------------------------------------*/

                    //FUNCTION BODY


SymbolTableFrame * frameInit(size_t size)
{
    SymbolTableFrame *frame = NULL;

    //allocation of the table with variadic array
    if( (frame = malloc(sizeof(SymbolTableFrame) + size * sizeof(struct variable)))  != NULL)
        {
            //inicializace prvku vytvorene tabulky
            frame->arr_size = size;
            frame->count = 0;
            frame->functionIndex = -1;
            frame->next = NULL;

            //fixing potential frees of empty entities
            for(size_t i = 0; i < size ;++i)
            {
                frame->arr[i].name = NULL;
                frame->arr[i].type = DataType_Integer;
            }
        }

    return frame;
}

void frameFree(SymbolTableFrame * frame)
{
    if(frame == NULL) return;

    //frees strings in frame
    for(size_t i = 0;i < frame->arr_size;i++)
    {
        free(frame->arr[i].name);

        if(frame->arr[i].type == DataType_String)
            free(frame->arr[i].value.svalue);
    }

    //destroys a frame
    free(frame);

    return;
}

size_t frameTableSize(SymbolTableFrame * frame)
{
    return frame->arr_size;
}

size_t frameEntityCount(SymbolTableFrame * frame)
{
    return frame->count;
}

SymbolTableFrame * frameResize(size_t newsize, SymbolTableFrame * frame2)
{
    SymbolTableFrame *frame = NULL;
    unsigned int hashNumber;

    if(frame2 == NULL) return NULL;

    if((frame = frameInit(newsize)) != NULL)
    {
        frame->count = frame2->count;
        frame->functionIndex = frame2->functionIndex;
        frame->next = frame2->next;

        //going through all variables in table and copying variables
        for(size_t i=0;i < frame2->arr_size;++i)
        {
            /*POTREBA ZMENIT CHOVANI HASH FCE*/
//hashNumber = hashFunction(frame2->arr[i].name) % newsize;
hashNumber = 0; //prekladac rve

            /*---------------------------------------------------------------------------*/
            frame->arr[hashNumber].type = frame2->arr[i].type;

            //allocation of memory for a name string in new table
            if((frame->arr[hashNumber].name = malloc(sizeof(char) * strlen(frame2->arr[i].name)
                                                    + sizeof(char))) != NULL)
            {
                strcpy(frame->arr[hashNumber].name, frame2->arr[i].name);
            }
                else return NULL;


            /*copying various types of values in union-----------------------------------*/
            if(frame2->arr[i].type == DataType_String)
            {
                //allocation of memory for a value string in union
                if((frame->arr[hashNumber].value.svalue = malloc(sizeof(char) * strlen(frame2->arr[i].value.svalue)
                                                        + sizeof(char))) != NULL)
                {
                    strcpy(frame->arr[hashNumber].value.svalue, frame2->arr[i].value.svalue);
                }
                    else return NULL;
            }
            else if(frame2->arr[i].type == DataType_Integer)
                frame->arr[hashNumber].value.ivalue = frame2->arr[i].value.ivalue;
            else if(frame2->arr[i].type == DataType_Double)
                frame->arr[hashNumber].value.dvalue = frame2->arr[i].value.dvalue;
            else    //functions are identified by integer (?)
                frame->arr[hashNumber].value.ivalue = frame2->arr[i].value.ivalue;
            //</copy union---------------------------------------------------------------->

        }
        //destroys old frame
        frameFree(frame2);
    }

    return frame;
}
/*
struct variable * frameFind(SymbolTableFrame * frame, const char * name)
{
    size_t hashNumber;

    hashNumber = hashFunction(name);

    WILL BE DONE, NEED HASH FUNCTION

    return ;
}

bool frameAddSymbol(SymbolTableFrame * frame, const char * name)
{
    if(frameFind(frame, name) == NULL) return false;

    WILL BE DONE, NEED HASH FUNCTION

    frame->count++;

    return ;
}*/

bool frameChangeValue(SymbolTableFrame * frame, const char * name, DataType type, DataUnion value)
{
    struct variable * var = NULL;

    //cannot change value if there is no symbol
//if((var = frameFind(frame, name)) == NULL) return false;
    (void *)name;
    (void *)frame;
    //types must be same
    if(var->type != type) return false;

    //Changes the value
    if(type == DataType_Integer || type == DataType_Function)
        var->value.ivalue = value.ivalue;
    else if(type == DataType_Double)
        var->value.dvalue = value.dvalue;
    else
        var->value.svalue = value.svalue;

    return true;
}

/*
STACK:
    push
    pop
    ...
FRAME:
    init
*/










/*-----------------------------------------------------------*/

                //CONST TABLE FUNCTIONS

     /*the rest of this file is most likely wrong and will be reworked*/

                    //FUNCTION TABLE FUNCTIONS

/**
 * @brief   Initialisation of table of functions.
 *
 * This function creates array of functions.
 * Returns NULL when unsuccessful, otherwise
 * returns pointer to array of size (10?).
 * @returns Pointer to array of functions.
 */
//FunctionData * functionInit(void);
// --- the same as what I type above

/**
 * @brief   Insert function.
 *
 * This function inserts function into array of functions.
 * Returns -1 when unsuccessful, otherwise returns 1
 * @param ptr    array pointer.
 * @param name   name of the function.
 * @returns -1 -> failure, 1 -> success.
 */
//int functionInsert(FunctionData * ptr, char * name);

/**
 * @brief   Insert parameter into an existing function.
 *
 * This function inserts parameter into a function.
 * Returns -1 when unsuccessful (function not found,...), otherwise returns 1
 * @param name   name of the function.
 * @param type   type of parameter.
 * @param paramName    name of parametre.
 * @returns -1 -> failure, 1 -> success.
 */
int paramInsert(char * name, DataType type, char * paramName);


/*TO DO:
    int numberOfParam(FunctionData * ptr, const char * name);
    freeTable
    findFunction
    findParametreName (finds one param of a function, takes index of param)
    findParametreType
    findParametreInd
*/

/*-----------------------------------------------------------*/

                //STACK FUNCTIONS
/*
TO DO: (jednoduseji!)
    init
    push
    pop
    free
    resize

    init_prom
    insert_prom
    change_value
    free
    ...
*/











/*               (IJC 2.proj) RUBBISH - IGNORE!             */

extern unsigned short int flag;

struct htab_listitem{
    char *key;
    size_t data;
    struct htab_listitem *next;
};

struct htab_t{
    size_t arr_size;
    size_t n;
    struct htab_listitem *arr[];
};


/**
 * hashovaci fce pro retezce
 * vraci unsigned int prislusici retezci
 @param str ukazatel na retezec
*/
unsigned int hash_function(const char *str);



/**
 * hleda zaznam retezce key v tabulce t
 * vraci ukazatel na zaznam, ve kterem je retezec ulozen nebo NULL, pokud nenajde
 @param t ukazatel na tabulku
 @param key ukazatel na retezec
*/
struct htab_listitem *htab_find(struct htab_t *t, const char *key);

/**
 * hleda zaznam retezce key v tabulce t
 * vraci ukazatel na zaznam, ve kterem je retezec ulozen,\
   pokud ho nenajde, vlozi ho do tabulky a vrati ukazatel na nej
 * pri neuspesnem vlozeni vraci NULL
 @param t ukazatel na tabulku
 @param key ukazatel na retezec
*/
struct htab_listitem *htab_lookup_add(struct htab_t *t, const char *key);

/**
 * odstrani ulozeny retezec z tabulky t
 * vraci true, pokud byl retezec odstranen nebo false, pokud nebyl nalezen
 @param t ukazatel na tabulku
 @param key hledany retezec
*/
_Bool htab_remove(struct htab_t *t, const char *key);

/**
 * pro vsechny prvky tabulky zavola funkci
 @param t tabulka
 @param (*func)... ukazatel na funkci
*/
void htab_foreach(struct htab_t *t, void (*func)(const char *constkey, size_t *valueptr));

/**
 * nacte slovo a vrati jeho delku
 * v pripade neuspechu vraci EOF
 @param s cilovy retezec pro nactene slovo
 @param maximum maximalni delka slova
 @param f ukazatel na strukturu FILE
*/
int get_word(char *s, int maximum, FILE *f);

/**
 * vypis tabulky - htab_foreach
 @param ret retezec, ktery se ma vypsat
 @param cislo cislo, ktere se ma vypsat
*/
void vypis(const char *ret, size_t *cislo);
