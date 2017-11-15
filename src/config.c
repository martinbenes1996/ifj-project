
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

void printConfig()
{
	out("---Config---\n"
			"help:   %d  \n"
			"bypass: %d  \n"
			"------------\n", ((d.help)?1:0), ((d.bypass)?1:0));
}
