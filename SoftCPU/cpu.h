#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/onegin.h"
#include "../include/stack.h"

#define VERSION 3
#define SIGN 0xC

typedef struct
{
    char* code;
    int code_size;
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

#define DEF_CMD(name, num, ...) CMD_##name = num,

enum comands
{
    #include "../include/cmd.h"
};

#undef DEF_CMD

#define DEF_CMD(name, num, arg, code_cpu)                    \
                                            case num:        \
                                            {                \
                                                code_cpu     \
                                                break;       \
                                            }                


void Ctor(CPU* proc, size_t ram_size, const char* adr);

void Run(CPU* proc);

void Dtor(CPU* proc);