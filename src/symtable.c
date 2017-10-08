
/**
 * @file symtable.c
 * @interface symtable
 * @authors xbenes49 xbolsh00 xkrato47 xpolan09
 * @date 6th november 2017
 * @brief Symbol table source code
 *
 * This implements interactions with symbol table.
 */

#include "symtable.h"
#include "types.h"
#include "err.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

    /*Work in progress, read with caution.*/

/*NOTES:
    --malloc memory needed to store string from dataUnion in various functions
      (i guess there is no max length of a constant)
*/


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

                //VARIABLE TABLE DATA - may be useless

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

                //STACK OF FRAMES DATA

/**
 * @brief   Structure representing a hash table of variables of a function.
 *
 * This structure contains index into array of functions, its size, number of entities
 * and variadic array of variables (identificators). Will resize automatically.
 * It is also an entity of stack.
 */
typedef struct symbolTableFrame{
    size_t functionIndex;
    struct symbolTableFrame * next;
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
    SymbolTableFrame * first;   //malloc array, potom realokovat
} SymbolTable;

//initialisation of symbol table
static SymbolTable symtable = {.first = NULL};




/*************************************************************/

                //SYMBOL TABLE FUNCTIONS

/*************************************************************/


                    //DECLARATIONS


#define STARTING_CHUNK 10   //size of initialised arrays
#define PORTION_OF_TABLE 2  //when should table resize (count > arr_size/PORTION_OF_TABLE)
#define RESIZE_RATE 2       //how much should it resize


/**
 * @brief   Manages two hash functions.
 *
 * This function returns an index into hash table.
 * Has multiple purposes:
 *      For function find finds a record or returns
 *      an empty spot (symbol is not there).
 *      For function add finds a record or not.
 * @param frame  pointer to a frame
 * @param name   name of the symbol
 * @returns Index into hash table.
 */
unsigned int hashCentral(SymbolTableFrame * frame, const char * name);

/**
 * @brief   Rehash function - colision solver.
 *
 * This function finds another index for a name
 * in case of a colision (+3 linear probing).
 * @param index  an index into hash table
 * @returns A +3 number.
 */
unsigned int rehashFunction(unsigned int index);

/**
 * @brief   Main hash function.
 *
 * This function maps a string into a number.
 * Cannot deal with string containing NULL.
 * @param name  name string
 * @returns A number.
 */
unsigned int hashFunction(const char * name);

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
struct variable * frameFindSymbol(SymbolTableFrame * frame, const char * name);

/**
 * @brief   Adds variable into frame.

 * @param pframe  pointer to a pointer to a frame (frameResize needs it)
 * @param name    name of the variable
 * @returns True if successful, false if not.
*/
bool frameAddSymbol(SymbolTableFrame ** pframe, const char * name);

/**
 * @brief   Changes value of a variable.

 * @param frame   pointer to a frame
 * @param name    name of the variable
 * @param type    type of the variable
 * @param value   new value of the variable
 * @returns True if successful, false if not.
*/
//bool frameChangeValue(SymbolTableFrame * frame, const char * name, DataType type, DataUnion value);

/**
 * @brief   Adds variable type.

 * @param frame   pointer to a frame
 * @param name    name of the variable
 * @param type    type of the variable
 * Types of symbols cannot be rewritten (atleast i think so)!
 * @returns True if successful, false if not.
*/
bool frameAddSymbolType(SymbolTableFrame * frame, const char * name, DataType type);

/*-----------------------------------------------------------*/

                    //FUNCTION BODY


//debug function
void EndHash(const char * msg, ErrorType errtype)
{
  if(msg != NULL)
  {
    #ifdef SYMTABLE_DEBUG
      debug(msg);
    #endif
    setErrorType(errtype);
    setErrorMessage(msg);
  }
  else
  {
    #ifdef SYMTABLE_DEBUG
      debug("End Hash.");
    #endif
  }
}
/*interface function
int varInsert(const char * name)
{


    return ;
}*/
                /*-----HASH FUNCTIONS-----*/

unsigned int hashCentral(SymbolTableFrame * frame, const char * name)
{
    unsigned int hashNumber;

    if(name == NULL)
    {
        EndHash("Hash: hashCentral: pointer to NULL as a name of symbol", ErrorType_Internal);
        return 0;
    }

    hashNumber = hashFunction(name) % frame->arr_size;

    //maximal number of rehashes is the size of table -1
    for(size_t i = 0; i < (frame->arr_size - 1) ;++i)
    {
        //not found or found an empty spot to save symbol
        if(frame->arr[hashNumber].name == NULL)
            return hashNumber;
        //found a record
        else if(strcmp(frame->arr[hashNumber].name, name) == 0)
            return hashNumber;
        //found a different record -> continues finding
        else
            hashNumber = rehashFunction(hashNumber) % frame->arr_size;
    }
    //table cant be full, it hopefully resizes automatically so it should not reach this
    return hashNumber;
}

unsigned int rehashFunction(unsigned int index)
{
    return index + 3;
}

unsigned int hashFunction(const char * name)
{
    unsigned int h=0;
    const unsigned char *p;

    for(p=(const unsigned char*)name;*p!='\0'; p++)
        h = 65599*h + *p;

    return h;
}
/*-----------------------------------------------------------*/

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
                frame->arr[i].type = DataType_Unknown;
            }
        }
    else
    {
        EndHash("Frame: frameInit: could not allocate memory", ErrorType_Internal);
        return NULL;
    }

    #ifdef SYMTABLE_DEBUG
        debug("New frame has been initialised.");
    #endif
    return frame;
}

void frameFree(SymbolTableFrame * frame)
{
    if(frame == NULL) return;

    //frees strings in frame
    for(size_t i = 0;i < frame->arr_size;i++)
    {
        free(frame->arr[i].name);

        //if(frame->arr[i].type == DataType_String)
            //free(frame->arr[i].value.svalue);
    }

    //destroys a frame
    free(frame);

    #ifdef SYMTABLE_DEBUG
        debug("Frame was freed.");
    #endif
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
        frame->count = frameEntityCount(frame2);
        frame->functionIndex = frame2->functionIndex;
        frame->next = frame2->next;

        //going through all variables in table and copying variables
        for(size_t i=0;i < frameTableSize(frame2);++i)
        {
            /*computes a new hash in a new frame*/
            if(frame2->arr[i].name == NULL) continue;
            hashNumber = hashCentral(frame, frame2->arr[i].name);

            /*---------------------------------------------------------------------------*/
            frame->arr[hashNumber].type = frame2->arr[i].type;

            //allocation of memory for a name string in new table
            if((frame->arr[hashNumber].name = malloc(sizeof(char) * strlen(frame2->arr[i].name)
                                                    + sizeof(char))) != NULL)
            {
                strcpy(frame->arr[hashNumber].name, frame2->arr[i].name);
            }
                else
                {
                    EndHash("Frame: frameResize: could not allocate memory for a symbol name",
                                    ErrorType_Internal);
                    return NULL;
                }

/*
            //copying various types of values in union-----------------------------------
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
            else if(frame2->arr[i].type == DataType_Function)   //functions are identified by integer (?)
                frame->arr[hashNumber].value.ivalue = frame2->arr[i].value.ivalue;
            //</copy union---------------------------------------------------------------->
*/
        }
        //destroys old frame
        frameFree(frame2);
    }
    else
    {
        EndHash("Frame: frameResize: could not resize table", ErrorType_Internal);
    }

    #ifdef SYMTABLE_DEBUG
        debug("Frame was resized.");
    #endif
    return frame;
}

struct variable * frameFindSymbol(SymbolTableFrame * frame, const char * name)
{
    size_t hashNumber;

    if(name == NULL) return NULL;
    hashNumber = hashCentral(frame, name);

    if(frame->arr[hashNumber].name == NULL) return NULL; //not found
        else return &frame->arr[hashNumber]; //found

    return &frame->arr[hashNumber];
}

bool frameAddSymbol(SymbolTableFrame ** pframe, const char * name)
{
    SymbolTableFrame * frame = *pframe;
    size_t hashNumber;
    if(name == NULL)
    {
        EndHash("SymTabFrame: frameAddSymbol: name of symbol is NULL", ErrorType_Internal);
        return false;
    }

    hashNumber = hashCentral(frame, name);

    if(frame->arr[hashNumber].name == NULL) //not found -> can be added
    {
        if((frame->arr[hashNumber].name = malloc(sizeof(char) * strlen(name)
                                                    + sizeof(char))) != NULL)
        {
            strcpy(frame->arr[hashNumber].name, name);
        }
            else
            {
                EndHash("SymTabFrame: frameAddSymbol: could not allocate memory for symbol name",
                                    ErrorType_Internal);
                return false;
            }
    }
        else return false; //found -> cant be added

    //increase the amount of symbols in table and resizes if needed
    frame->count++;
    if(frame->count > frame->arr_size/PORTION_OF_TABLE)
        *pframe = frameResize(RESIZE_RATE * frame->arr_size, frame);

    #ifdef SYMTABLE_DEBUG
        debug("New symbol added into frame.");
    #endif
    return true;
}

bool frameAddSymbolType(SymbolTableFrame * frame, const char * name, DataType type)
{
    struct variable * var;
    var = frameFindSymbol(frame, name);
    if(var == NULL) return false;   //symbol not found

    if(var->type == DataType_Unknown)   //if symbol type has not been set yet, do it
        var->type = type;
    else return false;  //cannot retype symbol

    #ifdef SYMTABLE_DEBUG
        debug("Symbol type was added.");
    #endif
    return true;
}
/*MIGHT NOT BE NEEDED, SYMBOLS DONT NEED VALUE
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
*/


/*-----------------------------------------------------------*/

                //STACK FUNCTIONS - not tested

    /*WILL ADD MORE WHEN I REALISE WHAT YOU WANT FROM ME*/

bool pushOntoSymtableStack(size_t functionId)
{
    SymbolTableFrame * frame;
    frame = frameInit(STARTING_CHUNK);
    if(frame == NULL)
    {
        EndHash("Symtable stack: pushOntoSymtableStack: could not initialise frame", ErrorType_Internal);
        return false;
    }
    //set function id and connect list
    frame->functionIndex = functionId;
    frame->next = symtable.first;
    symtable.first = frame;

    #ifdef SYMTABLE_DEBUG
        debug("Frame was created and pushed onto a symtable stack.");
    #endif
    return true;
}
//i think we will not need that frame so i dont have to return it
//if we need it i will fix it
bool popFromSymtableStack(void)
{
    if(symtable.first == NULL) return false;

    SymbolTableFrame * frame = symtable.first;
    symtable.first = symtable.first->next;
    frameFree(frame);

    #ifdef SYMTABLE_DEBUG
        debug("Frame was destroyed from top of the symtable stack.");
    #endif
    return true;
}
void clearSymtableStack()
{
    while(popFromSymtableStack() != false)
    {}

    #ifdef SYMTABLE_DEBUG
        debug("Symtable stack was cleared.");
    #endif
}
