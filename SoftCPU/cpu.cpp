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
            case CMD_ADD:
            {
                StackPush(&proc->stk, StackPop(&proc->stk) + StackPop(&proc->stk));
                ip += 1;
                break;
            }

            case CMD_SUB:
            {
                StackPush(&proc->stk, -StackPop(&proc->stk) + StackPop(&proc->stk));
                ip += 1;
                break;
            }

            case CMD_MUL:
            {
                StackPush(&proc->stk, StackPop(&proc->stk) * StackPop(&proc->stk));
                ip += 1;
                break;
            }

            case CMD_DIV:
            {
                elem_t last = StackPop(&proc->stk);
                StackPush(&proc->stk, StackPop(&proc->stk) / last);
                ip += 1;
                break;
            }

            case CMD_DUP:
            {
                elem_t last = StackPop(&proc->stk);
                StackPush(&proc->stk, last);
                StackPush(&proc->stk, last);
                ip += 1;
                break;
            }

            case CMD_DUMP:
            {
                StackDump(&proc->stk);
                ip += 1;
                break;
            }

            case CMD_OUT:
            {
                printf("out: ");
                printf(elem_fmt, StackPop(&proc->stk));
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
                        arg += proc->Regs[*ip];
                        ip  += 1;
                    }

                    proc->RAM[(int)arg] = new_elem;
                }
                else if(cmd & ARG_REG)
                {
                    proc->Regs[*ip] = new_elem;
                    ip += 1;
                }
                else
                {
                    StackPush(&proc->stk, new_elem);
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
                    arg += proc->Regs[*ip];
                    ip  += 1;
                }
                if(cmd & ARG_RAM)
                {
                    arg = proc->RAM[(int)arg];
                }

                StackPush(&proc->stk, arg);
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
                        arg += proc->Regs[*ip];
                        ip  += 1;
                    }

                    proc->RAM[(int)arg] = StackPop(&proc->stk);
                }
                else if(cmd & ARG_REG)
                {
                    proc->Regs[*ip] = StackPop(&proc->stk);
                    ip            += 1;
                }
                else
                {
                    StackPop(&proc->stk);
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
                ip      = proc->code + adr;
                break;
            }

            case CMD_JB:
            {
                ip     += 1;
                int adr = *((int*)ip);

                elem_t var1 = StackPop(&proc->stk);
                elem_t var2 = StackPop(&proc->stk);
                if(var1 < var2)
                {
                    ip = proc->code + adr;
                }
                else
                {
                    ip += sizeof(int);
                }

                StackPush(&proc->stk, var2);
                StackPush(&proc->stk, var1);
                break;
            }

            case CMD_JBE:
            {
                ip     += 1;
                int adr = *((int*)ip);

                elem_t var1 = StackPop(&proc->stk);
                elem_t var2 = StackPop(&proc->stk);
                if(var1 <= var2)
                {
                    ip = proc->code + adr;
                }
                else
                {
                    ip += sizeof(int);
                }

                StackPush(&proc->stk, var2);
                StackPush(&proc->stk, var1);
                break;
            }

            case CMD_JA:
            {
                ip     += 1;
                int adr = *((int*)ip);

                elem_t var1 = StackPop(&proc->stk);
                elem_t var2 = StackPop(&proc->stk);
                if(var1 > var2)
                {
                    ip = proc->code + adr;
                }
                else
                {
                    ip += sizeof(int);
                }

                StackPush(&proc->stk, var2);
                StackPush(&proc->stk, var1);
                break;
            }

            case CMD_JAE:
            {
                ip     += 1;
                int adr = *((int*)ip);

                elem_t var1 = StackPop(&proc->stk);
                elem_t var2 = StackPop(&proc->stk);
                if(var1 >= var2)
                {
                    ip = proc->code + adr;
                }
                else
                {
                    ip += sizeof(int);
                }

                StackPush(&proc->stk, var2);
                StackPush(&proc->stk, var1);
                break;
            }

            case CMD_JE:
            {
                ip     += 1;
                int adr = *((int*)ip);

                elem_t var1 = StackPop(&proc->stk);
                elem_t var2 = StackPop(&proc->stk);
                if(var1 == var2)
                {
                    ip = proc->code + adr;
                }
                else
                {
                    ip += sizeof(int);
                }

                StackPush(&proc->stk, var2);
                StackPush(&proc->stk, var1);
                break;
            }

            case CMD_JNE:
            {
                ip     += 1;
                int adr = *((int*)ip);

                elem_t var1 = StackPop(&proc->stk);
                elem_t var2 = StackPop(&proc->stk);
                if(var1 != var2)
                {
                    ip = proc->code + adr;
                }
                else
                {
                    ip += sizeof(int);
                }

                StackPush(&proc->stk, var2);
                StackPush(&proc->stk, var1);
                break;
            }

            case CMD_CALL:
            {
                ip     += 1;
                int adr = *((int*)ip);
                int ret_adr = ip + sizeof(int) - proc->code;
                StackPush(&proc->ret_stk, ret_adr);

                ip = proc->code + adr;
                break;
            }

            case CMD_RET:
            {
                int ret_adr = StackPop(&proc->ret_stk);
                ip          = proc->code + ret_adr;
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

void Dtor(CPU* proc)
{
    free(proc->RAM);
    proc->RAM = NULL;

    free(proc->code);
    proc->code = NULL;

    StackDtor(&proc->stk);
    StackDtor(&proc->ret_stk);
}