//
//  Utils.h
//  MIPS Assembler
//
//  Created by Navid Rojiani on 11/14/13.
//  Copyright (c) 2013 NKR. All rights reserved.
//

#ifndef MIPS_Assembler_Utils_h
#define MIPS_Assembler_Utils_h

// Contains various utility functions for converting data to and
// from string and numeric forms.

/***********************************************************************
 *                     NUMERIC UTILITY FUNCTIONS
 ***********************************************************************/

/** Converts an integer to its binary form as a string.*/
char* intToBinary(int iVal, int numBits, char* s);


/** 
 * Converts a decimal (base-10) unsigned integer value to its hexadecimal
 * string representation in the form '0x0000404C'
 */
char* decimalToHex(int decVal, char* hexStr);


/***********************************************************************
 *                     STRING UTILITY FUNCTIONS
 ***********************************************************************/
/**
 * Swaps two characters in a string
 * Ex) if s is "abcd", then swapChars(m, 0, 1)
 * will change s to "bacd".
 */
void swapChars(char* s, int c1idx, int c2idx);


/**
 * Given a character and its position in the original string,
 * writes the binary representation of the character in the
 * binary string provided as a parameter.
 */
void charToBinary(char s, int charIdx, char* binaryStr);


/**
 * Helper method that takes all characters in string 's' and
 * puts their binary representations in 'binaryStr'
 */
void stringToBinaryHelper(char* s, char* binaryStr);


/**
 * Reorganizes the chars in a string so that for each word (i.e.,
 * each 4-byte section), the chars are reversed.
 */
void stringToBigEndian(char* s);


/**
 * Converts a string to its machine code representation - the string
 * is rearranged in the form described in the spec where every set of
 * four chars is reversed, and all chars including the null terminator
 * are converted to binary.
 * For example, the string "The sum of numbers in array" is converted
 * to:
 * 00100000 01100101 01101000 01010100		_ h e T
 * 00100000 01101101 01110101 01110011		_ m u s
 * 01101110 00100000 01100110 01101111		n _ f o
 * 01100101 01100010 01101101 01110101		e b m u
 * 01101001 00100000 01110011 01110010		i _ s r
 * 01110010 01100001 00100000 01101110		r a _ n
 * 01111001 01100001 01110010 00000000		y a r \0
 * The output has been formatted with spaces and new lines only for
 * clarity, the actual string returned will contain neither.
 */
char* stringToBinary(char* s, char* binaryString);


/**
 * Produces the same result as 'stringToBinary' except that the result
 * is padded with zero bytes if necessary so that the result ends at a
 * word (32-bit) offset.
 * For example, the string "abcdab" is converted to:
 * 01100100 01100011 01100010 01100001      d c b a
 * 01100010 01100001 00000000 00000000      b a 
 * Where the null string terminator is represented and an additional zero
 * byte has been added for padding.
 * This version, rather than 'stringToBinary', should be used in the
 * assembler program when a 32-bit variable (e.g., int) follows the string.
 */
char* stringToBinaryPadded(char* s, char* binaryString);



#endif
