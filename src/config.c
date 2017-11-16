
#include "config.h"
#include "io.h"
#include "types.h"

/*----------- DATA ------------*/
args_t d;
static char * mfunction = NULL;
static long mline = 1;
/*----------------------------*/

void initConfig()
{
	d.help = false;
	d.bypass = false;
}

void printConfig()
{
	out("---Config---\n"
			"help:   %d  \n"
			"bypass: %d  \n"
			"function: %s\n"
			"------------\n", ((d.help)?1:0), ((d.bypass)?1:0), mfunction);
}

/*---------------------*/

char * Config_getFunction() { return mfunction; }
void Config_setFunction(char * f) { mfunction = f; }

/*---------------------*/

long Config_getLine() { return mline; }
void Config_setLine(long line) { mline = line; }

/*---------------------*/

void setHelp() { d.help = true; }
bool help() { return d.help; }

/*---------------------*/

void setBypass() { d.bypass = true; }
bool bypass() { return d.bypass; }

/*---------------------*/
