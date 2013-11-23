//
//  Symbols.h
//  MIPS Assembler
//
//  Created by Navid Rojiani on 11/19/13.
//  Copyright (c) 2013 NKR. All rights reserved.
//

#ifndef MIPS_Assembler_Symbols_h
#define MIPS_Assembler_Symbols_h

#include <stdio.h>  // FILE
#include <stdbool.h>

#define NON_UNIFORM_ARRAY            1
#define UNIFORM_ARRAY                2
#define STRING                       3
#define VARIABLE                     4
#define MAX_DATA_SYMBOLS            20
#define MAX_TEXT_SYMBOLS            20
#define MAX_ARRAY_SZ                40
#define SYMBOL_NOT_FOUND            -1
#define WORD_SZ                     32

/*******************************************************************************
 *                      Text Symbol Functions
 *******************************************************************************/

struct TSymbol {
    int address;
    char* symbolName;
};
typedef struct TSymbol TSymbol;


/** Initialize a .text Symbol */
TSymbol* initTSymbol(TSymbol* pTSym, char* tSymName, int address);


/** Produce the .text symbol's hex address */
char* textSymHex(TSymbol* pTSym, char* hexStr);


/** Retrieves the address of the text symbol with the specified name.*/
int getTSymbolAddress(TSymbol TSymbols[], char* symbolName, int numTSymbols);


/** Prints all .text symbols and their hex addresses to a file */
void printTSymbolTable(TSymbol TSymbols[], int numTSymbols, FILE* outfp);


/*******************************************************************************
 *                      Data Symbol Functions
 *******************************************************************************/
/**
 * Represents a symbol from the .data portion of the assembly file. 
 * 
 * Members:
 * dataType - value indicating if the symbol is a variable (32-bit word),
 *            constructed array, uniform array, or ASCII string
 * symbolName - the label name (e.g., "message" or "array_size")
 * str - if the data type is string, contains the string.
 * value - For a variable, this is its value.
 *         For a uniform array, represents the initialization value for each
 *         index of the array.
 * arraySz - For a uniform array, it is the array length.
 * array - Contains the elements of the array.
 * All non-applicable members are NULL.
 */
 struct DataSymbol {
    int dataType;
    char* symbolName;
    char* str;
    int value;
    int arraySz;
    int array[MAX_ARRAY_SZ];
    int address;
};
typedef struct DataSymbol DSymbol;


/** Determine whether the .data symbol is a string, array, or 32-bit variable */
void assignDataType(DSymbol* pDSym, char* line);


/**
 * Parses from a line string the specific parts needed to initialize the
 * data symbol (which may be an array, variable, or string).
 */
void processDataLine(char* line, DSymbol* pDSym);


/**
 * Creates an array of DSymbol structs containing all data labels in the
 * .data section of the input file.
 * Returns the number of data elements added to the table.
 */
int createDSymbolTable(DSymbol DSymbols[], char** lines, int numLines);


/** Retrieves the address of the data symbol with the specified name.*/
int getDSymbolAddress(DSymbol DSymbols[], char* symbolName, int numDSymbols);


/** Sets all addresses for all Data Symbols in the data symbol table. */
void setDSymbolAddresses(DSymbol DSymbols[], int numDSymbols);


/** Produce the .data symbol's hex address */
char* dataSymHex(DSymbol* pDSym, char* hexStr);


/** Returns the machine code for a .data word variable */
void printVariableMCode(DSymbol* dSym, FILE* outfp);


/** Returns the machine code for a .data string */
void printStringMCode(DSymbol* dSym, FILE* outfp);


/** Returns the machine code for a .data uniform array */
void printUniArrayMCode(DSymbol* dSym, FILE* outfp);


/** Returns the machine code for a .data non-uniform array */
void printNonUniArrayMCode(DSymbol* dSym, FILE* outfp);


/** Prints all .data symbols and their hex addresses to a file */
void printDSymbolTable(DSymbol DSymbols[], int numDSymbols, FILE* outfp);


/** Prints the machine code of the .data section */
void printDataSection(DSymbol DSymbols[], int numDSymbols, FILE* outfp);


/** Prints a table of all symbols and their hex addresses */
void printSymbolsModeOutput(TSymbol TSymbols[], int numTSymbols,
                            DSymbol DSymbols[], int numDSymbols,
                            FILE* outfp);


#endif
