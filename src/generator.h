#ifndef GENERATOR_H
#define GENERATOR_H

#include <stdbool.h>

#include "types.h"

/**
 * @brief   Generates the code.
 *
 * This function recieves stack from pedant with expression, which
 * has to be transferred from postfix to assembly generated code.
 * @param s         Stack with expression.
 * @returns True, if success. False otherwise.
 */
bool Send(Stack s);

void G_FunctionCall();
void G_Condition();
void G_Cycle();
void G_Assignment();
void G_Print();
void G_Input();



#endif // GENERATOR_H
