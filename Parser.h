//
//  Parser.h
//  MIPS Assembler
//
//  Created by Navid Rojiani on 11/16/13.
//  Copyright (c) 2013 NKR. All rights reserved.
//


#ifndef MIPS_Assembler_Parser_h
#define MIPS_Assembler_Parser_h

#include <stdio.h>
#include <stdbool.h>

#define MAX_LINES            200    // assumes there's no more than 200. Probably an overshoot.
#define MAX_LINE_LEN         256    
#define TEXT_BASE_ADDRESS      0    // 0x00000000
#define DATA_BASE_ADDRESS   8192    // 0x00002000


/**
 * Searches for comments indicated by '#', and replaces all characters that
 * follow it on the same line with whitespace. This and other whitespace
 * will be removed by later parsing operations.
 */
FILE* parseComments(FILE* fp, FILE* temp1fp);


/**
 * Takes as input a file pointer to a file, and file pointer to a second
 * file. Returns the second file pointer, which now contains the contents
 * of the first file with all whitespace trimmed to a single space.
 * Note: May contain consecutive lines which consist only of a single space.
 */
FILE* trimWhiteSpace(FILE* temp1fp, FILE* temp2fp);


/** Stores each line in the file in a 'ragged array' of strings */
int createLineTable(FILE* fp, char** lines);


/**
 * Returns true if the given string contains a section header directive (.text
 * or .data)
 */
bool isDirective(char* line);


/** Returns true if the given line contains the section header directive .text */
bool isTextDirective(char* line);


/** Returns true if the given line contains the section header directive .data */
bool isDataDirective(char* line);


/** Returns true if the given line contains a symbol (contains ':') */
bool isSymbol(char* line);


/** 
 * Returns true if the given line contains a nonuniform array (e.g.,
 * 2, 4, 9, 10) 
 */
bool isNonUniformArray(char* line);


/** Returns true if the given line contains a uniform array (e.g., 2:10) */
bool isUniformArray(char* line);


/** Returns true if the given line contains a string */
bool isASCIIZ(char* line);


/** Returns true if the given line contains a '.word' variable or array */
bool isWord(char* line);


/** Returns true if the given line contains only whitespace */
bool isBlankLine(char* line);


#endif