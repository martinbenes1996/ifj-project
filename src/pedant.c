
#include "pedant.h"

bool Sem_VariableDefined(long functionId, const char * varname)
{
  (void)functionId;
  (void)varname;
  return true;
}

bool Sem_FunctionDefined(const char * funcname)
{
  (void)funcname;
  return true;
}