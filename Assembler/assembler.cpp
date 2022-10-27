#include "assembler.h"

//=====================================================================================================

void AsmProgArray(Text* text, char* file)
{
    if(text == NULL)
    {
        PrintErrorMessage();
        printf("Text pointer is NULL!\n");
        abort();
    }
    if(file == NULL)
    {
        PrintErrorMessage();
        printf("pointer to file name is NULL\n");
        abort();
    }

    FILE* asm_prog = fopen(file, "r");
    if(asm_prog == NULL)
    {
        PrintErrorMessage();
        printf("can't open the file!\n");
        abort();
    }
    
    CountBytes(text, file);

    MakeBuf(text, asm_prog);

    CountStr(text);

    MakeStrArray(text);

    fclose(asm_prog);
}

//=====================================================================================================

void Compile(Text* text, struct Label* labels, int num_of_compilation)
{
    if(text == NULL)
    {
        PrintErrorMessage();
        printf("Text pointer is NULL!\n");
        abort();
    }

    FILE* listing = fopen("listing.txt", "w");
    assert(listing != NULL);
    fprintf(listing, "Signature: %X\nCPU version: %d\n\n", SIGN, VERSION);

    size_t size = sizeof(char) + sizeof(elem_t);
    char* code = (char*)calloc(text->num_of_str + 1, size);
    if(code == NULL)
    {
        PrintErrorMessage();
        printf("can't find the required memory block!\n");
        abort();
    }

    *code = SIGN;
    *(code + 1) = VERSION;
    char* ip = code + size;

    int line = 0;
    int label_index = 0;
    while(line < text->num_of_str)
    {
        char *cmd = (char*)calloc(text->str_array[line].size, sizeof(char));
        GetWord(text->str_array[line].str, cmd);
        size_t cmd_len = strlen(cmd);

        if(cmd[cmd_len - 1] == ':')
        {
            cmd[cmd_len - 1] = '\0';
            labels[label_index].name = cmd;
            labels[label_index].adr = ip - code - size;
            cmd = NULL;
            ++label_index;
        }

        else if(strcmp(cmd, "push") == 0)
        {
            *ip = CMD_PUSH;
            fprintf(listing, "%#8lX %2X ", ip - code - size, CMD_PUSH);
            ++ip;

            GetArgs(text->str_array[line].str, &ip, cmd, listing);
        }

        else if(strcmp(cmd, "add") == 0)
        {
            *ip = CMD_ADD;
            fprintf(listing, "%#8lX %2X   ADD\n", ip - code - size, CMD_ADD);
            ++ip;
        }

        else if(strcmp(cmd, "sub") == 0)
        {
            *ip = CMD_SUB;
            fprintf(listing, "%#8lX %2X   SUB\n", ip - code - size, CMD_SUB);
            ++ip;
        }

        else if(strcmp(cmd, "mul") == 0)
        {
            *ip = CMD_MUL;
            fprintf(listing, "%#8lX %2X   MUL\n", ip - code - size, CMD_MUL);
            ++ip;
        }

        else if(strcmp(cmd, "div") == 0)
        {
            *ip = CMD_DIV;
            fprintf(listing, "%#8lX %2X   DIV\n", ip - code - size, CMD_DIV);
            ++ip;
        }

        else if(strcmp(cmd, "dup") == 0)
        {
            *ip = CMD_DUP;
            fprintf(listing, "%#8lX %2X   DUP\n", ip - code - size, CMD_DUP);
            ++ip;
        }

        else if(strcmp(cmd, "out") == 0)
        {
            *ip = CMD_OUT;
            fprintf(listing, "%#8lX %2X   OUT\n", ip - code - size, CMD_OUT);
            ++ip;
        }

        else if(strcmp(cmd, "dump") == 0)
        {
            *ip = CMD_DUMP;
            fprintf(listing, "%#8lX %2X   DUMP\n", ip - code - size, CMD_DUMP);
            ++ip;
        }

        else if(strcmp(cmd, "in") == 0)
        {
            *ip = CMD_IN;
            fprintf(listing, "%#8lX %2X ", ip - code - size, CMD_IN);
            ++ip;

            GetArgs(text->str_array[line].str, &ip, cmd, listing);
        }

        else if(strcmp(cmd, "hlt") == 0)
        {
            *ip = CMD_HLT;
            fprintf(listing, "%#8lX %2X   HLT\n", ip - code - size, CMD_HLT);
            ++ip;
        }

        else if(strcmp(cmd, "jmp") == 0)
        {
            *ip = CMD_JMP;
            fprintf(listing, "%#8lX %2X ", ip - code - size, CMD_JMP);
            ++ip;

            GetLabel(text->str_array[line].str, &ip, labels, cmd, num_of_compilation);
            fprintf(listing, "%X JUMP %X\n", *((int*)(ip - sizeof(int))), *((int*)(ip - sizeof(int))));
        }

        else if(cmd[0] == '\0')
        {
            //skip, if the string is empty
        }

        else
        {
            PrintErrorMessage();
            printf("unknown command!\n");
            abort();
        }

        free(cmd);
        ++line;
    }

    *((short int*)(code + 2)) = ip - code - size;

    FILE* fout = fopen("result.bin", "wb");
    if(fout == NULL)
    {
        PrintErrorMessage();
        printf("can't open file for binary code\n");
        abort();
    } 

    fwrite(code, sizeof(char), ip - code, fout);
    free(code);
    fclose(listing);
    fclose(fout);
}

//=====================================================================================================

int GetWord(char* str, char* cmd)
{
    assert(str != NULL);
    assert(cmd != NULL);

    int res = sscanf(str, "%s", cmd);
    if(res < 0)
    {
        return res;
    }

    char* end = strchr(cmd, ';');
    if(end == NULL)
    {
        return 0;
    }

    while(*end != '\0' && end != NULL)
    {
        *end = '\0';
        ++end;
    }

    return 0;
}

//=====================================================================================================

void GetArgs(char* str, char** code, char* cmd, FILE* listing)
{
    assert(str != NULL);
    assert(code != NULL);
    assert(cmd != NULL);
    assert(listing != NULL);

    size_t cmd_len = strlen(cmd);
    char* cmd_name = (char*)calloc(cmd_len, sizeof(char));
    strcpy(cmd_name, cmd);
    ToUpper(cmd_name);
    int shift = strstr(str, cmd) - str + cmd_len;
    if(GetWord(str + shift, cmd) < 0) return;
    cmd_len = strlen(cmd);

    if(cmd[0] == '[' && cmd[cmd_len - 1] == ']')
    {
        if(cmd[1] == 'r' && cmd[3] == 'x')
        {
            char* plus_pointer = strchr(cmd, '+');
            if(plus_pointer != NULL)
            {
                int index;
                sscanf(plus_pointer + 1, "%d", &index);

                char reg = cmd[2] - 'a' + 1;
                if(reg > 4)
                {
                    printf("Syntax error: unknown register\n");
                    abort();
                }

                *((char*)(*code - 1)) |= 0xE0;
                *((elem_t*)(*code)) = index;
                *((char*)(*code + sizeof(elem_t))) = reg;
                
                fprintf(listing, "%X %s %s\n", *((elem_t*)(*code)), cmd_name, cmd);
                *code += sizeof(elem_t) + sizeof(char);
            }

            else
            {
                char reg = cmd[2] - 'a' + 1;
                if(reg > 4)
                {
                    printf("Syntax error: unknown register\n");
                    abort();
                }

                *((char*)(*code - 1)) |= 0xC0;
                *((char*)(*code)) = reg;

                fprintf(listing, "%X %s %s\n", *((char*)(*code)), cmd_name, cmd);
                *code += sizeof(char);
            }
        }

        else
        {
            int index;
            sscanf(cmd + 1, "%d", &index);

            *((char*)(*code - 1)) |= 0xA0;
            *((elem_t*)(*code)) = index;

            fprintf(listing, "%X %s %s\n", *((elem_t*)(*code)), cmd_name, cmd);
            *code += sizeof(elem_t);
        }
    }

    else if(cmd[0] == 'r' && cmd[2] == 'x')
    {
        char reg = cmd[1] - 'a' + 1;
        if(reg > 4)
        {
            printf("Syntax error: unknown register\n");
            abort();
        }

        *((char*)(*code - 1)) |= 0x40;
        *((char*)(*code)) = reg;

        fprintf(listing, "%X %s %s\n", *((char*)(*code)), cmd_name, cmd);
        *code += sizeof(char);
    }

    else if(IsNum(cmd))
    {
        elem_t val = 0;
        sscanf(cmd, elem_fmt, &val);

        *((char*)(*code - 1)) |= 0x20;
        *((elem_t*)(*code)) = val;

        fprintf(listing, "%X %s ", *((elem_t*)(*code)), cmd_name);
        fprintf(listing, elem_fmt, *((elem_t*)(*code)));
        fprintf(listing, "\n");
        *code += sizeof(elem_t);
    }

    else
    {
        printf("Syntax error: unknown parameter\n");
        abort();
    }

    free(cmd_name);
}

//=====================================================================================================

void GetLabel(char* str, char** code, struct Label* labels, char* cmd, int num_of_compilation)
{
    assert(str != NULL);
    assert(code != NULL);
    assert(labels != NULL);
    assert(cmd != NULL);

    int shift = strstr(str, cmd) - str + strlen(cmd);
    GetWord(str + shift, cmd);

    if(cmd[0] != ':')
    {
        printf("syntax error: this is not a label\n");
        abort();
    }
    
    int label_adr = LabelFind(cmd + 1, labels);
    
    if(num_of_compilation == 2 && label_adr == 0xFF)
    {
        printf("error: unknown label\n");
        abort();
    }

    *((int*)(*code)) = label_adr;
    *code += sizeof(int);
}

//=====================================================================================================

int LabelFind(const char* label, struct Label* labels)
{
    assert(label != NULL);
    assert(labels != NULL);

    int i = 0;
    while(labels[i].name != NULL && i < LABELS_SIZE)
    {
        if(strcmp(label, labels[i].name) == 0) return labels[i].adr;
        ++i;
    }

    return 0xFF;
}

//=====================================================================================================

int IsNum(char* str)
{
    assert(str != NULL);

    char* begin = str;
    while(*begin != '\0')
    {
        if(*begin < 48 || *begin > 57)
            return 0;
        ++begin;
    }
    return 1;
}

//=====================================================================================================

void ToUpper(char* str)
{
    assert(str != NULL);

    char* begin = str;
    while(*begin != '\0')
    {
        *begin -= 32;
        ++begin;
    }
}

//=====================================================================================================

void ClearLabels(struct Label* labels)
{
    assert(labels != NULL);

    int i = 0;
    while(labels[i].name != NULL && i < LABELS_SIZE)
    {
        free(labels[i].name);
        labels[i].name = NULL;
        ++i;
    }
}
