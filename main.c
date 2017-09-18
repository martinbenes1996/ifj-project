
// main.c
// Main module
// IFJ project
// FIT VUT
// 2017/2018

#include "io.h"

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
	
	/*
	// example of usage input
	const int max = 100;
	char buff[max];
	while( getLine(stdin, buff, max) ) printf("%s\n", buff);
	*/



	closeOut();
	return 0;
}
