
/**
 * @file scanner.h
 * @interface scanner
 * @authors xbenes49 xbolsh00 xkrato47 xpolan09
 * @date 29th september 2017
 * @brief Lexical Scanner interface.
 *
 * This interface declares Lexical Scanner and its operation. It writes
 * in a queue.
 */

#ifndef SCANNER_H
#define SCANNER_H

#include <stdbool.h>

#ifdef MULTITHREAD

/**
 * @brief    Scanner process status getter.
 *
 * The scanner reads input file. If the file gives EOF, this function will
 * return false.
 * @returns     True, if scanner is scanning, false if ended.
 */
bool ScannerIsScanning();

/**
 * @brief   Asks scanner to end.
 *
 * This function will ask scanner to end, at it will, when it processed phrasem.
 */
void AskScannerToEnd();

/**
 * @brief   Initial function for module.
 *
 * This function starts reading and pushing into queue.
 * @param v     Ignored.
 * @returns     NULL
 */
void *InitScanner(void * v);

#endif // MULTITHREAD

#endif // SCANNER_H
