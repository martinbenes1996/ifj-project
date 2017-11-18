
/**
 * @file err.h
 * @interface err
 * @authors xbenes49 xbolsh00 xkrato47 xpolan09
 * @date 29th september 2017
 * @brief Error library
 *
 * This interface enables setting errors, so it won't have to be returned in
 * the function cascade.
 */

#ifndef ERR_H
#define ERR_H


/*-----------------------------------------------------*/
/**
 * @brief   Error type enumeration.
 *
 * This enum contains list af all possible errors given.
 */
typedef enum
{
  ErrorType_Lexical,/**< Lexical error. */
  ErrorType_Syntax, /**< Syntax error. */
  ErrorType_Semantic1, /**< Semantic error, first type. */      // not defined function/variable, redefinition of function/variable
  ErrorType_Semantic2, /**< Semantic error, second type. */     // type incompatibility, bad argument/parameter count in function call
  ErrorType_Semantic3, /**< Semantic error, third type */       // other semantic errors
  ErrorType_Internal, /**< Internal error (allocation etc.). */
  ErrorType_Ok /**< Everything ok (returned when nothing happens). */
} ErrorType;


/*-----------------------------------------------------*/
/** @addtogroup error setters
 * Setters of error parameters.
 * @{
 */

/**
 * @brief   Sets error type.
 *
 * This function sets internal memory to error type given.
 * @param type    Error Type.
 */
void setErrorType(ErrorType type);
/**
 * @brief   Sets error message.
 *
 * This function sets internal message to the one given.
 * @param message    Error Message.
 */
void setErrorMessage(const char * message);
/**
 * @brief   Sets line with error.
 *
 * This function sets, on which line is the error.
 * @param line    Line with error.
 */
void setErrorLine(int);


/** @}*/
/*-----------------------------------------------------*/
/** @addtogroup error getters
 * Getters of error parameters.
 * @{
 */

/**
 * @brief   Error type getter.
 *
 * This function returns the error type, set inside the module.
 * @returns     Error type.
 */
ErrorType getErrorType();
/**
 * @brief   Error code getter.
 *
 * This function returns the error code, set inside the module.
 * It is then used as exit code.
 * @returns     Error code.
 */
int getErrorCode();
/**
 * @brief   Error message getter.
 *
 * This function returns the error message, set inside the module.
 * It is then used to announce.
 * @returns     Error message.
 */
const char * getErrorMessage();
/**
 * @brief   Error line getter.
 *
 * This function returns the line, which contains the error.
 * @returns     Line with error.
 */
int getErrorLine();


/** @}*/
#endif // ERR_H
