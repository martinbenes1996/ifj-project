
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

static bool isscanning = true;
bool done = false;
int line = 1;

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
extern bool AddToBuffer(char c);
/*
bool getOperator() {
  int state = 0;
  int input;
  bool end = false;

  while(!end) {
    input = getByte();

    switch (state) {

      case 0: 
        if ( input == '<' ) { state = 1; break;}
        else if ( input == '>') { state = 2; break;}
        else if (input == '=') {AddToBuffer(input); end = true; break;}
        else if (input == '+') {AddToBuffer(input); end = true; break;}
        else if (input == '-') {AddToBuffer(input); end = true; break;}
        else if (input == '*') {AddToBuffer(input); end = true; break;}
        else if (input == '/') {AddToBuffer(input); end = true; break;}
        else if (input == '\\') {AddToBuffer(input); end = true; break;}

        else goto ErrorLabel;
  

      case 1:
        if ( input == '>') {AddToBuffer('<>'); end = true; break;}
        else if (input == '=') {AddToBuffer('<='); end = true; break;}
        else if ((input == ' ')||((input >= '0')&&(input <= '9'))) {AddToBuffer('<'); returnByte(input); end = true; break;}
        else goto ErrorLabel;

      case 2:
        if ( input == '=') {AddToBuffer('>='); end = true; break;}
        else if ((input == ' ')||((input >= '0')&&(input <= '9'))) {AddToBuffer('>'); returnByte(input); end = true; break;}
        else goto ErrorLabel;



    return true;
    ErrorLabel:
      EndScanner("Scanner: getString: Syntax Error!", ErrorType_Syntax);
      return false;
   }
  }
}

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

void *InitScanner(void * v /*not used*/)
{
  (void)v; /* to avoid compiler warning */

  #ifdef SCANNER_DEBUG
    debug("Init Scanner.");
  #endif

  int input;
  while(!done)
  {
    input = getByte();
    Phrasem phr = malloc( sizeof(struct phrasem_data) );
    if( phr == NULL )
    {
      EndScanner("Scanner: InitScanner: couldn't allocate memory", ErrorType_Internal);
      return NULL;
    }

    // here will be lexical analysis ----------------------------------
/*
    if (input == '!') {
      returnByte(input);
      getString();
    }

    if ((input == '<')||(input == '>')||(input == '=')||(input == '+')||(input == '-')||( input == '*')||( input == '/')
       ||( input == '\' )) {
      returnByte(input);
      getOperator();  
    }
*/
    phr->d.index = input;

    #ifdef SCANNER_DEBUG
      PrintPhrasem(phr);
    #endif

    AddToQueue(phr);
  }

  EndScanner(NULL, ErrorType_Ok);
  return NULL;
}
