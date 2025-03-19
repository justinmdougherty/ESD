////////////////////////////////////////////////////////////////////////////////
//        Property of United States of America - For Official Use Only        //
////////////////////////////////////////////////////////////////////////////////
/*
 *  FILE NAME     : lcd.c
 *
 *  DESCRIPTION   : Defines interface to write font strings to UC1701X LCD
 *
 *      (1) bool LCDWriteFont(page, column, height, width, fontChar[], invert)
 *      (2) bool LCDWriteStringTerminal6X8(page, column, string[], invert)
 *      (3) bool LCDWriteStringTerminal12X16(page, column, string[], invert)
 *      (4) bool LCDWriteStringVerdana34X56(page, column, string[], invert)
 *      (5) bool LCDClearScreen(void)
 *
 *  USE: call
 *
 *  WRITTEN BY    : Nicholus Sunshine, NSWC H12
 *  MODIFICATIONS (in reverse chronological order)
 *    2021/06/01, Robert Kirby, NSWC H12
 *      Use VERDANA_34X56_SPACE instead of VERDANA_34X56_EQUALS for default:
 *      as <SP> is good way to clear 7 out of LCD's 8 character rows
 *    2017/03/31, Robert Kirby, NSWC H12
 *      Change LCDWriteString___() code to switch(*s) instead of switch(s[i])
 *      Change LCDWriteString___() switch(*s) cases to set pFontChar and then
 *      use a single call to LCDWriteFont() for significantly smaller code
 *      Change Verdana34X64 to Verdana34X56, saves memory by getting rid of
 *      wasted white space above characters and allows displaying a row of 6X8
 *      Rename TERMINAL_6X8_ENVELOPE_LEFT/RIGHT to ENVELOPE_6X8_LEFT/RIGHT
 *      Rename defined WIDTHs and HEIGHTs
 *      Comments on what Nick developed for ESD
 *
 *  REFERENCE DOCUMENTS
 *    1.
 */
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include "lcd.h"
#include "fonts.h"
#include "uc1701x.h"


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  bool LCDWriteFont(startPage,startColumn,fontHeight,fontWidth,fontChar,invert)
//  The character specified by fontChar is displayed on the LCD at position
//  (startColumn, startPage).
//
//  NOTE: Consideration needs to be given to turning off the display before
//        writing the character, and then turning the display back on again to
//        avoid flicker.  Needs testing to see if this step is necessary  .
//
//  INPUT : uint8_t startPage - equivalent to row on LCD at which to place text
//          uint8_t startColumn - LCD pixel column at which to place character
//          uint8_t fontHeight - height of font in pages (rows)
//          uint8_t fontWidth - width of font in columns (1-byte each column)
//          const uint8_t fontChar[] - array font info for character to display
//          bool invert - ? invert video of character ?
//  OUTPUT: bool - true if procedure succeeded, otherwise false
//  CALLS : UC1701XSetPageAddress
//          UC1701XSetColumnAddress
//          UC1701XWriteData
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool LCDWriteFont( uint8_t startPage, uint8_t startColumn, uint8_t fontHeight, uint8_t fontWidth, const uint8_t fontChar[], bool invert )
{
  // currentPage and currentColumn are used as loop counters, starting at the upper-left-most byte
  int16_t currentPage = startPage;
  int16_t currentColumn = startColumn;
  uint16_t index = 0;

  while ( currentPage < (startPage + fontHeight) )
  {
    if ( false == UC1701XSetPageAddress( currentPage ) )        {
      return false;                                             }
    while ( currentColumn < (startColumn + fontWidth) )
    {
      if ( false == UC1701XSetColumnAddress( currentColumn ) )  {
        return false;                                           }
      // Write byte indexed at current page and column
      // Since fontChar array is one dimension, have to perform multiplication to get the index right
      if ( invert )                                             {
        UC1701XWriteData( ~fontChar[index++] );                 }
      else                                                      {
        UC1701XWriteData( fontChar[index++] );                  }

      // Move on to the next column
      ++currentColumn;
    }
    // After column boundary has been reached, move back to beginning column
    currentColumn = startColumn;

    // Move on to the next page
    ++currentPage;
  }

  return true;
} // end function LCDWriteFont


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  bool LCDWriteStringTerminal6X8(startPage,startColumn,*str,invert)
//  The character string specified by s is displayed to the LCD at position
//  (startColumn, startPage).
//
//  INPUT : uint8_t startPage - equivalent to row on LCD at which to place text
//          uint8_t startColumn - LCD pixel column at which to place character
//          char    s[] - null terminated set of characters to display
//          bool invert - ? invert video of character ?
//  OUTPUT: bool - true if procedure succeeded, otherwise false
//  CALLS : LCDWriteFont
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool LCDWriteStringTerminal6X8( uint8_t startPage, uint8_t startColumn, char s[], bool invert )
{
  // TODO:  Change this function to print up to whichever comes first \0 or a len parameter
  uint8_t currentColumn;
  const uint8_t * pFontChar;
  bool    isOk = true;

  for( currentColumn = startColumn; isOk && (*s != '\0'); currentColumn += WIDTH_6X8, s++ )
  {
    switch (*s)
    {
      case ENVELOPE_LEFT_CHAR:  pFontChar = ENVELOPE_6X8_LEFT;   break;
      case ENVELOPE_RIGHT_CHAR: pFontChar = ENVELOPE_6X8_RIGHT;  break;

      case ' ':   pFontChar = TERMINAL_6X8_SPACE;         break;
      case '!':   pFontChar = TERMINAL_6X8_EXCLAMATION;   break;
      case '"':   pFontChar = TERMINAL_6X8_DOUBLEQUOTE;   break;
      case '#':   pFontChar = TERMINAL_6X8_HASH;          break;
      case '$':   pFontChar = TERMINAL_6X8_DOLLAR;        break;
      case '%':   pFontChar = TERMINAL_6X8_PERCENT;       break;
      case '&':   pFontChar = TERMINAL_6X8_AMPERSAND;     break;
      case '\'':  pFontChar = TERMINAL_6X8_SINGLEQUOTE;   break;
      case '(':   pFontChar = TERMINAL_6X8_OPENPAREN;     break;
      case ')':   pFontChar = TERMINAL_6X8_CLOSEPAREN;    break;
      case '*':   pFontChar = TERMINAL_6X8_ASTERISK;      break;
      case '+':   pFontChar = TERMINAL_6X8_PLUS;          break;
      case ',':   pFontChar = TERMINAL_6X8_COMMA;         break;
      case '-':   pFontChar = TERMINAL_6X8_MINUS;         break;
      case '.':   pFontChar = TERMINAL_6X8_PERIOD;        break;
      case '/':   pFontChar = TERMINAL_6X8_FWDSLASH;      break;
      case '0':   pFontChar = TERMINAL_6X8_0;             break;
      case '1':   pFontChar = TERMINAL_6X8_1;             break;
      case '2':   pFontChar = TERMINAL_6X8_2;             break;
      case '3':   pFontChar = TERMINAL_6X8_3;             break;
      case '4':   pFontChar = TERMINAL_6X8_4;             break;
      case '5':   pFontChar = TERMINAL_6X8_5;             break;
      case '6':   pFontChar = TERMINAL_6X8_6;             break;
      case '7':   pFontChar = TERMINAL_6X8_7;             break;
      case '8':   pFontChar = TERMINAL_6X8_8;             break;
      case '9':   pFontChar = TERMINAL_6X8_9;             break;
      case ':':   pFontChar = TERMINAL_6X8_COLON;         break;
      case ';':   pFontChar = TERMINAL_6X8_SEMICOLON;     break;
      case '<':   pFontChar = TERMINAL_6X8_LESS;          break;
      case '=':   pFontChar = TERMINAL_6X8_EQUALS;        break;
      case '>':   pFontChar = TERMINAL_6X8_GREATER;       break;
      case '?':   pFontChar = TERMINAL_6X8_QUESTION;      break;
      case '@':   pFontChar = TERMINAL_6X8_AT;            break;
      case 'A':   pFontChar = TERMINAL_6X8_A;             break;
      case 'B':   pFontChar = TERMINAL_6X8_B;             break;
      case 'C':   pFontChar = TERMINAL_6X8_C;             break;
      case 'D':   pFontChar = TERMINAL_6X8_D;             break;
      case 'E':   pFontChar = TERMINAL_6X8_E;             break;
      case 'F':   pFontChar = TERMINAL_6X8_F;             break;
      case 'G':   pFontChar = TERMINAL_6X8_G;             break;
      case 'H':   pFontChar = TERMINAL_6X8_H;             break;
      case 'I':   pFontChar = TERMINAL_6X8_I;             break;
      case 'J':   pFontChar = TERMINAL_6X8_J;             break;
      case 'K':   pFontChar = TERMINAL_6X8_K;             break;
      case 'L':   pFontChar = TERMINAL_6X8_L;             break;
      case 'M':   pFontChar = TERMINAL_6X8_M;             break;
      case 'N':   pFontChar = TERMINAL_6X8_N;             break;
      case 'O':   pFontChar = TERMINAL_6X8_O;             break;
      case 'P':   pFontChar = TERMINAL_6X8_P;             break;
      case 'Q':   pFontChar = TERMINAL_6X8_Q;             break;
      case 'R':   pFontChar = TERMINAL_6X8_R;             break;
      case 'S':   pFontChar = TERMINAL_6X8_S;             break;
      case 'T':   pFontChar = TERMINAL_6X8_T;             break;
      case 'U':   pFontChar = TERMINAL_6X8_U;             break;
      case 'V':   pFontChar = TERMINAL_6X8_V;             break;
      case 'W':   pFontChar = TERMINAL_6X8_W;             break;
      case 'X':   pFontChar = TERMINAL_6X8_X;             break;
      case 'Y':   pFontChar = TERMINAL_6X8_Y;             break;
      case 'Z':   pFontChar = TERMINAL_6X8_Z;             break;
      case '[':   pFontChar = TERMINAL_6X8_OPENBRACKET;   break;
      case '\\':  pFontChar = TERMINAL_6X8_BACKSLASH;     break;
      case ']':   pFontChar = TERMINAL_6X8_CLOSEDBRACKET; break;
      case '^':   pFontChar = TERMINAL_6X8_CARET;         break;
      case '_':   pFontChar = TERMINAL_6X8_UNDERSCORE;    break;
      case '`':   pFontChar = TERMINAL_6X8_GRAVE;         break;
      case 'a':   pFontChar = TERMINAL_6X8_a;             break;
      case 'b':   pFontChar = TERMINAL_6X8_b;             break;
      case 'c':   pFontChar = TERMINAL_6X8_c;             break;
      case 'd':   pFontChar = TERMINAL_6X8_d;             break;
      case 'e':   pFontChar = TERMINAL_6X8_e;             break;
      case 'f':   pFontChar = TERMINAL_6X8_f;             break;
      case 'g':   pFontChar = TERMINAL_6X8_g;             break;
      case 'h':   pFontChar = TERMINAL_6X8_h;             break;
      case 'i':   pFontChar = TERMINAL_6X8_i;             break;
      case 'j':   pFontChar = TERMINAL_6X8_j;             break;
      case 'k':   pFontChar = TERMINAL_6X8_k;             break;
      case 'l':   pFontChar = TERMINAL_6X8_l;             break;
      case 'm':   pFontChar = TERMINAL_6X8_m;             break;
      case 'n':   pFontChar = TERMINAL_6X8_n;             break;
      case 'o':   pFontChar = TERMINAL_6X8_o;             break;
      case 'p':   pFontChar = TERMINAL_6X8_p;             break;
      case 'q':   pFontChar = TERMINAL_6X8_q;             break;
      case 'r':   pFontChar = TERMINAL_6X8_r;             break;
      case 's':   pFontChar = TERMINAL_6X8_s;             break;
      case 't':   pFontChar = TERMINAL_6X8_t;             break;
      case 'u':   pFontChar = TERMINAL_6X8_u;             break;
      case 'v':   pFontChar = TERMINAL_6X8_v;             break;
      case 'w':   pFontChar = TERMINAL_6X8_w;             break;
      case 'x':   pFontChar = TERMINAL_6X8_x;             break;
      case 'y':   pFontChar = TERMINAL_6X8_y;             break;
      case 'z':   pFontChar = TERMINAL_6X8_z;             break;
      case '{':   pFontChar = TERMINAL_6X8_OPENBRACE;     break;
      case '}':   pFontChar = TERMINAL_6X8_CLOSEDBRACE;   break;
      case '|':   pFontChar = TERMINAL_6X8_VERTBAR;       break;
      // Print a '?' for an unhandled character as well as for '?'
      default:    pFontChar = TERMINAL_6X8_QUESTION;      break;
    }
    isOk = LCDWriteFont(startPage, currentColumn,
                        HEIGHT_6X8, WIDTH_6X8,
                        pFontChar, invert);
  }

  return isOk;
} // end function LCDWriteStringTerminal6X8


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  bool LCDWriteStringTerminal12X16(startPage,startColumn,*str,invert)
//  The character string specified by s is displayed to the LCD at position
//  (startColumn, startPage).
//
//  INPUT : uint8_t startPage - equivalent to row on LCD at which to place text
//          uint8_t startColumn - LCD pixel column at which to place character
//          char    s[] - null terminated set of characters to display
//          bool invert - ? invert video of character ?
//  OUTPUT: bool - true if procedure succeeded, otherwise false
//  CALLS : LCDWriteFont
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool LCDWriteStringTerminal12X16( uint8_t startPage, uint8_t startColumn, char s[], bool invert )
{
  // TODO:  Change this function to print up to whichever comes first \0 or a len parameter
  uint8_t currentColumn;
  const uint8_t * pFontChar;
  bool    isOk = true;

  for( currentColumn = startColumn; isOk && (*s != '\0'); currentColumn += WIDTH_12X16, s++ )
  {
    switch (*s)
    {
      case ' ':   pFontChar = TERMINAL_12X16_SPACE;         break;
      case '!':   pFontChar = TERMINAL_12X16_EXCLAMATION;   break;
      case '"':   pFontChar = TERMINAL_12X16_DOUBLEQUOTE;   break;
      case '#':   pFontChar = TERMINAL_12X16_HASH;          break;
      case '$':   pFontChar = TERMINAL_12X16_DOLLAR;        break;
      case '%':   pFontChar = TERMINAL_12X16_PERCENT;       break;
      case '&':   pFontChar = TERMINAL_12X16_AMPERSAND;     break;
      case '\'':  pFontChar = TERMINAL_12X16_SINGLEQUOTE;   break;
      case '(':   pFontChar = TERMINAL_12X16_OPENPAREN;     break;
      case ')':   pFontChar = TERMINAL_12X16_CLOSEPAREN;    break;
      case '*':   pFontChar = TERMINAL_12X16_ASTERISK;      break;
      case '+':   pFontChar = TERMINAL_12X16_PLUS;          break;
      case ',':   pFontChar = TERMINAL_12X16_COMMA;         break;
      case '-':   pFontChar = TERMINAL_12X16_MINUS;         break;
      case '.':   pFontChar = TERMINAL_12X16_PERIOD;        break;
      case '/':   pFontChar = TERMINAL_12X16_FWDSLASH;      break;
      case '0':   pFontChar = TERMINAL_12X16_0;             break;
      case '1':   pFontChar = TERMINAL_12X16_1;             break;
      case '2':   pFontChar = TERMINAL_12X16_2;             break;
      case '3':   pFontChar = TERMINAL_12X16_3;             break;
      case '4':   pFontChar = TERMINAL_12X16_4;             break;
      case '5':   pFontChar = TERMINAL_12X16_5;             break;
      case '6':   pFontChar = TERMINAL_12X16_6;             break;
      case '7':   pFontChar = TERMINAL_12X16_7;             break;
      case '8':   pFontChar = TERMINAL_12X16_8;             break;
      case '9':   pFontChar = TERMINAL_12X16_9;             break;
      case ':':   pFontChar = TERMINAL_12X16_COLON;         break;
      case ';':   pFontChar = TERMINAL_12X16_SEMICOLON;     break;
      case '<':   pFontChar = TERMINAL_12X16_LESS;          break;
      case '=':   pFontChar = TERMINAL_12X16_EQUALS;        break;
      case '>':   pFontChar = TERMINAL_12X16_GREATER;       break;
      case '?':   pFontChar = TERMINAL_12X16_QUESTION;      break;
      case '@':   pFontChar = TERMINAL_12X16_AT;            break;
      case 'A':   pFontChar = TERMINAL_12X16_A;             break;
      case 'B':   pFontChar = TERMINAL_12X16_B;             break;
      case 'C':   pFontChar = TERMINAL_12X16_C;             break;
      case 'D':   pFontChar = TERMINAL_12X16_D;             break;
      case 'E':   pFontChar = TERMINAL_12X16_E;             break;
      case 'F':   pFontChar = TERMINAL_12X16_F;             break;
      case 'G':   pFontChar = TERMINAL_12X16_G;             break;
      case 'H':   pFontChar = TERMINAL_12X16_H;             break;
      case 'I':   pFontChar = TERMINAL_12X16_I;             break;
      case 'J':   pFontChar = TERMINAL_12X16_J;             break;
      case 'K':   pFontChar = TERMINAL_12X16_K;             break;
      case 'L':   pFontChar = TERMINAL_12X16_L;             break;
      case 'M':   pFontChar = TERMINAL_12X16_M;             break;
      case 'N':   pFontChar = TERMINAL_12X16_N;             break;
      case 'O':   pFontChar = TERMINAL_12X16_O;             break;
      case 'P':   pFontChar = TERMINAL_12X16_P;             break;
      case 'Q':   pFontChar = TERMINAL_12X16_Q;             break;
      case 'R':   pFontChar = TERMINAL_12X16_R;             break;
      case 'S':   pFontChar = TERMINAL_12X16_S;             break;
      case 'T':   pFontChar = TERMINAL_12X16_T;             break;
      case 'U':   pFontChar = TERMINAL_12X16_U;             break;
      case 'V':   pFontChar = TERMINAL_12X16_V;             break;
      case 'W':   pFontChar = TERMINAL_12X16_W;             break;
      case 'X':   pFontChar = TERMINAL_12X16_X;             break;
      case 'Y':   pFontChar = TERMINAL_12X16_Y;             break;
      case 'Z':   pFontChar = TERMINAL_12X16_Z;             break;
      case '[':   pFontChar = TERMINAL_12X16_OPENBRACKET;   break;
      case '\\':  pFontChar = TERMINAL_12X16_BACKSLASH;     break;
      case ']':   pFontChar = TERMINAL_12X16_CLOSEDBRACKET; break;
      case '^':   pFontChar = TERMINAL_12X16_CARET;         break;
      case '_':   pFontChar = TERMINAL_12X16_UNDERSCORE;    break;
      case '`':   pFontChar = TERMINAL_12X16_GRAVE;         break;
      case 'a':   pFontChar = TERMINAL_12X16_a;             break;
      case 'b':   pFontChar = TERMINAL_12X16_b;             break;
      case 'c':   pFontChar = TERMINAL_12X16_c;             break;
      case 'd':   pFontChar = TERMINAL_12X16_d;             break;
      case 'e':   pFontChar = TERMINAL_12X16_e;             break;
      case 'f':   pFontChar = TERMINAL_12X16_f;             break;
      case 'g':   pFontChar = TERMINAL_12X16_g;             break;
      case 'h':   pFontChar = TERMINAL_12X16_h;             break;
      case 'i':   pFontChar = TERMINAL_12X16_i;             break;
      case 'j':   pFontChar = TERMINAL_12X16_j;             break;
      case 'k':   pFontChar = TERMINAL_12X16_k;             break;
      case 'l':   pFontChar = TERMINAL_12X16_l;             break;
      case 'm':   pFontChar = TERMINAL_12X16_m;             break;
      case 'n':   pFontChar = TERMINAL_12X16_n;             break;
      case 'o':   pFontChar = TERMINAL_12X16_o;             break;
      case 'p':   pFontChar = TERMINAL_12X16_p;             break;
      case 'q':   pFontChar = TERMINAL_12X16_q;             break;
      case 'r':   pFontChar = TERMINAL_12X16_r;             break;
      case 's':   pFontChar = TERMINAL_12X16_s;             break;
      case 't':   pFontChar = TERMINAL_12X16_t;             break;
      case 'u':   pFontChar = TERMINAL_12X16_u;             break;
      case 'v':   pFontChar = TERMINAL_12X16_v;             break;
      case 'w':   pFontChar = TERMINAL_12X16_w;             break;
      case 'x':   pFontChar = TERMINAL_12X16_x;             break;
      case 'y':   pFontChar = TERMINAL_12X16_y;             break;
      case 'z':   pFontChar = TERMINAL_12X16_z;             break;
      case '{':   pFontChar = TERMINAL_12X16_OPENBRACE;     break;
      case '}':   pFontChar = TERMINAL_12X16_CLOSEDBRACE;   break;
      case '|':   pFontChar = TERMINAL_12X16_VERTBAR;       break;
      // Print a '?' for an unhandled character as well as for '?'
      default:    pFontChar = TERMINAL_12X16_QUESTION;      break;
    }
    isOk = LCDWriteFont(startPage, currentColumn,
                        HEIGHT_12X16, WIDTH_12X16,
                        pFontChar, invert);
  }
  return isOk;
} // end function LCDWriteStringTerminal12X16


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  bool LCDWriteStringVerdana34X56(startPage,startColumn,*str,invert)
//  The character string specified by s is displayed to the LCD at position
//  (startColumn, startPage).
//
//  INPUT : uint8_t startPage - equivalent to row on LCD at which to place text
//          uint8_t startColumn - LCD pixel column at which to place character
//          char    s[] - null terminated set of characters to display
//          bool invert - ? invert video of character ?
//  OUTPUT: bool - true if procedure succeeded, otherwise false
//  CALLS : LCDWriteFont
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool LCDWriteStringVerdana34X56( uint8_t startPage, uint8_t startColumn, char s[], bool invert )
{
  uint8_t currentColumn;
  const uint8_t * pFontChar;
  bool    isOk = true;

  for( currentColumn = startColumn; isOk && (*s != '\0'); currentColumn += WIDTH_34X56, s++ )
  {
    switch (*s)
    {
      case '0':   pFontChar = VERDANA_34X56_0;        break;
      case '1':   pFontChar = VERDANA_34X56_1;        break;
      case '2':   pFontChar = VERDANA_34X56_2;        break;
      case '3':   pFontChar = VERDANA_34X56_3;        break;
      case '4':   pFontChar = VERDANA_34X56_4;        break;
      case '5':   pFontChar = VERDANA_34X56_5;        break;
      case '6':   pFontChar = VERDANA_34X56_6;        break;
      case '7':   pFontChar = VERDANA_34X56_7;        break;
      case '8':   pFontChar = VERDANA_34X56_8;        break;
      case '9':   pFontChar = VERDANA_34X56_9;        break;
      // Print a '=' for an unhandled character as well as for '='
      default:    pFontChar = VERDANA_34X56_SPACE;   break;
    }
    isOk = LCDWriteFont(startPage, currentColumn,
                        HEIGHT_34X56, WIDTH_34X56,
                        pFontChar, invert);
  }

  return isOk;
} // end function LCDWriteStringVerdana34X56


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  bool LCDClearScreen(void)
//  This function clears the LCD screen by writing ' ' to all positions.
//
//  INPUT : NONE
//  OUTPUT: bool - true if procedure succeeded, otherwise false
//  CALLS : LCDWriteStringTerminal6X8
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool LCDClearScreen(void)
{
  int         page;
  static char buf[] = "                 ";
  bool        isOk = true;

  for ( page = 0; isOk && (page <= LCD_MAX_PAGES); ++page )
  {
    isOk = LCDWriteStringTerminal6X8( page, 0, buf, false );
  }

  return isOk;
} // end function LCDClearScreen
