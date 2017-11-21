
/**
 * @file collector.h
 * @interface collector
 * @authors xbenes49 xbolsh00 xkrato47 xpolan09
 * @date 21st november 2017
 * @brief Garbage collector interface.
 *
 * This interface declares functions for structure Phrasem allocation
 * and freeing.
 */

#ifndef COLLECTOR_H
#define COLLECTOR_H

#include "types.h"

/*------------------------------- COLLECTOR ----------------------------------*/
/** @addtogroup Collector
 * Collector functions.
 * @{
 */

/**
 * @brief   Allocates Phrasem.
 *
 * This function allocates phrasem and saves its address to inner
 * pool.
 * @returns Pointer to allocated Phrasem, or NULL if fail.
 */
Phrasem allocPhrasem();

/**
 * @brief   Frees collector pool.
 *
 * This function clears memory of whole collector pool.
 */
void freeCollector();

/** @}*/
/*----------------------------------------------------------------------------*/

#endif // COLLECTOR_H
