
// main.c
// Main module
// IFJ project
// FIT VUT
// 2017/2018

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "io.h"
#include "types.h"


/** @brief 	Prints help to stdout. */
void printHelp();

/**
 * @brief 	Processes the arguments to the structure.
 * @param argc 		Number of arguments.
 * @param argv 		Argument pointer.
 * @returns 			Filled structure.
 */
bool processArguments(args_t* a, int argc, char *argv[]);

/**
 * @brief 	Main function.
 * @param argc 		Number of arguments (from shell).
 * @param argv 		Argument pointer.
 * @returns 			Return code of programme.
 */
int main(int argc, char *argv[])
{

	#ifdef INIT_DEBUG
		debug("IFJ project started.");
	#endif

	// argument process
	args_t a;
	if(!processArguments(&a, argc, argv)) exit(1);
	if(a.help) { printHelp(); exit(0); }


	/*
	// example of usage input
	const int max = 100;
	char buff[max];
	while( getLine(stdin, buff, max) ) printf("%s\n", buff);
	*/



	closeOut();
	return 0;
}

bool processArguments(args_t* a, int argc, char *argv[])
{

	// defaults
	a->help = false;

	for(int i = 1; i < argc; i++)
	{
		// help
		if( !strcmp(argv[i], "-h") || !strcmp(argv[i], "--help") )
		{
			a->help = true;
			#ifdef ARGS_DEBUG
				debug("Argument -h");
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
