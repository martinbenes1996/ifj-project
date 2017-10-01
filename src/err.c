
/**
 * @file err.c
 * @interface err
 * @authors xbenes49 xbolsh00 xkrato47 xpolan09
 * @date 29th september 2017
 * @brief Error module
 *
 * This interface enables storing error data.
 */

#include "err.h"
#include "types.h"


/*-----------------------------------------------------*/
// data
static ErrorType mtype = ErrorType_Ok; /**< Stored error type. */
static const char * mmessage = ""; /**< Stored error message. */
static int mline = -1; /**< Stored line number with error. */

/*-----------------------------------------------------*/
// setters
void setErrorType(ErrorType type)
{
  mtype = type;
  if(mtype == ErrorType_Ok)
  {
    mmessage = "";
    mline = -1;
  }
}
void setErrorMessage(const char * message) { mmessage = message; }
void setErrorLine(int line) { mline = line; }

/*-----------------------------------------------------*/
// getters
ErrorType getErrorType() { return mtype; }
int getErrorCode()
{
  switch(mtype)
  {
    // syntax error
    case ErrorType_Syntax:
      return SYNTAX_ERROR;

    // semantic error
    case ErrorType_Semantic1:
      return SEMANTIC1_ERROR;
    case ErrorType_Semantic2:
      return SEMANTIC2_ERROR;

    // internal error
    case ErrorType_Internal:
      return INTERNAL_ERROR;

    // ok
    case ErrorType_Ok:
      return OK;

  }
}
const char * getErrorMessage() { return mmessage; }
int getErrorLine() { return mline; }
/*-----------------------------------------------------*/
