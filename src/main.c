
/**
 * @file main.c
 * @authors xbenes49 xbolsh00 xkrato47 xpolan09
 * @date 19th september 2017
 * @brief Main module.
 *
 * This module contains the function main().
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "err.h"
#include "generator.h"
#include "io.h"
#include "parser.h"
#include "types.h"
#include "symtable.h"
#include "tables.h"

/*------------------------------------------------------*/
/** @addtogroup main
 * Main() function and tools.
 * @{
 */

/**
 * @brief 	Prints help.
 *
 * This function prints help to standard output.
 */
void printHelp();

/**
 * @brief 	Processes the arguments.
 *
 * This function processes the argc and argv from main into the config.
 * @param argc 		Number of arguments.
 * @param argv 		Argument pointer.
 * @returns 			True, if success, false otherwise.
 */
bool processArguments(int argc, char *argv[]);

/*-----------------------------------------------------*/

/**
 * @brief 	Main function.
 *
 * This function is called as first. It recieves arguments from cmd
 * into its parameter.
 * @param argc 		Number of arguments (from shell).
 * @param argv 		Argument pointer.
 * @returns 			Return code of programme.
 */
int main(int argc, char *argv[])
{

	#ifdef INIT_DEBUG
		debug("IFJ project started.");
	#endif

	/*----------- ARGS ----------------*/
	if(!processArguments(argc, argv)) exit(1);
	if(help()) { printHelp(); exit(0); }
	/*---------------------------------*/


	/*------ PROGRAM RUN -------*/
	RunParser();
	/*--------------------------*/


	/*-------- ERROR CHECK ---------*/
	// error log
	if(getErrorType() != ErrorType_Ok)
	{
		err("%s. Line: %d", getErrorMessage(), getErrorLine());
	}
	/*-----------------------------*/


	// final operations
	closeOut();
	return getErrorCode();
}

/** @}*/
/*-----------------------------------------------------*/

bool processArguments(int argc, char *argv[])
{
	initConfig();
	for(int i = 1; i < argc; i++)
	{
		// help
		if( !strcmp(argv[i], "-h") || !strcmp(argv[i], "--help") )
		{
			setHelp();
			#ifdef ARGS_DEBUG
				debug("Argument -h");
			#endif
			break;
		}

		// help
		if( !strcmp(argv[i], "-b") || !strcmp(argv[i], "--bypass") )
		{
			setBypass();
			#ifdef ARGS_DEBUG
				debug("Argument -b");
			#endif
			break;
		}

		// unknown
		else
		{
			err("Unknown parameter!");
			return false;
		}
	}

	#ifdef CONFIG_DEBUG
		printConfig();
	#endif

	return true;
}

void printHelp()
{
	out("IFJ project.\n"
					"2017/2018\n\n"
					"Usage:\n"
					"-h\tPrints this help."
	);
}
