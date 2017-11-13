
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
#include <unistd.h>

#include "config.h"
#include "err.h"
#include "functions.h"
#include "generator.h"
#include "io.h"
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

char * mfunction; /**< Id of actual function (for symbol table). */
bool setFunction(const char * f)
{
  if(f == NULL)
  {
    if(mfunction != NULL) free(mfunction);
    mfunction = NULL;
    return true;
  }

  if(mfunction != NULL) free(mfunction);
  mfunction = malloc(sizeof(char)*(strlen(f) + 1));
  if(mfunction == NULL) return false;

  strcpy(mfunction, f);

  #ifdef PARSER_DEBUG
    debug("Function: %s", (mfunction!=NULL)?mfunction:"none");
  #endif
  return true;
}

bool end = false; /**< Set to true, if keyword end reached. */

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
#define RaiseError(msg, phrasem, errtype)                       \
  do {                                                          \
    err("%s: %s: l.%d: %s", __FILE__, __func__, __LINE__, msg); \
    EndParser(msg, phrasem ->line, errtype);                    \
    freePhrasem(phrasem);                                       \
    return false;                                               \
  } while(0)

/**
 * @brief Queue error raiser.
 *
 * This macro raises specialized Queue error(when it returns NULL pointer).
 * @param phrasem     Target memory.
 */
#define RaiseQueueError(phrasem)                                \
  do {                                                          \
    RaiseError("queue error", phrasem, ErrorType_Internal);     \
  } while(0)

/**
 * @brief -expected- error raiser.
 *
 * This macro raises specialized error -what expected-.
 * @param what        Token expected (in quotes)
 * @param phrasem     Target memory.
 */
#define RaiseExpectedError(what, phrasem)                       \
  do {                                                          \
    RaiseError(what " expected", phrasem, ErrorType_Syntax);    \
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
      RaiseExpectedError("separator", p);                     \
    }                                                         \
    free(p);                                                  \
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
      RaiseExpectedError("operator \'" op "\'", p);           \
    }                                                         \
    free(p);                                                  \
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
      RaiseExpectedError("keyword \'" kw "\'", p);            \
    }                                                         \
    free(p);                                                  \
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
        RaiseQueueError(phrasem);               \
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
        RaiseQueueError(phrasem);               \
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
void EndParser(const char * msg, int line, ErrorType errtype);
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

  // clear memory
  constTableFree();       //free the table of constants
	functionTableEnd();     //free the table of symbols
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
 * @brief   Parses arguments of function call.
 *
 * This function will parse the arguments of function.
 * @returns True, if success. False otherwise.
 */
bool FunctionArgumentsParse();

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

  if(bypass())
  {
    while(1)
    {
      Phrasem p = CheckQueue(p);
      if(p->table == TokenType_EOF) return true;
      PrintPhrasem(p);
      free(p);
    }
  }

  if(!constTableInit()) return false;
  InitGenerator();

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
          if(end) break;

          // something to do after each function
        }
  #endif  // --------------------------------------------

  // end
  EndRoutine();
  if(mfunction != NULL) { free(mfunction); return false; }
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
    RaiseExpectedError("variable", p);
  }

  if(p->table != TokenType_Symbol)
  {
    RaiseExpectedError("variable name", p);
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
    RaiseExpectedError("function", p);
  }

  if(p->table != TokenType_Symbol)
  {
    RaiseExpectedError("function name", p);
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

/*
//returns an index into ExprParseArray
int decodeToken(...)
{
}
*/

#define MODIFY_STACK()              \
  do {                              \
    PopFromEPStack();               \
    PopFromEPStack();               \
    PopFromEPStack();               \
    PopFromEPStack();               \
    PushOntoEPStack(op_E);           \
  } while(0)

//tries to perform stack modifications based on rules (called when '>')
int checkEPRules(/*Stack returnStack, */Stack temporaryOpStack)
{
    #ifdef PARSER_DEBUG
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
        P_HandleOperand(PopFromStack(temporaryOpStack)); //it should pop an operator
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

  //creating a stack that will be given to pedant for semantical EP analysis
  //Stack returnStack = NULL;
  //returnStack = InitStack();

  //creating a stack for storing operator tokens
  Stack temporaryOpStack = NULL;
  temporaryOpStack = InitStack();

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

    PushOntoEPStack(op_$);     //start of the stack

    //get token hopefully - don't worry, you will
    Phrasem p = CheckQueue(p);

    do
    {
//printstackEP();   <<-- debug
        //token is operand
        if(p->table == TokenType_Symbol || p->table == TokenType_Constant)
        {
            //if symbol ...
            //zeptat se symtable, jestli je to fce nebo prom
            // + zavolat variable/fce parser
            //pokud je to fce -> chyba (mozne rozsireni)
            // ...

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
                P_HandleOperand(p);
                //PushOntoStack(returnStack, p);      //pushing operand on stack
                p = CheckQueue(p);
            }
            else if(x == '#')
            {
                failure = true;
            }

        }
        // token is operator
        // 7 is a magical constant -> index into array that determines the maximal value of operator
        // processed in this function (not logical operators)
        // see tables.c operators[9][]
        else if(p->table == TokenType_Operator && p->d.index < 9)
        {
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
                if(p->d.index != 5 && p->d.index != 6)
                    PushOntoStack(temporaryOpStack, p); //pushing operator (not brackets) on temp stack
                p = CheckQueue(p);
            }
            else if(x == '>')
            {
                int pom;
                pom = checkEPRules(/*returnStack, */temporaryOpStack);
                if(pom == 1)
                {
                    continue;       //successfuly managed to execute a rule
                }
                else if(pom == 0)
                {
                    //correct ending or a failure
                    if(p->table == TokenType_Operator && p->d.index == op_$)
                        endExprParsing = 1;
                    else failure = true;
                }
                else    // pom == -1
                {
                    //chybove hlaseni
                    failure = true;
                }
            }
            else if(x == '=')
            {
                PushOntoEPStack(p->d.index);
                p = CheckQueue(p);
            }
            else    // x == '#'
            {
                //chybove hlaseni
                failure = true;
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
    }while(!endExprParsing && !failure);

    ClearStack(temporaryOpStack);   //should be empty. If its not -> error.
    ClearEPStack();                 //destroying EPStack
    //if(!TurnStack(returnStack)) return false;   //turning the stack

    if(failure) EndRoutine();
    return !failure;
}

bool LogicParse()
{
  #ifdef PARSER_DEBUG
    debug("Logic parse.");
  #endif
  G_Logic();

  /*
  // left
  if(!ExpressionParse()) return false;
  */
  Phrasem q = CheckQueue(q);

  // parse the sign
  Phrasem p = CheckQueue(p);
  if( !isOperator(p, "=")
  &&  !isOperator(p, "<")
  &&  !isOperator(p, ">")
  &&  !isOperator(p, "<>")
  &&  !isOperator(p, "<=")
  &&  !isOperator(p, ">=") )
  {
    RaiseExpectedError("relation operator", p);
  }
  /*
  // right
  if(!ExpressionParse()) return false;
  */
  Phrasem r = CheckQueue(r);

  // sending logic operator
  #ifdef PARSER_DEBUG
    PrintPhrasem(p);
  #endif
  if(!HandlePhrasem(p)) return false;

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
  CheckSeparator();

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

  if(!VariableParse(var))
  {
    RaiseExpectedError("variable", var);
  }

  // getting keyword 'as'
  CheckKeyword("as");

  // getting datatype keyword
  Phrasem dt = CheckQueue(dt);

  if(!DataTypeParse(dt))
  {
    RaiseExpectedError("datatype keyword", dt);
  }
  DataType type = getDataType(dt);
  if(type == DataType_Unknown)
  {
    freePhrasem(var);
    RaiseError("unknown datatype", dt, ErrorType_Semantic1);
  }

  Phrasem dup = duplicatePhrasem(var); // deep copy

  // declare
  HandlePhrasem(var);           /*<<== var destroyed */

  if(P_VariableDefined(mfunction, dup))
  {
    freePhrasem(dt);
    RaiseError("variable was already defined", dup, ErrorType_Semantic1);
  }

  // semantics
  if(!P_DefineNewVariable(mfunction, dup, dt))
  {
    free(dup);
    free(dt);
    return false;
  }
  free(dt);

  G_Assignment();

  // getting LF/=
  Phrasem sep = CheckQueue(sep);
  if( isSeparator(sep) )
  {

    // initialization
    Phrasem nul = malloc(sizeof(struct phrasem_data));
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
        freePhrasem(sep);
        freePhrasem(dup);
        RaiseError("unknown datatype", nul, ErrorType_Semantic1);
    }

    /*
    // volavka
    ReturnToQueue(sep);         // return LF
    ReturnToQueue(nul);         // return 0/0.0/!""


    // process, send to generator
    if(!ExpressionParse()) return false;

    // LF (back, stop point for expressionparse)
    CheckSeparator();
    */

    // association target
    HandlePhrasem(dup);               /*<<== dup destroyed */


  }
  // =
  else if(isOperator(sep, "="))
  {
    freePhrasem(sep);

    // get expression
    if(!ExpressionParse())
    {
      RaiseError("invalid expression", dup, ErrorType_Syntax);
    }

    // getting LF
    CheckSeparator();

  }
  else
  {
    freePhrasem(dup);
    RaiseError("unexpected token", dup, ErrorType_Syntax);
  }

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
  if(!VariableParse(p))
  {
    return false;
  }

  // control of previous definition
  if( !P_VariableDefined(mfunction, p)) return false;

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
  // expression
  // first parameter
  if(first)
  {
    G_Print();
    G_Expression();
    if(!ExpressionParse()) return false;
  }
  // non first print parameter (may not exist)
  else
  {
    Phrasem q = CheckQueue(q);

    if( isSeparator(q) )
    {
      free(q);
      return true;
    }

    else
    {
      G_Print();

      if(!ReturnToQueue(q)) RaiseQueueError(q);

      if(!ExpressionParse()) return false;
    }
    free(q);
  }

  // ;
  CheckOperator(";");

  // recursive call of the same function
  return PrintParse(false);
}

bool FunctionArgumentsParse()
{
  #ifdef PARSER_DEBUG
    debug("Arguments parse.");
  #endif
  if(!ExpressionParse()) return false;

  Phrasem p = CheckQueue(p);

  if(isOperator(p, ","))
  {
    return FunctionArgumentsParse();
  }
  else if(isOperator(p, ")"))
  {
    if(!ReturnToQueue(p)) RaiseQueueError(p);
  }
  return true;
}

bool SymbolParse()
{
  #ifdef PARSER_DEBUG
    debug("Symbol parse.");
  #endif

  Phrasem p = CheckQueue(p);

  if( P_VariableDefined(mfunction, p) )
  {
    G_Assignment();

    // =
    CheckOperator("=");

    // expression
    if(!ExpressionParse()) return false;

    // LF
    CheckSeparator();

    // Generator
    HandlePhrasem(p);

  }
  else if( P_FunctionDefined(p->d.str) )
  {
    // '(' token
    CheckOperator("(");

    // arguments
    if(!FunctionArgumentsParse()) return false;

    // ')' token
    CheckOperator(")");

    // LF
    CheckSeparator();

  }
  else
  {
    // raise semantic error
  }

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
        if(!ConditionParse()) return false;
      }
      // printing
      else if(matchesKeyword(p, "print"))
      {
        PrintParse(true);
      }
      // loading
      else if(matchesKeyword(p, "input"))
      {
        InputParse();
      }
      // cycle
      else if(matchesKeyword(p, "do"))
      {
        CycleParse();
      }
      // end cycle
      else if(matchesKeyword(p, "loop"))
      {
        ReturnToQueue(p);
        end = true;
        return true;
      }
      // function return
      else if(matchesKeyword(p, "return"))
      {
      }
      // function declaration
      else if(matchesKeyword(p, "declare"))
      {
        RaiseError("declaring function inside function", p, ErrorType_Syntax);
      }
      // function definition
      else if(matchesKeyword(p, "function"))
      {
        RaiseError("defining function inside function", p, ErrorType_Syntax);
      }
      // main
      else if(matchesKeyword(p, "scope"))
      {
        RaiseError("defining scope inside function", p, ErrorType_Syntax);
      }
      // error
      else
      {
        RaiseError("unknown keyword", p, ErrorType_Syntax);
      }
      break;

    // constant
    case TokenType_Constant:
      RaiseError("line beginning with constant", p, ErrorType_Syntax);

    // operator
    case TokenType_Operator:
      RaiseError("line beginning with operator", p, ErrorType_Syntax);

    // empty line
    case TokenType_Separator:
      break;

    // assignment, function call
    case TokenType_Symbol:
      ReturnToQueue(p);
      return SymbolParse();

    // EOF
    case TokenType_EOF:
      RaiseError("EOF unexpected", p, ErrorType_Syntax);

    // default
    default:
      RaiseError("unknown token type", p, ErrorType_Syntax);
  }

  #ifdef PARSER_DEBUG
    PrintPhrasem(p);
  #endif

  freePhrasem(p);
  return true;
}

bool FunctionDeclarationParse()
{
  #ifdef PARSER_DEBUG
    debug("Declaration parse.");
  #endif

  // keyword function
  CheckKeyword("function");

  // function name
  Phrasem funcname = CheckQueue(funcname);
  if( !FunctionParse(funcname) ) return false;

  // nesting
  if(mfunction != NULL) RaiseError("nested function declaration", funcname, ErrorType_Syntax);

  // operator (
  CheckOperator("(");

  // parameters declaration
  // ...

  // operator )
  CheckOperator(")");

  // LF
  CheckSeparator();



  return true;
}

bool CycleParse()
{
  #ifdef PARSER_DEBUG
    debug("Cycle parse.");
  #endif

  if(!LogicParse()) return false;
  G_Cycle();

  // keyword then
  CheckKeyword("while");
  // LF
  CheckSeparator();

  // cycle body
  do {
    if(!BlockParse()) return false;
  } while(!end);

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

  // end if
  if(!EndIfParse()) return false;

  return true;
}

bool AssignmentParse()
{
  #ifdef PARSER_DEBUG
  debug("Assignment parse.");
  #endif
  G_Assignment();

  // =
  CheckOperator("=");

  // expression
  if(!ExpressionParse()) return false;

  // LF
  CheckSeparator();

  return true;
}

bool FunctionDefinitionParse()
{
  #ifdef PARSER_DEBUG
    debug("Function definition parse.");
  #endif

  // function name
  Phrasem funcname = CheckQueue(funcname);
  if( !FunctionParse(funcname) ) return false;

  // nesting control
  if(mfunction != NULL)
  {
    RaiseError("nested function definition", funcname, ErrorType_Syntax);
  }

  // operator (
  CheckOperator("(");

  // parameters declaration
  // ...

  // operator )
  CheckOperator(")");

  // LF
  CheckSeparator();

  // check symbol table
  // in the semantics
  // ...

  CheckKeyword("begin");

  do {
      if(!BlockParse()) return false;
  } while(!end);

  // end function
  if(!EndFunctionParse()) return false;


  return true;
}

bool ScopeParse()
{
  #ifdef PARSER_DEBUG
    debug("Scope parse.");
  #endif

  // nesting
  if(mfunction != NULL)
  {
    EndParser("nested functions not supported", -1, ErrorType_Internal);
    return false;
  }
  // actualizing function
  setFunction("scope");
  P_DefineNewFunction(mfunction);

  // LF
  CheckSeparator();

  do {
    if(!BlockParse()) return false;
  } while(!end);

  // end scope parse
  if(!EndScopeParse()) return false;

  end = false;
  return true;
}

bool GlobalBlockParse()
{
  #ifdef PARSER_DEBUG
    debug("Global block parse.");
  #endif
  bool status = true;

  if(mfunction != NULL)
  {
    EndParser("Parser: GlobalBlockParse: nested functions not supported", -1, ErrorType_Internal);
    return false;
  }

  // read function
  Phrasem p = CheckQueue(p);

  // EOF
  if(p->table == TokenType_EOF) end = true;
  // not a keyword
  else if(p->table != TokenType_Keyword) RaiseError("syntax error on global level", p, ErrorType_Syntax);

  // function declaration
  else if(matchesKeyword(p, "declare")) status = FunctionDeclarationParse();
  // function definition
  else if(matchesKeyword(p, "function")) status = FunctionDefinitionParse();
  // function definition
  else if(matchesKeyword(p, "scope")) status = ScopeParse();
  // error (global not supported)
  else RaiseError("syntax error on global level", p, ErrorType_Syntax);

  freePhrasem(p);
  return status;
}

/*---------------------------- CLEAR --------------------------------*/
void EndParser(const char * msg, int line, ErrorType errtype)
{

  //EndRoutine();

  if(msg != NULL)
  {
    #ifdef PARSER_DEBUG
      debug(msg);
    #endif
    setErrorType(errtype);
    setErrorMessage(msg);
    if(line != -1) setErrorLine(line);
  }
  else
  {
    setErrorType(ErrorType_Ok);
    setErrorMessage("");
  }
}
