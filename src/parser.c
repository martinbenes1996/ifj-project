
/**
 * @file parser.c
 * @authors xbenes49 xbolsh00 xkrato47 xpolan09
 * @date 2nd october 2017
 * @brief Syntax parser module.
 *
 * This module implements Syntax Analyser.
 */

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "collector.h"
#include "config.h"
#include "err.h"
#include "functions.h"
#include "generator.h"
#include "io.h"
#include "list.h"
#include "parser.h"
#include "pedant.h"
#include "queue.h"
#include "scanner.h"
#include "scanner_singlethrd.h"
#include "symtable.h"
#include "tables.h"
#include "stack.h"

#ifdef MULTITHREAD
pthread_t sc;
#endif

bool setFunction(const char * f)
{
  if(f == NULL)
  {
    char * func = Config_getFunction();
    if(func != NULL) free(func);
    Config_setFunction(NULL);
    return true;
  }

  if(Config_getFunction() != NULL) free( Config_getFunction() );
  char * mfunction = malloc(sizeof(char)*(strlen(f) + 1));
  if(mfunction == NULL) return false;

  strcpy(mfunction, f);
  Config_setFunction(mfunction);

  #ifdef PARSER_DEBUG
    debug("Function: %s", (f!=NULL)?f:"none");
  #endif
  return true;
}

bool end = false; /**< Set to true, if keyword end reached. */
bool wasReturn = true;
bool wasScope = false;

/*-------------------------- ERROR MACROS --------------------------------*/
/**
 * @brief  Error raiser.
 *
 * This macro writes debug info to err function. It also calls EndParser
 * and returns false from a function, where it is used.
 * @param msg         Message to write.
 * @param phrasem     Bad phrasem.
 * @param errtype     Type of error.
 */
#define RaiseError(msg, errtype)                       \
  do {                                                          \
    err("%s: %s: l.%d: %s", __FILE__, __func__, __LINE__, msg); \
    EndParser(msg, errtype);                                    \
    return false;                                               \
  } while(0)

/**
 * @brief Queue error raiser.
 *
 * This macro raises deals with lexical error.
 */
#define RaiseQueueError()                                       \
  do {                                                          \
    EndParser(NULL, ErrorType_Ok);                              \
    return false;                                               \
  } while(0)

/**
 * @brief -expected- error raiser.
 *
 * This macro raises specialized error -what expected-.
 * @param what        Token expected (in quotes)
 * @param phrasem     Target memory.
 */
#define RaiseExpectedError(what)                                \
  do {                                                          \
    RaiseError(what " expected", ErrorType_Syntax);             \
  } while(0)

/**
 * @brief checks separator in the queue
 *
 * This macro raises specialized error -separator expected-.
 * @param phrasem     Target memory.
 */
#define CheckSeparator()                                      \
  do {                                                        \
    Phrasem p = CheckQueue(p);                                \
    if(!isSeparator(p))                                       \
    {                                                         \
      RaiseExpectedError("separator");                        \
    }                                                         \
  } while(0)

/**
 * @brief checks operator in the queue
 *
 * This macro checks, if there is operator in the queue.
 * It raises error, if it isn't.
 * @param op          Operator.
 */
#define CheckOperator(op)                                     \
  do {                                                        \
    Phrasem p = CheckQueue(p);                                \
    if(!isOperator(p, op))                                    \
    {                                                         \
      RaiseExpectedError("operator \'" op "\'");              \
    }                                                         \
  } while(0)

/**
 * @brief checks separator in the queue
 *
 * This macro raises specialized error -separator expected-.
 * @param phrasem     Target memory.
 */
#define CheckKeyword(kw)                                      \
  do {                                                        \
    Phrasem p = CheckQueue(p);                                \
    if(!matchesKeyword(p, kw))                                \
    {                                                         \
      RaiseExpectedError("keyword \'" kw "\'");               \
    }                                                         \
  } while(0)

#ifdef MULTITHREAD
/**
 * @brief Secure Queue getter.
 *
 * This function will call RemoveFromQueue. And if it returns NULL,
 * it will raise the queue error (with macro RaiseQueueError).
 * @param phrasem       Target memory.
 * @returns phrasem reached
 */
#define CheckQueue(phrasem) RemoveFromQueue();  \
      if(phrasem == NULL) {                     \
        if(!ScannerIsScanning()) {              \
          return false;                         \
        }                                       \
        RaiseQueueError();                      \
      }

#else // single thread

/**
 * @brief Secure Queue getter.
 *
 * This function will call RemoveFromQueue. And if it returns NULL,
 * it will raise the queue error (with macro RaiseQueueError).
 * @param phrasem       Target memory.
 * @returns phrasem reached
 */
#define CheckQueue(phrasem) RemoveFromQueue();  \
      if(phrasem == NULL) {                     \
        RaiseQueueError();                      \
      }

#endif // MULTITHREAD
/*----------------------------------------------------------------*/
/** @addtogroup Parser_tools
 * Parser tools.
 * @{
 */

/*-------------------- CLEAR ----------------------*/


/**
 * @brief   Ends parser.
 *
 * This function ends scanner, and it will sets the error message to print,
 * if it is NULL, it will end well. It also deallocates queue and symbol table.
 * @param msg       Message to print.
 * @param errtype   Error type.
 */
void EndParser(const char * msg, ErrorType errtype);
void EndRoutine()
{
  #ifdef PARSER_DEBUG
    debug("Ending Parser.");
  #endif
  end = true;

  // end second thread
  #ifdef MULTITHREAD
  AskScannerToEnd(); /**< Symetric end. */
  pthread_join(sc, NULL);
  #endif // MULTITHREAD

  if(getErrorType() == ErrorType_Ok)
  {
    const char * notdefined = functionDefinitionCheck();
    if(notdefined != NULL) EndParser("not all declared functions defined", ErrorType_Semantic1);
  }

  // clear memory
  constTableFree();
	functionTableEnd();
  if(Config_getFunction() != NULL) free(Config_getFunction());
  ClearScanner();
  ClearGenerator();
  ClearPedant();
  freeCollector();
}

/** @} */
/*-----------------------------------------------------------*/
/** @addtogroup Lowlevel_parsers
 * Syntax parser help functions.
 * @{
 */

/**
 * @brief   Parses symbol to variable.
 *
 * This function will get the parameter, it will check, if it is symbol,
 * then it retypes it to variable.
 * @param p       Phrasem with variable.
 * @returns True, if success, false otherwise.
 */
bool VariableParse(Phrasem p);

/**
 * @brief   Parses symbol to function.
 *
 * This function will get the parameter, it will check, if it is symbol,
 * then it retypes it to function.
 * @param p       Phrasem with function.
 * @returns True, if success, false otherwise.
 */
bool FunctionParse(Phrasem p);

/**
 * @brief   Checks, if phrasem is datatype keyword.
 *
 * @param p       Phrasem with keyword.
 * @returns True, if it is. False if not.
 */
bool DataTypeParse(Phrasem p);

/**
 * @brief   Parses expression.
 *
 * This function reads queue and parses input expression.
 * @returns True, if success. False otherwise.
 */
bool ExpressionParse();

/**
 * @brief   Parses condition.
 *
 * This function will parse the condition inside loop and if statement.
 * @returns True, if success. False otherwise.
 */
bool LogicParse();

/** @} */
/*-----------------------------------------------------------*/
/** @addtogroup Highlevel_parsers
 * Syntax parser functions.
 * @{
 */

/**
 * @brief   Parses variable definition.
 *
 * This function will parse the line with variable definition. It will write
 * it to symbol table too.
 * @returns True if success. False otherwise.
 */
bool VariableDefinitionParse();

/**
 * @brief   Parses input command.
 *
 * This function will parse the line with input command.
 * @returns True, if success. False otherwise.
 */
bool InputParse();

/**
 * @brief   Parses print command.
 *
 * This function will parse the line with print command.
 * @param first   Tells, if this is recursive call, or from outside (send true).
 * @returns True if success. False otherwise.
 */
bool PrintParse(bool first);

/**
 * @brief   Parses line with symbol as first.
 *
 * This function will parse the line beginning with the symbol.
 * @returns True, if success. False otherwise.
 */
bool SymbolParse();

/**
 * @brief   Parses function declaration.
 *
 * This function will parse the line, containing the function declaration.
 * @returns True if success. False otherwise.
 */
bool FunctionDeclarationParse();

/**
 * @brief   Parses function declaration.
 *
 * This function will parse the block of function declaration.
 * @returns True if success. False otherwise.
 */
bool FunctionDefinitionParse();

/**
 * @brief   Parses scope.
 *
 * This function will parse the scope block (main).
 * @returns True if success. False otherwise.
 */
bool ScopeParse();

/**
 * @brief   Parses line/block with no prejustice.
 *
 * This function will parse the line/block inside the function and is
 * main state machine, which divides lines into parse functions.
 * @returns True, if line/block ended well. False otherwise.
 */
bool BlockParse();

/**
 * @brief   Parses global lines/blocks.
 *
 * This function will parse the global lines. It signs functions
 * to symbol table.
 * @returns True, if success. False otherwise.
 */
bool GlobalBlockParse();

/**
 * @brief   Parses cycle.
 *
 * This function will parse the cycle.
 * @returns True, if success. False otherwise.
 */
bool CycleParse();

/**
 * @brief   Parses condition.
 *
 * This function will parse the condition.
 * @returns True, if success. False otherwise.
 */
bool ConditionParse();

/**
 * @brief   Parses assignment.
 *
 * This function will parse the assignment.
 * @returns True, if success. False otherwise.
 */
bool AssignmentParse();

/**
 * @brief   Parses function call.
 *
 * This function will parse the condition on the right side of assignment.
 * @returns True, if success. False otherwise.
 */
bool FunctionCallParse();
bool LengthParse();
bool SubStrParse();
bool AscParse();
bool ChrParse();

bool ReturnParse();

/** @} */
/*----------------------------------------------------*/
/** @addtogroup End_parsers
 * Parsers of end lines.
 * @{
 */

/**
 * @brief   Parses end function.
 *
 * @returns True if success. False otherwise.
 */
bool EndFunctionParse();

/**
 * @brief   Parses end scope.
 *
 * @returns True if success. False otherwise.
 */
bool EndScopeParse();

/**
 * @brief   Parses end if.
 *
 * @returns True if success. False otherwise.
 */
bool EndIfParse();

/**
 * @brief   Parses end cycle.
 *
 * @returns True if success. False otherwise.
 */
bool EndCycleParse();

/** @} */
/*----------------------------- MAIN RUN ---------------------------*/
bool RunParser()
{
  #ifdef PARSER_DEBUG
    debug("Init Parser.");
  #endif
  bool status = true;

  if(!constTableInit()) RaiseError("Error allocating symbol table", ErrorType_Internal);
  InitGenerator();

  if(bypass())
  {
    while(1)
    {
      Phrasem p = CheckQueue(p);
      if(p->table == TokenType_EOF) break;
      PrintPhrasem(p);
    }

    freeCollector();
    constTableFree();
    functionTableEnd();
    return true;
  }

  # ifdef MULTITHREAD // ---------------------------------
        // queue init
        InitQueue();
        // running scanner
        pthread_create(&sc, NULL, InitScanner, NULL);

        // reading cycle
        while(ScannerIsScanning()) {
          // each global line (function declaration, definition etc.)
          if(!GlobalBlockParse()) {
            // fail
            if(end) {
              EndRoutine();
              return false;
            }
          }

          // success
          if(end) break;
        }

  #else // SINGLETHREAD -----------------------------------

        // reading cycle
        while(1) {
          // each global line (function declaration, definition etc.)
          if(!GlobalBlockParse()) {
            // fail
            status = false;
            break;
          }
          // success
          if(end)
          {
            if(!wasScope) RaiseError("no scope defined", ErrorType_Syntax);
            G_FinalLabel();
            break;
          }

          // something to do after each function
        }
  #endif  // --------------------------------------------

  // end
  EndRoutine();
  return status;
}

/*-------------------------- LOW LEVEL PARSER FUNCTIONS --------------------*/
bool VariableParse(Phrasem p)
{
  #ifdef PARSER_DEBUG
    debug("Variable parse.");
  #endif
  if(p == NULL)
  {
    RaiseExpectedError("variable");
  }

  if(p->table != TokenType_Symbol)
  {
    RaiseExpectedError("variable name");
  }

  // retyping
  p->table = TokenType_Variable;
  return true;
}

bool FunctionParse(Phrasem p)
{
  #ifdef PARSER_DEBUG
    debug("Function parse.");
  #endif
  if(p == NULL)
  {
    RaiseExpectedError("function");
  }

  if(p->table != TokenType_Symbol)
  {
    RaiseExpectedError("function name");
  }

  // retyping
  p->table = TokenType_Function;
  return true;
}

bool DataTypeParse(Phrasem p)
{
  #ifdef PARSER_DEBUG
    debug("Data type parse.");
  #endif
  if(p == NULL) return false;

  if(p->table != TokenType_Keyword) return false;

  return (matchesKeyword(p, "integer") || matchesKeyword(p, "double") || matchesKeyword(p, "string"));
}


/*-----------------expression parse----------------------------*/

/*--------------- DATA -----------------*/
static bool extraCloseBracket = false;
/*--------------------------------------*/


#define MODIFY_STACK()              \
  do {                              \
    PopFromEPStack();               \
    PopFromEPStack();               \
    PopFromEPStack();               \
    PopFromEPStack();               \
    PushOntoEPStack(op_E);           \
  } while(0)

#define ExpressionError(msg)                                \
    do {                                                    \
    setErrorMessage(msg);                                   \
    setErrorType(ErrorType_Syntax);                         \
    setErrorLine(Config_getLine());                         \
    } while(0)

//tries to perform stack modifications based on rules (called when '>')
int checkEPRules(Stack temporaryOpStack)
{
    #ifdef EXPRESSION_DEBUG
        debug("EP: checkEPRules: executing reduction rules.");
    #endif

    if(LookOneAheadEPStack(op_i))
    {
        PopFromEPStack();
        PopFromEPStack();
        PushOntoEPStack(op_E);
        return 1;
    }
    else if(LookTripleAheadEPStack(op_E, Add, op_E)  ||
            LookTripleAheadEPStack(op_E, Sub, op_E)  ||
            LookTripleAheadEPStack(op_E, DivInt, op_E) ||
            LookTripleAheadEPStack(op_E, Mul, op_E)  ||
            LookTripleAheadEPStack(op_E, DivDouble, op_E))
    {
        MODIFY_STACK();
        if(!P_HandleOperand(PopFromStack(temporaryOpStack))) return -1; //it should pop an operator
        return 1;
    }
    else if(LookTripleAheadEPStack(CloseBracket, op_E, OpenBracket))
    {
        MODIFY_STACK();
        return 1;
    }
    else if(LookEndAheadEPStack())
    {
        return 0;       //empty stack
    }
    else return -1;     //cannot find a rule and it is not the end of expression
}

bool ExpressionParse()
{
  #ifdef PARSER_DEBUG
    debug("Expression parse.");
  #endif
  G_Expression();

  //determines what to do for every input
  char ExprParseArray[9][9] = {
       // +    -    \    *    /    (    )    i    $
/* + */ {'>', '>', '<', '<', '<', '<', '>', '<', '>'},
/* - */ {'>', '>', '<', '<', '<', '<', '>', '<', '>'},
/* \ */ {'>', '>', '>', '<', '<', '<', '>', '<', '>'},
/* * */ {'>', '>', '>', '>', '>', '<', '>', '<', '>'},
/* / */ {'>', '>', '>', '>', '>', '<', '>', '<', '>'},
/* ( */ {'<', '<', '<', '<', '<', '<', '=', '<', '#'},
/* ) */ {'>', '>', '>', '>', '>', '#', '>', '#', '>'},
/* i */ {'>', '>', '>', '>', '>', '#', '>', '#', '>'},
/* $ */ {'<', '<', '<', '<', '<', '<', '#', '<', '>'},      //mozna v posledni kolonce '#'
    };

    char x;
    bool tokenChanged = false;
    short int endExprParsing = 0;
    PhrasemData tempIndex;
    TokenType tempType = TokenType_EOF;
    bool failure = false;
    int brackets = 0;
    bool newPhrasem = true;

    //get token hopefully - don't worry, you will
    Phrasem p = CheckQueue(p);

    #ifdef EXPRESSION_DEBUG
        debug("First token received by expression parser: ");
        PrintPhrasem(p);
    #endif

    //if the first token i get is wrong -> expression is empty -> error
    if(p->table != TokenType_Symbol && p->table != TokenType_Constant &&
      (p->table != TokenType_Operator || p->d.index > 9))
    {
        ExpressionError("Empty expression");
        ReturnToQueue(p);
        failure = true;
    }

    //creating a stack for storing operator tokens
    Stack temporaryOpStack = NULL;
    temporaryOpStack = InitStack();
    PushOntoEPStack(op_$);     //start of the stack

    while(!endExprParsing && !failure)
    {
        #ifdef EXPRESSION_DEBUG
            printstackEP();   //<<-- debug
            debug("Token received by expression parser: ");
            PrintPhrasem(p);
        #endif

        //token is operand
        if(p->table == TokenType_Symbol || p->table == TokenType_Constant)
        {
            //x is operation from array [top of stack][number of operator in token]
            x = ExprParseArray[(int)ExprOnTopOfEPStack()][op_i];
            if(x == '<')
            {
                if(LookEAheadEPStack())         //E correction  <E+ ...
                {
                    PopFromEPStack();           //E
                    PushOntoEPStack(op_les);    //<
                    PushOntoEPStack(op_E);      //E
                    PushOntoEPStack(op_i);      //i ... i: operand

                }
                else
                {
                    PushOntoEPStack(op_les);  // <
                    PushOntoEPStack(op_i);    // i: operand

                }
                if(!P_HandleOperand(p)) failure = true;
                p = CheckQueue(p);
                newPhrasem = true;
            }
            else if(x == '#')
            {
                failure = true;
                ExpressionError("Expression error");
            }

        }
        // token is operator
        // 9 is a magical constant -> index into array that determines the maximal value of operator
        // processed in this function (not logical operators)
        // see tables.c operators[9][]
        else if(p->table == TokenType_Operator && p->d.index < 9)
        {
            // close bracket in function call
            if(extraCloseBracket && newPhrasem)
            {
                if(p->d.index == OpenBracket) brackets++;
                else if(p->d.index == CloseBracket)
                {
                    brackets--;
                    if(brackets < 0)
                    {
                        //setting token to ending type to empty the EPstack
                        tempIndex = p->d;       //p->d.index;
                        tempType = p->table;
                        p->d.index = op_$;
                        p->table = TokenType_Operator;
                        tokenChanged = true;
                        //continue;
                    }
                }
                newPhrasem = false;
            }

            //x is operation from array [top of stack][number of operator in token]
            x = ExprParseArray[(int)ExprOnTopOfEPStack()][p->d.index];
            if(x == '<')
            {
                if(LookEAheadEPStack())         //E correction  <E+ ...
                {
                    PopFromEPStack();           //E
                    PushOntoEPStack(op_les);    //<
                    PushOntoEPStack(op_E);      //E
                    PushOntoEPStack(p->d.index);//operator

                }
                else
                {
                    PushOntoEPStack(op_les);        //<
                    PushOntoEPStack(p->d.index);    //operator

                }
                if(p->d.index != OpenBracket && p->d.index != CloseBracket)
                    PushOntoStack(temporaryOpStack, p); //pushing operator (not brackets) on temp stack
                //else freePhrasem(p);        //if token is a bracket, free it
                p = CheckQueue(p);
                newPhrasem = true;
            }
            else if(x == '>')
            {
                int pom;
                pom = checkEPRules(temporaryOpStack);
                if(pom == 1)
                {
                    continue;       //successfuly managed to execute a rule
                }
                else if(pom == 0)
                {
                    //correct ending or a failure
                    if(p->table == TokenType_Operator && p->d.index == op_$)
                        endExprParsing = 1;
                    else
                    {
                        failure = true;
                        ExpressionError("Expression error");
                    }
                }
                else    // pom == -1; cannot find a rule for reduction -> bad expression
                {
                    failure = true;
                    ExpressionError("Expression error");
                }
            }
            else if(x == '=')
            {
                PushOntoEPStack(p->d.index);
                p = CheckQueue(p);
                newPhrasem = true;
            }
            else    // x == '#'
            {
                failure = true;
                ExpressionError("Expression error");
            }
        }
        else    //it is not my symbol
        {
            //setting token to ending type to empty the EPstack
            tempIndex = p->d;       //p->d.index;
            tempType = p->table;
            p->d.index = op_$;
            p->table = TokenType_Operator;
            tokenChanged = true;
        }
        if((endExprParsing || failure) && tokenChanged)
        {   //restoring tokens value and ending analysis
            p->d = tempIndex;
            p->table = tempType;
            if(!ReturnToQueue(p)) failure = true;
        }
        // returns token for recovery after failure
        if(failure == true && !tokenChanged) ReturnToQueue(p);
    }


    //call pedant end function
    if(!failure)
        if(!ExpressionEnd()) failure = true;

    ClearStack(temporaryOpStack);   //should be empty. If its not -> error.
    ClearEPStack();                 //destroying EPStack

    return !failure;
}

bool LogicParse()
{
  #ifdef PARSER_DEBUG
    debug("Logic parse.");
  #endif
  G_Logic();

  bool logicInBraces = false;
  Phrasem br = CheckQueue(br);
  if( isOperator(br, "(") ) logicInBraces = true;
  else ReturnToQueue(br);

  // left
                                        if(logicInBraces) extraCloseBracket = true;
  if(!ExpressionParse()) return false;
                                        Phrasem blb = CheckQueue(blb);
                                        if(isOperator(blb, ")"))
                                        {
                                          logicInBraces = false;
                                        }
                                        else
                                        {
                                          ReturnToQueue(blb);
                                        }
                                        extraCloseBracket = false;

  P_MoveStackToGenerator();

  // parse the sign
  Phrasem p = CheckQueue(p);
  if( !isOperator(p, "=")
  &&  !isOperator(p, "<")
  &&  !isOperator(p, ">")
  &&  !isOperator(p, "<>")
  &&  !isOperator(p, "<=")
  &&  !isOperator(p, ">=") )
  {
    RaiseExpectedError("relation operator");
  }

  // right
  if(logicInBraces) extraCloseBracket = true;
  if(!ExpressionParse()) return false;
  if(logicInBraces) extraCloseBracket = false;

  if(logicInBraces) CheckOperator(")");

  // sending logic operator
  #ifdef PARSER_DEBUG
    PrintPhrasem(p);
  #endif
  if(!P_HandleCompareOperator(p)) return false;

  return true;
}

/*--------------------- ENDER FUNCTIONS -------------------*/

bool EndFunctionParse()
{
  #ifdef PARSER_DEBUG
    debug("End function parse.");
  #endif

  // keyword end
  CheckKeyword("end");

  // keyword function
  CheckKeyword("function");

  // separator
  Phrasem sep = CheckQueue(sep);
  if( !isSeparator(sep) ) RaiseExpectedError("separator");
  //CheckSeparator();

  G_EndBlock();

  // no return
  if(!wasReturn)
  {

    Phrasem def = allocPhrasem();
    if(def == NULL) RaiseError("allocation error", ErrorType_Internal);

    def->table = TokenType_Constant;
    switch( findFunctionType(Config_getFunction()) )
    {
      case DataType_Integer:
        def->d.index = getIntDefaultValue();
        break;
      case DataType_String:
        def->d.index = getStringDefaultValue();
        break;
      case DataType_Double:
        def->d.index = getDoubleDefaultValue();
        break;
      default: RaiseError("unknown datatype", ErrorType_Semantic2);
    }

    ReturnToQueue(sep);
    ReturnToQueue(def);

    if(!ReturnParse()) return false;
  }
  wasReturn = true;
  end = false;
  return true;
}

bool EndIfParse()
{
  #ifdef PARSER_DEBUG
    debug("End if parse.");
  #endif

  // keyword end
  CheckKeyword("end");

  // keyword if
  CheckKeyword("if");

  // separator
  CheckSeparator();

  // generate end
  G_EndBlock();

  end = false;
  return true;
}

bool EndCycleParse()
{
  #ifdef PARSER_DEBUG
    debug("End cycle parse.");
  #endif

  // keyword loop
  CheckKeyword("loop");

  // separator
  CheckSeparator();

  // generate end
  G_EndBlock();

  end = false;
  return true;
}

bool EndScopeParse()
{
  #ifdef PARSER_DEBUG
    debug("End scope parse.");
  #endif

  // keyword end
  CheckKeyword("end");

  // keyword scope
  CheckKeyword("scope");

  // separator
  CheckSeparator();

  setFunction(NULL);

  G_FinalJump(); // jump to end of file

  end = true;
  return true;
}

/*------------------ HIGH LEVEL PARSER FUNCTIONS ---------------------------*/

bool VariableDefinitionParse()
{
  #ifdef PARSER_DEBUG
    debug("Variable definition parse.");
  #endif

  G_VariableDeclaration();

  // getting symbol
  Phrasem var = CheckQueue(var);
  if(!VariableParse(var)) RaiseExpectedError("variable");

  // getting keyword 'as'
  CheckKeyword("as");

  // getting datatype keyword
  Phrasem dt = CheckQueue(dt);
  if(!DataTypeParse(dt)) RaiseExpectedError("datatype keyword");
  DataType type = getDataType(dt);
  if(type == DataType_Unknown) RaiseError("unknown datatype", ErrorType_Internal);

  // declare
  HandlePhrasem(var);
  if(P_VariableDefined(var)) RaiseError("variable already defined", ErrorType_Semantic1);
  if(P_FunctionExists(var)) RaiseError("function of the same name already defined", ErrorType_Semantic1);

  // semantics
  if(!P_DefineNewVariable(var, dt)) return false;

  G_Assignment();

  // getting LF/=
  Phrasem sep = CheckQueue(sep);
  if( isSeparator(sep) )
  {

    // initialization
    Phrasem nul = allocPhrasem();
    if(nul == NULL) return false;
    nul->table = TokenType_Constant;
    switch(type)
    {
      case DataType_Double:
        nul->d.index = getDoubleDefaultValue();
        break;
      case DataType_String:
        nul->d.index = getStringDefaultValue();
        break;
      case DataType_Integer:
        nul->d.index = getIntDefaultValue();
        break;
      default:
        RaiseError("unknown datatype", ErrorType_Internal);
    }

    // volavka
    ReturnToQueue(sep);         // return LF
    ReturnToQueue(nul);         // return 0/0.0/!""

    // process, send to generator
    if(!ExpressionParse()) return false;

    // LF (back, stop point for expressionparse)
    CheckSeparator();

    // assignment target
    if(!P_MoveStackToGenerator()) return false;
    if(!P_HandleTarget(var)) return false;

  }
  // =
  else if(isOperator(sep, "="))
  {
    // get expression
    if(!ExpressionParse()) return false;

    // getting LF
    CheckSeparator();

    // assignment target
    if(!P_MoveStackToGenerator()) return false;
    if(!P_HandleTarget(var)) return false;

  }
  else RaiseError("unexpected token", ErrorType_Syntax);

  return true;
}

bool InputParse()
{
  #ifdef PARSER_DEBUG
    debug("Input parse.");
  #endif

  G_Input();

  // variable
  Phrasem p = CheckQueue(p);

  // error
  if(!VariableParse(p)) return false;

  // control of previous definition
  if( !P_VariableDefined(p)) RaiseError("variable not defined", ErrorType_Semantic1);

  HandlePhrasem(p);

  // LF
  CheckSeparator();

  // semantics call
  return true;
}

bool PrintParse(bool first)
{
  #ifdef PARSER_DEBUG
    debug("Print parse.");
  #endif

  // first parameter
  if(first)
  {
    G_Print();
    if(!ExpressionParse()) return false;
  }

  // non first print parameter (may not exist)
  else
  {
    Phrasem q = CheckQueue(q);
    if( isSeparator(q) ) return true;

    else
    {
      G_Print();

      ReturnToQueue(q);
      if(!ExpressionParse()) return false;
    }
  }

  // ;
  CheckOperator(";");

  P_MoveStackToGenerator();

  G_EndBlock();

  // recursive call of the same function
  return PrintParse(false);
}

bool SymbolParse()
{
  #ifdef PARSER_DEBUG
    debug("Symbol parse.");
  #endif

  Phrasem p = CheckQueue(p);

  if( P_VariableDefined(p) )
  {
    ReturnToQueue(p);

    if( !AssignmentParse() ) return false;

  }
  else RaiseError("unknown symbol", ErrorType_Semantic1);

  return true;
}

bool ReturnParse()
{
  #ifdef PARSER_DEBUG
    debug("Return parse.");
  #endif

  G_Return();

  // Expression
  if(!ExpressionParse()) return false;

  // semantics
  if((Config_getFunction() == NULL) || !strcmp(Config_getFunction(), "scope")) RaiseError("return outside function", ErrorType_Syntax);
  DataType ftype = findFunctionType(Config_getFunction());
  if(!P_CheckType_MoveStackToGenerator(ftype)) return false;

  // LF
  CheckSeparator();

  G_EndBlock();
  wasReturn = true;
  return true;
}

bool BlockParse()
{
  #ifdef PARSER_DEBUG
    debug("Block parse.");
  #endif

  Phrasem p = CheckQueue(p);
  switch(p->table)
  {
    case TokenType_Keyword:
      // variable declaration/definition
      if(matchesKeyword(p, "dim"))
      {
        return VariableDefinitionParse();
      }
      // end
      else if(matchesKeyword(p, "end"))
      {
        ReturnToQueue(p);
        end = true;
        return true;
      }
      // condition
      else if(matchesKeyword(p, "if"))
      {
        return ConditionParse();
      }
      // printing
      else if(matchesKeyword(p, "print"))
      {
        return PrintParse(true);
      }
      // loading
      else if(matchesKeyword(p, "input"))
      {
        return InputParse();
      }
      // cycle
      else if(matchesKeyword(p, "do"))
      {
        return CycleParse();
      }
      // end cycle
      else if(matchesKeyword(p, "loop"))
      {
        ReturnToQueue(p);
        end = true;
      }
      // function return
      else if(matchesKeyword(p, "return"))
      {
        return ReturnParse();
      }
      // function declaration
      else if(matchesKeyword(p, "declare"))
      {
        RaiseError("declaring function inside function", ErrorType_Syntax);
      }
      // function definition
      else if(matchesKeyword(p, "function"))
      {
        RaiseError("defining function inside function", ErrorType_Syntax);
      }
      // main
      else if(matchesKeyword(p, "scope"))
      {
        RaiseError("defining scope inside function", ErrorType_Syntax);
      }
      else if(matchesKeyword(p, "else"))
      {
        ReturnToQueue(p);
        end = true;
      }
      // error
      else RaiseError("unknown keyword", ErrorType_Syntax);

      break;

    // constant
    case TokenType_Constant:
      RaiseError("line beginning with constant", ErrorType_Syntax);

    // operator
    case TokenType_Operator:
      RaiseError("line beginning with operator", ErrorType_Syntax);

    // empty line
    case TokenType_Separator:
      break;

    // assignment, function call
    case TokenType_Symbol:
      ReturnToQueue(p);
      return SymbolParse();

    // EOF
    case TokenType_EOF:
      RaiseError("EOF unexpected", ErrorType_Syntax);

    // default
    default:
      RaiseError("unknown token type", ErrorType_Syntax);
  }

  #ifdef PARSER_DEBUG
    PrintPhrasem(p);
  #endif

  return true;
}

bool FunctionDeclarationParse()
{
  #ifdef PARSER_DEBUG
    debug("Function declaration parse.");
  #endif

  // keyword function
  CheckKeyword("function");

  if(wasScope) RaiseError("declaration after scope", ErrorType_Syntax);

  // function name
  Phrasem funcname = CheckQueue(funcname);
  if( !FunctionParse(funcname) ) return false;

  // nesting
  if(Config_getFunction() != NULL) RaiseError("nested function declaration", ErrorType_Syntax);

  // redefinition
  // function returns -1 if function is not known, 0 if function is declared, 1 if its defined
  if(checkFunctionState(funcname->d.str) != FUNCTION_UNKNOWN)       //types.h l.246
  {
    RaiseError("FunctionDeclarationParse: redeclaration or declaration of an already defined function",
                                                                                ErrorType_Semantic1);
  }
  if(P_VariableDefined(funcname)) RaiseError("variable of the same name already defined", ErrorType_Semantic1);
  // later you just have to call P_DeclareNewFunction
  // pouzivat funkci P_FunctionDefined je zbytecne

  // parameters declaration
  Parameters params = paramInit();

  // operator (
  CheckOperator("(");
  Phrasem arg = CheckQueue(arg);
  // no parameters
  if(isOperator(arg, ")")) ReturnToQueue(arg);
  // parameters
  else
  {
    ReturnToQueue(arg);

    // parameters
    while(1)
    {
      // variable
      Phrasem arg = CheckQueue(arg);
      if(!VariableParse(arg)) RaiseExpectedError("identificator");
      if(findParamName(params, arg->d.str)) RaiseError("duplicit parameter name", ErrorType_Semantic1);

      // keyword 'as'
      CheckKeyword("as");

      // datatype keyword
      Phrasem type = CheckQueue(type);
      if(!DataTypeParse(type)) RaiseExpectedError("datatype");
      DataType dt = getDataType(type);

      // parameter to add
      if(!paramAdd(&params, arg->d.str, dt))
        RaiseError("list allocation error", ErrorType_Internal);

      // , or )
      Phrasem op = CheckQueue(op);
      if(isOperator(op, ",")) continue;        // ,
      else if(isOperator(op, ")"))             // )
      {
        ReturnToQueue(op);
        break;
      }
      else RaiseExpectedError("operator , or )");

    }

  }

  #ifdef PARSER_DEBUG
    PrintParameters(params);
  #endif

  // operator )
  CheckOperator(")");

  // keyword 'as'
  CheckKeyword("as");

  // datatype keyword
  Phrasem type = CheckQueue(type);
  if(!DataTypeParse(type)) RaiseExpectedError("datatype");

  // LF
  CheckSeparator();

  if(!P_DeclareNewFunction(funcname, type, params))
    RaiseError("symtable allocation error", ErrorType_Internal);

  return true;
}

bool CycleParse()
{
  #ifdef PARSER_DEBUG
    debug("Cycle parse.");
  #endif

  G_Cycle();

  // keyword then
  CheckKeyword("while");

  if(!LogicParse()) return false;

  // LF
  CheckSeparator();

  // cycle body
  do {
    if(!BlockParse()) return false;
  } while(!end);

  end = false;

  if(!EndCycleParse()) return false;

  end = false;
  return true;
}

bool ConditionParse()
{
  #ifdef PARSER_DEBUG
    debug("Condition parse.");
  #endif

  G_Condition();
  if(!LogicParse()) return false;

  // keyword 'then'
  CheckKeyword("then");
  // LF
  CheckSeparator();

  while(1)
  {
    if(!BlockParse()) return false;

    if(end) break;
  }
  end = false;

  G_Else();

  // else
  CheckKeyword("else");
  CheckSeparator();
  while(1)
  {
    if(!BlockParse()) RaiseError("unexpected token", ErrorType_Syntax);

    if(end) break;
  }
  end = false;

  // end if
  if(!EndIfParse()) return false;

  // error
  else


  return true;
}

bool AssignmentParse()
{
  #ifdef PARSER_DEBUG
  debug("Assignment parse.");
  #endif
  G_Assignment();

  Phrasem var = CheckQueue(var);
  if(!VariableParse(var)) return false;
  if(!P_VariableDefined(var)) RaiseError("unknown variable", ErrorType_Semantic1);

  // get variable type
  DataType dt = findVariableType(Config_getFunction(), var->d.str);

  // =
  CheckOperator("=");

  Phrasem func = CheckQueue(func);

  // embedded functions
  if( matchesKeyword(func, "length") )
  {
    if(!LengthParse()) return false;
    if(!P_HandleTarget(var)) return false;
  }
  else if( matchesKeyword(func, "substr") )
  {
    if(!SubStrParse()) return false;
    if(!P_HandleTarget(var)) return false;
  }
  else if( matchesKeyword(func, "asc") )
  {
    if(!AscParse()) return false;
    if(!P_HandleTarget(var)) return false;
  }
  else if( matchesKeyword(func, "chr") )
  {
    if(!ChrParse()) return false;
    if(!P_HandleTarget(var)) return false;
  }

  else if((func->table == TokenType_Symbol) && P_FunctionExists(func))
  {
    ReturnToQueue(func);
    // function call
    if(!FunctionCallParse()) return false;

    if(!P_CheckDataType(dt)) return false;

    G_FunctionAssignment(var);
  }
  else
  {
    ReturnToQueue(func);

    if(getUnary())
    {
      Phrasem vardup = duplicatePhrasem(var);
      if(vardup == NULL) RaiseError("allocation failed", ErrorType_Internal);
      vardup->table = TokenType_Symbol;

      if(!ReturnToQueue(vardup)) return false;
    }

    // expression
    if(!ExpressionParse()) return false;

    if(!P_MoveStackToGenerator()) return false;
    if(!P_HandleTarget(var)) return false;
  }

  // LF
  CheckSeparator();

  return true;
}

bool FunctionCallParse()
{
  #ifdef PARSER_DEBUG
    debug("Function call parse.");
  #endif
  G_FunctionCall();

  Phrasem funcname = CheckQueue(funcname);
  if( !FunctionParse(funcname) ) return false;

  // was declared/defined
  if( !P_FunctionExists(funcname) ) RaiseError("calling unknown function", ErrorType_Semantic1);

  // (
  CheckOperator("(");

  // iterate over arguments
  extraCloseBracket = true;
  Parameters params = findFunctionParameters(funcname->d.str);
  for(unsigned ord = 1; params != NULL; ord++)
  {
    DataType dt = params->type;
    G_ArgumentAssignment(ord);
    if(!ExpressionParse()) return false;

    if(!P_MoveStackToGenerator()) return false;
    if(!P_CheckDataType(dt)) return false;
    GenerateArgument();

    if(params->next != NULL)
    {
      // CheckOperator(",");
      Phrasem p = CheckQueue(p);
      if( isOperator(p, ")") ) RaiseError("bad arguments count", ErrorType_Semantic2);
      else if( !isOperator(p, ",") ) RaiseError("operator \',\' expected", ErrorType_Syntax);
    }
    params = params->next;
  }

  if(params != NULL) RaiseError("too many arguments", ErrorType_Semantic1);

  // )
  CheckOperator(")");
  extraCloseBracket = false;

  HandlePhrasem(funcname);
  P_HangDataType(findFunctionType(funcname->d.str));

  return true;
}

bool LengthParse()
{
  #ifdef PARSER_DEBUG
    debug("Length call parse.");
  #endif

  G_Length();
  G_Empty(); // 1. parameter action

  CheckOperator("(");

  extraCloseBracket = true;
  if(!ExpressionParse()) return false;
  extraCloseBracket = false;

  /*----------- GENERATOR -----------*/
  G_Expression2StringExpression();
  if(!P_MoveStackToGenerator()) return false;
  if(!P_CheckDataType(DataType_String)) return false;
  /*---------------------------------*/

  GenerateBuiltIn();
  CheckOperator(")");
  P_HangDataType(DataType_Integer);

  // call
  return true;
}

bool SubStrParse()
{
  #ifdef PARSER_DEBUG
    debug("SubStr call parse.");
  #endif

  G_SubStr();
  G_Empty(); // 1. parameter action
  G_Empty(); // 2. parameter action
  G_Empty(); // 3. parameter action

  CheckOperator("(");

  if(!ExpressionParse()) return false;
  /*-------------- GENERATOR ------------*/
  G_Expression2StringExpression();
  P_MoveStackToGenerator();
  if(!P_CheckDataType(DataType_String)) return false;
  /*-------------------------------------*/

  CheckOperator(",");

  if(!ExpressionParse()) return false;
  /*-------------- GENERATOR ------------*/
  P_MoveStackToGenerator();
  if(!P_CheckDataType(DataType_Integer)) return false;
  /*-------------------------------------*/

  CheckOperator(",");
  extraCloseBracket = true;
  if(!ExpressionParse()) return false;
  extraCloseBracket = false;
  /*-------------- GENERATOR ------------*/
  P_MoveStackToGenerator();
  if(!P_CheckDataType(DataType_Integer)) return false;
  /*-------------------------------------*/

  GenerateBuiltIn();

  CheckOperator(")");
  P_HangDataType(DataType_String);

  return true;
}

bool AscParse()
{
  #ifdef PARSER_DEBUG
    debug("Asc call parse.");
  #endif

  G_Asc();
  G_Empty(); // 1. parameter action
  G_Empty(); // 2. parameter action

  CheckOperator("(");

  if(!ExpressionParse()) return false;
  /*-------------- GENERATOR ------------*/

  G_Expression2StringExpression();
  P_MoveStackToGenerator();
  if(!P_CheckDataType(DataType_String)) return false;
  /*-------------------------------------*/

  CheckOperator(",");

  extraCloseBracket = true;
  if(!ExpressionParse()) return false;
  extraCloseBracket = false;
  /*-------------- GENERATOR ------------*/
  P_MoveStackToGenerator();
  if(!P_CheckDataType(DataType_Integer)) return false;
  /*-------------------------------------*/

  GenerateBuiltIn();

  CheckOperator(")");

  return true;
}

bool ChrParse()
{
  #ifdef PARSER_DEBUG
    debug("Chr call parse.");
  #endif

  G_Int2Str();
  G_Empty(); // 1. parameter action

  CheckOperator("(");

  extraCloseBracket = true;
  if(!ExpressionParse()) return false;

  /*-------------- GENERATOR ------------*/
  P_MoveStackToGenerator();
  if(!P_CheckDataType(DataType_Integer)) return false;
  /*-------------------------------------*/

  GenerateBuiltIn();

  CheckOperator(")");
  P_HangDataType(DataType_String);

  return true;
}

bool FunctionDefinitionParse()
{
  #ifdef PARSER_DEBUG
    debug("Function definition parse.");
  #endif

  G_Function();
  wasReturn = false;
  if(wasScope) RaiseError("definition after scope", ErrorType_Syntax);

  // function name
  Phrasem funcname = CheckQueue(funcname);
  if( !FunctionParse(funcname) ) return false;

  HandlePhrasem(funcname);

  // nesting control
  if(Config_getFunction() != NULL)
    RaiseError("nested function definition", ErrorType_Syntax);

  // parameters declaration
  Parameters params = paramInit();

  // operator (
  CheckOperator("(");
  Phrasem arg = CheckQueue(arg);
  // no parameters
  if(isOperator(arg, ")")) ReturnToQueue(arg);
  // parameters
  else
  {
    ReturnToQueue(arg);

    // parameters
    for(unsigned i = 1; 1; i++)
    {
      // variable
      Phrasem arg = CheckQueue(arg);
      if(!VariableParse(arg)) RaiseExpectedError("identificator");
      if(findParamName(params, arg->d.str)) RaiseError("duplicit parameter name", ErrorType_Semantic1);

      // keyword 'as'
      CheckKeyword("as");

      // datatype keyword
      Phrasem type = CheckQueue(type);
      if(!DataTypeParse(type)) RaiseExpectedError("datatype");
      DataType dt = getDataType(type);

      // parameter to add
      if(!paramAdd(&params, arg->d.str, dt))
        RaiseError("list allocation error", ErrorType_Internal);

      G_VariableDeclaration();
      if(!HandlePhrasem(arg)) return false;
      AssignArgument(arg, i);

      // , or )
      Phrasem op = CheckQueue(op);
      if(isOperator(op, ",")) continue;        // ,
      else if(isOperator(op, ")"))             // )
      {
        ReturnToQueue(op);
        break;
      }
      else RaiseExpectedError("operator , or )");

    }

  }

  #ifdef PARSER_DEBUG
    PrintParameters(params);
  #endif

  // operator )
  CheckOperator(")");

  // keyword 'as'
  CheckKeyword("as");

  // datatype keyword
  Phrasem type = CheckQueue(type);
  if(!DataTypeParse(type)) RaiseExpectedError("datatype");
  DataType dt = getDataType(type);

  // LF
  CheckSeparator();

  /*-------------------- SEMANTICS ----------------------*/

  short int state;
  state = checkFunctionState(funcname->d.str);
  if(state == FUNCTION_DEFINED)
  {
    RaiseError("FunctionDefinitionParse: redefinition of a function", ErrorType_Semantic1);
  }
  else if(state == FUNCTION_DECLARED)
  {
    // check params in declaration
    if(!ParametersMatches(params, findFunctionParameters(funcname->d.str)))
      RaiseError("not matching parameters in declaration and definition", ErrorType_Semantic1);
    // check return value datatype
    if(dt != findFunctionType(funcname->d.str))
      RaiseError("not matching return datatype in declaration and definition", ErrorType_Semantic1);
  }
  if(P_VariableDefined(funcname)) RaiseError("variable of the same name already defined", ErrorType_Semantic1);

  // define new function
  if(!P_DefineNewFunction(funcname, type, params))
    RaiseError("error defining function", ErrorType_Internal);  // cannot be any other type of error

  /*-----------------------------------------------------*/

  // in the semantics
  if(!setFunction(funcname->d.str)) RaiseError("allocation error", ErrorType_Internal);

  do {
      if(!BlockParse()) return false;
  } while(!end);

  // end function
  if(!EndFunctionParse()) return false;

  // set no function
  setFunction(NULL);

  end = false;
  return true;
}

bool ScopeParse()
{
  #ifdef PARSER_DEBUG
    debug("Scope parse.");
  #endif

  G_Scope();
  if(wasScope) RaiseError("multiple scope definition", ErrorType_Syntax);
  wasScope = true;

  // nesting
  if(Config_getFunction() != NULL) RaiseError("nested functions", ErrorType_Internal);

  // actualizing function
  setFunction("scope");
  addFunction(Config_getFunction());
  addFunctionParameters(Config_getFunction(), NULL, true);      // sets the function as defined

  // LF
  CheckSeparator();

  do {
    if(!BlockParse()) return false;
  } while(!end);

  // end scope parse
  if(!EndScopeParse()) return false;

  // set no function
  setFunction(NULL);
  end = false;
  return true;
}

bool GlobalBlockParse()
{
  #ifdef PARSER_DEBUG
    debug("Global block parse.");
  #endif

  // control not in function
  if(Config_getFunction() != NULL) RaiseError("nested functions", ErrorType_Internal);

  // read function
  Phrasem p = CheckQueue(p);

  // EOF
  if(p->table == TokenType_EOF) end = true;
  else if(p->table == TokenType_Separator) return true;
  // not a keyword
  else if(p->table != TokenType_Keyword) RaiseError("syntax error on global level", ErrorType_Syntax);

  // function declaration
  else if(matchesKeyword(p, "declare")) return FunctionDeclarationParse();
  // function definition
  else if(matchesKeyword(p, "function")) return FunctionDefinitionParse();
  // function definition
  else if(matchesKeyword(p, "scope")) return ScopeParse();
  // error (global not supported)
  else RaiseError("syntax error on global level", ErrorType_Syntax);

  return true;
}

/*---------------------------- CLEAR --------------------------------*/
void EndParser(const char * msg, ErrorType errtype)
{

  //EndRoutine();

  if(msg != NULL)
  {
    #ifdef PARSER_DEBUG
      debug(msg);
    #endif
    setErrorType(errtype);
    setErrorMessage(msg);
    setErrorLine(Config_getLine());
  }
}
