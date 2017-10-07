
/**
 * @file hash.c
 * @interface hash
 * @authors xbenes49 xbolsh00 xkrato47 xpolan09
 * @date 7th november 2017
 * @brief Source code of a hash function.
 *
 * This implements hash function.
 */


#include <stdlib.h>
#include "symtable.h"
#include "types.h"

/*function will probably be changed in the future*/

unsigned int hash_function(const char *str)
{
    unsigned int h=0;
    const unsigned char *p;

    for(p=(const unsigned char*)str;*p!='\0'; p++)
        h = 65599*h + *p;

    return h;
}
