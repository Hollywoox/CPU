#include "cpu.h"


void Ctor(CPU* cpu, size_t ram_size, char* adr)
{
    assert(cpu != NULL);

    cpu->RAM = (elem_t*)calloc(ram_size, sizeof(elem_t));
    assert(cpu->RAM != NULL);

    char* info = (char*)calloc(1, sizeof(char) + sizeof(elem_t));
    assert(info != NULL);

    FILE* fin = fopen(adr, "rb");
    assert(fin != NULL);
    fread(info, sizeof(char), sizeof(char) + sizeof(elem_t), fin);

    if(*info != SIGN)
    {
        printf("Error: The signature of the file does not match the signarure the CPU\n");
        abort();
    }
    if(*(info + 1) != VERSION)
    {
        printf("Warning: The version of your assemmbler is old, please, buy a new one\n");
    }
    
    cpu->code_size = *((short int*)(info + 2));
    cpu->code = (char*)calloc(cpu->code_size + 1, sizeof(char));
    assert(cpu->code != NULL);

    fread(cpu->code, sizeof(char), cpu->code_size, fin);

    StackCtor(&cpu->stk, 10);

    fclose(fin);
    free(info);
}

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
                ip += 1;
                elem_t new_elem = 0;
                int reg = 0;
                printf("in: ");
                scanf(elem_fmt, &new_elem);

                if(cmd & ARG_REG)
                {
                    int reg_num = *ip;
                    cpu->Regs[reg_num] = new_elem;
                    ip += sizeof(char);
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
                ip += 1;
                elem_t arg = 0;

                if(cmd & ARG_IMMED)
                {
                    arg += *((elem_t*)ip);
                    ip += sizeof(elem_t);
                }
                if(cmd & ARG_REG)
                {
                    arg += cpu->Regs[*ip];
                    ip += 1;
                }
                if(cmd & ARG_RAM)
                {
                    arg = cpu->RAM[arg];
                }

                StackPush(&cpu->stk, arg);
                break;
            }

            case CMD_HLT:
            {
                return;
            }

            case CMD_JMP:
            {
                int adr;
                adr = *((int*)(ip + 1));
                ip = cpu->code + adr;
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

void Dtor(CPU* cpu)
{
    free(cpu->RAM);
    cpu->RAM = NULL;

    free(cpu->code);
    cpu->code = NULL;

    StackDtor(&cpu->stk);
}