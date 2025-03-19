/*=============================================================================
* Title: hextext.c
* Purpose: This module provides a convenient function for converting a byte of
* data into a character representation of its two constituent hexadecimal
* digits.  The module also provides an enumerated type for expressing little
* and big endianness.
* Revision history: 2012-09-19 -- Initial creation
*                   2012-09-25 -- Added convertHexTextToByte function
*                   2014-09-19 -- Changed return type to uint8_t for function
*                                 convertHexTextToByte.  It will now return 1
*                                 if successful, otherwise 0.  Same change for
*                                 static function convertHexToNibble.
==============================================================================*/
#include "hextext.h"

/*=============================================================================
* Function: convertNibbleToHex
* Purpose: This function performs the short task of examining the lower four
* bits of an unsigned eight-bit integer and returning the ASCII character
* representation of that nibble.
* Input: uint8_t byte - the unsigned eight-bit integer to examine
*        char * hex - pointer to a character to contain the ASCII character
*        uint8_t lowerUpper - flag for which nibble to examine
*	                      0: examine lower nibble
*			      1: examine upper nibble
* Returns: None
* Modifies: Returns the resulting hex character in character pointer hex.
==============================================================================*/
static void convertNibbleToHex( uint8_t byte, char * hex, uint8_t lowerUpper )
{
	if (0 == lowerUpper) {
		byte &= 0x0F;
	} else {
		byte >>= 4;
	}

	if (byte <= 9) { 
		// Use byte as an ASCII index starting at '0'
		*hex = (char) byte + '0'; // Convert to decimal 0-9
	} else {
		// Use byte as an ASCII index starting at 'A'.  Must subtract out 
		// 0x0A (10) such that byte is an offset starting at 0.
		*hex = (char) byte + 'A' - (char) 0x0A; // Convert to uppercase A-F
	}

	return;
}

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
			                     endian endianness )
{
	if (BIG == endianness) {
		// Store upper hex char in leading char spot
		convertNibbleToHex(inputByte, upperHexText, 1); 
		// Store lower hex char in trailing char spot
		convertNibbleToHex(inputByte, lowerHexText, 0); 
	} else {
		// Store lower hex char in leading char spot
		convertNibbleToHex(inputByte, upperHexText, 0); 
		// Store upper hex char in trailing char spot
		convertNibbleToHex(inputByte, lowerHexText, 1); 
	}

	return;
}

/*=============================================================================
* Function convertHexToNibble
* Purpose: This function performs the short task of examining a supplied
*          ASCII character representing a hexadecimal nibble and then 
*          converting it to a decimal representation, storing it in the upper
*          or lower nibble of an unsigned eight-bit integer, depending on user
*          specification.
* Input:  char hex - ASCII character representing hexadecimal nibble
*         uint8_t * byte - the resultant unsigned eight-bit integer with
*                          modified nibble
*         uint8_t lowerUpper - flag for which nibble to convert
*                              0: convert lower nibble
*			       1: convert upper nibble
* Returns: 1 if successful, 0 if failed
* Modifies: Returns the resulting unsigned eight-bit integer in pointer "byte".
==============================================================================*/
static uint8_t convertHexToNibble( char hex, uint8_t * byte, uint8_t lowerUpper )
{
	uint8_t buildNibble;

	if (hex >= '0' && hex <= '9') {
    buildNibble = (uint8_t) (hex - '0');
	} else if ( hex >= 'A' && hex <= 'F') {
    buildNibble = (uint8_t) (hex - 'A') + 0x0A;;
	} else if ( hex >= 'a' && hex <= 'f') {
    buildNibble = (uint8_t) (hex - 'a') + 0x0A;;
  } else { // not in valid range for hextext characters
    return 0;
  }

	if (0 == lowerUpper) {
		*byte &= 0xF0; // Clear lower nibble
		*byte |= buildNibble;
	} else {
		*byte &= 0x0F; // Clear upper nibble
		buildNibble <<= 4;
		*byte |= buildNibble;
	}

	return 1;
}
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
* Returns: 1 if successful, 0 if failed due to hextext not being valid character
* Modifies: Returns the resulting unsigned eight-bit integer in resultByte.
==============================================================================*/
bool convertHexTextToByte( char upperHexText,
	                         char lowerHexText,
	                         uint8_t * resultByte,
		    	                 endian endianness )
{
  if (!( 
  // if NOT in ranges defined below, then return error w/o making any changes
    (upperHexText >= '0' && upperHexText <= '9') || 
    (upperHexText >= 'a' && upperHexText <= 'f') || 
    (upperHexText >= 'A' && upperHexText <= 'F') 
    )) {
    return false;
  }
    
  if (!( 
    // if NOT in ranges defined below, then return error w/o making any changes
      (lowerHexText >= '0' && lowerHexText <= '9') || 
      (lowerHexText >= 'a' && lowerHexText <= 'f') || 
      (lowerHexText >= 'A' && lowerHexText <= 'F') 
      )) {
    return false;
  }

  if (BIG == endianness) {
    // Get uppper nibble from upper hex char
    convertHexToNibble(upperHexText, resultByte, 1);
    // Get lower nibble from lower hex char
    convertHexToNibble(lowerHexText, resultByte, 0);
  } else {
    // Get upper nibble from lower hex char
    convertHexToNibble(lowerHexText, resultByte, 1);
    // Get lower nibble from upper hex char
    convertHexToNibble(upperHexText, resultByte, 0);
  }

  return true;
}