
/**
 * @file symtable.c
 * @interface symtable
 * @authors xbenes49 xbolsh00 xkrato47 xpolan09
 * @date 6th november 2017
 * @brief Symbol table source code
 *
 * This implements interactions with tables.
 */

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
static const char keywords[35][9] = {"as", "asc", "declare", "dim", "do", "double", "else",
                                     "end", "chr", "function", "if", "input", "integer",
                                     "length", "loop", "print", "return", "scope", "string",
                                     "substr", "then", "while", "and", "boolean", "continue",
                                     "elseif", "exit", "false", "for", "next", "not", "or",
                                     "shared", "static", "true"};

//----- yep, static, but this will be in symtable.c, so no one could reach it

//I think it will have to be accessible for syntax analysis
//you will need to know what keyword is in the token (there will be just an index into this array)

/*-----------------------------------------------------------*/

/**
 * @brief   Initialisation of table of constants.
 *
 * This function creates array of constants.
 * Returns NULL when unsuccessful, otherwise
 * returns pointer to array of size 10.
 * @returns Pointer to array of constants.
 */
ConstData * constInit(void);
// --- why do you need this? there will be only one table of constants,
// --- all you need is to keep pointer inside module. and first call of
// --- function below (constInsert) will check the table. if it is not
// --- initialized yet, it will initialize it.
// --- so Parser wouldn't have to deal with ConstData *
// --- and the structure ConstData could be private

// +++ yeah i see, will be removed

/*-----------------------------------------------------------*/

        //CONSTANT DATA TABLE (not sure if public or private yet)

/**
 * @brief   Structure representing characteristics of constants.
 *
 * This structure is filled with type and value of the constant.
 * Value is represented by union.
 */
struct constant{
    dataType type;
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
    size_t n;           //number of entities in array
    struct constant arr[];
} ConstData;
















struct htab_listitem{
    char *key;
    size_t data;
    struct htab_listitem *next;
};

struct htab_listitem{
    char *key;
    size_t data;
    struct htab_listitem *next;
};

struct htab_t{
    size_t arr_size;    //array size
    size_t n;           //number of entities in array
    struct htab_listitem *arr[];
};










     /*the rest of this file is most likely wrong and will be reworked*/




                    //FUNCTION TABLE DATA


/**
 * @brief   Structure representing type and name of parametres.
 *
 * List.
 */
struct paramFce{
    dataType type;
    char * name;
    struct paramFce *next;
};

/**
 * @brief   Structure representing function data.
 *
 * This structure is filled with informations about declared functions.
 * Contains structure with info about parametres.
 */
struct htab_listitem{
    char * name;
    size_t numberOfParam;
    struct paramFce * param;
};

/**
 * @brief   Structure representing hash table of function names/parametres.
 *
 * This structure is filled with functions.
 * It knows its size and number of entities. Will resize automatically.
 */
typedef struct htab_t{
    size_t arr_size;    //array size
    size_t n;           //number of entities in array
    struct htab_listitem *arr[];
} FunctionData;
/*-----------------------------------------------------------*/

                    //FUNCTION TABLE FUNCTIONS

/**
 * @brief   Initialisation of table of functions.
 *
 * This function creates array of functions.
 * Returns NULL when unsuccessful, otherwise
 * returns pointer to array of size (10?).
 * @returns Pointer to array of functions.
 */
FunctionData * functionInit(void);
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
int functionInsert(FunctionData * ptr, char * name);

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
int paramInsert(char * name, dataType type, char * paramName);


/*TO DO:
    int numberOfParam(FunctionData * ptr, const char * name);
    freeTable
    findFunction
    findParametreName (finds one param of a function, takes index of param)
    findParametreType
    findParametreInd
*/

/*-----------------------------------------------------------*/

            //ACTIVE FUNCTION STACK AND DATA OF VARIABLES

/**
 * @brief   Structure representing information about variables.
 *
 * It contains name, type and data of a variable.
 */
struct htab_listitem_prom{
    char * name;
    dataType type;
    DataUnion data;
};

/**
 * @brief   Structure representing hash table of variables.
 *
 * This structure is filled with variables.
 * It knows its size and number of entities. Will resize automatically.
 */
struct htab_prom{
    size_t arr_size;    //array size
    size_t n;           //number of entities in array
    struct htab_listitem_prom *arr[];
};
// --- why *arr[]? isn't one direction enought? it looks like linked list
// --- why not to save the whole structure in the table

/**
 * @brief   Structure representing informations about active functions.
 *
 * This structure contains an index into a hash table of declared functions
 * and a pointer to a hash table of variables of each function.
 */
typedef struct activeFunc{
    size_t index; /**< Index to function table! (Link) */
    struct htab_prom * ptr;
} ActiveFunc;

/**
 * @brief   Structure representing stack made by array (or is list better?).
 *
 * This structure contains an array (pointer), number of allocated entities
 * and index of the first free entity.
 * Will resize automatically.
 */
typedef struct arrStack{
    ActiveFunc * AFarray;   //malloc array, potom realokovat
    int length;
    int top;
} ArrStack;

/*-----------------------------------------------------------*/

            //ACTIVE FUNCTION FUNCTIONS

ArrStack * stackInit(void);




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
 * vytvori tabulku o zadane velikosti
 * vraci ukazatel na tabulku nebo NULL pointer v pripade neuspechu
 @param velikost celociselna hodnota udavajici velikost pole seznamu
*/
struct htab_t *htab_init(size_t velikost);

/**
 * uvolni tabulku z pameti
 @param t ukazatel na tabulku
*/
void htab_free(struct htab_t *t);

/**
 * vytvori tabulku t z tabulky t2 o nove velikosti
 * vraci NULL pri neuspechu a t2 je nezmenena, pri uspechu vraci ukazatel na novou tabulku a t2 je prazdna
 @param newsize nova velikost tabulky
 @param t2 ukazatel na zvetsovanou tabulku
*/
struct htab_t *htab_move(size_t newsize, struct htab_t *t2);

/**
 * fce odstrani polozky pole tabulky
 @param t ukazatel na tabulku
*/
void htab_clear(struct htab_t *t);

/**
 * vraci pocet prvku tabulky
 @param ukazatel na tabulku
*/
size_t htab_size(struct htab_t *t);

/**
 * vraci velikost pole tabulky
 @param ukazatel na tabulku
*/
size_t htab_bucket_count(struct htab_t *t);

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

//#endif

