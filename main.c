
// main.c
// Main module
// IFJ project
// FIT VUT
// 2017/2018

#include "log.h"

/**
 * @brief 	Main function.
 * @param argc 		Number of arguments (from shell).
 * @param argv 		Argument pointer.
 * @returns 			Return code of programme.
 */
int main()
{

	#ifdef INIT_DEBUG
		debug("Init.");
	#endif

	closeOut();
	return 0;
}
