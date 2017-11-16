
#include "config.h"
#include "io.h"
#include "types.h"

args_t d;
void initConfig()
{
	d.help = false;
	d.bypass = false;
}

void setHelp()
{
	d.help = true;
}
bool help()
{
	return d.help;
}

void setBypass()
{
	d.bypass = true;
}
bool bypass()
{
	return d.bypass;
}

static char * mfunction = NULL;
char * Config_getFunction() { return mfunction; }
void Config_setFunction(char * f) { mfunction = f; }

void printConfig()
{
	out("---Config---\n"
			"help:   %d  \n"
			"bypass: %d  \n"
			"------------\n", ((d.help)?1:0), ((d.bypass)?1:0));
}
