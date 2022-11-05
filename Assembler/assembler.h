#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/onegin.h"
#include "../include/stack.h"

//=====================================================================================================

#define VERSION 3
#define SIGN 0xC
#define LABELS_SIZE 50

#define PrintErrorMessage() printf("%s() at %s(%d): error:\n", __FUNCTION__, __FILE__, __LINE__)

struct Label
{
    int adr;
    char* name = NULL;
};


#define DEF_CMD(name, num, ...) CMD_##name = num,

enum comands
{
    #include "../include/cmd.h"
};

#undef DEF_CMD

#define DEF_CMD(name, num, arg, ...)                                                                                                       \
                                if(Stricmp(cmd, #name) == 0)                                                                               \
                                {                                                                                                          \
                                    *ip = num;                                                                                             \
                                    if(arg == 0)                                                                                           \
                                    {                                                                                                      \
                                        fprintf(listing, "%#8lX %2X   %s\n", ip - code - size, num, #name);                                \
                                        ++ip;                                                                                              \
                                    }                                                                                                      \
                                                                                                                                           \
                                    else if(arg == 1)                                                                                      \
                                    {                                                                                                      \
                                        fprintf(listing, "%#8lX %2X ", ip - code - size, num);                                             \
                                        ++ip;                                                                                              \
                                        GetArgs(text->str_array[line].str, &ip, cmd, listing);                                             \
                                    }                                                                                                      \
                                                                                                                                           \
                                    else                                                                                                   \
                                    {                                                                                                      \
                                        fprintf(listing, "%#8lX %2X ", ip - code - size, num);                                             \
                                        ++ip;                                                                                              \
                                        GetLabel(text->str_array[line].str, &ip, labels, cmd, num_of_compilation);                         \
                                        fprintf(listing, "%X %s %X\n", *((int*)(ip - sizeof(int))), #name, *((int*)(ip - sizeof(int))));   \
                                    }                                                                                                      \
                                }                                                                                                          \
                                else


//=====================================================================================================

void AsmCodeArray(Text* program, const char* file);

void Compile(Text* text, struct Label* labels, int num_of_compilation);

int GetWord(const char* str, char* cmd);

void GetArgs(const char* str, char** code, char* cmd, FILE* listing);

void GetLabel(const char* str, char** code, struct Label* labels, char* cmd, int num_of_compilation);

int LabelFind(const char* label, struct Label* labels);

int IsNum(const char* str);

void StrToUpper(char* str);

int Stricmp(const char* str1, const char* str2);

void ClearLabels(struct Label* labels);