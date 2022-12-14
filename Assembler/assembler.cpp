#include "assembler.h"

//=====================================================================================================

void AsmCodeArray(Text* text, const char* file)
{
    assert(text != NULL);
    assert(file != NULL);

    FILE* asm_code = fopen(file, "r");
    if(asm_code == NULL)
    {
        PrintErrorMessage();
        printf("can't open the file!\n");
        abort();
    }
    
    CountBytes(text, file);

    MakeBuf(text, asm_code);

    CountStr(text);

    MakeStrArray(text);

    fclose(asm_code);
}

//=====================================================================================================

void Compile(Text* text, struct Label* labels, int num_of_compilation)
{
    assert(text   != NULL);
    assert(labels != NULL);

    FILE* listing = fopen("listing.txt", "w");
    if(listing == NULL)
    {
        PrintErrorMessage();
        printf("can't open file for listing\n");
        abort();
    }

    fprintf(listing, "Signature: %X\nCPU version: %d\n\n", SIGN, VERSION);

    size_t size = 2 * sizeof(char) + sizeof(elem_t);
    char* code  = (char*)calloc(text->num_of_str + 1, size);
    assert(code != NULL);

    *code       = SIGN;
    *(code + 1) = VERSION;
    char* ip    = code + size;

    int line        = 0;
    int label_index = 0;
    while(line < text->num_of_str)
    {
        char *cmd = (char*)calloc(text->str_array[line].size, sizeof(char));
        GetWord(text->str_array[line].str, cmd);
        size_t cmd_len = strlen(cmd);

        #include "../include/cmd.h"
        /*else*/ if(cmd[cmd_len - 1] == ':')
        {
            cmd[cmd_len - 1]         = '\0';
            labels[label_index].name = cmd;
            labels[label_index].adr  = ip - code - size;
            cmd                      = NULL;
            ++label_index;
        }
        #undef DEF_CMD
        
        else if(cmd[0] == '\0')
        {
            //skip, if the string is empty
        }

        else
        {
            printf("Syntax error: unknown command\n");
            printf("%s\n", text->str_array[line].str);
            abort();
        }

        free(cmd);
        ++line;
    }

    *((int*)(code + 2)) = ip - code - size;

    FILE* fout = fopen("code.bin", "wb");
    if(fout == NULL)
    {
        PrintErrorMessage();
        printf("can't open a file for binary code\n");
        abort();
    } 

    fwrite(code, sizeof(char), ip - code, fout);
    free(code);
    fclose(listing);
    fclose(fout);
}

//=====================================================================================================

int GetWord(const char* str, char* cmd)
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

void GetArgs(const char* str, char** code, char* cmd, FILE* listing)
{
    assert(str     != NULL);
    assert(code    != NULL);
    assert(cmd     != NULL);
    assert(listing != NULL);

    size_t cmd_len = strlen(cmd);
    char* cmd_name = (char*)calloc(cmd_len, sizeof(char));
    int shift      = strstr(str, cmd) - str + cmd_len;

    strcpy(cmd_name, cmd);
    StrToUpper(cmd_name);

    if(GetWord(str + shift, cmd) < 0)
    {
        fprintf(listing, "  %s\n", cmd_name);
        return;
    }

    cmd_len = strlen(cmd);

    if(cmd[0] == '[')
    {
        if(tolower(cmd[1]) == 'r' && tolower(cmd[3]) == 'x')
        {
            const char* plus_pointer = strchr(str, '+');
            if(plus_pointer != NULL)
            {
                int index;
                sscanf(plus_pointer + 1, "%d", &index);

                char reg = tolower(cmd[2]) - 'a' + 1;
                if(reg > 4)
                {
                    printf("Syntax error: unknown register\n");
                    printf("%s\n", str);
                    abort();
                }

                *((char*)(*code - 1))             |= 0xE0;
                *((elem_t*)(*code))                = index;
                *((char*)(*code + sizeof(elem_t))) = reg;
                
                StrToUpper(cmd + 1);
                fprintf(listing, "%X %s %s + %d]\n", *((elem_t*)(*code)), cmd_name, cmd, index);
                *code += sizeof(elem_t) + sizeof(char);
            }

            else
            {
                char reg = tolower(cmd[2]) - 'a' + 1;
                if(reg > 4)
                {
                    printf("Syntax error: unknown register\n");
                    printf("%s\n", str);
                    abort();
                }

                *((char*)(*code - 1)) |= 0xC0;
                *((char*)(*code))      = reg;

                StrToUpper(cmd + 1);
                fprintf(listing, "%X %s %s\n", *((char*)(*code)), cmd_name, cmd);
                *code += sizeof(char);
            }
        }

        else
        {
            int index;
            sscanf(cmd + 1, "%d", &index);

            *((char*)(*code - 1)) |= 0xA0;
            *((elem_t*)(*code))    = index;

            fprintf(listing, "%X %s %s\n", *((elem_t*)(*code)), cmd_name, cmd);
            *code += sizeof(elem_t);
        }
    }

    else if(tolower(cmd[0]) == 'r' && tolower(cmd[2]) == 'x')
    {
        char reg = tolower(cmd[1]) - 'a' + 1;
        if(reg > 4)
        {
            printf("Syntax error: unknown register\n");
            printf("%s\n", str);
            abort();
        }

        *((char*)(*code - 1)) |= 0x40;
        *((char*)(*code))      = reg;

        StrToUpper(cmd);
        fprintf(listing, "%X %s %s\n", *((char*)(*code)), cmd_name, cmd);
        *code += sizeof(char);
    }

    else if(IsNum(cmd))
    {
        elem_t val = 0;
        sscanf(cmd, elem_fmt, &val);

        *((char*)(*code - 1)) |= 0x20;
        *((elem_t*)(*code))    = val;

        fprintf(listing, "%X %s ", *((elem_t*)(*code)), cmd_name);
        fprintf(listing, elem_fmt, *((elem_t*)(*code)));
        fprintf(listing, "\n");
        *code += sizeof(elem_t);
    }

    else
    {
        printf("Syntax error: unknown argument\n");
        printf("%s\n", str);
        abort();
    }

    free(cmd_name);
}

//=====================================================================================================

void GetLabel(const char* str, char** code, struct Label* labels, char* cmd, int num_of_compilation)
{
    assert(str    != NULL);
    assert(code   != NULL);
    assert(labels != NULL);
    assert(cmd    != NULL);

    int shift = strstr(str, cmd) - str + strlen(cmd);
    GetWord(str + shift, cmd);

    if(IsNum(cmd))
    {
        int label_adr = 0;
        sscanf(cmd, "%d", &label_adr);

        *((int*)(*code)) = label_adr;
        *code           += sizeof(int);
    }

    else
    {
        if(cmd[0] != ':')
        {
            printf("syntax error: this is not a label\n");
            printf("%s\n", str);
            abort();
        }
    
        int label_adr = LabelFind(cmd + 1, labels);
    
        if(num_of_compilation == 2 && label_adr == 0xFF)
        {
            printf("error: unknown label\n");
            printf("%s\n", str);
            abort();
        }

        *((int*)(*code)) = label_adr;
        *code           += sizeof(int);
    }
}

//=====================================================================================================

int LabelFind(const char* label, struct Label* labels)
{
    assert(label  != NULL);
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

int IsNum(const char* str)
{
    assert(str != NULL);

    while(*str != '\0')
    {
        if(!((*str > 47 && *str < 58) || *str == '.'))
            return 0;
        ++str;
    }
    return 1;
}

//=====================================================================================================

void StrToUpper(char* str)
{
    assert(str != NULL);

    char* begin = str;
    while(*begin != '\0' && isalpha(*begin))
    {
        *begin -= 32;
        ++begin;
    }
}

//=====================================================================================================

int Stricmp(const char* str1, const char* str2)
{
    assert(str1 != NULL);
    assert(str2 != NULL);

    while((*str1 != '\0' && *str2 != '\0') && (tolower(*str1) == tolower(*str2)))
    {
        ++str1;
        ++str2;
    }

    return *str1 - *str2;
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
