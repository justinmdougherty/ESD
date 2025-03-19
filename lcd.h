#ifndef LCD_H
#define LCD_H
////////////////////////////////////////////////////////////////////////////////
//        Property of United States of America - For Official Use Only        //
////////////////////////////////////////////////////////////////////////////////
/*
 *  FILE NAME     : lcd.h
 *
 *  DESCRIPTION   : Declares interface to write font strings to UC1701X LCD
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
 *    2017/03/31, Robert Kirby, NSWC H12
 *      Change Verdana34X64 to Verdana34X56, saves memory by getting rid of
 *      wasted white space above characters and allows displaying a row of 6X8
 *      Comments on what Nick developed for ESD
 *
 *  REFERENCE DOCUMENTS
 *    1.
 */
#include <stdint.h>          /* For uint32_t definition */
#include <stdbool.h>         /* For true/false definition */

#define LCD_MAX_PAGES (7)
#define LCD_MAX_COLS (16)   // correct answer is 17 for 6X8 font (102/6 = 17)

/* LCDWriteFont
 * In: startPage: starting page (row) number on LCD
 *     startColumn: starting column number on LCD
 *     fontHeight: height of font in pages (rows)
 *     fontWidth: width of font in columns (bytes)
 *     fontChar: specific font character organized as byte array
 *     invert: true to display inverted, false to display normal
 * Out: Returns true if success, otherwise false
 * Function:  The character specified by fontChar is displayed to the LCD
 *            at position (startColumn, startPage)
 * Notes: Consideration needs to be given to turning off the display before
 *        writing the character, and then turning the display back on again to
 *        avoid flicker.  Needs testing to see if this step is necessary  */
bool LCDWriteFont( uint8_t startPage, uint8_t startColumn, uint8_t fontHeight, uint8_t fontWidth, const uint8_t fontChar[], bool invert);

/* LCDWriteStringTerminal6X8
 * In: startPage: starting page (row) number on LCD
 *     startColumn: starting column number on LCD
 *     s: char array to be output to LCD
 *     invert: true to display inverted, false to display normal
 * Out: Returns true if success, otherwise false
 * Function:  The character string specified by s is displayed to the LCD
 *            at position (startColumn, startPage) */
bool LCDWriteStringTerminal6X8( uint8_t startPage, uint8_t startColumn, char s[], bool invert );

/* LCDWriteStringTerminal112X16
 * In: startPage: starting page (row) number on LCD
 *     startColumn: starting column number on LCD
 *     s: char array to be output to LCD
 *     invert: true to display inverted, false to display normal
 * Out: Returns true if success, otherwise false
 * Function:  The character string specified by s is displayed to the LCD
 *            at position (startColumn, startPage) */
bool LCDWriteStringTerminal12X16( uint8_t startPage, uint8_t startColumn, char s[], bool invert );

/* LCDWriteStringVerdana34X56
 * In: startPage: starting page (row) number on LCD
 *     startColumn: starting column number on LCD
 *     s: char array to be output to LCD
 *     invert: true to display inverted, false to display normal
 * Out: Returns true if success, otherwise false
 * Function:  The character string specified by s is displayed to the LCD
 *            at position (startColumn, startPage) */
bool LCDWriteStringVerdana34X56( uint8_t startPage, uint8_t startColumn, char s[], bool invert );

/* LCDClearScreen
 * In: N/A
 * Out: Returns true if success, otherwise false
 * Function:  This function clears the LCD screen */
bool LCDClearScreen( void );

#endif