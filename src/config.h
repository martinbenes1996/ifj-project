#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

void initConfig();

void setHelp();
bool help();

void setBypass();
bool bypass();

void printConfig();


#endif // CONFIG_H