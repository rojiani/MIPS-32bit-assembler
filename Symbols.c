//
//  Symbols.c
//  MIPS Assembler
//
//  Created by Navid Rojiani on 11/19/13.
//  Copyright (c) 2013 NKR. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Parser.h"

#include "Utils.h"
#include "Symbols.h"


////////////////////// Text Symbol Functions //////////////////////////////////

TSymbol* initTSymbol(TSymbol* pTSym, char* tSymName, int address) {
    pTSym->address = address;
    pTSym->symbolName = tSymName;
    return pTSym;
}


int getTSymbolAddress(TSymbol TSymbols[], char* symbolName, int numTSymbols) {
    for (int i = 0; i < numTSymbols; i++) {
        if (strcmp(TSymbols[i].symbolName, symbolName) == 0) {
            return TSymbols[i].address;
        }
    }
    return SYMBOL_NOT_FOUND;
}


char* textSymHex(TSymbol* pTSym, char* hexStr) {
    decimalToHex(pTSym->address, hexStr);
    return hexStr;
}


void printTSymbolTable(TSymbol TSymbols[], int numTSymbols, FILE* outfp) {
    fprintf(outfp, "Address             Symbol\n");
    fprintf(outfp, "_______________________________\n");
    for (int i = 0; i < numTSymbols; i++) {
        char* hex = malloc(28 * sizeof(char) + 1);
        textSymHex(&TSymbols[i], hex);
        fprintf(outfp, "%s", hex);
        fprintf(outfp, "          %s\n", TSymbols[i].symbolName);
        free(hex);
    }
}


////////////////////// Data Symbol Functions //////////////////////////////////

int createDSymbolTable(DSymbol DSymbols[], char** lines, int numLines) {
    int i;
    int inDataSegment = 0;
    int numDSymbols = 0;
    for (i = 0; i < numLines; i++) {
        if (isDataDirective(lines[i])) {
            inDataSegment = 1;
            i++;
        }
        if (inDataSegment && !isBlankLine(lines[i])) {
            processDataLine(lines[i], &DSymbols[numDSymbols]);
            numDSymbols++;
        }
    }
    return numDSymbols;
}


void assignDataType(DSymbol* pDSym, char* line) {
    if (isNonUniformArray(line)) {
        pDSym->dataType = NON_UNIFORM_ARRAY;
    }
    else if (isUniformArray(line)) {
        pDSym->dataType = UNIFORM_ARRAY;
    }
    else if (isASCIIZ(line)) {
        pDSym->dataType = STRING;
    }
    else {
        pDSym->dataType = VARIABLE;
    }
}


void processDataLine(char* line, DSymbol* pDSym) {
    assignDataType(pDSym, line);
    pDSym->address = -1;
    char* aTok;
    char* tok;
    char* sTok;
    char* vTok;
    if (pDSym->dataType == UNIFORM_ARRAY) {
        pDSym->str = NULL;
        aTok = strtok(line, ":");
        if (aTok != NULL) {
            pDSym->symbolName = aTok;      // "symbolName"
        }
        aTok = strtok(NULL, " ");
        if (aTok != NULL) { }             // ".word" - can toss result
        aTok = strtok(NULL, ":");
        if (aTok != NULL) {
            pDSym->value = atoi(aTok);
        }
        aTok = strtok(NULL, "\n");
        if (aTok != NULL) {
            pDSym->arraySz = atoi(aTok);
        }
        // initialize array
        for (int i = 0; i < pDSym->arraySz; i++) {
            pDSym->array[i] = pDSym->value;
        }
    }
    else if (pDSym->dataType == NON_UNIFORM_ARRAY) {
        pDSym->str = NULL;
        tok = strtok(line, ":");         // TODO decompose
        if (tok != NULL) {
            pDSym->symbolName = tok;      // "symbolName"
        }
        tok = strtok(NULL, " ");
        if (tok != NULL) { };
        int i = 0;
        tok = strtok(NULL, ",\n");
        while (tok != NULL) {           // parse array index values delimited by commas
            pDSym->array[i] = atoi(tok);
            i++;
            tok = strtok(NULL, ",\n");
        }
        pDSym->arraySz = i;
    }
    else if (pDSym->dataType == STRING) {
        pDSym->value = -1;
        pDSym->arraySz = -1;
        sTok = strtok(line, ":");
        if (sTok != NULL) {
            pDSym->symbolName = sTok;        // "symbolName"
        }
        sTok = strtok(NULL, "\"");          // up to start of string
        if (sTok != NULL) { }   // skip
        sTok = strtok(NULL, "\"\n");          // String Message
        if (sTok != NULL) {
            pDSym->str = sTok;
        }
    }
    else {      // Variable
        pDSym->str = NULL;
        pDSym->arraySz = -1;
        vTok = strtok(line, ":");
        if (vTok != NULL) {
            pDSym->symbolName = vTok;         // "symbolName"
        }
        vTok = strtok(NULL, " ");
        if (vTok != NULL) { }                // ".word" -- skip (don't need to store)
        vTok = strtok(NULL, "\n");
        if (vTok != NULL) {
            pDSym->value = atoi(vTok);
        }
    }
}


void setDSymbolAddresses(DSymbol DSymbols[], int numDSymbols) {
    int PC = DATA_BASE_ADDRESS;
    for (int i = 0; i < numDSymbols; i++) {
        // if Variable
        if (DSymbols[i].dataType == VARIABLE) {
            DSymbols[i].address = PC;
            PC += 4;
        }
        // Arrays: 4 * array length
        else if (DSymbols[i].dataType == UNIFORM_ARRAY || DSymbols[i].dataType == NON_UNIFORM_ARRAY) {
            DSymbols[i].address = PC;
            PC += (4 * DSymbols[i].arraySz);
        }
        // Strings: strlen(string) + 1 if followed by a string;
        //          strlen(string) + [pad up to 4] if followed by a variable or array
        else {
            DSymbols[i].address = PC;
            int sLen = (int) strlen(DSymbols[i].str) + 1;
            if (i + 1 < numDSymbols && DSymbols[i + 1].dataType != STRING) {
                int r = sLen % 4;
                if (r != 0) {
                    PC += (sLen + (4 - r));
                }
                else {
                    PC += sLen;
                }
            }
            else {
                PC += sLen;
            }
        }
    }
}


int getDSymbolAddress(DSymbol DSymbols[], char* symbolName, int numDSymbols) {
    for (int i = 0; i < numDSymbols; i++) {
        if (strcmp(DSymbols[i].symbolName, symbolName) == 0) {
            return DSymbols[i].address;
        }
    }
    return SYMBOL_NOT_FOUND;
}


char* dataSymHex(DSymbol* pDSym, char* hexStr) {
    decimalToHex(pDSym->address, hexStr);
    return hexStr;
}


void printVariableMCode(DSymbol* dSym, FILE* outfp) {
    char* mCode = malloc(WORD_SZ * sizeof(char) + 1);
    if (mCode == NULL) {
        printf("Error allocating memory\n");
        exit(EXIT_FAILURE);
    }
    mCode = intToBinary(dSym->value, 32, mCode);
    printf("%s\n", mCode);
    fprintf(outfp, "%s\n", mCode);
    free(mCode);
}


int printStringMCodeUnpadded(DSymbol* dSym, FILE* outfp, int bitOffset) {
    int sLen = (int) strlen(dSym->str);
    int nBits = sLen * 8 + 8;      // 8 bits per char, 8 for '\0'
    char* mCode = malloc(nBits + 1);
    if (mCode == NULL) {
        printf("Error allocating memory\n");
        exit(EXIT_FAILURE);
    }
    mCode = stringToBinary(dSym->str, mCode);
    for (int i = 0; i < nBits; i++) {
        if (i != 0 && bitOffset % 32 == 0) { // print new line characters every word
            printf("\n");
            fprintf(outfp, "\n");
            bitOffset = 0;   // now back at 0
        }
        printf("%c", *(mCode + i));         // print one bit at a time
        fprintf(outfp, "%c", *(mCode + i));
        bitOffset++;
    }
    free(mCode);
    return bitOffset;
}
 
 
void printStringMCodePadded(DSymbol* dSym, FILE* outfp, int bitOffset) {
    int sLen = (int) strlen(dSym->str);
    // number of zero bytes (including \0 representation):
    int zeroBytes = 4 - (sLen % 4);
    // size needed will be 8 bits per char, + 8 to represent the null terminator,
    // + 8 per padding byte, and one for the null terminator for the string
    // representing the string...
    int nBits = (sLen + zeroBytes) * 8;


    // NEED TO FIX: stringToBinaryPadded places "remainder" chars with padding
    // bytes afterwards, not before



    char* mCode = malloc(nBits * sizeof(char) + 1);
    if (mCode == NULL) {
        printf("Error allocating memory\n");
        exit(EXIT_FAILURE);
    }
    mCode = stringToBinaryPadded(dSym->str, mCode);
    for (int i = 0; i < nBits; i++) {
        if (i != 0 && bitOffset % 32 == 0) { // print new line characters every word
            printf("\n");
            fprintf(outfp, "\n");
            bitOffset = 0;   // now back at 0
        }
        printf("%c", *(mCode + i));         // print one bit at a time
        fprintf(outfp, "%c", *(mCode + i));
        bitOffset++;
    }
    printf("\n");
    fprintf(outfp, "\n");
    free(mCode);
}



void printUniArrayMCode(DSymbol* dSym, FILE* outfp) {
    char* mCode = malloc(WORD_SZ * sizeof(char) + 1);
    if (mCode == NULL) {
        printf("Error allocating memory\n");
        exit(EXIT_FAILURE);
    }
    mCode = intToBinary(dSym->value, 32, mCode);
    for (int i = 0; i < dSym->arraySz; i++) {
        printf("%s\n", mCode);
        fprintf(outfp, "%s\n", mCode);
    }
    free(mCode);
}


void printNonUniArrayMCode(DSymbol* dSym, FILE* outfp) {
    char* mCode;
    mCode = malloc(WORD_SZ * sizeof(char) + 1);
    if (mCode == NULL) {
        printf("Error allocating memory\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < dSym->arraySz; i++) {
        mCode = intToBinary(dSym->array[i], 32, mCode);
        printf("%s\n", mCode);
        fprintf(outfp, "%s\n", mCode);
    }
    free(mCode);
}

    
void printDSymbolTable(DSymbol DSymbols[], int numDSymbols, FILE* outfp) {
    for (int i = 0; i < numDSymbols; i++) {
        char* hex = malloc(28 * sizeof(char) + 1);
        dataSymHex(&DSymbols[i], hex);
        fprintf(outfp, "%s", hex);
        fprintf(outfp, "          %s\n", DSymbols[i].symbolName);
        free(hex);
    }
}


/** Prints the machine code of the .data section */
void printDataSection(DSymbol DSymbols[], int numDSymbols, FILE* outfp) {
    printf("PRINTING DATA SECTION: \n");
    int bitOffset = 0;
    for (int i = 0; i < numDSymbols; i++) {
        if (DSymbols[i].dataType == VARIABLE) {
            printf("\nPrinting: %s\n", DSymbols[i].symbolName);
            printVariableMCode(&DSymbols[i], outfp);
            bitOffset = 0;

        }
        else if (DSymbols[i].dataType == UNIFORM_ARRAY) {
            printf("\nPrinting: %s\n", DSymbols[i].symbolName);
            printUniArrayMCode(&DSymbols[i], outfp);
            bitOffset = 0;

        }
        else if (DSymbols[i].dataType == NON_UNIFORM_ARRAY) {
            printf("\nPrinting: %s\n", DSymbols[i].symbolName);
            printNonUniArrayMCode(&DSymbols[i], outfp);
            bitOffset = 0;

        }
        // Variables and arrays both always end on word offsets, but strings must be padded with
        // zero bytes if either they precede a non-string data element, if they are the sole item
        // in the .data segment, or they are the final item.
        else { //if (DSymbols[i].dataType == STRING) {
            printf("\nPrinting: %s\n", DSymbols[i].symbolName);
            if ( numDSymbols == 1 || (i + 1 == numDSymbols) ||
                (i + 1 < numDSymbols && DSymbols[i + 1].dataType != STRING) ){
                printf("\nString in data is: %s\n", DSymbols[i].str);
                printf("\nPrinting (PADDED): %s\n", DSymbols[i].symbolName);
                printStringMCodePadded(&DSymbols[i], outfp, bitOffset);
                bitOffset = 0;
            }
            else {
                printf("\nString in data is: %s\n", DSymbols[i].str);
                printf("\nPrinting (UNPADDED): %s\n", DSymbols[i].symbolName);
                bitOffset = printStringMCodeUnpadded(&DSymbols[i], outfp, bitOffset);
            }
        }
    }
}


void printSymbolsModeOutput(TSymbol TSymbols[], int numTSymbols,
                            DSymbol DSymbols[], int numDSymbols, FILE* outfp) {
    printTSymbolTable(TSymbols, numTSymbols, outfp);
    printDSymbolTable(DSymbols, numDSymbols, outfp);
}


