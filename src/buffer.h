/**
 * @file buffer.h
 * @interface buffer
 * @authors xbenes49 xbolsh00 xkrato47 xpolan09
 * @date 5th october 2017
 * @brief Buffer interface.
 *
 * This interface declares dynamic C-string (buffer).
 */

#ifndef BUFFER_H
#define BUFFER_H


#include <stdbool.h>

/*---------------------------- STACK ----------------------------------*/
/** @addtogroup Buffer
 * Buffer functions.
 * @{
 */


/**
 * @brief   Buffer getter.
 *
 * This function will release the inner buffer. It will reinit the
 * state of module to create new buffer.
 * @returns   Pointer to C-string assembled.
 */
char * GetBuffer();

/**
 * @brief   Char handler.
 *
 * This function will merge the incoming character with string.
 * @param c       Character to merge.
 * @returns True if success. False otherwise.
 */
bool AddToBuffer(char c);

/**
 * @brief   Buffer cutter.
 *
 * This function will take the buffer and normalize it to size 128,
 * if it is bigger. It is used for identificators, which must be
 * up to this size.
 * @param buff    Incoming buffer.
 * @returns Normalized buffer, or NULL, if error.
 */
char * CutBuffer(char * buff);

/**
 * @brief   Buffer cleaner.
 *
 * This function will reinit buffer module and free the memory allocated.
 */
void FreeBuffer();


/** @}*/
/*---------------------------------------------------------------------*/

#endif // BUFFER_H
