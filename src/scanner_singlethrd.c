/**
 * @file scanner
 * @authors xbenes49 xbolsh00 xkrato47 xpolan09
 * @date 2nd october 2017
 * @brief Lexical Scanner module.
 *
 * This module implements Lexical Scanner.
 */

#ifndef MULTITHREAD

#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"
#include "err.h"
#include "functions.h"
#include "io.h"
#include "queue.h"
#include "scanner_singlethrd.h"
#include "stack.h"
#include "tables.h"
#include "types.h"

bool done = false;
int line = 1;

void EndScanner(const char * msg, ErrorType errtype)
{
  if(msg != NULL)
  {
    #ifdef SCANNER_DEBUG
      debug(msg);
    #endif
    setErrorType(errtype);
    setErrorMessage(msg);
  }
  else
  {
    #ifdef SCANNER_DEBUG
      debug("End Scanner.");
    #endif
  }
}

/**
 * @brief  Error raiser.
 *
 * This macro writes debug info to err function. It also calls EndScanner
 * and returns false from a function, where it is used.
 * @param msg         Message to write.
 * @param errtype     Type of error.
 */
#define RaiseError(msg, errtype)                                \
  do {                                                          \
    err("%s: %s: l.%d: %s", __FILE__, __func__, __LINE__, msg); \
    EndScanner(msg, errtype);                                   \
    char * p = GetBuffer();                                     \
    if (p != NULL) free(p);                                     \
    return NULL;                                                \
  } while(0)


/**
 * @brief  Alloc error raiser.
 */
#define RaiseAllocError() RaiseError("bad allocation", ErrorType_Internal);

/**
 * @brief SAfe buffer saver.
 */
#define SaveToBuffer(c) if(!AddToBuffer(c)) { RaiseAllocError(); }


#define ALLOC_PHRASEM(id)                                               \
  Phrasem id = malloc(sizeof(struct phrasem_data));                     \
  if (id == NULL) RaiseError("allocation error", ErrorType_Internal)


/*------------------------------------------------------------------------*/


/*---- DATA ------*/
static Stack mem = NULL;
static bool meminit = false;
/*----------------*/


/**
 * @brief function for comment
 *
 * This function recognizes if this a line or block comment
 * return true if everything is allright in other way - false.
 */
bool getComment() {
  #ifdef SCANNER_DEBUG
    debug("Get Comment");
  #endif

  int state = 0;
  int input;
  bool end = false;

  while (!end) {
    input = getByte();

    switch (state) {
      case 0:
        if ( input == '~') { state = 2; break; }
        else if (input == '\'') { state = 1; break; }
        else RaiseError("bad internal state", ErrorType_Internal);

      case 1:
        if ( input != '\n') { break; }
        else { end = true; break; }

      case 2:
        if (input != '/') { break; }
        else if (input == EOF) RaiseError("expected \'//\' ", ErrorType_Syntax);
        else { state = 3; break; }

      case 3:
        if (input != '/') { state = 2; break; }
        else if (input == EOF) RaiseError("expected \'//\' ", ErrorType_Syntax);
        else { end = true; break; }

      default:
        EndScanner("unknown state in comment", ErrorType_Syntax);
        end = true;
        break;
    }
  }
  return true;
}

/**
 * @brief the function can recognize if this is an operator and save it to buffer
 * return Phrasem if everything is alright in other way - NULL
 */
Phrasem getOperator() {
  #ifdef SCANNER_DEBUG
    debug("Get Operator");
  #endif

  int state = 0;
  int input;
  bool end = false;

  while(!end) {
    input = getByte();

    switch (state) {

      case 0:
        if ( input == '<' ) {
          SaveToBuffer(input);
          state = 1;
        }
        else if ( input == '>') {
          SaveToBuffer(input);
          state = 2;
        }

        else if (input == '=') {
          SaveToBuffer(input);
          end = true;
        }

        else if (input == '+') {
          SaveToBuffer(input);
          end = true;
        }

        else if (input == '-') {
          SaveToBuffer(input);
          end = true;
        }

        else if (input == '*') {
          SaveToBuffer(input);
          end = true;
        }

        else if (input == '\\') {
          SaveToBuffer(input);
          end = true;
        }

        else if (input == '(') {
          SaveToBuffer(input);
          end = true;
        }

        else if (input == ')') {
          SaveToBuffer(input);
          end = true;
        }

        else if (input == ',') {
          SaveToBuffer(input);
          end = true;
        }

        else if (input == ';') {
          SaveToBuffer(input);
          end = true;
        }

        else RaiseError("not a operator", ErrorType_Syntax);
        break;


      case 1:
        if ( input == '>') {
          if ( !AddToBuffer(input) ) RaiseError("parsing not possible", ErrorType_Syntax);
          end = true;
        }

        else if (input == '=') {
          if ( !AddToBuffer(input) ) RaiseError("parsing not possible", ErrorType_Syntax);
          end = true;
        }
        else {
          returnByte(input);
          end = true;
        }
        break;

      case 2:
        if ( input == '=') {
          if ( !AddToBuffer(input) ) RaiseError("parsing not possible", ErrorType_Syntax);
          end = true;
        }

        else {
          returnByte(input);
          end = true;
        }
        break;

      default:
        RaiseError("unknown state", ErrorType_Internal);

    }
  }

  char * p = GetBuffer();
  if(p == NULL) RaiseError("buffer allocation error", ErrorType_Internal);


  long x = getOperatorId(p);
  free(p);

  if ( x == -1) RaiseError("constant table error", ErrorType_Internal);

  ALLOC_PHRASEM(phr);
  phr->table = TokenType_Operator;
  phr->d.index = x;
  phr->line = line;

  #ifdef SCANNER_DEBUG
    PrintPhrasem(phr);
  #endif

  return phr;
}

/**
 * @brief The function can recognize if this is string and save it to buffer
 * return true if everything is ok in otherway -false
 */
Phrasem getString() {
  #ifdef SCANNER_DEBUG
    debug("Get String");
  #endif
  int state = 0;
  int input;
  int asciival = 0;
  bool end = false;

  while(!end) {
    input = getByte();
    switch (state) {

      case 0:
        if (input == '!') { state = 1; }
        else RaiseError("bad internal state", ErrorType_Internal);
        break;

      case 1:
        if (input == '"') { state = 2; }
        else RaiseError("bad internal state", ErrorType_Internal);
        break;

      case 2:
        if (input == '"') {

          char * p = GetBuffer();
          if(p == NULL) RaiseError("buffer allocation error", ErrorType_Internal);

          DataUnion du;
          du.svalue = p;

          int x = constInsert(DataType_String, du);
          if ( x == -1) RaiseError("constant table allocation error", ErrorType_Internal);

          ALLOC_PHRASEM(phr);
          phr->table = TokenType_Constant;
          phr->d.index = x;
          phr->line = line;

          #ifdef SCANNER_DEBUG
            PrintPhrasem(phr);
          #endif

          return phr;
        }
        else if (input == EOF)  RaiseError("string not ended", ErrorType_Syntax);
        else if (input != '\\') {
          SaveToBuffer(input);
        }
        else { state = 3; }
        break;

      case 3:
        // process \n
        if (input == 'n') {
          SaveToBuffer('\n');
          state = 2;
        }
        // process \t
        else if (input == 't') {
          SaveToBuffer('\t')
          state = 2;
        }
        // process /"
        else if (input == '"') {
          SaveToBuffer('"')
          state = 2;
          break;
        }
        // process \<backslash>
        else if (input == '\\') {
          SaveToBuffer('\\');
          state = 2;
        }
        // number (first of \xxx)
        else if (isdigit(input)) {
          asciival += 100*(input - '0');
          state = 4;
        }
        else RaiseError("bad internal state", ErrorType_Syntax);

        break;
      // second digit of \xxx
      case 4:
        if(isdigit(input))
        {
          asciival += 10*(input-'0');
          state = 5;
        }
        else RaiseError("not valid escape sequence", ErrorType_Syntax);

        break;
      // third digit
      case 5:
        if(isdigit(input))
        {
          asciival += input-'0';
          if((asciival > 255) && (asciival < 1)) RaiseError("not valid escape sequence", ErrorType_Syntax);
          SaveToBuffer(asciival);
          state = 2;
          break;
        }
        else RaiseError("not valid escape sequence", ErrorType_Syntax);

      default:
        RaiseError("bad internal state", ErrorType_Syntax);
    }
  }
  return NULL;
}


Phrasem getIdentifier(){
  #ifdef SCANNER_DEBUG
    debug("Get Identifier");
  #endif

	int state = 0;
 	int input;

  do {
  	input = getByte();

  	switch (state) {
      // first letter
      case 0:
	  		if ( (input == '_')
          || ((input >= 'A') && (input <= 'Z'))
          || ((input >= 'a') && (input <= 'z')) )
        {
          SaveToBuffer(input);
          state = 1;
          break;
        }

	      else RaiseError("bad internal state", ErrorType_Internal);

      // other letters
      case 1:

        if (((input >= 'A') && (input <= 'Z'))
              || ((input >= 'a') && (input <= 'z'))
              || ((input >= '0') && (input <= '9'))
              || (input == '_'))
        {
          SaveToBuffer(input);
          break;
        }
			  else {
          returnByte(input);

          char * p = GetBuffer();
          if(p == NULL) RaiseError("buffer allocation error", ErrorType_Internal);

          // keyword parse
          int id = isKeyword(p);
          if ( id != -1) {
            free(p);

            ALLOC_PHRASEM(phr);
            phr->table = TokenType_Keyword;
            phr->d.index = id;
            phr->line = line;

            // DEBUG
            #ifdef SCANNER_DEBUG
              PrintPhrasem(phr);
            #endif

            return phr;
          }

          // identifier parse
          else {
            ALLOC_PHRASEM(phr);
            phr->table = TokenType_Symbol;
            phr->d.str = CutBuffer(p);
            phr->line = line;

             // DEBUG
            #ifdef SCANNER_DEBUG
              PrintPhrasem(phr);
            #endif

            return phr;
          }
          break;

        }

      // bad state
      default:
        RaiseError("parsing not possible", ErrorType_Syntax);
    }
  } while(1);
  return NULL;
}

Phrasem getNumber(){
  #ifdef SCANNER_DEBUG
    debug("Get Number");
  #endif

  int state = 0; // state of the machine
  // integer
  double result = 0; // result of integers
  // double
  int order = 1; // iterator of exponent
  double exponent = 0; // result of the exponent

  int input;
  while(1){
    input = getByte();
    switch (state) {

      case 0:
        if (isdigit(input)) {
          result = input-'0';
          state = 1;
          break;
        }
        else {
          RaiseError("bad internal state", ErrorType_Internal);
        }

      case 1:
        if (isdigit(input)) {
          result = result*10 + input-'0';
        }

        else if ( input == '.') {
          state = 2;
        }

        else if ((input == 'e') || (input == 'E')) {
          state = 4;
        }

        else {
          returnByte(input);
          state = 12;
        }

        break;

      case 2:
        if (isdigit(input)) {
          result = result + (input - '0')/pow(10,order);
          order++;
          state = 3;
        }

        else {
          RaiseError("digit expected", ErrorType_Syntax);
        }

        break;

      case 3:
        if (isdigit(input)) {
          result = result + (input - '0')/pow(10,order);
          order++;
        }

        else {
          returnByte(input);
          state = 11;
        }
        break;

      // right after E/e
      case 4:
        if (input == '+') {
          state = 5;
        }

        else if (isdigit(input))
        {
          returnByte(input);
          state = 5;
        }

        else if (input == '-') {
          state = 6;
        }

        else {
          RaiseError("digit expected", ErrorType_Syntax);
        }

        break;

      // positive exponent, first digit
      case 5:
        if (isdigit(input)) {
          exponent = exponent*10 + (input - '0');
          state = 9;
        }

        else {
          RaiseError("digit expected", ErrorType_Syntax);
        }
        break;
      // positive exponent, other digits
      case 9:
        if (isdigit(input)) {
          exponent = exponent*10 + (input - '0');
          break;
        }

        else {
          state = 7;
          returnByte(input);
          break;
        }

      // negative exponent, first digit
      case 6:
        if (isdigit(input)) {
          exponent = exponent*10 + (input - '0');
          state = 10;
        }

        else RaiseError("digit expected", ErrorType_Syntax);

        break;

      // negative exponent, other digits
      case 10:
        if (isdigit(input)) {
          exponent = exponent*10 + input;
        }

        else {
          state = 8;
          returnByte(input);
        }
        break;

      // double process XeY
      case 7:
        do {
          returnByte(input);
          result *= pow(2, exponent);

          DataUnion uni;
          uni.dvalue = result;

          int i = constInsert(DataType_Double, uni);

          if ( i == -1) RaiseError("constant table allocation error", ErrorType_Internal);

          ALLOC_PHRASEM(p);
          p->table = TokenType_Constant;
          p->d.index = i;
          p->line = line;

          #ifdef SCANNER_DEBUG
            PrintPhrasem(p);
          #endif

          return p;

        } while(0);

      // double process Xe-Y
      case 8:
        do {
          returnByte(input);

          result *= pow(2, -exponent);

          DataUnion uni;
          uni.dvalue = result;

          int i = constInsert(DataType_Double, uni);

          if ( i == -1) RaiseError("constant table allocation error", ErrorType_Internal);

          ALLOC_PHRASEM(p);
          p->table = TokenType_Constant;
          p->d.index = i;
          p->line = line;

          #ifdef SCANNER_DEBUG
            PrintPhrasem(p);
          #endif

          return p;

        } while(0);

      // double process X.Y
      case 11:
        do {
          returnByte(input);
          DataUnion uni;
          uni.dvalue = result;

          int i = constInsert(DataType_Double, uni);

          if ( i == -1) RaiseError("constant table allocation error", ErrorType_Internal);

          ALLOC_PHRASEM(p);
          p->table = TokenType_Constant;
          p->d.index = i;
          p->line = line;

          #ifdef SCANNER_DEBUG
            PrintPhrasem(p);
          #endif

          return p;

        } while(0);

      // integer process
      case 12:
        do {
          returnByte(input);
          DataUnion uni;
          uni.ivalue = (int)result;

          int i = constInsert(DataType_Integer, uni);

          if ( i == -1) RaiseError("constant table allocation error", ErrorType_Internal);

          ALLOC_PHRASEM(p);
          p->table = TokenType_Constant;
          p->d.index = i;
          p->line = line;

          #ifdef SCANNER_DEBUG
            PrintPhrasem(p);
          #endif

          return p;

        } while(0);

      // error state
      default:
        RaiseError("bad internal state", ErrorType_Internal);
    }
  }
  return NULL;
}



Phrasem RemoveFromQueue()
{
  // stack init (if first)
  if(!meminit) { mem = InitStack(); meminit = true; }

  #ifdef SCANNER_DEBUG
    debug("Get Phrasem.");
    PrintStack(mem);
  #endif

  Phrasem pom;
  if((pom = PopFromStack(mem)) != NULL)
  {
    return pom;
  }

  // reading
  int input;
  loadAnother:
  do {
    input = getByte();
  } while( (input == ' ') || (input == '\t') );

  // EOF
  if(input == EOF)
  {
    ALLOC_PHRASEM(phr);
    phr->table = TokenType_EOF;

    #ifdef SCANNER_DEBUG
      PrintPhrasem(phr);
    #endif

    return phr;
  }


  if(input == '\n')
  {
    line += 1;
    ALLOC_PHRASEM(phr);
    phr->table = TokenType_Separator;
    phr->d.str = NULL;
    #ifdef SCANNER_DEBUG
      PrintPhrasem(phr);
    #endif
    return phr;
  }

  // string
  else if (input == '!') {
    returnByte(input);
    return getString();
  }

  else if ( input == '\'' ) {
    returnByte(input);
    getComment();
    goto loadAnother;
  }

  else if (input == '/') {
    input = getByte();
    if (input == '/')
    {
      returnByte('~');
      getComment();
      goto loadAnother;
    }
    else {
      returnByte(input);

      // AddToQueue '/' as operator
      long x = getOperatorId("/");

      if ( x == -1) RaiseError("constant table allocation error", ErrorType_Internal);

      ALLOC_PHRASEM(phr);
      phr->table = TokenType_Operator;
      phr->d.index = x;
      phr->line = line;

      #ifdef SCANNER_DEBUG
        PrintPhrasem(phr);
      #endif

      return phr;
    }
  }


  else if ( (input == '<')
          ||(input == '>')
          ||(input == '=')
          ||(input == '+')
          ||(input == '-')
          ||(input == '*')
          ||(input == '\\')
          ||(input == '(')
          ||(input == ')')
          ||(input == ',')
          ||(input == ';')) {
    returnByte(input);
    return getOperator();
  }


  else if (((input >= 'A') && (input <= 'Z'))
        || ((input >= 'a') && (input <= 'z'))
        || (input == '_')) {
    returnByte(input);
    return getIdentifier();
  }


  else if ((input >= '0') && (input <= '9')) {
    returnByte(input);
    return getNumber();
  }

  else RaiseError("unknown symbol", ErrorType_Syntax);

}


bool ReturnToQueue(Phrasem p)
{
  bool ret = PushOntoStack(mem, p);
  #ifdef SCANNER_DEBUG
    debug("return to queue");
    PrintStack(mem);
  #endif
  return ret;
}



#endif // MULTITHREAD
