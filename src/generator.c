
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "functions.h"
#include "generator.h"
#include "io.h"
#include "stack.h"
#include "symtable.h"
#include "types.h"

/*--------------------------------------------------*/
/** @addtogroup Generator_types
 * Types for generator.
 * @{
 */

/*------- GENERATOR STATE STACK --------*/
/**
 * @brief   Types of generator states.
 *
 * Types of states, generator can coordinate
 * throughout his inner state stack.
 */
typedef enum
{
  // blocks
  GState_Condition,             /**< Condition. */
  GState_Else,                  /**< Alternative. */
  GState_Cycle,                 /**< Cycle. */
  GState_Function,              /**< Function. */

  // lines
  GState_Assignment,            /**< Assignment. */
  GState_Input,                 /**< Input. */
  GState_Print,                 /**< Print. */
  GState_VariableDeclaration,   /**< Variable declaration. */
  GState_Return,                /**< Return. */

  // partials
  GState_Expression,            /**< Expression. */
  GState_StringExpression,      /**< String expression. */
  GState_FunctionCall,          /**< Function call. */
  GState_Argument,              /**< Argument in function call. */
  GState_Logic,                 /**< Logic (cycle, condition). */
  GState_FunctionHeader,        /**< Head of a function. */

  // built-in functions
  GState_Length,                /**< Function length(). */
  GState_Int2Str,               /**< Function chr(). */
  GState_Asc,                   /**< Function asc(). */
  GState_SubStr,                /**< Function substr(). */

  // others
  GState_Empty                  /**< Empty. */
} GState;

/**
 * @brief   GState to string convertor.
 *
 * This function returns string from GState given. It is used for debug.
 * @param st        Given state.
 * @returns string representation.
 */
char * GStateToStr(GState st)
{
  switch(st)
  {
    case GState_Asc: return "GState_Asc";
    case GState_Argument: return "GState_Argument";
    case GState_Assignment: return "GState_Assignment";
    case GState_Condition: return "GState_Condition";
    case GState_Cycle: return "State_Cycle";
    case GState_Empty: return "GState_Empty";
    case GState_Expression: return "GState_Expression";
    case GState_Function: return "GState_Function";
    case GState_FunctionCall: return "GState_FunctionCall";
    case GState_FunctionHeader: return "GState_FunctionHeader";
    case GState_Input: return "GState_Input";
    case GState_Int2Str: return "GState_Int2Str";
    case GState_Length: return "GState_Length";
    case GState_Logic: return "GState_Logic";
    case GState_Print: return "GState_Print";
    case GState_Return: return "GState_Return";
    case GState_StringExpression: return "GState_StringExpression";
    case GState_SubStr: return "GState_SubStr";
    case GState_VariableDeclaration: return "GState_VariableDeclaration";
    default: return "UNKNOWN STATE!";
  }
}

/**
 * @brief   Item in the generator state stack.
 */
typedef struct state_item
{
  GState state;               /**< State. */
  struct state_item * next;   /**< Pointer to next. */
} * StateItem;

/**
 * @brief Generator inner state stack.
 */
typedef struct genstack
{
  StateItem first; /**< Pointer to first. */
} GeneratorStack;

/**
 * @brief   Pushes new state.
 *
 * This function takes GState, makes an item from it (dynamically)
 * and pushes it to inner GeneratorStack.
 * @param newstate      New state.
 * @returns False if fail. True otherwise.
 */
inline bool PushGState(GState newstate);
/**
 * @brief   Removes top state.
 *
 * This function deletes the most top GState from a GeneratorStack.
 */
inline void RemoveGState();

/**
 * @brief   Returns top state.
 *
 * This function returns the most top GState from a GeneratorStack.
 * @returns GState on the top, or GState_Empty, if empty.
 */
inline GState LookUpGState();

/**
 * @brief   Removes and returns top state.
 *
 * This function removes the most top GState from a GeneratorStack
 * and returns it.
 * @returns GState on the top, or GState_Empty, if empty.
 */
inline GState PopGState();

/**
 * @brief   Clears state stack.
 *
 * This function clears whole GeneratorStack.
 * @returns GState on the top, or GState_Empty, if empty.
 */
inline void ClearGStates();

/**
 * @brief   Prints state stack.
 *
 * This function prints whole GeneratorStack.
 */
inline void PrintGStateStack();

/*-------------------- LABEL STACK ------------------------*/

/**
 * @brief   Item of the LabelStack.
 */
typedef struct label_item
{
  const char * lbl;           /**< Name of the label. */
  struct label_item * next;   /**< Pointer to next. */
} * LabelItem;

/**
 * @brief   Stack of labels.
 *
 * This will be used to save the names of labels to jump onto
 * in the future (after condition, cycle, else branch etc.)
 */
typedef struct labelstack
{
  LabelItem first;   /**< Pointer to first item. */
} LabelStack;

/**
 * @brief   Pushes new label.
 *
 * This function will create (dynamically) new item and pushes
 * it onto LabelStack.
 * @param lbl     Name of the label.
 * @returns True, if success. False otherwise.
 */
inline bool PushLabel(const char * lbl);

/**
 * @brief   Generates and returns new label.
 *
 * This function will generate new label name, pushes it
 * onto stack and returns it.
 * @returns Name of the label, or error.
 */
inline const char * GenerateLabel();

/**
 * @brief   Returns top label.
 *
 * This function will return the most top label on LabelStack.
 * @returns Label, NULL if error.
 */
inline const char * LookUpLabel();

/**
 * @brief   Removes top label.
 *
 * This function will remove the most top label on Label Stack.
 */
inline void RemoveLabel();

/**
 * @brief   Returns and removes top label.
 *
 * This function will remove and return the most top label on Label Stack.
 * @returns Label, NULL if error.
 */
inline const char * PopLabel();

/**
 * @brief   Clears label stack
 *
 * This function will remove all the labels in Label Stack.
 */
inline void ClearLabels();

/** @} */
/*---------------------------------------------------------*/
/** @addtogroup Private_generators.
 * Private generator functions.
 * @{
 */

/**
 * @brief   Label name generator.
 *
 * This function generates the name of the variable for IFJcode17.
 * Do not save the given string. It is freed after next call of the function.
 * @param p     Variable to generate.
 * @returns Name
 */
char * GenerateName(Phrasem p);
/**
 * @brief   Clears generated name.
 */
void ClearGeneratedName();

char * GenerateType(Phrasem p);

char * GenerateTmpVariable();
void ClearGeneratedTmpVariable();

/** @} */
/*---------------------------------------------------------*/


/*----------- DATA ------------*/
static GeneratorStack mStack;
static LabelStack mLabels;
/*-----------------------------*/

void InitGenerator()
{
  #ifdef GENERATOR_DEBUG
    debug("Init generator.");
  #endif

  // header
  out("\n"
      "# Generated code\n"
      "# IFJ\n"
      "# xbenes49 xbolsh00 xpolan09\n"
      "# 2017\n");


  out(".IFJcode17");
  out("CREATEFRAME");
  out("PUSHFRAME");
  out("DEFVAR LF@*tmp");
  out("DEFVAR LF@*foo");
  out("DEFVAR LF@*bar");
  out("JUMP $main");
  out("");
}


/*------------------------------ CODE GENERATORS -----------------------------*/
void GenerateLogic(Phrasem p)
{
  #ifdef GENERATOR_DEBUG
    debug("Generating logic.");
  #endif

  const char * aftercond = GenerateLabel();

  if(isOperator(p, "="))
  {
    // =
    out("JUMPIFNEQS %s", aftercond);
  }
  else if(isOperator(p, "<>"))
  {
    // <>
    out("JUMPIFEQS %s", aftercond);
  }
  else if(isOperator(p, ">"))
  {
    // >
    out("GTS");
    out("PUSHS bool@true");
    out("JUMPIFNEQS %s", aftercond);
  }

  else if(isOperator(p, "<"))
  {
    // <
    out("LTS");
    out("PUSHS bool@true");
    out("JUMPIFNEQS %s", aftercond);
  }
  else if(isOperator(p, ">="))
  {
    // >=
    out("LTS");
    out("PUSHS bool@true");
    out("JUMPIFEQS %s", aftercond);

  }
  else
  {
    // <=
    out("GTS");
    out("PUSHS bool@true");
    out("JUMIFEQS %s", aftercond);
  }

}

void GenerateFunctionCall(Phrasem p)
{
  out("CALL %s", p->d.str);
  out("POPFRAME");
  out("PUSHS TF@*ret");
}

void GenerateAssignment(Phrasem p)
{
  #ifdef GENERATOR_DEBUG
    debug("Generating assignment.");
  #endif

  out("POPS %s", GenerateName(p));
}
void GenerateVariableDeclaration(Phrasem p)
{
  #ifdef GENERATOR_DEBUG
    debug("Generating variable declaration.");
  #endif
  out("DEFVAR %s", GenerateName(p));
}

void GeneratePrint()
{
  #ifdef GENERATOR_DEBUG
    debug("Generating print.");
  #endif

  out("POPS LF@*tmp");
  out("WRITE LF@*tmp");
}

void GenerateRead(Phrasem p)
{
  #ifdef GENERATOR_DEBUG
    debug("Generating read.");
  #endif

  // this will go from symbol table
  out("WRITE string@?\\032");
  out("READ %s %s", GenerateName(p), GenerateType(p));
}

void GenerateReturn()
{
  #ifdef GENERATOR_DEBUG
    debug("Generating return.");
  #endif
}

void GenerateFunctionHeader(Phrasem p)
{
  #ifdef GENERATOR_DEBUG
    debug("Generating function.");
  #endif

  // function
  out("\n# function %s", p->d.str);
  out("LABEL %s", p->d.str);
  out("PUSHFRAME");
  out("DEFVAR LF@*tmp");
  out("DEFVAR LF@*foo");
  out("DEFVAR LF@*bar");
  out("DEFVAR LF@*ret");
}

void GenerateArgument();

void GenerateLength()
{
  #ifdef GENERATOR_DEBUG
    debug("Generating length.");
  #endif

  const char * str = GenerateTmpVariable();
  out("DEFVAR LF@%s", str);
  out("POPS LF@%s", str);
  out("STRLEN LF@*tmp LF@%s", str);
  out("PUSHS LF@*tmp", str);

}

void GenerateInt2Str()
{
  #ifdef GENERATOR_DEBUG
    debug("Generating int2str.");
  #endif

  out("POPS LF@*tmp");
  out("INT2CHAR LF@*foo LF@*tmp");
  out("PUSHS LF@*foo");
}

void GenerateAsc()
{
  #ifdef GENERATOR_DEBUG
    debug("Generating asc.");
  #endif
  out("# asc()");
  out("\tPOPS LF@*tmp"); // index
  out("\tPOPS LF@*foo"); // string
  out("\tSTRLEN LF@*bar LF@*foo"); // size of
  out("\tPUSHS LF@*tmp");
  out("\tPUSHS LF@*bar");
  out("\tLTS");
  out("\tPUSHS bool@true");
  out("\tJUMPIFNEQS $zero");

  out("\tPUSHS LF@*tmp");
  out("\tPUSHS int@0");
  out("\tGTS");
  out("\tPUSHS bool@true");
  out("\tJUMPIFNEQS $zero");

  out("\t\tPUSHS LF@*foo");
  out("\t\tPUSHS LF@*tmp");
  out("\t\tSTRI2INTS");
  const char * lbl = GenerateLabel();
  out("\t\tJUMP %s", lbl);

  out("\tLABEL $zero");
  out("\tPUSHS int@0");

  out("LABEL %s", lbl);
  free((void *)lbl);
}

void GenerateSubStr()
{
  #ifdef GENERATOR_DEBUG
    debug("Generating substr.");
  #endif

  out("CREATEFRAME");
  const char * result = strdup(GenerateTmpVariable());
  out("DEFVAR TF@%s", result);
  out("MOVE TF@%s string@", result);
  out("POPS LF@*foo"); // foo - n
  out("POPS LF@*bar"); // bar - i
  out("POPS LF@*tmp"); // tmp - str
  const char * len = strdup(GenerateTmpVariable());
  out("DEFVAR TF@%s", len);
  out("STRLEN TF@%s LF@*tmp", len);

  const char * nempty = GenerateLabel();
  out("PUSHS LF@*tmp");
  out("PUSHS string@");
  out("JUMPIFNEQS %s", nempty);
    const char * retempty = GenerateLabel();
    out("LABEL %s", retempty);
    const char * done = GenerateLabel();
    out("JUMP %s", done);

  out("LABEL %s", nempty);

  out("PUSHS LF@*bar");
  out("PUSHS int@1");
  out("LTS");
  out("PUSHS bool@true");
  out("JUMPIFEQS %s", retempty);

  out("PUSHS LF@*foo");
  out("PUSHS int@0");
  out("LTS");
  out("PUSHS bool@true");
  const char * retall = GenerateLabel();
  out("JUMPIFEQS %s", retall);

  const char * nall = GenerateLabel();
  out("JUMP %s", nall);

  out("LABEL %s", retall);
    // n = len - 1 + 1

  out("LABEL %s", nall);

  const char * pom = GenerateTmpVariable();
  out("DEFVAR TF@%s", pom);
  out("SUB LF@*bar LF@*bar int@1");
  const char *newchar = GenerateLabel();
  out("LABEL %s", newchar);
  out("GETCHAR TF@%s LF@*tmp LF@*bar", pom);
  out("CONCAT TF@%s TF@%s TF@%s", result, result, pom);
  out("ADD LF@*bar LF@*bar int@1");
  out("PUSHS LF@*bar");
  out("PUSHS TF@%s", len);
  out("LTS");
  out("PUSHS bool@true");
  out("JUMPIFEQS %s", newchar);

  out("LABEL %s", done);

  out("PUSHS TF@%s", result);

  free((void *)result);
  free((void *)len);
  free((void *)nempty);
  free((void *)retempty);
  free((void *)done);
  free((void *)retall);
  free((void *)nall);
  //free(pom);
  free((void *)newchar);

}

void GenerateAritm(Stack s)
{
  #ifdef GENERATOR_DEBUG
    debug("Generating arithmetics.");
  #endif

  Phrasem p;
  while((p = PopFromStack(s)) != NULL)
  {

    // operand
    if((p->table == TokenType_Constant)
    || (p->table == TokenType_Variable))
    {
      out("PUSHS %s", GenerateName(p) );
    }

    // operator
    else if(p->table == TokenType_Operator)
    {
      if (isOperator(p, "+")) {
        out("ADDS");
      }

      else if (isOperator(p, "-")) {
        out("SUBS");
      }

      else if (isOperator(p, "*")) {
        out("MULS");
      }

      else if (isOperator(p, "/")) {
        out("DIVS");
      }
      else if (isOperator(p, "\\")) {
        out("DIVS");
        out("FLOAT2R2EINTS");
      }
    }

    // typecast
    else if(isTypeCast(p))
    {
      GenerateTypeCast(p->table);
    }
  }

}

void GenerateStringArithm(Stack s)
{
  #ifdef GENERATOR_DEBUG
    debug("Generating string arithmetics.");
  #endif

  const char * tmp = GenerateTmpVariable();
  out("DEFVAR LF@%s", tmp);
  out("MOVE LF@%s string@", tmp);

  Phrasem p;
  while((p = PopFromStack(s)) != NULL)
  {
    if(isOperator(p, "+")) continue;

    out("CONCAT LF@%s LF@%s %s", tmp, tmp, GenerateName(p));
  }

  out("PUSHS LF@%s", tmp);

}

/*------------------------------- RECIEVERS ----------------------------------*/

bool Send(Stack s)
{
  #ifdef GENERATOR_DEBUG
    debug("Send to Generator");
    PrintStack(s);
    PrintGStateStack();
  #endif

  // incoming stack process
  GState top = PopGState();
  if(top == GState_Expression) GenerateAritm(s);
  else if(top == GState_StringExpression) GenerateStringArithm(s);
  ClearStack(s);

  // underneath
  GState below = LookUpGState();
  if( below == GState_Print )
  {
    GeneratePrint();
  }
  else if( below == GState_Return )
  {
    GenerateReturn();
  }
  else if(below == GState_Length)
  {
    GenerateLength();
  }
  else if(below == GState_Empty)
  {
    PopGState();
  }
  else if(below == GState_SubStr)
  {
    GenerateSubStr();
  }

  #ifdef GENERATOR_DEBUG
    PrintGStateStack();
  #endif
  return true;
}

bool HandlePhrasem(Phrasem p)
{
  #ifdef GENERATOR_DEBUG
    debug("Handeling phrasem.");
    PrintPhrasem(p);
  #endif

  // look to state stack
  GState top = LookUpGState();
  // logic
  if(top == GState_Logic)
  {
    GenerateLogic(p);
  }
  // variable declaration
  else if(top == GState_VariableDeclaration)
  {
    GenerateVariableDeclaration(p);
  }
  // read
  else if (top == GState_Input)
  {
    GenerateRead(p);
  }
  // assignment
  else if(top == GState_Assignment)
  {
    GenerateAssignment(p);
  }
  else if(top == GState_FunctionHeader)
  {
    GenerateFunctionHeader(p);
  }
  else if(top == GState_FunctionCall)
  {
    GenerateFunctionCall(p);
  }

  PopGState();
  #ifdef GENERATOR_DEBUG
    PrintGStateStack();
  #endif
  return true;
}

/*---------- DATA -----------*/
static char param_name[4]; // there can't be more, than 999 parameters
/*---------------------------*/
void AssignArgument(Phrasem p, unsigned ord)
{
    sprintf(param_name, "*%u", ord);
    out("MOVE LF@%s LF@%s", p->d.str, param_name);
}

void GenerateBuiltIn()
{

  GState func = PopGState();
  if(func == GState_Asc)
  {
    GenerateAsc();
  }
  else if(func == GState_Int2Str)
  {
    GenerateInt2Str();
  }
  else if(func == GState_SubStr)
  {
    GenerateSubStr();
  }
  else if(func == GState_Length)
  {

    GenerateLength();
  }
}


/*------------------------------ INDICATORS ----------------------------------*/

void G_Asc()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate asc (ordinal of string from position).");
  #endif

  PushGState(GState_Asc);
}

void G_Assignment()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate assignment.");
  #endif

  PushGState(GState_Assignment);

}

void G_ArgumentAssignment(unsigned ord)
{
  #ifdef GENERATOR_DEBUG
    debug("Generate argument assignment.");
  #endif

  PushGState(GState_Argument);

  sprintf(param_name, "*%u", ord);
  out("DEFVAR TF@%s", param_name);

}
void GenerateArgument()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate argument.");
  #endif

  out("POPS TF@%s", param_name);
  RemoveGState();
}

void G_Condition()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate condition.");
  #endif

  PushGState(GState_Condition);

}

void G_Cycle()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate cycle.");
  #endif

  PushGState(GState_Cycle);
  out("LABEL %s", GenerateLabel());

}

void G_Else()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate else.");
  #endif

  const char * els = PopLabel();
  out("JUMP %s", GenerateLabel());
  out("LABEL %s", els);
  free((void *)els);
}

void G_EndBlock()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate end.");
  #endif

  GState up = PopGState();
  if( up == GState_Condition )
  {
    const char * aftercond = PopLabel();
    out("LABEL %s", aftercond);
    free((void *)aftercond);
  }
  else if( up == GState_Cycle )
  {
    const char * aftercycle = PopLabel();
    const char * tocycle = PopLabel();

    out("JUMP %s", tocycle);
    out("LABEL %s", aftercycle);
    free((void *)aftercycle);
    free((void *)tocycle);
  }
  else if( up == GState_Return )
  {
    out("POPS LF@*ret");
    out("CLEARS");
    out("RETURN");
  }
  else if( up == GState_Function )
  {
    out("");
  }

  #ifdef GENERATOR_DEBUG
    PrintGStateStack();
  #endif
}

void G_Empty()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate empty.");
  #endif

  PushGState(GState_Empty);
}

void G_Expression()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate expression.");
  #endif
  PushGState(GState_Expression);
}

void G_Expression2StringExpression()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate string expression.");
  #endif

  if(LookUpGState() == GState_Expression)
  {
    PopGState();
    PushGState(GState_StringExpression);
  }
}

void G_FinalJump()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate final jump.");
  #endif

  out("JUMP $end");
}

void G_FinalLabel()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate final label.");
  #endif

  out("LABEL $end");
}

void G_Function()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate function.");
  #endif

  PushGState(GState_Function);
  PushGState(GState_FunctionHeader);
}

void G_FunctionAssignment(Phrasem p)
{
  #ifdef GENERATOR_DEBUG
    debug("Generate function assignment.");
  #endif

  out("POPS LF@%s", p->d.str);
}

void G_FunctionCall()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate function call.");
  #endif

  PushGState(GState_FunctionCall);
  out("CREATEFRAME");

}

void G_Input()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate input.");
  #endif
  PushGState(GState_Input);

}

void G_Int2Str()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate int to string conversion.");
  #endif

  PushGState(GState_Int2Str);
}

void G_Length()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate string length.");
  #endif

  PushGState(GState_Length);
}

void G_Logic()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate logic.");
  #endif
  PushGState(GState_Logic);

}

void G_Print()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate print.");
  #endif

  PushGState(GState_Print);
}

void G_Return()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate return.");
  #endif

  PushGState(GState_Return);
}

void G_Scope()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate scope.");
  #endif

  out("LABEL $main");
}

void G_SubStr()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate substr.");
  #endif

  PushGState(GState_SubStr);
}

void G_VariableDeclaration()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate variable declaration.");
  #endif

  PushGState(GState_VariableDeclaration);
}

/*----------------------------- PRIVATE GENERATORS ---------------------------*/

void GenerateTypeCast(TokenType tc)
{
  #ifdef GENERATOR_DEBUG
    debug("Generate typecast.");
  #endif

  switch(tc)
  {
    case TypeCast_Int2Double:
      out("INT2FLOATS");
      break;
    case TypeCast_Double2Int:
      out("FLOAT2R2EINTS");
      break;
    default:
      break;
  }
}

/*------------- DATA ---------------*/
static char * namebuff = NULL;
/*----------------------------------*/

char * GenerateName(Phrasem p)
{
  if(p == NULL) return NULL;

  if(namebuff != NULL) free(namebuff);

  DataType dt;
  int maxlen;
  switch(p->table)
  {
    case TokenType_Constant:
      dt = findConstType(p->d.index);
      switch(dt)
      {
        case DataType_Integer:
          maxlen = 1/*sign*/ + (log10(getIntConstValue(p->d.index)) + 1) /*digits*/;
          if(getIntConstValue(p->d.index) <= 0) maxlen = 1;
          namebuff = malloc(sizeof(char) * (4/*int@*/ + maxlen + 1 /*end zero*/));
          if(namebuff == NULL) return NULL;
          sprintf(namebuff, "int@%d", getIntConstValue(p->d.index));
          return namebuff;

        case DataType_Double:
          maxlen = 1 /*sign*/ + 53 /*mantissa - IEEE754*/ + 1 /*decimal point*/;
          namebuff = malloc(sizeof(char) * (6 /*float@*/ + maxlen + 1 /*end zero*/));
          if(namebuff == NULL) return NULL;
          sprintf(namebuff, "float@%g", getDoubleConstValue(p->d.index));
          return namebuff;

        case DataType_String:
          maxlen = strlen(getStringConstValue(p->d.index));
          namebuff = malloc(sizeof(char) * (7/*string@*/ + maxlen + 1 /*end zero*/));
          if(namebuff == NULL) return NULL;
          sprintf(namebuff, "string@%s", getStringConstValue(p->d.index));
          return namebuff;

        default:
          return "TODO";
      }

    case TokenType_Variable:
      dt = findVariableType(Config_getFunction(), p->d.str);

      namebuff = malloc(sizeof(char) * (4/*LF@*/ + strlen(p->d.str) + 1 /*end zero*/));
      if(namebuff == NULL) return NULL;
      sprintf(namebuff, "LF@%s", p->d.str);
      return namebuff;

    default:
      // TODO
      PrintPhrasem(p);
      return "TODO";
  }
}

void ClearGeneratedName()
{
  if(namebuff != NULL)
  {
    free(namebuff);
    namebuff = NULL;
  }
}

char * GenerateType(Phrasem p)
{
  DataType dt;
  switch(p->table)
  {
    case TokenType_Variable:
      dt = findVariableType(Config_getFunction(), p->d.str);
      switch(dt)
      {
        case DataType_Integer: return "int";
        case DataType_Double: return "float";
        case DataType_String: return "string";
        default: return NULL;
      }
    default: return NULL;
  }
}



static char varname[] = "*aaaaaa";
void incrementVarname(unsigned it)
{

  if(it >= strlen(varname))
  {
    err("empty");
    strcpy(varname, "*aaaaaa");
  }

  // overflow

  if(varname[it] == 'z') varname[it] = 'A';
  else if(varname[it] == 'Z' ) varname[it] = '0';
  else if(varname[it] == '9' )
  {
    varname[it++] = 'a';
    incrementVarname(it);
  }

  // increment
  else
  {
    varname[it]++;
  }

}
char * GenerateTmpVariable()
{
  incrementVarname(1);
  return varname;
}






/*-------------------- STATE STACK ---------------------------*/

bool PushGState(GState newstate)
{
  #ifdef GSTATE_STACK_DEBUG
    debug("Push %s.", GStateToStr(newstate));
  #endif

  // alloc
  StateItem newitem = malloc(sizeof(struct state_item));
  if(newitem == NULL) return false;

  // filling up
  newitem->state = newstate;
  newitem->next = mStack.first;
  // moving head
  mStack.first = newitem;

  return true;
}

GState LookUpGState()
{
  // look for item
  StateItem readitem = mStack.first;

  // empty
  if(readitem == NULL) return GState_Empty;
  // not empty
  else return readitem->state;
}

void RemoveGState()
{
  // look for item
  StateItem olditem = mStack.first;

  // empty
  if(olditem == NULL) return;
  // not empty
  #ifdef GSTATE_STACK_DEBUG
    debug("Remove %s.", GStateToStr(olditem->state));
  #endif
  mStack.first = mStack.first->next;
  free(olditem);
}

GState PopGState()
{
  // look up
  GState readitem = LookUpGState();
  // remove
  RemoveGState();

  return readitem;
}

void ClearGStates()
{
  #ifdef GSTATE_STACK_DEBUG
    debug("Clear GState stack.");
  #endif
  // clear
  while(PopGState() != GState_Empty) { }
}

void PrintGStateStack()
{
  StateItem it = mStack.first;
  if(it == NULL) debug("empty stack");
  else
  {
    debug("\n---GState Stack---");
    do {
      debug("| %s", GStateToStr(it->state));
      it = it->next;
    } while(it != NULL);
    debug("------------------\n");
  }
}

/*---------------- LABEL STACK ------------------*/

bool PushLabel(const char * lbl)
{
  // alloc
  LabelItem newitem = malloc(sizeof(struct label_item));
  if(newitem == NULL) return false;

  // fill up
  newitem->lbl = lbl;
  newitem->next = mLabels.first;
  // moving head
  mLabels.first = newitem;

  return true;
}

static char form[] = "$aaaaaa";
void incrementForm(unsigned it)
{

  if(it >= strlen(form))
  {
    err("empty");
    strcpy(form, "$aaaaaa");
  }

  // overflow

  if(form[it] == 'z') form[it] = 'A';
  else if(form[it] == 'Z' ) form[it] = '0';
  else if(form[it] == '9' )
  {
    form[it++] = 'a';
    incrementForm(it);
  }

  // increment
  else
  {
    form[it]++;
  }

}
const char * GenerateLabel()
{
  char * msg = malloc(sizeof(char)*8);
  strcpy(msg, form);
  PushLabel(msg);
  incrementForm(1);
  return msg;
}

const char * LookUpLabel()
{
  // look up
  LabelItem readitem = mLabels.first;

  // empty
  if(readitem == NULL) return NULL;
  // not empty
  return readitem->lbl;
}

void RemoveLabel()
{
  // look up
  LabelItem olditem = mLabels.first;

  // empty
  if(olditem == NULL) return;
  // not empty
  mLabels.first = olditem->next;

  // remove label first
  free((void*)olditem->lbl);
  free(olditem);

}

const char * PopLabel()
{
  // olditem
  LabelItem olditem = mLabels.first;
  // empty
  if(olditem == NULL) return NULL;

  // not empty
  mLabels.first = olditem->next;

  // str
  const char * lbl = olditem->lbl;

  // remove label first
  free(olditem);

  return lbl;
}

void ClearLabels()
{
  // clear
  const char * lbl;
  while( (lbl = PopLabel()) != NULL) { free((void *)lbl);  }
}


void ClearGenerator()
{
  ClearGStates();
  ClearLabels();
  ClearGeneratedName();
}
