//
//  Instructions.h
//  MIPS Assembler
//
//  Created by Navid Rojiani on 11/17/13.
//  Copyright (c) 2013 NKR. All rights reserved.
//

#ifndef MIPS_Assembler_Instructions_h
#define MIPS_Assembler_Instructions_h

#include "Symbols.h"

#define INSTR_SIZE              32
#define OPCODE_LEN               6
#define RS_LEN                   5
#define RT_LEN                   5
#define RD_LEN                   5
#define SHAMT_LEN                5
#define FUNCT_LEN                6
#define IMM_LEN                 16

#define MACHINE_CODE_LEN        32
#define BLX_MACHINE_CODE_LEN    64
#define OFFSET_STR_LEN          16
#define JUMP_INSTR_IDX_LEN      26

#include <stdbool.h>


bool isValidInstr(char* mnem);


/**
 * Retrieves the address of a the specified target (a symbol from the
 * .text segment) from the TSymbol table.
 */
int intTargetAddr(char* target, TSymbol TSymbols[], int numTSymbols);


/**********************************************************************
 *                      R-TYPE INSTRUCTIONS:
 *  R-1: [mnem] rd, rs, rt
 *      1) add
 *      2) sub
 *      3) or
 *      4) and
 *      5) slt
 *  R-2: [mnem] rd, rt, sa
 *      6) sll
 *      7) srl
 *      8) sra
 *  Special:
 *      9) nop
 *     10) syscall
 *     11) jr
 ***********************************************************************/


/** Struct representing all necessary data for an R-Type instruction */
struct RType {
    char opcode[OPCODE_LEN + 1];
    char rs[RS_LEN + 1];
    char rt[RT_LEN + 1];
    char rd[RD_LEN + 1];
    char shamt[SHAMT_LEN + 1];
    char funct[FUNCT_LEN + 1];
    int address;
};
typedef struct RType R;


/** 
 * Checks if the mnemonic is an R-type instruction of the form:
 *              [mnem] rd, rs, rt
 */
bool isR1(char* mnem);


/**
 * Checks if the mnemonic is an R-type instruction of the form:
 *              [mnem] rd, rt, sa
 */
bool isR2(char* mnem);


/** Checks if the instruction is an R-Type */
bool isRType(char* mnem);


/** 
 * Given the mnemonic for an R-type instruction and its 3 operands,
 * initializes a struct that holds relevant data about its encoding.
 * Return: pointer to the R struct, or NULL if mnem parameter was not
 * an R-type instruction.
 */
R* initRType(R* pR, char* mnem, char* op1, char* op2, char* op3, int address);


/** Returns a string consisting of the instruction's 32 bits */
char* RMachineCode(R* pR, char* mCode32);


/** Checks if the instruction is a NOP */
bool isNOP(char* mnem);


/** Initializes a NOP struct. Really just used as a placeholder in the
 *  instruction table for consistency. */
R* initNOP(R* pNOP, int address);


/** Returns a string corresponding to the machine code for a NOP instruction */
char* NOPMachineCode(R* pNOP, char mCode32[]);


/** Checks if the instruction is a Syscall */
bool isSYSCALL(char* mnem);


/** Initializes a Syscall struct. Really just used as a placeholder in the
 *  instruction table for consistency. */
R* initSYSCALL(R* pSYS, int address);


/** Returns a string corresponding to the machine code for a syscall instruction */
char* SyscallMachineCode(R* pSyscall, char mCode32[]);


/** Checks if the instruction is a JR */
bool isJR(char* mnem);


/** Initializes a Syscall struct. */
R* initJR(R* pJR, char* rs, int address);


/** 
 * Returns a string corresponding to the machine code for the jr instruction
 * pointed to by pJR 
 */
char* JRMachineCode(R* pJR, char mCode32[]);

/**********************************************************************
 *                      I-TYPE INSTRUCTIONS:
 *  I-1: [mnem] rt, offset(rs)
 *      1) lw
 *      2) sw
 *  I-2: [mnem] rt, rs, imm16
 *      3) addi
 *      4) addiu
 *      5) ori
 *      6) andi
 *      7) slti
 *  I-3: [mnem] rs, rt, offset
 *      8) beq
 *      9) bne
 *  I-4: [mnem] rs, offset
 *      10) bltz
 *      11) blez
 ***********************************************************************/

/** Struct representing all necessary data for an I-Type instruction */
struct IType {
    char opcode[OPCODE_LEN + 1];
    char rs[RS_LEN + 1];
    char rt[RT_LEN + 1];
    char imm[IMM_LEN + 1];
    int address;
};
typedef struct IType I;


bool isI1(char* mnem);
bool isI2(char* mnem);
bool isI3(char* mnem);
bool isI4(char* mnem);
bool isIType(char* mnem);

/** 
 * Returns true if a lw/sw (I1) instruction is in its standard format -
 * e.g., "lw  rt,  offset(rs)". When the offset is zero, the assembly
 * instruction may appear as "lw  rt,  rs". Hence, this function aids
 * in determining its format to aid in parsing the instruction's 
 * operands.
 */
bool hasImmOffset(char* str);


// op3 null for I1, I4
I* initIType(I* pI, char* imm16, char* mnem, char* op1, char* op2, char* op3, int address,
             TSymbol TSymbols[], int numTSymbols);


char* IMachineCode(I* pI, char* mCode32);


/**
 * Calculate the unshifted branch offset and return it as a
 * string so it can be used to initialize beq, bne, bltz, or blez
 * Inputs: - PC is the integer value of the
 *           I3 or I4 (beq/bne/bltz/blez) instruction
 *         - target is the integer value of the address
 *           to branch to if condition is met.
 *         - offSetStr is a char* to store the length 16 binary string
 */
char* getBranchOffset(int PC, int target, char* offsetStr);



/**********************************************************************
 *                      J-TYPE INSTRUCTIONS:
 *                          [mnem] target
 *  1) j
 *  2) jal
 *
 ***********************************************************************/


struct JType {
    char opcode[OPCODE_LEN + 1];
    char instr_idx[JUMP_INSTR_IDX_LEN + 1];
    int address;
};
typedef struct JType J;


bool isJType(char* mnem);

J* initJType(J* pJ, char* instr_idx, char* mnem, char* target, int address,
             TSymbol TSymbols[], int numTSymbols);

char* JMachineCode(J* pJ, char* mCode32);

int getJumpInstrIdx(char* target, TSymbol TSymbols[], int numTSymbols);


/**********************************************************************
 *                      PSEUDO-INSTRUCTIONS:
 *  Lx: LA & LI
 *      1) la
 *      2) li
 *  BLx: BLT & BLE
 *      3) blt
 *      4) ble
 *
 ***********************************************************************/

/**
 * Returns true if the mnemonic is one of the four pseudo-instructions
 * that are handled by the assembler (LA, LI, BLT, or BLE).
 */
bool isPseudo(char* mnem);


/** LA and LI instructions */
struct Pseudo_LX {
    char mnem[2 + 1];
    char destRegID[RS_LEN + 1];
    char source[16 + 1];
    int address;
};
typedef struct Pseudo_LX LX;



/** Returns true if the line (or other string) contains a Lx (LA or LI) pseudo-instruction */
bool isLX(char* mnem);
bool isLA(char* mnem);
bool isLI(char* mnem);



/** 
 * Initializes a struct representation of an LA or LI pseudo-instruction.
 * Inputs: pLX - a pointer to an LX struct
 *         imm16 - a pointer to a string to store the 16-bit value, which
 *                 can either be a pointer to an .data segment element (for
 *                 la) or an immediate (for li)
 *         mnem, destReg, srcOp - parsed operands
 *         address - the current PC value
 *         DSymbols - table of arrays, variables, and strings from .data 
 *         numDSymbols - the number of elements in the table
 */
LX* initLX(LX* pLX, char* imm16, char* mnem,
           char* destReg, char* srcOp, int address,
           DSymbol DSymbols[], int numDSymbols);


char* LXMachineCode(LX* pLX, char* mCode32);


/** BLT & BLE instructions */
struct Pseudo_BLX {
    char mnem[4];
    char rs[RS_LEN + 1];
    char rt[RT_LEN + 1];
    char branch[IMM_LEN + 1];
    int address;
};
typedef struct Pseudo_BLX BLX;


BLX* initBLX(BLX* pBLX, char* branchStr, char* mnem, char* rs, char* rt, 
             char* offsetSym, int address, TSymbol TSymbols[], int numTSymbols);

char* BLXMachineCode(BLX* pBLX, char* mCode64);

/** Returns true if the string contains a BLx subtype (BLT or BLE) pseudo-instruction */
bool isBLX(char* mnem);
bool isBLT(char* mnem);
bool isBLE(char* mnem);



#endif
