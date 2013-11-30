//
//  main.c
//  MIPS Assembler
//
//  Created by Navid Rojiani on 11/15/13.
//  Copyright (c) 2013 NKR. All rights reserved.
//

//   On my honor:
//
// - I have not discussed the C language code in my program with anyone other
//   than my instructor or the teaching assistants assigned to this course.
//
// - I have not used C language code obtained from another student, or any other
//   unauthorized source, either modified or unmodified.
//
// - If any C language code or documentation used in my program
//   was obtained from another source, such as a text book or course notes, that
//   has been clearly noted with a proper citation in the comments of my
//   program.
//
//    Navid K. Rojiani


#define STD_MODE        1
#define SYMBOLS_MODE    2
#define LIST_MODE       3


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "Instructions.h"
#include "Parser.h"
#include "Symbols.h"
#include "Utils.h"



void printInvocError(void);


/**
 * TODO
 *
 */
int main(int argc, const char* argv[]) {
    int mode = STD_MODE;

    int numLines;           // Number of lines in the input file
    int numTSymbols;        // Number of symbols (labels) in .text section
    int numDSymbols;        // Number of symbols (labels) in .data section
    FILE* infp =    NULL;   // File pointer for input file
    FILE* outfp =   NULL;   // File pointer for output file
    FILE* temp1fp = NULL;   // File pointer for first parsing pass
    FILE* temp2fp = NULL;   // File pointer for second parsing pass
    FILE* temp3fp = NULL;   // File pointer for third parsing pass
    


    // VALIDATE CLAs
    if (strcmp(argv[0], "assemble") != 0 && strcmp(argv[0], "./assemble") != 0) {
        printf("debug\n");
        printInvocError();
        exit(EXIT_FAILURE);
    }
    
    // DETERMINE INVOCATION MODE
    if (argc == 3) {
        infp = fopen(argv[1], "r");
        if (infp == NULL) {
            printf("Invalid input file.\n");
            exit(EXIT_FAILURE);
        }
        outfp = fopen(argv[2], "w");
    }
    else if (argc == 4) {
        if (strcmp(argv[1], "-symbols") == 0) {
            mode = SYMBOLS_MODE;
        }
        if (strcmp(argv[1], "-list") == 0) {
            mode = LIST_MODE;
        }
        infp = fopen(argv[2], "r");
        if (infp == NULL) {
            printf("Invalid input file.\n");
            exit(EXIT_FAILURE);
        }
        outfp = fopen(argv[3], "w");
    }
    else {
        printInvocError();
        exit(EXIT_FAILURE);
    }



    if (mode == LIST_MODE) {
        printf("-list mode not implemented yet.\n");
        exit(EXIT_FAILURE);
        fclose(outfp);
    }
    
    
    // PARSER PASS 1 - Remove Comments.
    temp1fp = fopen("temp1.txt", "w");
    temp1fp = parseComments(infp, temp1fp);
    fclose(infp);
    
    
    // PARSER PASS 2 - Trim Whitespace.
    fclose(temp1fp);    // close & reopen as read-only
    temp1fp = fopen("temp1.txt", "r");
    temp2fp = fopen("temp2.txt", "w");
    temp2fp = trimWhiteSpace(temp1fp, temp2fp);
    fclose(temp1fp);
    fclose(temp2fp);
    
    
    // PARSER PASS 3 - Store each line in an array
    temp3fp = fopen("temp2.txt", "r");
    char* lines[MAX_LINES];
    numLines = createLineTable(temp3fp, lines);
    fclose(temp3fp);
    
    
    // Initialize PC
    int PC = TEXT_BASE_ADDRESS;
    
    
    // NOTE: make .text section only, change name
    // Create Text Symbols Table (any mode)
    TSymbol TSymbols[MAX_TEXT_SYMBOLS];                // ASSUMING <= 20 LABELS
    numTSymbols = 0;
    char* tSymbol;
    const char delim[2] = ":";
    for (int i = 0; i < numLines; i++) {
        if (isDataDirective(lines[i])) {
            PC = DATA_BASE_ADDRESS;
            break;
        }
        if (!isBlankLine(lines[i])) {
            if(isSymbol(lines[i])) {
                TSymbol* pTSym = malloc(sizeof(TSymbol));
                if (pTSym == NULL) {
                    printf("Error allocating memory. Exiting.\n");
                    exit(EXIT_FAILURE);
                }
                tSymbol = strtok(lines[i], delim);
                pTSym = initTSymbol(pTSym, tSymbol, PC);
                TSymbols[numTSymbols] = *pTSym;
                numTSymbols++;
                free(pTSym);             // affect table?
            }
            else if (!isDirective(lines[i])) {
                PC += 4;
            }
        }
    }
    
    
    // CREATE DATA TABLE (stores .data elements needed by instructions or for output)
    DSymbol DSymbols[MAX_DATA_SYMBOLS];
    numDSymbols = createDSymbolTable(DSymbols, lines, numLines);
    setDSymbolAddresses(DSymbols, numDSymbols);


    if (mode == SYMBOLS_MODE) {
        printSymbolsModeOutput(TSymbols, numTSymbols, DSymbols, numDSymbols, outfp);
        fclose(outfp);
        return(0);
    }


    PC = TEXT_BASE_ADDRESS;
    // PROCESS EACH INSTRUCTION: Parse mnemonic and operands, determine type, initialize its
    // data, and print its machine code to the output file.
    int i = 0;  // current line
    char* tok;
    char* mnem;
    char* op1;
    char* op2;
    char* op3;
    // scroll to the first instruction
    while (isBlankLine(lines[i]) || isDirective(lines[i]) || 
           isSymbol(lines[i])) {  i++;  }
    printf("----------------------PRINTING INSTRUCTIONS----------------------\n\n");
    for (; i < numLines; i++) {
        printf("\n-------------------------------------------\n");
        printf("NEXT INSTRUCTION OR LINE:\n");
        printf("Line: %s", lines[i]);
        if (isDataDirective(lines[i])) { break; }   // stop when .data segment reached
        if (isBlankLine(lines[i])) { continue; }
        // allocate memory for the binary string for the current instruction
        printf("PC = %d\n", PC);
        /**
        char* mCode32 = malloc(MACHINE_CODE_LEN + 1);
        if (mCode32 == NULL) {
            printf("Could not allocate memory for Machine Code String\n");
            exit(EXIT_FAILURE);
        }
        */
        char mCode[] = "77777777777777777777777777777777";
        char* mCode32 = mCode;
        tok = strtok(lines[i], " \n");
        mnem = tok;
        printf("mnem: %s\n", tok);
        // PROCESS R-TYPE INSTRUCTION
        if (isRType(mnem)) {
            R* pR = malloc(sizeof(R));
            if (pR == NULL) {
                printf("Could not allocate memory for R-Type\n");
                exit(EXIT_FAILURE);
            }
            if (isR1(mnem)) {    // R1: [mnem] rd, rs, rt
                tok = strtok(NULL, " ,\n");
                op1 = tok;
                tok = strtok(NULL, " ,\n");
                op2 = tok;
                tok = strtok(NULL, " ,\n");
                op3 = tok;
                printf("operands:\nop1: %s\nop2: %s\nop3: %s\n", op1, op2, op3);
                if (op1 == NULL || op2 == NULL || op3 == NULL) {
                    printf("Error parsing R1 operands\n");
                }
                pR = initRType(pR, mnem, op1, op2, op3, PC);
                mCode32 = RMachineCode(pR, mCode32);
                printf("R1 MACHINE CODE:\n");
                printf("|___| %s |__|\n", mCode32);
                fprintf(outfp, "%s\n", mCode32);
                free(pR);
            }
            else if (isR2(mnem)) {  // R2: [mnem] rd, rt, sa
                tok = strtok(NULL, " ,\n");
                op1 = tok;
                tok = strtok(NULL, " ,\n");
                op2 = tok;
                tok = strtok(NULL, " ,\n");
                op3 = tok;
                printf("operands:\nop1: %s\nop2: %s\nop3: %s\n", op1, op2, op3);
                if (op1 == NULL || op2 == NULL || op3 == NULL) {
                    printf("Error parsing R2 operands\n");
                }
                pR = initRType(pR, mnem, op1, op2, op3, PC);
                mCode32 = RMachineCode(pR, mCode32);
                printf("R2 MACHINE CODE:\n");
                printf("|___| %s |__|\n", mCode32);
                fprintf(outfp, "%s\n", mCode32);
                free(pR);

            }
            else if (isNOP(mnem)) { 
                pR = initNOP(pR, PC); 
                mCode32 = RMachineCode(pR, mCode32);
                printf("NOP MACHINE CODE:\n");
                printf("|___| %s |__|\n", mCode32);
                fprintf(outfp, "%s\n", mCode32);
                free(pR);
            }
            else if (isSYSCALL(mnem)) { 
                pR = initSYSCALL(pR, PC); 
                mCode32 = RMachineCode(pR, mCode32);
                printf("SYSCALL MACHINE CODE:\n");
                printf("|___| %s |__|\n", mCode32);
                fprintf(outfp, "%s\n", mCode32);
                free(pR);
            }
            else if (isJR(mnem)) {     // [jr] rs
                tok = strtok(NULL, " \n");
                op1 = tok;
                if (op1 == NULL) { printf("Error parsing R2 operands\n"); }
                printf("operand:\n op1: %s\n", op1);
                initJR(pR, op1, PC);
                mCode32 = RMachineCode(pR, mCode32);
                printf("JR MACHINE CODE:\n");
                printf("|___| %s |__|\n", mCode32);
                fprintf(outfp, "%s\n", mCode32);
                free(pR);
            }
        }
        
        // PROCESS I-TYPE INSTRUCTION
        else if (isIType(mnem)) {
            I* pI = malloc(sizeof(I));
            char* imm16 = malloc(IMM_LEN + 1);
            if (pI == NULL || imm16 == NULL) {
                printf("Could not allocate memory for I-Type\n");
                exit(EXIT_FAILURE);
            }
            if (isI1(mnem)) {       // [mnem] rt, offset(rs)
                tok = strtok(NULL, " ,\n");
                op1 = tok;
                tok = strtok(NULL, " \n");
                op2 = tok;
                printf("operands:\nop1: %s\nop2: %s\n", op1, op2);
                if (op1 == NULL || op2 == NULL) {
                    printf("Error parsing I1 operands\n");
                }
                
                initIType(pI, imm16, mnem, op1, op2, NULL, PC, TSymbols, numTSymbols);
                mCode32 = IMachineCode(pI, mCode32);
                printf("I1 MACHINE CODE:\n");
                printf("|___| %s |__|\n", mCode32);
                fprintf(outfp, "%s\n", mCode32);
                free(pI);
                free(imm16);
            }
            else if (isI2(mnem)) {  // [mnem] rt, rs, imm16
                tok = strtok(NULL, " ,\n");
                op1 = tok;
                tok = strtok(NULL, " ,\n");
                op2 = tok;
                tok = strtok(NULL, " ,\n");
                op3 = tok;
                printf("operands:\nop1: %s\nop2: %s\nop3: %s\n", op1, op2, op3);
                if (op1 == NULL || op2 == NULL || op3 == NULL) {
                    printf("Error parsing I2 operands\n");
                }
                initIType(pI, imm16, mnem, op1, op2, op3, PC, TSymbols, numTSymbols);
                mCode32 = IMachineCode(pI, mCode32);
                printf("I2 MACHINE CODE:\n");
                printf("|___| %s |__|\n", mCode32);
                fprintf(outfp, "%s\n", mCode32);
                free(pI);
                free(imm16);
            }
            else if (isI3(mnem)) {  // [mnem] rs, rt, offset
                tok = strtok(NULL, " ,\n");
                op1 = tok;
                tok = strtok(NULL, " ,\n");
                op2 = tok;
                tok = strtok(NULL, " ,\n");
                op3 = tok;
                printf("operands:\nop1: %s\nop2: %s\nop3: %s\n", op1, op2, op3);
                if (op1 == NULL || op2 == NULL || op3 == NULL) {
                    printf("Error parsing I3 operands\n");
                }
                initIType(pI, imm16, mnem, op1, op2, op3, PC, TSymbols, numTSymbols);
                mCode32 = IMachineCode(pI, mCode32);
                printf("I3 MACHINE CODE:\n");
                printf("|___| %s |__|\n", mCode32);
                fprintf(outfp, "%s\n", mCode32);
                free(pI);
                free(imm16);
            }
            else if (isI4(mnem)) {  // [mnem] rs, offset
                tok = strtok(NULL, " ,\n");
                op1 = tok;
                tok = strtok(NULL, " ,\n");
                op2 = tok;
                printf("operands:\nop1: %s\nop2: %s\n", op1, op2);
                if (op1 == NULL || op2 == NULL) {
                    printf("Error parsing I4 operands\n");
                }
                initIType(pI, imm16, mnem, op1, op2, NULL, PC, TSymbols, numTSymbols);
                mCode32 = IMachineCode(pI, mCode32);
                printf("I4 MACHINE CODE:\n");
                printf("|___| %s |__|\n", mCode32);
                fprintf(outfp, "%s\n", mCode32);
                free(pI);
                free(imm16);
            }
            /**
            // print I-type instruction machine code
            mCode32 = IMachineCode(pI, mCode32);
            printf("I MACHINE CODE:\n");
            printf("|___| %s |__|\n", mCode32);
            fprintf(outfp, "%s\n", mCode32);
            free(pI);
            free(imm16);
            */
        }
        
        // PROCESS J-TYPE INSTRUCTION
        else if (isJType(mnem)) {
            tok = strtok(NULL, " \n");
            op1 = tok;
            printf("operand:\nop1: %s\n", op1);
            if (op1 == NULL) { printf("Error parsing J operands\n"); }
            
            J* pJ = malloc(sizeof(R));
            char* instr_idx = malloc(JUMP_INSTR_IDX_LEN + 1);
            if (pJ == NULL || instr_idx == NULL) {
                printf("Could not allocate memory for J-Type\n");
                exit(EXIT_FAILURE);
            }
            
            initJType(pJ, instr_idx, mnem, op1, PC, TSymbols, numTSymbols);
            mCode32 = JMachineCode(pJ, mCode32);
            printf("J MACHINE CODE:\n");
            printf("|___| %s |__|\n", mCode32);
            fprintf(outfp, "%s\n", mCode32);
            //free(instr_idx);  IF FREED: DOUBLE-FREE/CORRUPTION ERROR...WHY?
            free(pJ);
        }

        // PROCESS PSEUDO-INSTRUCTION
        else if (isPseudo(mnem)) {
            if (isLX(mnem)) {
                LX* pLX = malloc(sizeof(LX));
                char* imm16 = malloc(IMM_LEN + 1);
                if (pLX == NULL || imm16 == NULL) {
                    printf("Could not allocate memory for la or li\n");
                    exit(EXIT_FAILURE);
                }
                tok = strtok(NULL, " ,\n");
                op1 = tok;
                tok = strtok(NULL, " ,\n");
                op2 = tok;
                tok = strtok(NULL, " ,\n");
                op3 = tok;
                printf("operands:\nop1: %s\nop2: %s\nop3: %s\n", op1, op2, op3);
                if (op1 == NULL || op2 == NULL) {
                    printf("Error parsing LA or LI operands\n");
                }
                initLX(pLX, imm16, mnem, op1, op2, PC, DSymbols, numDSymbols);
                mCode32 = LXMachineCode(pLX, mCode32);

                printf("LX MACHINE CODE:\n");
                printf("|___| %s |__|\n", mCode32);

                fprintf(outfp, "%s\n", mCode32);
                free(imm16);
                free(pLX);
            }
            else if (isBLX(mnem)) {
                BLX* pBLX = malloc(sizeof(BLX));
                char* mCode64 = malloc(BLX_MACHINE_CODE_LEN + 1);
                char* branchStr = malloc(IMM_LEN + 1);
                if (pBLX == NULL || mCode64 == NULL) {
                    printf("Could not allocate memory for blt or ble\n");
                    exit(EXIT_FAILURE);
                }
                tok = strtok(NULL, " ,\n");
                op1 = tok;
                tok = strtok(NULL, " ,\n");
                op2 = tok;
                tok = strtok(NULL, " ,\n");
                op3 = tok;
                printf("operands:\nop1: %s\nop2: %s\nop3: %s\n", op1, op2, op3);
                if (op1 == NULL || op2 == NULL || op3 == NULL) {
                    printf("Error parsing BLX operands\n");
                }
                initBLX(pBLX, branchStr, mnem, op1, op2, op3, PC, TSymbols, numTSymbols);
                mCode64 = BLXMachineCode(pBLX, mCode64);

                printf("BLX MACHINE CODE STRING IS:\n");
                printf("|__%s__|\n\n\n", mCode64);
                
                // print new line every 32 bits
                int bitIdx = 0;
                for (int j = 0; j < 64; j++) {
                    if (bitIdx != 0 && bitIdx % 32 == 0) { 
                        printf("\n");
                        fprintf(outfp, "\n");
                        bitIdx = 0;
                    } 
                    printf("%c", *(mCode64 + j));     // print one bit at a time
                    fprintf(outfp, "%c", *(mCode64 + j));
                    bitIdx++;
                }
                fprintf(outfp, "\n");
                free(pBLX);
                free(branchStr);
                free(mCode64);
            }
        }
        //free(mCode32);
        if (!isValidInstr(mnem)) { continue; }; // skip PC increment
        printf("\nINCREMENTING PC\n");
        PC += 4;
        printf("-------------------------------------------\n");
    }
    printf("------------------------------------------------------------------\n\n");
    fprintf(outfp, "\n");
    printDataSection(DSymbols, numDSymbols, outfp);
    fclose(outfp);
    return 0;
}



void printInvocError(void) {
    printf("\nInvalid command line arguments. Please use invocation\n"
           "assemble <input file> <output file>, or \n"
           "assemble -symbols <input file> <output file>, or "
           "assemble -list <input file> <output file>\n\n");
}
