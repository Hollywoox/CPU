#include "cpu.h"

//=====================================================================================================



void Ctor(CPU* cpu, size_t ram_size, char* adr)
{
    assert(cpu != NULL);
    assert(adr != NULL);

    cpu->RAM = (elem_t*)calloc(ram_size, sizeof(elem_t));
    assert(cpu->RAM != NULL);

    char* info = (char*)calloc(1, 2 * sizeof(char) + sizeof(elem_t));
    assert(info != NULL);

    FILE* bin_file = fopen(adr, "rb");
    assert(bin_file != NULL);

    fread(info, sizeof(char), 2 * sizeof(char) + sizeof(elem_t), bin_file);

    if(*info != SIGN)
    {
        printf("Error: The signature of the file does not match the signarure the CPU\n");
        abort();
    }
    if(*(info + 1) != VERSION)
    {
        printf("Warning: The version of your assemmbler is old, please, buy a new one\n");
    }
    
    cpu->code_size = *((int*)(info + 2));
    cpu->code      = (char*)calloc(cpu->code_size + 1, sizeof(char));
    assert(cpu->code != NULL);

    fread(cpu->code, sizeof(char), cpu->code_size, bin_file);

    StackCtor(&cpu->stk,    10);
    StackCtor(&cpu->ret_stk, 0);

    fclose(bin_file);
    free(info);
}

//=====================================================================================================

void Run(CPU* cpu)
{
    char* ip = cpu->code;
    while(ip)
    {
        switch(*ip & 0x1F)
        {
            case CMD_ADD:
            {
                StackPush(&cpu->stk, StackPop(&cpu->stk) + StackPop(&cpu->stk));
                ip += 1;
                break;
            }

            case CMD_SUB:
            {
                StackPush(&cpu->stk, -StackPop(&cpu->stk) + StackPop(&cpu->stk));
                ip += 1;
                break;
            }

            case CMD_MUL:
            {
                StackPush(&cpu->stk, StackPop(&cpu->stk) * StackPop(&cpu->stk));
                ip += 1;
                break;
            }

            case CMD_DIV:
            {
                elem_t last = StackPop(&cpu->stk);
                StackPush(&cpu->stk, StackPop(&cpu->stk) / last);
                ip += 1;
                break;
            }

            case CMD_DUP:
            {
                elem_t last = StackPop(&cpu->stk);
                StackPush(&cpu->stk, last);
                StackPush(&cpu->stk, last);
                ip += 1;
                break;
            }

            case CMD_DUMP:
            {
                StackDump(&cpu->stk);
                ip += 1;
                break;
            }

            case CMD_OUT:
            {
                printf("out: ");
                printf(elem_fmt, StackPop(&cpu->stk));
                printf("\n");
                ip += 1;
                break;
            }

            case CMD_IN:
            {
                int cmd = *ip;
                ip     += 1;

                elem_t arg      = 0;
                elem_t new_elem = 0;
                printf("in: ");
                scanf(elem_fmt, &new_elem);

                if(cmd & ARG_RAM)
                {
                    if(cmd & ARG_IMMED)
                    {
                        arg += *((elem_t*)ip);
                        ip  += sizeof(elem_t);
                    }
                    if(cmd & ARG_REG)
                    {
                        arg += cpu->Regs[*ip];
                        ip  += 1;
                    }

                    cpu->RAM[(int)arg] = new_elem;
                }
                else if(cmd & ARG_REG)
                {
                    cpu->Regs[*ip] = new_elem;
                    ip += 1;
                }
                else
                {
                    StackPush(&cpu->stk, new_elem);
                }
                
                break;
            }

            case CMD_PUSH:
            {
                int cmd = *ip;
                ip     += 1;

                elem_t arg = 0;

                if(cmd & ARG_IMMED)
                {
                    arg += *((elem_t*)ip);
                    ip  += sizeof(elem_t);
                }
                if(cmd & ARG_REG)
                {
                    arg += cpu->Regs[*ip];
                    ip  += 1;
                }
                if(cmd & ARG_RAM)
                {
                    arg = cpu->RAM[(int)arg];
                }

                StackPush(&cpu->stk, arg);
                break;
            }

            case CMD_POP:
            {
                int cmd = *ip;
                ip     += 1;

                elem_t arg = 0;

                if(cmd & ARG_RAM)
                {
                    if(cmd & ARG_IMMED)
                    {
                        arg += *((elem_t*)ip);
                        ip  += sizeof(elem_t);
                    }
                    if(cmd & ARG_REG)
                    {
                        arg += cpu->Regs[*ip];
                        ip  += 1;
                    }

                    cpu->RAM[(int)arg] = StackPop(&cpu->stk);
                }
                else if(cmd & ARG_REG)
                {
                    cpu->Regs[*ip] = StackPop(&cpu->stk);
                    ip            += 1;
                }
                else
                {
                    StackPop(&cpu->stk);
                }

                break;
            }

            case CMD_HLT:
            {
                return;
            }

            case CMD_JMP:
            {
                ip     += 1;
                int adr = *((int*)ip);
                ip      = cpu->code + adr;
                break;
            }

            case CMD_JB:
            {
                ip     += 1;
                int adr = *((int*)ip);

                elem_t var1 = StackPop(&cpu->stk);
                elem_t var2 = StackPop(&cpu->stk);
                if(var1 < var2)
                {
                    ip = cpu->code + adr;
                }
                else
                {
                    ip += sizeof(int);
                }

                StackPush(&cpu->stk, var2);
                StackPush(&cpu->stk, var1);
                break;
            }

            case CMD_JBE:
            {
                ip     += 1;
                int adr = *((int*)ip);

                elem_t var1 = StackPop(&cpu->stk);
                elem_t var2 = StackPop(&cpu->stk);
                if(var1 <= var2)
                {
                    ip = cpu->code + adr;
                }
                else
                {
                    ip += sizeof(int);
                }

                StackPush(&cpu->stk, var2);
                StackPush(&cpu->stk, var1);
                break;
            }

            case CMD_JA:
            {
                ip     += 1;
                int adr = *((int*)ip);

                elem_t var1 = StackPop(&cpu->stk);
                elem_t var2 = StackPop(&cpu->stk);
                if(var1 > var2)
                {
                    ip = cpu->code + adr;
                }
                else
                {
                    ip += sizeof(int);
                }

                StackPush(&cpu->stk, var2);
                StackPush(&cpu->stk, var1);
                break;
            }

            case CMD_JAE:
            {
                ip     += 1;
                int adr = *((int*)ip);

                elem_t var1 = StackPop(&cpu->stk);
                elem_t var2 = StackPop(&cpu->stk);
                if(var1 >= var2)
                {
                    ip = cpu->code + adr;
                }
                else
                {
                    ip += sizeof(int);
                }

                StackPush(&cpu->stk, var2);
                StackPush(&cpu->stk, var1);
                break;
            }

            case CMD_JE:
            {
                ip     += 1;
                int adr = *((int*)ip);

                elem_t var1 = StackPop(&cpu->stk);
                elem_t var2 = StackPop(&cpu->stk);
                if(var1 == var2)
                {
                    ip = cpu->code + adr;
                }
                else
                {
                    ip += sizeof(int);
                }

                StackPush(&cpu->stk, var2);
                StackPush(&cpu->stk, var1);
                break;
            }

            case CMD_JNE:
            {
                ip     += 1;
                int adr = *((int*)ip);

                elem_t var1 = StackPop(&cpu->stk);
                elem_t var2 = StackPop(&cpu->stk);
                if(var1 != var2)
                {
                    ip = cpu->code + adr;
                }
                else
                {
                    ip += sizeof(int);
                }

                StackPush(&cpu->stk, var2);
                StackPush(&cpu->stk, var1);
                break;
            }

            case CMD_CALL:
            {
                ip     += 1;
                int adr = *((int*)ip);
                int ret_adr = ip + sizeof(int) - cpu->code;
                StackPush(&cpu->ret_stk, ret_adr);

                ip = cpu->code + adr;
                break;
            }

            case CMD_RET:
            {
                int ret_adr = StackPop(&cpu->ret_stk);
                ip          = cpu->code + ret_adr;
                break;
            }

            default:
            {
                printf("error: unknown command\n");
                return;
            }
        }
    }
}
 
//=====================================================================================================

void Dtor(CPU* cpu)
{
    free(cpu->RAM);
    cpu->RAM = NULL;

    free(cpu->code);
    cpu->code = NULL;

    StackDtor(&cpu->stk);
    StackDtor(&cpu->ret_stk);
}