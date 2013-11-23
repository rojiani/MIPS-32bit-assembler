//
//  Instructions.c
//  MIPS Assembler
//
//  Created by Navid Rojiani on 11/17/13.
//  Copyright (c) 2013 NKR. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Instructions.h"
#include "LookupTables.h"
#include "Symbols.h"
#include "Utils.h"




bool isValidInstr(char* mnem) {
    return (isRType(mnem) || isIType(mnem) || isJType(mnem) || isPseudo(mnem));
}



/***********************************************************************
 *                      R-TYPE INSTRUCTIONS
 ***********************************************************************/
bool isR1(char* mnem) {
    return ((strcmp(mnem, "add") == 0)  ||
            (strcmp(mnem, "sub") == 0)  ||
            (strcmp(mnem, "or") == 0)   ||
            (strcmp(mnem, "and") == 0)  ||
            (strcmp(mnem, "slt") == 0));
}


bool isR2(char* mnem) {
    return ((strcmp(mnem, "sll") == 0) ||
            (strcmp(mnem, "srl") == 0) ||
            (strcmp(mnem, "sra") == 0));
}


bool isSYSCALL(char* mnem) {
    return (strcmp(mnem, "syscall") == 0);
}


bool isNOP(char* mnem) {
    return (strcmp(mnem, "nop") == 0);
}


bool isJR(char* mnem) {
    return (strcmp(mnem, "jr") == 0);
}


bool isRType(char* mnem) {
    return (isR1(mnem) || isR2(mnem) || isNOP(mnem) || isSYSCALL(mnem) || isJR(mnem));
}


R* initRType(R* pR, char* mnem, char* op1, char* op2, char* op3, int address) {
    if (!isRType(mnem)) {
        return NULL;
    }
    strcpy(pR->opcode, getOpCode(mnem));
    strcpy(pR->funct, getFunct(mnem));
    pR->address = address;
    if (isR1(mnem)) {
        strcpy(pR->shamt, "00000");
        strcpy(pR->rs, getRegID(op2));
        strcpy(pR->rt, getRegID(op3));
        strcpy(pR->rd, getRegID(op1));
    }
    if (isR2(mnem)) {
        strcpy(pR->rs, "00000");
        strcpy(pR->rt, getRegID(op2));
        strcpy(pR->rd, getRegID(op1));
        int sa = atoi(op3);
        char* shift = malloc(5 * sizeof(char) + 1);
        if (shift == NULL) {
            printf("Unable to allocate memory (initRType)\n");
            exit(EXIT_FAILURE);
        }
        shift = intToBinary(sa, 5, shift);
        strcpy(pR->shamt, shift);
        free(shift);
    }
    return pR;
}


R* initNOP(R* pNOP, int address) {
    pNOP->address = address;
    strcpy(pNOP->opcode, "000000");
    strcpy(pNOP->rs,     "00000");
    strcpy(pNOP->rt,     "00000");
    strcpy(pNOP->rd,     "00000");
    strcpy(pNOP->shamt,  "00000");
    strcpy(pNOP->funct,  "000000");
    return pNOP;
}


R* initSYSCALL(R* pSYS, int address) {
    pSYS->address = address;
    strcpy(pSYS->opcode, "000000");
    strcpy(pSYS->rs,     "00000");
    strcpy(pSYS->rt,     "00000");
    strcpy(pSYS->rd,     "00000");
    strcpy(pSYS->shamt,  "00000");
    strcpy(pSYS->funct,  "001100");
    return pSYS;
}


R* initJR(R* pJR, char* rs, int address) {
    pJR->address = address;
    strcpy(pJR->opcode, "000000");
    strcpy(pJR->rs, getRegID(rs));
    strcpy(pJR->rt,     "00000");
    strcpy(pJR->rd,     "00000");
    strcpy(pJR->shamt,  "00000");
    strcpy(pJR->funct,  "001000");
    return pJR;
}


char* RMachineCode(R* pR, char* mCode32) {
    memmove(mCode32, pR->opcode, 6);
    memmove(mCode32 + 6, pR->rs, 5);
    memmove(mCode32 + 11, pR->rt, 5);
    memmove(mCode32 + 16, pR->rd, 5);
    memmove(mCode32 + 21, pR->shamt, 5);
    memmove(mCode32 + 26, pR->funct, 6);
    *(mCode32 + 32) = '\0';
    return mCode32;
}


/***********************************************************************
 *                      I-TYPE INSTRUCTIONS
 ***********************************************************************/


bool isI1(char* mnem) {
    return ((strcmp(mnem, "lw") == 0) ||
             (strcmp(mnem, "sw") == 0));
}

bool isI2(char* mnem) {
    return ((strcmp(mnem, "addi") == 0)  ||
            (strcmp(mnem, "addiu") == 0) ||
            (strcmp(mnem, "ori") == 0)   ||
            (strcmp(mnem, "andi") == 0)  ||
            (strcmp(mnem, "slti") == 0));
}


bool isI3(char* mnem) {
    return ((strcmp(mnem, "beq") == 0) || 
            (strcmp(mnem, "bne") == 0));
}


bool isI4(char* mnem) {
    return ((strcmp(mnem, "bltz") == 0) || (strcmp(mnem, "blez") == 0));
}


bool isIType(char* mnem) {
    return (isI1(mnem) || isI2(mnem) || isI3(mnem) || isI4(mnem));
}


bool hasImmOffset(char* str) {
    while (*str++) {
        if (*str == '(') {
            return true;
        }
    }
    return false;
}


char* getBranchOffset(int PC, int target, char* offsetStr) {
    int iBranch = (target - (PC + 4)) / 4;
    offsetStr = intToBinary(iBranch, 16, offsetStr);
    return offsetStr;
}


//////// TODO - break into smaller parts
// op3 null for I1, I4
I* initIType(I* pI, char* imm16, char* mnem, char* op1, char* op2, char* op3, int address,
             TSymbol TSymbols[], int numTSymbols) {
    if (!isIType(mnem)) { return NULL; }
    // Allocate memory for struct
    // Allocate memory for binary string for the immediate/offset
    pI->address = address;
    strcpy(pI->opcode, getOpCode(mnem));
    // I1: [mnem] rt, offset(rs)
    if (isI1(mnem)) {
        // check if has offset: if 0 may be in form [lw/sw] rt, rs
        strcpy(pI->rt, getRegID(op1));
        if (!hasImmOffset(op2)) {
            strcpy(pI->rs, getRegID(op2));
        }
        // if regular form [lw/sw] rt, offset(rs), must parse offset.
        // get digits before opening parenthesis = offset
        char* op2OrigStart = op2;
        int i = 0;
        char off[12];
        while (*op2 != '(') {
            off[i] = *op2;
            op2++;
            i++;
        }
        off[i] = '\0';
        int iOffset = atoi(off);
        imm16 = intToBinary(iOffset, 16, imm16);
        strcpy(pI->imm, imm16);
        // then set rs value from inside parentheses
        int j = 0;
        char rsStr[12];
        op2 = op2OrigStart;
        while (*op2 != '(') {
            op2++;
        }
        op2++;  // skips (
        while (*op2 != ')') {
            rsStr[j] = *op2;
            j++;
            op2++;
        }
        rsStr[j] = '\0';
        strcpy(pI->rs, getRegID(rsStr));
    }
    // I2: [mnem] rt, rs, imm16
    else if (isI2(mnem)) {
        strcpy(pI->rt, getRegID(op1));
        strcpy(pI->rs, getRegID(op2));
        int iOffset = atoi(op3);
        imm16 = intToBinary(iOffset, 16, imm16);
        strcpy(pI->imm, imm16);
    }
    // I3: [mnem] rs, rt, offset
    else if (isI3(mnem)) {
        strcpy(pI->rs, getRegID(op1));
        strcpy(pI->rt, getRegID(op2));
        // op3 is the target address symbol
        int target = intTargetAddr(op3, TSymbols, numTSymbols);
        imm16 = getBranchOffset(address, target, imm16);
        strcpy(pI->imm, imm16);
    }
    // I4: [mnem] rs, offset
    else {
        strcpy(pI->rs, getRegID(op1));
        strcpy(pI->rt, "00000");
        // op2 is the target address label
        int target = intTargetAddr(op2, TSymbols, numTSymbols);
        imm16 = getBranchOffset(address, target, imm16);
        strcpy(pI->imm, imm16);
    }
    return pI;
}


int intTargetAddr(char* target, TSymbol TSymbols[], int numTSymbols) {
    return getTSymbolAddress(TSymbols, target, numTSymbols);
}


char* IMachineCode(I* pI, char* mCode32) {
    memmove(mCode32, pI->opcode, 6);
    memmove(mCode32 + 6, pI->rs, 5);
    memmove(mCode32 + 11, pI->rt, 5);
    memmove(mCode32 + 16, pI->imm, 16);
    *(mCode32 + 32) = '\0';
    return mCode32;
}


/***********************************************************************
 *                      J-TYPE INSTRUCTIONS
 ***********************************************************************/
bool isJType(char* mnem) {
    return (strcmp(mnem, "j") == 0) || (strcmp(mnem, "jal") == 0);
}

J* initJType(J* pJ, char* instr_idx, char* mnem, char* target, int address, TSymbol TSymbols[], int numTSymbols) {
    if (!isJType(mnem)) {  return NULL; }
    strcpy(pJ->opcode, getOpCode(mnem));
    pJ->address = address;
    int intJump = getJumpInstrIdx(target, TSymbols, numTSymbols);
    intJump >>= 2;               // since 26 bits will be shifted << 2 to get
    // target.
    instr_idx = intToBinary(intJump, 26, instr_idx);
    strcpy(pJ->instr_idx, instr_idx);
    free(instr_idx);
    return pJ;
}


char* JMachineCode(J* pJ, char* mCode32) {
    memmove(mCode32, pJ->opcode, 6);
    memmove(mCode32 + 6, pJ->instr_idx, 26);
    *(mCode32 + 32) = '\0';
    return mCode32;
}


int getJumpInstrIdx(char* target, TSymbol TSymbols[], int numTSymbols) {
    return intTargetAddr(target, TSymbols, numTSymbols);
}


/***********************************************************************
 *                      PSEUDO-INSTRUCTIONS
 ***********************************************************************/

LX* initLX(LX* pLX, char* imm16, char* mnem, char* destReg, char* srcOp, int address,
           DSymbol DSymbols[], int numDSymbols) {
    pLX->address = address;
    strcpy(pLX->mnem, mnem);
    strcpy(pLX->destRegID, getRegID(destReg));
    if (isLA(mnem)) {
        int address = getDSymbolAddress(DSymbols, srcOp, numDSymbols);
        imm16 = intToBinary(address, 16, imm16);
    }
    else {  // LI
        int immSrc = atoi(srcOp);
        imm16 = intToBinary(immSrc, 16, imm16);
    }
    strcpy(pLX->source, imm16);
    return pLX;
}


bool isPseudo(char* mnem) {
    return (isLX(mnem) || isBLX(mnem));
}


bool isLX(char* mnem) {
    return (isLA(mnem)) || (isLI(mnem));
}


bool isLA(char* mnem) {
    return strcmp(mnem, "la") == 0;
}


bool isLI(char* mnem) {
    return strcmp(mnem, "li") == 0;
}


char* LXMachineCode(LX* pLX, char* mCode32) {
    if (isLA(pLX->mnem)) {
        memmove(mCode32, "001000", 6);          // addi opcode
    }
    else if (isLI(pLX->mnem)) {
        memmove(mCode32, "001001", 6);          // addi opcode
    }
    memmove(mCode32 + 6, "00000", 5);           // rs = $zero register
    memmove(mCode32 + 11, pLX->destRegID, 5);   // destination register
    memmove(mCode32 + 16, pLX->source, 16);     // .data source
    *(mCode32 + 32) = '\0';
    return mCode32;
}


BLX* initBLX(BLX* pBLX, char* branchStr, char* mnem, char* rs, char* rt,
             char* offsetSym, int address,
             TSymbol TSymbols[], int numTSymbols) {
    if (!isBLX(mnem))   { return NULL; }
    strcpy(pBLX->mnem, mnem);
    pBLX->address = address;
    strcpy(pBLX->rs, rs);
    strcpy(pBLX->rt, rt);
    int target = intTargetAddr(offsetSym, TSymbols, numTSymbols);
    branchStr = getBranchOffset(address, target, branchStr);
    strcpy(pBLX->branch, branchStr);
    printf("BLX INSTRUCTION AT END OF INIT_BLX:\n");
    printf("name: %s\n", pBLX->mnem);
    printf("address: %d\n", address);
    printf("rs: %s\n", pBLX->rs);
    printf("rt: %s\n", pBLX->rt);
    printf("branch: %s\n", pBLX->branch);
    return pBLX;
}


char* BLXMachineCode(BLX* pBLX, char* mCode64) {
    if (isBLT(pBLX->mnem)) {
        // translation, part 1: slt $at, rs, rt     [rd, rs, rt]
        printf("BLXMachinecode (BLT): \n");
        memmove(mCode64, getOpCode("slt"), 6);      // SLT opcode
        memmove(mCode64 + 6, getRegID(pBLX->rs), 5);    // BLT rs = SLT rs
        memmove(mCode64 + 11, getRegID(pBLX->rt), 5);    // BLT rt = SLT rt
        memmove(mCode64 + 16, getRegID("$at"), 5);       // BLT rd = SLT rd = $at
        memmove(mCode64 + 21, "00000", 5);               // SLT shiftAmt = 0
        memmove(mCode64 + 26, getFunct("slt"), 6);       // SLT function code
        // translation, part 2: bne $at, $zero, offset  [rs, rt, offset]
        memmove(mCode64 + 32, getOpCode("bne"), 6);      // BNE opcode
        memmove(mCode64 + 38, getRegID("$at"), 5);       // BNE rs = $at
        memmove(mCode64 + 43, getRegID("$zero"), 5);     // BNE rt = $zero
        memmove(mCode64 + 48, pBLX->branch, 16);          // .text symbol becomes
        // branch target
        return mCode64;
    }
    else {  // BLE
        // translation, part 1: slt at, rt, rs      [rd, rs, rt]
        memmove(mCode64, getOpCode("slt"), 6);      // SLT opcode
        memmove(mCode64 + 6, getRegID(pBLX->rt), 5);    // SLT rs = BLE rt
        memmove(mCode64 + 11, getRegID(pBLX->rs), 5);    // ...and vice versa
        memmove(mCode64 + 16, getRegID("$at"), 5);       // dest. reg is $at
        memmove(mCode64 + 21, "00000", 5);               // shift amt
        memmove(mCode64 + 26, getFunct("slt"), 6);       // funct
        // translation, part 2: beq at, zero, offset    [rs, rt, offset]
        memmove(mCode64 + 32, getOpCode("beq"), 6);      // BEQ opcode
        memmove(mCode64 + 38, getRegID("$at"), 5);       // BEQ rs = $at
        memmove(mCode64 + 43, getRegID("$zero"), 5);     // BEQ rt = $zero
        memmove(mCode64 + 48, pBLX->branch, 16);          // branch target
        return mCode64;
    }
}


bool isBLX(char* mnem) {
    return (isBLT(mnem) || isBLE(mnem));
}

bool isBLT(char* mnem) {
    return strcmp(mnem, "blt") == 0;
}

bool isBLE(char* mnem) {
    return strcmp(mnem, "ble") == 0;
}
