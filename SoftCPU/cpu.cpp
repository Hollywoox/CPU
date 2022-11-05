#include "cpu.h"

//=====================================================================================================


void Ctor(CPU* proc, size_t ram_size, const char* adr)
{
    assert(proc != NULL);
    assert(adr != NULL);

    proc->RAM = (elem_t*)calloc(ram_size, sizeof(elem_t));
    assert(proc->RAM != NULL);

    char* info = (char*)calloc(1, 2 * sizeof(char) + sizeof(elem_t));
    assert(info != NULL);

    FILE* bin_file = fopen(adr, "rb");
    assert(bin_file != NULL);

    fread(info, sizeof(char), 2 * sizeof(char) + sizeof(elem_t), bin_file);

    if(*info != SIGN)
    {
        printf("Error: The signature of the file does not match the signature of the CPU\n");
        abort();
    }
    if(*(info + 1) != VERSION)
    {
        printf("Warning: The version of your CPU is old, please, buy a new one\n");
    }
    
    proc->code_size = *((int*)(info + 2));
    proc->code      = (char*)calloc(proc->code_size + 1, sizeof(char));
    assert(proc->code != NULL);

    fread(proc->code, sizeof(char), proc->code_size, bin_file);

    StackCtor(&proc->stk,    10);
    StackCtor(&proc->ret_stk, 0);

    fclose(bin_file);
    free(info);
}

//=====================================================================================================

void Run(CPU* proc)
{
    char* ip = proc->code;
    while(ip)
    {
        switch(*ip & 0x1F)
        {
            #include "../include/cmd.h"

            default:
            {
                printf("error: unknown command\n");
                return;
            }

            #undef DEF_CMD
        }
    }
}
 
//=====================================================================================================

void Dtor(CPU* proc)
{
    free(proc->RAM);
    proc->RAM = NULL;

    free(proc->code);
    proc->code = NULL;

    StackDtor(&proc->stk);
    StackDtor(&proc->ret_stk);
}