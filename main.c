
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
#include "tools.h"
#include "types.h"


/** @brief 	Prints help to stdout. */
void printHelp();

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

void printHelp()
{
	printf("IFJ project.\n"
					"2017/2018\n\n"
					"Usage:\n"
					"-h\tPrints this help.\n"
	);
}
