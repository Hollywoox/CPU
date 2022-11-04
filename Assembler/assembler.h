#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/onegin.h"
#include "../include/stack.h"

#define VERSION 2
#define SIGN 0xC
#define LABELS_SIZE 50

#define PrintErrorMessage() printf("%s() at %s(%d): error:\n", __FUNCTION__, __FILE__, __LINE__)

struct Label
{
    int adr;
    char* name = NULL;
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
    CMD_RET,
};


void AsmCodeArray(Text* program, char* file);

void Compile(Text* text, struct Label* labels, int num_of_compilation);

int GetWord(char* str, char* cmd);

void GetArgs(char* str, char** code, char* cmd, FILE* listing);

void GetLabel(char* str, char** code, struct Label* labels, char* cmd, int num_of_compilation);

int LabelFind(const char* label, struct Label* labels);

int IsNum(char* str);

void ToUpper(char* str);

void ClearLabels(struct Label* labels);