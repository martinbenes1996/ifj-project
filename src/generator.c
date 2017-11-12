
#include <stdlib.h>
#include <string.h>

#include "functions.h"
#include "generator.h"
#include "io.h"
#include "stack.h"
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
  GState_Condition,
  GState_Cycle,
  GState_Assignment,
  GState_FunctionCall,
  GState_Input,
  GState_Print,
  GState_Expression,
  GState_Logic,
  GState_VariableDeclaration,
  GState_Empty
} GState;

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
 * This function clears whole GeneratorStack
 * @returns GState on the top, or GState_Empty, if empty.
 */
inline void ClearGStates();

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

/*----------- DATA ------------*/
static GeneratorStack mStack;
static LabelStack mLabels;
/*-----------------------------*/

char * form = "blabla";
const char * GenerateUniqueName()
{
  char * msg = malloc(sizeof(char)*10);
  strcpy(msg, form);
  return msg;
}

static Phrasem mlogic;
void GenerateCondition()
{
  if(isOperator(mlogic, "="))
  {
    // =
    const char * aftercond = GenerateLabel();
    out("JUMIFEQS %s", aftercond);
  }
  else if(isOperator(mlogic, "<>"))
  {
    // <>
    const char * aftercond = GenerateLabel();
    out("JUMIFNEQS %s", aftercond);
  }
  else if(isOperator(mlogic, ">"))
  {
    // <
  }

  else if(isOperator(mlogic, "<"))
  {
    // >
  }
  else if(isOperator(mlogic, ">="))
  {
    // <=
  }
  else
  {
    // >=
  }

  free(mlogic);
  mlogic = NULL;
}

static const char * mtarget;
void GenerateAssignment()
{
  out("POPS %s", mtarget);
  mtarget = NULL;
}
void GenerateVariableDeclaration()
{
  out("DEFVAR %s", mtarget);
  // do not free, assignment will follow
}

void GeneratePrint()
{
  const char *tmp = "var";
  out("DEFVAR %s", tmp);
  out("POPS %s", tmp);
  out("WRITE %s", tmp);
  // do not free, assignment will follow
}

void GenerateRead(Phrasem p) {
  // this will go from symbol table
  out("READ %s %s", p->d.str, "integer");
}

void GenerateAritm(Stack s)
{
  const char * result = GenerateUniqueName();
  const char * tmp = GenerateUniqueName();
  out("VARDEF %s", result);
  out("VARDEF %s", tmp);

  // first
  Phrasem p = PopFromStack(s);
  out("MOVE %s %s", result, p->d.str);
  free(p);


  while(1)
  {

    // second
    Phrasem q = PopFromStack(s);
    if(q == NULL) break;
    out("MOVE %s %s", tmp, q->d.str);
    free(q);

    // operator
    Phrasem operator = PopFromStack(s);
    if (isOperator(operator, "+")) {
      out("ADD %s %s %s", result, result, tmp);
    }

    else if (isOperator(operator, "-")) {
      out("SUB %s %s %s", result, result, tmp);
    }

    else if (isOperator(operator, "*")) {
      out("MUL %s %s %s", result, result, tmp);
    }

    else if (isOperator(operator, "/")) {
      out("DIV %s %s %s", result, result, tmp);
    }
  }

  out("PUSHS %s", result);

}

bool Send(Stack s)
{
  #ifdef GENERATOR_DEBUG
    debug("Send to Generator");
  #endif

  GenerateAritm(s);

  // here will be stack process
  // ...
  // result will be on the data stack of result program (not in the variable)


  PopGState();
  // underneath
  GState below = LookUpGState();
  if( below == GState_Condition )
  {
    GenerateCondition();
  }
  else if( below == GState_Assignment )
  {
    GenerateAssignment();
  }
  else if( below == GState_Print )
  {
    GeneratePrint();
  }
  return true;
}

bool HandlePhrasem(Phrasem p)
{
  if(LookUpGState() == GState_Logic)
  {
    mlogic = p;
  }
  if(LookUpGState() == GState_VariableDeclaration)
  {
    mtarget = p->d.str;
    free(p);
    GenerateVariableDeclaration();
  }
  if (LookUpGState() == GState_Input) {
    GenerateRead(p);
  }

  PopGState();
  return true;
}

void G_FunctionCall()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate function call.");
  #endif

  PushGState(GState_FunctionCall);

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


}

void G_Assignment()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate assignment.");
  #endif


}

void G_Print()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate print.");
  #endif


}

void G_Input()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate input.");
  #endif
  PushGState(GState_Input);

}

void G_Logic()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate logic.");
  #endif
  PushGState(GState_Logic);

}

void G_Expression()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate expression.");
  #endif
  PushGState(GState_Expression);
}

void G_VariableDeclaration()
{
  #ifdef GENERATOR_DEBUG
    debug("Generate variable declaration.");
  #endif

  PushGState(GState_VariableDeclaration);
}



















/*-------------------- STATE STACK ---------------------------*/
bool PushGState(GState newstate)
{
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
  // clear
  while(PopGState() != GState_Empty) { }
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

const char * GenerateLabel()
{
  // TODO
  // generate
  //...
  // neco = ??? funkce() ???
  // dynamicka alokace

  // if(!PushLabel(neco)) return NULL
  return "var";
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
  // ...
  free(olditem);

}

const char * PopLabel()
{
  // look up
  const char * lbl = LookUpLabel();
  // remove
  RemoveLabel();

  return lbl;
}

void ClearLabels()
{
  // clear
  while(PopLabel() != NULL) { }
}
