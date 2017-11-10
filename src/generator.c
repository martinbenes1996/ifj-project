
#include "generator.h"
#include "io.h"

bool Send(Stack s)
{
  #ifdef GENERATOR_DEBUG
    debug("Send to Generator");
  #endif

  (void)s;
  return true;
}

void G_FunctionCall()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate function call");
  #endif


}

void G_Condition()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate condition");
  #endif


}

void G_Cycle()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate cycle");
  #endif


}

void G_Assignment()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate assignment");
  #endif


}

void G_Print()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate print");
  #endif


}

void G_Input()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate input");
  #endif


}

void G_Logic()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate logic");
  #endif


}

void G_Expression()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate expression");
  #endif

}
