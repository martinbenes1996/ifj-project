
/**
 * @file scanner
 * @authors xbenes49 xbolsh00 xkrato47 xpolan09
 * @date 2nd october 2017
 * @brief Lexical Scanner module.
 *
 * This module implements Lexical Scanner.
 */

#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"
#include "err.h"
#include "io.h"
#include "queue.h"
#include "scanner.h"
#include "tables.h"
#include "types.h"


bool done = false;
int line = 1;

bool outer_error = false;
void AskScannerToEnd() { outer_error = true; }

static volatile bool isscanning = true;
bool ScannerIsScanning() { return isscanning; }

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
  isscanning = false;
  FinishConnectionToQueue();
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
    return false;                                               \
  } while(0)


#define ALLOC_PHRASEM(id)                                               \
  Phrasem id = malloc(sizeof(struct phrasem_data));                     \
  if (id == NULL) RaiseError("allocation error", ErrorType_Internal)



// extern bool AddToBuffer(char c);

/**
 * @brief function for comment
 *
 * This function recognizes if this a line or block comment
 * return true if everything is allright in other way - false.
 */


bool getComment() {
  int state = 0;
  int input;
  bool end = false;

  while (!end) {
    input = getByte();

    switch (state) {
      case 0:
        if ( input == '~') {state = 2; break;}
        else if (input == '\'') {state = 1; break;}
        else RaiseError("parsing not possible", ErrorType_Syntax);

      case 1:
        if ( input != '\n') {break;}
        else {end = true; break;}

      case 2:
        if (input != '/') {break;}
        else { state = 3; break;}

      case 3:
        if (input != '/') {state = 2; break;}
        else {end = true; break;}

      default:
        RaiseError("unknown state in comment", ErrorType_Syntax);



    }

  }

  return true;

}




/**
 * @brief the function can recognize if this is an operator and save it to buffer
 * return true if everything is allright in other way - false
 */


bool getOperator() {
  int state = 0;
  int input;
  bool end = false;

  while(!end) {
    input = getByte();

    switch (state) {

      case 0:
        if ( input == '<' ) {
          if ( !AddToBuffer(input) ) RaiseError("parsing not possible", ErrorType_Syntax);
          state = 1;
        }
        else if ( input == '>') {
          if ( !AddToBuffer(input) ) RaiseError("parsing not possible", ErrorType_Syntax);
          state = 2;
        }

        else if (input == '=') {
          if ( !AddToBuffer(input) ) RaiseError("parsing not possible", ErrorType_Syntax);
          end = true;
        }

        else if (input == '+') {
          if ( !AddToBuffer(input) ) RaiseError("parsing not possible", ErrorType_Syntax);
          end = true;
        }

        else if (input == '-') {
          if ( !AddToBuffer(input) ) RaiseError("parsing not possible", ErrorType_Syntax);
          end = true;
        }

        else if (input == '*') {
          if ( !AddToBuffer(input) ) RaiseError("parsing not possible", ErrorType_Syntax);
          end = true;
        }

        else if (input == '\\') {
          if ( !AddToBuffer(input) ) RaiseError("parsing not possible", ErrorType_Syntax);
          end = true;
        }

        else if (input == '(') {
          if ( !AddToBuffer(input) ) RaiseError("parsing not possible", ErrorType_Syntax);
          end = true;
        }

        else if (input == ')') {
          if ( !AddToBuffer(input) ) RaiseError("parsing not possible", ErrorType_Syntax);
          end = true;
        }

        else if (input == ',') {
          if ( !AddToBuffer(input) ) RaiseError("parsing not possible", ErrorType_Syntax);
          end = true;
        }
        
        else RaiseError("parsing not possible", ErrorType_Syntax);

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
        RaiseError("parsing not possible", ErrorType_Syntax);

    }


    char * p = GetBuffer();
    if(p == NULL) RaiseError("buffer allocation error", ErrorType_Internal);


    long x = getOperatorId(p);
    free(p);

    if ( x == -1) RaiseError("constant table allocation error", ErrorType_Internal);

    ALLOC_PHRASEM(phr);
    phr->table = TokenType_Constant;
    phr->d.index = x;
    phr->line = line;

    #ifdef SCANNER_DEBUG
      PrintPhrasem(phr);
    #endif

    if( !AddToQueue(phr) ) RaiseError ("queue allocation error", ErrorType_Internal);


  }
  return true;

}




/**
 * @brief The function can recognize if this is string and save it to buffer
 * return true if everything is ok in otherway -false
 */


bool getString() {
  int state = 0;
  int input;
  int asciival = 0;
  bool end = false;

  while(!end)
    input = getByte();
    switch (state) {

      case 0:
        if (input == '!') {state = 1; break;}
        else  RaiseError("parsing not possible", ErrorType_Syntax);

      case 1:
        if (input == '"') {state = 2; break;}
        else RaiseError("parsing not possible", ErrorType_Syntax);

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

          if( !AddToQueue(phr) ) RaiseError ("queue allocation error", ErrorType_Internal);

          end = true;
          break;
        }
        else if (input == EOF)  RaiseError("parsing not possible", ErrorType_Syntax);
        else if (input != '\\') {
          if( !AddToBuffer(input) ) RaiseError("parsing not possible", ErrorType_Syntax);
          break;
        }
        else { state = 3; break; }

      case 3:
        if (input == 'n') {
          if( !AddToBuffer('\n') ) RaiseError("parsing not possible", ErrorType_Syntax);

          state = 2; break;
        }

        else if (input == 't') {
         if ( !AddToBuffer('\t') ) RaiseError("parsing not possible", ErrorType_Syntax);
         state = 2;
         break;
        }

        else if (input == '"') {
          if( !AddToBuffer ('"') ) RaiseError("parsing not possible", ErrorType_Syntax);
          state = 2;
          break;
        }

        else if (input == '\\') {
          if( !AddToBuffer('\\') ) RaiseError("parsing not possible", ErrorType_Syntax);
          state = 2;
          break;
        }

        else if (isdigit(input)) { asciival += 100*(input - '0'); state = 4; break; }

        else RaiseError("parsing not possible", ErrorType_Syntax);

      case 4:
        if(isdigit(input)) { asciival += 10*(input-'0'); state = 5; break;}
        else RaiseError("parsing not possible", ErrorType_Syntax);

      case 5:
        if(isdigit(input))
        {
          asciival += input-'0';
          if(asciival > 255) RaiseError("parsing not possible", ErrorType_Syntax);
          if ( !AddToBuffer(asciival) ) RaiseError("parsing not possible", ErrorType_Syntax);
          state = 2;
          break;
        }
        else RaiseError("parsing not possible", ErrorType_Syntax);

      default:
        RaiseError("parsing not possible", ErrorType_Syntax);
  }

  return true;


}


bool getIdentifier(){
	int state = 0;
 	int input;
  bool end = false;

  do {
  	input = getByte();

  	switch (state) {
      // first letter
      case 0:
	  		if ( (input == '_')
          || ((input >= 'A') && (input <= 'Z'))
          || ((input >= 'a') && (input <= 'z')) )
        {
          if( !AddToBuffer(input) ) RaiseError("buffer allocation failed", ErrorType_Internal);
          state = 1;
          break;
        }

	      else RaiseError("parsing not possible", ErrorType_Syntax);

      // other letters
      case 1:

        if (((input >= 'A') && (input <= 'Z'))
              || ((input >= 'a') && (input <= 'z'))
              || ((input >= '0') && (input <= '9'))
              || (input == '_'))
        {
          if( !AddToBuffer(input) ) RaiseError("parsing not possible", ErrorType_Syntax);

          break;
        }

			  else {
          returnByte(input);

          char * p = GetBuffer();
          if(p == NULL) RaiseError("buffer allocation error", ErrorType_Internal);

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

            if( !AddToQueue(phr) ) RaiseError("queue allocation error", ErrorType_Internal);
          }

          else {


            ALLOC_PHRASEM(phr);
            phr->table = TokenType_Symbol;
            phr->d.str = p;
            phr->line = line;

             // DEBUG
            #ifdef SCANNER_DEBUG
              PrintPhrasem(phr);
            #endif

            if( !AddToQueue(phr) ) RaiseError ("queue allocation error", ErrorType_Internal);
          }

          end = true;
          break;
        }

      // bad state
      default:
        RaiseError("parsing not possible", ErrorType_Syntax);
    }
  } while(!end);

	return true;
}


bool getNumber(){
  int state = 0;
  int input;
  bool end = false;
  double result = 0;
  int order = 1;
  double resultE = 0;
  double res = 0;

  while(!end){
    input = getByte();
    switch (state) {

      case 0:
        if ((input >= '0') && (input <= '9')) {
          result = result*10 + input-'0';
          state = 1;
          break;
        }

        else {
          RaiseError("parsing not possible", ErrorType_Syntax);
        }

      case 1:
        if ((input >= '0') && (input <= '9')) {
          result = result*10 + input-'0';
          break;
        }

        else if ( input == '.') {
          state = 2;
          break;
        }

        else if ((input == 'e') || (input == 'E')) {
          state = 4;
          break;
        }

        else {
          returnByte(input);
          end = true;
          break;
        }

      case 2:
        if ((input >= '0') && (input <= '9')) {
          result = result + (input - '0')/pow(10,order);
          order++;
          state = 3;
          break;
        }

        else {
          returnByte(input);
          end = true;
          break;
        }

      case 3:
        if ((input >= '0') && (input <= '9')) {
          result = result + (input - '0')/pow(10,order);
          order++;
          break;
        }

        else {
          returnByte(input);
          end = true;
          break;
        }

      case 4:
        if (input == '+') {
          state = 5;
          break;
        }

        else if ((input >= '0') && (input <= '9'))
        {
          returnByte(input);
          state = 5;
          break;
        }

        else if (input == '-') {
          state = 6;
          break;
        }

        else {
          returnByte(input);
          end = true;
          break;
        }

      case 5:
        if ((input >= '0') && (input <= '9')) {
          resultE = resultE*10 + input;
          break;
        }

        else {
          state = 7;
          returnByte(input);
          break;
        }

      case 6:
        if ((input >= '0') && (input <= '9')) {
          resultE = resultE*10 + input;
          break;

        }

        else {
          state = 8;
          returnByte(input);
          break;
        }
//TODO
      case 7:
        res = result * pow(2, resultE);
            

        DataUnion dataU;
        dataU.dvalue = res;
        int z = constInsert(TokenType_Constant, dataU);

        if ( z == -1) RaiseError("constant table allocation error", ErrorType_Internal);

        ALLOC_PHRASEM(phr);
        phr->table = TokenType_Constant;
        phr->d.index = z;
        phr->line = line;

        #ifdef SCANNER_DEBUG
          PrintPhrasem(phr);
        #endif

        if( !AddToQueue(phr) ) RaiseError ("queue allocation error", ErrorType_Internal);
        

        end = true;
        break;

      case 8:
        res = result * pow(2, -resultE);

        DataUnion uni;
        uni.dvalue = res;
        int y = constInsert(TokenType_Constant, uni);

        if ( y == -1) RaiseError("constant table allocation error", ErrorType_Internal);

        ALLOC_PHRASEM(p);
        p->table = TokenType_Constant;
        p->d.index = y;
        p->line = line;

        #ifdef SCANNER_DEBUG
          PrintPhrasem(p);
        #endif

        if( !AddToQueue(p) ) RaiseError ("queue allocation error", ErrorType_Internal);

        end = true;
        break;
//TODO
      default:
        RaiseError("parsing not possible", ErrorType_Syntax);

    }
  }
  return true;
}


void *InitScanner(void * v /*not used*/)
{
  (void)v; /* to avoid compiler warning */

  #ifdef SCANNER_DEBUG
    debug("Init Scanner.");
  #endif

  int input;

  while(!done)
  {
    // outer error
    if(outer_error) { EndScanner("", ErrorType_Ok); return NULL; }

    // reading
    input = getByte();
    if(input == EOF)
    {
      done = true;
      continue;
    }

    if((input == ' ') || (input == '\t')) continue;

    Phrasem phr = malloc( sizeof(struct phrasem_data) );
    if( phr == NULL )
    {
      EndScanner("Scanner: InitScanner: couldn't allocate memory", ErrorType_Internal);
      return NULL;
    }

    // here will be lexical analysis ----------------------------------

    if (input == EOF) {
      ALLOC_PHRASEM(phr);
      phr->table = TokenType_EOF;
      #ifdef SCANNER_DEBUG
        PrintPhrasem(phr);
      #endif
      if( !AddToQueue(phr) ) RaiseError("queue error", ErrorType_Internal);
      done = true;

    }

    else if (input == '\n') {
      line += 1;
      ALLOC_PHRASEM(phr);
      phr->table = TokenType_Separator;
      phr->d.str = NULL;
      #ifdef SCANNER_DEBUG
        PrintPhrasem(phr);
      #endif
      if( !AddToQueue(phr) ) RaiseError("queue error", ErrorType_Internal);
    }

    else if (input == '!') {
      returnByte(input);
      getString();
    }


    else if ( input == '\'' ) {
      returnByte(input);
      getComment();
    }


    else if (input == '/') {
      input = getByte();
      if (input == '/') {returnByte('~'); getComment();}
      else {
        returnByte(input);

        // AddToQueue '/' as operator
        char * p = GetBuffer();
        if(p == NULL) RaiseError("buffer allocation error", ErrorType_Internal);


        long x = getOperatorId(p);
        free(p);

        if ( x == -1) RaiseError("constant table allocation error", ErrorType_Internal);

        ALLOC_PHRASEM(phr);
        phr->table = TokenType_Constant;
        phr->d.index = x;
        phr->line = line;

        #ifdef SCANNER_DEBUG
          PrintPhrasem(phr);
        #endif

        if( !AddToQueue(phr) ) RaiseError ("queue allocation error", ErrorType_Internal);

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
            ||(input == ',')) {
      returnByte(input);
      getOperator();
    }


    else if (((input >= 'A') && (input <= 'Z'))
          || ((input >= 'a') && (input <= 'z'))
          || (input == '_')) {
      returnByte(input);
      getIdentifier();
    }

/*
    else if ((input >= '0') && (input <= '9')) {
      returnByte(input);
      getNumber();
    }
*/
    else RaiseError("unknown symbol", ErrorType_Syntax);

    //phr->d.index = input;
    //phr->table = TokenType_Constant;
    //phr->line = 1;

    //#ifdef SCANNER_DEBUG
    //  PrintPhrasem(phr);
    //#endif

    //AddToQueue(phr);
  }
// #undef RaiseError, ALLOC_PHRASEM;
  EndScanner(NULL, ErrorType_Ok);
  return NULL;
}
