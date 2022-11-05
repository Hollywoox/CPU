#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/onegin.h"
#include "../include/stack.h"

#define VERSION 2
#define SIGN 0xC

#define PrintErrorMessage() printf("%s() at %s(%d): error:\n", __FUNCTION__, __FILE__, __LINE__)

enum args
{
    ARG_IMMED = 0x20,
    ARG_REG = 0x40,
    ARG_RAM = 0x80
};

enum comands
{
    CMD_HLT,
    CMD_PUSH,
    CMD_POP,
    CMD_ADD,
    CMD_SUB,
    CMD_MUL,
    CMD_DIV,
    CMD_DUP,
    CMD_OUT,
    CMD_DUMP,
    CMD_IN,
    CMD_JMP,
    CMD_JB,
    CMD_JBE,
    CMD_JA,
    CMD_JAE,
    CMD_JE,
    CMD_JNE,
    CMD_CALL,
    CMD_RET
};

char* DisasmCodeArray(const char* file);

void Disassemble(char* code);