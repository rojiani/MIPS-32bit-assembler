//
//  LookupTables.h
//  MIPS Assembler
//
//  Created by Navid Rojiani on 11/14/13.
//  Copyright (c) 2013 NKR. All rights reserved.
//

/** 
 * A series of lookup tables for the values associated with a mnemonic or register.
 * Using these tables, the corresponding value can be retrieved for a key through
 * sequential search.
 */

#ifndef MIPS_Assembler_LookupTables_h
#define MIPS_Assembler_LookupTables_h

#include <stdbool.h>

#define NUM_REGISTERS   32
#define NUM_R_TYPE      11
#define NUM_I_TYPE      11
#define NUM_J_TYPE       2
#define NUM_INSTRXNS    28



/***********************************************************************
 *                             REGISTERS
 ***********************************************************************/
/** Dictionary entry type structure which pairs a register name and its binary number */
struct RegisterID {
    char* regName;
    char* binaryRegID;
};
typedef struct RegisterID RegID;


/** Registers */
static RegID Registers[NUM_REGISTERS] = {
    {"$zero",   "00000"},   // 0
    {"$at",     "00001"},   // 1
    {"$v0",     "00010" },  // 2
    {"$v1",     "00011" },  // 3
    {"$a0",     "00100" },  // 4
    {"$a1",     "00101" },  // 5
    {"$a2",     "00110" },  // 6
    {"$a3",     "00111" },  // 7
    {"$t0",     "01000" },  // 8
    {"$t1",     "01001" },  // 9
    {"$t2",     "01010" },  // 10
    {"$t3",     "01011" },  // 11
    {"$t4",     "01100" },  // 12
    {"$t5",     "01101" },  // 13
    {"$t6",     "01110" },  // 14
    {"$t7",     "01111" },  // 15
    {"$s0",     "10000" },  // 16
    {"$s1",     "10001" },  // 17
    {"$s2",     "10010" },  // 18
    {"$s3",     "10011" },  // 19
    {"$s4",     "10100" },  // 20
    {"$s5",     "10101" },  // 21
    {"$s6",     "10110" },  // 22
    {"$s7",     "10111" },  // 23
    {"$t8",     "11000" },  // 24
    {"$t9",     "11001" },  // 25
    {"$k0",     "11010" },  // 26
    {"$k1",     "11011" },  // 27
    {"$gp",     "11100" },  // 28
    {"$sp",     "11101" },  // 29
    {"$fp",     "11110" },  // 30
    {"$ra",     "11111" }  // 31
};


/** Returns a string of the 5 bits corresponding to the registers ID# (0-31) */
char* getRegID(char* regName);


/***********************************************************************
 *                          INSTRUCTIONS
 ***********************************************************************/
/** Pairs a R-type instruction with its funct code. */
struct RTypeFunct {
    char* rMnem;
    char* rFunct;
};

typedef struct RTypeFunct RFunct;


/** R-type funct codes lookup table */
static RFunct FunctCodes[NUM_R_TYPE] = {
    {"add",     "100000"},
    {"sub",     "100010"},
    {"or",      "100101"},
    {"and",     "100100"},
    {"slt",     "101010"},
    {"sll",     "000000"},
    {"srl",     "000010"},
    {"sra",     "000011"},
    {"nop",     "000000"},
    {"syscall", "001100"},
    {"jr,"      "001000"}
};


/**
 * Searches the FunctCodes table for the specified R-type mnemonic
 * If found: returns its funct code
 * If not found: returns NULL
 */
char* getFunct(char* rMnem);


/** Key-Value dictionary entry...mnemonic & its corresponding opcode. */
struct OpCodeEntry {
    char* mnem;
    char* opcode;
};

typedef struct OpCodeEntry OpCode;


/** OpCode Lookup Table */
static OpCode OpCodes[NUM_INSTRXNS] = {
    {"add",     "000000"},      // R
    {"sub",     "000000"},
    {"or",      "000000"},
    {"and",     "000000"},
    {"slt",     "000000"},
    {"sll",     "000000"},
    {"srl",     "000000"},
    {"sra",     "000000"},
    {"nop",     "000000"},
    {"syscall", "000000"},
    {"jr",      "000000"},
    {"lw",      "100011"},     // I
    {"sw",      "101011"},
    {"addi",    "001000"},
    {"addiu",   "001001"},
    {"ori",     "001101"},
    {"andi",    "001100"},
    {"slti",    "001010"},
    {"beq",     "000100"},
    {"bne",     "000101"},
    {"bltz",    "000001"},
    {"blez",    "000110"},
    {"j",       "000010"},      // J
    {"jal",     "000011"}
};


/**
 * Searches the OpCodes table for the specified instruction.
 * If found: returns the instruction's opcode
 * If not found: returns NULL
 */
char* getOpCode(char* mnem);



#endif