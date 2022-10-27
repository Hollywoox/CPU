#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/onegin.h"
#include "../include/stack.h"

#define VERSION 2
#define SIGN 0xC

typedef struct
{
    char* code;
    short int code_size;
    Stack stk;
    Stack ret_stk;
    elem_t Regs[5];
    elem_t* RAM;
} CPU;

enum registers
{
    REG_RAX = 1,
    REG_RBX,
    REG_RCX,
    REG_RDX
};

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


void Ctor(CPU* cpu, size_t ram_size, char* adr);

void Run(CPU* cpu);

void Dtor(CPU* cpu);