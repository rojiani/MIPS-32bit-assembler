# Makefile for MIPS Assembler, CS 2506, Fall 2013
#
# Set compilation options:
# -O0        no optimizations; remove after debugging
# -m32       32-bit executable
# -std=c99   use C99 Standard features
# -Wall		 show "all" warnings
# -ggdb3     add extra debug info
#
CC=gcc
CFLAGS=-O0 -std=c99 -Wall -m32 -ggdb3
EXECUTABLE = assemble
C_FILES = Instructions.c LookupTables.c main.c Parser.c Symbols.c Utils.c
H_FILES = Instructions.h LookupTables.h Parser.h Symbols.h Utils.h

assembler: $(C_FILES) $(H_FILES)
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(C_FILES) $(H_FILES)

