#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

void initConfig();

void setHelp();
bool help();

void setBypass();
bool bypass();

char * Config_getFunction();
void Config_setFunction(char *);

void printConfig();


#endif // CONFIG_H
