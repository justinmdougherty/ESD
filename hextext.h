#ifndef HEXTEXT_H
#define HEXTEXT_H
/*=============================================================================
* Title: hextext.h
* Purpose: This module provides a convenient function for converting a byte of
* data into a character representation of its two constituent hexadecimal
* digits.  The module also provides an enumerated type for expressing little
* and big endianness.
* Revision history: 2012-09-19 -- Initial creation
*                   2012-09-25 -- Added convertHexTextToByte function
*                   2014-09-19 -- Changed return type to uint8_t for function
*                                 convertHexTextToByte.  It will now return 1
*                                 if successful, otherwise 0.
==============================================================================*/
#include <stdint.h>
#include <stdbool.h>

typedef enum
{
	LITTLE,
	BIG
} endian;

#define IS_HEX_CHAR(c) ( ( (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || \
                           (c >= 'A' && c <= 'F') ) ? 1 : 0 )

/*=============================================================================
* Function: convertByteToHexText
* Purpose: This function is used to provide a character representation of the
* two four-bit portions (nibbles) of an eight-bit unsigned integer.  The user
* can specify the endianness desired and receive the appropriate upper and
* lower characters.
* Input: uint8_t inputByte - the unsigned eight-bit integer to convert
*        char * upperHexText - pointer to a character to contain the upper char
*        char * lowerHexText - pointer to a character to contain the lower char
*        endian endianness - enum value corresponding to big or little endian
* Returns: None
* Modifies: Returns the resulting hex characters in upperHexText and
*           lowerHexText.
==============================================================================*/
void convertByteToHexText( uint8_t inputByte,
                           char * upperHexText,
			                     char * lowerHexText,
			                     endian endianness );

/*=============================================================================
* Function: convertHexTextToByte
* Purpose: This function is used to build an eight-bit unsigned integer from
* the hexadecimal character representation of two four-bit portions (nibbles).
* The user can specify the endianness desired and receive the appropriate
* unsigned eight-bit integer.
* Input: char upperHexText - the upper nibble hexadecimal character
*        char lowerHexText - the lower nibble hexadecimal character
*        uint8_t * resultByte - the resultant unsigned eight-bit integer
*        endian endianness - enum value corresponding to big or little endian
* Returns: true if successful, false if failed
* Modifies: Returns the resulting unsigned eight-bit integer in resultByte.
==============================================================================*/
bool convertHexTextToByte( char upperHexText,
	                         char lowerHexText,
			                     uint8_t * resultByte,
			                     endian endianness );

#endif