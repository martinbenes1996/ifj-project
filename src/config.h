
/**
 * @file config.h
 * @interface config
 * @authors xbenes49 xbolsh00 xkrato47 xpolan09
 * @date 5th october 2017
 * @brief Configuration interface.
 *
 * This interface generates access to global data for whole project.
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

/*--------------------------- CONFIGURATION ----------------------------------*/
/** @addtogroup Configuration.
 * Configuration functions.
 * @{
 */

/*-------------- CONSTRUCTOR --------------*/

/**
 * @brief   Configuration constructor.
 *
 * This function sets the defaults to inner data.
 */
void initConfig();

/**
 * @brief    Prints whole configuration.
 *
 * This function prints the state of the configuration.
 */
void printConfig();

/*-------------- HELP --------------*/

/**
 * @brief   Name of the processed function.
 *
 * This function returns the name of the function,
 * which is being processed at the moment.
 * @returns Function name.
 */
char * Config_getFunction();

/**
 * @brief   Sets the name of the processed function.
 *
 * This function sets the name of the function,
 * which is being processed at the moment. It takes the
 * address as it is. It is called from parser module.
 * @param func    Function name.
 */
void Config_setFunction(char * func);

/*-------------- HELP --------------*/
/**
 * @brief   Sets help flag.
 *
 * This function sets the inner help flag to true (defaultly false).
 */
void setHelp();

/**
 * @brief   Help flag.
 *
 * This function returns, wheather the help flag is '1', or '0'.
 * @returns Status of help flag.
 */
bool help();

/*-------------- BYPASS --------------*/

/**
 * @brief   Sets bypass flag.
 *
 * This function sets the inner bypass flag to true (defaultly false).
 */
void setBypass();

/**
 * @brief   Bypass flag.
 *
 * This function returns, wheather the bypass flag is '1', or '0'.
 * @returns Status of bypass flag.
 */
bool bypass();

/** @}*/
/*-----------------------------------------------------------------------------*/

#endif // CONFIG_H
