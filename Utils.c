//
//  Utils.c
//  MIPS Assembler
//
//  Created by Navid Rojiani on 11/14/13.
//  Copyright (c) 2013 NKR. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "Utils.h"

// Note: before calling stringToBinary, check if padding need (if 32-bit variable
// follows it) --> if so call stringToBinaryPadded


char* intToBinary(int iVal, int numBits, char* s) {
    int isoBit, ct;
    ct = 0;
    for (int i = numBits; i > 0 ; i--) {
        isoBit = iVal >> (i - 1);
        if (isoBit & 1) {
            *(s + ct) = '1';
        }
        else {
            *(s + ct) = '0';
        }
        ct++;
    }
    *(s + ct) = '\0';
    return s;
}


char* decimalToHex(int decVal, char* hexStr) {
    char hex[] = "XXXX";
    char* binary = intToBinary(decVal, 32, hexStr);
    for (int c = 0; c < 32; c += 4) {
        strncpy(hex, binary + c, 4);
        if      (strncmp(hex, "0000", 4) == 0)   *(hexStr + c/4 + 2) = '0';
        else if (strncmp(hex, "0001", 4) == 0)   *(hexStr + c/4 + 2) = '1';
        else if (strncmp(hex, "0010", 4) == 0)   *(hexStr + c/4 + 2) = '2';
        else if (strncmp(hex, "0011", 4) == 0)   *(hexStr + c/4 + 2) = '3';
        else if (strncmp(hex, "0100", 4) == 0)   *(hexStr + c/4 + 2) = '4';
        else if (strncmp(hex, "0101", 4) == 0)   *(hexStr + c/4 + 2) = '5';
        else if (strncmp(hex, "0110", 4) == 0)   *(hexStr + c/4 + 2) = '6';
        else if (strncmp(hex, "0111", 4) == 0)   *(hexStr + c/4 + 2) = '7';
        else if (strncmp(hex, "1000", 4) == 0)   *(hexStr + c/4 + 2) = '8';
        else if (strncmp(hex, "1001", 4) == 0)   *(hexStr + c/4 + 2) = '9';
        else if (strncmp(hex, "1010", 4) == 0)   *(hexStr + c/4 + 2) = 'A';
        else if (strncmp(hex, "1011", 4) == 0)   *(hexStr + c/4 + 2) = 'B';
        else if (strncmp(hex, "1100", 4) == 0)   *(hexStr + c/4 + 2) = 'C';
        else if (strncmp(hex, "1101", 4) == 0)   *(hexStr + c/4 + 2) = 'D';
        else if (strncmp(hex, "1110", 4) == 0)   *(hexStr + c/4 + 2) = 'E';
        else                                      *(hexStr + c/4 + 2) = 'F';
    }
    *(hexStr) = '0';
    *(hexStr + 1) = 'x';
    *(hexStr + 10) = '\0';
    return hexStr;
}


void swapChars(char* s, int c1idx, int c2idx) {
    char temp = *(s + c2idx);
    *(s + c2idx) = *(s + c1idx);
    *(s + c1idx) = temp;
}


void charToBinary(char c, int charIdx, char* binaryStr) {
    int isoBit, count;
    int chVal = c;
    int binaryStrPos = 8 * charIdx;
    count = 0;
    for (int i = 8; i > 0 ; i--) {
        isoBit = chVal >> (i - 1);
        if (isoBit & 1) {
            *(binaryStr + binaryStrPos) = '1';
        }
        else {
            *(binaryStr + binaryStrPos) = '0';
        }
        count++;
        binaryStrPos++;
    }
}


// converts four chars to a binary string of length 32 + 1
// NOTE: does not rearrange. must call stringBigEndian first to rearrange the
// string as described in the spec. The function stringToBinary calls both.
void stringToBinaryHelper(char* s, char* binaryStr) {
    int nChars = (int) strlen(s);
    for (int i = 0; i < nChars; i++) {
        charToBinary(s[i], i, binaryStr);
    }
}



// Does not include padding if followed by 32-bit variable.
void stringToBigEndian(char* s) {
    int sLen = (int) strlen(s);
    // # chars in last 4-byte group (not including \0)
    int k = sLen % 4;
    int i;
    for (i = 0; i < sLen - k; i += 4) {
        swapChars(s, i, i + 3); // swap first & last in word block
        swapChars(s, i + 1, i + 2); // swap middle 2 chars in word block
    }
    // if not evenly divided into 4-byte blocks, reverse whatever is left
    if (k == 2) swapChars(s, i, i + 1);
    if (k == 3) swapChars(s, i, i + 2);
}


char* stringToBinary(char* s, char* binaryString) {
    stringToBigEndian(s);
    stringToBinaryHelper(s, binaryString);
    charToBinary('\0', (int)strlen(s), binaryString);     // Represents String Terminator
    *(binaryString + strlen(s) * 8 + 8) = '\0';      // Actual String Terminator
    return binaryString;
}


char* stringToBinaryPadded(char* s, char* binaryString) {
    int numChars = (int) strlen(s);
    int k = numChars % 4;
    stringToBigEndian(s);
    stringToBinaryHelper(s, binaryString);
    if (k == 0) {
        char zeroX4[] = "00000000000000000000000000000000";
        strncat(binaryString, zeroX4, 32);
    }
    else if (k == 1) {
        charToBinary('\0', numChars, binaryString);
        charToBinary('\0', numChars + 1, binaryString);
        charToBinary('\0', numChars + 2, binaryString);
    }
    else if (k == 2) {
        charToBinary('\0', numChars, binaryString);
        charToBinary('\0', numChars + 1, binaryString);
    }
    else if (k == 3) {
        charToBinary('\0', numChars, binaryString);
    }
    return binaryString;
}