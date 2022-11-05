#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/onegin.h"
#include "../include/stack.h"

#define VERSION 3
#define SIGN 0xC

#define PrintErrorMessage() printf("%s() at %s(%d): error:\n", __FUNCTION__, __FILE__, __LINE__)

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

#define DEF_CMD(name, num, arg, code_cpu)                                    \
                                case num:                               \
                                {                                       \
                                    if(num == 0)                        \
                                    {                                   \
                                        fprintf(asm_file, "HLT\n");     \
                                        if(*(ip + 1) == 0)              \
                                        {                               \
                                            fclose(asm_file);           \
                                            return;                     \
                                        }                               \
                                                                        \
                                        ip += 1;                        \
                                        break;                          \
                                    }                                   \
                                                                        \
                                    else if(arg == 0)                   \
                                    {                                   \
                                        fprintf(asm_file, #name);       \
                                        fprintf(asm_file, "\n");        \
                                        ip += 1;                        \
                                        break;                          \
                                    }                                   \
                                                                        \
                                    else if(arg == 1)                   \
                                    {                                   \
                                        fprintf(asm_file, #name);       \
                                        int cmd = *ip;                  \
                                        ip     += 1;                    \
                                                                        \
                                        int index   = 0;                \
                                        char reg    = 0;                \
                                        elem_t argument  = 0;                \
                                                                        \
                                        if(cmd & ARG_RAM)               \
                                        {                               \
                                            if(cmd & ARG_IMMED)         \
                                            {                           \
                                                index = *((elem_t*)ip); \
                                                ip  += sizeof(elem_t);  \
                                            }                           \
                                            if(cmd & ARG_REG)           \
                                            {                           \
                                                reg = *ip + 'A' - 1;    \
                                                ip  += 1;               \
                                            }                           \
                                                                        \
                                            fprintf(asm_file, " [");                          \
                                            if(reg != 0) fprintf(asm_file, "R%cX + ", reg);   \
                                            fprintf(asm_file, "%d]\n", index);                \
                                        }                                                     \
                                        else if(cmd & ARG_REG)                                \
                                        {                                                     \
                                            reg = *ip + 'A' - 1;                              \
                                            ip += 1;                                          \
                                                                                              \
                                            fprintf(asm_file, " R%cX\n", reg);                \
                                        }                                                     \
                                        else if(cmd & ARG_IMMED)                              \
                                        {                                                     \
                                            argument = *((elem_t*)ip);                        \
                                            ip += sizeof(elem_t);                             \
                                                                                              \
                                            fprintf(asm_file, " ");                           \
                                            fprintf(asm_file, elem_fmt, argument);            \
                                            fprintf(asm_file, "\n");                          \
                                        }                                                     \
                                                                                              \
                                        else                                                  \
                                        {                                                     \
                                            fprintf(asm_file, "\n");                          \
                                        }                                                     \
                                                                                              \
                                        break;                                                \
                                    }                                                         \
                                                                                              \
                                    else if(arg == 2)                                         \
                                    {                                                         \
                                        fprintf(asm_file, #name);                             \
                                        fprintf(asm_file, " ");                               \
                                        ip += 1;                                              \
                                                                                              \
                                        int adr = *((int*)ip);                                \
                                        fprintf(asm_file, "%d\n", adr);                       \
                                        ip += sizeof(int);                                    \
                                                                                              \
                                        break;                                                \
                                    }                                                         \
                                }

char* DisasmCodeArray(const char* file);

void Disassemble(char* code);