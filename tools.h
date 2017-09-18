
// tools.h
// Necessary functions.
// IFJ project
// FIT VUT
// 2017/2018

#ifndef TOOLS_H
#define TOOLS_H

#include "types.h"

/**
 * @brief 	Processes the arguments to the structure.
 * @param argc 		Number of arguments.
 * @param argv 		Argument pointer.
 * @returns 			Filled structure.
 */
inline bool processArguments(args_t* a, int argc, char *argv[])
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



#endif // TOOLS_H
