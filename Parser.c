//
//  Parser.c
//  MIPS Assembler
//
//  Created by Navid Rojiani on 11/16/13.
//  Copyright (c) 2013 NKR. All rights reserved.
//

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "Parser.h"


int createLineTable(FILE* fp, char** lines) {
    rewind(fp);
    int sz = 0;
    for (int i = 0; i < MAX_LINES; i++) {
        char s[MAX_LINE_LEN];
        if (fgets(s, MAX_LINE_LEN, fp) != NULL) {
            lines[i] = malloc(strlen(s) + 1);
            if (lines[i] == NULL) {
                printf("Error allocating memory in createLineTable\n");
                exit(EXIT_FAILURE);
            }
            strcpy(lines[i], s);
            sz++;
        }
        if (sz == MAX_LINE_LEN) {
            printf("CREATED LINE TABLE, BUT INPUT FILE LINES > MAX LINES (200)\n");
        }
    }
    return sz;
}


FILE* parseComments(FILE* fp, FILE* temp1fp) {
    int c;
    int inComment = 0;
    // copy original file into temp file, replacing everything after
    // # with whitespace
    while ( (c = fgetc(fp)) != EOF) {
        if (c == '\n') {
            inComment = 0;
        }
        if (c != '#' && !inComment) {
            fputc(c, temp1fp);
        }
        else {
            inComment = 1;
            fputc(' ', temp1fp);
        }
    }
    return temp1fp;
}


FILE* trimWhiteSpace(FILE* temp1fp, FILE* temp2fp) {
    int c;
    int inSpace = 0;
    // copy original file into temp file, replacing everything after
    // # with whitespace
    while ( (c = fgetc(temp1fp)) != EOF) {
        if (isspace(c) && c != '\n') {
            if (!inSpace) {
                fputc(' ', temp2fp);
            }
            inSpace = 1;
        }
        else {
            inSpace = 0;
            fputc(c, temp2fp);
        }
    }
    return temp2fp;
}


bool isDirective(char* line) {
    if (strstr(line, ".text") != NULL || strstr(line, ".data") != NULL) {
        return true;
    }
    return false;
}


bool isTextDirective(char* line) {
    if (strstr(line, ".text") != NULL) {
        return true;
    }
    return false;
}


bool isDataDirective(char* line) {
    if (strstr(line, ".data") != NULL) {
        return true;
    }
    return false;
}


bool isSymbol(char* line) {
    return (strstr(line, ":") != NULL);
}


bool isUniformArray(char* line) {
    if (isASCIIZ(line)) {       // would have 2nd ':' if string contains ':'
        return false;
    }
    char* first = strstr(line, ":");
    if (first == NULL) {
        return false;
    }
    return (strstr(first + 1, ":"));
}


bool isNonUniformArray(char* line) {
    return (isWord(line) && (strstr(line, ",") != NULL));
}


bool isASCIIZ(char* line) {
    return (strstr(line, ".asciiz") != NULL);
}


bool isWord(char* line) {
    return (strstr(line, ".word") != NULL);
}


bool isBlankLine(char* line) {
    while (*line++) {
        if (!isspace(*line) && *line != '\n' && *line != '\0') {
            return false;
        }
    }
    return true;
}


