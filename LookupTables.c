//
//  LookupTables.c
//  MIPS Assembler
//
//  Created by Navid Rojiani on 11/16/13.
//  Copyright (c) 2013 NKR. All rights reserved.
//


#include <stdio.h>
#include <string.h>
#include "LookupTables.h"


char* getRegID(char* regName) {
    for (int i = 0; i < NUM_REGISTERS; i++) {
        if (strcmp(regName, Registers[i].regName) == 0) {
            return Registers[i].binaryRegID;
        }
    }
    return NULL;
}


char* getFunct(char* rMnem) {
    for (int i = 0; i < NUM_R_TYPE; i++) {
        if (strcmp(rMnem, FunctCodes[i].rMnem) == 0) {
            return FunctCodes[i].rFunct;
        }
    }
    return NULL;
}


char* getOpCode(char* mnem) {
    for (int i = 0; i < NUM_INSTRXNS; i++) {
        if (strcmp(mnem, OpCodes[i].mnem) == 0) {
            return OpCodes[i].opcode;
        }
    }
    return NULL;
}

