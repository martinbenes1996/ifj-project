
/**
 * @file scanner
 * @authors xbenes49 xbolsh00 xkrato47 xpolan09
 * @date 2nd october 2017
 * @brief Lexical Scanner module.
 *
 * This module implements Lexical Scanner.
 */

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "err.h"
#include "io.h"
#include "queue.h"
#include "scanner.h"
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
  FinishConnectionToQueue();
  isscanning = false;
}


// extern bool AddToBuffer(char c);

/**
 * @brief function for comment
 *
 * This function recognizes if this a line or block comment
 * return true if everything is allright in other way - false.
 */

/*
bool getComment() {
  int state = 0;
  int input;
  bool end = false;

  while (!end) {
    input = getByte();

    switch (state) {
      case 0:
        if ( input == '/') {state = 1; break;}
        elseif (input == ''') {state = 2; break;}
        else goto ErrorLabel;

      case 1: 
        if (input != '/') {AddToBuffer('/'); end = true; getByte(input); break;}
        else {state = 3; break;}

      case 2:
        if ( input != '\n') {break;}
        else {end = true; break;}

      case 3: 
        if (input != '/') {break;}
        else { state = 4; break;}

      case 4: 
        if (input != '/') {state = 3; break;}
        else {end = true; break;}



      return true;
      ErrorLabel:
        EndScanner("Scanner: getString: Syntax Error!", ErrorType_Syntax);
        return false;

    }

  }

}

/**
 * @brief the function can recognize if this is an operator and save it to buffer
 * return true if everything is allright in other way - false
 */

/*
bool getOperator() {
  int state = 0;
  int input;
  bool end = false;

  while(!end) {
    input = getByte();

    switch (state) {

      case 0:
        if ( input == '<' ) {AddToBuffer(input); state = 1; break;}
        else if ( input == '>') {AddToBuffer(input); state = 2; break;}
        else if (input == '=') {AddToBuffer(input); end = true; break;}
        else if (input == '+') {AddToBuffer(input); end = true; break;}
        else if (input == '-') {AddToBuffer(input); end = true; break;}
        else if (input == '*') {AddToBuffer(input); end = true; break;}
        else if (input == '/') {AddToBuffer(input); end = true; break;}
        else if (input == '\\') {AddToBuffer(input); end = true; break;}

        else goto ErrorLabel;


      case 1:
        if ( input == '>') {AddToBuffer('>'); end = true; break;}
        else if (input == '=') {AddToBuffer('='); end = true; break;}
      
        else goto ErrorLabel;

      case 2:
        if ( input == '=') {AddToBuffer('='); end = true; break;}
        
        else goto ErrorLabel;



    return true;
    ErrorLabel:
      EndScanner("Scanner: getString: Syntax Error!", ErrorType_Syntax);
      return false;
   }
  }
  return true;
}

/**
 * @brief The function can recognize if this is string and save it to buffer
 * return true if everything is ok in otherway -false
 */


/*
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
        else goto ErrorLabel;

      case 1:
        if (input == '"') {state = 2; break;}
        else goto ErrorLabel;

      case 2:
        if (input == '"') { end = true; break; }
        else if (input == EOF) goto ErrorLabel;
        else if (input != '\\') { AddToBuffer(input); break; }
        else { state = 3; break; }

      case 3:
        if (input == 'n') { AddToBuffer('\n'); state = 2; break; }
        else if (input == 't') { AddToBuffer('\t'); state = 2; break; }
        else if (input == '"') { AddToBuffer ('"'); state = 2; break; }
        else if (input == '\\') { AddToBuffer('\\'); state = 2; break; }
        else if (isdigit(input)) { asciival += 100*(input - '0'); state = 4; break; }

        else goto ErrorLabel;

      case 4:
        if(isdigit(input)) { asciival += 10*(input-'0'); state = 5; break;}
        else goto ErrorLabel;

      case 5:
        if(isdigit(input))
        {
          asciival += input-'0';
          if(asciival > 255) goto ErrorLabel;
          AddToBuffer(asciival);
          state = 2;
          break;
        }
        else goto ErrorLabel;
  }

  return true;
  // i know it is pigstyle, but Martin typed it.
  ErrorLabel:
    EndScanner("Scanner: getString: Syntax Error!", ErrorType_Syntax);
    return false;

}

*/

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
    Phrasem phr = malloc( sizeof(struct phrasem_data) );
    if( phr == NULL )
    {
      EndScanner("Scanner: InitScanner: couldn't allocate memory", ErrorType_Internal);
      return NULL;
    }

    // here will be lexical analysis ----------------------------------

   /*

    if (input == '\n') { line += 1;}

    else if (input == '!') {
      returnByte(input);
      getString();
    }

    else if ((input == '/')||( input == ''')) {
      returnByte(input);
      getComment();
    }

    else if ((input == '<')||(input == '>')||(input == '=')||(input == '+')||(input == '-')||( input == '*')||( input == '/')
       ||( input == '\\' )) {
      returnByte(input);
      getOperator();
    }
    */

    phr->d.index = input;
    phr->table = TokenType_Constant;
    phr->line = 1;

    #ifdef SCANNER_DEBUG
      PrintPhrasem(phr);
    #endif

    AddToQueue(phr);
  }

  EndScanner(NULL, ErrorType_Ok);
  return NULL;
}
