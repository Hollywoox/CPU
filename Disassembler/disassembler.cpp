#include "disassembler.h"

//=====================================================================================================

char* DisasmCodeArray(const char* adr)
{
    assert(adr != NULL);

    char* info = (char*)calloc(1, 2 * sizeof(char) + sizeof(elem_t));
    assert(info != NULL);

    FILE* bin_file = fopen(adr, "r");
    if(bin_file == NULL)
    {
        PrintErrorMessage();
        printf("cant't open a file with binary code\n");
        abort();
    }

    fread(info, sizeof(char), 2 * sizeof(char) + sizeof(elem_t), bin_file);
    int code_size = *((int*)(info + 2));

    char* code = (char*)calloc(code_size + 1, sizeof(char));
    assert(code != NULL);

    fread(code, sizeof(char), code_size, bin_file);

    fclose(bin_file);
    free(info);

    return code;
}

//=====================================================================================================

void Disassemble(char* code)
{
    FILE* asm_file = fopen("result.asm", "wb");
    if(asm_file == NULL)
    {
        PrintErrorMessage();
        printf("can't open file for asm code\n");
        abort();
    }

    char* ip = code;
    while(ip)
    {
        switch(*ip & 0x1F)
        {
            #include "../include/cmd.h"

            default:
            {
                printf("error: unknown command\n");
                fclose(asm_file);
                return;
            }
            #undef DEF_CMD
        }
    }

    fclose(asm_file);
}