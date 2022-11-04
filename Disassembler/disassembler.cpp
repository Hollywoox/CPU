#include "disassembler.h"

//=====================================================================================================

char* DisasmCodeArray(char* adr)
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
            case CMD_ADD:
            {
                fprintf(asm_file, "add\n");
                ip += 1;
                break;
            }

            case CMD_SUB:
            {
                fprintf(asm_file, "sub\n");
                ip += 1;
                break;
            }

            case CMD_MUL:
            {
                fprintf(asm_file, "mul\n");
                ip += 1;
                break;
            }

            case CMD_DIV:
            {
                fprintf(asm_file, "div\n");
                ip += 1;
                break;
            }

            case CMD_DUP:
            {
                fprintf(asm_file, "dup\n");
                ip += 1;
                break;
            }

            case CMD_DUMP:
            {
                fprintf(asm_file, "dump\n");
                ip += 1;
                break;
            }

            case CMD_OUT:
            {
                fprintf(asm_file, "out\n");
                ip += 1;
                break;
            }

            case CMD_IN:
            {
                fprintf(asm_file, "in");
                int cmd = *ip;
                ip     += 1;

                int index   = 0;
                char reg    = 0;

                if(cmd & ARG_RAM)
                {
                    if(cmd & ARG_IMMED)
                    {
                        index = *((elem_t*)ip);
                        ip  += sizeof(elem_t);
                    }
                    if(cmd & ARG_REG)
                    {
                        reg = *ip + 'a' - 1;
                        ip  += 1;
                    }

                    fprintf(asm_file, " [");
                    if(reg != 0) fprintf(asm_file, "r%cx + ", reg);
                    fprintf(asm_file, "%d]\n", index);
                }

                else if(cmd & ARG_REG)
                {
                    reg = *ip + 'a' - 1;
                    ip += 1;

                    fprintf(asm_file, " r%cx\n", reg);
                }
                
                else
                {
                    fprintf(asm_file, "\n");
                }
                
                break;
            }

            case CMD_PUSH:
            {
                fprintf(asm_file, "push");
                int cmd = *ip;
                ip     += 1;

                int index   = 0;
                char reg    = 0;
                elem_t arg  = 0;

                if(cmd & ARG_RAM)
                {
                    if(cmd & ARG_IMMED)
                    {
                        index = *((elem_t*)ip);
                        ip  += sizeof(elem_t);
                    }
                    if(cmd & ARG_REG)
                    {
                        reg = *ip + 'a' - 1;
                        ip  += 1;
                    }

                    fprintf(asm_file, " [");
                    if(reg != 0) fprintf(asm_file, "r%cx + ", reg);
                    fprintf(asm_file, "%d]\n", index);
                }
                else if(cmd & ARG_REG)
                {
                    reg = *ip + 'a' - 1;
                    ip += 1;

                    fprintf(asm_file, " r%cx\n", reg);
                }
                else
                {
                    arg = *((elem_t*)ip);
                    ip += sizeof(elem_t);

                    fprintf(asm_file, " ");
                    fprintf(asm_file, elem_fmt, arg);
                    fprintf(asm_file, "\n");
                }

                break;
            }

            case CMD_POP:
            {
                fprintf(asm_file, "pop");
                int cmd = *ip;
                ip     += 1;

                int index   = 0;
                char reg    = 0;

                if(cmd & ARG_RAM)
                {
                    if(cmd & ARG_IMMED)
                    {
                        index = *((elem_t*)ip);
                        ip  += sizeof(elem_t);
                    }
                    if(cmd & ARG_REG)
                    {
                        reg = *ip + 'a' - 1;
                        ip  += 1;
                    }

                    fprintf(asm_file, " [");
                    if(reg != 0) fprintf(asm_file, "r%cx + ", reg);
                    fprintf(asm_file, "%d]\n", index);
                }
                else if(cmd & ARG_REG)
                {
                    reg = *ip + 'a' - 1;
                    ip += 1;

                    fprintf(asm_file, " r%cx\n", reg);
                }
                else
                {
                    fprintf(asm_file, "\n");
                }
                
                break;
            }

            case CMD_HLT:
            {
                fprintf(asm_file, "hlt\n");
                if(*(ip + 1) == 0)
                {
                    fclose(asm_file);
                    return;
                }

                ip += 1;
                break;
            }

            case CMD_JMP:
            {
                fprintf(asm_file, "jmp ");
                ip += 1;

                int adr = *((int*)ip);
                fprintf(asm_file, "%d\n", adr);
                ip += sizeof(int); 
                
                break;
            }

            case CMD_JB:
            {
                fprintf(asm_file, "jb ");
                ip += 1;

                int adr = *((int*)ip);
                fprintf(asm_file, "%d\n", adr);
                ip += sizeof(int); 
                
                break;
            }

            case CMD_JBE:
            {
                fprintf(asm_file, "jbe ");
                ip += 1;

                int adr = *((int*)ip);
                fprintf(asm_file, "%d\n", adr);
                ip += sizeof(int); 
                
                break;
            }

            case CMD_JA:
            {
                fprintf(asm_file, "ja ");
                ip += 1;

                int adr = *((int*)ip);
                fprintf(asm_file, "%d\n", adr);
                ip += sizeof(int); 
                
                break;
            }

            case CMD_JAE:
            {
                fprintf(asm_file, "jae ");
                ip += 1;

                int adr = *((int*)ip);
                fprintf(asm_file, "%d\n", adr);
                ip += sizeof(int); 
                
                break;
            }

            case CMD_JE:
            {
                fprintf(asm_file, "je ");
                ip += 1;

                int adr = *((int*)ip);
                fprintf(asm_file, "%d\n", adr);
                ip += sizeof(int); 
                
                break;
            }

            case CMD_JNE:
            {
                fprintf(asm_file, "jne ");
                ip += 1;

                int adr = *((int*)ip);
                fprintf(asm_file, "%d\n", adr);
                ip += sizeof(int); 
                
                break;
            }

            case CMD_CALL:
            {
                fprintf(asm_file, "call ");
                ip += 1;

                int adr = *((int*)ip);
                fprintf(asm_file, "%d\n", adr);
                ip += sizeof(int); 

                break;
            }

            case CMD_RET:
            {
                fprintf(asm_file, "ret\n");
                ip += 1;
                break;
            }

            default:
            {
                printf("error: unknown command\n");
                fclose(asm_file);
                return;
            }
        }
    }

    fclose(asm_file);
}