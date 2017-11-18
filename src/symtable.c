
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



/*-----------------------------------------------------------------------------------*/


//                          SYMBOL TABLE DATA

/**
 * @brief   Structure representing a variable.
 *
 * This structure contains type, name and value of a variable.
 */
struct variable{
    DataType type;
    char * name;
    //DataUnion value;
};

/**
 * @brief   Structure representing a hash table of variables of a function.
 *
 * This structure contains index into array of functions, its size, number of entities
 * and variadic array of variables (identificators). Will resize automatically.
 */
typedef struct symbolTableFrame{
    size_t arr_size;        //array size
    size_t count;           //number of entities in array
    struct variable arr[];
} SymbolTableFrame;

/**
 * @brief   Structure representing a function frame.
 *
 * This structure contains type, name, variables and parametres of a function.
 */
typedef struct symbolTable{
    DataType type;
    char *name;
    size_t numberOfParameters;
    struct paramFce *firstParam;
    SymbolTableFrame *variables;
} SymbolTable;

/**
 * @brief   Structure representing hash table consisting of functions.
 *
 * This structure contains informations about declared functions.
 */
typedef struct functionHashTable{
    size_t arr_size;
    size_t count;
    SymbolTable ** arr;
} FunctionHashTable;

//function hash table
static FunctionHashTable functionTable = {.arr_size = 0, .count = 0, .arr = NULL};




/*************************************************************/

                //SYMBOL TABLE FUNCTIONS

/*************************************************************/


                    //DECLARATIONS


#define STARTING_CHUNK 10   //size of initialised arrays
#define PORTION_OF_TABLE 2  //when should table resize (count > arr_size/PORTION_OF_TABLE)
#define RESIZE_RATE 2       //how much should it resize

#define STARTING_CHUNK_FUNCTIONS 10   //size of initialised array of functions
#define PORTION_OF_TABLE_FUNCTIONS 2  //when should function table resize (count > arr_size/PORTION_OF_TABLE_FUNCTIONS)
#define RESIZE_RATE_FUNCTIONS 2       //how much should it resize


unsigned int hashFunctionCentral(const char * name, SymbolTable ** array, size_t arrSize);

unsigned int hashCentral(SymbolTableFrame * frame, const char * name);

unsigned int rehashFunction(unsigned int index);

unsigned int hashFunction(const char * name);

SymbolTableFrame * frameInit(size_t size);

void frameFree(SymbolTableFrame * frame);

size_t frameEntityCount(SymbolTableFrame * frame);

size_t frameTableSize(SymbolTableFrame * frame);

SymbolTableFrame * frameResize(size_t newsize, SymbolTableFrame * frame2);

struct variable * frameFindSymbol(SymbolTableFrame * frame, const char * name);

bool frameAddSymbol(SymbolTableFrame ** pframe, const char * name);

bool frameAddSymbolType(SymbolTableFrame * frame, const char * name, DataType type);

/*------------------------------------------------------------------------------*/

//                          FUNCTION BODY


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

                /*-----HASH FUNCTIONS-----*/

/**
 * @brief   Manages two hash functions. (for hash table of functions)
 *
 * This function returns an index into hash table.
 * Has multiple purposes:
 *      For function find finds a record or returns
 *      an empty spot (symbol is not there).
 *      For function add finds a record or an empty spot for insertion.
 * @param name      name of the function (that will be inserted)
 * @param array     hash table of functions
 * @param arrSize   size of the hash table
 * @returns Index into hash table.
 */
unsigned int hashFunctionCentral(const char * name, SymbolTable ** array, size_t arrSize)
{
    unsigned int hashNumber;

    if(name == NULL || array == NULL || arrSize == 0)
    {
        EndHash("Hash: hashFunctionCentral: pointer to NULL as a name of function", ErrorType_Internal);
        return 0;
    }

    hashNumber = hashFunction(name) % arrSize;

    //maximal number of rehashes is the size of table -1
    for(size_t i = 0; i < (arrSize - 1) ;++i)
    {
        //not found or found an empty spot to save function
        if(array[hashNumber] == NULL)
            return hashNumber;
        //found a record
        else if(strcmp(array[hashNumber]->name, name) == 0)
            return hashNumber;
        //found a different record -> continues finding
        else
            hashNumber = rehashFunction(hashNumber) % arrSize;
    }
    //table cant be full, it hopefully resizes automatically so it should not reach this
    return hashNumber;
}
/**
 * @brief   Manages two hash functions. (for hash table of variables)
 *
 * This function returns an index into hash table.
 * Has multiple purposes:
 *      For function find finds a record or returns
 *      an empty spot (symbol is not there).
 *      For function add finds a record or an empty spot for insertion.
 * @param frame  pointer to a frame
 * @param name   name of the symbol
 * @returns Index into hash table.
 */
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
/**
 * @brief   Rehash function - colision solver.
 *
 * This function finds another index for a name
 * in case of a colision (+3 linear probing).
 * @param index  an index into hash table
 * @returns A +3 number.
 */
unsigned int rehashFunction(unsigned int index)
{
    return index + 3;
}
/**
 * @brief   Main hash function.
 *
 * This function maps a string into a number.
 * Cannot deal with string containing NULL.
 * @param name  name string
 * @returns A number.
 */
unsigned int hashFunction(const char * name)
{
    unsigned int h=0;
    const unsigned char *p;

    for(p=(const unsigned char*)name;*p!='\0'; p++)
        h = 65599*h + *p;

    return h;
}
/*----------------------------------------------------------------------------------*/
/**
 * @brief   Initialisation of hash symbol table.
 *
 * This function creates a frame and sets its size on 10.
 * Returns NULL when unsuccessful, otherwise
 * returns pointer to the structure.
 * @param size  size of the table
 * @returns Pointer to array of functions or NULL.
 */
SymbolTableFrame * frameInit(size_t size)
{
    SymbolTableFrame *frame = NULL;

    //allocation of the table with variadic array
    if( (frame = malloc(sizeof(SymbolTableFrame) + size * sizeof(struct variable)))  != NULL)
        {
            //inicializace prvku vytvorene tabulky
            frame->arr_size = size;
            frame->count = 0;
            //frame->functionIndex = -1;
            //frame->next = NULL;

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
/**
 * @brief   Destruction of hash symbol table.
 *
 * This function frees memory of a frame.
 */
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
/**
 * @brief   Returns the size of a table.
 *
 * @param frame pointer to a frame
 * @returns Size of the table.
 */
size_t frameTableSize(SymbolTableFrame * frame)
{
    return frame->arr_size;
}
/**
 * @brief   Returns the number of entities in table.
 *
 * @param frame pointer to a frame
 * @returns Number of entities in table.
 */
size_t frameEntityCount(SymbolTableFrame * frame)
{
    return frame->count;
}
/**
 * @brief   Resizes a table, deletes the old one.

 * Creates a new resized table from the previous one.
 * Returns NULL when unsuccessful + frame2 is unchanged,
 * returns pointer to a new table and frame2 is deleted when successful.
 * @param newsize   new size of the table
 * @param frame2    pointer to a source table
 * @returns Pointer to a new frame or NULL.
*/
SymbolTableFrame * frameResize(size_t newsize, SymbolTableFrame * frame2)
{
    SymbolTableFrame *frame = NULL;
    unsigned int hashNumber;

    if(frame2 == NULL) return NULL;

    if((frame = frameInit(newsize)) != NULL)
    {
        frame->count = frameEntityCount(frame2);
        //frame->functionIndex = frame2->functionIndex;
        //frame->next = frame2->next;

        //going through all variables in table and copying variables
        for(size_t i=0;i < frameTableSize(frame2);++i)
        {
            /*computes a new hash in a new frame*/
            if(frame2->arr[i].name == NULL) continue;
            hashNumber = hashCentral(frame, frame2->arr[i].name);

            /*---------------------------------------------------------------------------*/
            frame->arr[hashNumber].type = frame2->arr[i].type;
            frame->arr[hashNumber].name = frame2->arr[i].name;
            frame2->arr[i].type = DataType_Unknown;
            frame2->arr[i].name = NULL;
/*
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
*/
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
/**
 * @brief   Finds variable with specified name.

 * Returns pointer to a variable or NULL
 * @param frame   pointer to a frame
 * @param name    name of the variable
 * @returns Pointer to a variable or NULL.
*/
struct variable * frameFindSymbol(SymbolTableFrame * frame, const char * name)
{
    size_t hashNumber;

    if(name == NULL) return NULL;
    hashNumber = hashCentral(frame, name);

    if(frame->arr[hashNumber].name == NULL) return NULL; //not found
        else return &frame->arr[hashNumber]; //found

    return &frame->arr[hashNumber];
}
/**
 * @brief   Adds variable into frame.

 * @param pframe  pointer to a pointer to a frame (frameResize needs it)
 * @param name    name of the variable
 * @returns True if successful, false if not.
*/
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
/**
 * @brief   Adds variable type.

 * @param frame   pointer to a frame
 * @param name    name of the variable
 * @param type    type of the variable
 * Types of symbols cannot be rewritten (atleast i think so)!
 * @returns True if successful, false if not.
*/
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


/*----------------------------------------------------------------------------------------------*/

                    //FUNCTION TABLE DECLARATIONS

SymbolTable ** functionTableInit(size_t size);

SymbolTable * functionFrameInit();

void functionTableFree(size_t size);

void functionFrameFree(SymbolTable * frame);

void functionTableResize(size_t newsize, SymbolTable ** array2);

SymbolTable * findFunction(const char * name);


                    //FUNCTION TABLE FUNCTIONS

/*----------------------------------------------------------------------------------------------*/

/**
 * @brief   Initialisation of hash symbol table for functions.
 *
 * This function initialises a function hash table.
 * Returns NULL when unsuccessful, otherwise
 * returns pointer to the structure.
 * @param size      size of the table
 * @returns Pointer to array of functions or NULL.
 */
SymbolTable ** functionTableInit(size_t size)
{
    SymbolTable ** array = NULL;

    //allocation of the table
    if( (array = malloc(sizeof(SymbolTable *) * size)) != NULL)
    {
        //initialisation of the table
        for(size_t i = 0;i < size;++i)
            array[i] = NULL;
        if(functionTable.arr_size == 0) functionTable.arr_size = size;
    }
    else
    {
        EndHash("functionTable: functionTableInit: could not allocate memory", ErrorType_Internal);
        return NULL;
    }

    #ifdef SYMTABLE_DEBUG
        debug("New array of functions has been initialised.");
    #endif
    return array;
}
/**
 * @brief   Initialisation of a function frame.
 *
 * This function initialises a function frame.
 * Returns NULL when unsuccessful, otherwise
 * returns pointer to the function frame.
 * @returns Pointer to array of functions or NULL.
 */
SymbolTable * functionFrameInit()
{
    SymbolTable * frame = NULL;

    if( (frame = malloc(sizeof(SymbolTable))) != NULL)
    {
        //initialisation of the frame
        frame->firstParam = NULL;
        frame->name = NULL;
        frame->variables = frameInit(STARTING_CHUNK);
        frame->numberOfParameters = 0;
        frame->type = DataType_Unknown;
    }
    else
    {
        EndHash("functionTable: functionFrameInit: could not allocate memory", ErrorType_Internal);
        return NULL;
    }

    #ifdef SYMTABLE_DEBUG
        debug("New function has been initialised.");
    #endif
    return frame;
}
/**
 * @brief   Destruction of hash symbol table.
 *
 * This function frees memory of a function table and resets its values.
 */
void functionTableFree(size_t size)
{
    if(functionTable.arr == NULL) return;

    //frees functions in table
    for(size_t i = 0;i < size;++i)
        functionFrameFree(functionTable.arr[i]);

    //destroys function table
    free(functionTable.arr);

    functionTable.arr_size = 0;
    functionTable.count = 0;
    functionTable.arr = NULL;

    #ifdef SYMTABLE_DEBUG
        debug("Function table was freed.");
    #endif
    return;
}
/**
 * @brief   Destruction of function frame.
 *
 * This function frees memory of a function frame.
 */
void functionFrameFree(SymbolTable * frame)
{
    if(frame == NULL) return;

    //freeing the list of parametres
    while(frame->firstParam != NULL)
    {
        struct paramFce * pom = frame->firstParam;
        frame->firstParam = frame->firstParam->next;
        free(pom->name);
        free(pom);
    }

    //freeing name and variable table
    free(frame->name);
    frameFree(frame->variables);

    //destroys a frame
    free(frame);

    #ifdef SYMTABLE_DEBUG
        debug("Function frame was freed.");
    #endif
    return;
}
/**
 * @brief   Resizes a function hash table, deletes the old one.

 * Creates a new resized table from the previous one.
 * @param newsize   new size of the table
 * @param array2    pointer to a source table
*/
void functionTableResize(size_t newsize, SymbolTable ** array2)
{
    SymbolTable ** array = NULL;
    unsigned int hashNumber;
    size_t count;

    if(array2 == NULL) return;

    if((array = functionTableInit(newsize)) != NULL)
    {
        //going through all functions in table and copying function pointers
        for(size_t i=0;i < functionTable.arr_size;++i)
        {
            /*computes a new hash in a new frame*/
            if(array2[i] == NULL) continue;
            hashNumber = hashFunctionCentral(array2[i]->name, array, newsize);

            array[hashNumber] = array2[i];
            array2[i] = NULL;
        }
        count = functionTable.count;
        //destroys old table
        functionTableFree(functionTable.arr_size);
        functionTable.arr = array; //saves new array
        functionTable.arr_size = newsize;
        functionTable.count = count;
    }
    else
    {
        EndHash("Function Frame: functionTableResize: could not resize table", ErrorType_Internal);
    }

    #ifdef SYMTABLE_DEBUG
        debug("Function table was resized.");
    #endif
    return;
}
/**
 * @brief   Finds a function in function table.

 * Returns pointer to a function or NULL
 * @param name    name of the function
 * @returns Pointer to a function or NULL.
*/
SymbolTable * findFunction(const char * name)
{
    //not allocated array
    if(functionTable.arr_size == 0) return NULL;

    size_t hashNumber;

    if(name == NULL) return NULL;
    hashNumber = hashFunctionCentral(name, functionTable.arr, functionTable.arr_size);

    if(functionTable.arr[hashNumber] == NULL) return NULL; //not found
        else return functionTable.arr[hashNumber];         //found

    return functionTable.arr[hashNumber];
}
/**
 * @brief   Adds function into function hash table.
 *
 * @param name    name of the function
 * @returns True if successful, false if not.
*/
bool addFunction(const char * name)
{
    //initialisation when used for the first time
    if(functionTable.arr_size == 0) functionTable.arr = functionTableInit(STARTING_CHUNK_FUNCTIONS);

    if(name == NULL)
    {
        EndHash("FunctionTable: AddFunction: name of symbol is NULL", ErrorType_Internal);
        return false;
    }

    //increase the amount of symbols in table and resizes if needed
    if((functionTable.count + 1) > functionTable.arr_size/PORTION_OF_TABLE_FUNCTIONS)
        functionTableResize(RESIZE_RATE_FUNCTIONS * functionTable.arr_size, functionTable.arr);


    size_t hashNumber;
    hashNumber = hashFunctionCentral(name, functionTable.arr, functionTable.arr_size);

    if(functionTable.arr[hashNumber] == NULL) //not found -> can be added
    {
        functionTable.arr[hashNumber] = functionFrameInit();

        if((functionTable.arr[hashNumber]->name = malloc(sizeof(char) * strlen(name)
                                                    + sizeof(char))) != NULL)
        {
            strcpy(functionTable.arr[hashNumber]->name, name);
        }
        else
        {
            EndHash("FunctionTable: AddFunction: could not allocate memory for symbol name",
                                    ErrorType_Internal);
            return false;
        }
    }
        else return false; //found -> cant be added

    functionTable.count++;

    #ifdef SYMTABLE_DEBUG
        debug("New function added into table.");
    #endif
    return true;
}



/*---------------------------------------------------------------------------------------*/


/**
 * @brief   Sets datatype of a function.
 *
 * Returns false when unsuccessful or true
 * @param functionName  name of the function
 * @param type          type of the function
 * @returns True/false.
 */
bool setFunctionType(const char * functionName, DataType type)
{
    SymbolTable * function;
    function = findFunction(functionName);
    if(function == NULL)
    {
        //error?
        return false;
    }
    else
    {
        function->type = type;
    }

    #ifdef SYMTABLE_DEBUG
        debug("Setting type of a function %s.", functionName);
    #endif
    return true;
}
/**
 * @brief   Finds datatype of a function.
 *
 * @param functionName  name of the function
 * @returns success -> DataType, failure -> datatype_unknown.
 */
DataType findFunctionType(const char * functionName)
{
    SymbolTable * function;
    function = findFunction(functionName);
    if(function == NULL)
    {
        //error?
        return DataType_Unknown;
    }

    #ifdef SYMTABLE_DEBUG
        debug("Finding type of a function %s.", functionName);
    #endif
    return function->type;
}
/**
 * @brief   Finds number of parametres of a function.
 *
 * Returns 0 when unsuccessful or number (!Cannot detect if function exists!)
 * @param functionName  name of the function
 * @returns success -> number of parametres, failure -> 0.
 */
size_t findFunctionNumberOfParametres(const char * functionName)
{
    SymbolTable * function;
    function = findFunction(functionName);
    if(function == NULL)
    {
        //error?
        return 0;
    }

    #ifdef SYMTABLE_DEBUG
        debug("Finding number of parametres of a function %s.", functionName);
    #endif
    return function->numberOfParameters;
}
/**
 * @brief   finds all parameters of a function.
 *
 * Returns pointer to a list of parameters or NULL
 * @param functionName  name of the function
 * @returns pointer or NULL.
 */
struct paramFce * findFunctionParameters(const char * functionName)
{
    #ifdef SYMTABLE_DEBUG
        debug("Looking for parametres");
    #endif

    SymbolTable * function;
    function = findFunction(functionName);
    if(function == NULL)
    {
        //error?
        return NULL;
    }
    return function->firstParam;
}
/**
 * @brief   Counts list length.
 *
 * @param parametres  pointer to the list of parametres
 * @returns number of parametres.
 */
size_t listLength(struct paramFce * parametres)
{
    size_t i = 0;

    while(parametres != NULL)
    {
        i++;
        parametres = parametres->next;
    }
    return i;
}
/**
 * @brief   Adds a parameter of a function and saves them as variables.
 *
 * Returns false when unsuccessful or true
 * @param functionName  name of the function
 * @param type          type of the parameter
 * @returns True/false.
 */
bool addFunctionParameters(const char * functionName, struct paramFce * parametres)
{
    SymbolTable * function;
    function = findFunction(functionName);
    if(function == NULL)
    {
        //error?
        return false;
    }
    else
    {
        if(function->firstParam != NULL) return false;
        //inserts parametres into list
        function->firstParam = parametres;
        function->numberOfParameters = listLength(parametres);

        //inserts parametres into variable array
        struct paramFce * pom = function->firstParam;
        while(pom != NULL)
        {
            if(!addVariable(function->name, pom->name)) return false;
            if(!addVariableType(function->name, pom->name, pom->type)) return false;
            pom = pom->next;
        }
    }

    #ifdef SYMTABLE_DEBUG
        debug("Adding parameters into function %s.", functionName);
    #endif
    return true;
}
/**
 * @brief   Adds a variable into a  function.
 *
 * Returns false when unsuccessful or true
 * @param functionName  name of the function
 * @param name          name of the variable
 * @returns True/false.
 */
bool addVariable(const char * functionName, const char * name)
{
    SymbolTable * function;
    function = findFunction(functionName);
    if(function == NULL)
    {
        //error?
        return false;
    }

    #ifdef SYMTABLE_DEBUG
        debug("Adding variable into function %s.", functionName);
    #endif

    return frameAddSymbol( &function->variables, name);
}
/**
 * @brief   Adds a variable type.
 *
 * Returns false when unsuccessful or true
 * @param functionName  name of the function
 * @param name          name of the variable
 * @param type          type of the variable
 * @returns True/false.
 */
bool addVariableType(const char * functionName, const char * name, DataType type)
{
    SymbolTable * function;
    function = findFunction(functionName);
    if(function == NULL)
    {
        //error?
        return false;
    }

    #ifdef SYMTABLE_DEBUG
        debug("Adding the type of %s in function %s.", name, functionName);
    #endif

    return frameAddSymbolType(function->variables, name, type);
}
/**
 * @brief   Finds a variable type.
 *
 * @param functionName  name of the function
 * @param name          name of the variable
 * @returns success -> DataType, failure -> datatype_unknown.
 */
DataType findVariableType(const char * functionName, const char * name)
{
    SymbolTable * function;
    function = findFunction(functionName);
    if(function == NULL)
    {
        //error?
        return DataType_Unknown;
    }
    struct variable * var;
    var = frameFindSymbol(function->variables, name);
    if(var == NULL)
    {
        //error?
        return DataType_Unknown;
    }

    #ifdef SYMTABLE_DEBUG
        debug("Finding the type of %s in function %s.", name, functionName);
    #endif

    return var->type;
}
/**
 * @brief   Finds a variable in a function.
 *
 * Returns false when unsuccessful or true
 * @param functionName  name of the function
 * @param name          name of the variable
 * @returns True/false.
 */
bool findVariable(const char * functionName, const char * name)
{
    SymbolTable * function;
    function = findFunction(functionName);
    if(function == NULL)
    {
        //error?
        return false;
    }

    #ifdef SYMTABLE_DEBUG
        debug("Finding variable %s in function %s.", name, functionName);
    #endif

    if( frameFindSymbol(function->variables, name) == NULL ) return false;
    return true;
}
/**
 * @brief   Finds a function in the table.
 *
 * Returns false when unsuccessful or true
 * @param functionName  name of the function
 * @returns True/false.
 */
bool findFunctionInTable(const char * functionName)
{
    if(findFunction(functionName) == NULL) return false;
        else return true;
}
/**
 * @brief   Frees the function table.
 *
 */
void functionTableEnd()
{
    functionTableFree(functionTable.arr_size);
}



//----------------------------------------------------------------------------------------------
//--------------------------TEST FUNCTIONS------------------------------------------------------

void print(SymbolTableFrame * frame)
{
    printf("%d/%d\n\n", (int)frameEntityCount(frame), (int)frameTableSize(frame));

    for(size_t i = 0;i<frameTableSize(frame);++i)
    {
        if(frame->arr[i].name == NULL) printf("- | ");
        else printf("Symbol: %s, Type: %d | ", frame->arr[i].name, (int)frame->arr[i].type);
    }
    printf("\n");
}
void printfunction(void)
{
    int pocitadlo = 1;
    struct paramFce * pom;

    printf("\n\n------------Prochazim polem majicim %d polozek!--------------\n\n", (int)functionTable.count);
    for(size_t i = 0;i < functionTable.arr_size;++i)
    {
        printf("%d. polozka:\n", (int)pocitadlo++);
        if(functionTable.arr[i] == NULL) continue;
        printf("Jmeno funkce: %s\n", functionTable.arr[i]->name);
        printf("Typ funkce: %d\n", (int)functionTable.arr[i]->type);
        printf("Pocet parametru funkce: %d\n", (int)functionTable.arr[i]->numberOfParameters);
        pom = functionTable.arr[i]->firstParam;
        while(pom != NULL)
        {
            printf(" - Jmeno param: %s\n", pom->name);
            printf(" - Typ param: %d\n", (int)pom->type);
            pom = pom->next;
        }
        print(functionTable.arr[i]->variables);
        printf("==========================================================================\n\n");
    }
}

//----------------------------------------------------------------------------------------------------




#ifdef skladka

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
 * @brief   Deletes a function symbol table from stack.
 *
 * Returns false when unsuccessful or true
 * @returns True/false.
 */
//i think we will not need that frame so i dont have to return it
//if we need it i will fix it
bool popFromSymtableStack(void)
{
    if(symtableStack.first == NULL) return false;

    SymbolTable * symtable = symtableStack.first;
    symtableStack.first = symtableStack.first->next;

    symtableFree(symtable);
    symtableStack.count--;

    #ifdef SYMTABLE_DEBUG
        debug("Frame was destroyed from top of the symtable stack.");
    #endif
    return true;
}
/**
 * @brief   Destroys the whole stack of symbol tables (functions).
 *
 */
void clearSymtableStack()
{
    while(popFromSymtableStack() != false)
    {}

    #ifdef SYMTABLE_DEBUG
        debug("Symtable stack was cleared.");
    #endif
}






    /*WILL ADD MORE WHEN I REALISE WHAT YOU WANT FROM ME*/

/**
 * @brief   Searches for a function in a stack.
 *
 * Returns NULL when unsuccessful or pointer to function frame
 * @param functionName  name of the function
 * @returns pointer/NULL.
 */
SymbolTable * findFunction(char * name)
{
    SymbolTable * pom = symtableStack.first;

    while(pom != NULL)
    {
        if(!strcmp(pom->name, name)) return pom;
        pom = pom->next;
    }
    return NULL;
}

/**
 * @brief   Inserts a function symbol table onto stack.
 *
 * This function creates a symbol table and initialises it.
 * Returns false when unsuccessful or true
 * @param functionName  name of the function
 * @returns True/false.
 */
bool pushOntoSymtableStack(char * functionName)
{
    //there cannot be two functions with the same name
    if(findFunction(functionName) != NULL) return false;

    SymbolTable *symtable;
    symtable = malloc(sizeof(SymbolTable));
    if(symtable == NULL)
    {
        EndHash("Symtable stack: pushOntoSymtableStack: could not allocate memory", ErrorType_Internal);
        return false;
    }

    //initialising symbol table of a function --------------------------
    symtable->type = DataType_Unknown;
    symtable->firstParam = NULL;
    symtable->numberOfParameters = 0;

    //allocating memory for a function name
    symtable->name = malloc(sizeof(char) * strlen(functionName) + sizeof(char));
    if(symtable->name == NULL)
    {
        EndHash("Symtable stack: pushOntoSymtableStack: could not allocate memory for a symbol name",
                                                                                    ErrorType_Internal);
        return false;
    }
    strcpy(symtable->name, functionName);

    //initialising hash table of variables
    symtable->variables = frameInit(STARTING_CHUNK);
    if(symtable->variables == NULL)
    {
        EndHash("Symtable stack: pushOntoSymtableStack: could not initialise frame", ErrorType_Internal);
        return false;
    }

    //connecting stack -------------------------------------------------
    symtable->next = symtableStack.first;
    symtableStack.first = symtable;
    symtableStack.count++;

    #ifdef SYMTABLE_DEBUG
        debug("Frame was created and pushed onto a symtable stack.");
    #endif
    return true;
}





/*************************************************************/

                    //FUNCTION TABLE

/*************************************************************/

                //FUNCTION TABLE DATA

/**
 * @brief   Structure representing type and name of parametres.
 *
 * List.
 */
 /*
struct paramFce{
    DataType type;
    char * name;        //do i need to know name of the parameter?
    struct paramFce *next;
};
*/

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
    struct paramFce * param;    //list
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
    struct function * arr;  //dynamic array
} FunctionArray;


static FunctionArray functiontable = {.arr_size = 0, .count = 0, .arr = NULL};

/*-----------------------------------------------------------*/

                    //DECLARATIONS

/**
 * @brief   Initiation of table of functions.
 *
 * This function allocates table of a starting size.
 * @returns true -> ok, false -> error.
 */
bool functionTableInit(void);
/**
 * @brief   Destroys table of functions.
 *
 * This function frees the whole table.
 */
void functionTableFree(void);
/**
 * @brief   Reallocs array of functions.
 *
 * This function increases size of table of functions.
 * @returns true -> ok, false -> fail.
 */
bool functionTableResize(void);
/*-----------------------------------------------------------*/

                    //FUNCTION BODY

bool functionTableInit(void)
{
    //allocation of the table
    if( (functiontable.arr = malloc(STARTING_CHUNK * sizeof(struct function)))  == NULL)
    {
        //error message
        return false;
    }
    functiontable.arr_size = STARTING_CHUNK;
    functiontable.count = 0;

    for(size_t i = 0; i < functiontable.arr_size ;++i)  //maybe not necessary, count = 0
    {
        functiontable.arr[i].name = NULL;
        functiontable.arr[i].type = DataType_Unknown;
        functiontable.arr[i].numberOfParam = 0;
        functiontable.arr[i].param = NULL;
    }

    return true;
}

void functionTableFree(void)
{
    struct paramFce * pomparam = NULL;

    for(size_t i = 0;i < functiontable.count;++i)
    {
        free(functiontable.arr[i].name);   //free name of function and
        while(functiontable.arr[i].param != NULL)  //while function has parameters in a list
        {
            pomparam = functiontable.arr[i].param;
            functiontable.arr[i].param = functiontable.arr[i].param->next;
            free(pomparam->name);   //free them one by one
            free(pomparam);
        }
    }

    functiontable.arr_size = 0;
    functiontable.count = 0;
    free(functiontable.arr);
}

bool functionTableResize(void)
{
    if( (functiontable.arr = realloc(functiontable.arr, functiontable.arr_size * RESIZE_RATE
                                                    * sizeof(struct function)))  == NULL)
    {
        //error message
        return false;
    }

    //for(size_t i = 0;i < consttable.arr_size;++i)     incomplete, may not be needed
        //consttable.arr[i]->type = DataType_Unknown;

    functiontable.arr_size = functiontable.arr_size * RESIZE_RATE;

    return true;
}

/*TO DO:
    finding
    adding + resizing controll

    Need to know more information about interaction requirements.
*/

/*-----------------------------------------------------------*/

/*TO DO:
    int numberOfParam();
    findFunction
    findParametreName (finds one param of a function, takes index of param)
    findParametreType
    findParametreInd
*/

#endif
